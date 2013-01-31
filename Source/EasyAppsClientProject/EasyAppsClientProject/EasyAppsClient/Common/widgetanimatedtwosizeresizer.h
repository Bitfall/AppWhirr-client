#ifndef WIDGETANIMATEDTWOSIZERESIZER_H
#define WIDGETANIMATEDTWOSIZERESIZER_H

#include <QObject>
#include <QSizeF>

QT_BEGIN_NAMESPACE
class QTimer;
QT_END_NAMESPACE

/*! With this class makes it easy to implement an optimized, fix FPS based size-switch for a given widget.

    You just have to specify the target-widget (will be resized when requested) and set some parameters like the duration of the change animation and the resize mode (only resize horizontally/vertically/both, ...). After this you can safely call the switch..() slots and the \a targetWidget will be resized (expanded or collapsed).
*/
class WidgetAnimatedTwoSizeResizer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QSizeF currFixedSize READ getCurrFixedSize WRITE setCurrFixedSize)

public:
    enum WidgetResizeModeEnum {
        WRM_None = 0,
        WRM_SetOnlyMinimumSize = 1,
        WRM_SetOnlyMaximumSize = 2,
        WRM_SetBothMinimumAndMaximumSizes = 3
    };

public:
    explicit WidgetAnimatedTwoSizeResizer(QWidget *targetWidget, int sizeChangeDurationMilliseconds, QSize collapsedSize, QSize expandedSize, bool isExpandedByDefault, bool isSetWidth, bool isSetHeight, WidgetResizeModeEnum widgetResizeMode, QObject *parent = 0);

//    QSize sizeHint() const;

    bool getIsExpanded() const;

    QSizeF getCurrFixedSize() const;
    void setCurrFixedSize(QSizeF newFixedSize);

    /*! With this you can set that only minimum, only maximum or both sizes should be changed.

        @note Calling this method will change the \a targetWidget 's size-policy.
    */
    void setWidgetResizeMode(WidgetResizeModeEnum widgetResizeMode);
//    void _setIsSetOnlyMinimumSize(bool value);
    void _setIsSetWidth(bool value);
    void _setIsSetHeight(bool value);

public Q_SLOTS:
    void switchToExpandedStateSlot();
    void switchToCollapsedStateSlot();
    void switchState(bool isExpanded);

Q_SIGNALS: // public interface signals
    void switchToExpandedStateFinished();
    void switchToCollapsedStateFinished();

Q_SIGNALS: // internal, private signals
    //
    // connected to the state-machine -> emit this to expand
    void _switchToExpandedStateSignal();
    //
    // connected to the state-machine -> emit this to collapse
    void _switchToCollapsedStateSignal();

private Q_SLOTS:
//    void stateChangeStarted();
    void stateChangeFinished();

    void _updateByCurrFixedSize();

private:
    QWidget *_targetWidget;

    bool _isExpanded;
    QSizeF _currFixedSize;
//    bool _isSetOnlyMinimumSize;
    WidgetResizeModeEnum _widgetResizeMode;
    bool _isSetWidth;
    bool _isSetHeight;

    int _sizeChangeDurationMilliseconds;

    QTimer *_guiRefreshTimer;
};


#endif // WIDGETANIMATEDTWOSIZERESIZER_H
