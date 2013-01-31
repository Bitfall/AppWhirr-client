#ifndef RUNNABLE_DELETEDIRECTORYRECURSIVELY_H
#define RUNNABLE_DELETEDIRECTORYRECURSIVELY_H

#include <QObject>
#include <QRunnable>

class Runnable_DeleteDirectoryRecursively : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit Runnable_DeleteDirectoryRecursively(QString topDirectory, QObject *parent = 0);

    void run();

Q_SIGNALS:
    void finishedWithSuccess();
    void failedWithError(QString errorMessage);

private:
    QString _topDirectory;
};

#endif // RUNNABLE_DELETEDIRECTORYRECURSIVELY_H
