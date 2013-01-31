#ifndef GOODBYEVIEWCONTROLLER_H
#define GOODBYEVIEWCONTROLLER_H

#include "viewcontrollerinterface.h"

class GoodbyeView;
class UninstallConfigs;
class GoodbyeViewController : public ViewControllerInterface
{
    Q_OBJECT
public:
    explicit GoodbyeViewController(GoodbyeView *view, UninstallConfigs *uninstallConfigs, QObject *parent = 0);

    virtual QWidget *getView() const;
    virtual void destroyView();

Q_SIGNALS:
    void quitFromUninstaller();

private Q_SLOTS:
    void openThisFolder(QString folderPath);

private:
    GoodbyeView *_view;
    UninstallConfigs *_uninstallConfigs;
};

#endif // GOODBYEVIEWCONTROLLER_H
