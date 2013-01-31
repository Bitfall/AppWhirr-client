#ifndef GRABAPPLICATIONNETWORKENTITY_H
#define GRABAPPLICATIONNETWORKENTITY_H

#include <QObject>
#include <QUrl>

#include "Common/Communication/basenetworkentity.h"

class GrabApplicationNetworkEntity : public AW::Common::Communication::BaseNetworkEntity
{
    Q_OBJECT
public:
    explicit GrabApplicationNetworkEntity(QUrl zippedApplicationURL, QString relatedAppVersion, QObject *parent = 0);

    QUrl getZippedApplicationURL();
    QString getRelatedAppVersion();
private:
    QUrl    _zippedApplicationURL;
    QString _relatedAppVersion;
};

#endif // GRABAPPLICATIONNETWORKENTITY_H
