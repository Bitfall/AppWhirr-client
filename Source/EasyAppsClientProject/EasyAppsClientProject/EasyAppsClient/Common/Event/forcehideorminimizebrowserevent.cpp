#include "forcehideorminimizebrowserevent.h"

ForceHideOrMinimizeBrowserEvent::ForceHideOrMinimizeBrowserEvent()
        : QEvent(getCustomEventType())
{
}
