#ifndef APPLICATIONRUNNER_H
#define APPLICATIONRUNNER_H

#include <QObject>
#include <QString>
#include <QTimer>
#include <QQueue>
#include <QStringList>

class ApplicationRunner : public QObject
{
    Q_OBJECT

private:
    class ApplicationRunnerQueueItem {
    public:
        ApplicationRunnerQueueItem(QString absoluteExecutablePath, QString absoluteWorkingDirPath, QString appName) :
            _absoluteExecutablePath(absoluteExecutablePath), _absoluteWorkingDirPath(absoluteWorkingDirPath), _appName(appName)
        {}

        QString getAbsoluteExecutablePath() const { return this->_absoluteExecutablePath; }
        QString getAbsoluteExecutableWorkingDirectoryPath() const { return this->_absoluteWorkingDirPath; }
        QString getAppName() const { return this->_appName; }

    private:
        QString _absoluteExecutablePath;
        QString _absoluteWorkingDirPath;
        QString _appName;
    };

public:
    // if isQueueMode = true then you have to use the addApplicationToRunQueue to queue apps
    //  and call runApplicationsInQueueWithDelay to start to run them
    explicit ApplicationRunner(bool isQueueMode, QObject *parent = 0);

    //
    // static method to start a single application
    //  ! if you want to start a bunch of applications (even if you want to start 2) then don't use this method
    //      instead create an object from this class and call addApplicationToRunQueue() and runApplicationsInQueueWithDelay() and delete the object when it emits finishedWithQueue()
    void runSingleApplication(QString absoluteExecutablePath, QString absoluteWorkingDirPath, QString appName, QStringList appArgs = QStringList());

    void addApplicationToRunQueue(QString absoluteExecutablePath, QString absoluteWorkingDirPath, QString appName);

    bool isRunQueueEmpty() const;
    int getAppCountInQueue() const;

    bool getIsQueueMode() const;

Q_SIGNALS:
    // batch/queue run operation signals
    //  -> the simple run operations will be also called!
    // will be called even if every app-run fails - this just marks the end of a queue running
    void finishedWithQueueAndSafeToDelete(ApplicationRunner *source);
    // run operation signals
    void successWithApp(QString appName, ApplicationRunner *source);
    void failedWithApp(QString appName, QString errorMessage, ApplicationRunner *source);

public Q_SLOTS:
    void runApplicationsInQueueWithDelay();
    void clearApplicationsFromQueue();

private Q_SLOTS:
    void _tryToRunTheNextItemInTheQueue();

    void _successWithApp(QString appName);
    void _failedWithApp(QString appName, QString errorMessage);

private:
    QTimer *_delayStarterTimer;
    QQueue<ApplicationRunnerQueueItem> _queueItems;
    int _startedQueueItemCnt;
    const bool _isQueueMode;
};

#endif // APPLICATIONRUNNER_H
