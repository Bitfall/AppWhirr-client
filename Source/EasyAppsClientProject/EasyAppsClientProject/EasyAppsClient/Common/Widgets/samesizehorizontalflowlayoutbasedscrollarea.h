#ifndef FLOWLAYOUTBASEDSCROLLAREA_H
#define FLOWLAYOUTBASEDSCROLLAREA_H

#include <QScrollArea>

#include "../Layouts/samesizelayoutbase.h"

//
// Horizontal Flow layout means: it will fill the available space with items horizontally and will grow vertically
class SameSizeHorizontalFlowLayoutBasedScrollArea : public QScrollArea
{
    Q_OBJECT
public:
    explicit SameSizeHorizontalFlowLayoutBasedScrollArea(SameSizeLayoutBase *sameSizeLayout, QWidget *parent = 0);

    void addWidget(QWidget *widget);

protected:
    void resizeEvent(QResizeEvent *e);

protected:
    QWidget *_contentHolderWidget;
    SameSizeLayoutBase *_contentHolderLayout;
};

#endif // FLOWLAYOUTBASEDSCROLLAREA_H
