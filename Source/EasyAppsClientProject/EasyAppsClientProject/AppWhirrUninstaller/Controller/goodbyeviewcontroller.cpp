#include "goodbyeviewcontroller.h"

#include "../View/goodbyeview.h"
#include "../View/goodbyeerrorlistitemwidget.h"

#include "../uninstallconfigs.h"
#include "../EasyAppsClient/Application/appspecificpathhelper.h"

#include <QVBoxLayout>

#include <QDesktopServices>
#include <QUrl>

GoodbyeViewController::GoodbyeViewController(GoodbyeView *view, UninstallConfigs *uninstallConfigs, QObject *parent) :
    ViewControllerInterface(parent),
    _view(view),
    _uninstallConfigs(uninstallConfigs)
{
    connect(_view, SIGNAL(quitFromUninstallerButtonClicked()), this, SIGNAL(quitFromUninstaller()));

    if(uninstallConfigs->getFailedUninstallSteps().isEmpty()) {
        _view->setErrorFoundText(tr("The uninstaller finished and removed every file successfully."));
    }
    else
    {
        _view->setErrorFoundText(tr("The uninstaller finished but could not remove some files/folders:"));

        Q_FOREACH(UninstallDeleteSteps::UninstallDeleteStepsEnum failedStep, uninstallConfigs->getFailedUninstallSteps())
        {
            QVBoxLayout *errorListLayout = _view->getErrorListLayout();
            if( failedStep == UninstallDeleteSteps::RemoveAppWhirrClientFiles ) {
                GoodbyeErrorListItemWidget *listWidget = new GoodbyeErrorListItemWidget(tr("Could not remove the AppWhirr client app."), _uninstallConfigs->getAppWhirrClientAppFolderPath());
                connect(listWidget, SIGNAL(openThisFolder(QString)), this, SLOT(openThisFolder(QString)));
                errorListLayout->addWidget(listWidget);
            }
            else if( failedStep == UninstallDeleteSteps::RemoveUserApps ) {
                GoodbyeErrorListItemWidget *listWidget = new GoodbyeErrorListItemWidget(tr("Could not remove some installed apps."), AppSpecificPathHelper::getLocalUsersDirectoryPath());
                connect(listWidget, SIGNAL(openThisFolder(QString)), this, SLOT(openThisFolder(QString)));
                errorListLayout->addWidget(listWidget);
            }
            else if( failedStep == UninstallDeleteSteps::RemoveAppWhirrLocalFiles ) {
                GoodbyeErrorListItemWidget *listWidget = new GoodbyeErrorListItemWidget(tr("Could not remove some local files."), AppSpecificPathHelper::getLocalRootDirectory());
                connect(listWidget, SIGNAL(openThisFolder(QString)), this, SLOT(openThisFolder(QString)));
                errorListLayout->addWidget(listWidget);
            }
            else {
                WLog("Invalid step type");
                errorListLayout->addWidget(new GoodbyeErrorListItemWidget(tr("Unknown error."), QString("")));
            }
        }
    }
}

QWidget *GoodbyeViewController::getView() const
{
    return _view;
}

void GoodbyeViewController::destroyView()
{
    AW_QOBJECT_SAFE_DELETE( _view );
}

void GoodbyeViewController::openThisFolder(QString folderPath)
{
    QDesktopServices::openUrl( QUrl( QString("file:///%1").arg(folderPath), QUrl::TolerantMode ) );
}
