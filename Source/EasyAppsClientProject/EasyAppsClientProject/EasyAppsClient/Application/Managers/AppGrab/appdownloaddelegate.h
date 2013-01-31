#ifndef APPDOWNLOADDELEGATE_H
#define APPDOWNLOADDELEGATE_H

#include <QObject>
#include <QMap>
#include <QUrl>

#include "grabapplicationqueueitem.h"

class CommunicationResourceGateway;

namespace AW {
namespace Common {
namespace Communication {
class ResourceDownloadStreamToFile;
}
}
}

/*! Delegate to handle app downloading.

    It's designed to be an internal delegate of the \a GrabApplicationManager.
*/
class AppDownloadDelegate : public QObject
{
    Q_OBJECT
public:
    explicit AppDownloadDelegate(QObject *parent = 0);
    ~AppDownloadDelegate();

    // ------------------------
    // --- public interface ---
public Q_SLOTS:
    void startDownloadingApp(GrabApplicationQueueItem grabItem);

Q_SIGNALS:
    void appDownloadStarted(GrabApplicationQueueItem grabItem);
    void appDownloadProgress(GrabApplicationQueueItem grabItem, int percent);
    void appDownloadFinished(GrabApplicationQueueItem grabItem);
    void appDownloadFailed(GrabApplicationQueueItem grabItem, QString errorMessage);

    // ----------------
    // --- internal ---
private Q_SLOTS:
    void tryToStartDownloadingApplication(QUrl zippedApplicationURL);
//    void __appDownloadRetrievedResourceForURL(QUrl url, QByteArray resource);
    void __appDownloadResourceSuccess(QUrl url, QString localFilePath);
    void __downloadProgressForUrl(int percent, QUrl url);
    void __allAppDownloadResourceRequestsFinished();
    void __appDownloadFailed(QUrl url, QString localFilePath, QString errorMessage);

    void _appZipSavedToFile(QString filePath);
    void _appZipSaveToFileFailed(QString filePath);

private:
    QUrl _getUrlByZipPath(QString zipPath);

private:
//    CommunicationResourceGateway *_appDownloadCommunicationResourceGateway;
    AW::Common::Communication::ResourceDownloadStreamToFile *_resourceDownloader;
    QMap<QUrl, GrabApplicationQueueItem> _downloadingResourceMapper;
};

#endif // APPDOWNLOADDELEGATE_H
