#include "runnable_saveresourcetofile.h"

#include <QFile>

Runnable_SaveResourceToFile::Runnable_SaveResourceToFile(QString resourceTargetFilePath, QByteArray resourceContent, QObject *parent) :
    QObject(parent),
    _resourceTargetFilePath(resourceTargetFilePath),
    _resourceContent(resourceContent)
{
}

void Runnable_SaveResourceToFile::run()
{
    QFile file(_resourceTargetFilePath);

    bool isFileError = false;
    if(!file.open(QIODevice::WriteOnly)) {
        isFileError = true;
    } else {
        if(file.write(_resourceContent) < 0) {
            isFileError = true;
        }
    }
    file.close();

    if(isFileError) {
        Q_EMIT finishedButFailed(_resourceTargetFilePath);
    }

    Q_EMIT finishedWithSuccess(_resourceTargetFilePath);
}
