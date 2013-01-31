#ifndef BACKGROUND_CLIENTUNCOMPRESSOR_H
#define BACKGROUND_CLIENTUNCOMPRESSOR_H

#include <QObject>
#include <QRunnable>

class Background_ClientUncompressor : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit Background_ClientUncompressor(QObject *parent = 0);

    void run();

Q_SIGNALS:
    void finishedSuccessfully();
    void failedWithError(QString errorMessage);
};

#endif // BACKGROUND_CLIENTUNCOMPRESSOR_H
