#ifndef CONFIRMAPPLICATIONDEACTIVATEREQUESTEVENT_H
#define CONFIRMAPPLICATIONDEACTIVATEREQUESTEVENT_H

#include <QEvent>

class ConfirmApplicationDeactivateRequestEvent: public QEvent
{
public:
    static inline Type getCustomEventType()
    {
        static Type sEventType = static_cast<QEvent::Type>(QEvent::registerEventType());
        return sEventType;
    }

    ConfirmApplicationDeactivateRequestEvent();
};

#endif // CONFIRMAPPLICATIONDEACTIVATEREQUESTEVENT_H
