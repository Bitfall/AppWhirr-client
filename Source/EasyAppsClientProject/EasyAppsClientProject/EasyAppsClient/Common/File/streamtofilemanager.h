#ifndef APPWHIRR_STREAMTOFILEMANAGER_H
#define APPWHIRR_STREAMTOFILEMANAGER_H

#include <QObject>
#include <QMutex>

QT_BEGIN_NAMESPACE
class QFile;
QT_END_NAMESPACE


namespace AW {
namespace Common {
namespace Utils {

class BufferedTransmitter;
class BackgroundWorkerQueue;


class StreamToFileManager : public QObject
{
    Q_OBJECT
public:
    explicit StreamToFileManager(QObject *parent = 0);

    bool startStreamingToFile(QString targetFilePath, bool isRemoveTargetFileIfExists);
    QString getTargetFilePath() const;

Q_SIGNALS:
//    void dataChunkWrittenToFile();
//    void dataChunkWriteFailedWithError(QString errorMessage);

    void streamingFinishedWithSuccess(QString filePath);
    void streamingFinishedWithError(QString filePath, QString errorMessage);

public Q_SLOTS:
    void writeDataToFile(QByteArray data);
    void closeStreaming(); //

    void abortStreaming(); // won't accept new data after this call

private Q_SLOTS:
    void _bufferTransmitterFlush(QByteArray bufferedData);
    void _chunkWrittenToFile();
    void _chunkWriteToFileFailedWithError(QString errorMessage);

    void _executorQueueEmtpy();

    // ------------------------------------------------------
    // --- private getters and setters
private:
    void _setIsStreamingAborted(bool isAborted);
    bool _getIsStreamingAborted();

    void _setIsStreamingClosing(bool isClosing);
    bool _getIsStreamingClosing();
    
private:
    QString _targetFilePath;
    BufferedTransmitter *_bufferedTransmitter;
    BackgroundWorkerQueue *_backgroundWorkerQueue;

    int _chunkCounter;
    QFile *_targetFile;

    QMutex _abortMutex;
    bool _isStreamingAborted;

    QMutex _streamClosingMutex;
    bool _isStreamClosing;
};

} // namespace Utils
} // namespace Common
} // namespace AW

#endif // APPWHIRR_STREAMTOFILEMANAGER_H
