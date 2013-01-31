#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

QT_BEGIN_NAMESPACE
class QLineEdit;
class QTextEdit;
QT_END_NAMESPACE

class ApiTestNetworkRequest;
class CommunicationDescriptionGateway;
class BaseNetworkEntity;
class SimpleSslClient;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private Q_SLOTS:
    void _send();

    void _authenticationChallenge();
    void _failedToSend();
    void _sendFailedWithError(QString errorMessage);
    void _sendFinished();
    void _sendTimeouted();
    void _sendResponseDownloaded();

    void _response(QList<BaseNetworkEntity*> responseEntities);
    void _plainResponseDataReceived(QByteArray responseData);

    void _openWebViewWindow();

private:
    QLineEdit *_urlToCallLineEdit;
    QLineEdit *_versionLineEdit;
    QLineEdit *_additionalHeadersLineEdit;
    QLineEdit *_postValuesLineEdit;

    QTextEdit *_responseTextEdit;
    QTextEdit *_testLogTextEdit;

    CommunicationDescriptionGateway *_communicationDescriptionGateway;
    ApiTestNetworkRequest *_currTestRequest;

    // TEST
    SimpleSslClient *_simpleSslClient;
};

#endif // MAINWINDOW_H
