#include "appdatabaseupdatecheckermainwindow.h"

// UI
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QGridLayout>
#include <QTextEdit>

// UI utility
#include <QMessageBox>
#include <QFileInfo>
#include <QFileDialog>

//
#include <QUrl>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QDateTime>
#include <QFile>
#include <QTextStream>

#include "AppSpec/Database/websitelistdatabasemanager.h"
#include "AppSpec/websitecheckbackgroundworker.h"
#include "AppSpec/Dialogs/checkresultlistwindow.h"

AppDatabaseUpdateCheckerMainWindow::AppDatabaseUpdateCheckerMainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->resize(600, 300);

    QWidget *mainCentralWidget = new QWidget;
    this->setCentralWidget(mainCentralWidget);

    QGridLayout *mainLayout = new QGridLayout;
    mainCentralWidget->setLayout(mainLayout);

    //
    // main layout content
    QLabel *sourceDatabasePath = new QLabel(tr("Database path:"));
    mainLayout->addWidget(sourceDatabasePath, 0, 0);
    this->_databasePathLineEdit = new QLineEdit;
    this->_databasePathLineEdit->setPlaceholderText(tr("Type the path here, or drag-n-drop the file"));
    mainLayout->addWidget(this->_databasePathLineEdit, 0, 1);
    QPushButton *browseDatabaseFileButton = new QPushButton;
    browseDatabaseFileButton->setText(tr("Open database file"));
    connect(browseDatabaseFileButton, SIGNAL(clicked()), this, SLOT(browseDatabaseFileButtonPressed()));
    mainLayout->addWidget(browseDatabaseFileButton, 0, 2);

    _startCheckingButton = new QPushButton;
    _startCheckingButton->setText(tr("Start Checking"));
    connect(_startCheckingButton, SIGNAL(clicked()), this, SLOT(startCheckingButtonPressed()));
    mainLayout->addWidget(_startCheckingButton, 1, 0, 1, -1);

    this->_statusTextEdit = new QTextEdit;
//    this->_statusTextEdit->setText(tr("Specify the (input, webpage list) database path."));
    this->_statusTextEdit->append(tr("Status messages will appear here..."));
    this->_statusTextEdit->setStyleSheet(QString("QLabel { font-size: 18px }"));
    this->_statusTextEdit->setReadOnly(true);
    mainLayout->addWidget(this->_statusTextEdit, 2, 0, 1, -1);

    this->acceptDrops();
}

AppDatabaseUpdateCheckerMainWindow::~AppDatabaseUpdateCheckerMainWindow()
{

}

void AppDatabaseUpdateCheckerMainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void AppDatabaseUpdateCheckerMainWindow::dropEvent(QDropEvent *event)
{
    QList<QUrl> droppedUrls = event->mimeData()->urls();
    int droppedUrlCnt = droppedUrls.size();
    if(droppedUrlCnt != 1) {
        QMessageBox::information(this, tr("Invalid"), tr("Only 1 website-list database file can be drag-and-dropped at a time."));
        event->ignore();
    }

    QString localPath = droppedUrls[0].toLocalFile();
    QFileInfo fileInfo(localPath);
    if(fileInfo.isFile()) {
        this->_databasePathLineEdit->setText(fileInfo.absoluteFilePath());
        event->acceptProposedAction();
    }
    else if(fileInfo.isDir()) {
        QMessageBox::information(this, tr("Invalid"), tr("Only website-list database files accepted - you dropped a folder."));
        event->ignore();
    }
    else {
        QMessageBox::information(this, tr("Dropped, but unknown"), tr("Only website-list database files accepted - unknown drop: %1").arg(localPath));
        event->ignore();
    }
}

void AppDatabaseUpdateCheckerMainWindow::browseDatabaseFileButtonPressed()
{
    QFileDialog fileDialog(this);
    fileDialog.setFileMode(QFileDialog::ExistingFile);
    fileDialog.setNameFilter(tr("App Update Checking database (*.awauc)"));

    QStringList fileNames;
    if (fileDialog.exec()) {
        fileNames = fileDialog.selectedFiles();
        this->_databasePathLineEdit->setText(fileNames.first());
    }
}

void AppDatabaseUpdateCheckerMainWindow::_addToStatus(QString statusMessage)
{
    this->_statusTextEdit->append(statusMessage);
}

void AppDatabaseUpdateCheckerMainWindow::startCheckingButtonPressed()
{
    QString inputDatabasePath = this->_databasePathLineEdit->text();
    if(inputDatabasePath.isEmpty()) {
        QMessageBox::warning(this, tr("No input database specified"), tr("First you have to specify an input database"));
        return;
    }

    this->_addToStatus("-------------------");
    this->_addToStatus(tr("Checking started - opening the list database..."));

//    // backup the current input database
//    QFileInfo inputDatabaseFileInfo(inputDatabasePath);
//    QString backupDatabasePath = QString("%1__backup__%2.awauc").arg(inputDatabaseFileInfo.absolutePath() + "/" + inputDatabaseFileInfo.baseName()).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd__hh-mm-ss"));
//    DLog("Backup file path: ") << backupDatabasePath;
//    if(!QFile::copy(inputDatabasePath, backupDatabasePath)) {
//        QMessageBox::warning(this, tr("Error"), tr("Cannot backup the input database to path: %1").arg(backupDatabasePath));
//        return;
//    }

    // read the database
    bool isReadSuccess = true;
    WebsiteListDatabaseManager *websiteListDatabaseManager = new WebsiteListDatabaseManager;
    websiteListDatabaseManager->_completelyDeleteDatabase();
    if(!websiteListDatabaseManager->importDatabase(inputDatabasePath)) {
        isReadSuccess = false;
    }
    else {
        _originalItems = websiteListDatabaseManager->getEveryItem();
    }
    delete websiteListDatabaseManager;

    if(!isReadSuccess || _originalItems.isEmpty()) {
        WLog("Cannot read the input database or it's empty: ") << inputDatabasePath;
        this->_addToStatus(tr("Cannot read the input database or it's empty: %1").arg(inputDatabasePath));
        return;
    }

    _startCheckingButton->setEnabled(false);

    _itemsLeftToCheckCount = _originalItems.size();
    this->_addToStatus(tr("%1 items found. Checking started...").arg(this->_itemsLeftToCheckCount));

    this->_websiteChecker = new WebsiteCheckBackgroundWorker(this->_originalItems);
    connect(_websiteChecker, SIGNAL(startedCheckingItem(QString,QString)), this, SLOT(checkingStartedForItem(QString,QString)));
    connect(_websiteChecker, SIGNAL(finishedCheckingItem(QString,QString)), this, SLOT(checkingFinishedForItem(QString,QString)));
    connect(_websiteChecker, SIGNAL(finishedCompletely()), this, SLOT(checkingFinished()));
    this->_websiteChecker->startChecking();
}

void AppDatabaseUpdateCheckerMainWindow::checkingStartedForItem(QString appname, QString url)
{
    this->_addToStatus(tr("Checking started for item: %1 [%2 remaining]").arg(appname).arg(_itemsLeftToCheckCount));
}

void AppDatabaseUpdateCheckerMainWindow::checkingFinishedForItem(QString appname, QString url)
{
    _itemsLeftToCheckCount--;

//    this->_addToStatus(tr("Checking finished for app [%2 remaining]: %1").arg(appname).arg(_itemsLeftToCheckCount));
}

void AppDatabaseUpdateCheckerMainWindow::checkingFinished()
{
    QList<WebsiteListDatabaseItemModel> resultItems = this->_websiteChecker->getResultWebsiteListItems();
    this->_websiteChecker->deleteLater();
    this->_addToStatus(tr("Checking finished - result count: %1 - start to compare...").arg(resultItems.size()));

    CheckResultListWindow *resultListWindow = new CheckResultListWindow(this, this);
    if(!resultListWindow->setCheckItems(this->_originalItems, resultItems)) {
        QMessageBox::warning(this, tr("Error"), tr("Cannot set the items for check-result-list window. Some inconsistency."));
    } else {
        resultListWindow->showNormal();
    }

    _startCheckingButton->setEnabled(true);
}

void AppDatabaseUpdateCheckerMainWindow::saveFinalResultItems(QList<WebsiteListDatabaseItemModel> resultItems)
{
    QString inputDatabasePath = this->_databasePathLineEdit->text();
    WebsiteListDatabaseManager *websiteListDatabaseManager = new WebsiteListDatabaseManager;
    // remove the current content
    websiteListDatabaseManager->_completelyDeleteDatabase();
    // add the new (result) content
    if(!websiteListDatabaseManager->storeItems(resultItems)) {
        this->_addToStatus(tr("Results cannot be saved to file completely - some errors happened."));
    }
    else {
        // export
        if(websiteListDatabaseManager->exportDatabase(QString("%1").arg(inputDatabasePath))) {
            this->_addToStatus(tr("Results saved to file successfully: %1").arg(inputDatabasePath));
        }
        else {
            this->_addToStatus(tr("Results cannot be saved to file completely - export error."));
        }
    }
    delete websiteListDatabaseManager;
}

void AppDatabaseUpdateCheckerMainWindow::saveUpdatedAppLinks(QList<QString> urlList)
{
    QFileInfo inputDatabaseFileInfo(this->_databasePathLineEdit->text());
    QString linkOutputFilePath = inputDatabaseFileInfo.absolutePath() + "/updatedLinks.txt";

    QFile file(linkOutputFilePath);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);
    for(int i = 0; i < urlList.size(); i++) {
        out << urlList[i] << endl;
    }

    // optional, as QFile destructor will already do it:
    file.close();

    this->_addToStatus(tr("Updated urls saved to file successfully: %1").arg(linkOutputFilePath));
}
