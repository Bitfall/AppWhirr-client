#include "incontentnavigationwidget.h"

#include <QHBoxLayout>
#include <QPushButton>

#include <QStyleOption>
#include <QPainter>

#include "Application/globalobjectregistry.h"
#include "Common/guiconfigurationmanager.h"

InContentNavigationWidget::InContentNavigationWidget(QString backButtonText, QWidget *parent) :
    QWidget(parent)
{
    GuiConfigurationManager *guiConfManager = GlobalObjectRegistry::sharedGlobalObjectRegistry()->getGuiConfigurationManager();

    QHBoxLayout *mainLayout = new QHBoxLayout;
    this->setLayout(mainLayout);

    this->setStyleSheet(guiConfManager->getStyleForId("Default/InContentNavigationWidget/style"));

    this->setContentsMargins(0, 0, 0, 0);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    this->setFixedHeight(40);

    QPushButton *backButton = new QPushButton;
    if(backButtonText.isEmpty()) {
        backButton->setText(tr("Back"));
    } else {
        backButton->setText(backButtonText);
    }
    connect(backButton, SIGNAL(clicked()), this, SIGNAL(backButtonClicked()));
    backButton->setMinimumSize(100, 30);
    backButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    backButton->setStyleSheet(guiConfManager->getStyleForId("Button/Default/LowPriority/style"));
    backButton->setFlat(true);
    mainLayout->addWidget(backButton);

    mainLayout->addSpacerItem(new QSpacerItem(5, 5, QSizePolicy::Expanding));
}

void InContentNavigationWidget::paintEvent(QPaintEvent *e)
{
    // must be provided for widgets inherits directly from QWidget if you want to style it with css
    //  check QWidget section here: http://doc.qt.nokia.com/4.7-snapshot/stylesheet-reference.html

    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
