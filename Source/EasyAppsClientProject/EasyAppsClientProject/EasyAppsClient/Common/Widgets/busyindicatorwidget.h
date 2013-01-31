#ifndef BUSYINDICATORWIDGET_H
#define BUSYINDICATORWIDGET_H

#include <QWidget>
#include <QRect>

//
// Usually the busy-indicator will be presented above other contents. To allow this you have to set it's parent to a window/widget "under" the BusyIndicator (without this the indicator will show in a different window)
//  * To allow resize properly call the setGeometryToThis() method every time the main content area's size/geometry changes to allow the BusyIndicator to follow and cover it.
class BusyIndicatorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BusyIndicatorWidget(QPixmap busyIndicatorImage, int showHideAnimationMilliseconds = 300, QWidget *parent = 0);

public Q_SLOTS:
    void setGeometryToThis(QRect targetGeometry);
    void showWithAnimationAndActivate();
    void hideWithAnimationAndDeactivate();

    void doBusyStep();

protected:
    void paintEvent(QPaintEvent *e);

private:
    QPixmap _originalBusyIndicatorImage;
    QTimer *_uiUpdateTimer;
    float _currRotationValue;
    float _fadeTransparency; // [0;1]
    bool _isFadeInMode;
    int _showHideAnimationMilliseconds;
};

#endif // BUSYINDICATORWIDGET_H
