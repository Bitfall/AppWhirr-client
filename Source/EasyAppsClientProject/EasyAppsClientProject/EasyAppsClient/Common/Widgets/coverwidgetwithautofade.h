#ifndef COVERWIDGETWITHAUTOFADE_H
#define COVERWIDGETWITHAUTOFADE_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QHBoxLayout;
QT_END_NAMESPACE

class FaderWidget;

/*! Special Widget to cover another widget / widgets. A general version of the \a ContentCoverStatusMessageWidget class.

    Can be used to cover another widget or widgets and present a custom widget.

    @note Similar to \a CoverWithCustomWidget but this one doesn't require a special custom content widget, it works for every QWidget based widget. Instead of delegating the fade-in/fade-out messages to the custom-widget it's based on 'snapshot based fade' solution: when a fade message appears it takes a snapshot of the target widget and fades the snapshot instead of the real widget. When the fade is finished it replaces the snapshot with the real widget.

    @remark Usage:\n
    - Create an object from this Widget class\n
    - Set the parent to the widget you want to cover or to the controller object (this is a recommendation, you can set the parent to anything, but you have to define a parent for this widget or it will show up as a new window)\n
    - Every time the cover-target widget's or area's geometry changes change the geometry of this cover-widget as well to be keep it synchronized - use the \a setGeometryToThis() method. You can connect a signal to the \a setGeometryToThis() slot as well.
    - Call the \a showWithAnimationAndActivate() and \a hideWithAnimationAndDeactivate() methods to activate (show) / deactivate (hide) with animation.
  */
class CoverWidgetWithAutoFade : public QWidget
{
    Q_OBJECT
public:    
    /*!
        @param contentWidget : the widget, will be presented in the cover-widget's content area. Currently it only works for widgets with fixed-size!
        @param showHideAnimationMilliseconds : Duration of the fade-in/out animations
        @param showTimeMilliseconds : Duration of 'show time'. After the show animation ended the cover will be shown for this amount of time, then starts to fade-out.\n
                        If it's < 0 then this widget won't disappear/hide automatically, only if \a hideWithAnimationAndDeactivate() is called, or if 'is-hide-if-mouse-pressed' is true (default) and the User clicks on the widget.
    */
    explicit CoverWidgetWithAutoFade(QWidget *contentWidget, int showHideAnimationMilliseconds = 300, int showTimeMilliseconds = 2000, QWidget *parent = 0);
    ~CoverWidgetWithAutoFade();

    /*! If true: the cover will fade-out and deactivate if the User clicks on it as well as when the 'showTimeMilliseconds' expires

        \b default : true
        @param value : Set whether to fade-out if User clicks, or only if the show-time expires. Set this to true if you want to allow "click-hide/dismiss"
    */
    void setIsHideIfMousePressed(bool value);

public Q_SLOTS:
    /*! Call this method to synchronize it's size with the target widget/area to cover.

      @param targetGeometry : the geometry this cover-widget will be shown.
    */
    void setGeometryToThis(QRect targetGeometry);

    /*! Fades-in. It fades a snapshot of the \a contentWidget instead of the real widget, then it replaces the snapshot with the real widget when the fade finishes.
    */
    void showWithAnimationAndActivate();

    /*! Fades-out. It fades a snapshot of the \a contentWidget instead of the real widget, then it replaces the snapshot with the real widget when the fade finishes.
    */
    void hideWithAnimationAndDeactivate();

Q_SIGNALS:
    /*! Will be sent every time the cover-widget finishes with deactivate/hide.
    */
    void finishedAndHidden();

private Q_SLOTS:
    void __updateStep();
    void _contentFadeFinished();

protected:
    void paintEvent(QPaintEvent *e);
    void mousePressEvent(QMouseEvent *e);

private:
    QTimer *_uiUpdateTimer;
    bool _isFadeInMode;
    float _fadeTransparency; // [0;1]
    int _currRemainingShowTimeMilliseconds;

    const int _showTimeMilliseconds;
    const int _showHideAnimationMilliseconds;
//    QString _currMessageToPresent;
    QHBoxLayout *_mainLayout;
    QWidget *_contentWidget;
    FaderWidget *_contentWidgetFaderWidget;

    bool _isHideIfMousePressed;
};

#endif // COVERWIDGETWITHAUTOFADE_H
