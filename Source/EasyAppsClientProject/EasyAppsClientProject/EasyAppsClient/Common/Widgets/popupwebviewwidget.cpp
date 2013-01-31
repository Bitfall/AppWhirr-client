#include "popupwebviewwidget.h"

#include <QWebView>
#include <QVBoxLayout>
#include <QUrl>

PopupWebviewWidget::PopupWebviewWidget(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    this->_mainContentWebView = new QWebView;
    this->_mainContentWebView->settings()->setAttribute(QWebSettings::PluginsEnabled, true);
    mainLayout->addWidget(this->_mainContentWebView);
}

// SLOT
void PopupWebviewWidget::synchronizeGeometry(QRect targetGeometry)
{
    this->setGeometry(targetGeometry);
}

void PopupWebviewWidget::setHtmlContent(QString content) {
    this->_mainContentWebView->setHtml(content);
}
