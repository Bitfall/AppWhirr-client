#include "helpitempresenterwindow.h"

#include <QWebView>
#include <QLabel>
#include <QGridLayout>
#include <QUrl>

HelpItemPresenterWindow::HelpItemPresenterWindow(QWidget *parent) :
    QMainWindow(parent)
{
    this->resize(600, 400);

    QWidget *mainCentralWidget = new QWidget;
    this->setCentralWidget(mainCentralWidget);

    QGridLayout *mainLayout = new QGridLayout;
    mainCentralWidget->setLayout(mainLayout);

    this->_titleLabel = new QLabel;
    this->_titleLabel->setText(tr("Help title"));
    mainLayout->addWidget(this->_titleLabel, 0, 0, 1, -1);

    this->_mainContentWebView = new QWebView;
    this->_mainContentWebView->settings()->setAttribute(QWebSettings::PluginsEnabled, true);
    mainLayout->addWidget(this->_mainContentWebView, 1, 0, 1, -1);
}

void HelpItemPresenterWindow::setTitle(QString titleText)
{
    this->_titleLabel->setText(titleText);
}

void HelpItemPresenterWindow::setUrl(QString urlString) {
    this->_mainContentWebView->setUrl(QUrl(urlString));
}

void HelpItemPresenterWindow::setHtmlContent(QString content) {
    this->_mainContentWebView->setHtml(content);
}
