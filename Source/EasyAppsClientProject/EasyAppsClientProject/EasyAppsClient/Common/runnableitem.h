#ifndef APPWHIRR_RUNNABLE_RUNNABLEITEM_H
#define APPWHIRR_RUNNABLE_RUNNABLEITEM_H

#include <QObject>
#include <QRunnable>

namespace AW {
namespace Common {
namespace Utils {

class RunnableItem : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit RunnableItem(QObject *parent = 0) : QObject(parent) {}

    virtual void run() = 0;

Q_SIGNALS:
    void finishedWithWork();
};

} // namespace Utils
} // namespace Common
} // namespace AW

#endif // APPWHIRR_RUNNABLE_RUNNABLEITEM_H

