#ifndef IDBASEDWIDGETCOLLECTION_H
#define IDBASEDWIDGETCOLLECTION_H

#include <QObject>
#include <QMap>
#include <QString>

#include "Common/Widgets/idbasedwidgetbase.h"

//
// NOT THREADSAFE
class IdBasedWidgetCollection : public QObject
{
    Q_OBJECT
public:
    explicit IdBasedWidgetCollection(QObject *parent = 0);

    void add(IdBasedWidgetBase *item);
    IdBasedWidgetBase *getFirstById(QString id);
    QList<IdBasedWidgetBase *> getAllById(QString id);

    QList<IdBasedWidgetBase *> getAll();
    QList<QWidget *> _getAllAsWidgetList();
    QList<QString> _getAllIds();

    //
    // call this ONLY if the items in this collection belongs to the collection -> for example if the items are NOT in any layout. If an item is added to a layout then the layout will reparent it and will free it. In this case DON'T call this method with isDeleteAsWell=true.
    void _removeAll(bool isDeleteAsWell);

    //
    // the incoming ordered-list's every element HAVE TO BE ALREADY ADDED
    //  to the list, but because of speed this method won't check,
    //  IT WILL CLEAR THE CURRENT LIST, and add these items
    //  IT WILL ONLY CHECK in the title-to-presenter map
    //      and add only the items present in the map
    //      BUT won't add the items not contained / presented in the new list!!!
    void _refreshListOrderByIdList(QList<QString> newOrderedIdList);

    //
    // will clear (but not delete the items!) the current list then store the new one
    //  !!! every item of the newItems have to be a subclass of IdBasedWidgetBase. If it cannot cast to IdBasedWidgetBase then the item will be skipped!!!
    void _setItemsFromWidgetList(QList<QWidget *> newItems);

private:
    QList<IdBasedWidgetBase *> _allItems;
//    QList<QString> allIconTitles;
//    QMap<QString, IconPresenterWidget *> iconTitleToPresenterWidgetMap;
};

#endif // IDBASEDWIDGETCOLLECTION_H
