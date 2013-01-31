#include "navigationwidget.h"

#include "Common/Widgets/simpleimagepresenterwidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QPoint>
#include <QPushButton>
#include <QLabel>

#include <QPainter>
#include <QStyleOption>

#include <QApplication>
#include <QDesktopWidget>
#include "Application/appwhirrapplication.h"
#include "Browser/browserwindow.h"

// config
//#include "Settings/guiconfigurations.h"
#include "Application/globalobjectregistry.h"
#include "Common/guiconfigurationmanager.h"

NavigationWidget::NavigationWidget(QWidget *parent) :
    QWidget(parent)
{
//    QVBoxLayout* mainLayout = new QVBoxLayout;
//    mainLayout->setContentsMargins(0, 0, 0, 0);
//    this->setLayout(mainLayout);

    GuiConfigurationManager *guiConfManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager();

//    QWidget* container = new QWidget;
//    container->setFixedHeight(31);
//    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
//    container->setObjectName("NavigationWidgetContentContainer");
//    container->setStyleSheet(guiConfManager->getStyleForId("MainWindow/Header/style"));

    this->setFixedHeight(31);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    this->setObjectName("NavigationWidgetContentContainer");
    this->setStyleSheet(guiConfManager->getStyleForId("MainWindow/Header/style"));

    // defaultDarkGreenColorHex

//    mainLayout->addWidget(container);

    QHBoxLayout* navigationLayout = new QHBoxLayout;
    navigationLayout->setContentsMargins(0, 0, 0, 0);
//    navigationLayout->setSpacing(16);
    this->setLayout(navigationLayout);
    navigationLayout->setSpacing(3);

//    navigationLayout->addSpacerItem(new QSpacerItem(10, 0));

#ifdef Q_OS_WIN
    navigationLayout->addSpacing(3*16 + 3*3 + 10);
    navigationLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding));
#endif

//    navigationLayout->addStretch(0);

//    SimpleImagePresenterWidget* logo = new SimpleImagePresenterWidget;
//    logo->setIsAutoResizeToImageSize(false);
//    logo->setFixedSize(155, 31);
//    logo->setImageByPath(QString(":/navigation_logo"), ImageTypes::PixelBased);
//    navigationLayout->addWidget(logo);
    _userNameText = new QLabel(tr("user [offline]"));
    _userNameText->setStyleSheet(guiConfManager->getStyleForId("Label/Default/LightGray/style"));
    navigationLayout->addWidget(_userNameText/*, 0, Qt::AlignHCenter*/);

    navigationLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding));

//    navigationLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Maximum));
//    navigationLayout->addStretch(0);

    //
    // window state management buttons



    QPushButton *minimizeButton = new QPushButton;
    minimizeButton->setFlat(true);
    minimizeButton->setFixedSize(20, 20);
    connect(minimizeButton, SIGNAL(clicked()), this, SIGNAL(minimizeButtonClicked()));
    connect(minimizeButton, SIGNAL(clicked()), this, SLOT(_minimizeButtonClicked()));
    minimizeButton->setStyleSheet(guiConfManager->getStyleForId("MainWindow/Header/Buttons/Minimize/style"));
#ifdef Q_OS_WIN
    navigationLayout->addWidget(minimizeButton);
#endif
#ifdef Q_OS_MAC
    navigationLayout->insertWidget(0, minimizeButton);
#endif

    QPushButton *maximizeButton = new QPushButton;
    maximizeButton->setFixedSize(20, 20);
    maximizeButton->setFlat(true);
    connect(maximizeButton, SIGNAL(clicked()), this, SIGNAL(maximizeButtonClicked()));
    maximizeButton->setStyleSheet(guiConfManager->getStyleForId("MainWindow/Header/Buttons/Maximize/style"));
#ifdef Q_OS_WIN
    navigationLayout->addWidget(maximizeButton);
#endif
#ifdef Q_OS_MAC
    navigationLayout->insertWidget(1, maximizeButton);
#endif

    QPushButton *closeButton = new QPushButton;
    closeButton->setFixedSize(20, 20);
    closeButton->setFlat(true);
    connect(closeButton, SIGNAL(clicked()), this, SIGNAL(closeButtonClicked()));
    closeButton->setStyleSheet(guiConfManager->getStyleForId("MainWindow/Header/Buttons/Close/style"));
#ifdef Q_OS_WIN
    navigationLayout->addWidget(closeButton);
#endif
#ifdef Q_OS_MAC
    navigationLayout->insertWidget(0, closeButton);
#endif

#ifdef Q_OS_WIN
    navigationLayout->addSpacing(10);
#endif
#ifdef Q_OS_MAC
    navigationLayout->insertSpacing(0, 10);

    navigationLayout->addSpacing(3*16 + 3*3 + 10);
    navigationLayout->insertSpacerItem(4, new QSpacerItem(0, 0, QSizePolicy::Expanding));
#endif

}

void NavigationWidget::setUserNameText(QString userNameText)
{
    _userNameText->setText(userNameText);
}

void NavigationWidget::mousePressEvent (QMouseEvent * e) {
    if (e->button() == Qt::LeftButton) {
//        DLog("Mouse press");
        moving = true;
        offset = e->pos();
    }
    QWidget::mousePressEvent(e);
}

void NavigationWidget::mouseMoveEvent (QMouseEvent *e) {
    if (moving) {
//        DLog("Mouse move in header.");
        QPoint moveWith = e->globalPos() - offset;

#ifdef Q_OS_MAC
        // Check if we are in the allowed rect
        QRect availableRect = QApplication::desktop()->availableGeometry(this);
        if (moveWith.y() < availableRect.y())
            moveWith.setY(availableRect.y());
#endif

        AppwhirrApplication::instance()->getBrowserWindow()->move(moveWith);
    }
    QWidget::mouseMoveEvent(e);
}

void NavigationWidget::mouseReleaseEvent (QMouseEvent *e) {
    moving = false;
    QWidget::mouseReleaseEvent(e);
}

void NavigationWidget::paintEvent(QPaintEvent *e)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void NavigationWidget::_minimizeButtonClicked() {
    DLog("Minimize button click");
}
