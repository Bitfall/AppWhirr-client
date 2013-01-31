#ifndef APPLICATIONTERMINATOR_H
#define APPLICATIONTERMINATOR_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QTimer;
class QEvent;
QT_END_NAMESPACE

#ifdef Q_WS_WIN
#include <windows.h>
#endif

#include "Application/Controllers/appterminationcontroller.h"

class PlatformspecificRunningExecutableDataInterface;

namespace Ui {
    class ApplicationTerminator;
}

//
// usage:
//  * basicly use it as any other dialog
//
// [!!!] -> will delete/free up itself after close
class ApplicationTerminator : public QWidget
{
    Q_OBJECT

public:
    explicit ApplicationTerminator(QWidget *parent = 0);
    ~ApplicationTerminator();

public Q_SLOTS:
    void actualize(QString appId, QString appName, AppTerminationController::TerminationReasonCodeEnum terminationReasonCode);

Q_SIGNALS:
    void abortTermination();
    void startTermination();

//protected:
//    bool event(QEvent *e);

private:
    Ui::ApplicationTerminator*      ui;
};

#endif // APPLICATIONTERMINATOR_H
