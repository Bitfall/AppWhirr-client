#ifndef MYAPPSMANAGEMENTSIDEBAR_H
#define MYAPPSMANAGEMENTSIDEBAR_H

#include "Common/Widgets/animatedtwofixedsizewidgetbase.h"

#include "Application/ApplicationDescription/applicationdescriptionmodel.h"
#include "Application/ApplicationDescription/applicationstatesenum.h"

QT_BEGIN_NAMESPACE
class QLabel;
class QPushButton;
class QCheckBox;
QT_END_NAMESPACE

// UI
class SimpleImagePresenterWidget;
class CheckboxButton;

class MyAppsManagementSidebar : public AnimatedTwoFixedSizeWidgetBase
{
    Q_OBJECT
public:
    explicit MyAppsManagementSidebar(QSize collapsedSize, QSize expandedSize, bool isExpandedByDefault, QWidget *parent = 0);

    void setContent(ApplicationDescriptionModel appDescriptionModel, QPixmap mainImagePixmap);
//    void refreshCurrentContent();
    QString getRelatedAppId() const;

Q_SIGNALS:
//    void mainAppStateDependentButtonClicked(QString appId);
    void runThisApp(ApplicationDescriptionModel);
    void installThisApp(ApplicationDescriptionModel);
//    void updateThisApp(ApplicationDescriptionModel);

    void showAppDetails(ApplicationDescriptionModel);
    void removeThisApp(ApplicationDescriptionModel);
    void createDesktopShortcutForThisApp(ApplicationDescriptionModel);

    void isStartThisAppWhenClientStartsChangedSignal(QString appId, bool newValue);
    void isThisAppHiddenChangedSignal(QString appId, bool newValue);

private Q_SLOTS:

    void _mainAppStateDependentButtonClickedSlot();
    void _updateButtonClickedSlot();
    void _showAppDetailsButtonClickedSlot();
    void _removeThisAppButtonClickedSlot();
    void _createDesktopShortcutForThisAppButtonClickedSlot();

    void _autoStartThisAppToggled(bool newValue);
    void _isHideThisAppToggled(bool newValue);
    void suggestUpdateAvailable();

    // app state changes
    void appStateChanged(QString appId, AppWhirr::ApplicationGrabStates::ApplicationGrabStatesEnum newAppState);
    void wholeAppDescriptionChanged(ApplicationDescriptionModel inputAppDescriptionModel, ApplicationDescriptionModel updatedAppDescriptionModel);

private:
    void _refreshByCurrentAppDescription(QPixmap preferredImagePixmap);

private:
    QLabel *_titleLabel;
    SimpleImagePresenterWidget *_imagePresenter;
    QPushButton *_updateAppButton;
    QPushButton *_runAppButton;
    QPushButton *_removeAppButton;
    QPushButton *_showAppDetailsButton;
    QPushButton *_suggestUpdateAvailableButton;
    CheckboxButton *_autostartCheckbox;
    CheckboxButton *_isHiddenCheckbox;
    QPushButton *_createDesktopShortcutButton;

//    QString _currAppId;
//    AppWhirr::ApplicationStates::ApplicationStatesEnum _currAppState;
    ApplicationDescriptionModel _appDescriptionModel;
};

#endif // MYAPPSMANAGEMENTSIDEBAR_H
