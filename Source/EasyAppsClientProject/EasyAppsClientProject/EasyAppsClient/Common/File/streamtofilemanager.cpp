#include "streamtofilemanager.h"

#include <QFile>

#include "Common/bufferedtransmitter.h"
#include "Common/File/streamtofile.h"
#include "Common/backgroundworkerqueue.h"

namespace AW {
namespace Common {
namespace Utils {

StreamToFileManager::StreamToFileManager(QObject *parent) :
    QObject(parent),
    _chunkCounter(0),
    _targetFile(NULL),
    _isStreamingAborted(false)
{
    _bufferedTransmitter = new BufferedTransmitter(1024 * 1024 * 2, this); // 2MB
    connect(_bufferedTransmitter, SIGNAL(bufferFlush(QByteArray)), this, SLOT(_bufferTransmitterFlush(QByteArray)));

    _backgroundWorkerQueue = new BackgroundWorkerQueue(this);
    connect(_backgroundWorkerQueue, SIGNAL(queueEmpty()), this, SLOT(_executorQueueEmtpy()));
}

bool StreamToFileManager::startStreamingToFile(QString targetFilePath, bool isRemoveTargetFileIfExists)
{
    if(isRemoveTargetFileIfExists)
    {
        // check whether the target-file exists
        QFile resourceTargetFile(targetFilePath);
        if(resourceTargetFile.exists()) {
            if( !resourceTargetFile.remove() ) {
                WLog("Cannot remove the file:") << targetFilePath;
                _isStreamingAborted = true;
    //            Q_EMIT streamingFinishedWithError(targetFilePath, "File cannot be removed.");
                return false;
            }
        }
    }

    AW_QOBJECT_SAFE_DELETE(_targetFile);
    _targetFile = new QFile(targetFilePath, this);
    if(!_targetFile->open(QIODevice::WriteOnly | QIODevice::Append)) {
        WLog("Cannot open the file: ") << _targetFile->errorString();
        _isStreamingAborted = true;
//        Q_EMIT streamingFinishedWithError(targetFilePath, "File cannot be opened.");
        return false;
    }

    DLog("File opened");
    _isStreamingAborted = false;
    _isStreamClosing = false;
    _targetFilePath = targetFilePath;

    return true;
}

QString StreamToFileManager::getTargetFilePath() const
{
    return _targetFilePath;
}

void StreamToFileManager::writeDataToFile(QByteArray data)
{
    if( this->_getIsStreamingAborted() || this->_getIsStreamingClosing() ) {
        return;
    }

    _bufferedTransmitter->bufferData(data);
}

void StreamToFileManager::closeStreaming()
{
    _isStreamClosing = true;
    _bufferedTransmitter->forceFlushBuffer();
}

void StreamToFileManager::abortStreaming()
{
    if(this->_getIsStreamingAborted()) {
        // already aborted
        return;
    }

    this->_setIsStreamingAborted(true);

    Q_EMIT streamingFinishedWithError(this->_targetFilePath, "Aborted");
}

void StreamToFileManager::_bufferTransmitterFlush(QByteArray bufferedData)
{
    DLog("Buffer flush");
    StreamToFile *worker = new StreamToFile(_targetFile, bufferedData);
    connect(worker, SIGNAL(finishedWithSuccess()), this, SLOT(_chunkWrittenToFile()));
    connect(worker, SIGNAL(failedWithError(QString)), this, SLOT(_chunkWriteToFileFailedWithError(QString)));
//    QThreadPool::globalInstance()->start(worker);
    _backgroundWorkerQueue->addItemToQueue(worker);
}

void StreamToFileManager::_chunkWrittenToFile()
{
//    DLog("Chunk written to file:") << _targetFilePath;
}

void StreamToFileManager::_chunkWriteToFileFailedWithError(QString errorMessage)
{
    WLog("Chunk write to file FAILED:") << _targetFilePath << "error:" << errorMessage;
    this->abortStreaming();
}

void StreamToFileManager::_executorQueueEmtpy()
{
    if( this->_getIsStreamingClosing() && !this->_getIsStreamingAborted() ) {
        _targetFile->close();
        AW_QOBJECT_SAFE_DELETE(_targetFile);
        Q_EMIT streamingFinishedWithSuccess(_targetFilePath);
    }
}

// ------------------------------------------------------
// --- private getters and setters

void StreamToFileManager::_setIsStreamingAborted(bool isAborted)
{
    QMutexLocker locker(&_abortMutex);
    _isStreamingAborted = isAborted;
}

bool StreamToFileManager::_getIsStreamingAborted()
{
    QMutexLocker locker(&_abortMutex);
    return _isStreamingAborted;
}

void StreamToFileManager::_setIsStreamingClosing(bool isClosing)
{
    QMutexLocker locker(&_streamClosingMutex);
    _isStreamClosing = isClosing;
}

bool StreamToFileManager::_getIsStreamingClosing()
{
    QMutexLocker locker(&_streamClosingMutex);
    return _isStreamClosing;
}

} // namespace Utils
} // namespace Common
} // namespace AW
