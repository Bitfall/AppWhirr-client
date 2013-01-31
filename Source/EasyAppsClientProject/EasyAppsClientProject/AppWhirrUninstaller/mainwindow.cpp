#include "mainwindow.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QApplication>

#include "Controller/viewcontrollerinterface.h"

#include "View/welcomeview.h"
#include "Controller/welcomeviewcontroller.h"
#include "View/uninstallingview.h"
#include "Controller/uninstallingviewcontroller.h"
#include "View/goodbyeview.h"
#include "Controller/goodbyeviewcontroller.h"

#include "uninstallconfigs.h"

#include "guiconfigurationmanagersingleton.h"

#include <QDir>
#include "../EasyAppsClient/Application/appspecificpathhelper.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      _currentViewController(NULL)
{
    QWidget *mainWidget = new QWidget;
    this->setCentralWidget(mainWidget);

    _mainLayout = new QVBoxLayout;
    mainWidget->setLayout(_mainLayout);

    this->setMinimumSize(600, 400);
    this->resize(600, 400);

    _uninstallConfigs = new UninstallConfigs(this);
    this->processAppArgsAndFillTheUninstallConfigs(_uninstallConfigs);

    DLog("Given AW client dir path: ") << _uninstallConfigs->getAppWhirrClientAppFolderPath();

    GuiConfigurationManagerSingleton::setSharedGuiConfigurationManager(new GuiConfigurationManager(":/AWGuiConfig"));

#ifdef Q_OS_WIN
    // force set the current / working dir to the Uninstaller's dir (in system temp)
    QDir::setCurrent(QCoreApplication::applicationDirPath());
#endif

    this->switchToWelcomeScreen();
}

MainWindow::~MainWindow()
{
    
}

void MainWindow::switchToWelcomeScreen()
{
    WelcomeViewController *vc = new WelcomeViewController(new WelcomeView, _uninstallConfigs, this);
    connect(vc, SIGNAL(startUninstall()), this, SLOT(switchToUninstallingScreen()));
    this->switchFromCurrentViewAndControllerToThisOne(vc);
}

void MainWindow::switchToUninstallingScreen()
{
    UninstallingViewController *vc = new UninstallingViewController(new UninstallingView, _uninstallConfigs, this);
    connect(vc, SIGNAL(finishedWithUninstall()), this, SLOT(switchToGoodbyeScreen()));
    this->switchFromCurrentViewAndControllerToThisOne(vc);
    // start uninstalling
    vc->startUninstall();
}

void MainWindow::switchToGoodbyeScreen()
{
    GoodbyeViewController *vc = new GoodbyeViewController(new GoodbyeView, _uninstallConfigs, this);
    connect(vc, SIGNAL(quitFromUninstaller()), this, SLOT(quitFromUninstaller()));
    this->switchFromCurrentViewAndControllerToThisOne(vc);
}

void MainWindow::quitFromUninstaller()
{
    qApp->quit();
}

void MainWindow::setThisViewAndControllerAsCurrent(ViewControllerInterface *viewController)
{
    _currentViewController = viewController;
    _mainLayout->addWidget(viewController->getView());
}

void MainWindow::removeAndDestroyCurrentViewAndController()
{
    if( _currentViewController != NULL ) {
        // remove view from layout
        _mainLayout->removeWidget(_currentViewController->getView());
        // destroy the view
        _currentViewController->destroyView();
        // and delete the view-controller as well
        AW_QOBJECT_SAFE_DELETE( _currentViewController );
    }
}

void MainWindow::switchFromCurrentViewAndControllerToThisOne(ViewControllerInterface *viewController)
{
    this->removeAndDestroyCurrentViewAndController();
    this->setThisViewAndControllerAsCurrent(viewController);
}

void MainWindow::processAppArgsAndFillTheUninstallConfigs(UninstallConfigs *uninstallConfigs)
{
    bool isClientRootArgNext = false;
    Q_FOREACH(QString argStr, qApp->arguments())
    {
        if(isClientRootArgNext) {
            uninstallConfigs->setAppWhirrClientAppFolderPath(argStr);
            isClientRootArgNext = false;
        }
        else if(argStr == QString("clientRootPath")) {
            isClientRootArgNext = true;
        }
    }
}
