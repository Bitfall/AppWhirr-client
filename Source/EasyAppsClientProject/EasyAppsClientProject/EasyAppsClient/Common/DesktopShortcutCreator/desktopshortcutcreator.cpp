#include "desktopshortcutcreator.h"

#include <QProcess>
#include <QDir>
#include <QFileInfo>
#include <QTextStream>

#ifdef Q_WS_WIN
#include "windows.h"

HRESULT CreateLink(LPCWSTR lpszPathObj, LPCWSTR lpszPathLink, LPCWSTR lpszDesc, LPCWSTR lpszIconLocation);
#endif

#ifdef Q_WS_MAC
#include <Carbon/Carbon.h>
#endif

#include "Common/Helpers/pathhelper.h"
#include "Common/Helpers/qstringhelper.h"

DesktopShortcutCreator::DesktopShortcutCreator()
{
}

// static
bool DesktopShortcutCreator::createShortcut(QString pathToSaveShortcutFile, QString pathToShortcutTarget, QString iconPath, QString shortcutDescription, bool isNormalizePathesForCurrentPlatform)
{
    bool isSucceeded = false;

    if(isNormalizePathesForCurrentPlatform) {
        pathToSaveShortcutFile = PathHelper::makePathPlatformDependent(pathToSaveShortcutFile);
        pathToShortcutTarget = PathHelper::makePathPlatformDependent(pathToShortcutTarget);
        iconPath = PathHelper::makePathPlatformDependent(iconPath);
    }

    DLog("Save path: ") << pathToSaveShortcutFile;
    DLog("Target path: ") << pathToShortcutTarget;
    DLog("Short description: ") << shortcutDescription;
    DLog("Icon path: ") << iconPath;

    if (pathToSaveShortcutFile.contains("~")) {
        pathToSaveShortcutFile.replace("~", QDir::homePath());
    }

#ifdef Q_WS_WIN

    WCHAR *win_shortcutFilePath = (WCHAR *)malloc(sizeof(WCHAR) * (pathToSaveShortcutFile.size() + 1));
    WCHAR *win_shortcutTarget = (WCHAR *)malloc(sizeof(WCHAR) * (pathToShortcutTarget.size() + 1));
    WCHAR *win_shortcutDescription = (WCHAR *)malloc(sizeof(WCHAR) * (shortcutDescription.size() + 1));
    pathToSaveShortcutFile.toWCharArray(win_shortcutFilePath);
    pathToShortcutTarget.toWCharArray(win_shortcutTarget);
    shortcutDescription.toWCharArray(win_shortcutDescription);
    // null terminate them
    win_shortcutFilePath[pathToSaveShortcutFile.size()] = 0;
    win_shortcutTarget[pathToShortcutTarget.size()] = 0;
    win_shortcutDescription[shortcutDescription.size()] = 0;

    // icon path with test
    WCHAR *win_iconPath = NULL;
    if(!iconPath.isEmpty()) {
        win_iconPath = (WCHAR *)malloc(sizeof(WCHAR) * (iconPath.size() + 1));
        iconPath.toWCharArray(win_iconPath);
        win_iconPath[iconPath.size()] = 0;
    }
    if(SUCCEEDED( CreateLink(win_shortcutTarget, win_shortcutFilePath, win_shortcutDescription, win_iconPath) )) {
        isSucceeded = true;
    }
    free(win_shortcutFilePath);
    free(win_shortcutTarget);
    free(win_shortcutDescription);
    if(win_iconPath != NULL) {
        free(win_iconPath);
    }
#endif

#ifdef Q_WS_MAC
    if (pathToShortcutTarget.startsWith("appwhirr://")) {
        // If we have a URL shortcut create an application that the user can execute
        QDir fullPath(pathToSaveShortcutFile);
        QString newDirectory = fullPath.dirName();
        QDir destination(pathToSaveShortcutFile.remove(newDirectory));

        QString iconRelativePath;

        if (QDir(PathHelper::combineAndCleanPathes(destination.absolutePath(), newDirectory)).exists()) {
            PathHelper::deleteThisDirectoryAndEverythingBelow(PathHelper::combineAndCleanPathes(destination.absolutePath(), newDirectory));
        }
        destination.mkdir(newDirectory);

        // Get the bundle
        CFURLRef bundleURL;
        CFBundleRef applicationBundle;

        CFStringRef iconPathString = CFStringCreateWithCharacters(0,
                                                                  reinterpret_cast<const UniChar *>(iconPath.unicode()),
                                                                  iconPath.length());
        bundleURL = CFURLCreateWithFileSystemPath(kCFAllocatorDefault,
                                                  iconPathString,
                                                  kCFURLPOSIXPathStyle,
                                                  true);

        applicationBundle = CFBundleCreate(kCFAllocatorDefault, bundleURL);
        CFRelease(bundleURL);

        // Get bundle information
        CFDictionaryRef bundleInfoDict;
        CFStringRef     iconFilePath;

        // Get an instance of the non-localized keys.
        bundleInfoDict = CFBundleGetInfoDictionary(applicationBundle);

        // If we succeeded, look for our property.
        if ( bundleInfoDict != NULL ) {
                iconFilePath = (CFStringRef)CFDictionaryGetValue(bundleInfoDict,
                                                                 CFSTR("CFBundleIconFile") );

                iconRelativePath = "Contents/Resources/" + QStringHelper::CFStringToQString(iconFilePath);
                if (!iconRelativePath.endsWith(".icns", Qt::CaseInsensitive))
                    iconRelativePath = iconRelativePath.append(".icns");
        }

        // Copy icon
        PathHelper::ensureDirectoryCreated(PathHelper::combineAndCleanPathes(fullPath.absolutePath(),
                                                                             "Contents/Resources"));
        DLog(PathHelper::combineAndCleanPathes(iconPath,
                                               iconRelativePath));
        QFile::copy(PathHelper::combineAndCleanPathes(iconPath,
                                                      iconRelativePath),
                    PathHelper::combineAndCleanPathes(fullPath.absolutePath(),
                                                      "Contents/Resources/icon.icns"));

        // Create shell script
        QString executablePath = PathHelper::combineAndCleanPathes(fullPath.absolutePath(),
                                                                   "Contents/MacOS");
        PathHelper::ensureDirectoryCreated(executablePath);
        QFile script(PathHelper::combineAndCleanPathes(executablePath,
                                                       "run.sh"));
        if (script.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream stream(&script);

            stream << "#!/bin/bash\n"
                   << "open "
                   << pathToShortcutTarget;

            script.close();

            script.setPermissions(QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner |
                                  QFile::ReadGroup | QFile::WriteGroup | QFile::ExeGroup |
                                  QFile::ReadOther | QFile::WriteOther | QFile::ExeOther);
        }

        // Info.plist
        QFile plist(PathHelper::combineAndCleanPathes(fullPath.absolutePath(),
                                                      "Contents/Info.plist"));
        if (plist.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream stream(&plist);

            stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                   << "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n"
                   << "<plist version=\"1.0\">\n"
                   << "<dict>\n"
                   << "<key>CFBundleExecutable</key>\n"
                   << "<string>run.sh</string>\n"
                   << "<key>CFBundleIconFile</key>\n"
                   << "<string>icon.icns</string>\n"
                   << "<key>CFBundleInfoDictionaryVersion</key>\n"
                   << "<string>1.0</string>\n"
                   << "<key>CFBundlePackageType</key>\n"
                   << "<string>APPL</string>\n"
                   << "<key>CFBundleSignature</key>\n"
                   << "<string>????</string>\n"
                   << "<key>CFBundleVersion</key>\n"
                   << "<string>1.0</string>\n"
                   << "</dict>\n"
                   << "</plist>";

            plist.close();
        }

        isSucceeded = true;
    } else {
        // Create alias with applescript
        QDir target(pathToShortcutTarget);
        target.cdUp();
        target.cdUp();
        target.cdUp();
        QFileInfo app(target.absolutePath());

        QString createAliasWithFinder =
                "tell application \"Finder\"\n"
                "make new alias file at desktop to file \"" + app.fileName() + "\" ";

        while (target.cdUp() && !target.dirName().isEmpty())
            createAliasWithFinder += "of folder \"" + target.dirName() + "\" ";
        createAliasWithFinder += "of startup disk\nend tell";

        QString osascript = "/usr/bin/osascript";
        QStringList processArguments;
        processArguments << "-l" << "AppleScript";

        QProcess p;
        p.start(osascript, processArguments);
        p.write(createAliasWithFinder.toUtf8());
        p.closeWriteChannel();
        isSucceeded = p.waitForFinished();
    }
#endif

    return isSucceeded;
}



#ifdef Q_WS_WIN

//
// based on: http://msdn.microsoft.com/en-us/library/aa969393.aspx
//


//#include "stdafx.h"
#include "winnls.h"
#ifdef __MINGW32__
    #include "shlobj.h"
#else // MSVC
    #include "shobjidl.h"
#endif
#include "objbase.h"
#include "objidl.h"
#include "shlguid.h"

#pragma comment(lib,"user32.lib")
#pragma comment(lib,"shell32.lib")
#pragma comment(lib,"Ole32.lib")


// CreateLink - Uses the Shell's IShellLink and IPersistFile interfaces
//              to create and store a shortcut to the specified object.
//
// Returns the result of calling the member functions of the interfaces.
//
// Parameters:
// lpszPathObj  - Address of a buffer that contains the path of the object,
//                including the file name.
// lpszPathLink - Address of a buffer that contains the path where the
//                Shell link is to be stored, including the file name.
// lpszDesc     - Address of a buffer that contains a description of the
//                Shell link, stored in the Comment field of the link
//                properties.
//
// if lpszIconLocation == NULL then it won't be set
//
HRESULT CreateLink(LPCWSTR lpszPathObj, LPCWSTR lpszPathLink, LPCWSTR lpszDesc, LPCWSTR lpszIconLocation)
{
    HRESULT hres;
    IShellLink* psl;

    // Get a pointer to the IShellLink interface. It is assumed that CoInitialize
    // has already been called.
    hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl);
    if (SUCCEEDED(hres))
    {
        IPersistFile* ppf;

        // Set the path to the shortcut target and add the description.
        psl->SetPath(lpszPathObj);
        psl->SetDescription(lpszDesc);
        DLog("Win_IconPath: ") << lstrlen(lpszIconLocation);
        if(lpszIconLocation != NULL) {
            psl->SetIconLocation(lpszIconLocation, 0);
        }

        // Query IShellLink for the IPersistFile interface, used for saving the
        // shortcut in persistent storage.
        hres = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf);

        if (SUCCEEDED(hres))
        {
//            WCHAR wsz[MAX_PATH];

//            // Ensure that the string is Unicode.
//            MultiByteToWideChar(CP_ACP, 0, lpszPathLink, -1, wsz, MAX_PATH);

            // Add code here to check return value from MultiByteWideChar
            // for success.

            // Save the link by calling IPersistFile::Save.
//            hres = ppf->Save(wsz, TRUE);
            hres = ppf->Save(lpszPathLink, TRUE);
            ppf->Release();
        }
        psl->Release();
    }
    return hres;
}

#endif
