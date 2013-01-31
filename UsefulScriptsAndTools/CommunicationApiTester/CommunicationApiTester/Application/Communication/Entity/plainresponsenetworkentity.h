#ifndef PLAINRESPONSENETWORKENTITY_H
#define PLAINRESPONSENETWORKENTITY_H

#include "Common/Communication/basenetworkentity.h"

class PlainResponseNetworkEntity : public BaseNetworkEntity
{
    Q_OBJECT
public:
    explicit PlainResponseNetworkEntity(QString responseString, QObject *parent = 0);

    QString getResponseAsString() const;

private:
    QString _responseString;
};

#endif // PLAINRESPONSENETWORKENTITY_H
