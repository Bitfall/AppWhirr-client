#ifndef LOGGERSERVER_H
#define LOGGERSERVER_H

#include <QObject>

QT_BEGIN_NAMESPACE
class QFile;
class QThread;
QT_END_NAMESPACE

class LoggerServer : public QObject
{
    Q_OBJECT
public:
    explicit LoggerServer(QObject *parent = 0);
    ~LoggerServer();
    
    /*! Installs and initializes a log handler to handle logging the Qt style

        After this you can simply qDebug() or qError() or use any other Qt logger method and it will be handled by the LoggerServer.

        This will also open the specified log-file for logging.

        @return False if initialization fails (ex: cannot open the log-file). True otherwise.
    */
    bool installAndInitializeQtLogHandler(QString logFilePath);

    //
    // internal interface, don't use these outside of this class
Q_SIGNALS:
    void __logThis(QString msg);

private:
    QFile *_outputFile;
    QThread *_serverThread;
};

#endif // LOGGERSERVER_H
