#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
QT_END_NAMESPACE

class ViewControllerInterface;
class UninstallConfigs;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private Q_SLOTS:
    void switchToWelcomeScreen();
    void switchToUninstallingScreen();
    void switchToGoodbyeScreen();

    void quitFromUninstaller();

private:
    void setThisViewAndControllerAsCurrent(ViewControllerInterface *viewController);
    void removeAndDestroyCurrentViewAndController();

    void switchFromCurrentViewAndControllerToThisOne(ViewControllerInterface *viewController);

    void processAppArgsAndFillTheUninstallConfigs(UninstallConfigs *uninstallConfigs);

private:
    QVBoxLayout *_mainLayout;
    UninstallConfigs *_uninstallConfigs;
    ViewControllerInterface *_currentViewController;
};

#endif // MAINWINDOW_H
