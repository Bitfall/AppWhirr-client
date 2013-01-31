#ifndef FORCEHIDEORMINIMIZEBROWSEREVENT_H
#define FORCEHIDEORMINIMIZEBROWSEREVENT_H

#include <QEvent>

class ForceHideOrMinimizeBrowserEvent : public QEvent
{
public:
    static inline Type getCustomEventType()
    {
        static Type sEventType = static_cast<QEvent::Type>(QEvent::registerEventType());
        return sEventType;
    }

    ForceHideOrMinimizeBrowserEvent();
};

#endif // FORCEHIDEORMINIMIZEBROWSEREVENT_H
