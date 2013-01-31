#include "simplewebviewwindow.h"

// UI
#include <QWebView>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

// Network
#include <QUrl>
#include <QDesktopServices>
#include <QNetworkReply>
#include <QAuthenticator>

SimpleWebViewWindow::SimpleWebViewWindow(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    QHBoxLayout *addressLayout = new QHBoxLayout;
    _addressLineEdit = new QLineEdit;
    connect(_addressLineEdit, SIGNAL(returnPressed()), this, SLOT(_openCurrentAddress()));
    addressLayout->addWidget(_addressLineEdit);
    QPushButton *goToAddressButton = new QPushButton(tr("Go"));
    connect(goToAddressButton, SIGNAL(clicked()), this, SLOT(_openCurrentAddress()));
    addressLayout->addWidget(goToAddressButton);

    mainLayout->addLayout(addressLayout);


    //
    _webView = new QWebView;
    connect(_webView->page(), SIGNAL(linkClicked(QUrl)), this, SLOT(_linkClicked(QUrl)));
    connect(_webView->page(), SIGNAL(unsupportedContent(QNetworkReply*)), this, SLOT(_unsupportedContent(QNetworkReply*)));
    connect(_webView->page()->networkAccessManager(), SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)), this, SLOT(_authenticationRequired(QNetworkReply*,QAuthenticator*)));
    connect(_webView->page()->networkAccessManager(), SIGNAL(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)), this, SLOT(_proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)));
    connect(_webView->page()->networkAccessManager(), SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), this, SLOT(_sslErrors(QNetworkReply*,QList<QSslError>)));
    mainLayout->addWidget(_webView);
}

void SimpleWebViewWindow::_openCurrentAddress()
{
    DLog("Start to open address / url: ") << _addressLineEdit->text();
    _webView->load(QUrl(_addressLineEdit->text()));
}

void SimpleWebViewWindow::_linkClicked(QUrl url)
{
    QDesktopServices::openUrl(url);
}

// ----------------------
// --- error handling ---

void SimpleWebViewWindow::_unsupportedContent(QNetworkReply *reply)
{
    DLog("Unsupported content: ") << reply->url();
}

void SimpleWebViewWindow::_authenticationRequired(QNetworkReply *reply, QAuthenticator *authenticator)
{
    DLog("Authentication required: ") << reply->url();
}

void SimpleWebViewWindow::_proxyAuthenticationRequired(QNetworkProxy proxy, QAuthenticator *authenticator)
{
    DLog("PROXY Authentication required: ") << proxy.hostName();
}

void SimpleWebViewWindow::_sslErrors(QNetworkReply *reply, QList<QSslError> sslErrors)
{
    DLog("SSL errors from url: ") << reply->url();

    for(int i = 0; i < sslErrors.size(); ++i)
    {
        DLog("* SSL Error: ") << sslErrors[i].errorString();
    }

    reply->ignoreSslErrors(sslErrors);
}
