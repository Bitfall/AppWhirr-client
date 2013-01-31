#include "confirmapplicationdeactivaterequestevent.h"

ConfirmApplicationDeactivateRequestEvent::ConfirmApplicationDeactivateRequestEvent()
  : QEvent(getCustomEventType())
{
}
