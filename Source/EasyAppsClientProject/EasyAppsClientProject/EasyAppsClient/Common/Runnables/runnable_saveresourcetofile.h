#ifndef RUNNABLE_SAVERESOURCETOFILE_H
#define RUNNABLE_SAVERESOURCETOFILE_H

#include <QObject>
#include <QRunnable>

class Runnable_SaveResourceToFile : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit Runnable_SaveResourceToFile(QString resourceTargetFilePath, QByteArray resourceContent, QObject *parent = 0);

    void run();
    
Q_SIGNALS:
    void finishedWithSuccess(QString filePath);
    void finishedButFailed(QString filePath);

private:
    QByteArray _resourceContent;
    QString _resourceTargetFilePath;
};

#endif // RUNNABLE_SAVERESOURCETOFILE_H
