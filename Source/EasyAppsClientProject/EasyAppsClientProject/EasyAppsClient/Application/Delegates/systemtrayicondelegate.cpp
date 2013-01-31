#include "systemtrayicondelegate.h"

#include <QAction>
#include <QMenu>
#include <QApplication>

#include "Common/Event/ensureapplicationpresentedevent.h"

#include "Application/appwhirrapplication.h"

// config
#include "Settings/configmanager.h"


#ifdef Q_OS_MAC
extern void qt_mac_set_dock_menu(QMenu *menu);
#endif


SystemTrayIconDelegate::SystemTrayIconDelegate(QObject *parent) :
    QObject(parent),
    _showMyAppsAction(NULL),
    _showStoreAction(NULL),
    _showFeedbackPageAction(NULL),
    _quitAction(NULL),
    _trayIcon(NULL),
    _trayIconMenu(NULL)
{
}

SystemTrayIconDelegate::~SystemTrayIconDelegate() {

    if(_trayIcon != NULL) {
        if(_trayIcon->parent() == NULL) {
            _trayIcon->hide();
            delete _trayIcon;
        }
    }


    if(_trayIconMenu->parent() == NULL) {
        delete _trayIconMenu;
    }
}

void SystemTrayIconDelegate::createTheSystemTrayItem()
{
    // if we have system tray
    if(!QSystemTrayIcon::isSystemTrayAvailable()) {
        qDebug("There's no system-tray - cannot create the icon in it.");
        return;
    }

    // create the actions
    _showMyAppsAction = new QAction(tr("Show &MyApps"), this);
    connect(_showMyAppsAction, SIGNAL(triggered()), this, SIGNAL(showMyAppsPageSelected()));

    _showStoreAction = new QAction(tr("Show &Store"), this);
    connect(_showStoreAction, SIGNAL(triggered()), this, SIGNAL(showStorePageSelected()));

    _showFeedbackPageAction = new QAction(tr("Send &Feedback"), this);
    connect(_showFeedbackPageAction, SIGNAL(triggered()), this, SIGNAL(showFeedbackPageSelected()));

#ifdef Q_OS_WIN
    _quitAction = new QAction(tr("&Quit"), this);
    connect(_quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
#endif

    // create the tray-menu
    QWidget *parentWidget = NULL;

    _trayIconMenu = new QMenu(parentWidget);
    _trayIconMenu->addAction(_showMyAppsAction);
    _trayIconMenu->addAction(_showStoreAction);
    _trayIconMenu->addAction(_showFeedbackPageAction);

#ifdef Q_OS_WIN
    _trayIconMenu->addSeparator();
    _trayIconMenu->addAction(_quitAction);
#endif

#ifdef Q_OS_MAC
    // show the tray icon in the Dock
    qt_mac_set_dock_menu(_trayIconMenu);
#endif

#ifdef Q_OS_WIN
    // show tray icon in system-tray area
    _trayIcon = new QSystemTrayIcon(parentWidget);
    _trayIcon->setToolTip(ConfigManager::getClientTrayIconTooltipText());
    _trayIcon->setContextMenu(_trayIconMenu);

    // mouse event handler
    connect(_trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
    connect(_trayIcon, SIGNAL(messageClicked()), this, SLOT(messageClicked()));

    // setup resources - set the tray-icon
    _trayIcon->setIcon(QIcon(":/system_tray_icon"));

    _trayIcon->show();
#endif
}

// Q_SLOT
void SystemTrayIconDelegate::messageClicked()
{
    qDebug("system tray message clicked");
}

void SystemTrayIconDelegate::showSystemTrayMessage(QString message, QString title)
{
    QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon(QSystemTrayIcon::Information);

    //QSettings settings;
    int showMessageDurationMilliseconds = 5000;
    //int t = settings.value("showMessageDurationMilliseconds").toInt();
    int t = showMessageDurationMilliseconds;
    if(t != 0) {
        showMessageDurationMilliseconds = t;
    }

    if(_trayIcon == NULL) {
        WLog("_trayIcon is null");
    } else {
        _trayIcon->showMessage(title, message, icon, showMessageDurationMilliseconds);
    }
}

void SystemTrayIconDelegate::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
//        trayIcon->show();
//        trayIconMenu->showNormal();
//        trayIconMenu->exec();
//        trayIconMenu->hide();
//        trayIconMenu->show();
#ifdef Q_WS_WIN
//        MyApplication::instance()->postEventToBrowserWindow(new EnsureApplicationPresentedEvent());
        Q_EMIT showBrowserInCurrentState();
#endif
        break;
    case QSystemTrayIcon::DoubleClick:
// TODO: behaviour from settings
#ifdef Q_WS_WIN
//        QApplication::postEvent(this->parent(), new EnsureApplicationPresentedEvent());
        //MyApplication::instance()->postEventToLauncherWindow(new EnsureApplicationPresentedEvent());
//        MyApplication::instance()->postEventToBrowserWindow(new EnsureApplicationPresentedEvent());
        Q_EMIT showBrowserInCurrentState();
#endif
        break;
    case QSystemTrayIcon::MiddleClick:
        break;
    default:
        ;
    }
}
