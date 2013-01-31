#include "delayedretryhelper.h"

#include <QTimer>

DelayedRetryHelper::DelayedRetryHelper(int maxRetryCount, int retryDelayMilliseconds, QObject *parent) :
    QObject(parent),
    _retryCountLeft(maxRetryCount),
    _maxRetryCount(maxRetryCount)
{
    _retryDelayTimer = new QTimer(this);
    _retryDelayTimer->setInterval(retryDelayMilliseconds);
    _retryDelayTimer->setSingleShot(true);
    connect(_retryDelayTimer, SIGNAL(timeout()), this, SIGNAL(retryNow()));
    _retryDelayTimer->stop();
}

void DelayedRetryHelper::retryWithDelay()
{
    this->_retryCountLeft--;
    if(this->_retryCountLeft >= 0) {
        // retry with delay
        this->_retryDelayTimer->start();
    }
    else {
        // no more retry
        Q_EMIT noMoreRetry();
    }
}

void DelayedRetryHelper::resetRetryCount()
{
    this->_retryCountLeft = this->_maxRetryCount;
}
