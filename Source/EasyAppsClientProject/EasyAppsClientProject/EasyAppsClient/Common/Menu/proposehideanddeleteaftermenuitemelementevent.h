#ifndef PROPOSEHIDEANDDELETEAFTERMENUITEMELEMENTEVENT_H
#define PROPOSEHIDEANDDELETEAFTERMENUITEMELEMENTEVENT_H

#include <QEvent>

class ProposeHideAndDeleteAfterMenuItemElementEvent : public QEvent
{
public:
    static inline Type getCustomEventType()
    {
        static Type sEventType = static_cast<QEvent::Type>(QEvent::registerEventType());
        return sEventType;
    }

    ProposeHideAndDeleteAfterMenuItemElementEvent();
};

#endif // PROPOSEHIDEANDDELETEAFTERMENUITEMELEMENTEVENT_H
