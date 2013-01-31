#ifndef ANIMATEDWIDGETREPLACER_H
#define ANIMATEDWIDGETREPLACER_H

#include <QObject>

#include <QSize>

QT_BEGIN_NAMESPACE
class QWidget;
class QBoxLayout;
class QTimer;
class QPropertyAnimation;
QT_END_NAMESPACE

class CrossFaderWidget;

//
// manages the replace of a widget in a given layout to another widget,
//  with animation
class AnimatedWidgetReplacer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(float transitionPercentage READ getTransitionPercentage WRITE setTransitionPercentage)

public:
    explicit AnimatedWidgetReplacer(QObject *parent = 0);
    ~AnimatedWidgetReplacer();

    //
    // ! the sourceWidget has to be in the given layout
    //      and the targetWidget has to be NOT inlcuded in the layout
    void replaceWidgetInLayoutToWidget(QWidget *sourceWidget, QWidget *targetWidget, QBoxLayout *layout, int durationMilliseconds = 2500, bool isAlsoDeleteSourceWidget = true);

    void jumpToFinalState();

Q_SIGNALS:

    void replaceFinished();

public Q_SLOTS:

private Q_SLOTS:
    void _transitionFinished();

private:
    //
    // transition-percentage
    float getTransitionPercentage() const;
    void setTransitionPercentage(float value);


private:
    float transitionPercentage;
    CrossFaderWidget *faderWidget;
    QTimer *guiRefresherTimer;
    QPropertyAnimation *theAnimation;

    QSize sourceWidgetSize;
    QSize targetWidgetSize;

    QWidget *_tmpTargetWidget;
    QBoxLayout *_tmpLayout;
};

#endif // ANIMATEDWIDGETREPLACER_H
