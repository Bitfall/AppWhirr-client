#ifndef APPVERSIONNETWORKENTITY_H
#define APPVERSIONNETWORKENTITY_H

#include "Common/Communication/basenetworkentity.h"

class AppVersionNetworkEntity : public AW::Common::Communication::BaseNetworkEntity
{
    Q_OBJECT
public:
    explicit AppVersionNetworkEntity(QString versionInfo, QObject *parent = 0);

    QString getVersionInfo() const;

private:
    QString _versionInfo;
};

#endif // APPVERSIONNETWORKENTITY_H
