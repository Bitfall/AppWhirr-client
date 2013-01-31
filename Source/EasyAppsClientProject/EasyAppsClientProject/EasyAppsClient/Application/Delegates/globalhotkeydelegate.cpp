#include "globalhotkeydelegate.h"

#include <QMainWindow>
#include "Application/appwhirrapplication.h"

// utility
#include "Common/Event/ensureapplicationpresentedevent.h"

// config
#include "Settings/configmanager.h"

// MAC
#ifdef Q_WS_MAC
// Key handler under mac
OSStatus cocoaKeyHandler(EventHandlerCallRef nextHandler, EventRef theEvent, void* userData);
#endif

// WIN
#ifdef Q_WS_WIN
#pragma comment(lib,"user32.lib")
#endif

GlobalHotkeyDelegate::GlobalHotkeyDelegate(QObject *parent) :
    QObject(parent)
{
}

#ifdef Q_WS_WIN
bool GlobalHotkeyDelegate::processWinEvent(MSG *message, long *result)
{
    if (message != NULL && message->message == WM_HOTKEY)
    {
#if defined(ENABLE_HOTKEY_EVENT_LOGGING)
        qDebug("hotkey to show");
#endif
        //this->ensureWindowPresented();
        //QApplication::postEvent(this->parent(), new EnsureApplicationPresentedEvent());
//        MyApplication::instance()->postEventToLauncherWindow(new EnsureApplicationPresentedEvent());
        AppwhirrApplication::instance()->postEventToBrowserWindow(new EnsureApplicationPresentedEvent());
        return true;
    }

    return false;
}
#endif

#ifdef Q_WS_MAC
OSStatus cocoaKeyHandler(EventHandlerCallRef nextHandler, EventRef theEvent, void* userData) {
    EventHotKeyID hotKeyID;
    GetEventParameter(theEvent,kEventParamDirectObject,typeEventHotKeyID, NULL,sizeof(hotKeyID),NULL, &hotKeyID);

    int temphotKeyId = hotKeyID.id;
    switch(temphotKeyId) {
    case 1:
        // Activate the window
        //QApplication::postEvent(((GlobalHotkeyDelegate*)userData)->parent(), new EnsureApplicationPresentedEvent());
        AppwhirrApplication::instance()->postEventToBrowserWindow(new EnsureApplicationPresentedEvent());
        return true;
    }
}
#endif

void GlobalHotkeyDelegate::activateGlobalHotkey(WId platformDependentWindowId)
{
#if defined(ENABLE_HOTKEY_EVENT_LOGGING)
        qDebug("activateGlobalHotkey");
#endif

#ifdef Q_WS_WIN
//    qDebug("win global shortcut - activate");

    // "In Windows XP and previous versions of Windows, if a hot key already exists with the same hWnd and id
    //  parameters, it is replaced by the new hot key. In subsequent versions of Windows,
    //  if a hot key already exists with the same hWnd and id parameters, it is maintained along with
    //  the new hot key. In these versions of Windows, the application must explicitly call
    //  UnregisterHotKey to unregister the old hot key."
    // from: http://msdn.microsoft.com/en-us/library/ms646309(v=vs.85).aspx

    UnregisterHotKey(platformDependentWindowId, ConfigManager::_win_getAppwhirrClientWindowsGlobalShortcutId());
    RegisterHotKey(platformDependentWindowId, ConfigManager::_win_getAppwhirrClientWindowsGlobalShortcutId(), MOD_CONTROL | MOD_SHIFT, VK_SPACE);
#endif

#ifdef Q_WS_MAC
    qDebug("mac global shortcut - with Carbon");

    this->deactivateGlobalHotkey(platformDependentWindowId);

    // Carbon Event Handler
    EventTypeSpec eventType = {kEventClassKeyboard, kEventHotKeyPressed};

    // Register the event
    OSStatus errEvent = InstallApplicationEventHandler(&cocoaKeyHandler, 1, &eventType, (void*)this, NULL);
    if (errEvent) qDebug("Failed to register keyboard event handler for global shortcut: %d", errEvent);

    // Register the hotkey
    OSStatus errHotKey;
    EventHotKeyID hotKeyID = {'EasyAppsHKForWindowActivation',  1};

    errHotKey = RegisterEventHotKey(kVK_Space, // 'space' key
                                    controlKey | shiftKey,
                                    hotKeyID,
                                    GetApplicationEventTarget(),
                                    0,
                                    &hotKeyRef);

    if(errHotKey) qDebug("Failed to register hot key: %d", errHotKey);
#endif
}

void GlobalHotkeyDelegate::deactivateGlobalHotkey(WId platformDependentWindowId)
{
#if defined(ENABLE_HOTKEY_EVENT_LOGGING)
        qDebug("deactivateGlobalHotkey");
#endif

#ifdef Q_WS_WIN
#ifdef ENABLE_HOTKEY_EVENT_LOGGING
    DLog("win global shortcut - deactivate");
#endif
    UnregisterHotKey(platformDependentWindowId, ConfigManager::_win_getAppwhirrClientWindowsGlobalShortcutId());
#endif

#ifdef Q_WS_MAC
    OSStatus error = UnregisterEventHotKey(hotKeyRef);
    if (error) qDebug("Failed to unregister hot key");
#endif
}
