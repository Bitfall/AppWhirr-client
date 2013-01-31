#ifndef SECTIONBASEDHORIZONTALFLOWLAYOUTPRESENTERDELEGATE_H
#define SECTIONBASEDHORIZONTALFLOWLAYOUTPRESENTERDELEGATE_H

#include <QSize>

class SameSizeLayoutBase;

class SectionBasedHorizontalFlowLayoutPresenterDelegate {
public:
    //
    // must to be implemented
    virtual SameSizeLayoutBase *createLayout() = 0;

    //
    // optional
    virtual QString getSectionTitle(int sectionIndex) { return QString(); }
    virtual QString getSectionTitleLabelStyleSheet(int sectionIndex) { return QString(); }
    virtual QString getSectionWidgetStyleSheet(int sectionIndex) { return QString(); }
};

#endif // SECTIONBASEDHORIZONTALFLOWLAYOUTPRESENTERDELEGATE_H
