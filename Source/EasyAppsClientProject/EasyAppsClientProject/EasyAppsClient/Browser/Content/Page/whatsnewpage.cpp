#include "whatsnewpage.h"

// UI
#include <QVBoxLayout>
#include <QTextEdit>

#include "Application/Widgets/incontentnavigationwidget.h"
#include "Application/uifactory.h"

// global objects
#include "Application/globalobjectregistry.h"
#include "Common/Menu/menumanager.h"
#include "Common/Page/pagenavigationmanager.h"

// config
#include "Settings/menuitemidcenter.h"
#include "Common/guiconfigurationmanager.h"

WhatsNewPage::WhatsNewPage(QString pageId, QWidget *parent) :
    PageWidgetBase(pageId, parent)
{

}

void WhatsNewPage::initializePageContent()
{
    QVBoxLayout *mainLayout = AppWhirr::UiFactory::createDefaultVBoxLayout();
    this->setLayout(mainLayout);
    this->setContentsMargins(0, 0, 0, 0);

    GuiConfigurationManager *guiConfManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager();

    this->setStyleSheet(guiConfManager->getStyleForId("Widget/Default/Background/style"));

    InContentNavigationWidget *inContentNavigationWidget = new InContentNavigationWidget(tr("Go to Options"));
    connect(inContentNavigationWidget, SIGNAL(backButtonClicked()), this, SLOT(_backButtonPressed()));
    mainLayout->addWidget(inContentNavigationWidget);

    QVBoxLayout *contentLayout = new QVBoxLayout;
    contentLayout->setContentsMargins(guiConfManager->getMarginsForId("PageWidget/Default/contentMargins"));

    QTextEdit *whatsNewTextEdit = new QTextEdit;
    whatsNewTextEdit->setStyleSheet(guiConfManager->getStyleForId("TextEdit/NoBorder/LightGray/style"));
    whatsNewTextEdit->setReadOnly(true);
    whatsNewTextEdit->setHtml(QString(
                                  "<p>"
                                  "<b>2012.06.22<b>"
                                  "<ul>"
                                  "<li>'Uninstall AppWhirr' option added to the Options page. From now you can uninstall AppWhirr completely from your system or every app you've installed with AppWhirr, or both.</li>"
                                  "<li>New option added: notify us if an app is outdated in our app catalog. You can notify us from the MyApps page after you've selected an installed app (through the app's management sidebar).</li>"
                                  "<li>App updates and uninstalls require about 50% less time.</li>"
                                  "</ul>"
                                  "</p>"

                                  "<p>"
                                  "<b>2012.06.15<b>"
                                  "<ul>"
                                  "<li>Text corrections and enhancements to make it easier to understand what these actions will do.</li>"
                                  "<li>App update checking correction.</li>"
                                  "</ul>"
                                  "</p>"

                                  "<p>"
                                  "<b>2012.06.01<b>"
                                  "<ul>"
                                  "<li>Self Updater style enhancements.</li>"
                                  "<li>Store: app icon state correction.</li>"
                                  "<li>Options: remove every local data presents notification about whether it could remove everything or not.</li>"
                                  "<li>Better app install and uninstall state presentation.</li>"
                                  "</ul>"
                                  "</p>"

                                  "<p>"
                                  "<b>2012.05.20<b>"
                                  "<ul>"
                                  "<li>Adjusted, better mouse scrolling both on Windows and on Mac.</li>"
                                  "<li>App install/uninstall reliability improvements.</li>"
                                  "</ul>"
                                  "</p>"

                                  "<p>"
                                  "<b>2012.05.11<b>"
                                  "<ul>"
                                  "<li>You can access the feedback page right from the tray icon (on Windows) or Dock menu (on Mac).</li>"
                                  "<li>Store user interface enhancements to make it more responsive.</li>"
                                  "</ul>"
                                  "</p>"

                                  "<p>"
                                  "<b>2012.04.21<b>"
                                  "<ul>"
                                  "<li>Windows apps added to the Start Menu after install (and removed if you uninstall the app). From now you can start your apps from the Start Menu and with quick launcher apps like Launchy.</li>"
                                  "</ul>"
                                  "</p>"

                                  "<p>"
                                  "<b>2012.04.14<b>"
                                  "<ul>"
                                  "<li>Mac OS X Snow Leopard is officially supported from now!</li>"
                                  "</ul>"
                                  "</p>"

                                  "<p>"
                                  "<b>2012.04.12<b>"
                                  "<ul>"
                                  "<li>Faster and more reliable communication with the server.</li>"
                                  "<li>And other general improvements.</li>"
                                  "</ul>"
                                  "</p>"

                                  "<p>"
                                  "<b>2012.03.26<b>"
                                  "<ul>"
                                  "<li>App install improvements and fixes: install is much faster even for bigger apps.</li>"
                                  "</ul>"
                                  "</p>"

                                  "<p>"
                                  "<b>2012.03.18<b>"
                                  "<ul>"
                                  "<li>Huge app install improvements:</li>"
                                  "<li>App installation is 2-3 times faster now!</li>"
                                  "<li>Also you can decide now whether you allow or deny framework downloading and installation.</li>"
                                  "<li>And more, general improvements in app installation.</li>"
                                  "</ul>"
                                  "</p>"

                                  "<p>"
                                  "<b>2012.03.10<b>"
                                  "<ul>"
                                  "<li>Better multi-monitor support.</li>"
                                  "</ul>"
                                  "</p>"

                                  "<p>"
                                  "<b>2012.03.07<b>"
                                  "<ul>"
                                  "<li>App install improvements - AppWhirr is able to handle Java apps as well from now.</li>"
                                  "</ul>"
                                  "</p>"

                                  "<p>"
                                  "<b>2012.03.03<b>"
                                  "<ul>"
                                  "<li>Mac: the separate menu bar icon is replaced with a Dock-integrated menu.</li>"
                                  "</ul>"
                                  "</p>"

                                  "<p>"
                                  "<b>2012.03.01<b>"
                                  "<ul>"
                                  "<li>Shortcut key improvements:</li>"
                                  "<li>You can now close the AppWhirr window with Ctrl+W (Cmd+W on Mac).</li>"
                                  "<li>Minimize with Ctrl+M (Cmd+M on Mac).</li>"
                                  "<li>And hide with Ctrl+H (Cmd+H on Mac).</li>"
                                  "</ul>"
                                  "</p>"

                                  "<p>"
                                  "<b>2012.02.29<b>"
                                  "<ul>"
                                  "<li>Windows 8 support.</li>"
                                  "</ul>"
                                  "</p>"

                                  "<p>"
                                  "<b>2012.02.25<b>"
                                  "<ul>"
                                  "<li>You can clear the search-text in MyApps by pressing Escape.</li>"
                                  "</ul>"
                                  "</p>"


                                  "<p>"
                                  "<b>2012.02.20<b>"
                                  "<ul>"
                                  "<li>Search interface correction, also removed the 'minimum 3 character required for search' limit.</li>"
                                  "<li>Minor Store design corrections.</li>"
                                  "</ul>"
                                  "</p>"

                                  "<p>"
                                  "<b>2012.02.14<b>"
                                  "<ul>"
                                  "<li>Countdown before starting 'autostart' apps.</li>"
                                  "<li>We added distinct indicators for 'grabbed' and for 'installed' apps on store pages.</li>"
                                  "</ul>"
                                  "</p>"

                                  "<p>"
                                  "<b>2012.02.08<b>"
                                  "<ul>"
                                  "<li>Automatic user login: you don't have to log-in every time you start AppWhirr.</li>"
                                  "<li>'Manage' button added for the MyApps page's icons.</li>"
                                  "<li>Notifications are more responsive from now.</li>"
                                  "</ul>"
                                  "</p>"

                                  "<p>"
                                  "<b>2012.01.30<b>"
                                  "<ul>"
                                  "<li>Major app installation improvements.</li>"
                                  "<li>UTF-8 support added for every app information and for username and password - you can now use accented username and password if you want.</li>"
                                  "<li>Completely new design when AppWhirr updates itself.</li>"
                                  "</ul>"
                                  "</p>"

                                  "<p>"
                                  "<b>2012.01.28<b>"
                                  "<ul>"
                                  "<li>Improvements to make AppWhirr app installation faster and more reliable.</li>"
                                  "<li>You can use accented username and password.</li>"
                                  "</ul>"
                                  "</p>"

                                  "<p>"
                                  "<b>2012.01.24<b>"
                                  "<ul>"
                                  "<li>Improvements to make AppWhirr faster and more reliable.</li>"
                                  "</ul>"
                                  "</p>"

                                  "<p>"
                                  "<b>2012.01.18<b>"
                                  "<ul>"
                                  "<li>App installation improvements: new apps with .Net and Visual Studio dependencies allowed.</li>"
                                  "<li>Store extension: 'show more' button added to the sections. With this you can check even more great apps.</li>"
                                  "</ul>"
                                  "</p>"

                                  "<p>"
                                  "<b>2012.01.12<b>"
                                  "<ul>"
                                  "<li>App installation improvements! (Thank you for your feedbacks!)</li>"
                                  "</ul>"
                                  "</p>"

                                  "<p>"
                                  "<b>2012.01.09<b>"
                                  "<ul>"
                                  "<li>Initial public Beta release!</li>"
                                  "</ul>"
                                  "</p>"

                                  "<p>"
                                  "<b>2011.12.15<b>"
                                  "<ul>"
                                  "<li>'Send Feedback' feature.</li>"
                                  "<li>'Suggest an App' feature.</li>"
                                  "</ul>"
                                  "</p>"

                                  "<p>"
                                  "<b>2011.12.09<b>"
                                  "<ul>"
                                  "<li>Videos available for applications as an addition to screenshots.</li>"
                                  "<li>Main menu changes - looks much better!</li>"
                                  "</ul>"
                                  "</p>"

                                  "<p>"
                                  "<b>2011.11.20<b>"
                                  "<ul>"
                                  "<li>Awesome new logo!</li>"
                                  "<li>Improved notifications.</li>"
                                  "<li>Additional app information.</li>"
                                  "</ul>"
                                  "</p>"

                                  "<p>"
                                  "<b>2011.10.31<b>"
                                  "<ul>"
                                  "<li>Window can be resized.</li>"
                                  "</ul>"
                                  "</p>"

                                  "<p>"
                                  "<b>2011.10.28<b>"
                                  "<ul>"
                                  "<li>Tons of User Interface and User Experience modifications and improvements.</li>"
                                  "</ul>"
                                  "</p>"

                                  "<p>"
                                  "<b>2011.10.18<b>"
                                  "<ul>"
                                  "<li>Search in the Store improvement with Instant Search (type at least 3 characters and it will automatically start a search).</li>"
                                  "</ul>"
                                  "</p>"

                                  "<p>"
                                  "<b>2011.10.15<b>"
                                  "<ul>"
                                  "<li>Search in the Store.</li>"
                                  "</ul>"
                                  "</p>"

                                  "<p>"
                                  "<b>2011.10.03<b>"
                                  "<ul>"
                                  "<li>Create Desktop Shortcut for installed app added.</li>"
                                  "<li>Installed app uninstall, and hide app without uninstall options added.</li>"
                                  "<li>Show only installed apps option added.</li>"
                                  "</ul>"
                                  "</p>"

                                  "<p>"
                                  "<b>2011.09.24<b>"
                                  "<ul>"
                                  "<li>Installed application loading is much much faster now.</li>"
                                  "<li>Also there are less slow-downs everywhere else.</li>"
                                  "</ul>"
                                  "</p>"

                                  "<p>"
                                  "<b>2011.09.19<b>"
                                  "<ul>"
                                  "<li>What's new page added.</li>"
                                  "<li>App local settings remain even if the app is uninstalled.</li>"
                                  "</ul>"
                                  "</p>"
                                  ));

    contentLayout->addWidget(whatsNewTextEdit);
    mainLayout->addLayout(contentLayout);
}

void WhatsNewPage::pagePushWillHappen()
{

}

void WhatsNewPage::pageWillAppear(QString param)
{
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getMenuManager()->highlightItemById(MenuItemIdCenter::whatsNewPageMenuId());
}

void WhatsNewPage::pageDidAppear()
{
//    qDebug("page did appear");
}

void WhatsNewPage::pagePopWillHappen()
{
}

void WhatsNewPage::pageWillDisappear()
{

}

void WhatsNewPage::_backButtonPressed()
{
    GlobalObjectRegistry::sharedGlobalObjectRegistry()->getPageNavigationManager()->popPageFromCurrentRootStackAndRemove();
}
