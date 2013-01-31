#include "clientselfupdatemanager.h"

#include <QProcess>
#include <QThreadPool>
#include <QDir>
#include <QTimer>

// client version checking
#include "Application/Communication/Gateway/communicationdescriptiongateway.h"
#include "Application/Communication/Entity/applicationbaseinfosnetworkentity.h"
#include "Application/Communication/Entity/grabapplicationnetworkentity.h"
//#include "Application/Communication/Gateway/communicationresourcegateway.h"
#include "Application/Communication/awresourcedownloadstreamtofile.h"
#include "Application/Communication/Entity/appversionnetworkentity.h"

// utility
#include "Common/Helpers/pathhelper.h"
#include "Application/appspecificpathhelper.h"

#include "Common/Uncompressor/uncompressor.h"
#include "Common/InterProcessCommunication/interprocesscommunicator.h"
#include "Common/System/systemprofiler.h"
#include "Common/Helpers/platformflagshelper.h"

#include "./background_clientuncompressor.h"

// configuration
#include "Settings/configmanager.h"

ClientSelfUpdateManager::ClientSelfUpdateManager(QObject *parent) :
    QObject(parent),
    _clientVersionInfoCheckerCommunicationGateway(NULL),
    _clientZipUrlGetterCommunicationGateway(NULL),
    _clientNewVersionZipDownloader(NULL),
    _isInSelfUpdatingProcess(false)
{
    _clientSelfUpdateCheckTimer = new QTimer;
    _clientSelfUpdateCheckTimer->setInterval(ConfigManager::getClientSelfUpdateCheckIntervalMilliseconds());
    _clientSelfUpdateCheckTimer->setSingleShot(false);
    connect(_clientSelfUpdateCheckTimer, SIGNAL(timeout()), this, SLOT(startCheckIsNewVersionAvailableImmediately()));

    // internal connections
    connect(this, SIGNAL(newVersionFound()), this, SLOT(__clientSelfUpdatingProcessEnded()));
    connect(this, SIGNAL(noNewVersionFound()), this, SLOT(__clientSelfUpdatingProcessEnded()));
    connect(this, SIGNAL(failedToGetNewVersion(QString)), this, SLOT(__clientSelfUpdatingProcessEnded(QString)));

    connect(this, SIGNAL(downloadFinished()), this, SLOT(__clientSelfUpdatingProcessEnded()));
    connect(this, SIGNAL(downloadFailedWithError(QString)), this, SLOT(__clientSelfUpdatingProcessEnded(QString)));

    connect(this, SIGNAL(readyToQuitAndPerformInstall()), this, SLOT(__clientSelfUpdatingProcessEnded()));
    connect(this, SIGNAL(installFailedWithError(QString)), this, SLOT(__clientSelfUpdatingProcessEnded(QString)));
}

void ClientSelfUpdateManager::__clientSelfUpdatingProcessEnded() {
    this->_isInSelfUpdatingProcess = false;
}

void ClientSelfUpdateManager::__clientSelfUpdatingProcessEnded(QString dontUse) {
    this->__clientSelfUpdatingProcessEnded();
}


bool ClientSelfUpdateManager::clearRelatedTmpDirectories() {
    return PathHelper::deleteThisDirectoryAndEverythingBelow(AppSpecificPathHelper::getTmp_ClientSelfUpdate_RootPath());
}

bool ClientSelfUpdateManager::getIsInSelfUpdatingProcess() {
    return this->_isInSelfUpdatingProcess;
}

// ---------------------
// --- version check ---

void ClientSelfUpdateManager::startCheckIsNewVersionAvailablePeriodically()
{
    _clientSelfUpdateCheckTimer->start();
}

void ClientSelfUpdateManager::startCheckIsNewVersionAvailableImmediately()
{
    if(_isInSelfUpdatingProcess) {
        WLog("Already checking for new version, won't start a new check.");
        return;
    }
    _isInSelfUpdatingProcess = true;

    DLog("--client self update check");
    if(_clientVersionInfoCheckerCommunicationGateway == NULL) {
        this->_clientVersionInfoCheckerCommunicationGateway = new CommunicationDescriptionGateway(this);

        connect(_clientVersionInfoCheckerCommunicationGateway, SIGNAL(failedToRetrieveDescription(QString)), this, SLOT(_failedToGetVersionInfoWithError(QString)));
        connect(_clientVersionInfoCheckerCommunicationGateway, SIGNAL(finished(QList<AW::Common::Communication::BaseNetworkEntity*>)), this, SLOT(_finishedGettingVersionInfo(QList<AW::Common::Communication::BaseNetworkEntity*>)));
        connect(_clientVersionInfoCheckerCommunicationGateway, SIGNAL(authenticationChallengeHappened()), this, SLOT(_versionInfoAuthenticationChallangeHappened()));
    }
    else {
        WLog("Another checking is active. Won't start another one.");
        return;
    }

    _clientVersionInfoCheckerCommunicationGateway->requestForClientUpdateVersionCheck();
}

void ClientSelfUpdateManager::_failedToGetVersionInfoWithError(const QString& error)
{
    WLog("Failed to get version-info with error: ") << error;

    AW_QOBJECT_SAFE_DELETE(_clientVersionInfoCheckerCommunicationGateway);

    Q_EMIT failedToGetNewVersion(error);
}

void ClientSelfUpdateManager::_versionInfoAuthenticationChallangeHappened()
{
    AW_QOBJECT_SAFE_DELETE(_clientVersionInfoCheckerCommunicationGateway);

    WLog("Auth. challenge during version-info check. Should not happen.");
    Q_EMIT failedToGetNewVersion("Internal error (authentication). Not good...");
}

void ClientSelfUpdateManager::_finishedGettingVersionInfo(const QList<AW::Common::Communication::BaseNetworkEntity*>& entities)
{
#ifdef ENABLE_COMMUNICATION_DEBUG_LOGGING
    DLog("networkRequestFinished");
#endif

    AW_QOBJECT_SAFE_DELETE(_clientVersionInfoCheckerCommunicationGateway);

    // If there is no entity to iterate on...
    if (entities.isEmpty()) {
        WLog("Request finished but 0 entities returned. Oh my..");
        Q_EMIT failedToGetNewVersion(tr("0 versions found. It's not good..."));
        return;
    }

    bool isFailed = false;
    bool isNewVersionFound = false;
    for(int i = 0; i < entities.size() && !isNewVersionFound; i++) {
        AW::Common::Communication::BaseNetworkEntity *currEntity = entities[i];
        AppVersionNetworkEntity *currCastedEntity = dynamic_cast<AppVersionNetworkEntity *>(currEntity);

        if(currCastedEntity == NULL) {
            WLog("Invalid communication entity - cannot cast it!");
            isFailed = true;
        }

        else
        {
            DLog("Found current client version: ") << currCastedEntity->getVersionInfo();
            if(currCastedEntity->getVersionInfo().isEmpty()) {
                isFailed = true;
            }
            else if(currCastedEntity->getVersionInfo() != ConfigManager::getClientVersion()) {
                isNewVersionFound = true;
            }
        }
    }

    if(isFailed) {
        Q_EMIT failedToGetNewVersion("Internal error. Not good...");
    }
    else if(!isNewVersionFound) {
        Q_EMIT noNewVersionFound();
    }
    else {
        Q_EMIT newVersionFound();
    }
}


// ----------------
// --- download ---

void ClientSelfUpdateManager::startToDownloadNewAvailableVersion()
{
    if(_isInSelfUpdatingProcess) {
        WLog("Already checking for new version, won't start a new check.");
        return;
    }
    _isInSelfUpdatingProcess = true;

    _clientNewVersionZipUrl.clear(); // make it empty

    if(_clientZipUrlGetterCommunicationGateway == NULL) {
        this->_clientZipUrlGetterCommunicationGateway = new CommunicationDescriptionGateway(this);

        connect(_clientZipUrlGetterCommunicationGateway, SIGNAL(failedToRetrieveDescription(QString)), this, SLOT(_failedToGetUpdateZipInfoWithError(QString)));
        connect(_clientZipUrlGetterCommunicationGateway, SIGNAL(finished(QList<AW::Common::Communication::BaseNetworkEntity*>)), this, SLOT(_finishedGettingUpdateZipInfo(QList<AW::Common::Communication::BaseNetworkEntity*>)));
        connect(_clientZipUrlGetterCommunicationGateway, SIGNAL(authenticationChallengeHappened()), this, SLOT(_updateZipInfoAuthenticationChallangeHappened()));
    }

    _clientZipUrlGetterCommunicationGateway->requestForClientUpdateZip();
}


void ClientSelfUpdateManager::_failedToGetUpdateZipInfoWithError(const QString& error)
{
    WLog("Failed to get version-info with error: ") << error;

    AW_QOBJECT_SAFE_DELETE(_clientZipUrlGetterCommunicationGateway);

    Q_EMIT downloadFailedWithError(error);
}

void ClientSelfUpdateManager::_updateZipInfoAuthenticationChallangeHappened()
{
    AW_QOBJECT_SAFE_DELETE(_clientZipUrlGetterCommunicationGateway);

    WLog("Auth. challenge during zip-info check. Should not happen.");
    Q_EMIT downloadFailedWithError("Client-update - download error: Internal error (authentication). Not good...");
}

void ClientSelfUpdateManager::_finishedGettingUpdateZipInfo(const QList<AW::Common::Communication::BaseNetworkEntity*>& entities)
{
#ifdef ENABLE_COMMUNICATION_DEBUG_LOGGING
    DLog("networkRequestFinished");
#endif

    AW_QOBJECT_SAFE_DELETE(_clientZipUrlGetterCommunicationGateway);

    // If there is no entity to iterate on...
    if (entities.isEmpty()) {
        WLog("Request finished but 0 entities returned. Oh my..");
        Q_EMIT downloadFailedWithError(tr("New version is unavailable."));
        return;
    }

    // Find the best fit for the uuuuuser ( Thron Legacy;) )
//    unsigned int currentPlatformFlags = SystemProfiler::sharedSystemProfiler()->_getSystemFlagsAsUInt();
    bool isCompatibleVersionFound = false;
    for(int i = 0; i < entities.size() && !isCompatibleVersionFound; i++) {
        AW::Common::Communication::BaseNetworkEntity* currEntity = entities[i];
        GrabApplicationNetworkEntity* currCastedGrabEntity = dynamic_cast<GrabApplicationNetworkEntity*>(currEntity);

        if(currCastedGrabEntity == NULL) {
            WLog("Invalid communication entity - cannot cast it!");
        }

        else
        {
//            DLog(" - zip version: ") << currCastedGrabEntity->getZippedApplicationFlag();

//            if(PlatformFlagsHelper::_isTheseFlagsAreCompatibleIntVersion(currentPlatformFlags, currCastedGrabEntity->getZippedApplicationFlag()))
//            {
                // this is a compatible version, pick this!

                // map the url to the related appId
                _clientNewVersionZipUrl = currCastedGrabEntity->getZippedApplicationURL();

                // and stop processing others
                isCompatibleVersionFound = true;

//                DLog(" - Accepted zip version: ") << currCastedGrabEntity->getZippedApplicationFlag();
//                DLog(" - Accepted zip url: ") << currCastedGrabEntity->getZippedApplicationURL();
//                supportedPlatformFlags = PlatformFlagsHelper::_convertIntBasedFlagsToStringBased(currCastedGrabEntity->getZippedApplicationFlag());
//                FLAG_FOR_REVIEW_WITH_HINT("Code revision: if no compatible version found for the current platform then this code won't be executed. So supportedPlatformFlags is not requied.");
//                supportedPlatformFlags = 1;
//            }
        }
    }

    if(!isCompatibleVersionFound || this->_clientNewVersionZipUrl.toString().isEmpty()) {
        Q_EMIT downloadFailedWithError(tr("No compatible version found."));
    }
    else {
        // zip found - continue and download it
        this->_startDownloadingTheFoundNewVersionZip();
    }
}

// ---
// download the zip

void ClientSelfUpdateManager::_startDownloadingTheFoundNewVersionZip()
{
    // start downloading
    if (_clientNewVersionZipDownloader == NULL) {
        _clientNewVersionZipDownloader = AW::Client::Communication::AWResourceDownloadStreamToFile::createDefaultResourceDownloadStreamToFile(NULL);

        connect(_clientNewVersionZipDownloader, SIGNAL(resourceDownloadFinishedWithSuccess(QUrl,QString)), this, SLOT(_newVersionZipDownloaded(QUrl,QString)));
        connect(_clientNewVersionZipDownloader, SIGNAL(resourceDownloadProgressForUrl(int,QUrl)), this, SLOT(_newVersionZipDownloadProgress(int,QUrl)));
        connect(_clientNewVersionZipDownloader, SIGNAL(finishedWithAllResourceDownloads()), this, SLOT(_newVersionZipDownloadProgressFinished()));
        connect(_clientNewVersionZipDownloader, SIGNAL(resourceDownloadFailed(QUrl,QString,QString)), this, SLOT(_newVersionZipDownloadFailed(QUrl,QString,QString)));
    }

    QString tempZipFilePath = AppSpecificPathHelper::getTmp_ClientSelfUpdate_ZipPath();
    if( !_clientNewVersionZipDownloader->addAndInitializeResourceDownloadRequest(this->_clientNewVersionZipUrl, tempZipFilePath, true) ) {
        AW_QOBJECT_SAFE_DELETE(_clientNewVersionZipDownloader);
        Q_EMIT downloadFailedWithError(tr("Failed to download the new version - cannot save it."));
    }
    else {
        _clientNewVersionZipDownloader->startDownloadingResources();
    }
}

void ClientSelfUpdateManager::_newVersionZipDownloadFailed(QUrl url, QString localFilePath, QString errorMessage)
{
    WLog("Download failed: ") << url << " file-path: " << localFilePath << " error: " << errorMessage;
    AW_QOBJECT_SAFE_DELETE(_clientNewVersionZipDownloader);

    Q_EMIT downloadFailedWithError(tr("Failed to download the new version - probably internet connection lost."));
}

void ClientSelfUpdateManager::_newVersionZipDownloadProgress(int progress, QUrl url)
{
    Q_EMIT downloadProgress(progress);
}

void ClientSelfUpdateManager::_newVersionZipDownloaded(QUrl url, QString localFilePath)
{
//    // Save to file
//    QString tempZipFilePath = AppSpecificPathHelper::getTmp_ClientSelfUpdate_ZipPath();
//    QFile file(tempZipFilePath);

//    bool isFileError = false;
//    if(!file.open(QIODevice::WriteOnly)) {
//        isFileError = true;
//    } else {
//        if(file.write(resource) < 0) {
//            isFileError = true;
//        }
//    }

//    file.close();

//    if (isFileError) {
//        DLog("Failed to save zip file to path: " << tempZipFilePath);
//        Q_EMIT downloadFailedWithError(tr("Failed to save the update."));
//        return;
//    }

    Q_EMIT downloadFinished();
}

void ClientSelfUpdateManager::_newVersionZipDownloadProgressFinished()
{
    AW_QOBJECT_SAFE_DELETE(_clientNewVersionZipDownloader);
}

// ---------------
// --- install ---

void ClientSelfUpdateManager::startToPrepareInstallDownloadedNewVersion()
{
    if(_isInSelfUpdatingProcess) {
        WLog("Already checking for new version, won't start a new check.");
        return;
    }
    _isInSelfUpdatingProcess = true;

    this->uncompressDownloadedClient();
}

bool ClientSelfUpdateManager::startUpdaterInClientUpdateMode()
{
    if(_isInSelfUpdatingProcess) {
        WLog("Already checking for new version, won't start a new check.");
        return false;
    }
    _isInSelfUpdatingProcess = true;

    // start the installation
    DLog(" -- Start Updater in client-self-update mode");
    QString tmpUpdaterPath = PathHelper::combineAndCleanPathes(AppSpecificPathHelper::getTmp_ClientSelfUpdate_UncompressedTmpCopyWillPerformSelfUpdatePath(), AppSpecificPathHelper::getUpdaterExecutableFileName());
    QString tmpUpdaterWorkingDir = AppSpecificPathHelper::getTmp_ClientSelfUpdate_UncompressedTmpCopyWillPerformSelfUpdatePath();
    QString appWhirrUpdaterModeCode("2"); // client-self-update mode code

    // start the aw updater (AppWhirrU) app with
    //  * 'client self update' mode code (2)
    //  * current AppWhirr client executable directory path
    //  * current AppWhirr local data root directory path
#ifdef Q_WS_WIN
    if (!QProcess::startDetached(tmpUpdaterPath, QStringList() << appWhirrUpdaterModeCode << AppSpecificPathHelper::getClientExecutableDirectory() << AppSpecificPathHelper::getLocalRootDirectory(), tmpUpdaterWorkingDir)) {
#endif
#ifdef Q_WS_MAC
        if (!QProcess::startDetached("open", QStringList() << tmpUpdaterPath << "--args" << appWhirrUpdaterModeCode << AppSpecificPathHelper::getClientExecutableDirectory() << AppSpecificPathHelper::getLocalRootDirectory(), tmpUpdaterWorkingDir)) {
#endif
        WLog("Failed to start the installer in self-update mode for client-path: " << AppSpecificPathHelper::getClientExecutableDirectory());
        return false;
    }

    return true;
}

void ClientSelfUpdateManager::uncompressDownloadedClient() {
    Background_ClientUncompressor *uncompressor = new Background_ClientUncompressor;
    connect(uncompressor, SIGNAL(failedWithError(QString)), this, SLOT(_uncompressorFailedWithError(QString)));
    connect(uncompressor, SIGNAL(finishedSuccessfully()), this, SLOT(_uncompressorFinishedSuccessfully()));
    QThreadPool::globalInstance()->start(uncompressor);
}

void ClientSelfUpdateManager::_uncompressorFailedWithError(QString errorMessage)
{
    Q_EMIT installFailedWithError(errorMessage);
}

void ClientSelfUpdateManager::_uncompressorFinishedSuccessfully()
{
    // duplicate uncompressed version - this will be copied later as the new version
    if(!PathHelper::copyWholeDirectory(AppSpecificPathHelper::getTmp_ClientSelfUpdate_UncompressedTmpCopyWillPerformSelfUpdatePath(), AppSpecificPathHelper::getTmp_ClientSelfUpdate_UncompressedNewVersionPath(), true, PathHelper::SHM_UseAsFile, true)) {
        WLog("Copy of the unzipped new client version failed.");
        Q_EMIT installFailedWithError(QString("Self-update internal error: cannot duplicate the new version."));
        return;
    }
    else {
        DLog("Finished duplication");
    }

    Q_EMIT readyToQuitAndPerformInstall();
}
