#include "awqprocessextension.h"

#include <QStringList>

#ifdef Q_OS_WIN
#include <windows.h>
#include <shellapi.h>

#include "Common/Helpers/qstringhelper.h"
#endif

#ifdef Q_OS_MAC
#include <QProcess>
#endif

AWQProcessExtension::AWQProcessExtension(QObject *parent) :
    QObject(parent)
{
}

bool AWQProcessExtension::startDetachedAndAllowElevation(const QString &programPath, const QStringList &arguments, const QString &workingDirectoryPath)
{

#ifdef Q_OS_MAC
    return QProcess::startDetached(programPath,
                                   arguments,
                                   workingDirectoryPath);
#endif

#ifdef Q_OS_WIN

    // convert parameters for WinAPI
    QString argsAsString = QStringHelper::convertQStringListToQuotedSpaceSeparatedQString(arguments);
    DLog("######## startDetached:") << programPath << workingDirectoryPath << argsAsString << "| orig args:" << arguments;
    WCHAR *winapi_argsAsString = NULL;
    if(!argsAsString.isEmpty()) {
        winapi_argsAsString = (WCHAR *)malloc(sizeof(WCHAR) * (argsAsString.size() + 1));
        argsAsString.toWCharArray(winapi_argsAsString);
        // null terminate
        winapi_argsAsString[argsAsString.size()] = 0;
    }

    WCHAR *winapi_programPath = NULL;
    if(!programPath.isEmpty()) {
        winapi_programPath = (WCHAR *)malloc(sizeof(WCHAR) * (programPath.size() + 1));
        programPath.toWCharArray(winapi_programPath);
        // null terminate
        winapi_programPath[programPath.size()] = 0;
    }

    WCHAR *winapi_workingDirectoryPath = NULL;
    if(workingDirectoryPath.isEmpty()) {
        winapi_workingDirectoryPath = (WCHAR *)malloc(sizeof(WCHAR) * (workingDirectoryPath.size() + 1));
        workingDirectoryPath.toWCharArray(winapi_workingDirectoryPath);
        // null terminate
        winapi_workingDirectoryPath[workingDirectoryPath.size()] = 0;
    }

//        if(SUCCEEDED( CreateLink(win_shortcutTarget, win_shortcutFilePath, win_shortcutDescription, win_iconPath) )) {
//            isSucceeded = true;
//        }
//        if( CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE) == S_OK )
    SHELLEXECUTEINFO shExecInfo;
    shExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
    shExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_NOASYNC;
    shExecInfo.hwnd = NULL;
    shExecInfo.lpVerb = L"open"; // it can be replaced with 'open' but have to be tested! | 'runas' will _force_ admin rights, and it's a bit confusing on XP
    shExecInfo.lpFile = winapi_programPath;
    shExecInfo.lpParameters = winapi_argsAsString;
    shExecInfo.lpDirectory = winapi_workingDirectoryPath;
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
    free(winapi_programPath);
    free(winapi_workingDirectoryPath);
    free(winapi_argsAsString);

    return isShellExecSuccess;
#endif
}
