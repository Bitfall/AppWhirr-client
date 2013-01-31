#include "helpitemwidget.h"
#include "ui_helpitemwidget.h"

#include <QWebView>
#include <QWebSettings>
#include <QUrl>

HelpItemWidget::HelpItemWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HelpItemWidget)
{
    ui->setupUi(this);
}

HelpItemWidget::~HelpItemWidget()
{
    delete ui;
}

void HelpItemWidget::setTitleText(QString titleText)
{
    this->ui->label->setContentsMargins(20, 0, 0, 0);
    this->ui->label->setText(titleText);
}

void HelpItemWidget::loadThisHtml(QString html)
{
//    QNetworkProxyFactory::setUseSystemConfiguration (true);
//    QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled, true);
//    QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled, false); // disable flash plugin
//    QWebSettings::globalSettings()->setAttribute(QWebSettings::AutoLoadImages, true);
//    QWebSettings::globalSettings()->setAttribute(QWebSettings::AutoLoadImages, false);

    this->ui->webView->settings()->setAttribute(QWebSettings::PluginsEnabled, false);
    this->ui->webView->setHtml(html);
}
