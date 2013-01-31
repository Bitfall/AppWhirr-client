#ifndef SPECIALPAGEACTIONCONTROLLER_H
#define SPECIALPAGEACTIONCONTROLLER_H

#include <QObject>

#include "Application/ApplicationDescription/applicationdescriptionmodel.h"

/*! It's basically a notification-transmitter class

    Connect the related pages' slots to this class' signals then you can emit the signals with the realted ...Slot() methods
*/
class SpecialPageActionController : public QObject
{
    Q_OBJECT
public:
    explicit SpecialPageActionController(QObject *parent = 0);

public Q_SLOTS:
    void emitShowStoreDetailPageForApp(ApplicationDescriptionModel appDescriptionModel);
    void emitSearchInStoreByFilterText(QString searchFilterText);

Q_SIGNALS:
    void showStoreDetailPageForAppSignal(ApplicationDescriptionModel appDescriptionModel);
    void searchInStoreByFilterTextSignal(QString searchFilterText);
};

#endif // SPECIALPAGEACTIONCONTROLLER_H
