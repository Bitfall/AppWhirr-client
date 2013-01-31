#ifndef APPFRAMEWORKDELEGATE_H
#define APPFRAMEWORKDELEGATE_H

#include <QObject>
#include <QQueue>
#include <QUrl>

#include "grabapplicationqueueitem.h"
#include "Common/Framework/frameworkcheckmodel.h"

class CommunicationDescriptionGateway;
//class CommunicationResourceGateway;

class AvailableFrameworkChecker;
class FrameworkInstaller;

namespace AW {
namespace Common {
namespace Communication {
class BaseNetworkEntity;
class ResourceDownloadStreamToFile;
}
}
}



/*! Delegate to handle app framework checking, downloading and installing.

    It's designed to be an internal delegate of the \a GrabApplicationManager.

    @note Steps:\n
    1) Get framework checker(s) for the app's required framework(s).\n
    1.1) If the checker is already downloaded (and that's the latest version of the checker) use the 'cached' checker. If not yet downloaded download and store it.\n
    2) Check the required frameworks.\n
    3) Download required installers for frameworks not yet installed.\n
    3.1) Install them.\n
    3.2) Check the installed frameworks again to test whether installed correctly or not.\n
*/
class AppFrameworkDelegate : public QObject
{
    Q_OBJECT

    enum FrameworkCheckTypeEnum {
        FCT_None = 0,
        FCT_InitialChecking = 1,
        FCT_AfterInstallReChecking = 2
    };

private:
    class FrameworkCheckInternalModel {
    public:
        explicit FrameworkCheckInternalModel() {}
        explicit FrameworkCheckInternalModel(FrameworkCheckModel frameworkCheckModel, QUrl frameworkDownloadUrl, QUrl frameworkCheckerDownloadUrl) :
            _frameworkCheckModel(frameworkCheckModel), _frameworkDownloadUrl(frameworkDownloadUrl), _frameworkCheckerDownloadUrl(frameworkCheckerDownloadUrl)
        {}

        FrameworkCheckModel getFrameworkCheckModel() const { return _frameworkCheckModel; }
        QUrl getFrameworkDownloadUrl() const { return _frameworkDownloadUrl; }
        QUrl getFrameworkCheckerDownloadUrl() const { return _frameworkCheckerDownloadUrl; }

        FrameworkCheckModel &getFrameworkCheckModel_AllowModify() { return _frameworkCheckModel; }

        void _setFrameworkDownloadUrl(QUrl value) { _frameworkDownloadUrl = value; }

    private:
        FrameworkCheckModel _frameworkCheckModel;
        QUrl _frameworkDownloadUrl;
        QUrl _frameworkCheckerDownloadUrl;
    };

public:
    explicit AppFrameworkDelegate(QObject *parent = 0);
    ~AppFrameworkDelegate();
    
    // ------------------------
    // --- public interface ---
public Q_SLOTS:
    void checkFrameworksForApp(GrabApplicationQueueItem grabItem);
Q_SIGNALS:
    void frameworkCheckingStartedForApp(GrabApplicationQueueItem grabItem);
    void frameworksAvailableForApp(GrabApplicationQueueItem grabItem);
    void frameworksCannotBeInstalledForApp(GrabApplicationQueueItem grabItem, QString errorMessage);

    // allow/deny fw install
    /*! This signal will be emitted if framework installation is required for an app.

        This will \b suspend \b every checking until \a __allowFrameworkInstallForApp() or \a __denyFrameworkInstallForApp() is called.
    */
    void __frameworksRequireInstallForApp(QString appId, QString appName);
public Q_SLOTS:
    void __allowFrameworkInstallForApp(QString appId);
    void __denyFrameworkInstallForApp(QString appId);

    // ----------------
    // --- internal ---
Q_SIGNALS:
    void __tryNextFrameworkCheckInQueue();
private Q_SLOTS:
    void _doTryNextFramworkCheckInQueue();

    // --------------------------
    // --- framework checking ---
private Q_SLOTS:
    void _appFrameworkCheckerDescriptionRequestFinished(QList<AW::Common::Communication::BaseNetworkEntity *> entities);
    void _appFrameworkCheckerFailedToGetDescription(QString errorMessage);
private:
    /*! Call this method to download required, not yet downloaded checkers
    */
    void _downloadRequiredCheckers();
private Q_SLOTS:
//    void _fwCheckerDownloadRetrievedResourceForURL(QUrl url, QByteArray checkerResource);
    void _fwCheckerDownloadFinished(QUrl url, QString localFilePath);
    void _fwCheckerDownloadProgressForUrl(int percent, QUrl url);
    void _fwCheckerAllDownloadResourceRequestsFinished();
    void _fwCheckerDownloadFailed(QUrl url, QString localFilePath, QString errorMessage);

    /*! After every required framework is downloaded and available call this method to start the checking.
    */
    void _startFrameworkChecking();
private Q_SLOTS:
    void _fwCheckerFrameworksAvailableInSystem(QList<FrameworkCheckModel> frameworkModels);
    void _fwCheckerFrameworksNotFoundInSystem(QList<FrameworkCheckModel> frameworkModels);
    void _fwCheckerInSystemCheckingFinished();

    // ------------------------------
    // --- framework installation ---
    void _startFrameworkInstallation();
    // -- fw download
private Q_SLOTS:
    void _appFrameworkInstallerDescriptionRequestFinished(QList<AW::Common::Communication::BaseNetworkEntity *> entities);
    void _appFrameworkInstallerFailedToGetDescription(QString errorMessage);
    void _startFrameworkInstallerDownloading();
    //
    void _fwInstallerDownloadProgressForUrl(int percent, QUrl url);
    void _fwInstallerDownloaded(QUrl url, QString localFilePath);
    void _fwInstallerDownloadFailed(QUrl url, QString localFilePath, QString errorMessage);
    void _fwInstallerAllDownloadResourceRequestsFinished();

    // -- fw install
    void _installFrameworkFromWaitingQueue();
    void _fwInstallerFrameworkInstalled(FrameworkCheckModel frameworkModel);
    void _fwInstallerFrameworkInstallFailed(FrameworkCheckModel frameworkModel, QString errorMessage);


    /*! Re-checks the installed frameworks to ensure the installation was correct
    */
    void _startAfterFrameworkInstallationChecking();


    // ---------------
    // --- utility ---
private:
    bool getIsCheckingForFramework() const;
    void setIsCheckingForFramework(bool value);

    /*! Will emit the related signal(s) and start the next checking.

        @note The current item will be removed from the waiting-queue, you won't be able to access it after this method called!
    */
    void __checkingFinishedForCurrentAppTryNextOne();

    /*! Will emit the related signal(s) and start the next checking.

        @note The current item will be removed from the waiting-queue, you won't be able to access it after this method called!
    */
    void __checkingFailedForCurrentAppTryNextOne(QString errorMessage);

    QString getFrameworkCheckerDatabasePath() const;

    QString _getFrameworkCheckerTmpZipPathById(QString frameworkId);
    QString _getFrameworkInstallerTmpZipPathById(QString frameworkId);

private:
    CommunicationDescriptionGateway *_commDescriptionGateway;
//    CommunicationResourceGateway *_commResourceGateway;
    AW::Common::Communication::ResourceDownloadStreamToFile *_resourceDownloadStreamToFile;
    QQueue<GrabApplicationQueueItem> _appsWaitingForFrameworkCheck;
    bool _isCheckingForFramework;
    // fw checking
    QList<FrameworkCheckInternalModel> _currRequiredDownloadedFrameworks;
    QList<FrameworkCheckInternalModel> _tmp_currRequiredDownloadNeededFrameworks;
    QList<FrameworkCheckInternalModel> _currFrameworksNotFoundInSystem;
    // fw installation
    QList<FrameworkCheckInternalModel> _currFrameworkInstallersWaitingForDownload;
    QList<FrameworkCheckInternalModel> _currFrameworksForAfterInstallReCheck;
    QQueue<FrameworkCheckInternalModel> _currFrameworksWaitingForInstallation;
    bool _isFrameworkReCheckFailed;

    // managers
    AvailableFrameworkChecker *_frameworkCheckManager;
    FrameworkInstaller *_frameworkInstallManager;

    FrameworkCheckTypeEnum _currFrameworkCheckType;
};

#endif // APPFRAMEWORKDELEGATE_H
