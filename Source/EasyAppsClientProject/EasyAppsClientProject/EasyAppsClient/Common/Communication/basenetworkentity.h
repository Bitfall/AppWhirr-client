#ifndef BASENETWORKENTITY_H
#define BASENETWORKENTITY_H

#include <QObject>


namespace AW {
namespace Common {
namespace Communication {


class BaseNetworkEntity : public QObject
{
    Q_OBJECT
public:
    explicit BaseNetworkEntity(QObject *parent = 0) : QObject(parent) {}
    virtual ~BaseNetworkEntity() {}
};


} // namespace Communication
} // namespace Common
} // namespace AW

#endif // BASENETWORKENTITY_H
