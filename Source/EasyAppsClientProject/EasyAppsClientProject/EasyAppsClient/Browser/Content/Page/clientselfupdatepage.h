#ifndef CLIENTSELFUPDATEPAGE_H
#define CLIENTSELFUPDATEPAGE_H

#include "Common/Page/pagewidgetbase.h"

QT_BEGIN_NAMESPACE
class QLabel;
class QPushButton;
class QTimer;
QT_END_NAMESPACE

#include "Common/Helpers/delayedretryhelper.h"

class MultiContentManager;

class ClientSelfUpdateAndInitialLocalAppDatabaseLoaderPage : public PageWidgetBase
{
    Q_OBJECT

private:
    enum PageModes {
        OnlyClientUpdateCheck,
        ClientUpdateCheckThenLoadLocalAppDatabase
    };

    enum PageContentTypeEnum {
        PCT_StatusContent = 0,
        PCT_RegisterNewAccountContent = 1
    };

public:
    explicit ClientSelfUpdateAndInitialLocalAppDatabaseLoaderPage(QString pageId, QWidget *parent = 0);

    //
    // for more information see the interface's documentation (in it's header file)
    void initializePageContent();
    void pagePushWillHappen();
    void pageWillAppear(QString param);
    void pageDidAppear();
    void pagePopWillHappen();
    void pageWillDisappear();

Q_SIGNALS:
    void finishedWithCheckingAndNoUpdateFound();

private Q_SLOTS:
    void mainActionButtonPressed();

    //
    void _clientSelfUpdateNewVersionFound();
    void _clientSelfUpdateNoNewVersionFound();
    void _clientSelfUpdateFailedToGetNewVersion(QString errorMessage);

    void _retryGetNewVersion();
    void _noMoreVersionCheckRetry();

    void _clientSelfUpdateDownloadFailedWithError(QString errorMessage);
    void _clientSelfUpdateDownloadProgress(int progressPercent);
    void _clientSelfUpdateDownloadFinishedSuccessfully();

    void _clientSelfUpdateInstallFailedWithError(QString errorMessage);
    void _clientSelfUpdateReadyToQuitAndInstall();

    // ------------------------------
    // --- app local scan related ---
    void _userLoginStarted();
    void _userLoggedIn();
    void _userLoginAborted();
    void _userLoginCommunicationError(QString errorMessage);
    void _userAppsLoadedToLocalAppDatabase();

    //
    void _registerNewAccount();

private:
    //
    // mainActionButtonText can be empty - in this case it won't change it's text
    void actualizeGuiToState(QString stateText, QString mainActionButtonText, bool isMainActionButtonEnabled, PageContentTypeEnum contentType = PCT_StatusContent);

    void _startToCheckForNewAvailableVersion();
    void _startToDownloadNewAvailableVersion();

    void finishedVersionChecking(bool isErrorHappened);
    void _finishedAndSwitchToStorePage(bool isErrorHappened);

    // ------------------------------
    // --- app local scan related ---
    void showLoginThenStartAppLocalScan();
    void _loginFinishedTryToStartLocalAppScan(bool isStartGrabSyncAfterLocalAppSearchIfUserSelected);

    void _connectToClientSelfUpdateManager();
    void _disconnectFromClientSelfUpdateManager();
    void _connectToUserLogin();
    void _disconnectFromUserLogin();

private:
    MultiContentManager *_pageContentManager;
    QLabel *_statusLabel;
    QPushButton *_mainActionButton;

    DelayedRetryHelper *_versionCheckDelayedRetryHelper;
    PageModes _currPageMode;

    bool _isErrorHappenedDuringClientSelfUpdateCheck;
    bool _isStartGrabSyncAfterLocalAppSearch;
    bool _isInAutoLoginMode;
};

#endif // CLIENTSELFUPDATEPAGE_H
