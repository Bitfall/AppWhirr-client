#include "criticalactionmanager.h"

CriticalActionManager::CriticalActionManager(QObject *parent) :
    QObject(parent)
{
}

void CriticalActionManager::addCriticalAction(int criticalActionCode)
{
    _mutex.lock();

    if(_criticalActionReferenceMap.contains(criticalActionCode)) {
        int refCount = _criticalActionReferenceMap.value(criticalActionCode);
        ++refCount;
        _criticalActionReferenceMap[criticalActionCode] = refCount;
#ifdef ENABLE_CRITICAL_ACTION_LOGGING
        DLog("Critical action - add - reference incremented: ") << criticalActionCode << refCount;
#endif
    }
    else {
        // not yet referenced - set the reference-count to 1
        _criticalActionReferenceMap[criticalActionCode] = 1;

#ifdef ENABLE_CRITICAL_ACTION_LOGGING
        DLog("Critical action - add - new reference, set to 1: ") << criticalActionCode;
#endif
    }

    _mutex.unlock();
}

bool CriticalActionManager::isThisCriticalActionActive(int criticalActionCode)
{
    _mutex.lock();

    bool result = false;
    if(this->_criticalActionReferenceMap.contains(criticalActionCode)) {
        if(this->_criticalActionReferenceMap.value(criticalActionCode) > 0) {
            result = true;
        }
    }

    _mutex.unlock();
    return result;
}

void CriticalActionManager::removeCriticalAction(int criticalActionCode)
{
    _mutex.lock();

    if(this->_criticalActionReferenceMap.contains(criticalActionCode)) {
        int refCount = _criticalActionReferenceMap.value(criticalActionCode);
        --refCount;
        if(refCount <= 0) {
            // remove
            _criticalActionReferenceMap.remove(criticalActionCode);

#ifdef ENABLE_CRITICAL_ACTION_LOGGING
        DLog("Critical action - remove - removed totally: ") << criticalActionCode;
#endif
        }
        else {
            // decrement
            _criticalActionReferenceMap[criticalActionCode] = refCount;

#ifdef ENABLE_CRITICAL_ACTION_LOGGING
        DLog("Critical action - remove - reference decremented: ") << criticalActionCode << refCount;
#endif
        }
    }

    _mutex.unlock();
}

bool CriticalActionManager::isAnyCriticalActionActive()
{
    _mutex.lock();

    bool result = (_criticalActionReferenceMap.size() > 0);

#ifdef ENABLE_CRITICAL_ACTION_LOGGING
    DLog(" --- Critical action - isAnyCriticalActionActive - full list");
    QMap<int, int>::const_iterator i = _criticalActionReferenceMap.constBegin();
    for(; i != _criticalActionReferenceMap.constEnd(); i++) {
        DLogS << "Action code: " << i.key() << " | ref-count: " << i.value();
    }
    DLog(" --- end of list");
#endif

    _mutex.unlock();
    return result;
}
