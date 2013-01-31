#ifndef PROPOSEHIDEMENUITEMELEMENTEVENT_H
#define PROPOSEHIDEMENUITEMELEMENTEVENT_H

#include <QEvent>

class ProposeHideMenuItemElementEvent : public QEvent
{
public:
    static inline Type getCustomEventType()
    {
        static Type sEventType = static_cast<QEvent::Type>(QEvent::registerEventType());
        return sEventType;
    }

    ProposeHideMenuItemElementEvent();
};

#endif // PROPOSEHIDEMENUITEMELEMENTEVENT_H
