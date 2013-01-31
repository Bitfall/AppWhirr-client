#include "pagewidgetbase.h"

#include <QStyleOption>
#include <QPainter>

// config
#include "Settings/guiconfigurations.h"
#include "Common/guiconfigurationmanager.h"
#include "Application/globalobjectregistry.h"

PageWidgetBase::PageWidgetBase(QString pageId, QWidget *parent) :
        QWidget(parent),
    _pageId(pageId)
{
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    this->setStyleSheet(QString("PageWidgetBase { background-color: %1; }").arg(GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager()->getStyleForId("ColorHex/Default/LightGray/WidgetBackground")));

//    DLog(" -- test style: ") << GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager()->getStyleForId("ColorHex/Default/LightGray/WidgetBackground");
    //    this->setStyleSheet(QString("PageWidgetBase { background-color: #00ff00; }"));
}

PageWidgetBase::~PageWidgetBase()
{
    DLog("Page destruct.");
}

QString PageWidgetBase::getPageId() const
{
    return this->_pageId;
}

void PageWidgetBase::setPageId(QString id)
{
    this->_pageId = id;
}

void PageWidgetBase::paintEvent(QPaintEvent *e)
{
    // must be provided for widgets inherits directly from QWidget if you want to style it with css
    //  check QWidget section here: http://doc.qt.nokia.com/4.7-snapshot/stylesheet-reference.html

    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
