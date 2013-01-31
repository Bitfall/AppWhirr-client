#include "helppage.h"
#include "ui_helppage.h"

#include <QLabel>

#include "Common/Widgets/helpitemwidget.h"
// menu
#include "Common/Menu/menuitemwidget.h"
// page navigation
#include "Common/Page/pagenavigationmanager.h"
#include "Settings/pageidcenter.h"
// global objects
#include "Application/globalobjectregistry.h"
// utility
#include "Common/Widgets/simpleimagepresenterwidget.h"
#include "Browser/Content/Widget/helpitempresenterwindow.h"

HelpPage::HelpPage(QString pageId, QWidget *parent) :
    PageWidgetBase(pageId, parent),
    ui(new Ui::HelpPage)
{
}

HelpPage::~HelpPage()
{
    delete ui;
}

// slot
void HelpPage::relatedMenuItemActivated(QString menuItemId)
{
    PageNavigationManager *pageNavigationManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getPageNavigationManager();
    if(pageNavigationManager->isThisRootStackIsTheActiveOne(PageIdCenter::helpPageId())) {
        // this root page's stack is the active one, so only pop-back to it
        pageNavigationManager->popBackToThisPage(PageIdCenter::helpPageId());
    } else {
        // currently this root page's stack is not the active one, so switch to it
        pageNavigationManager->switchToRootPageStack(PageIdCenter::helpPageId());
    }
}

void HelpPage::initializePageContent()
{
    ui->setupUi(this);
    ui->scrollArea->setFrameShape(QFrame::NoFrame);

    //this->setContentsMargins(guiConfManager->getMarginsForId("PageWidget/Default/contentMargins"));

    QWidget *itemContainerWidget = new QWidget;
    QVBoxLayout *itemLayout = new QVBoxLayout;
    itemContainerWidget->setLayout(itemLayout);

    HelpItemWidget *item = new HelpItemWidget;
    item->setTitleText("Welcome / promo video");
    item->loadThisHtml("<iframe width=\"560\" height=\"349\" src=\"http://www.youtube.com/embed/sXX5drqRD9s\" frameborder=\"0\" allowfullscreen></iframe>");
    itemLayout->addWidget(item);

//    HelpItemWidget *item2 = new HelpItemWidget;
//    item2->setTitleText("How to use the system (basics)");
//    item2->loadThisHtml("<iframe width=\"560\" height=\"349\" src=\"http://www.youtube.com/embed/i-OGD2DgIHI\" frameborder=\"0\" allowfullscreen></iframe>");
//    itemLayout->addWidget(item2);

    QLabel *helpItem2TitleLabel = new QLabel;
    helpItem2TitleLabel->setText(tr("How to use the system (basics)"));
    itemLayout->addWidget(helpItem2TitleLabel);
    SimpleImagePresenterWidget *helpItem2Image = new SimpleImagePresenterWidget;
    helpItem2Image->setIsAutoResizeToImageSize(true);
    helpItem2Image->setImageByPath(QString(":/help/HowToUseBasics"), ImageTypes::PixelBased);
    connect(helpItem2Image, SIGNAL(clicked()), this, SLOT(presentHelpItem2()));
    itemLayout->addWidget(helpItem2Image);


    //
    // help item 3
    QWidget *helpItem3ContainerWidget = new QWidget;
    QVBoxLayout *helpItem3ContainerLayout = new QVBoxLayout;
    helpItem3ContainerWidget->setLayout(helpItem3ContainerLayout);

    QLabel *helpItem3TitleLabel = new QLabel;
    helpItem3TitleLabel->setText(tr("How to create a Desktop Shortcut"));
    SimpleImagePresenterWidget *helpItem3Image = new SimpleImagePresenterWidget;
    helpItem3Image->setIsAutoResizeToImageSize(true);
    helpItem3Image->setImageByPath(QString(":/help/HowToCreateDesktopShortcut"), ImageTypes::PixelBased);

    helpItem3ContainerLayout->addWidget(helpItem3TitleLabel);
    helpItem3ContainerLayout->addWidget(helpItem3Image);
    itemLayout->addWidget(helpItem3ContainerWidget);


    //
    // finalize / set the main container scroll area
    ui->scrollArea->setWidget(itemContainerWidget);
}


void HelpPage::presentHelpItem2()
{
    HelpItemPresenterWindow *helpPresenterWindow = new HelpItemPresenterWindow(this);
    helpPresenterWindow->setTitle(tr("How to use the system (basics)"));
    helpPresenterWindow->setHtmlContent("<iframe width=\"560\" height=\"349\" src=\"http://www.youtube.com/embed/i-OGD2DgIHI\" frameborder=\"0\" allowfullscreen></iframe>");
    helpPresenterWindow->showNormal();
}


void HelpPage::pagePushWillHappen()
{

}

void HelpPage::pageWillAppear(QString param)
{

}

void HelpPage::pageDidAppear()
{

}

void HelpPage::pagePopWillHappen()
{

}

void HelpPage::pageWillDisappear()
{

}
