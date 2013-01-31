#ifndef FORCEHIDEORMINIMIZEEVENT_H
#define FORCEHIDEORMINIMIZEEVENT_H

#include <QEvent>

//
// use this if you want to hide or minimize the application
// the default Qt solution (showMinimized / hide) can cause
// bugs sometimes because of our workaround for ApplicationDeactivate
class ForceHideOrMinimizeEvent : public QEvent
{
public:
    static inline Type getCustomEventType()
    {
        static Type sEventType = static_cast<QEvent::Type>(QEvent::registerEventType());
        return sEventType;
    }

    ForceHideOrMinimizeEvent();
};

#endif // FORCEHIDEORMINIMIZEEVENT_H
