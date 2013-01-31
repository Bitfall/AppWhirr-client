#ifndef APPTERMINATORSIDEBARWIDGET_H
#define APPTERMINATORSIDEBARWIDGET_H

#include "Common/Widgets/animatedtwofixedsizewidgetbase.h"

#include "Application/Controllers/appterminationcontroller.h"

class ApplicationTerminator;

class AppTerminatorSidebarWidget : public AnimatedTwoFixedSizeWidgetBase
{
    Q_OBJECT
public:
    explicit AppTerminatorSidebarWidget(QSize expandedSize, QWidget *parent = 0);

public Q_SLOTS:
    void actualizeAndShow(QString appId, QString appName, AppTerminationController::TerminationReasonCodeEnum terminationReasonCode);
    void hideWithAnimation();

Q_SIGNALS:
    void abortTermination();
    void startTermination();

private Q_SLOTS:
    void _abortTerminationSlot();
    void _startTerminationSlot();

private:
    ApplicationTerminator *_applicationTerminatorWidget;
};

#endif // APPTERMINATORSIDEBARWIDGET_H
