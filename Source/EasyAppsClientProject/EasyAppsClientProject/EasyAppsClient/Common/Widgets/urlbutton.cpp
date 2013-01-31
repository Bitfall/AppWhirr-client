#include "urlbutton.h"

UrlButton::UrlButton(QWidget *parent) :
    QPushButton(parent)
{
    this->__commonInit();
}

UrlButton::UrlButton(QString titleText, QWidget *parent) :
    QPushButton(titleText)
{
    this->__commonInit();
}

UrlButton::UrlButton(QString titleText, QString relatedUrl, QWidget *parent) :
    QPushButton(titleText),
    _relatedUrl(relatedUrl)
{
    this->__commonInit();
}

void UrlButton::__commonInit()
{
    connect(this, SIGNAL(clicked()), this, SLOT(__clickedSlot()));
}

void UrlButton::setRelatedUrl(QString relatedUrl) {
    _relatedUrl = relatedUrl;
}

QString UrlButton::getRelatedUrl() const {
    return _relatedUrl;
}

void UrlButton::__clickedSlot()
{
    Q_EMIT clickedWithUrl(_relatedUrl);
}
