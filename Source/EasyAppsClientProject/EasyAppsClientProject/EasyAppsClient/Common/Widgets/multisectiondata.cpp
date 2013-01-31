#include "multisectiondata.h"

#include "../Layouts/samesizelayoutbase.h"

MultiSectionData::MultiSectionData(QWidget *sectionWidget, SameSizeLayoutBase *sectionLayout, QString sectionIdentifier)
    : QObject(0),
      _sectionWidget(sectionWidget),
      _sectionLayout(sectionLayout),
      _sectionIdentifier(sectionIdentifier) {
}

QWidget *MultiSectionData::getSectionWidget() const {
    return _sectionWidget;
}

SameSizeLayoutBase *MultiSectionData::getSectionLayout() const {
    return _sectionLayout;
}

void MultiSectionData::sectionActivated() {
    Q_EMIT sectionIdentifierRequested(_sectionIdentifier);
}
