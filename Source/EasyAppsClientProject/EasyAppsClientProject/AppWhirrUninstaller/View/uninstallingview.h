#ifndef UNINSTALLINGVIEW_H
#define UNINSTALLINGVIEW_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QTextEdit;
QT_END_NAMESPACE

class UninstallingView : public QWidget
{
    Q_OBJECT
public:
    explicit UninstallingView(QWidget *parent = 0);
    
    void setStatusText(QString statusText);

    void showActionBar();
    void hideActionBar();

Q_SIGNALS:
    void recheckRunningAppsButtonClicked();
    void forceUninstallButtonClicked();

private:
    QTextEdit *_statusTextEdit;
    QWidget *_actionBarWidget;
};

#endif // UNINSTALLINGVIEW_H
