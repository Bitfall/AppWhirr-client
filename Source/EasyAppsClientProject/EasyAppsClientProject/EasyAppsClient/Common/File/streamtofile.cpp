#include "streamtofile.h"

#include <QFile>

namespace AW {
namespace Common {
namespace Utils {

StreamToFile::StreamToFile(QFile *targetFile, QByteArray resourceContent, QObject *parent) :
    RunnableItem(parent),
    _resourceContent(resourceContent),
    _targetFile(targetFile)
{

}

void StreamToFile::run()
{
    bool isFileError = false;

    // target-file is defined, use it
    if(_targetFile->write(_resourceContent) < 0) {
        isFileError = true;
        WLog("Write error: ") << _targetFile->errorString();
    }

    if(isFileError) {
        Q_EMIT failedWithError(QString("File open or write error"));
    }

    Q_EMIT finishedWithSuccess();

    Q_EMIT finishedWithWork();
}

} // namespace Utils
} // namespace Common
} // namespace AW
