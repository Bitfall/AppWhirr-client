#include "urlschemehandlerdelegate.h"

#include <QMessageBox>

// utility
#include "Application/appspecificpathhelper.h"
#include "Common/Helpers/pathhelper.h"

// config
#define APPWHIRR_CUSTOM_URL_SCHEME_PREFIX "appwhirr"

// platform spec
#ifdef Q_WS_WIN
#include <windows.h>
//#include <Tchar.h>
//#pragma comment(lib,"shell32.lib")
#pragma comment(lib,"advapi32.lib")
#endif



UrlSchemeHandlerDelegate::UrlSchemeHandlerDelegate(QObject *parent) :
    QObject(parent)
{
}

QString UrlSchemeHandlerDelegate::getSchemePrefix() {
    return APPWHIRR_CUSTOM_URL_SCHEME_PREFIX;
}

QString UrlSchemeHandlerDelegate::getSchemeFixFirstPart() {
    return QString("%1://").arg(getSchemePrefix());
}

QString UrlSchemeHandlerDelegate::getShowAppDetailSchemeContentPart() {
    return QString("appdetail");
}

QString UrlSchemeHandlerDelegate::getRunAppSchemeContentPart() {
    return QString("runapp");
}

QString UrlSchemeHandlerDelegate::getSearchInStoreSchemeContentPart()
{
    return QString("searchinstore");
}

QString UrlSchemeHandlerDelegate::createSchemeUrlForRunApp(QString appId)
{
    return QString("%1%2/%3").arg(UrlSchemeHandlerDelegate::getSchemeFixFirstPart()).arg(UrlSchemeHandlerDelegate::getRunAppSchemeContentPart()).arg(appId);
}

void UrlSchemeHandlerDelegate::__schemeCalledWithURL(QString url)
{
//    QMessageBox::warning(0, "Open URL scheme", url);
    // void openAppDetailPageForAppId(QString appId);
    DLog("Sceme url: ") << url;

    QString schemeFirstPart = this->getSchemePrefix()+"://";
    if(url.startsWith(schemeFirstPart)) {
        QString schemeContentPart = url;
        schemeContentPart.remove(0, schemeFirstPart.length());
        DLog("Scheme content part: ") << schemeContentPart;

        QStringList contentParts = schemeContentPart.split("/", QString::SkipEmptyParts);
        if(!contentParts.isEmpty()) {
            if(contentParts[0] == UrlSchemeHandlerDelegate::getShowAppDetailSchemeContentPart()) {
                if(contentParts.size() < 2) {
                    WLog("Invalid app specifier.");
                }
                else {
                    QString foundAppId = contentParts[1];
                    DLog("AppId found in scheme to show details.");
                    Q_EMIT openAppDetailPageForAppId(foundAppId);

//                    QMessageBox::warning(0, "Show app details for appId", foundAppId);
                }
            }
            else if(contentParts[0] == UrlSchemeHandlerDelegate::getRunAppSchemeContentPart()) {
                if(contentParts.size() < 2) {
                    WLog("Invalid app specifier.");
                }
                else {
                    QString foundAppId = contentParts[1];
                    DLog("AppId found in scheme to RUN.");
                    Q_EMIT runAppForAppId(foundAppId);

//                    QMessageBox::warning(0, "Show app details for appId", foundAppId);
                }
            }
            else if(contentParts[0] == UrlSchemeHandlerDelegate::getSearchInStoreSchemeContentPart()) {
                if(contentParts.size() < 2) {
                    WLog("Invalid app specifier.");
                }
                else {
                    QString foundSearchFilterText = contentParts[1];
                    DLog("Search in store with filter-text:") << foundSearchFilterText;
                    Q_EMIT searchInStore(foundSearchFilterText);
                }
            }
            else {
                WLog("Invalid content part - command.");
            }
        }
        else {
            WLog("Invalid content part");
        }
    }
    else {
        WLog("Invalid scheme!");
    }
}

void UrlSchemeHandlerDelegate::registerUrlSchemeHandler()
{
#ifdef Q_WS_MAC
    setupMacEvents();
#endif

#ifdef Q_WS_WIN
    this->win_registerUrlSchemeHandler();
#endif
}

void UrlSchemeHandlerDelegate::unregisterUrlSchemeHandler()
{
#ifdef Q_WS_MAC
    removeMacEvents();
#endif

#ifdef Q_WS_WIN
    this->win_unregisterUrlSchemeHandler();
#endif
}

#ifdef Q_WS_WIN
void UrlSchemeHandlerDelegate::win_registerUrlSchemeHandler() {
    this->win_RegisterCustomUrlSchemeToRegistry(this->getSchemePrefix(), AppSpecificPathHelper::getClientExecutablePath(), AppSpecificPathHelper::getClientExecutablePath());
}

void UrlSchemeHandlerDelegate::win_unregisterUrlSchemeHandler() {
    LOG_AS_NOT_IMPLEMENTED_WITH_HINT("It's safe to never remove it - registering a new one will overwrite the old settings in the registry.");
}


bool UrlSchemeHandlerDelegate::win_RegisterCustomUrlSchemeToRegistry(QString urlScheme, QString targetAppPathToCall, QString iconPath)
{
    bool isSuccess = true;
    HKEY hKey;
    HKEY hRootSchemeKey;
    LPCTSTR sk = TEXT("Software\\Classes");

    targetAppPathToCall = PathHelper::makePathPlatformDependent(targetAppPathToCall);
    iconPath = PathHelper::makePathPlatformDependent(iconPath);

    //
    // step 1: open the root of url scheme handlers
    LONG regOpResult = RegOpenKeyEx(HKEY_CURRENT_USER, sk, 0, KEY_ALL_ACCESS , &hKey);

    if(regOpResult != ERROR_SUCCESS) {
        WLog("Cannot open the root registry key.");
        return false;
    }

    //
    // step 2: create the base scheme key ----------------
    regOpResult = RegCreateKeyEx(hKey, urlScheme.utf16(), 0, 0, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hRootSchemeKey, NULL);

    if (regOpResult == ERROR_SUCCESS) {
        DLog("Success creating and opening root scheme key.");

        // default value
        QString protocolName = "URL:"+urlScheme+" protocoll";
        regOpResult = RegSetValueExW(hRootSchemeKey, NULL, 0, REG_SZ, (const BYTE*)protocolName.toStdWString().c_str(), (lstrlen(protocolName.toStdWString().c_str()) + 1) * sizeof(WCHAR));

        if(regOpResult == ERROR_SUCCESS) {
            // URL Protocol value
            regOpResult = RegSetValueEx(hRootSchemeKey, TEXT("URL Protocol"), 0, REG_SZ, 0, 0);
        }
    }
    else {
        WLog("Error opening root scheme key.");
        isSuccess = false;
    }

    if (regOpResult != ERROR_SUCCESS){
        WLog("Error creating/set root scheme values.");
        isSuccess = false;
    }

    if(RegCloseKey(hKey) != ERROR_SUCCESS) {
        WLog("Cannot close the root registry key.");
        isSuccess = false;
    }

    //
    // step 3: create sub-keys ---------------------

    // shell subkey tree
    regOpResult = RegCreateKeyEx(hRootSchemeKey, TEXT("shell\\open\\command"), 0, 0, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL);

    if (regOpResult == ERROR_SUCCESS) {
        DLog("Success creating and opening shell subtree keys.");

        // default value
        QString commandString = "\"" + targetAppPathToCall + "\" \"%1\"";
        regOpResult = RegSetValueExW(hKey, NULL, 0, REG_SZ, (CONST BYTE*)commandString.toStdWString().c_str(), (lstrlen(commandString.toStdWString().c_str()) + 1) * sizeof(WCHAR));

        if (regOpResult != ERROR_SUCCESS){
            WLog("Error creating/set command key values.");
            isSuccess = false;
        }
    } else {
        WLog("Error creating shell subtree keys.");
        isSuccess = false;
    }

    if(RegCloseKey(hKey) != ERROR_SUCCESS) {
        WLog("Cannot close the shell subtree keys.");
        isSuccess = false;
    }

    // DefaultIcon subkey
    regOpResult = RegCreateKeyEx(hRootSchemeKey, TEXT("DefaultIcon"), 0, 0, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL);

    if(regOpResult == ERROR_SUCCESS) {
        DLog("Success creating and opening DefaultIcon sub-key.");

        QString iconString = "\"" + iconPath + "\"";
        regOpResult = RegSetValueExW(hKey, NULL, 0, REG_SZ, (CONST BYTE*)iconString.toStdWString().c_str(), (lstrlen(iconString.toStdWString().c_str()) + 1) * sizeof(WCHAR));

        if (regOpResult != ERROR_SUCCESS){
            WLog("Error creating/set DefaultIcon key values.");
            isSuccess = false;
        }
    }
    else {
        WLog("Error: creating and opening DefaultIcon sub-key!");
        isSuccess = false;
    }

    if(RegCloseKey(hKey) != ERROR_SUCCESS) {
        WLog("Cannot close the DefaultIcon subkey.");
        isSuccess = false;
    }

    if(RegCloseKey(hRootSchemeKey) != ERROR_SUCCESS) {
        WLog("Cannot close the root scheme key.");
        isSuccess = false;
    }

    if(!isSuccess) {
        WLog("Couldn't create the required registry keys.");
        return false;
    }

    return isSuccess;
}

#endif
