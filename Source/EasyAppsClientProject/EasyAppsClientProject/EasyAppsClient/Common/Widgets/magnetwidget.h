#ifndef MAGNETWIDGET_H
#define MAGNETWIDGET_H

#include <QWidget>

/*! MagnetWidget is a placeholder widget which can 'magnet' a target widget which is not in the layout to cover this MagnetWidget.

    The purpose of this widget is to be a placeholder / magnet area for a target-widget. The target-widget is removed from it's layout (if it's in a layout) and should not added to any layout. This MagnetWidget will automatically resize and move the target-widget to cover this MagnetWidget.
*/
class MagnetWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MagnetWidget(QWidget *parent = 0);

    /*! Sets the target-widget.

        @param isApplyEventFilter : if true then event-filter will be applied (installed) on the target-widget. This will override the 'close' event. Useful for target-widgets which are windows positioned over this magnet-widget.
        @param isApplyFlagsForFramelessWindow : if true it will set required window-flags and attributes on the target-widget to be a separate window but look like it's in the layout (to not look like it's a separate window)
        @param isReparent : if true (default) then the target-widget will be reparented to this MagnetWidget.
    */
    void setMagnetTargetWidget(QWidget *targetWidget, bool isApplyEventFilter, bool isApplyFlagsForFramelessWindow, bool isReparent = true);

    QWidget *getMagnetTargetWidget() const;

public Q_SLOTS:
    /*! Force update the target-widget's geometry
    */
    void updateMagnetTargetGeometry();
    
protected:
    void resizeEvent(QResizeEvent *e);
    bool event(QEvent *e);
    bool eventFilter(QObject *o, QEvent *e);

    void paintEvent(QPaintEvent *e);

private:
    void showTargetWidget();
    void hideTargetWidget();
private Q_SLOTS:
    void refreshTargetWidgetVisibility();

private:
    QWidget *_magnetTargetWidget;
    bool _isShowTargetWidgetAtNextPaintEvent;
};

#endif // MAGNETWIDGET_H
