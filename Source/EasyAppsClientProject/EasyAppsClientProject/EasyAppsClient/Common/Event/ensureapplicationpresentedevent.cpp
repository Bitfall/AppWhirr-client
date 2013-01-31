#include "ensureapplicationpresentedevent.h"

EnsureApplicationPresentedEvent::EnsureApplicationPresentedEvent()
        : QEvent(getCustomEventType())
{
}
