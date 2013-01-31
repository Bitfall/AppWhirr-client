#include "mainwindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>

#include "Common/Widget/simplewebviewwindow.h"

#include "Application/Communication/Request/apitestnetworkrequest.h"
#include "Application/Communication/Gateway/communicationdescriptiongateway.h"
#include "Application/Communication/Entity/plainresponsenetworkentity.h"

#include "Common/Security/authorization.h"

// TEST
#include "Common/Communication/simplesslclient.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      _communicationDescriptionGateway(NULL),
      _currTestRequest(NULL),
      _simpleSslClient(NULL)
{
    QWidget *mainCentralWidget = new QWidget;
    this->setCentralWidget(mainCentralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainCentralWidget->setLayout(mainLayout);

    QPushButton *openWebViewWindowButton = new QPushButton(tr("Open a WebView window"));
    connect(openWebViewWindowButton, SIGNAL(clicked()), this, SLOT(_openWebViewWindow()));
    mainLayout->addWidget(openWebViewWindowButton);

    QLabel *urlToCallLabel = new QLabel(tr("URL to call"));
    mainLayout->addWidget(urlToCallLabel);
    _urlToCallLineEdit = new QLineEdit;
    mainLayout->addWidget(_urlToCallLineEdit);

    // version
    QHBoxLayout *versionLayout = new QHBoxLayout;
    mainLayout->addLayout(versionLayout);
    QLabel *versionLabel = new QLabel(tr("Version:"));
    versionLayout->addWidget(versionLabel);
    _versionLineEdit = new QLineEdit;
    versionLayout->addWidget(_versionLineEdit);

    // additional headers
    QHBoxLayout *additionalHeadersLayout = new QHBoxLayout;
    mainLayout->addLayout(additionalHeadersLayout);
    QLabel *additionalHeadersLabel = new QLabel(tr("Additional headers:"));
    additionalHeadersLayout->addWidget(additionalHeadersLabel);
    _additionalHeadersLineEdit = new QLineEdit;
    _additionalHeadersLineEdit->setPlaceholderText(tr("value: key, entries should be separated by ';'"));
    additionalHeadersLayout->addWidget(_additionalHeadersLineEdit);

    // post values
    QHBoxLayout *postValuesLayout = new QHBoxLayout;
    mainLayout->addLayout(postValuesLayout);
    QLabel *postValuesLabel = new QLabel(tr("Post values:"));
    postValuesLayout->addWidget(postValuesLabel);
    _postValuesLineEdit = new QLineEdit;
    _postValuesLineEdit->setPlaceholderText(tr("value: key, entries should be separated by ';'"));
    postValuesLayout->addWidget(_postValuesLineEdit);

    // response
    QLabel *responseLabel = new QLabel(tr("Response:"));
    mainLayout->addWidget(responseLabel);
    _responseTextEdit = new QTextEdit;
    mainLayout->addWidget(_responseTextEdit);

    // response
    QLabel *testLogLabel = new QLabel(tr("Log:"));
    mainLayout->addWidget(testLogLabel);
    _testLogTextEdit = new QTextEdit;
    mainLayout->addWidget(_testLogTextEdit);

    // send
    QPushButton *sendButton = new QPushButton(tr("Send"));
    connect(sendButton, SIGNAL(clicked()), this, SLOT(_send()));
    mainLayout->addWidget(sendButton);
}

MainWindow::~MainWindow()
{

}

void MainWindow::_send()
{
    QMap<QString, QString> additionalHeaderKeyValueMap;
    bool useAuthentication = false;
    // Setup security information
    if (!_additionalHeadersLineEdit->text().isEmpty()) {
        QStringList headers = _additionalHeadersLineEdit->text().split(";", QString::SkipEmptyParts);

        QString header;
        Q_FOREACH(header, headers) {
            QStringList values = header.split(":");

            if(values.size() != 2) {
                WLog("Invalid header key-value: ") << header;
                continue;
            }

            QString key = values.at(0).trimmed().toLower();
            QString value = values.at(1).trimmed();

            if (key == "username" ||
                key == "password")
            {
                if (key == "username")
                    Authorization::sharedAuthorization()->setUsername(value);
                if (key == "password")
                    Authorization::sharedAuthorization()->setPassword(value);

                useAuthentication = true;
            }
            else {
                additionalHeaderKeyValueMap[key] = value;
            }
        }
    }

    _communicationDescriptionGateway = new CommunicationDescriptionGateway(this);
    connect(_communicationDescriptionGateway, SIGNAL(authenticationChallengeHappened()), this, SLOT(_authenticationChallenge()));
    connect(_communicationDescriptionGateway, SIGNAL(failedToRetrieveDescription(QString)), this, SLOT(_sendFailedWithError(QString)));
    connect(_communicationDescriptionGateway, SIGNAL(finished(QList<BaseNetworkEntity*>)), this, SLOT(_response(QList<BaseNetworkEntity*>)));
    connect(_communicationDescriptionGateway, SIGNAL(finishedCompletely()), this, SLOT(_sendFinished()));
    connect(_communicationDescriptionGateway, SIGNAL(_receivedResponseData(QByteArray)), this, SLOT(_plainResponseDataReceived(QByteArray)));

    _communicationDescriptionGateway->performThisRequest(new ApiTestNetworkRequest(BaseNetworkRequest::BaseCommunicationRequestTypeGet,
                                                                                   _urlToCallLineEdit->text(),
                                                                                   additionalHeaderKeyValueMap,
                                                                                   useAuthentication,
                                                                                   _postValuesLineEdit->text().toAscii(),
                                                                                   QNetworkRequest::AlwaysNetwork));
    DLog("!!!!!");
    DLog(useAuthentication);
    DLog("!!!!!");
}


void MainWindow::_authenticationChallenge() {
    _testLogTextEdit->append(tr("Authentication challange!"));
}

void MainWindow::_failedToSend() {
    _testLogTextEdit->append(tr("- Failed to send!"));
}

void MainWindow::_sendFailedWithError(QString errorMessage) {
    _testLogTextEdit->append(tr("- Faild to send. Error message: %1").arg(errorMessage));
}

void MainWindow::_sendFinished() {
    _testLogTextEdit->append(tr("Request success"));
}

void MainWindow::_sendTimeouted() {
    _testLogTextEdit->append(tr("Timeout!"));
}

void MainWindow::_sendResponseDownloaded() {
    _testLogTextEdit->append(tr("Response downloaded!"));
}

void MainWindow::_response(QList<BaseNetworkEntity*> responseEntities)
{
//    _responseTextEdit->clear();
//    _responseTextEdit->append("Valid response returned:");
//    for(int i = 0; i < responseEntities.size(); ++i)
//    {
//        PlainResponseNetworkEntity *plainResponseEntity = dynamic_cast<PlainResponseNetworkEntity *>(responseEntities[i]);
//        if(plainResponseEntity == NULL) {
//            _testLogTextEdit->append("Invalid response entity - cannot cast!");
//        }
//        else {
//            _responseTextEdit->append("--");
//            _responseTextEdit->append(plainResponseEntity->getResponseAsString());
//        }
//    }
}

void MainWindow::_plainResponseDataReceived(QByteArray responseData)
{
    _responseTextEdit->clear();
    _responseTextEdit->append(responseData);
}


void MainWindow::_openWebViewWindow()
{
//    if(_simpleSslClient == NULL) {
//        _simpleSslClient = new SimpleSslClient(this);
//    }
//    _simpleSslClient->startRequest("https://twitter.com", 80);

    SimpleWebViewWindow *webView = new SimpleWebViewWindow();
    webView->setAttribute(Qt::WA_DeleteOnClose);
    webView->showNormal();
}
