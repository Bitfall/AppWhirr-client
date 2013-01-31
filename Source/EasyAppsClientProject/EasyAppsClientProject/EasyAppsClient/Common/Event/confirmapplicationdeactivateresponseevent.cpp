#include "confirmapplicationdeactivateresponseevent.h"

ConfirmApplicationDeactivateResponseEvent::ConfirmApplicationDeactivateResponseEvent()
  : QEvent(getCustomEventType())
{
}
