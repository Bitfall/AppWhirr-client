#ifndef IDBASEDWIDGETBASE_H
#define IDBASEDWIDGETBASE_H

#include <QWidget>

#include "Common/idbasediteminterface.h"

class IdBasedWidgetBase : public QWidget, public IdBasedItemInterface
{
    Q_OBJECT
public:
    explicit IdBasedWidgetBase(QWidget *parent = 0);

    //
    virtual QString getId() const = 0;
};

#endif // IDBASEDWIDGETBASE_H
