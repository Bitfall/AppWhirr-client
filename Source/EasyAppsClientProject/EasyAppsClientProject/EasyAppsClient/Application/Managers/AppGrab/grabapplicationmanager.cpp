#include "grabapplicationmanager.h"

#include <QFile>
#include <QMetaClassInfo>

// delegates
#include "appgrabdelegate.h"
#include "appdownloaddelegate.h"
#include "appframeworkdelegate.h"
#include "appinstalldelegate.h"

// global objects
//#include "Application/globalobjectregistry.h"
//#include "Application/User/usermanager.h"

// utility
#include "Common/Helpers/pathhelper.h"
#include "Application/appspecificpathhelper.h"

GrabApplicationManager::GrabApplicationManager(QObject *parent) :
    QObject(parent)
{
    qRegisterMetaType<GrabApplicationQueueItem>("GrabApplicationQueueItem");

    // -- grab delegate
    _appGrabDelegate = new AppGrabDelegate(this);
    connect(this, SIGNAL(__grabThisItem(GrabApplicationQueueItem)), _appGrabDelegate, SLOT(grabThisItem(GrabApplicationQueueItem)));
    connect(_appGrabDelegate, SIGNAL(grabStarted(GrabApplicationQueueItem)), this, SLOT(__grabStarted(GrabApplicationQueueItem)));
    connect(_appGrabDelegate, SIGNAL(grabSucceeded(GrabApplicationQueueItem,QUrl)), this, SLOT(__grabSucceeded(GrabApplicationQueueItem,QUrl)));
    connect(_appGrabDelegate, SIGNAL(grabFailed(GrabApplicationQueueItem,QString)), this, SLOT(__grabFailed(GrabApplicationQueueItem,QString)));

    // -- app download delegate
    _appDownloadDelegate = new AppDownloadDelegate(this);
    connect(this, SIGNAL(__downloadThisItem(GrabApplicationQueueItem)), _appDownloadDelegate, SLOT(startDownloadingApp(GrabApplicationQueueItem)));
    connect(_appDownloadDelegate, SIGNAL(appDownloadStarted(GrabApplicationQueueItem)), this, SLOT(__downloadStarted(GrabApplicationQueueItem)));
    connect(_appDownloadDelegate, SIGNAL(appDownloadProgress(GrabApplicationQueueItem,int)), this, SLOT(__downloadProgress(GrabApplicationQueueItem,int)));
    connect(_appDownloadDelegate, SIGNAL(appDownloadFinished(GrabApplicationQueueItem)), this, SLOT(__downloadFinished(GrabApplicationQueueItem)));
    connect(_appDownloadDelegate, SIGNAL(appDownloadFailed(GrabApplicationQueueItem,QString)), this, SLOT(__downloadFailed(GrabApplicationQueueItem,QString)));

    // -- app framework delegate
    _appFrameworkDelegate = new AppFrameworkDelegate(this);
    connect(this, SIGNAL(__checkFrameworksForThisApp(GrabApplicationQueueItem)), _appFrameworkDelegate, SLOT(checkFrameworksForApp(GrabApplicationQueueItem)));
    connect(_appFrameworkDelegate, SIGNAL(frameworksAvailableForApp(GrabApplicationQueueItem)), this, SLOT(__checkFrameworksSucceededForApp(GrabApplicationQueueItem)));
    connect(_appFrameworkDelegate, SIGNAL(frameworksCannotBeInstalledForApp(GrabApplicationQueueItem,QString)), this, SLOT(__checkFrameworksFailedForApp(GrabApplicationQueueItem,QString)));
    // fw install allow/deny
    connect(_appFrameworkDelegate, SIGNAL(__frameworksRequireInstallForApp(QString,QString)), this, SIGNAL(__frameworksRequireInstallForApp(QString,QString)));

    // -- app install delegate
    _appInstallDelegate = new AppInstallDelegate(this);
    connect(this, SIGNAL(__installThisApp(GrabApplicationQueueItem)), _appInstallDelegate, SLOT(installThisApp(GrabApplicationQueueItem)));
    connect(_appInstallDelegate, SIGNAL(__resolutionNeededForRunningApp(QString,QString)), this, SIGNAL(__resolutionNeededForRunningApp(QString,QString)));
    connect(_appInstallDelegate, SIGNAL(appInstalledSuccessfullyToDirectory(GrabApplicationQueueItem,QString)), this, SLOT(__installSucceededToDirectory(GrabApplicationQueueItem,QString)));
    connect(_appInstallDelegate, SIGNAL(appInstallProgressStateMessage(GrabApplicationQueueItem,QString)), this, SLOT(__installProgressStateMessage(GrabApplicationQueueItem,QString)));
    connect(_appInstallDelegate, SIGNAL(installFailed(GrabApplicationQueueItem,QString)), this, SLOT(__installFailed(GrabApplicationQueueItem,QString)));
}

// ------------------------
// --- public interface ---

void GrabApplicationManager::grabApplication(const QString& applicationID, QString appName, QString appSupportedSummedPlatformFlags, bool isUpdate, AppWhirr::ApplicationStatusInStore::ApplicationStatusInStoreEnum appStatusInStore)
{
    GrabApplicationQueueItem grabItem(applicationID, appName, appSupportedSummedPlatformFlags, isUpdate, appStatusInStore);
    Q_EMIT __grabThisItem(grabItem);
}

// ------------------------------
// --- internal grab handlers ---

void GrabApplicationManager::__grabStarted(GrabApplicationQueueItem grabItem)
{
    DLog("Grab started slot: ") << grabItem.getAppId() << grabItem.getAppName();
    Q_EMIT appGrabStarted(grabItem.getAppId(), grabItem.getAppName());
    Q_EMIT appInstallationStarted(grabItem.getAppId(), grabItem.getAppName());
}

void GrabApplicationManager::__grabSucceeded(GrabApplicationQueueItem grabItem, QUrl appDownloadUrl)
{
    DLog("Grab succeeded slot: ") << grabItem.getAppId() << grabItem.getAppName() << appDownloadUrl;
    Q_EMIT appGrabbedSuccessfully(grabItem.getAppId(), grabItem.getAppName(), grabItem.getSupportedSummedPlatformFlags(), grabItem.getRelatedAppVersion(), grabItem.getAppStatusInStore());

    grabItem._setRelatedDownloadUrl(appDownloadUrl);

    Q_EMIT appInstallationStarted(grabItem.getAppId(), grabItem.getAppName());

//    Q_EMIT __downloadThisItem(grabItem);
    Q_EMIT __checkFrameworksForThisApp(grabItem);
}

void GrabApplicationManager::__grabFailed(GrabApplicationQueueItem grabItem, QString errorMessage)
{
    WLog("Grab failed slot: ") << grabItem.getAppId() << grabItem.getAppName() << errorMessage;
    Q_EMIT appInstallFailed(grabItem.getAppId(), grabItem.getAppName(), grabItem.getIsUpdate(), errorMessage);
}


// ------------------------------------------------
// --- internal app framework checking handlers ---

void GrabApplicationManager::__checkFrameworksSucceededForApp(GrabApplicationQueueItem grabItem)
{
    DLog("Check frameworks for app - succeeded: ") << grabItem.getAppId() << grabItem.getAppName();



//    Q_EMIT __installThisApp(grabItem);

    // ---
    QString savedApplicationZip = grabItem.getAppId() + ".zip";

    if(savedApplicationZip.isEmpty()) {
        WLog("AppName is empty");
        Q_EMIT appInstallFailed(grabItem.getAppId(), grabItem.getAppName(), grabItem.getIsUpdate(), tr("The downloaded app is invalid. Oh my..."));
//        Q_EMIT appDownloadFailed(grabItem, tr("The downloaded app is invalid. Oh my..."));
        return;
    }

    // Create file
    QString tempZipFileContainingDir = AppSpecificPathHelper::getLocalAppWhirrTmpDirectory();
    QString tempZipFilePath = PathHelper::combineAndCleanPathes(tempZipFileContainingDir, savedApplicationZip);
    grabItem._setRelatedDownloadedZip(tempZipFilePath);
    // ---

    Q_EMIT __downloadThisItem(grabItem);
}

void GrabApplicationManager::__checkFrameworksFailedForApp(GrabApplicationQueueItem grabItem, QString errorMessage)
{
    DLog("Check frameworks for app - failed: ") << grabItem.getAppId() << grabItem.getAppName();

    Q_EMIT appInstallFailed(grabItem.getAppId(), grabItem.getAppName(), grabItem.getIsUpdate(), tr("Cannot install required frameworks. Error: %1").arg(errorMessage));
}

void GrabApplicationManager::__allowFrameworkInstallForApp(QString appId)
{
    _appFrameworkDelegate->__allowFrameworkInstallForApp(appId);
}

void GrabApplicationManager::__denyFrameworkInstallForApp(QString appId)
{
    _appFrameworkDelegate->__denyFrameworkInstallForApp(appId);
}


// --------------------------------------
// --- internal app download handlers ---

void GrabApplicationManager::__downloadStarted(GrabApplicationQueueItem grabItem)
{
    DLog("Download started slot: ") << grabItem.getAppId() << grabItem.getAppName();
}

void GrabApplicationManager::__downloadProgress(GrabApplicationQueueItem grabItem, int percent)
{
//    DLog("Download progress slot: ") << grabItem.getAppId() << grabItem.getAppName() << percent;
    Q_EMIT appDownloadProgress(grabItem.getAppId(), percent);
}

void GrabApplicationManager::__downloadFinished(GrabApplicationQueueItem grabItem)
{
    DLog("Download finished slot: ") << grabItem.getAppId() << grabItem.getAppName();



//   //  start checking the app's frameworks
//    Q_EMIT __checkFrameworksForThisApp(grabItem);
    Q_EMIT __installThisApp(grabItem);


//    Q_EMIT appDownloadFinished(grabItem);

    //    this->addItemToInstallQueue(grabItem);
    //    Q_EMIT _tryToStartNextInstall();
}

void GrabApplicationManager::__downloadFailed(GrabApplicationQueueItem grabItem, QString errorMessage)
{
    DLog("Download failed slot: ") << grabItem.getAppId() << grabItem.getAppName() << errorMessage;
    Q_EMIT appInstallFailed(grabItem.getAppId(), grabItem.getAppName(), grabItem.getIsUpdate(), tr("Download failed with error: %1").arg(errorMessage));
}


// -------------------------------------
// --- internal app install handlers ---

void GrabApplicationManager::__installProgressStateMessage(GrabApplicationQueueItem grabItem, QString progressStateMessage)
{
    Q_EMIT appInstallProgressStateMessage(grabItem.getAppId(), progressStateMessage);
}

void GrabApplicationManager::__installSucceededToDirectory(GrabApplicationQueueItem grabItem, QString relatedAppInstalledRootPath)
{
    DLog("Install succeeded slot: ") << grabItem.getAppId() << grabItem.getAppName() << relatedAppInstalledRootPath;
    Q_EMIT appInstalledSuccessfullyToDirectory(grabItem.getAppId(), grabItem.getAppName(), relatedAppInstalledRootPath, grabItem.getRelatedAppVersion());
}

void GrabApplicationManager::__installFailed(GrabApplicationQueueItem grabItem, QString errorMessage)
{
    DLog("Install failed slot: ") << grabItem.getAppId() << grabItem.getAppName() << errorMessage;
    Q_EMIT appInstallFailed(grabItem.getAppId(), grabItem.getAppName(), grabItem.getIsUpdate(), errorMessage);
}



void GrabApplicationManager::__resolutionDoneRetryInstall(QString appId)
{
    _appInstallDelegate->__resolutionDoneRetryInstall(appId);
}

void GrabApplicationManager::__resolutionFailed(QString appId)
{
    _appInstallDelegate->__resolutionFailed(appId);
}


//void GrabApplicationManager::__resolutionDoneRetryInstall(QString appId)
//{
//    if(!this->_installWaitsForResolutionMap.contains(appId)) {
//        WLog("Cannot find the given app-id in the resolution-needed map.");
//        return;
//    }

//    // put back to install queue
//    GrabApplicationQueueItem grabItem = this->_installWaitsForResolutionMap.value(appId);
//    this->addItemToInstallQueue(grabItem);
//    this->_installWaitsForResolutionMap.remove(appId);
//    Q_EMIT _tryToStartNextInstall();
//}

//void GrabApplicationManager::__resolutionFailed(QString appId)
//{
//    if(!this->_installWaitsForResolutionMap.contains(appId)) {
//        WLog("Cannot find the given app-id in the resolution-needed map.");
//        return;
//    }

//    GrabApplicationQueueItem grabItem = this->_installWaitsForResolutionMap.value(appId);
//    Q_EMIT appInstallFailed(grabItem.getAppId(), grabItem.getAppName(), grabItem.getIsUpdate(), tr("Cannot close the running application."));
//    this->_installWaitsForResolutionMap.remove(appId);
//}

//void GrabApplicationManager::authenticationChallengeCancelled() {
//    GrabApplicationQueueItem item;
//    Q_FOREACH(item, _grabItemsWaitingForDownload) {
//        Q_EMIT this->appGrabFailed(item.getAppId(), item.getAppName(), item.getIsUpdate(), "Authentication error");
//    }

//    _grabItemsWaitingForDownload.clear();
//    _applicationZipsWaitingForInstallation.clear();
//    AW_QOBJECT_SAFE_DELETE(_appGrabCommunicationDescriptionGateway);
//    AW_QOBJECT_SAFE_DELETE(_appDownloadCommunicationResourceGateway);
//}
