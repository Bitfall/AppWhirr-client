#ifndef APPDATABASEUPDATECHECKERMAINWINDOW_H
#define APPDATABASEUPDATECHECKERMAINWINDOW_H

#include <QtGui/QMainWindow>

#include "AppSpec/Dialogs/checkresultlistwindow.h"

QT_BEGIN_NAMESPACE
class QLineEdit;
class QTextEdit;
class QPushButton;
QT_END_NAMESPACE

class WebsiteCheckBackgroundWorker;

class AppDatabaseUpdateCheckerMainWindow : public QMainWindow, public CheckResultListWindow::CheckResultListWindowDelegate
{
    Q_OBJECT

public:
    AppDatabaseUpdateCheckerMainWindow(QWidget *parent = 0);
    ~AppDatabaseUpdateCheckerMainWindow();

    void saveFinalResultItems(QList<WebsiteListDatabaseItemModel> resultItems);
    void saveUpdatedAppLinks(QList<QString> urlList);

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

private Q_SLOTS:
    void browseDatabaseFileButtonPressed();
    void startCheckingButtonPressed();

    void checkingStartedForItem(QString appname, QString url);
    void checkingFinishedForItem(QString appname, QString url);
    void checkingFinished();

    void _addToStatus(QString statusMessage);

private:
    QPushButton *_startCheckingButton;
    QLineEdit *_databasePathLineEdit;
    QTextEdit *_statusTextEdit;

    int _itemsLeftToCheckCount;

    QList<WebsiteListDatabaseItemModel> _originalItems;

    WebsiteCheckBackgroundWorker *_websiteChecker;
};

#endif // APPDATABASEUPDATECHECKERMAINWINDOW_H
