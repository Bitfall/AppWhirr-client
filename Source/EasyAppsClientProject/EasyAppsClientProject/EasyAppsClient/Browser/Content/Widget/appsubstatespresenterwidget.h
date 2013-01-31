#ifndef APPSUBSTATESPRESENTERWIDGET_H
#define APPSUBSTATESPRESENTERWIDGET_H

#include <QWidget>

#include "Application/ApplicationDescription/applicationstatesenum.h"

class SimpleImagePresenterWidget;

class AppSubStatesPresenterWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AppSubStatesPresenterWidget(QWidget *parent = 0);

    void setSubStates(AppWhirr::ApplicationSubStateFlags::SummedApplicationSubStateFlags summedFlags);
    AppWhirr::ApplicationSubStateFlags::SummedApplicationSubStateFlags getSummedSubStateFlags() const;

Q_SIGNALS:
    void updateAvailableIconPresenterActivated();
    void notAvailableForThisPlatformIconPresenterActivated();
    void removedFromStoreIconPresenterActivated();

protected:
    void paintEvent(QPaintEvent *e);

private:
    SimpleImagePresenterWidget *_updateAvailableIconPresenterWidget;
    SimpleImagePresenterWidget *_notSupportedOnThisPlatformIconPresenterWidget;
    SimpleImagePresenterWidget *_removedFromTheSystemIconPresenterWidget;

    AppWhirr::ApplicationSubStateFlags::SummedApplicationSubStateFlags _summedSubStateFlags;
};

#endif // APPSUBSTATESPRESENTERWIDGET_H
