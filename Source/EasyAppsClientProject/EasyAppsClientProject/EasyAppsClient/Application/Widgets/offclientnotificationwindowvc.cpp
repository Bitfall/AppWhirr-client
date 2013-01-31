#include "offclientnotificationwindowvc.h"

OffClientNotificationWindowVC::OffClientNotificationWindowVC(OffClientNotificationWindowView *view, int showHideFadeTimeMilliseconds, QObject *parent) :
    _view(view)
{

}

void OffClientNotificationWindowVC::showNotificationWithTimeout(QString notificationText, int showTimeoutMilliseconds)
{
}
