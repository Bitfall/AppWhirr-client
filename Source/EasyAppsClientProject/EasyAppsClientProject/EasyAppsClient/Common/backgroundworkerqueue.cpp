#include "backgroundworkerqueue.h"

#include <QThreadPool>

#include "Common/runnableitem.h"


namespace AW {
namespace Common {
namespace Utils {


BackgroundWorkerQueue::BackgroundWorkerQueue(QObject *parent) :
    QObject(parent),
    _isRunnableActive(false)
{
}

void BackgroundWorkerQueue::addItemToQueue(RunnableItem *runnableItem)
{
    _queueMutex.lock();
    _runnableQueue.enqueue(runnableItem);
    _queueMutex.unlock();

    this->_tryNextItemInQueue();
}

void BackgroundWorkerQueue::_tryNextItemInQueue()
{
//    DLog("Try next item");
    if( this->_getIsRunnableActive() ) {
        return;
    }

    _queueMutex.lock();
    RunnableItem *currRunnable = NULL;
    if( !_runnableQueue.isEmpty() ) {
         currRunnable = _runnableQueue.dequeue();
    }
    _queueMutex.unlock();

    if(currRunnable == NULL) {
        // no more items
//        DLog("No more items");
        Q_EMIT queueEmpty();
        return;
    }

    connect(currRunnable, SIGNAL(finishedWithWork()), this, SLOT(_currentItemFinishedWithWork()));
    QThreadPool::globalInstance()->start(currRunnable);
//    DLog("Work started");

    this->_setIsRunnableActive(true);
}

void BackgroundWorkerQueue::_currentItemFinishedWithWork()
{
//    DLog("Item finished with work");

    this->_setIsRunnableActive(false);

    this->_tryNextItemInQueue();
}

void BackgroundWorkerQueue::_setIsRunnableActive(bool value)
{
    QMutexLocker locker(&_activityMutex);
    _isRunnableActive = value;
}

bool BackgroundWorkerQueue::_getIsRunnableActive()
{
    QMutexLocker locker(&_activityMutex);
    return _isRunnableActive;
}

} // namespace Utils
} // namespace Common
} // namespace AW
