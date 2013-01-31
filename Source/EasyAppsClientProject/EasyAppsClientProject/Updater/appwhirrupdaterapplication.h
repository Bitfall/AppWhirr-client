#ifndef APPWHIRRUPDATERAPPLICATION_H
#define APPWHIRRUPDATERAPPLICATION_H

#include <QApplication>

QT_BEGIN_NAMESPACE
class QTimer;
QT_END_NAMESPACE

#include "BackgroundClientInstaller/backgroundclientinstaller.h"

class AppWhirrUpdaterMainWindow;
class AppInstallManager;

class AppWhirrUpdaterApplication : public QApplication
{
    Q_OBJECT

public:
    enum AppWhirrUpdaterModes {
        AppInstallOrUpdate = 2,
        ClientSelfUpdate = 3
    };

public:
    explicit AppWhirrUpdaterApplication(int &argc, char **argv, int = QT_VERSION);
//    ~AppWhirrUpdaterApplication();

    void startAppInstallOrUpdate(QString targetZip, QString appId);
    void startClientSelfUpdate(QString clientRootPath);
    void startAppWhirrRestart(QString appWhirrClientExecutablePath, QString appWhirrExecutableWorkingDirPath);

private Q_SLOTS:
    void clientInstallFailedWithError(QString errorMessage, InstallMajorSteps lastStepDone);
    void clientInstallFinishedSuccessfully();

    void tryToInstall();

    void _doAppWhirrRestart();

private:
    AppWhirrUpdaterMainWindow *_updaterMainWindow;
    AppInstallManager *_appInstallManager;

    QString _clientRootPath;
    QTimer *_retryClientInstallTimer;
    int _retryClientInstallCountLeft;
    InstallMajorSteps _fallbackInstallStep;

    QString _awRestart_awClientExecutablePath;
    QString _awRestart_awClientExecutableWorkingDirPath;
};

#endif // APPWHIRRUPDATERAPPLICATION_H
