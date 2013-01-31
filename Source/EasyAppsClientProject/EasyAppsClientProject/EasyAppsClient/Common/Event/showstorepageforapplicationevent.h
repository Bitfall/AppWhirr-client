#ifndef SHOWSTOREPAGEFORAPPLICATIONEVENT_H
#define SHOWSTOREPAGEFORAPPLICATIONEVENT_H

#include <QEvent>

class ShowStorePageForApplicationEvent : public QEvent
{
public:
    static inline Type getCustomEventType()
    {
        static Type sEventType = static_cast<QEvent::Type>(QEvent::registerEventType());
        return sEventType;
    }

    ShowStorePageForApplicationEvent(QString appId);

    QString getAppId() const;

private:
    QString _appId;
};

#endif // SHOWSTOREPAGEFORAPPLICATIONEVENT_H
