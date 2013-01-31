#include "appwhirrupdatermainwindow.h"
#include "ui_appwhirrupdatermainwindow.h"

AppWhirrUpdaterMainWindow::AppWhirrUpdaterMainWindow(QWidget *parent) :
    QMainWindow(parent, Qt::FramelessWindowHint | Qt::CustomizeWindowHint),
    ui(new Ui::AppWhirrUpdaterMainWindow)
{
    ui->setupUi(this);

//    ui->progressBar->setMinimum(0);
//    ui->progressBar->setMaximum(0);

    this->setWindowTitle("AppWhirr Updater");

    // --- transparent window
    // it only works if the window is frameless
    setAttribute(Qt::WA_TranslucentBackground, true);
}

AppWhirrUpdaterMainWindow::~AppWhirrUpdaterMainWindow()
{
    delete ui;
}

void AppWhirrUpdaterMainWindow::setStatusText(QString statusText, StatusTextVisualTypeEnum visualStyle)
{
    if(visualStyle == STVT_Error) {
        this->ui->statusLabel->setStyleSheet("* { color: #ff999a; font: bold 16px arial, sans-serif; }");
    }
    else {
        this->ui->statusLabel->setStyleSheet("* { color: #9d9d9d; font: 14px arial, sans-serif; }");
    }

    this->ui->statusLabel->setText(statusText);
}

void AppWhirrUpdaterMainWindow::setProgress(quint32 progress)
{
    quint32 maxSize = 444;

    if (progress > 100) progress = 100;

    ui->progressBar->setGeometry(2, 2, progress * maxSize / 100.0, ui->progressBar->height());
    ui->progessLabel->setText(QString("%1%").arg(progress));
}
