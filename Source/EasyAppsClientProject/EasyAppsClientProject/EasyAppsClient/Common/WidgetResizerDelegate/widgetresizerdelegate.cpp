#include "widgetresizerdelegate.h"

#include <QMouseEvent>
#include <QHoverEvent>
#include <QWidget>

// config
#define MOUSE_RESIZE_BORDER_TRESHOLD_PIXELS 2

WidgetResizerDelegate::WidgetResizerDelegate(QWidget *targetWidget, QObject *parent) :
    QObject(parent),
    _targetWidget(targetWidget),
    _resizeModeFlags(0)
{
    _targetWidget->setMouseTracking(true);
}

bool WidgetResizerDelegate::delegateMousePressEvent(QMouseEvent *e)
{
    quint32 resizeModeFlags = this->_calculateResizeModeFlagsFromMousePosition(e->pos());

    if(resizeModeFlags != 0) {
        // start the resize
        _resizeModeFlags = resizeModeFlags;
        _mousePressGlobalPos = e->globalPos();
//        _mousePressGlobalPos = e->pos();
        _initialGeom = _targetWidget->geometry();

        DLog("Resize flags: ") << _resizeModeFlags;

        return true;
    }

    return false;
}

quint32 WidgetResizerDelegate::_calculateResizeModeFlagsFromMousePosition(QPoint mouseRelativePos)
{
    quint32 calculatedResizeModeFlags = 0;

    if(mouseRelativePos.x() <= MOUSE_RESIZE_BORDER_TRESHOLD_PIXELS || mouseRelativePos.x() >= (_targetWidget->geometry().width() - MOUSE_RESIZE_BORDER_TRESHOLD_PIXELS) ||
            mouseRelativePos.y() <= MOUSE_RESIZE_BORDER_TRESHOLD_PIXELS || mouseRelativePos.y() >= (_targetWidget->geometry().height() - MOUSE_RESIZE_BORDER_TRESHOLD_PIXELS))
    {
        if(mouseRelativePos.x() <= MOUSE_RESIZE_BORDER_TRESHOLD_PIXELS) {
            calculatedResizeModeFlags |= (quint32)WidgetResizerDelegate::WRM_ResizeLeft;
        }
        else if(mouseRelativePos.x() >= (_targetWidget->geometry().width() - MOUSE_RESIZE_BORDER_TRESHOLD_PIXELS)) {
            calculatedResizeModeFlags |= (quint32)WidgetResizerDelegate::WRM_ResizeRight;
        }

        if(mouseRelativePos.y() <= MOUSE_RESIZE_BORDER_TRESHOLD_PIXELS) {
            calculatedResizeModeFlags |= (quint32)WidgetResizerDelegate::WRM_ResizeTop;
        }
        else if(mouseRelativePos.y() >= (_targetWidget->geometry().height() - MOUSE_RESIZE_BORDER_TRESHOLD_PIXELS)) {
            calculatedResizeModeFlags |= (quint32)WidgetResizerDelegate::WRM_ResizeBottom;
        }
    }

    return calculatedResizeModeFlags;
}

bool WidgetResizerDelegate::delegateMouseMoveHoverEvent(QHoverEvent *e)
{
//    DLog("Delegate mouse ") << _resizeModeFlags;
//    QCursor::setShape();
    if(_resizeModeFlags == ((quint32)WidgetResizerDelegate::WRM_NoResize)) {
        // not in resize mode

        // test for cursor shape
        quint32 resizeModeFlags = this->_calculateResizeModeFlagsFromMousePosition(e->pos());
        if(resizeModeFlags != 0) {

            if( (resizeModeFlags & ((int)WidgetResizerDelegate::WRM_ResizeTop)) != 0 )
            {
                if( (resizeModeFlags & ((int)WidgetResizerDelegate::WRM_ResizeLeft)) != 0 ) {
                    // top left
                    QCursor resizeCursor(Qt::SizeFDiagCursor);
                    _targetWidget->setCursor(resizeCursor);
                }
                else if( (resizeModeFlags & ((int)WidgetResizerDelegate::WRM_ResizeRight)) != 0 ) {
                    // top right
                    QCursor resizeCursor(Qt::SizeBDiagCursor);
                    _targetWidget->setCursor(resizeCursor);
                }
                else {
                    // top
                    QCursor resizeCursor(Qt::SizeVerCursor);
                    _targetWidget->setCursor(resizeCursor);
                }
            }

            else if( (resizeModeFlags & ((int)WidgetResizerDelegate::WRM_ResizeBottom)) != 0 )
            {
                if( (resizeModeFlags & ((int)WidgetResizerDelegate::WRM_ResizeLeft)) != 0 ) {
                    // bottom left
                    QCursor resizeCursor(Qt::SizeBDiagCursor);
                    _targetWidget->setCursor(resizeCursor);
                }
                else if( (resizeModeFlags & ((int)WidgetResizerDelegate::WRM_ResizeRight)) != 0 ) {
                    // bottom right
                    QCursor resizeCursor(Qt::SizeFDiagCursor);
                    _targetWidget->setCursor(resizeCursor);
                }
                else {
                    // bottom
                    QCursor resizeCursor(Qt::SizeVerCursor);
                    _targetWidget->setCursor(resizeCursor);
                }
            }

            else if( (resizeModeFlags & ((int)WidgetResizerDelegate::WRM_ResizeLeft)) != 0 ) {
                // left
                QCursor resizeCursor(Qt::SizeHorCursor);
                _targetWidget->setCursor(resizeCursor);
            }

            else if( (resizeModeFlags & ((int)WidgetResizerDelegate::WRM_ResizeRight)) != 0 ) {
                // right
                QCursor resizeCursor(Qt::SizeHorCursor);
                _targetWidget->setCursor(resizeCursor);
            }
        }
        else {
//            QCursor defaultCursor;
//            _targetWidget->setCursor(defaultCursor);
            _targetWidget->unsetCursor();
        }

        return false;
    }

    qint32 mouseMoveDifX = QCursor::pos().x() - _mousePressGlobalPos.x();
    qint32 mouseMoveDifY = QCursor::pos().y() - _mousePressGlobalPos.y();

    qint32 widgetNewPosX = _initialGeom.x();
    qint32 widgetNewPosY = _initialGeom.y();
    qint32 widgetNewWidth = _initialGeom.width();
    qint32 widgetNewHeight = _initialGeom.height();

    if( (_resizeModeFlags & ((quint32)WidgetResizerDelegate::WRM_ResizeLeft)) > 0 ) {
        widgetNewWidth = _initialGeom.width() - mouseMoveDifX;
        if(widgetNewWidth > _targetWidget->minimumWidth()) {
//            widgetNewPosX = e->globalPos().x();
            widgetNewPosX = QCursor::pos().x();
//            widgetNewPosX = widgetNewPosX + mouseMoveDifX;
        }
        else {
            widgetNewWidth = _targetWidget->minimumWidth();
        }
    }
    else if( (_resizeModeFlags & ((quint32)WidgetResizerDelegate::WRM_ResizeRight)) > 0 ) {
        widgetNewWidth = _initialGeom.width() + mouseMoveDifX;
    }

    if( (_resizeModeFlags & ((quint32)WidgetResizerDelegate::WRM_ResizeTop)) > 0 ) {
        widgetNewHeight = _initialGeom.height() - mouseMoveDifY;
        if(widgetNewHeight > _targetWidget->minimumHeight()) {
//            widgetNewPosY = e->globalPos().y();
            widgetNewPosY = QCursor::pos().y();
//            widgetNewPosY = widgetNewPosY = mouseMoveDifY;
        }
        else {
            widgetNewHeight = _targetWidget->minimumHeight();
        }
    }
    else if( (_resizeModeFlags & ((quint32)WidgetResizerDelegate::WRM_ResizeBottom)) > 0 ) {
        widgetNewHeight = _initialGeom.height() + mouseMoveDifY;
    }

    _targetWidget->setGeometry(widgetNewPosX, widgetNewPosY, widgetNewWidth, widgetNewHeight);

    return true;
}

bool WidgetResizerDelegate::delegateMouseReleaseEvent(QMouseEvent *e)
{
    if(_resizeModeFlags != ((quint32)WidgetResizerDelegate::WRM_NoResize)) {
        _resizeModeFlags = (quint32)WidgetResizerDelegate::WRM_NoResize;
        return true;
    }

    return false;
}
