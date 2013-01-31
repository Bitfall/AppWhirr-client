#include "goodbyeview.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpacerItem>

#include "../guiconfigurationmanagersingleton.h"

GoodbyeView::GoodbyeView(QWidget *parent) :
    QWidget(parent)
{
    GuiConfigurationManager *guiConf = GuiConfigurationManagerSingleton::sharedGuiConfigurationManager();

    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    QLabel *titleLabel = new QLabel(tr("Finished!"));
    mainLayout->addWidget(titleLabel);

//    mainLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));

    mainLayout->addSpacing(15);

    _errorFoundLabel = new QLabel;
    mainLayout->addWidget(_errorFoundLabel);

    QWidget *errorListContainerWidget = new QWidget;
    _errorListLayout = new QVBoxLayout;
    errorListContainerWidget->setLayout(_errorListLayout);
    mainLayout->addWidget(errorListContainerWidget);

    mainLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));

    QPushButton *quitFromUninstallerButton = new QPushButton(tr("Quit"));
    connect(quitFromUninstallerButton, SIGNAL(clicked()), this, SIGNAL(quitFromUninstallerButtonClicked()));
    mainLayout->addWidget(quitFromUninstallerButton);

    //
    // style
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet(guiConf->getStyleForId("Label/Large/DarkGray/style"));
    _errorFoundLabel->setStyleSheet(guiConf->getStyleForId("Label/Default/DarkGray/style"));
    quitFromUninstallerButton->setStyleSheet(guiConf->getStyleForId("Button/Default/NormalPriority/style"));
    quitFromUninstallerButton->setFlat(true);
    quitFromUninstallerButton->setFixedHeight(guiConf->getIntForId("Size/Default/SingleLineWidget/fixHeight"));
}

QVBoxLayout *GoodbyeView::getErrorListLayout() const
{
    return _errorListLayout;
}

void GoodbyeView::setErrorFoundText(QString value)
{
    _errorFoundLabel->setText(value);
}
