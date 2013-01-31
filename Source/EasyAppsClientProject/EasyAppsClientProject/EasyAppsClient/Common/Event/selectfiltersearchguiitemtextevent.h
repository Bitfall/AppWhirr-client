#ifndef SELECTFILTERSEARCHGUIITEMTEXTEVENT_H
#define SELECTFILTERSEARCHGUIITEMTEXTEVENT_H

#include <QEvent>

class SelectFilterSearchGuiItemTextEvent : public QEvent
{
public:
    static inline Type getCustomEventType()
    {
        static Type sEventType = static_cast<QEvent::Type>(QEvent::registerEventType());
        return sEventType;
    }

    SelectFilterSearchGuiItemTextEvent();
};

#endif // SELECTFILTERSEARCHGUIITEMTEXTEVENT_H
