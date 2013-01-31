#include "goodbyeerrorlistitemwidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpacerItem>

#include "../guiconfigurationmanagersingleton.h"

GoodbyeErrorListItemWidget::GoodbyeErrorListItemWidget(QString errorText, QString relatedFolderPath, QWidget *parent) :
    QWidget(parent),
    _errorText(errorText),
    _relatedFolderPath(relatedFolderPath)
{
    GuiConfigurationManager *guiConf = GuiConfigurationManagerSingleton::sharedGuiConfigurationManager();

    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    QHBoxLayout *contentLayout = new QHBoxLayout;
    mainLayout->addLayout(contentLayout);

    QLabel *errorTextLabel = new QLabel(errorText);
    contentLayout->addWidget(errorTextLabel);
    contentLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));

#ifdef Q_OS_WIN
    QPushButton *openThisFolderButton = new QPushButton(tr("Open folder in Explorer"));
#endif
#ifdef Q_OS_MAC
    QPushButton *openThisFolderButton = new QPushButton(tr("Open folder in Finder"));
#endif
    connect(openThisFolderButton, SIGNAL(clicked()), this, SLOT(openThisFolderButtonClicked()));
    contentLayout->addWidget(openThisFolderButton);

    if( relatedFolderPath.isEmpty() ) {
        openThisFolderButton->hide();
    }

    //
    // style
    errorTextLabel->setStyleSheet(guiConf->getStyleForId("Label/Default/MediumGray/style"));
    openThisFolderButton->setStyleSheet(guiConf->getStyleForId("Button/Default/NormalPriority/style"));
    openThisFolderButton->setFlat(true);
    openThisFolderButton->setFixedHeight(guiConf->getIntForId("Size/Default/SingleLineWidget/fixHeight"));
}

void GoodbyeErrorListItemWidget::openThisFolderButtonClicked()
{
    Q_EMIT openThisFolder(_relatedFolderPath);
}
