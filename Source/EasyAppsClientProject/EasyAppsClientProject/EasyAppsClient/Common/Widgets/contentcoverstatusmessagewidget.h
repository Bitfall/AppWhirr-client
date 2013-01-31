#ifndef CONTENTCOVERSTATUSMESSAGEWIDGET_H
#define CONTENTCOVERSTATUSMESSAGEWIDGET_H

#include <QWidget>

/*! Special Widget to cover another widget / widgets

    Can be used to cover another widget or widgets.

    @remark Usage:
    - Create an object from this Widget class\n
    - Set the parent to the widget you want to cover or to the controller object (this is a recommendation, you can set the parent to anything, but you have to define a parent for this widget or it will show up as a new window)\n
    - Every time the cover-target widget's or area's geometry changes change the geometry of this cover-widget as well to be keep it synchronized - use the \a setGeometryToThis() method. You can connect a signal to the \a setGeometryToThis() slot as well.
    - Call the \a showWithAnimationAndActivate() and \a hideWithAnimationAndDeactivate() methods to activate (show) / deactivate (hide) with animation.
  */
class ContentCoverStatusMessageWidget : public QWidget
{
    Q_OBJECT
public:
    /*!
        @param showHideAnimationMilliseconds : Duration of the fade-in/out animations
        @param showTimeMilliseconds : Duration of 'show time'. After the show animation ended the
                    cover will be shown for this amount of time, then starts to fade-out.
    */
    explicit ContentCoverStatusMessageWidget(int showHideAnimationMilliseconds = 300, int showTimeMilliseconds = 2000, QWidget *parent = 0);

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
    void showWithAnimationAndActivate(QString messageToPresent);
    void hideWithAnimationAndDeactivate();

Q_SIGNALS:
    /*! Will be sent every time the cover-widget finishes with deactivate/hide.
    */
    void finishedAndHidden();

private Q_SLOTS:
    void __updateStep();

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
    QString _currMessageToPresent;

    bool _isHideIfMousePressed;
};

#endif // CONTENTCOVERSTATUSMESSAGEWIDGET_H
