#ifndef OFFCLIENTNOTIFICATIONWINDOWVC_H
#define OFFCLIENTNOTIFICATIONWINDOWVC_H

#include <QObject>

class OffClientNotificationWindowView;

class OffClientNotificationWindowVC : public QObject
{
    Q_OBJECT
public:
    explicit OffClientNotificationWindowVC(OffClientNotificationWindowView *view, int showHideFadeTimeMilliseconds, QObject *parent = 0);

    void showNotificationWithTimeout(QString notificationText, int showTimeoutMilliseconds);

private:
    OffClientNotificationWindowView *_view;
};

#endif // OFFCLIENTNOTIFICATIONWINDOWVC_H
