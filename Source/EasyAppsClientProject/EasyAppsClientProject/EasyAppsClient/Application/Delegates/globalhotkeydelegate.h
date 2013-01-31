#ifndef GLOBALHOTKEYDELEGATE_H
#define GLOBALHOTKEYDELEGATE_H

#include <QObject>
#include <QWidget>

// WINDOWS
#ifdef Q_WS_WIN
#include <windows.h>
#endif

// MAC
#ifdef Q_WS_MAC
#include <Carbon/Carbon.h>
#endif


class GlobalHotkeyDelegate : public QObject
{
    Q_OBJECT
public:
    explicit GlobalHotkeyDelegate(QObject *parent = 0);

    void activateGlobalHotkey(WId platformDependentWindowId);
    void deactivateGlobalHotkey(WId platformDependentWindowId);

#ifdef Q_WS_WIN
    bool processWinEvent(MSG *message, long *result);
#endif

#ifdef Q_WS_MAC
private:
    EventHotKeyRef hotKeyRef;
#endif

};

#endif // GLOBALHOTKEYDELEGATE_H
