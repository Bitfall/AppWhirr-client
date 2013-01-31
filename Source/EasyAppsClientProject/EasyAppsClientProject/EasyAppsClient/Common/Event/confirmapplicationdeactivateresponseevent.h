#ifndef CONFIRMAPPLICATIONDEACTIVATERESPONSEEVENT_H
#define CONFIRMAPPLICATIONDEACTIVATERESPONSEEVENT_H

#include <QEvent>

class ConfirmApplicationDeactivateResponseEvent : public QEvent
{
public:
    static inline Type getCustomEventType()
    {
        static Type sEventType = static_cast<QEvent::Type>(QEvent::registerEventType());
        return sEventType;
    }

    ConfirmApplicationDeactivateResponseEvent();
};

#endif // CONFIRMAPPLICATIONDEACTIVATERESPONSEEVENT_H
