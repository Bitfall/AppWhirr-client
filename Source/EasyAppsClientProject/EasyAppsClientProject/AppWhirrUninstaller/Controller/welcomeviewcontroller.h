#ifndef WELCOMEVIEWCONTROLLER_H
#define WELCOMEVIEWCONTROLLER_H

#include <QObject>

#include "viewcontrollerinterface.h"
#include "../View/welcomeview.h"
#include "../uninstallconfigs.h"

class WelcomeViewController : public ViewControllerInterface
{
    Q_OBJECT
public:    
    explicit WelcomeViewController(WelcomeView *view, UninstallConfigs *uninstallConfigs, QObject *parent = 0) :
        ViewControllerInterface(parent),
        _view(view),
        _uninstallConfigs(uninstallConfigs)
    {
        connect(_view, SIGNAL(startUninstall()), this, SIGNAL(startUninstall()));
        connect(_view, SIGNAL(isRemoveAppsAsWellCheckboxValueChanged(bool)), this, SLOT(isRemoveAppsAsWellCheckboxValueChanged(bool)));
        connect(_view, SIGNAL(isRemoveAppWhirrAndLocalDataCheckboxValueChanged(bool)), this, SLOT(isRemoveAppWhirrAndLocalDataCheckboxValueChanged(bool)));

        _view->setIsRemoveAppWhirrAndLocalDataCheckboxChecked(_uninstallConfigs->getIsRemoveAppWhirrAndLocalData());
        _view->setIsRemoveInstalledAppsCheckboxChecked(_uninstallConfigs->getIsRemoveInstalledAppsAsWell());
    }

    virtual QWidget *getView() const {
        return _view;
    }

    virtual void destroyView() {
        AW_QOBJECT_SAFE_DELETE( _view );
    }

Q_SIGNALS:
    void startUninstall();

private Q_SLOTS:
    void isRemoveAppsAsWellCheckboxValueChanged(bool newValue) {
        _uninstallConfigs->setIsRemoveInstalledAppsAsWell(newValue);
        this->checkAndUpdateValidity();
    }
    void isRemoveAppWhirrAndLocalDataCheckboxValueChanged(bool newValue) {
        _uninstallConfigs->setIsRemoveAppWhirrAndLocalData(newValue);
        this->checkAndUpdateValidity();
    }

private:
    void checkAndUpdateValidity() {
        if( !_uninstallConfigs->getIsRemoveAppWhirrAndLocalData() && !_uninstallConfigs->getIsRemoveInstalledAppsAsWell() ) {
            _view->setStartUninstallButtonEnabled(false);
        }
        else {
            _view->setStartUninstallButtonEnabled(true);
        }
    }
    
private:
    WelcomeView *_view;
    UninstallConfigs *_uninstallConfigs;
};

#endif // WELCOMEVIEWCONTROLLER_H
