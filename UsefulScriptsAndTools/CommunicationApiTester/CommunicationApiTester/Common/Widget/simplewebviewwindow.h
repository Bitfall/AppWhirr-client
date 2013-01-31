#ifndef SIMPLEWEBVIEWWINDOW_H
#define SIMPLEWEBVIEWWINDOW_H

#include <QWidget>

#include <QUrl>
#include <QNetworkProxy>
#include <QSslError>

QT_BEGIN_NAMESPACE
class QLineEdit;
class QWebView;
class QNetworkReply;
class QAuthenticator;
QT_END_NAMESPACE

class SimpleWebViewWindow : public QWidget
{
    Q_OBJECT
public:
    explicit SimpleWebViewWindow(QWidget *parent = 0);

private Q_SLOTS:
    void _openCurrentAddress();
    void _linkClicked(QUrl url);

    // ----------------------
    // --- error handling ---
    void _unsupportedContent(QNetworkReply *reply);
    void _authenticationRequired(QNetworkReply *reply, QAuthenticator *authenticator);
    void _proxyAuthenticationRequired(QNetworkProxy proxy, QAuthenticator *authenticator);
    void _sslErrors(QNetworkReply *reply, QList<QSslError> sslErrors);

private:
    QLineEdit *_addressLineEdit;
    QWebView *_webView;
};

#endif // SIMPLEWEBVIEWWINDOW_H
