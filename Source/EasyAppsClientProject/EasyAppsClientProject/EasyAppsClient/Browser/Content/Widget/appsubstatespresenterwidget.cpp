#include "appsubstatespresenterwidget.h"

// UI
#include <QVBoxLayout>
#include <QSpacerItem>
#include "Common/Widgets/simpleimagepresenterwidget.h"

//
#include <QStyleOption>
#include <QPainter>
#include "Common/imagetypesenum.h"

// config
#define SUBSTATE_ICON_SIZE 20

AppSubStatesPresenterWidget::AppSubStatesPresenterWidget(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(5);
    this->setLayout(mainLayout);

    _updateAvailableIconPresenterWidget = new SimpleImagePresenterWidget;
    _updateAvailableIconPresenterWidget->setIsAutoResizeToImageSize(false);
    _updateAvailableIconPresenterWidget->setFixedSize(SUBSTATE_ICON_SIZE, SUBSTATE_ICON_SIZE);
    connect(_updateAvailableIconPresenterWidget, SIGNAL(clicked()), this, SIGNAL(updateAvailableIconPresenterActivated()));
    mainLayout->addWidget(_updateAvailableIconPresenterWidget);
    _updateAvailableIconPresenterWidget->hide();

    _notSupportedOnThisPlatformIconPresenterWidget = new SimpleImagePresenterWidget;
    _notSupportedOnThisPlatformIconPresenterWidget->setIsAutoResizeToImageSize(false);
    _notSupportedOnThisPlatformIconPresenterWidget->setFixedSize(SUBSTATE_ICON_SIZE, SUBSTATE_ICON_SIZE);
    connect(_notSupportedOnThisPlatformIconPresenterWidget, SIGNAL(clicked()), this, SIGNAL(notAvailableForThisPlatformIconPresenterActivated()));
    mainLayout->addWidget(_notSupportedOnThisPlatformIconPresenterWidget);
    _notSupportedOnThisPlatformIconPresenterWidget->hide();

    _removedFromTheSystemIconPresenterWidget = new SimpleImagePresenterWidget;
    _removedFromTheSystemIconPresenterWidget->setIsAutoResizeToImageSize(false);
    _removedFromTheSystemIconPresenterWidget->setFixedSize(SUBSTATE_ICON_SIZE, SUBSTATE_ICON_SIZE);
    connect(_removedFromTheSystemIconPresenterWidget, SIGNAL(clicked()), this, SIGNAL(removedFromStoreIconPresenterActivated()));
    mainLayout->addWidget(_removedFromTheSystemIconPresenterWidget);
    _removedFromTheSystemIconPresenterWidget->hide();

//    mainLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));
}

void AppSubStatesPresenterWidget::setSubStates(AppWhirr::ApplicationSubStateFlags::SummedApplicationSubStateFlags summedFlags)
{
    this->_summedSubStateFlags = summedFlags;

    if((summedFlags & AppWhirr::ApplicationSubStateFlags::UpdateAvailable) == AppWhirr::ApplicationSubStateFlags::UpdateAvailable) {
        if(!_updateAvailableIconPresenterWidget->getIsImageDefined()) {
            _updateAvailableIconPresenterWidget->setImageByPath(":/images/appSubStateIconImages/UpdateAvailable-Normal", ImageTypes::PixelBased);
        }
        _updateAvailableIconPresenterWidget->show();
    } else {
        _updateAvailableIconPresenterWidget->hide();
    }

    if((summedFlags & AppWhirr::ApplicationSubStateFlags::NotAvailableOnThisPlatform) == AppWhirr::ApplicationSubStateFlags::NotAvailableOnThisPlatform) {
        if(!_notSupportedOnThisPlatformIconPresenterWidget->getIsImageDefined()) {
            _notSupportedOnThisPlatformIconPresenterWidget->setImageByPath(":/images/appSubStateIconImages/NotAvailableOnThisPlatform-Normal", ImageTypes::PixelBased);
        }
        _notSupportedOnThisPlatformIconPresenterWidget->show();
    } else {
        _notSupportedOnThisPlatformIconPresenterWidget->hide();
    }

    if((summedFlags & AppWhirr::ApplicationSubStateFlags::AppRemovedFromTheSystem) == AppWhirr::ApplicationSubStateFlags::AppRemovedFromTheSystem) {
        if(!_removedFromTheSystemIconPresenterWidget->getIsImageDefined()) {
            _removedFromTheSystemIconPresenterWidget->setImageByPath(":/images/appSubStateIconImages/AppRemovedFromTheSystem-Normal", ImageTypes::PixelBased);
        }
        _removedFromTheSystemIconPresenterWidget->show();
    } else {
        _removedFromTheSystemIconPresenterWidget->hide();
    }
}

AppWhirr::ApplicationSubStateFlags::SummedApplicationSubStateFlags AppSubStatesPresenterWidget::getSummedSubStateFlags() const {
    return this->_summedSubStateFlags;
}

void AppSubStatesPresenterWidget::paintEvent(QPaintEvent *e)
{
    // must be provided for widgets inherits directly from QWidget if you want to style it with css
    //  check QWidget section here: http://doc.qt.nokia.com/4.7-snapshot/stylesheet-reference.html

    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
