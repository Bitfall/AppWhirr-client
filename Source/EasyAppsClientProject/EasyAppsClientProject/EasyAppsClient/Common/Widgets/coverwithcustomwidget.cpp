#include "coverwithcustomwidget.h"

#include <QPaintEvent>
#include <QPainter>
#include <QTimer>
#include <QFont>
#include <QTextOption>
#include <QHBoxLayout>

// utility
#include "Common/Helpers/textsizehelper.h"

// config
#define CENTER_HIGHLIGHTED_AREA_TRASHOLD_PIXELS 30
#define UI_UPDATE_FPS 30

#define MESSAGE_BACKGROUND_WIDTH 300.0f
#define MESSAGE_BACKGROUND_HEIGHT 100.0f

CoverWithCustomWidget::CoverWithCustomWidget(FadingItemInterface *customContentWidget, int showHideAnimationMilliseconds, int showTimeMilliseconds, QWidget *parent) :
    QWidget(parent),
    _fadeTransparency(0),
    _currRemainingShowTimeMilliseconds(showTimeMilliseconds),
    _showHideAnimationMilliseconds(showHideAnimationMilliseconds),
    _showTimeMilliseconds(showTimeMilliseconds),
    _isFadeInMode(true),
    _customContentWidget(customContentWidget),
    _isHideIfMousePressed(true)
{
    this->setAttribute(Qt::WA_NoSystemBackground, true);

    _uiUpdateTimer = new QTimer(this);
    _uiUpdateTimer->setSingleShot(false);
    _uiUpdateTimer->setInterval( (int)(1000.0f / (float)UI_UPDATE_FPS) );
    connect(_uiUpdateTimer, SIGNAL(timeout()), this, SLOT(__updateStep()));

    QHBoxLayout *mainLayout = new QHBoxLayout;
    this->setLayout(mainLayout);

    QWidget *castedCustomWidget = dynamic_cast<QWidget *>(customContentWidget);
    if(castedCustomWidget == NULL) {
        WLog("It's not a widget!");
    }
    else {
        mainLayout->addWidget(castedCustomWidget);
//        castedCustomWidget->hide();
    }
}

void CoverWithCustomWidget::paintEvent(QPaintEvent *e)
{
//    if(_isFullRepaintNeeded) {
//        this->_refreshPresentablePixmapByRenderImage();
//        this->_isFullRepaintNeeded = false;
//    }

//    DLog("Paint with fade-transparency: ") << _fadeTransparency;

#if 0

    if(this->rect().size().width() <= 0 || this->rect().size().height() <= 0) {
        WLog("Invalid rect size (zero or smaller)");
        return;
    }

    QPainter painter(this);

    painter.setOpacity(_fadeTransparency);
    // background
    painter.fillRect(this->rect(), QColor(200, 200, 200, 100));
//    painter.setRenderHints(QPainter::SmoothPixmapTransform);

//    QSize textSize = TextSizeHelper::getTextSizeWithFont(_currMessageToPresent, QFont("Arial", 12, QFont::Bold));
//    QSize centerPartSize;

//    if(textSize.width() > 0 && textSize.height() > 0) {
//        centerPartSize = QSize( (textSize.width() < this->rect().size().width() ? textSize.width() : this->rect().size().width()),
//                                  (textSize.height() < this->rect().size().height() ? textSize.height() : this->rect().size().height()) );
//    }
//    else {
//        WLog("Invalid text size (zero).");
//        return;
//    }

    QSize centerPartSize = this->_customContentWidget->size();

    // center, highlighted area
    QRectF centerHighlightedAreaRect(((float)this->rect().size().width() / 2.0f) - (centerPartSize.width() / 2.0f) - CENTER_HIGHLIGHTED_AREA_TRASHOLD_PIXELS, // x
                                    ((float)this->rect().size().height() / 2.0f) - (centerPartSize.height() / 2.0f) - CENTER_HIGHLIGHTED_AREA_TRASHOLD_PIXELS, // y
                                    (2.0f * CENTER_HIGHLIGHTED_AREA_TRASHOLD_PIXELS) + centerPartSize.width(), // width
                                    (2.0f * CENTER_HIGHLIGHTED_AREA_TRASHOLD_PIXELS) + centerPartSize.height() // height
                                    );
    QPainterPath path;
    path.addRoundedRect(centerHighlightedAreaRect, 10, 10);
    painter.fillPath(path, QColor(60, 60, 60, 200));

//    painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);
//    painter.setFont(QFont("Arial", 12, QFont::Bold));
//    painter.setPen(QColor(250, 250, 250));
//    QTextOption textOption;
//    textOption.setAlignment(Qt::AlignCenter);
//    textOption.setWrapMode(QTextOption::NoWrap);
//    painter.drawText(centerHighlightedAreaRect, _currMessageToPresent, textOption);


////    painter.fillRect(centerHighlightedAreaRect, QColor(200, 200, 200, 60));
//    painter.setRenderHints(QPainter::SmoothPixmapTransform);

//    if(!_originalBusyIndicatorImage.isNull() && _originalBusyIndicatorImage.size().width() > 0 && _originalBusyIndicatorImage.size().height() > 0)
//    {
//        int posX = (this->rect().size().width() / 2);// - (_originalBusyIndicatorImage.width() / 2);
//        int posY = (this->rect().size().height() / 2);// - (_originalBusyIndicatorImage.height() / 2);

//        painter.save();
//        painter.translate(posX, posY);
//        painter.rotate(_currRotationValue);
//        painter.drawPixmap(-(_originalBusyIndicatorImage.width() / 2), -(_originalBusyIndicatorImage.height() / 2), _originalBusyIndicatorImage);
////        painter.drawPixmap(posX, posY, _originalBusyIndicatorImage.width(), _originalBusyIndicatorImage.height(), _transformedPixmapToPresent);
//        painter.restore();
//    }
//    else {
//        WLog("Invalid busy indicator pixmap.");
//    }
#endif
}

void CoverWithCustomWidget::mousePressEvent(QMouseEvent *e) {
    if(_isHideIfMousePressed) {
        this->hideWithAnimationAndDeactivate();
    }

    QWidget::mousePressEvent(e);
}

void CoverWithCustomWidget::setGeometryToThis(QRect targetGeometry)
{
    this->setGeometry(targetGeometry);
}

void CoverWithCustomWidget::showWithAnimationAndActivate()
{
    DLog("showWithAnimationAndActivate");

//    _currMessageToPresent = messageToPresent;
    _currRemainingShowTimeMilliseconds = _showTimeMilliseconds;
    this->show();
    this->raise();
    this->setFocus();
    _isFadeInMode = true;
    _uiUpdateTimer->start();

    _customContentWidget->startFadeInAnimation();
}

void CoverWithCustomWidget::hideWithAnimationAndDeactivate()
{
    DLog("hideWithAnimationAndDeactivate");

    _isFadeInMode = false;

    _customContentWidget->startFadeOutAnimation();
}

void CoverWithCustomWidget::__updateStep()
{
    static int stepDurationMilliseconds = (int)(1000.0f / (float)UI_UPDATE_FPS);

    //
    // also update the fade transparency
    if(_isFadeInMode) {
        if(_fadeTransparency < 0.9) {
            _fadeTransparency += 1000.0f / ((float)_showHideAnimationMilliseconds) / (float)UI_UPDATE_FPS;
        }
        else {
            // showtime!!
            _fadeTransparency = 1.0f;

            if(_showTimeMilliseconds >= 0) {
                // if show-time is < 0 then it marks it shouldn't deactivate automatically
                _currRemainingShowTimeMilliseconds -= stepDurationMilliseconds;
                if(_currRemainingShowTimeMilliseconds < 0) {
                    _isFadeInMode = false;
                }
            }
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
            Q_EMIT finishedAndHidden();
        }
    }

    DLog("Update - fade trans: ") << _fadeTransparency;

    this->update();
}

void CoverWithCustomWidget::setIsHideIfMousePressed(bool value) {
    _isHideIfMousePressed = value;
}
