#ifndef PARALLELANIMATEDWIDGETREPLACER_H
#define PARALLELANIMATEDWIDGETREPLACER_H

#include <QObject>

#include <QSize>

QT_BEGIN_NAMESPACE
class QWidget;
class QBoxLayout;
class QTimer;
class QPropertyAnimation;
QT_END_NAMESPACE

#include "Common/WidgetReplacers/parallelanimatedwidgetreplaceritem.h"

class CrossFaderWidget;

//
// manages the replace of a bunch of widgets in a given layout to another widgets,
//  with animation
//
// (first) you have to specify the container layout which contains the source widget(s)
//  and which-to the target widget(s) will be inserted
//
// (possibly) set the duration
//
// (then) you can add the source-target widget pair(s) and whether the source widgets have to be deleted !when! the replace starts
//
// (finally) start the replace-animation
//
// ! you cannot add new widgets to replace after you called start
// ! you have to set the container layout before every start replace call, because it will be set to 0 (NULL) when the transition is finished.
class ParallelAnimatedWidgetReplacer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double transitionPercentage READ getTransitionPercentage WRITE setTransitionPercentage)

public:
    explicit ParallelAnimatedWidgetReplacer(QObject *parent = 0);
    ~ParallelAnimatedWidgetReplacer();

    //
    // first you have to specify the container layout which contains the source widget(s)
    //  and which-to the target widget(s) will be inserted
    void setTheContainerLayout(QBoxLayout *layout);

    void setDuration(int durationMilliseconds);

    //
    // ! the sourceWidget has to be in the given (container) layout when the replace starts
    //      and the targetWidget has to be NOT inlcuded in the layout when the replace starts
    // ! when the replace starts the sourceWidget is removed from the layout
    //      and it's parent is set to 0 (NULL) -> if the isAlsoDeleteSourceWidget is true (default)
    //          then the sourceWidget is also released, if false then the caller have to release it manually
    void addReplaceRequest(QWidget *sourceWidget, QWidget *targetWidget, bool isAlsoDeleteSourceWidget);

    void startBatchReplace();

    void jumpToFinalState();

Q_SIGNALS:

    void replaceFinished();

public Q_SLOTS:

private Q_SLOTS:
    void _transitionFinished();
    void _refreshTransitionWidgets();

private:
    //
    // transition-percentage
    double getTransitionPercentage() const;
    void setTransitionPercentage(double value);


private:
    double transitionPercentage;
    QTimer *guiRefresherTimer;
    QPropertyAnimation *theAnimation;

    QBoxLayout *_tmpContainerLayout;
    int durationMilliseconds;

    QList<ParallelAnimatedWidgetReplacerItem> replaceItems;
};

#endif // PARALLELANIMATEDWIDGETREPLACER_H

