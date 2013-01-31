#ifndef APPLICATIONSCREENSHOTENTITY_H
#define APPLICATIONSCREENSHOTENTITY_H

#include <QObject>
#include "Common/Communication/basenetworkentity.h"

QT_BEGIN_NAMESPACE
class QString;
QT_END_NAMESPACE

class ApplicationScreenshotEntity : public AW::Common::Communication::BaseNetworkEntity
{
    Q_OBJECT
public:
    ApplicationScreenshotEntity(QObject* parent = 0);

    void setScreenshotUrl(QString screenshotUrl);
    void setThumbnailUrl(QString thumbnailUrl);

    QString getScreenshotUrl();
    QString getThumbnailUrl();
private:
    QString     screenshotUrl;
    QString     thumbnailUrl;
};

#endif // APPLICATIONSCREENSHOTENTITY_H
