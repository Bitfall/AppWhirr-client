#ifndef RUNNABLE_CHECKFORRUNNINGEXECUTABLE_H
#define RUNNABLE_CHECKFORRUNNINGEXECUTABLE_H

#include <QObject>
#include <QRunnable>

class PlatformspecificRunningExecutableDataInterface;

//
// it's a recursive search
//  [!!!] IMPORTANT NOTE: the result resultPlatformSpecificRunningAppInfos will contain dynamically allocated items -> it's your responsibility to free them up!!!
class Runnable_CheckForRunningExecutable : public QObject, public QRunnable
{
    Q_OBJECT
public:
    //
    // if target-dir-path does not exists then it will try to create it
    // if isCopyOnlyContentOfSourceDirectory is true then it won't create a directory inside the targetDir called sourceDir, but it will try to copy the content of the sourceDir only
    //      if it's false then it will create the given sourceDir as well inside the targetDir
    explicit Runnable_CheckForRunningExecutable(QString rootDirPath, QObject *parent = 0);

    void run();

Q_SIGNALS:
    //
    //  [!!!] IMPORTANT NOTE: the result resultPlatformSpecificRunningAppInfos will contain dynamically allocated items -> it's your responsibility to free them up!!!
    void finishedWithSuccess(QList<PlatformspecificRunningExecutableDataInterface *> resultPlatformSpecificRunningAppInfos);
//    void failedWithError(QString errorMessage);

private:
    QString _rootDirPath;
};

#endif // RUNNABLE_CHECKFORRUNNINGEXECUTABLE_H
