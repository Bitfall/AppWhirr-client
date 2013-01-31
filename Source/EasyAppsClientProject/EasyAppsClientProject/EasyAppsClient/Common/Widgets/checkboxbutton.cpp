#include "checkboxbutton.h"

#include <QLabel>
#include <QCheckBox>
#include <QSpacerItem>
#include <QHBoxLayout>

#include <QPaintEvent>
#include <QPainter>
#include <QStyleOption>

#include <QEvent>
#include <QMouseEvent>

CheckboxButton::CheckboxButton(QWidget *parent) :
    QWidget(parent)
{
    this->__init();
}

CheckboxButton::CheckboxButton(QString titleText, QWidget *parent) :
    QWidget(parent)
{
    this->__init(titleText);
}

void CheckboxButton::__init(QString titleText)
{
//    _titleText = titleText;

    QHBoxLayout *mainLayout = new QHBoxLayout;
    this->setLayout(mainLayout);

    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(5);
    this->setContentsMargins(0, 0, 2, 0); // right-margin required, if 0 right margin is set it can cause visual artifacts

    _titleLabel = new QLabel(titleText);
    mainLayout->addWidget(_titleLabel);

    mainLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));

    _checkbox = new QCheckBox("");
    mainLayout->addWidget(_checkbox);

    this->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
//    this->setObjectName(getDefaultObjectName());

    connect(_checkbox, SIGNAL(toggled(bool)), this, SIGNAL(toggled(bool)));
    connect(_checkbox, SIGNAL(clicked()), this, SIGNAL(clicked()));
    connect(_checkbox, SIGNAL(pressed()), this, SIGNAL(pressed()));
    connect(_checkbox, SIGNAL(released()), this, SIGNAL(released()));
}

void CheckboxButton::paintEvent(QPaintEvent *e)
{
    // must be provided for widgets inherits directly from QWidget if you want to style it with css
    //  check QWidget section here: http://doc.qt.nokia.com/4.7-snapshot/stylesheet-reference.html

    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

//    QStyleOptionButton option;
//    option.initFrom(this);
//    option.state = _checkbox->isChecked() ? QStyle::State_Sunken : QStyle::State_Raised;
////    if (isDefault())
////        option.features |= QStyleOptionButton::DefaultButton;
//    option.text = _titleText;

//    QPainter painter(this);
//    style()->drawControl(QStyle::CE_PushButton, &option, &painter, this);
}

void CheckboxButton::enterEvent(QEvent *e)
{
    QWidget::enterEvent(e);
}

void CheckboxButton::leaveEvent(QEvent *e)
{
    QWidget::leaveEvent(e);
}

void CheckboxButton::mousePressEvent(QMouseEvent *e)
{
    Q_EMIT pressed();
    QWidget::mousePressEvent(e);
}

void CheckboxButton::mouseMoveEvent(QMouseEvent *e)
{
    QWidget::mouseMoveEvent(e);
}

void CheckboxButton::mouseReleaseEvent(QMouseEvent *e)
{
    DLogS << " -- mouse relese";
    _checkbox->setChecked(!_checkbox->isChecked());
    Q_EMIT released();
    Q_EMIT clicked();
    QWidget::mouseReleaseEvent(e);
}

void CheckboxButton::setTitleText(QString titleText)
{
    _titleLabel->setText(titleText);
//    _titleText = titleText;
}

//void CheckboxButton::setCheckboxChecked(bool isChecked)
//{
//    _checkbox->setChecked(isChecked);
//}

void CheckboxButton::setChecked(bool isChecked)
{
    _checkbox->setChecked(isChecked);
}

bool CheckboxButton::getIsChecked() const {
    return _checkbox->isChecked();
}
