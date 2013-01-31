#include "busyindicatorwidget.h"

#include <QPaintEvent>
#include <QPainter>
#include <QTimer>

// config
#define CENTER_HIGHLIGHTED_AREA_TRASHOLD_PIXELS 20
#define UI_UPDATE_FPS 30

BusyIndicatorWidget::BusyIndicatorWidget(QPixmap busyIndicatorImage, int showHideAnimationMilliseconds, QWidget *parent) :
    QWidget(parent),
    _originalBusyIndicatorImage(busyIndicatorImage),
    _showHideAnimationMilliseconds(showHideAnimationMilliseconds),
    _fadeTransparency(0)
{
    this->setAttribute(Qt::WA_NoSystemBackground, true);

    _uiUpdateTimer = new QTimer(this);
    _uiUpdateTimer->setSingleShot(false);
    _uiUpdateTimer->setInterval(1000 / UI_UPDATE_FPS);
    connect(_uiUpdateTimer, SIGNAL(timeout()), this, SLOT(doBusyStep()));
}

void BusyIndicatorWidget::paintEvent(QPaintEvent *e)
{
//    if(_isFullRepaintNeeded) {
//        this->_refreshPresentablePixmapByRenderImage();
//        this->_isFullRepaintNeeded = false;
//    }

//    DLog("Paint with fade-transparency: ") << _fadeTransparency;

    if(this->rect().size().width() <= 0 || this->rect().size().height() <= 0) {
        WLog("Invalid rect size (zero or smaller)");
        return;
    }

    if(_originalBusyIndicatorImage.isNull() || _originalBusyIndicatorImage.width() <= 0 || _originalBusyIndicatorImage.height() <= 0) {
        WLog("Invalid busy-indicator-image size.");
        return;
    }

    QPainter painter(this);

    painter.setOpacity(_fadeTransparency);
    // background
    painter.fillRect(this->rect(), QColor(200, 200, 200, 100));
    painter.setRenderHints(QPainter::SmoothPixmapTransform);

    // center, highlighted area
    QRectF centerHighlightedAreaRect(((float)this->rect().size().width() / 2.0f) - ((float)_originalBusyIndicatorImage.width() / 2.0f) - CENTER_HIGHLIGHTED_AREA_TRASHOLD_PIXELS, // x
                                    ((float)this->rect().size().height() / 2.0f) - ((float)_originalBusyIndicatorImage.height() / 2.0f) - CENTER_HIGHLIGHTED_AREA_TRASHOLD_PIXELS, // y
                                    (2.0f * CENTER_HIGHLIGHTED_AREA_TRASHOLD_PIXELS) + _originalBusyIndicatorImage.width(), // width
                                    (2.0f * CENTER_HIGHLIGHTED_AREA_TRASHOLD_PIXELS) + _originalBusyIndicatorImage.height() // height
                                    );
    QPainterPath path;
    path.addRoundedRect(centerHighlightedAreaRect, 10, 10);
    painter.fillPath(path, QColor(170, 170, 170, 120));
//    painter.fillRect(centerHighlightedAreaRect, QColor(200, 200, 200, 60));
    painter.setRenderHints(QPainter::SmoothPixmapTransform);

    if(!_originalBusyIndicatorImage.isNull() && _originalBusyIndicatorImage.size().width() > 0 && _originalBusyIndicatorImage.size().height() > 0)
    {
        int posX = (this->rect().size().width() / 2);// - (_originalBusyIndicatorImage.width() / 2);
        int posY = (this->rect().size().height() / 2);// - (_originalBusyIndicatorImage.height() / 2);

        painter.save();
        painter.translate(posX, posY);
        painter.rotate(_currRotationValue);
        painter.drawPixmap(-(_originalBusyIndicatorImage.width() / 2), -(_originalBusyIndicatorImage.height() / 2), _originalBusyIndicatorImage);
//        painter.drawPixmap(posX, posY, _originalBusyIndicatorImage.width(), _originalBusyIndicatorImage.height(), _transformedPixmapToPresent);
        painter.restore();
    }
    else {
        WLog("Invalid busy indicator pixmap.");
    }
}

void BusyIndicatorWidget::setGeometryToThis(QRect targetGeometry)
{
    this->setGeometry(targetGeometry);
}

void BusyIndicatorWidget::showWithAnimationAndActivate()
{
    this->show();
    this->raise();
//    this->setFocus();
    _currRotationValue = 0;
    _isFadeInMode = true;
    _uiUpdateTimer->start();
}

void BusyIndicatorWidget::hideWithAnimationAndDeactivate()
{
    _isFadeInMode = false;
//    _uiUpdateTimer->stop();
//    this->hide();
}

void BusyIndicatorWidget::doBusyStep()
{
    // will be called with ~FPS speed when active -> about 33 times / seconds
    // this means: if you want to spin the indicator with 360 degrees every second you have to increment it with 360/33 degrees every time (about 11 degree every time)
    //
    // currently it's set to 5,5 degrees every update -> 180 degrees / seconds

    _currRotationValue += 1440.0 / (1000.0f / (float)UI_UPDATE_FPS);

    //
    // also update the fade transparency
    if(_isFadeInMode) {
        if(_fadeTransparency < 0.9) {
            _fadeTransparency += 1000.0f / ((float)_showHideAnimationMilliseconds) / (float)UI_UPDATE_FPS;
        }
        else {
            _fadeTransparency = 1.0f;
        }
    }
    else {
        if(_fadeTransparency > 0.1f) {
            // still fading out
            _fadeTransparency -= 1000.0f / ((float)_showHideAnimationMilliseconds) / (float)UI_UPDATE_FPS;
        }
        else {
            // fade out completed
            _fadeTransparency = 0;
            _uiUpdateTimer->stop();
            this->hide();
        }
    }

//    QMatrix rm;
//    rm.rotate(_currRotationValue);
//    _transformedPixmapToPresent = _originalBusyIndicatorImage.transformed(rm);

//    this->repaint();
    this->update();
}
