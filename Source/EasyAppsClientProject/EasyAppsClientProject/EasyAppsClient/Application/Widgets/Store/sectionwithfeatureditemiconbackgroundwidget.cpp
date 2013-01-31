#include "sectionwithfeatureditemiconbackgroundwidget.h"

#include <QStyleOption>
#include <QPainter>

#include "Common/Helpers/qpixmaphelper.h"

SectionWithFeaturedItemIconBackgroundWidget::SectionWithFeaturedItemIconBackgroundWidget(QWidget *parent) :
    QWidget(parent)
{
}

void SectionWithFeaturedItemIconBackgroundWidget::setBackgroundIconFromResource(QByteArray pixmapData) {
    QPixmap pixmap = QPixmapHelper::createPixmapFromByteArray(pixmapData);
    this->setBackgroundIconPixmap(pixmap);
}

void SectionWithFeaturedItemIconBackgroundWidget::setBackgroundIconPixmap(QPixmap pixmap) {
    this->_backgroundIconPixmap = pixmap;
    this->update();
}

void SectionWithFeaturedItemIconBackgroundWidget::paintEvent(QPaintEvent *e) {
    // must be provided for widgets inherits directly from QWidget if you want to style it with css
    //  check QWidget section here: http://doc.qt.nokia.com/4.7-snapshot/stylesheet-reference.html

    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    if(!_backgroundIconPixmap.isNull() && _backgroundIconPixmap.size().height() > 0) {
        int pixmapHeight = _backgroundIconPixmap.size().height();

        int posY = this->rect().height() - (pixmapHeight / 2);
        if(posY < 5) {
            posY = 5;
        }
        int pixmapDrawHeight = this->contentsRect().height() - posY - 1; // -1 to don't draw over the border
//        DLog("Pixmap draw height: ") << pixmapDrawHeight;
//        DLog("Pixmap size: ") << _backgroundIconPixmap.size();
        if(pixmapDrawHeight > (pixmapHeight - 1)) { // -1 to don't draw over the border
            pixmapDrawHeight = pixmapHeight - 1;
        }
//        DLog("Pixmap draw height: ") << pixmapDrawHeight;
        p.drawPixmap(50, posY, _backgroundIconPixmap.size().width(), pixmapDrawHeight, _backgroundIconPixmap, 0, 0, _backgroundIconPixmap.size().width(), pixmapDrawHeight);
    }
}
