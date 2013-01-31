#ifndef CRITICALACTIONMANAGER_H
#define CRITICALACTIONMANAGER_H

#include <QObject>
#include <QMutex>
#include <QMap>

/*! Manages critical-action blocks with reference counting.

    You can add (increment) and remove (decrement) critical action sections/blocks and test for the critical-action-code when needed. For example if you want to present a warning dialog if any critical action is active and the user wants to quit from the app you can add/remove your critical actions and test for a specific one or for all.

    @note
    For critical-action-codes check the CriticalActionCodesEnum
*/
class CriticalActionManager : public QObject
{
    Q_OBJECT
public:
    explicit CriticalActionManager(QObject *parent = 0);

    void addCriticalAction(int criticalActionCode);
    bool isThisCriticalActionActive(int criticalActionCode);
    void removeCriticalAction(int criticalActionCode);

    bool isAnyCriticalActionActive();

private:
    QMutex _mutex;
    QMap<int, int> _criticalActionReferenceMap;
};

#endif // CRITICALACTIONMANAGER_H
