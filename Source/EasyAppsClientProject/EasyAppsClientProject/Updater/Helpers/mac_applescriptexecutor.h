#ifndef MAC_APPLESCRIPTEXECUTOR_H
#define MAC_APPLESCRIPTEXECUTOR_H

#include <QObject>

#ifdef Q_OS_MAC

class Mac_AppleScriptExecutor : public QObject
{
    Q_OBJECT
public:
    explicit Mac_AppleScriptExecutor(QObject *parent = 0);
    
    static void executeAppleScript(QString appleScriptCode);
};

#endif // Q_OS_MAC

#endif // MAC_APPLESCRIPTEXECUTOR_H
