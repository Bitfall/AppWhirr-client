#ifndef ENSUREAPPLICATIONPRESENTEDEVENT_H
#define ENSUREAPPLICATIONPRESENTEDEVENT_H

#include <QEvent>

class EnsureApplicationPresentedEvent : public QEvent
{
public:
    static inline Type getCustomEventType()
    {
        static Type sEventType = static_cast<QEvent::Type>(QEvent::registerEventType());
        return sEventType;
    }

    EnsureApplicationPresentedEvent();
};

#endif // ENSUREAPPLICATIONPRESENTEDEVENT_H
