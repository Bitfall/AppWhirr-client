#ifndef DELAYEDRETRYHELPER_H
#define DELAYEDRETRYHELPER_H

#include <QObject>

QT_BEGIN_NAMESPACE
class QTimer;
QT_END_NAMESPACE

class DelayedRetryHelper : public QObject
{
    Q_OBJECT
public:
    explicit DelayedRetryHelper(int maxRetryCount, int retryDelayMilliseconds, QObject *parent = 0);

    void resetRetryCount();

Q_SIGNALS:
    void retryNow();
    void noMoreRetry();

public Q_SLOTS:
    void retryWithDelay();

private:
    QTimer *_retryDelayTimer;
    int _retryCountLeft;
    const int _maxRetryCount;
};

#endif // DELAYEDRETRYHELPER_H
