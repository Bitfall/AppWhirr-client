#ifndef IDBASEDITEMINTERFACE_H
#define IDBASEDITEMINTERFACE_H

#include <QString>

class IdBasedItemInterface {
public:
    virtual QString getId() const = 0;
};

#endif // IDBASEDITEMINTERFACE_H
