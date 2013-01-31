#ifndef APPWHIRRUPDATERMAINWINDOW_H
#define APPWHIRRUPDATERMAINWINDOW_H

#include <QMainWindow>

namespace Ui {
    class AppWhirrUpdaterMainWindow;
}

class AppWhirrUpdaterMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum StatusTextVisualTypeEnum {
        STVT_Normal = 0,
        STVT_Error = 1
    };

public:
    explicit AppWhirrUpdaterMainWindow(QWidget *parent = 0);
    ~AppWhirrUpdaterMainWindow();

    void setStatusText(QString statusText, StatusTextVisualTypeEnum visualStyle = STVT_Normal);

private:
    Ui::AppWhirrUpdaterMainWindow *ui;

public Q_SLOTS:
    void setProgress(quint32 progress);
};

#endif // APPWHIRRUPDATERMAINWINDOW_H
