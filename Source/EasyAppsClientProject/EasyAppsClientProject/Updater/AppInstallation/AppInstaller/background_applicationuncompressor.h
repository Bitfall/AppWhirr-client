#ifndef BACKGROUND_APPLICATIONUNCOMPRESSOR_H
#define BACKGROUND_APPLICATIONUNCOMPRESSOR_H

#include <QObject>
#include <QRunnable>

class Background_ApplicationUncompressor : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit Background_ApplicationUncompressor(QString appId, QString zipFileName, QObject *parent=0);

    void run();

Q_SIGNALS:
    void finishedSuccessfully();
    void failedWithError(QString errorMessage);

protected:
    QString _appId;
    QString _zipFileName;
};

#endif // BACKGROUND_APPLICATIONUNCOMPRESSOR_H
