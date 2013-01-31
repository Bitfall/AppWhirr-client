#ifndef REMOVEFROMQUARANTINE_H
#define REMOVEFROMQUARANTINE_H

#include <QObject>

class RemoveFromQuarantine
{
public:
    RemoveFromQuarantine();

#ifdef Q_OS_MAC
    static int removeXAttr(const char* name, const char* fromFile, int options);
    static void releaseFromQuarantine(const char* file);
#endif
};

#endif // REMOVEFROMQUARANTINE_H
