#include "welcomeview.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QTextEdit>
#include <QLabel>

#include "../EasyAppsClient/Common/Widgets/checkboxbutton.h"
#include "../guiconfigurationmanagersingleton.h"

WelcomeView::WelcomeView(QWidget *parent) :
    QWidget(parent)
{
    GuiConfigurationManager *guiConf = GuiConfigurationManagerSingleton::sharedGuiConfigurationManager();

    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    QLabel *titleLabel = new QLabel(tr("AppWhirr: Uninstall"));
    mainLayout->addWidget(titleLabel);

    QTextEdit *descriptionTextEdit = new QTextEdit;
    QString descriptionTextString = tr(
                "You can uninstall the AppWhirr app and it's local data files (logs, setting, caches, etc.) "
                "or every app you've intalled in AppWhirr or both.\n"
                "\n"
                "Important: the uninstaller is not able to remove files which are open in an app and also it cannot remove running apps "
                "so you should "
                "quit from every app you've installed in AppWhirr first and close AppWhirr if it's still running.\n"
                "\n"
                "The uninstaller will try to remove as much files and apps as it can. If it fails to remove "
                "an app or file then it will present you a list of folders it could not delete and you will be able to "
                "delete these files and folders yourself.\n"
                );
    descriptionTextEdit->setText(descriptionTextString);
    descriptionTextEdit->setReadOnly(true);
    mainLayout->addWidget(descriptionTextEdit);

    QLabel *componentSelectorLabel = new QLabel(tr("Select the components you want to remove:"));
    mainLayout->addWidget(componentSelectorLabel);

    _removeAppWhirrAndLocalDataCheckboxButton = new CheckboxButton(tr("Remove AppWhirr and AppWhirr's local data (logs, settings, etc)."));
    connect(_removeAppWhirrAndLocalDataCheckboxButton, SIGNAL(toggled(bool)), this, SIGNAL(isRemoveAppWhirrAndLocalDataCheckboxValueChanged(bool)));
    mainLayout->addWidget(_removeAppWhirrAndLocalDataCheckboxButton);

    _removeAppsAsWellCheckboxButton = new CheckboxButton(tr("Remove every apps installed in AppWhirr."));
    connect(_removeAppsAsWellCheckboxButton, SIGNAL(toggled(bool)), this, SIGNAL(isRemoveAppsAsWellCheckboxValueChanged(bool)));
    mainLayout->addWidget(_removeAppsAsWellCheckboxButton);

    _startUninstallButton = new QPushButton(tr("Start the Uninstall"));
    connect(_startUninstallButton, SIGNAL(clicked()), this, SIGNAL(startUninstall()));
    mainLayout->addWidget(_startUninstallButton);

    //
    // style
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet(guiConf->getStyleForId("Label/Large/DarkGray/style"));
    descriptionTextEdit->setStyleSheet(guiConf->getStyleForId("TextEdit/NoBorder/LightGray/style"));
    componentSelectorLabel->setStyleSheet(guiConf->getStyleForId("Label/DarkDarkGray/Small/style"));
    _removeAppWhirrAndLocalDataCheckboxButton->setStyleSheet(guiConf->getStyleForId("CheckboxButton/Default/Gray/style"));
    _removeAppWhirrAndLocalDataCheckboxButton->setFixedHeight(guiConf->getIntForId("Size/Default/SingleLineWidget/fixHeight"));
    _removeAppsAsWellCheckboxButton->setStyleSheet(guiConf->getStyleForId("CheckboxButton/Default/Gray/style"));
    _removeAppsAsWellCheckboxButton->setFixedHeight(guiConf->getIntForId("Size/Default/SingleLineWidget/fixHeight"));
    _startUninstallButton->setStyleSheet(guiConf->getStyleForId("Button/DangerousAction/style"));
    _startUninstallButton->setFlat(true);
    _startUninstallButton->setFixedHeight(guiConf->getIntForId("Size/Default/SingleLineWidget/fixHeight"));
}

void WelcomeView::setIsRemoveAppWhirrAndLocalDataCheckboxChecked(bool value)
{
    _removeAppWhirrAndLocalDataCheckboxButton->setChecked(value);
}

void WelcomeView::setIsRemoveInstalledAppsCheckboxChecked(bool value)
{
    _removeAppsAsWellCheckboxButton->setChecked(value);
}

void WelcomeView::setStartUninstallButtonEnabled(bool value)
{
    _startUninstallButton->setEnabled(value);
}
