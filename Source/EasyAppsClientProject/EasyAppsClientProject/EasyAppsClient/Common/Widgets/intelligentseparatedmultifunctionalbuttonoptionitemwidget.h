#ifndef IntelligentSeparatedMultifunctionalButtonWidgetOPTIONITEMWIDGET_H
#define IntelligentSeparatedMultifunctionalButtonWidgetOPTIONITEMWIDGET_H

#include "animatedtwofixedsizewidgetbase.h"

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

class IntelligentSeparatedMultifunctionalButtonWidgetOptionItemModel;

class IntelligentSeparatedMultifunctionalButtonWidgetOptionItemWidget : public AnimatedTwoFixedSizeWidgetBase
{
    Q_OBJECT
public:
    explicit IntelligentSeparatedMultifunctionalButtonWidgetOptionItemWidget(IntelligentSeparatedMultifunctionalButtonWidgetOptionItemModel *relatedOptionItem, QSize collapsedSize, QSize expandedSize, bool isExpandedByDefault, QWidget *parent = 0);

    IntelligentSeparatedMultifunctionalButtonWidgetOptionItemModel *getRelatedOptionItemModel() const;

protected:
//    void mousePressEvent(QMouseEvent * event);
    bool event(QEvent *e);

private:
    IntelligentSeparatedMultifunctionalButtonWidgetOptionItemModel *relatedOptionItem;
    QLabel *titleLabel;
};

#endif // IntelligentSeparatedMultifunctionalButtonWidgetOPTIONITEMWIDGET_H
