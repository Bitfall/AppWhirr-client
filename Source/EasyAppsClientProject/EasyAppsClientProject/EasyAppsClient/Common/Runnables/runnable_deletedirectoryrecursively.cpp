#include "runnable_deletedirectoryrecursively.h"

#include "../Helpers/pathhelper.h"

Runnable_DeleteDirectoryRecursively::Runnable_DeleteDirectoryRecursively(QString topDirectory, QObject *parent) :
    QObject(parent),
    _topDirectory(topDirectory)
{
}

void Runnable_DeleteDirectoryRecursively::run()
{
    if(PathHelper::deleteThisDirectoryAndEverythingBelow(this->_topDirectory)) {
        Q_EMIT finishedWithSuccess();
    }
    else {
        Q_EMIT failedWithError("Cannot delete.");
    }
}
