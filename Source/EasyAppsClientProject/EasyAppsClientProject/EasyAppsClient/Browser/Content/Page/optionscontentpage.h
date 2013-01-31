#ifndef OPTIONSCONTENTPAGE_H
#define OPTIONSCONTENTPAGE_H

#include "Common/Page/pagewidgetbase.h"

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QCheckBox;
class QSignalMapper;
QT_END_NAMESPACE

class MenuItemWidget;
class CheckboxButton;

class OptionsContentPage : public PageWidgetBase
{
    Q_OBJECT
public:
    explicit OptionsContentPage(QString pageId, QWidget *parent = 0);

    //
    // for more information see the interface's documentation (in it's header file)
    void initializePageContent();
    void pagePushWillHappen();
    void pageWillAppear(QString param);
    void pageDidAppear();
    void pagePopWillHappen();
    void pageWillDisappear();

public Q_SLOTS:
    void relatedMenuItemActivated(QString menuItemId);

private Q_SLOTS:
    void presentSuggestAnAppPage(QString menuItemId);
    void presentFeedbackPage(QString menuItemId);
    void presentWhatsNewPage(QString menuItemId);
    void presentAppwhirrLicenceMenuItem(QString menuItemId);

    void checkBoxChanged(QWidget* value);
    void _createDesktopShortcutForClient();
    void switchUserButtonPressed();
    void deleteLocalDataButtonPressed();
    void deleteCacheButtonPressed();
    void quitButtonPressed();
    void logOffUserButtonPressed();

    void _doDeleteLocalData();

    void uninstallAppWhirrButtonClicked();

private:
    QVBoxLayout *mainLayout;
    QSignalMapper* signalMapper;

    CheckboxButton *_autoStartupCheckbox;
    CheckboxButton* _globalHotkeyToLauncherCheckbox;
//    CheckboxButton* _showLauncherAtStartupCheckbox;
};

#endif // OPTIONSCONTENTPAGE_H
