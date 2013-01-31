#ifndef WIDGETRESIZERDELEGATE_H
#define WIDGETRESIZERDELEGATE_H

#include <QObject>
#include <QRect>

QT_BEGIN_NAMESPACE
class QMouseEvent;
class QHoverEvent;
QT_END_NAMESPACE

class WidgetResizerDelegate : public QObject
{
    Q_OBJECT

private:
    enum WidgetResizeModesEnum {
        WRM_NoResize = 0,
        WRM_ResizeTop = 1,
        WRM_ResizeBottom = 2,
        WRM_ResizeLeft = 4,
        WRM_ResizeRight = 8
    };

public:
    explicit WidgetResizerDelegate(QWidget *targetWidget, QObject *parent = 0);

    /*! Delegate mouser press event here

        @return True if handled, false if not handled
    */
    bool delegateMousePressEvent(QMouseEvent *e);

    /*! Delegate mouser move event here

        @return True if handled, false if not handled
    */
//    bool delegateMouseMoveEvent(QMouseEvent *e);
    bool delegateMouseMoveHoverEvent(QHoverEvent *e);

    /*! Delegate mouser release event here

        @return True if handled, false if not handled
    */
    bool delegateMouseReleaseEvent(QMouseEvent *e);

private:
    quint32 _calculateResizeModeFlagsFromMousePosition(QPoint mouseRelativePos);

private:
    QWidget *_targetWidget;
    quint32 _resizeModeFlags;
    QRect _initialGeom;
    QPoint _mousePressGlobalPos;
};

#endif // WIDGETRESIZERDELEGATE_H
