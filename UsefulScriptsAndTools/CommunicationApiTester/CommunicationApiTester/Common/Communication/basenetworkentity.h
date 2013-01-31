#ifndef BASENETWORKENTITY_H
#define BASENETWORKENTITY_H

#include <QObject>

class BaseNetworkEntity : public QObject
{
    Q_OBJECT
public:
    explicit BaseNetworkEntity(QObject *parent = 0);
    virtual ~BaseNetworkEntity() {}
};

#endif // BASENETWORKENTITY_H
