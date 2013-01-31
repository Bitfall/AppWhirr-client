#ifndef FADINGITEMDELEGATE_H
#define FADINGITEMDELEGATE_H

#include <QObject>

#include <QPixmap>

QT_BEGIN_NAMESPACE
class QTimer;
class QPaintEvent;
QT_END_NAMESPACE

/*! This is a delegate to allow widgets to support fade-in / fade-out animations relatively simply

    @remark Usage:
    * Inherit your (fading) widget or widget-manager class from FadingPerformerDelegateInterface
    * Delegate paintEvent to delegatePaintEventHere(), and delegate your start-fade-in/out to startFadeInAnimation() and startFadeOutAnimation()
    * Also you should probably set this in the presenter widget:\n
        this->setAttribute(Qt::WA_NoSystemBackground, true);
    * That's it, nothing else to do

    @note Technical details:
    * When showWithAnimation() called it renders the widget's normal (faded-in) state into a pixmap.
    * Then it fades in this snapshot, not the widget itself. No user interaction can be performed, and the widget itself is hidden.
    * And when the fade-in animation is finished it shows and starts to render the widget normally.
    * When it's in fade-out mode and finishes with the fade it turns off rendering of the widget to stay faded-out / invisible
    * Also note that the snapshot (re-)painting / updating happens differently with fix FPS to make it less resource intensive.
*/
class FadingItemDelegate : public QObject
{
    Q_OBJECT

    Q_PROPERTY(double fadeOpacity READ getFadeOpacity WRITE setFadeOpacity)

public:
    class FadingPerformerDelegateInterface {
    public:
        /*! When this is called the widget has to perform a normal paint, like it would in a normal \a paintEvent(QPaintEvent e) method.

            @note Do at least something like this (QWidget is the class your widget inherits from):
                QWidget::paintEvent(e);

            But you can render anything you would in a normal \a paintEvent(QPaintEvent e) method.
        */
        virtual void performNormalPaint(QPaintEvent *e) = 0;

        /*! Render only the given pixmap with the given opacity

            @note do it like this:
                QPainter painter(this);
                painter.setOpacity(requiredOpacity);
                painter.drawPixmap(this->rect(), pixmap);
        */
        virtual void performPaintOnlyThisPixmapWithOpacity(QPixmap pixmap, double requiredOpacity, QPaintEvent *e) = 0;

        /*! Return the fading presenter widget.

            If you use the \a FadingItemDelegate in the fading widget then you probably want to return the widget itself (return this;).\n
            But the implementer of \a FadingPerformerDelegateInterface does not have to be a widget, it can be a controller as well. In this case return the related fading-widget here.
        */
        virtual QWidget *getPresenterWidget() = 0;

        // ----------------
        // --- optional ---

        /*! Hide the content, requested by fade-animation-manager (the \a FadingItemDelegate )

            Implement this if the fading-widget has child-widgets, because a widget's paintEvent is performed only on the widget, and it does not influence it's child widgets. So the child widgets have to be hidden during the fading.

            @note If you implement this method you should implement \a showContentForFadeAnimation() , \a hideContentForFadeAnimation() , \a preserveCurrentSize() and \a dynamicSizeAllowed() methods.
        */
        virtual void hideContentForFadeAnimation() {}

        /*! Show the content, requested by fade-animation-manager (the \a FadingItemDelegate )

            Implement this if the fading-widget has child-widgets, because a widget's paintEvent is performed only on the widget, and it does not influence it's child widgets. So the child widgets have to be hidden during the fading.

            @note If you implement this method you should implement \a showContentForFadeAnimation() , \a hideContentForFadeAnimation() , \a preserveCurrentSize() and \a dynamicSizeAllowed() methods.
        */
        virtual void showContentForFadeAnimation() {}

        /*! Set the current size as fix size - don't allow resize.

            It's used to preserve the widget's size during the fade animation - if you hide a widget's content then the widget will be shrunked, and the fade-painting will be incorrect (shrunked as well). So we have to keep / preserve the size during the fading.

            @note If you implement this method you should implement \a showContentForFadeAnimation() , \a hideContentForFadeAnimation() , \a preserveCurrentSize() and \a dynamicSizeAllowed() methods.
        */
        virtual void preserveCurrentSize() {}

        /*! The widget is allowed to use dynamic size again. Usually you should set the size-policy and maybe the min/max sizes of the widget to the ones before the \a preserveCurrentSize() was called.

            It's used to preserve the widget's size during the fade animation - if you hide a widget's content then the widget will be shrunked, and the fade-painting will be incorrect (shrunked as well). So we have to keep / preserve the size during the fading.

            @note If you implement this method you should implement \a showContentForFadeAnimation() , \a hideContentForFadeAnimation() , \a preserveCurrentSize() and \a dynamicSizeAllowed() methods.
        */
        virtual void dynamicSizeAllowed() {}
    };

    enum FadeModes {
        FM_In,
        FM_Out
    };

public:
    explicit FadingItemDelegate(FadingPerformerDelegateInterface *fadingPreformerDelegate, int fadeDurationMilliseconds, QObject *parent = 0);

    void startFadeInAnimation();
    void startFadeOutAnimation();

    void delegatePaintEventHere(QPaintEvent *e);

private:
    void saveCurrentSnapshot();

    // ------------------------
    // --- internal signals ---
Q_SIGNALS:
    void __startFadeIn();
    void __startFadeOut();

private Q_SLOTS:
    void _fadeAnimationFinished();
    void _fadeAnimationStarted();

    void _updateGui();

private:
    double getFadeOpacity() const;
    void setFadeOpacity(double value);

    void _startGuiUpdater();
    void _stopGuiUpdater();

private:
    double _fadeOpacity;

    QPixmap _savedSnapshotPixmap;
    bool _isSavingSnapshot;

    bool _isFading;
    QTimer *_guiUpdaterTimer;

    FadeModes _currentFadeMode;

    FadingPerformerDelegateInterface *_fadingPerformerDelegate;
};

#endif // FADINGITEMDELEGATE_H
