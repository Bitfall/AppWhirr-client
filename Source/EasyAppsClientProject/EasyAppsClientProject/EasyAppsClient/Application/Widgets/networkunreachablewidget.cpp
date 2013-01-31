#include "networkunreachablewidget.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

NetworkUnreachableWidget::NetworkUnreachableWidget(QWidget *parent) :
    FadingWidgetBase(1000, parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    _contentHolderWidget = new QWidget;
    QVBoxLayout *contentLayout = new QVBoxLayout;
    _contentHolderWidget->setLayout(contentLayout);
    mainLayout->addWidget(_contentHolderWidget);

    QLabel *messageLabel = new QLabel(tr("Server unreachable."));
    contentLayout->addWidget(messageLabel);

    QPushButton *retryButton = new QPushButton(tr("Retry"));
    connect(retryButton, SIGNAL(clicked()), this, SIGNAL(retry()));
    contentLayout->addWidget(retryButton);

    QPushButton *okButton = new QPushButton(tr("OK"));
    connect(okButton, SIGNAL(clicked()), this, SIGNAL(ok()));
    contentLayout->addWidget(okButton);
}

void NetworkUnreachableWidget::paintEvent(QPaintEvent *e) {
    FadingWidgetBase::paintEvent(e);
}

void NetworkUnreachableWidget::hideContentForFadeAnimation() {
    _contentHolderWidget->hide();
}

void NetworkUnreachableWidget::showContentForFadeAnimation() {
    _contentHolderWidget->show();
}

void NetworkUnreachableWidget::preserveCurrentSize() {
    this->setFixedSize(this->size());
}

void NetworkUnreachableWidget::dynamicSizeAllowed() {
    this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}
