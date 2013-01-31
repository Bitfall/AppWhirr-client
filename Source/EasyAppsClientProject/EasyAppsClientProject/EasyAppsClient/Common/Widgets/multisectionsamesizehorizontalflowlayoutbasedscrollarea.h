#ifndef MULTISECTIONSAMESIZEHORIZONTALFLOWLAYOUTBASEDSCROLLAREA_H
#define MULTISECTIONSAMESIZEHORIZONTALFLOWLAYOUTBASEDSCROLLAREA_H

#include <QScrollArea>

#include "../Layouts/samesizelayoutbase.h"
#include "multisectiondata.h"

class SectionBasedHorizontalFlowLayoutPresenterDelegate;
class MouseGrabScrollDelegate;

class MultiSectionSameSizeHorizontalFlowLayoutBasedScrollArea : public QScrollArea
{
    Q_OBJECT

public:
    //
    // the delegate won't be deleted, you have to do it's memory-management
    explicit MultiSectionSameSizeHorizontalFlowLayoutBasedScrollArea(SectionBasedHorizontalFlowLayoutPresenterDelegate *delegate, int sectionCount, QWidget *parent = 0);
    ~MultiSectionSameSizeHorizontalFlowLayoutBasedScrollArea();

    //
    // delegate based
    void addSection();
    //
    // non-delegate based
    void addSection(QString sectionTitleText, QString sectionTitleLabelStyleSheet, QString sectionWidgetStyleSheet, QString sectionMoreIdentifier = "");
    //
    // non-delegate based, and accepts custom widget as section-content-holder-widget
    //  ! the customSectionContentHolderWidget should not have any layout, the related layout will be added to it
    void addSectionWithWidget(QString sectionTitleText, QString sectionTitleLabelStyleSheet, QWidget *customSectionContentHolderWidget, QString sectionMoreIdentifier = "");
    void addWidgetToSection(QWidget *widget, int sectionIndex);

protected:
    void resizeEvent(QResizeEvent *e);
    bool eventFilter(QObject *obj, QEvent *event);

Q_SIGNALS:
    void showListPageWithId(QString);

private:
    bool processMouseWheelEventFilter(QWheelEvent *wheelEvent);
    bool processMouseButtonPressEvent(QObject *obj, QMouseEvent *e);
    bool processMouseMoveEvent(QObject *obj, QMouseEvent *e);
    bool processMouseButtonReleaseEvent(QObject *obj, QMouseEvent *e);

protected:
    QWidget *_mainContentHolderWidget;
    QVBoxLayout *_mainContentHolderLayout;
    QList<MultiSectionData *> _sections;
    SectionBasedHorizontalFlowLayoutPresenterDelegate *_delegate;

    MouseGrabScrollDelegate *_mouseGrabScrollDelegate;
};

#endif // MULTISECTIONSAMESIZEHORIZONTALFLOWLAYOUTBASEDSCROLLAREA_H
