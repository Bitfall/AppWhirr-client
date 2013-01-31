#ifndef IntelligentSeparatedMultifunctionalButtonWidget_H
#define IntelligentSeparatedMultifunctionalButtonWidget_H

#include <QWidget>
#include <QList>
#include <QPixmap>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QHBoxLayout;
QT_END_NAMESPACE

#include "animatedtwofixedsizewidgetbase.h"

class IntelligentSeparatedMultifunctionalButtonWidgetOptionItemModel;
class IntelligentSeparatedMultifunctionalButtonWidgetOptionItemWidget;
class SimpleImagePresenterWidget;

class IntelligentSeparatedMultifunctionalButtonWidget : public AnimatedTwoFixedSizeWidgetBase
{
    Q_OBJECT
private:
    explicit IntelligentSeparatedMultifunctionalButtonWidget(IntelligentSeparatedMultifunctionalButtonWidgetOptionItemModel *defaultOptionItemModel, QList<IntelligentSeparatedMultifunctionalButtonWidgetOptionItemModel *> otherOptionItemModels, QSize collapsedSize, QSize expandedSize, bool isExpandedByDefault, QPixmap expandCollapseButtonPixmap, QPixmap backgroundPixmap, QWidget *parent = 0);

public:
    //
    // factory method - this will automatically calculate the sizes
    static IntelligentSeparatedMultifunctionalButtonWidget *createWithItems(IntelligentSeparatedMultifunctionalButtonWidgetOptionItemModel *defaultOptionItemModel, QList<IntelligentSeparatedMultifunctionalButtonWidgetOptionItemModel *> otherOptionItemModels, QPixmap expandCollapseButtonPixmap, QPixmap backgroundPixmap, QWidget *parent = 0);

protected:
    void paintEvent(QPaintEvent *e);

public Q_SLOTS:

//private Q_SLOTS: // tmp
    void expandCollapseWidgetClicked();

private:
    IntelligentSeparatedMultifunctionalButtonWidgetOptionItemWidget *createWidgetFromOptionItemModel(IntelligentSeparatedMultifunctionalButtonWidgetOptionItemModel *optionItemModel, QSize collapsedSize, QSize expandedSize, bool isExpandedByDefault);

private:
    IntelligentSeparatedMultifunctionalButtonWidgetOptionItemWidget *defaultOptionItemWidget;
    QList<IntelligentSeparatedMultifunctionalButtonWidgetOptionItemWidget *> otherOptionItemWidgets;
    QVBoxLayout *mainLayout;
    QHBoxLayout *defaultOptionItemLineHorizontalLayout;
    SimpleImagePresenterWidget *expandCollapseImageWidget;

    QPixmap backgroundPixmap;
};

#endif // IntelligentSeparatedMultifunctionalButtonWidget_H
