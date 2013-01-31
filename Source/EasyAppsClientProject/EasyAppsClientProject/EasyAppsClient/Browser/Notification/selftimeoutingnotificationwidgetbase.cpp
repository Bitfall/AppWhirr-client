#include "selftimeoutingnotificationwidgetbase.h"

#include <QStyleOption>
#include <QPainter>

void SelfTimeoutingNotificationWidgetBase::paintEvent(QPaintEvent *e) {
    // must be provided for widgets inherits directly from QWidget if you want to style it with css
    //  check QWidget section here: http://doc.qt.nokia.com/4.7-snapshot/stylesheet-reference.html

    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void SelfTimeoutingNotificationWidgetBase::setOptionalId(QString id) {
    this->_optionalId = id;
}

QString SelfTimeoutingNotificationWidgetBase::getOptionalId() const {
    return this->_optionalId;
}
