#ifndef SYSTEMTRAYICONDELEGATE_H
#define SYSTEMTRAYICONDELEGATE_H

#include <QObject>
#include <QSystemTrayIcon>

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
QT_END_NAMESPACE

class SystemTrayIconDelegate : public QObject
{
    Q_OBJECT
public:
    explicit SystemTrayIconDelegate(QObject *parent = 0);
    ~SystemTrayIconDelegate();

    void showSystemTrayMessage(QString message, QString title = "");
    void createTheSystemTrayItem();

private Q_SLOTS:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void messageClicked();

Q_SIGNALS:
    void showStorePageSelected();
    void showMyAppsPageSelected();
    void showBrowserInCurrentState();
    void showFeedbackPageSelected();

private:
    //
    // system-tray
    QAction *_showMyAppsAction;
    QAction *_showStoreAction;
    QAction *_showFeedbackPageAction;
    QAction *_quitAction;

    QSystemTrayIcon *_trayIcon;
    QMenu *_trayIconMenu;
};

#endif // SYSTEMTRAYICONDELEGATE_H
