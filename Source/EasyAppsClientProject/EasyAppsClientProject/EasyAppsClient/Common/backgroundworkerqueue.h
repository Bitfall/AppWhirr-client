#ifndef AW_COMMON_UTILS_BACKGROUNDWORKERQUEUE_H
#define AW_COMMON_UTILS_BACKGROUNDWORKERQUEUE_H

#include <QObject>
#include <QQueue>
#include <QMutex>

namespace AW {
namespace Common {
namespace Utils {


class RunnableItem;

class BackgroundWorkerQueue : public QObject
{
    Q_OBJECT
public:
    explicit BackgroundWorkerQueue(QObject *parent = 0);

Q_SIGNALS:
    void queueEmpty();

public Q_SLOTS:
    void addItemToQueue(RunnableItem *runnableItem);

private Q_SLOTS:
    void _tryNextItemInQueue();
    void _currentItemFinishedWithWork();

private:
    void _setIsRunnableActive(bool value);
    bool _getIsRunnableActive();

private:
    QQueue<RunnableItem *> _runnableQueue;
    QMutex _queueMutex;
    bool _isRunnableActive;
    QMutex _activityMutex;
};


} // namespace Utils
} // namespace Common
} // namespace AW

#endif // AW_COMMON_UTILS_BACKGROUNDWORKERQUEUE_H
