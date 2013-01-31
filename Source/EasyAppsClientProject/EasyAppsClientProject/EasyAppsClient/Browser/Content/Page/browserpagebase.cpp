#include "browserpagebase.h"

#include "Application/Communication/Gateway/communicationdescriptiongateway.h"
//#include "Application/Communication/Gateway/communicationresourcegateway.h"

#include "Common/Security/authorization.h"

// UI
#include "Application/Widgets/networkunreachablewidget.h"
#include "Common/Widgets/coverwithcustomwidget.h"

#include <QResizeEvent>

BrowserPageWidgetBase::BrowserPageWidgetBase(QString pageId, QWidget *parent) :
    PageWidgetBase(pageId, parent),
    communicationDescriptionGateway(0)
//    communicationResourceGateway(0)
{
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    NetworkUnreachableWidget *networkUnreachableCoverContentWidget = new NetworkUnreachableWidget;
//    networkUnreachableCoverContentWidget->setFixedSize(300, 300);
    networkUnreachableCoverContentWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _networkUnreachableCoverWidget = new CoverWithCustomWidget(networkUnreachableCoverContentWidget, 1000, -1, this);
    _networkUnreachableCoverWidget->hide();
}

BrowserPageWidgetBase::~BrowserPageWidgetBase() {
    AW_QOBJECT_SAFE_DELETE(communicationDescriptionGateway);
//    AW_QOBJECT_SAFE_DELETE(communicationResourceGateway);
}

void BrowserPageWidgetBase::resizeEvent(QResizeEvent *e)
{
    _networkUnreachableCoverWidget->setGeometryToThis(this->geometry());
}

void BrowserPageWidgetBase::initializeRequests() {
}
