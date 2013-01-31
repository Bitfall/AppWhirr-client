#ifndef CROSSFADERWIDGET_H
#define CROSSFADERWIDGET_H

#include <QWidget>
#include <QPixmap>

QT_BEGIN_NAMESPACE
class QTimer;
class QPropertyAnimation;
QT_END_NAMESPACE

//
// cross-fading between 2 widget snapshots
//
// can be used in 2 modes:
//  1) takes the snapshots and automatically fades from the source-widget's snapshot to the target's, and when it's finishes
//      sends a Q_SIGNAL about. Can be stopped with stopFading().
//  2) only takes the snapshots and an "outside" controller manages the fading via controlling the transition-percentages
class CrossFaderWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(float transitionPercentage READ getTransitionPercentage WRITE setTransitionPercentage)

public:
    explicit CrossFaderWidget(QWidget *parent = 0);
    ~CrossFaderWidget();

    //
    // mode 1)

    // automatically takes snapshots of the given widgets and animates between them
    void takeSnapshotsOfWidgetsAndFadeBetweenThem(QWidget *sourceWidget, QWidget *targetWidget,
                                                  QRect sourceSnapshotGeometry, QRect targetSnapshotGeometry,
                                                  int durationMillisec = 500);
    void stopFading();

    //
    // mode 2)

    // takes snapshots of the provided widgets but doesn't start any animation - the "outside" controller
    //  can manage the fading via controlling the transition-percentage
    void takeSnapshotsOfWidgetsAndInitializeFading(QWidget *sourceWidget, QWidget *targetWidget,
                                                   QRect sourceSnapshotGeometry, QRect targetSnapshotGeometry);

    //
    // transition-percentage
    float getTransitionPercentage() const;
    void setTransitionPercentage(float value);

Q_SIGNALS:
    void fadeFinish();

public Q_SLOTS:

private Q_SLOTS:
    void _fadeAnimationFinished();


protected:
    void paintEvent(QPaintEvent *e);

private:
    void takeAndStoreSnapshotsAboutWidgets(QWidget *sourceWidget, QWidget *targetWidget);

private:
    QPixmap sourceWidgetSnapshot;
    QPixmap targetWidgetSnapshot;

    float transitionPercentage;
    QTimer *guiUpdaterTimer;

    QPoint sourcePos;
    QPoint targetPos;

    QRect sourceGeom;
    QRect targetGeom;

    QPropertyAnimation *fadeAnimation;
};

#endif // CROSSFADERWIDGET_H
