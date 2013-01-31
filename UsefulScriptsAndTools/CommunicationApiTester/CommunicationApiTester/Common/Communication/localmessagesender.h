#ifndef LOCALMESSAGESENDER_H
#define LOCALMESSAGESENDER_H

#include <QObject>

QT_BEGIN_NAMESPACE
class QString;
class QByteArray;
QT_END_NAMESPACE

class LocalMessageSender : public QObject
{
    Q_OBJECT
public:
    explicit LocalMessageSender(QString uniqueKey, unsigned int timeout = 3000);
//    ~LocalMessageSender();

    /*! Sends a message to the server */
    /** Creates a connection to the server specified in the uniqueKey attribute,
        then sends the message specified.
        Also the finished Q_SIGNAL will be emitted if the sending did succeeded or failed.
        @param message Message to be sent
       */
    void sendSingleMessage(const QByteArray& message);

Q_SIGNALS:
    void finishedWithSuccess();
    void failed(QString errorMessage);

private:
    QString         uniqueKey;  //!< Identicates the server
    unsigned int    timeout;    //!< Stops sending after this interval (milliseconds)

};

#endif // LOCALMESSAGESENDER_H
