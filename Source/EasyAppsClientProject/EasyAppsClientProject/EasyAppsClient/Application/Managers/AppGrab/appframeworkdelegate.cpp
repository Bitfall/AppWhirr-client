#include "appframeworkdelegate.h"

// network
#include "Application/Communication/Gateway/communicationdescriptiongateway.h"
#include "Common/Communication/basenetworkentity.h"
#include "Application/Communication/Request/appframeworkinfonetworkrequest.h"
#include "Application/Communication/Entity/appframeworkinfonetworkentity.h"
//#include "Application/Communication/Gateway/communicationresourcegateway.h"
#include "Application/Communication/awresourcedownloadstreamtofile.h"

// framework checking
#include "Application/Frameworks/frameworkcheckerdatabase.h"
#include "Common/Framework/availableframeworkchecker.h"

// framework installing
#include "Common/Framework/frameworkinstaller.h"


//
#include <QDir>

// utility
#include "Common/Helpers/pathhelper.h"
#include "Application/appspecificpathhelper.h"
#include "Common/Uncompressor/uncompressor.h"
#include "Settings/configmanager.h"

AppFrameworkDelegate::AppFrameworkDelegate(QObject *parent) :
    QObject(parent),
    _commDescriptionGateway(NULL),
//    _commResourceGateway(NULL),
    _resourceDownloadStreamToFile(NULL),
    _isCheckingForFramework(false),
    _isFrameworkReCheckFailed(false),
    _currFrameworkCheckType(FCT_InitialChecking)
{
    connect(this, SIGNAL(__tryNextFrameworkCheckInQueue()), this, SLOT(_doTryNextFramworkCheckInQueue()));

    //
    _frameworkCheckManager = new AvailableFrameworkChecker(this);
    connect(_frameworkCheckManager, SIGNAL(frameworksAvailable(QList<FrameworkCheckModel>)), this, SLOT(_fwCheckerFrameworksAvailableInSystem(QList<FrameworkCheckModel>)));
    connect(_frameworkCheckManager, SIGNAL(frameworksNotFound(QList<FrameworkCheckModel>)), this, SLOT(_fwCheckerFrameworksNotFoundInSystem(QList<FrameworkCheckModel>)));
    connect(_frameworkCheckManager, SIGNAL(finishedWithChecking()), this, SLOT(_fwCheckerInSystemCheckingFinished()));

    //
    _frameworkInstallManager = new FrameworkInstaller(this);
    connect(_frameworkInstallManager, SIGNAL(frameworkInstalled(FrameworkCheckModel)), this, SLOT(_fwInstallerFrameworkInstalled(FrameworkCheckModel)));
    connect(_frameworkInstallManager, SIGNAL(frameworkInstallFailed(FrameworkCheckModel,QString)), this, SLOT(_fwInstallerFrameworkInstallFailed(FrameworkCheckModel,QString)));
}

AppFrameworkDelegate::~AppFrameworkDelegate()
{
    AW_QOBJECT_SAFE_DELETE(_commDescriptionGateway);
//    AW_QOBJECT_SAFE_DELETE(_commResourceGateway);
    AW_QOBJECT_SAFE_DELETE(_resourceDownloadStreamToFile);
}

// ------------------------
// --- public interface ---

void AppFrameworkDelegate::checkFrameworksForApp(GrabApplicationQueueItem grabItem)
{
    _appsWaitingForFrameworkCheck.enqueue(grabItem);
    Q_EMIT __tryNextFrameworkCheckInQueue();
}


// ----------------
// --- internal ---

void AppFrameworkDelegate::_doTryNextFramworkCheckInQueue()
{
    if(this->getIsCheckingForFramework()) {
        DLog("Still checking other app's framework.");
        return;
    }

    if(_appsWaitingForFrameworkCheck.isEmpty()) {
        DLog("No more apps waiting for framework checking.");
        return;
    }

    // start checking
    this->setIsCheckingForFramework(true);

    GrabApplicationQueueItem currentGrabItem = _appsWaitingForFrameworkCheck.first();
    Q_EMIT frameworkCheckingStartedForApp(currentGrabItem);

    // cleanup
    _currRequiredDownloadedFrameworks.clear();
    _tmp_currRequiredDownloadNeededFrameworks.clear();
    _currFrameworksNotFoundInSystem.clear();
    _currFrameworkInstallersWaitingForDownload.clear();
    _currFrameworksForAfterInstallReCheck.clear();
    _currFrameworksWaitingForInstallation.clear();


    AW_QOBJECT_SAFE_DELETE(_commDescriptionGateway);
    _commDescriptionGateway = new CommunicationDescriptionGateway;

    connect(_commDescriptionGateway, SIGNAL(finished(QList<AW::Common::Communication::BaseNetworkEntity*>)), this, SLOT(_appFrameworkCheckerDescriptionRequestFinished(QList<AW::Common::Communication::BaseNetworkEntity*>)));
    connect(_commDescriptionGateway, SIGNAL(failedToRetrieveDescription(QString)), this, SLOT(_appFrameworkCheckerFailedToGetDescription(QString)));
//    connect(_commDescriptionGateway, SIGNAL(authenticationChallengeHappened()), this, SLOT(_authenticationChallangeHappened_PutBackToQueue()));

    // start getting framework checker info
    _commDescriptionGateway->performThisRequest(new AppFrameworkInfoNetworkRequest(currentGrabItem.getAppId()));
}


// --------------------------
// --- framework checking ---

void AppFrameworkDelegate::_appFrameworkCheckerDescriptionRequestFinished(QList<AW::Common::Communication::BaseNetworkEntity *> entities)
{
    DLog("App framework checker info.");

    GrabApplicationQueueItem relatedGrabItem;

    AW_QOBJECT_SAFE_DELETE(_commDescriptionGateway);

    if (!_appsWaitingForFrameworkCheck.isEmpty()) {
        // finished - remove from the queue
        relatedGrabItem = _appsWaitingForFrameworkCheck.first();
    } else {
        WLog("No item in waiting-for-framework-check queue. This should never happen here!");
        return;
    }

    // If there is no entity to iterate on - no framework required
    if (entities.isEmpty()) {
        DLog("No framework required for app: ") << relatedGrabItem.getAppId() << relatedGrabItem.getAppName();
        this->__checkingFinishedForCurrentAppTryNextOne();
        return;
    }

    // if some framework required
    Q_FOREACH(AW::Common::Communication::BaseNetworkEntity *entity, entities) {
        AppFrameworkInfoNetworkEntity *castedEntity = dynamic_cast<AppFrameworkInfoNetworkEntity *>(entity);
        if(castedEntity == NULL) {
            WLog("Invalid entity.");
            continue;
        }

        bool isFwCheckerAlreadyDownloaded = false;
        FrameworkCheckerDatabase *fwCheckerDb = new FrameworkCheckerDatabase(this->getFrameworkCheckerDatabasePath());
        FrameworkCheckerInfo currFwCheckerInfo;
        isFwCheckerAlreadyDownloaded = fwCheckerDb->getFrameworkChecker(castedEntity->getFrameworkId(), currFwCheckerInfo);
        delete fwCheckerDb;

        QString fwCheckerLocalPath;
        if(isFwCheckerAlreadyDownloaded) {
            DLog("Fw checker found locally - already downloaded: ") << castedEntity->getFrameworkId();
            if(!PathHelper::isFileExistsAndItsReallyAFile(currFwCheckerInfo.getFrameworkCheckerPath())) {
                WLog("Invalid fw checker path: ") << currFwCheckerInfo.getFrameworkCheckerPath();
//                isFwCheckerAlreadyDownloaded = false;
            }
            else {
                fwCheckerLocalPath = currFwCheckerInfo.getFrameworkCheckerPath();
            }

            if(currFwCheckerInfo.getFrameworkCheckerUpdateTimestamp() != castedEntity->getFrameworkCheckerUpdateTimestamp()) {
                DLog("Fw checker update-timestamp does not match - new download required.");
//                isFwCheckerAlreadyDownloaded = false;
                fwCheckerLocalPath.clear();
            }
        }

        FrameworkCheckModel fwCheckModel(FrameworkInfo(castedEntity->getFrameworkId(), castedEntity->getFrameworkName()), fwCheckerLocalPath, castedEntity->getFrameworkCheckerUpdateTimestamp(), AppSpecificPathHelper::getLocalFrameworkCheckResultDirectory(), QString(), QStringList());
        fwCheckModel.addOptionalFrameworkCheckerArg("-checkmode");
        fwCheckModel.addOptionalFrameworkCheckerArg("recheck");

        FrameworkCheckInternalModel fwCheckInternalModel(fwCheckModel, QUrl(), castedEntity->getFrameworkCheckerDownloadUrl());

        if(!fwCheckerLocalPath.isEmpty()) {
            _currRequiredDownloadedFrameworks.append(fwCheckInternalModel);
        }
        else {
            _tmp_currRequiredDownloadNeededFrameworks.append(fwCheckInternalModel);
        }
    }

    if(_tmp_currRequiredDownloadNeededFrameworks.isEmpty()) {
        // every checker is available - start checking
        this->_startFrameworkChecking();
    }
    else {
        // some checkers are not available - start downloading them
        this->_downloadRequiredCheckers();
    }
}

void AppFrameworkDelegate::_appFrameworkCheckerFailedToGetDescription(QString errorMessage)
{
    this->__checkingFailedForCurrentAppTryNextOne(tr("Checking error: %1").arg(errorMessage));
}

void AppFrameworkDelegate::_downloadRequiredCheckers()
{
    DLog("Download required checkers");
//    AW_QOBJECT_SAFE_DELETE(_commResourceGateway);
//    _commResourceGateway = new CommunicationResourceGateway;

//    connect(_commResourceGateway, SIGNAL(resourceReceivedForURL(QUrl,QByteArray)), this, SLOT(_fwCheckerDownloadRetrievedResourceForURL(QUrl,QByteArray)));
//    connect(_commResourceGateway, SIGNAL(downloadProgressForUrl(int,QUrl)), this, SLOT(_fwCheckerDownloadProgressForUrl(int,QUrl)));
//    connect(_commResourceGateway, SIGNAL(finished()), this, SLOT(_fwCheckerAllDownloadResourceRequestsFinished()));
//    connect(_commResourceGateway, SIGNAL(resourceDownloadFailedForUrl(QUrl)), this, SLOT(_fwCheckerDownloadFailedForUrl(QUrl)));

//    Q_FOREACH(FrameworkCheckInternalModel fwCheckInternalModel, _tmp_currRequiredDownloadNeededFrameworks) {
//        _commResourceGateway->addResourceURL(fwCheckInternalModel.getFrameworkCheckerDownloadUrl());
//    }

//    _commResourceGateway->startRequests();

    AW_QOBJECT_SAFE_DELETE(_resourceDownloadStreamToFile);
    _resourceDownloadStreamToFile = AW::Client::Communication::AWResourceDownloadStreamToFile::createDefaultResourceDownloadStreamToFile(NULL);

    connect(_resourceDownloadStreamToFile, SIGNAL(resourceDownloadProgressForUrl(int,QUrl)), this, SLOT(_fwCheckerDownloadProgressForUrl(int,QUrl)));
    connect(_resourceDownloadStreamToFile, SIGNAL(resourceDownloadFinishedWithSuccess(QUrl,QString)), this, SLOT(_fwCheckerDownloadFinished(QUrl,QString)));
    connect(_resourceDownloadStreamToFile, SIGNAL(resourceDownloadFailed(QUrl,QString,QString)), this, SLOT(_fwCheckerDownloadFailed(QUrl,QString)));
    connect(_resourceDownloadStreamToFile, SIGNAL(finishedWithAllResourceDownloads()), this, SLOT(_fwCheckerAllDownloadResourceRequestsFinished()));

    bool isCannotStartStreaming = false;
    Q_FOREACH(FrameworkCheckInternalModel fwCheckInternalModel, _tmp_currRequiredDownloadNeededFrameworks)
    {
        QString tempZipFilePath = this->_getFrameworkCheckerTmpZipPathById(fwCheckInternalModel.getFrameworkCheckModel().getFrameworkInfo().getFrameworkId());
        if( !_resourceDownloadStreamToFile->addAndInitializeResourceDownloadRequest(fwCheckInternalModel.getFrameworkCheckerDownloadUrl(), tempZipFilePath, true) ) {
            isCannotStartStreaming = true;
        }
    }

    if(isCannotStartStreaming) {
        this->__checkingFailedForCurrentAppTryNextOne("Cannot save required framework checker");
    }
    else {
        _resourceDownloadStreamToFile->startDownloadingResources();
    }
}

void AppFrameworkDelegate::_fwCheckerDownloadFinished(QUrl url, QString localFilePath)
{
    DLog("Fw checker downloaded from Url: ") << url;

    FrameworkCheckInternalModel fwCheckInternalModel;
    int foundItemIdx = -1;
    for(int i = 0; i < _tmp_currRequiredDownloadNeededFrameworks.size() && foundItemIdx < 0; i++) {
        if(_tmp_currRequiredDownloadNeededFrameworks[i].getFrameworkCheckerDownloadUrl() == url) {
            fwCheckInternalModel = _tmp_currRequiredDownloadNeededFrameworks[i];
            foundItemIdx = i;
        }
    }

    if(foundItemIdx >= 0)
    {
        // save it to file
        QString tempZipFilePath = localFilePath;

        // unzip it
        QString fwCheckerLocalDirPath = PathHelper::combineAndCleanPathes(AppSpecificPathHelper::getLocalFrameworkCheckerDirectory(), fwCheckInternalModel.getFrameworkCheckModel().getFrameworkInfo().getFrameworkId());
        if(!PathHelper::deleteThisDirectoryAndEverythingBelow(fwCheckerLocalDirPath)) {
            WLog("Cannot delete fw checker dir: ") << fwCheckerLocalDirPath;
            return;
        }
        if(!PathHelper::ensureDirectoryCreated(fwCheckerLocalDirPath)) {
            WLog("Cannot create fw checker dir: ") << fwCheckerLocalDirPath;
            return;
        }
        Uncompressor::uncompress(tempZipFilePath, fwCheckerLocalDirPath, true);

        // get the fw checker executable path
        DLog("Uncompressed folder: ") << fwCheckerLocalDirPath;
        QDir fwCheckerLocalDir(fwCheckerLocalDirPath);
        QStringList entriesInFolder = fwCheckerLocalDir.entryList(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files | QDir::Hidden);
        DLog("Found entry cnt: ") << entriesInFolder.size();

        if(entriesInFolder.isEmpty()) {
            WLog("No file found in the uncompressed folder.");
            return;
        }

        QString fwCheckerLocalExecutablePath;
        //
        // try to find fw-checker runner/launcher
        //
//        if(entriesInFolder.size() >= 2)
//        {
            // search for appwhirr 'checker launcher'
//            bool isCheckerLauncherFound = false;
            for(int i = 0; i < entriesInFolder.size() && fwCheckerLocalExecutablePath.isEmpty(); i++)
            {
                QString currEntryFullFilePath = PathHelper::combineAndCleanPathes(fwCheckerLocalDirPath, entriesInFolder[i]);
                QFileInfo currEntryFileInfo(currEntryFullFilePath);
                DLog( QString("Found entry: %1 | basename: %2").arg(currEntryFullFilePath).arg(currEntryFileInfo.baseName()) );
                if( currEntryFileInfo.baseName() == ConfigManager::getFrameworkCheckerLauncherExecutableFileNameWithoutExtension() )
                {
                    fwCheckerLocalExecutablePath = currEntryFullFilePath;
                }
            }

//            if(isCheckerLauncherFound) {
//                DLog("AppWhirr fw checker launcher found.");
//            }
//            else {
//                WLog("Cannot find AppWhirr fw checker launcher!");
//                return;
//            }
//        }


        if(fwCheckerLocalExecutablePath.isEmpty())
        {
            //
            // no fw-checker runner/launcher found - check whether only 1 fw-checker is available (except special mac-folder)
            //  - if only 1 found use it as the checker
            //

            DLog("No AppWhirr fw checker launcher found - check for possible single fw-checker...");

            bool isSpecialMacOsFolderFound = false;
            for(int i = 0; i < entriesInFolder.size(); i++) {
                DLog("Found entry: ") << entriesInFolder[i];
                if(entriesInFolder[i] != "__MACOSX") {
                    fwCheckerLocalExecutablePath = PathHelper::combineAndCleanPathes(fwCheckerLocalDirPath, entriesInFolder[i]);
                }
                else {
                    isSpecialMacOsFolderFound = true;
                }
            }

            if( (isSpecialMacOsFolderFound && entriesInFolder.size() > 2) ||
                    (!isSpecialMacOsFolderFound && entriesInFolder.size() > 1)
                    )
            {
                WLog("No fw-checker found!");
                return;
            }
        }

        if(fwCheckerLocalExecutablePath.isEmpty()) {
            WLog("No fw-checker found in the uncompressed folder.");
            return;
        }

        DLog("Fw-checker path: ") << fwCheckerLocalExecutablePath;
        // remove the zip
        if(!QFile(tempZipFilePath).remove()) {
            // it's not critical but log it
            WLog("Cannot delete the tmp fw-checker zip: ") << tempZipFilePath;
        }

        fwCheckInternalModel.getFrameworkCheckModel_AllowModify()._setFrameworkCheckerAppPath(fwCheckerLocalExecutablePath);

        // store into database
        FrameworkCheckerDatabase *fwCheckerDb = new FrameworkCheckerDatabase(this->getFrameworkCheckerDatabasePath());
        FrameworkCheckModel tmpFwCheckModel = fwCheckInternalModel.getFrameworkCheckModel();
        FrameworkCheckerInfo fwCheckerInfo(tmpFwCheckModel.getFrameworkInfo().getFrameworkId(),
                                           tmpFwCheckModel.getFrameworkInfo().getFrameworkPrintableName(),
                                           tmpFwCheckModel.getFrameworkCheckerAppPath(),
                                           tmpFwCheckModel.getFrameworkCheckerUpdateTimestamp());
        if(!fwCheckerDb->addFrameworkChecker(fwCheckerInfo)) {
            WLog("Cannot store the framework checker into database.");
        }

        // store in internal 'already downloaded' list
        _currRequiredDownloadedFrameworks.append(fwCheckInternalModel);

        // remove from the 'download required' list
        _tmp_currRequiredDownloadNeededFrameworks.removeAt(foundItemIdx);
    }
    else {
        WLog("No fw-checker found for url: ") << url;
    }
}

void AppFrameworkDelegate::_fwCheckerDownloadProgressForUrl(int percent, QUrl url)
{
    //
}

void AppFrameworkDelegate::_fwCheckerAllDownloadResourceRequestsFinished()
{
    DLog("Required fw checkers download finished.");

    // every download finished - check whether every required fw retrieved
    if(_tmp_currRequiredDownloadNeededFrameworks.isEmpty()) {
        // success - start checking
        this->_startFrameworkChecking();
    }
    else {
        WLog("Some fw checker cannot be retrieved.");
        this->__checkingFailedForCurrentAppTryNextOne(tr("Some checker cannot be retrieved."));
    }
}

void AppFrameworkDelegate::_fwCheckerDownloadFailed(QUrl url, QString localFilePath, QString errorMessage)
{
    WLog("Fw cheker download faild for url: ") << url << "path:" << localFilePath << " with error: " << errorMessage;
}


void AppFrameworkDelegate::_startFrameworkChecking()
{
    DLog("Start fw checking.");

    _currFrameworkCheckType = FCT_InitialChecking;

    if(!_tmp_currRequiredDownloadNeededFrameworks.isEmpty()) {
        WLog("Some required framework checkers are not downloaded.");
        this->__checkingFailedForCurrentAppTryNextOne(tr("Some checker cannot be retrieved."));
        return;
    }

    QList<FrameworkCheckModel> tmpFwCheckModels;
    Q_FOREACH(FrameworkCheckInternalModel fwCheckInternalModel, _currRequiredDownloadedFrameworks) {
        tmpFwCheckModels.append(fwCheckInternalModel.getFrameworkCheckModel());
    }

    _frameworkCheckManager->checkForFrameworksAsync(tmpFwCheckModels);
}

void AppFrameworkDelegate::_fwCheckerFrameworksAvailableInSystem(QList<FrameworkCheckModel> frameworkModels)
{
    if(_currFrameworkCheckType == FCT_AfterInstallReChecking) {
        // don't care
        return;
    }

    if(frameworkModels.isEmpty()) {
        return;
    }

    DLog("Frameworks found in system:");
    Q_FOREACH(FrameworkCheckModel fwCheckModel, frameworkModels) {
        DLog("Fw: ") << fwCheckModel.getFrameworkInfo().getFrameworkId() << fwCheckModel.getFrameworkInfo().getFrameworkPrintableName();
    }
}

void AppFrameworkDelegate::_fwCheckerFrameworksNotFoundInSystem(QList<FrameworkCheckModel> frameworkModels)
{    
    if(frameworkModels.isEmpty()) {
        return;
    }

    if(_currFrameworkCheckType == FCT_AfterInstallReChecking) {
        WLog("Framework re-check failed.");
        _isFrameworkReCheckFailed = true;
        return;
    }

    DLog("Frameworks NOT FOUND in system:");
    Q_FOREACH(FrameworkCheckModel fwCheckModel, frameworkModels) {
        DLog("Fw: ") << fwCheckModel.getFrameworkInfo().getFrameworkId() << fwCheckModel.getFrameworkInfo().getFrameworkPrintableName();
    }

    Q_FOREACH(FrameworkCheckModel fwCheckModel, frameworkModels)
    {
        bool isFound = false;
        for(int i = 0; i < _currRequiredDownloadedFrameworks.size() && !isFound; i++) {
            if(_currRequiredDownloadedFrameworks[i].getFrameworkCheckModel().getFrameworkInfo().getFrameworkId() == fwCheckModel.getFrameworkInfo().getFrameworkId()) {
                isFound = true;
                _currFrameworksNotFoundInSystem.append(_currRequiredDownloadedFrameworks[i]);
            }
        }
        if(!isFound) {
            WLog("Cannot find matching framework for framework id: ") << fwCheckModel.getFrameworkInfo().getFrameworkId();
        }
    }
}

void AppFrameworkDelegate::_fwCheckerInSystemCheckingFinished()
{

    if(_currFrameworkCheckType == FCT_AfterInstallReChecking) {
        if(_isFrameworkReCheckFailed) {
            WLog("Some framework re-check faild.");
            this->__checkingFailedForCurrentAppTryNextOne(tr("Framework installation incorrect."));
        }
        else {
            DLog("Framework re-check passed!");
            // we're finished
            this->__checkingFinishedForCurrentAppTryNextOne();
        }
        return;
    }

    if(_currFrameworksNotFoundInSystem.isEmpty()) {
        DLog("Every required framework found.");
        this->__checkingFinishedForCurrentAppTryNextOne();
        return;
    }

    // some frameworks require install
    GrabApplicationQueueItem currentGrabItem = _appsWaitingForFrameworkCheck.first();
    Q_EMIT __frameworksRequireInstallForApp(currentGrabItem.getAppId(), currentGrabItem.getAppName());
}

void AppFrameworkDelegate::__allowFrameworkInstallForApp(QString appId)
{
    GrabApplicationQueueItem currentGrabItem = _appsWaitingForFrameworkCheck.first();
    if(currentGrabItem.getAppId() != appId) {
        WLog("Invalid appId, cannot allow fw install for it: ") << appId << currentGrabItem.getAppId();
        this->__checkingFailedForCurrentAppTryNextOne(tr("Framework install [allow]/deny error."));
        return;
    }
    DLog("Install frameworks for app: ") << currentGrabItem.getAppId() << currentGrabItem.getAppName();
    this->_startFrameworkInstallation();
}

void AppFrameworkDelegate::__denyFrameworkInstallForApp(QString appId)
{
    GrabApplicationQueueItem currentGrabItem = _appsWaitingForFrameworkCheck.first();
    if(currentGrabItem.getAppId() != appId) {
        WLog("Invalid appId, cannot allow fw install for it: ") << appId << currentGrabItem.getAppId();
        this->__checkingFailedForCurrentAppTryNextOne(tr("Framework install allow/[deny] error."));
        return;
    }

    DLog("Don't install frameworks for app: ") << currentGrabItem.getAppId() << currentGrabItem.getAppName();
    this->__checkingFailedForCurrentAppTryNextOne(tr("Framework installation aborted."));
}


// ------------------------------
// --- framework installation ---

void AppFrameworkDelegate::_startFrameworkInstallation()
{
    if(_currFrameworksNotFoundInSystem.isEmpty()) {
        WLog("No 'framework-not-found-in-system' item found.");
        this->__checkingFailedForCurrentAppTryNextOne(tr("Internal framework checking error."));
        return;
    }

    // get required frameworks (and download them)
    GrabApplicationQueueItem currentGrabItem = _appsWaitingForFrameworkCheck.first();

    AW_QOBJECT_SAFE_DELETE(_commDescriptionGateway);
    _commDescriptionGateway = new CommunicationDescriptionGateway;

    connect(_commDescriptionGateway, SIGNAL(finished(QList<AW::Common::Communication::BaseNetworkEntity*>)), this, SLOT(_appFrameworkInstallerDescriptionRequestFinished(QList<AW::Common::Communication::BaseNetworkEntity*>)));
    connect(_commDescriptionGateway, SIGNAL(failedToRetrieveDescription(QString)), this, SLOT(_appFrameworkInstallerFailedToGetDescription(QString)));
//    connect(_commDescriptionGateway, SIGNAL(authenticationChallengeHappened()), this, SLOT(_authenticationChallangeHappened_PutBackToQueue()));

    // start getting framework checker info
    _commDescriptionGateway->performThisRequest(new AppFrameworkInfoNetworkRequest(currentGrabItem.getAppId()));
}

void AppFrameworkDelegate::_appFrameworkInstallerDescriptionRequestFinished(QList<AW::Common::Communication::BaseNetworkEntity *> entities)
{
    GrabApplicationQueueItem relatedGrabItem;

    AW_QOBJECT_SAFE_DELETE(_commDescriptionGateway);

    if (!_appsWaitingForFrameworkCheck.isEmpty()) {
        // finished - remove from the queue
        relatedGrabItem = _appsWaitingForFrameworkCheck.first();
    } else {
        WLog("No item in waiting-for-framework-check queue. This should never happen here!");
        return;
    }

    // If there is no entity to iterate on - no framework required
    if (entities.isEmpty()) {
        WLog("No framework installer found for app (should be!): ") << relatedGrabItem.getAppId() << relatedGrabItem.getAppName();
        this->__checkingFailedForCurrentAppTryNextOne(tr("Framework installer download info error."));
        return;
    }

    // if some framework required
    Q_FOREACH(AW::Common::Communication::BaseNetworkEntity *entity, entities)
    {
        AppFrameworkInfoNetworkEntity *castedEntity = dynamic_cast<AppFrameworkInfoNetworkEntity *>(entity);
        if(castedEntity == NULL) {
            WLog("Invalid entity.");
            continue;
        }

        // get the related (internal) fw check model
        FrameworkCheckInternalModel fwCheckInternalModel;
        bool isFound = false;
        for(int i = 0; i < _currFrameworksNotFoundInSystem.size() && !isFound; i++) {
            if(_currFrameworksNotFoundInSystem[i].getFrameworkCheckModel().getFrameworkInfo().getFrameworkId() == castedEntity->getFrameworkId()) {
                isFound = true;
                fwCheckInternalModel = _currFrameworksNotFoundInSystem[i];
            }
        }

        if(!isFound) {
            WLog("Cannot find matching checker data for framework ID: ") << castedEntity->getFrameworkId();
            continue; // it's probably already installed, that's why it's not in the frameworks-not-found-in-system list
//            this->__checkingFailedForCurrentAppTryNextOne(tr("Internal framework checking error."));
//            return;
        }

        fwCheckInternalModel._setFrameworkDownloadUrl(castedEntity->getFrameworkDownloadUrl());

//        FrameworkCheckInternalModel fwCheckInternalModel(FrameworkCheckModel(FrameworkInfo(castedEntity->getFrameworkId(), castedEntity->getFrameworkName()), QString(), QString(), QString(), QString()), castedEntity->getFrameworkDownloadUrl(), QUrl());

        _currFrameworkInstallersWaitingForDownload.append(fwCheckInternalModel);
    }

    if(_currFrameworkInstallersWaitingForDownload.isEmpty()) {
        WLog("After parsing error: No framework installer found for app (should be!): ") << relatedGrabItem.getAppId() << relatedGrabItem.getAppName();
        this->__checkingFailedForCurrentAppTryNextOne(tr("Framework installer download info error."));
        return;
    }

    // start to download them
    this->_startFrameworkInstallerDownloading();
}

void AppFrameworkDelegate::_appFrameworkInstallerFailedToGetDescription(QString errorMessage)
{
    WLog("Framework installer - get info error: ") << errorMessage;
    this->__checkingFailedForCurrentAppTryNextOne(tr("Cannot get framework installer info: %1").arg(errorMessage));
}

void AppFrameworkDelegate::_startFrameworkInstallerDownloading()
{
    DLog("Downloading required framework installers.");

//    AW_QOBJECT_SAFE_DELETE(_commResourceGateway);
//    _commResourceGateway = new CommunicationResourceGateway;

//    connect(_commResourceGateway, SIGNAL(resourceReceivedForURL(QUrl,QByteArray)), this, SLOT(_fwInstallerDownloadRetrievedResourceForURL(QUrl,QByteArray)));
//    connect(_commResourceGateway, SIGNAL(downloadProgressForUrl(int,QUrl)), this, SLOT(_fwInstallerDownloadProgressForUrl(int,QUrl)));
//    connect(_commResourceGateway, SIGNAL(finished()), this, SLOT(_fwInstallerAllDownloadResourceRequestsFinished()));
//    connect(_commResourceGateway, SIGNAL(resourceDownloadFailedForUrl(QUrl)), this, SLOT(_fwInstallerDownloadFailedForUrl(QUrl)));

//    Q_FOREACH(FrameworkCheckInternalModel fwCheckInternalModel, _currFrameworkInstallersWaitingForDownload) {
//        _commResourceGateway->addResourceURL(fwCheckInternalModel.getFrameworkDownloadUrl());
//    }

//    _commResourceGateway->startRequests();


    AW_QOBJECT_SAFE_DELETE(_resourceDownloadStreamToFile);
    _resourceDownloadStreamToFile = AW::Client::Communication::AWResourceDownloadStreamToFile::createDefaultResourceDownloadStreamToFile(NULL);

    connect(_resourceDownloadStreamToFile, SIGNAL(resourceDownloadProgressForUrl(int,QUrl)), this, SLOT(_fwInstallerDownloadProgressForUrl(int,QUrl)));
    connect(_resourceDownloadStreamToFile, SIGNAL(resourceDownloadFinishedWithSuccess(QUrl,QString)), this, SLOT(_fwInstallerDownloaded(QUrl,QString)));
    connect(_resourceDownloadStreamToFile, SIGNAL(resourceDownloadFailed(QUrl,QString,QString)), this, SLOT(_fwInstallerDownloadFailed(QUrl,QString,QString)));
    connect(_resourceDownloadStreamToFile, SIGNAL(finishedWithAllResourceDownloads()), this, SLOT(_fwInstallerAllDownloadResourceRequestsFinished()));

    bool isCannotStartStreaming = false;
    Q_FOREACH(FrameworkCheckInternalModel fwCheckInternalModel, _currFrameworkInstallersWaitingForDownload)
    {
        QString tempZipFilePath = this->_getFrameworkInstallerTmpZipPathById(fwCheckInternalModel.getFrameworkCheckModel().getFrameworkInfo().getFrameworkId());
        if( !_resourceDownloadStreamToFile->addAndInitializeResourceDownloadRequest(fwCheckInternalModel.getFrameworkDownloadUrl(), tempZipFilePath, true) ) {
            isCannotStartStreaming = true;
        }
    }

    if(isCannotStartStreaming) {
        this->__checkingFailedForCurrentAppTryNextOne("Cannot save required framework installer");
    }
    else {
        _resourceDownloadStreamToFile->startDownloadingResources();
    }
}

void AppFrameworkDelegate::_fwInstallerDownloaded(QUrl url, QString localFilePath)
{
    DLog("Fw installer downloaded from Url: ") << url;

    FrameworkCheckInternalModel fwCheckInternalModel;
    int foundItemIdx = -1;
    for(int i = 0; i < _currFrameworkInstallersWaitingForDownload.size() && foundItemIdx < 0; i++) {
        if(_currFrameworkInstallersWaitingForDownload[i].getFrameworkDownloadUrl() == url) {
            fwCheckInternalModel = _currFrameworkInstallersWaitingForDownload[i];
            foundItemIdx = i;
        }
    }

    if(foundItemIdx >= 0)
    {
        // save it to file
        QString tempZipFilePath = localFilePath;


        // unzip it
        QString fwInstallerLocalDirPath = PathHelper::combineAndCleanPathes(AppSpecificPathHelper::getLocalFrameworkInstallerDirectory(), fwCheckInternalModel.getFrameworkCheckModel().getFrameworkInfo().getFrameworkId());
        if(!PathHelper::ensureDirectoryCreated(fwInstallerLocalDirPath)) {
            WLog("Cannot create dir: ") << fwInstallerLocalDirPath;
            return;
        }
        Uncompressor::uncompress(tempZipFilePath, fwInstallerLocalDirPath, true);

        // get the fw installer executable path
        DLog("Uncompressed folder: ") << fwInstallerLocalDirPath;
        QDir fwInstallerLocalDir(fwInstallerLocalDirPath);
        QStringList entriesInFolder = fwInstallerLocalDir.entryList(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files | QDir::Hidden);

        QString fwInstallerLocalExecutablePath;
        if(entriesInFolder.size() > 2) {
            WLog("Too much files found.");
            return;
        }
        else if(entriesInFolder.isEmpty()) {
            WLog("No file found in the uncompressed folder.");
            return;
        }
        else {
            for(int i = 0; i < entriesInFolder.size() && fwInstallerLocalExecutablePath.isEmpty(); i++) {
                DLog("Found entry: ") << entriesInFolder[i];
                if(entriesInFolder[i] != "__MACOSX") {
                    fwInstallerLocalExecutablePath = PathHelper::combineAndCleanPathes(fwInstallerLocalDirPath, entriesInFolder[i]);
                }
            }
        }

        if(fwInstallerLocalExecutablePath.isEmpty()) {
            WLog("No fw-installer found in the uncompressed folder.");
            return;
        }

        DLog("Fw-installer path: ") << fwInstallerLocalExecutablePath;
        // remove the zip
        if(!QFile(tempZipFilePath).remove()) {
            // it's not critical but log it
            WLog("Cannot delete the tmp fw-installer zip: ") << tempZipFilePath;
        }

        fwCheckInternalModel.getFrameworkCheckModel_AllowModify()._setFrameworkInstallerPath(fwInstallerLocalExecutablePath);

        // store in internal 'already downloaded' list
        _currFrameworksWaitingForInstallation.enqueue(fwCheckInternalModel);
        _currFrameworksForAfterInstallReCheck.append(fwCheckInternalModel);

        // remove from the 'download required' list
        _currFrameworkInstallersWaitingForDownload.removeAt(foundItemIdx);
    }
    else {
        WLog("No fw-installer found for url: ") << url;
    }
}

void AppFrameworkDelegate::_fwInstallerDownloadProgressForUrl(int percent, QUrl url)
{
    //
}

void AppFrameworkDelegate::_fwInstallerAllDownloadResourceRequestsFinished()
{
    DLog("Required fw installer downloads finished.");

    // every download finished - check whether every required fw installer retrieved
    if(_currFrameworkInstallersWaitingForDownload.isEmpty()) {
        // success - start installation
        this->_installFrameworkFromWaitingQueue();
    }
    else {
        WLog("Some fw installer cannot be retrieved.");
        this->__checkingFailedForCurrentAppTryNextOne(tr("Some framework installer cannot be retrieved."));
    }
}

void AppFrameworkDelegate::_fwInstallerDownloadFailed(QUrl url, QString localFilePath, QString errorMessage)
{
    WLog("Fw installer failed to download: ") << url << " file path: " << localFilePath << " error: " << errorMessage;
}


void AppFrameworkDelegate::_installFrameworkFromWaitingQueue()
{
    if(_currFrameworksWaitingForInstallation.isEmpty()) {
        DLog("No more frameworks waiting for install. Hurray!");
        this->_startAfterFrameworkInstallationChecking();
        return;
    }

    // install the next framework
    FrameworkCheckInternalModel fwForInstall = _currFrameworksWaitingForInstallation.dequeue();
    _frameworkInstallManager->startFrameworkInstallAsync(fwForInstall.getFrameworkCheckModel());
}

void AppFrameworkDelegate::_fwInstallerFrameworkInstalled(FrameworkCheckModel frameworkModel)
{
    DLog("Framework installed: ") << frameworkModel.getFrameworkInfo().getFrameworkId() << frameworkModel.getFrameworkInfo().getFrameworkPrintableName();
    this->_installFrameworkFromWaitingQueue();
}

void AppFrameworkDelegate::_fwInstallerFrameworkInstallFailed(FrameworkCheckModel frameworkModel, QString errorMessage)
{
    DLog("Framework install failed: ") << frameworkModel.getFrameworkInfo().getFrameworkId() << frameworkModel.getFrameworkInfo().getFrameworkPrintableName() << errorMessage;
    this->__checkingFailedForCurrentAppTryNextOne(tr("Framework installation failed: %1").arg(errorMessage));
}

void AppFrameworkDelegate::_startAfterFrameworkInstallationChecking()
{
    DLog("Start framework installation(s).");
    _currFrameworkCheckType = FCT_AfterInstallReChecking;
    _isFrameworkReCheckFailed = false;

    if(!_currFrameworksWaitingForInstallation.isEmpty()) {
        WLog("Some required framework are not installed.");
        this->__checkingFailedForCurrentAppTryNextOne(tr("Framework installation failed."));
        return;
    }

    QList<FrameworkCheckModel> tmpFwCheckModels;
    Q_FOREACH(FrameworkCheckInternalModel fwCheckInternalModel, _currFrameworksForAfterInstallReCheck)
    {
        // DON'T modify items in a foreach loop!
        FrameworkCheckModel recheckFwCheckModel = fwCheckInternalModel.getFrameworkCheckModel();
        QStringList fwCheckModeArg;
        fwCheckModeArg << "-checkmode" << "recheck";
        recheckFwCheckModel.replaceOptionalFrameworkCheckerArg(fwCheckModeArg);
//        FrameworkCheckInternalModel recheckFwCheckInternalModel(recheckFwCheckModel, fwCheckInternalModel.getFrameworkDownloadUrl(), fwCheckInternalModel.getFrameworkCheckerDownloadUrl());
        tmpFwCheckModels.append(recheckFwCheckModel);
    }

    _frameworkCheckManager->checkForFrameworksAsync(tmpFwCheckModels);
}


// ---------------
// --- utility ---

bool AppFrameworkDelegate::getIsCheckingForFramework() const
{
    return _isCheckingForFramework;
}

void AppFrameworkDelegate::setIsCheckingForFramework(bool value)
{
    _isCheckingForFramework = value;
}

void AppFrameworkDelegate::__checkingFinishedForCurrentAppTryNextOne()
{
    if(_appsWaitingForFrameworkCheck.isEmpty()) {
        WLog("Empty queue!");
        return;
    }
    GrabApplicationQueueItem currentGrabItem = _appsWaitingForFrameworkCheck.dequeue();
    Q_EMIT frameworksAvailableForApp(currentGrabItem);

    // checking finished for this app, try the next one
    this->setIsCheckingForFramework(false);
    Q_EMIT __tryNextFrameworkCheckInQueue();
}

void AppFrameworkDelegate::__checkingFailedForCurrentAppTryNextOne(QString errorMessage)
{
    if(_appsWaitingForFrameworkCheck.isEmpty()) {
        WLog("Empty queue!");
        return;
    }
    GrabApplicationQueueItem currentGrabItem = _appsWaitingForFrameworkCheck.dequeue();
    Q_EMIT frameworksCannotBeInstalledForApp(currentGrabItem, tr("Framework install error: %1").arg(errorMessage));

    // checking finished for this app, try the next one
    this->setIsCheckingForFramework(false);
    Q_EMIT __tryNextFrameworkCheckInQueue();
}

QString AppFrameworkDelegate::getFrameworkCheckerDatabasePath() const
{
    return PathHelper::combineAndCleanPathes(AppSpecificPathHelper::getLocalFrameworkRootDirectory(), "fwChecker.db");
}

QString AppFrameworkDelegate::_getFrameworkCheckerTmpZipPathById(QString frameworkId)
{
    QString tempZipFilePath = PathHelper::combineAndCleanPathes(AppSpecificPathHelper::getLocalAppWhirrTmpDirectory(), "fwchecker-" + frameworkId + ".zip");
    return tempZipFilePath;
}

QString AppFrameworkDelegate::_getFrameworkInstallerTmpZipPathById(QString frameworkId)
{
    QString tempZipFilePath = PathHelper::combineAndCleanPathes(AppSpecificPathHelper::getLocalAppWhirrTmpDirectory(), "fwinstaller-" + frameworkId + ".zip");
    return tempZipFilePath;
}
