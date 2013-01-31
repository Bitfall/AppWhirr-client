#ifndef GRABAPPLICATIONNETWORKENTITY_H
#define GRABAPPLICATIONNETWORKENTITY_H

#include <QObject>

#include "Common/Communication/basenetworkentity.h"

class GrabApplicationNetworkEntity : public BaseNetworkEntity
{
    Q_OBJECT
public:
    explicit GrabApplicationNetworkEntity(QObject *parent = 0);

    void setZippedApplicationURL(const QString& zippedApplicationURL);
    void setRelatedAppVersion(QString relatedAppVersion);
//    void setZippedApplicationFlag(const quint32& zippedApplicationFlag);

    QString getZippedApplicationURL();
    QString getRelatedAppVersion();
//    quint32 getZippedApplicationFlag();

private:
    QString                     zippedApplicationURL;
//    quint32                     zippedApplicationFlag;
    QString _relatedAppVersion;

};

#endif // GRABAPPLICATIONNETWORKENTITY_H
