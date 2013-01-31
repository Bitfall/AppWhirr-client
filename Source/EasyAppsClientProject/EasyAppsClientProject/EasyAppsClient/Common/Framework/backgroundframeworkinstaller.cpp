#include "backgroundframeworkinstaller.h"

#include <QDir>
#include <QFileInfo>
#include <QFileInfo>

#ifdef Q_OS_WIN
#include <windows.h>
#include <shellapi.h>
#endif

#include "Common/Helpers/pathhelper.h"

BackgroundFrameworkInstaller::BackgroundFrameworkInstaller(FrameworkCheckModel frameworkInstallModel) :
    QObject(NULL),
    _frameworkInstallModel(frameworkInstallModel)
{
}

void BackgroundFrameworkInstaller::startFrameworkInstall()
{
    QString pathForFwInstaller = _frameworkInstallModel.getFrameworkInstallerPath();
    if(pathForFwInstaller.isEmpty() || !QFile::exists(pathForFwInstaller)) {
        this->_emitFailedWithError("Installer not found.");
        return;
    }

    QFileInfo fwInstallerFileInfo(pathForFwInstaller);
    pathForFwInstaller = fwInstallerFileInfo.canonicalFilePath();
    pathForFwInstaller = PathHelper::makePathPlatformDependent(pathForFwInstaller);

    DLog("Final path for fw installer: ") << pathForFwInstaller;

#ifdef Q_OS_WIN
    WCHAR *win_pathForFwInstaller = (WCHAR *)malloc(sizeof(WCHAR) * (pathForFwInstaller.size() + 1));
    pathForFwInstaller.toWCharArray(win_pathForFwInstaller);
    // null terminate
    win_pathForFwInstaller[pathForFwInstaller.size()] = 0;
//        if(SUCCEEDED( CreateLink(win_shortcutTarget, win_shortcutFilePath, win_shortcutDescription, win_iconPath) )) {
//            isSucceeded = true;
//        }
//        if( CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE) == S_OK )
    SHELLEXECUTEINFO shExecInfo;
    shExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
    shExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_NOASYNC;
    shExecInfo.hwnd = NULL;
    shExecInfo.lpVerb = L"open"; // it can be replaced with 'open' but have to be tested! | 'runas' will _force_ admin rights, and it's a bit confusing on XP
    shExecInfo.lpFile = win_pathForFwInstaller;
    shExecInfo.lpParameters = NULL;
    shExecInfo.lpDirectory = NULL;
    shExecInfo.nShow = SW_SHOWNORMAL;
    shExecInfo.hInstApp = NULL;

    bool isShellExecSuccess = false;
    if(ShellExecuteEx(&shExecInfo)) {
        DLog("FW installer started successfully!");
        isShellExecSuccess = true;
    }
    else {
        // "To retrieve more accurate error information, use GetLastError. It may return one of the following values."
        //  http://msdn.microsoft.com/en-us/library/windows/desktop/bb762154(v=vs.85).aspx
        WLog("FW installer faild to start (can happen if the user has no admin right if required or if the user clicks 'cancel' in UAC)!");
    }
//        int result = ShellExecute(0, "runas", win_pathForFwInstaller, 0, 0, SW_SHOWNORMAL);
    free(win_pathForFwInstaller);

    if(!isShellExecSuccess) {
        this->_emitFailedWithError("Cannot start the installer. Probably User don't have sufficient privileges.");
        return;
    }

//        if (ShellExecuteEx(&shExInfo))
    {
//        DWORD theWaitTimeout = 1000 * 60 * 30; // 30 minutes
        DWORD dwExitCode = WaitForSingleObject(shExecInfo.hProcess, INFINITE);
        CloseHandle(shExecInfo.hProcess);

        DLog("FW install exit code: ") << dwExitCode;
    }
#endif

#ifdef Q_OS_MAC
    // !TODO!
#endif

    Q_EMIT finishedWithSuccess(_frameworkInstallModel);
    Q_EMIT finished();
}

void BackgroundFrameworkInstaller::_emitFailedWithError(QString errorMessage)
{
    Q_EMIT finishedWitError(_frameworkInstallModel, errorMessage);
}


