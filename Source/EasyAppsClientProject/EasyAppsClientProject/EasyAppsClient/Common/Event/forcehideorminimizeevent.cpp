#include "forcehideorminimizeevent.h"

ForceHideOrMinimizeEvent::ForceHideOrMinimizeEvent()
        : QEvent(getCustomEventType())
{
}
