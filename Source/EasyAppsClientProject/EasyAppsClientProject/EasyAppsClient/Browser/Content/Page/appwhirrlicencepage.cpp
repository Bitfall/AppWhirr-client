#include "appwhirrlicencepage.h"

// UI
#include <QVBoxLayout>
#include <QTextEdit>

#include "Application/Widgets/incontentnavigationwidget.h"
#include "Application/uifactory.h"

//
#include <QFile>

// global objects
#include "Application/globalobjectregistry.h"
#include "Common/Menu/menumanager.h"
#include "Common/Page/pagenavigationmanager.h"

// config
#include "Settings/menuitemidcenter.h"
#include "Common/guiconfigurationmanager.h"

AppwhirrLicencePage::AppwhirrLicencePage(QString pageId, QWidget *parent) :
    PageWidgetBase(pageId, parent)
{
}

void AppwhirrLicencePage::initializePageContent()
{
    QVBoxLayout *mainLayout = AppWhirr::UiFactory::createDefaultVBoxLayout();
    this->setLayout(mainLayout);
    this->setContentsMargins(0, 0, 0, 0);

    InContentNavigationWidget *inContentNavigationWidget = new InContentNavigationWidget(tr("Go to Options"));
    connect(inContentNavigationWidget, SIGNAL(backButtonClicked()), this, SLOT(_backButtonPressed()));
    mainLayout->addWidget(inContentNavigationWidget);

    GuiConfigurationManager *guiConfManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager();

    this->setStyleSheet(guiConfManager->getStyleForId("Widget/Default/Background/style"));

    QVBoxLayout *contentLayout = new QVBoxLayout;
    contentLayout->setContentsMargins(guiConfManager->getMarginsForId("PageWidget/Default/contentMargins"));

    QTextEdit *appwhirrLicenceTextEdit = new QTextEdit;
    appwhirrLicenceTextEdit->setStyleSheet(guiConfManager->getStyleForId("TextEdit/NoBorder/LightGray/style"));
    appwhirrLicenceTextEdit->setReadOnly(true);

    QFile awTosFile(QString(":/appwhirr_tos"));
    if (!awTosFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        WLog("Cannot open the AppWhirr ToS file!");
        appwhirrLicenceTextEdit->setHtml("Cannot present AppWhirr Terms of Service...");
    }
    else {
        QString awTosText = awTosFile.readAll();
        awTosFile.close();

        appwhirrLicenceTextEdit->setHtml(awTosText);
//        appwhirrLicenceTextEdit->setHtml(QString(
//                                      "<p>"
//                                      "<b>Terms of Service<b>"
//                                      "</p>"

//                                      "<p>"
//                                      "AppWhirr licence"
//                                      "</p>"
//                                      ));
    }

    contentLayout->addWidget(appwhirrLicenceTextEdit);
    mainLayout->addLayout(contentLayout);
}

void AppwhirrLicencePage::pagePushWillHappen()
{

}

void AppwhirrLicencePage::pageWillAppear(QString param)
{
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getMenuManager()->highlightItemById(MenuItemIdCenter::appwhirrLicencePresenterPageMenuId());
}

void AppwhirrLicencePage::pageDidAppear()
{
//    qDebug("page did appear");
}

void AppwhirrLicencePage::pagePopWillHappen()
{
}

void AppwhirrLicencePage::pageWillDisappear()
{

}

void AppwhirrLicencePage::_backButtonPressed()
{
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getPageNavigationManager()->popPageFromCurrentRootStackAndRemove();
}
