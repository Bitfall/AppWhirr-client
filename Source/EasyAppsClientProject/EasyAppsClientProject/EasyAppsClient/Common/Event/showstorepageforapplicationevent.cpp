#include "showstorepageforapplicationevent.h"

ShowStorePageForApplicationEvent::ShowStorePageForApplicationEvent(QString appId)
    : QEvent(getCustomEventType()),
      _appId(appId)
{
}

QString ShowStorePageForApplicationEvent::getAppId() const {
    return this->_appId;
}
