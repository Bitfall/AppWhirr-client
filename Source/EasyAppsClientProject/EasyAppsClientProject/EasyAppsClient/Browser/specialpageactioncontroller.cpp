#include "specialpageactioncontroller.h"

SpecialPageActionController::SpecialPageActionController(QObject *parent) :
    QObject(parent)
{
}

void SpecialPageActionController::emitShowStoreDetailPageForApp(ApplicationDescriptionModel appDescriptionModel)
{
    Q_EMIT showStoreDetailPageForAppSignal(appDescriptionModel);
}

void SpecialPageActionController::emitSearchInStoreByFilterText(QString searchFilterText)
{
    Q_EMIT searchInStoreByFilterTextSignal(searchFilterText);
}
