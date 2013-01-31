#ifndef APPWHIRR_RUNNABLE_STREAMTOFILE_H
#define APPWHIRR_RUNNABLE_STREAMTOFILE_H

#include "Common/runnableitem.h"

QT_BEGIN_NAMESPACE
class QFile;
QT_END_NAMESPACE

namespace AW {
namespace Common {
namespace Utils {

class StreamToFile : public RunnableItem
{
    Q_OBJECT
public:
    explicit StreamToFile(QFile *targetFile, QByteArray resourceContent, QObject *parent = 0);

    void run();

Q_SIGNALS:
    void finishedWithSuccess();
    void failedWithError(QString errorMessage);

private:
    QByteArray _resourceContent;
    QFile *_targetFile;
};

} // namespace Utils
} // namespace Common
} // namespace AW

#endif // APPWHIRR_RUNNABLE_STREAMTOFILE_H
