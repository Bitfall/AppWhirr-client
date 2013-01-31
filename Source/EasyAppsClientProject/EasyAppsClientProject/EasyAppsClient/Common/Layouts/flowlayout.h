//
// based on the Qt Flow Layout example: http://doc.qt.nokia.com/4.7/layouts-flowlayout.html
//

#ifndef FLOWLAYOUT_H
#define FLOWLAYOUT_H

#include <QLayout>
#include <QRect>
#include <QWidgetItem>
#include <QStyle>

class FlowLayout : public QLayout
{
public:
    FlowLayout(QWidget *parent, int margin = -1, int hSpacing = -1, int vSpacing = -1);
    FlowLayout(int margin = -1, int hSpacing = -1, int vSpacing = -1);
    ~FlowLayout();

    void addItem(QLayoutItem *item);
    int horizontalSpacing() const;
    int verticalSpacing() const;
    Qt::Orientations expandingDirections() const;
    bool hasHeightForWidth() const;
    int heightForWidth(int) const;
    int count() const;
    QLayoutItem *itemAt(int index) const;
    QSize minimumSize() const;
    void setGeometry(const QRect &rect);
    QSize sizeHint() const;
    QLayoutItem *takeAt(int index);

    //
    // if you set a fixedWidth > 0 then it will be used as width and will overwrite every rect size after it's set
    //  this is useful if the related parent widget is used in a scroll-area and want to keep the area's width and grow only in height
    void setFixedWidthMode(int fixedWidth);

private:
    void __init(int marginLeft, int marginTop, int marginRight, int marginBottom);

private:
    //
    // returns the optimal calculated size
    QSize __doLayout(const QRect &rect, bool testOnly) const;
    int smartSpacing(QStyle::PixelMetric pm) const;

    QList<QLayoutItem *> itemList;
    int m_hSpace;
    int m_vSpace;

    QRect _lastReceivedRect;
    int _fixedWidth;
};

#endif // FLOWLAYOUT_H
