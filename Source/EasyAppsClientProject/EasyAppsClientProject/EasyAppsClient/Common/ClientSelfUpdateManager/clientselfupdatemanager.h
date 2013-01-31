#ifndef CLIENTSELFUPDATEMANAGER_H
#define CLIENTSELFUPDATEMANAGER_H

#include <QObject>

#include <QUrl>

QT_BEGIN_NAMESPACE
class QTimer;
QT_END_NAMESPACE

class CommunicationDescriptionGateway;

namespace AW {
namespace Common {
namespace Communication {
class BaseNetworkEntity;
class ResourceDownloadStreamToFile;
}
}
}

/*!

    0) check for new version
    1) download the new version
    3) unzip the downloaded new version - this will be the one performing the self-update
    4) and copy it to a different folder, which will be copied as the new version
    5) install with a temporary copy of the new version
    5.1) start the AppWhirrU (updater) and close the current running client
    5.2) backup then remove the current version
    5.3) start the new version of the client
    6) remove temporary data

    By default client-self-update-manager won't check periodically but you can start to do it.
*/
class ClientSelfUpdateManager : public QObject
{
    Q_OBJECT
public:
    explicit ClientSelfUpdateManager(QObject *parent = 0);

    bool clearRelatedTmpDirectories();

    bool getIsInSelfUpdatingProcess();

    // ---------------------
    // --- version check ---

public Q_SLOTS:
    //
    // starts to check whether a new client version is available
    void startCheckIsNewVersionAvailableImmediately();

public:
    //
    // Starts to check for new versions periodically. Will emit the same signals when check done.
    void startCheckIsNewVersionAvailablePeriodically();

    // ----------------
    // --- download ---
    void startToDownloadNewAvailableVersion();

    // ---------------
    // --- install ---
    void startToPrepareInstallDownloadedNewVersion();
private:
    void uncompressDownloadedClient();

public:
    //
    // returns false if it cannot start the updater
    bool startUpdaterInClientUpdateMode();

Q_SIGNALS:
    // ---------------------
    // --- version check ---
    void newVersionFound();
    void noNewVersionFound();
    void failedToGetNewVersion(QString error);

    // ----------------
    // --- download ---
    void downloadFinished();
    void downloadProgress(int progressPercent);
    void downloadFailedWithError(QString errorMessage);

    // ---------------
    // --- install ---

    //
    // this signal will be emitted when everything is ready to close the client and start the installing / copying of the new version
    void readyToQuitAndPerformInstall();
    //
    //
    void installFailedWithError(QString errorMessage);

private Q_SLOTS:

    // ---------------------
    // --- version check ---
    void _failedToGetVersionInfoWithError(const QString& error);
    void _finishedGettingVersionInfo(const QList<AW::Common::Communication::BaseNetworkEntity*>& entities);
    void _versionInfoAuthenticationChallangeHappened();

    // ----------------
    // --- download ---

    // zip info
    void _failedToGetUpdateZipInfoWithError(const QString& error);
    void _finishedGettingUpdateZipInfo(const QList<AW::Common::Communication::BaseNetworkEntity*>& entities);
    void _updateZipInfoAuthenticationChallangeHappened();

    // zip download
    void _newVersionZipDownloadProgressFinished();
    void _newVersionZipDownloaded(QUrl url, QString localFilePath);
    void _newVersionZipDownloadFailed(QUrl url, QString localFilePath, QString errorMessage);
    void _newVersionZipDownloadProgress(int progress, QUrl url);

    // ---------------
    // --- install ---
    void _uncompressorFinishedSuccessfully();
    void _uncompressorFailedWithError(QString errorMessage);

    //
    void __clientSelfUpdatingProcessEnded();
    //
    // this is just a convinience method to be able to connect it to signals have an errorMessage
    void __clientSelfUpdatingProcessEnded(QString dontUse);

private:
    void _startDownloadingTheFoundNewVersionZip();

private:
    CommunicationDescriptionGateway *_clientVersionInfoCheckerCommunicationGateway;
    CommunicationDescriptionGateway *_clientZipUrlGetterCommunicationGateway;
    AW::Common::Communication::ResourceDownloadStreamToFile *_clientNewVersionZipDownloader;
//    CommunicationResourceGateway *_clientNewVersionZipDownloaderResourceGateway;

    QUrl _clientNewVersionZipUrl;
    QTimer *_clientSelfUpdateCheckTimer;

    bool _isInSelfUpdatingProcess;
};

#endif // CLIENTSELFUPDATEMANAGER_H
