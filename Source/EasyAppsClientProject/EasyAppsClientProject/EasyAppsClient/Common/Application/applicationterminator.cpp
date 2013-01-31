#include "applicationterminator.h"
#include "ui_applicationterminator.h"

ApplicationTerminator::ApplicationTerminator(QWidget *parent) :
//    QDialog(parent, Qt::Sheet),
    QWidget(parent),
    ui(new Ui::ApplicationTerminator)
{
    ui->setupUi(this);

    connect(ui->TerminateButton, SIGNAL(released()), this, SIGNAL(startTermination()));
    connect(ui->CancelButton, SIGNAL(released()), this, SIGNAL(abortTermination()));
}

ApplicationTerminator::~ApplicationTerminator()
{
    delete ui;
}

void ApplicationTerminator::actualize(QString appId, QString appName, AppTerminationController::TerminationReasonCodeEnum terminationReasonCode)
{
    QString terminationMessageText;
    terminationMessageText = tr("To continue you have to quit this application, or terminate it from here.");
//    if(terminationReasonCode == AppTerminationController::TRC_TerminateForUninstall) {
//        terminationMessageText = tr("To continue the uninstall process, first you should quit from the application, or terminate it from here.");
//    }
//    else {
//        terminationMessageText = tr("To continue installation, first you should quit from the application, or terminate it from here.");
//    }

    ui->Title->setText(terminationMessageText);

    ui->appNameLabel->setText(appName);
}

//// Event handling
//bool ApplicationTerminator::event(QEvent *e) {
//    if (e->type() == QEvent::Close) {
//        if (!signalAlreadyEmitted) {
//            Q_EMIT terminationAborted();
//        }
//    }

//    return QWi::event(e);
//}
