#ifndef AUTOSTARTUPDELEGATE_H
#define AUTOSTARTUPDELEGATE_H

#include <QObject>

// MAC
#ifdef Q_WS_MAC
#include <Carbon/Carbon.h>
#endif

class AutoStartupDelegate : public QObject
{
    Q_OBJECT
public:
    explicit AutoStartupDelegate(QObject *parent = 0);

    bool willStartAtSystemStartup(QString& applicationPath);
    bool makeItAutoStartAtSystemStartup();
    bool removeItFromAutoStartList();

};

#endif // AUTOSTARTUPDELEGATE_H
