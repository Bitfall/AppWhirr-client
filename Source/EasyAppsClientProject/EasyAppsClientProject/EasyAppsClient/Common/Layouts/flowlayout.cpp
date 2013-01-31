#include "flowlayout.h"

#include <QWidget>
#include <QSize>

FlowLayout::FlowLayout(QWidget *parent, int margin, int hSpacing, int vSpacing)
    : QLayout(parent), m_hSpace(hSpacing), m_vSpace(vSpacing)
{
    this->__init(margin, margin, margin, margin);
}

FlowLayout::FlowLayout(int margin, int hSpacing, int vSpacing)
    : m_hSpace(hSpacing), m_vSpace(vSpacing)
{
    this->__init(margin, margin, margin, margin);
}

FlowLayout::~FlowLayout()
{
    QLayoutItem *item;
    while ((item = takeAt(0)))
        delete item;
}

void FlowLayout::__init(int marginLeft, int marginTop, int marginRight, int marginBottom)
{
    setContentsMargins(marginLeft, marginTop, marginRight, marginBottom);
    _fixedWidth = -1;
}

void FlowLayout::addItem(QLayoutItem *item)
{
    itemList.append(item);
}

int FlowLayout::horizontalSpacing() const
{
    if (m_hSpace >= 0) {
        return m_hSpace;
    } else {
        return smartSpacing(QStyle::PM_LayoutHorizontalSpacing);
    }
}

int FlowLayout::verticalSpacing() const
{
    if (m_vSpace >= 0) {
        return m_vSpace;
    } else {
        return smartSpacing(QStyle::PM_LayoutVerticalSpacing);
    }
}

int FlowLayout::count() const
{
    return itemList.size();
}

QLayoutItem *FlowLayout::itemAt(int index) const
{
    return itemList.value(index);
}

QLayoutItem *FlowLayout::takeAt(int index)
{
    if (index >= 0 && index < itemList.size()) {
        return itemList.takeAt(index);
    }

    return 0;
}

Qt::Orientations FlowLayout::expandingDirections() const
{
    return 0;
}

bool FlowLayout::hasHeightForWidth() const
{
    return true;
}

int FlowLayout::heightForWidth(int width) const
{
    int height = __doLayout(QRect(0, 0, width, 0), true).height();
    return height;
}

void FlowLayout::setGeometry(const QRect &rect)
{
    QLayout::setGeometry(rect);
    _lastReceivedRect = rect;

    __doLayout(rect, false);
}

QSize FlowLayout::sizeHint() const
{
    return minimumSize();
}

QSize FlowLayout::minimumSize() const
{
//    QSize size;
//    QLayoutItem *item;
//    Q_FOREACH (item, itemList) {
//        size = size.expandedTo(item->minimumSize());
//    }

//    size += QSize(2*margin(), 2*margin());
//    return size;

    QSize minSize = __doLayout(_lastReceivedRect, true);

    DLog("Min size: ") << minSize;

    return minSize;
//    return QSize(500, 500);
}

void FlowLayout::setFixedWidthMode(int fixedWidth) {
    _fixedWidth = fixedWidth;
    DLog("Fixed width: ") << _fixedWidth;
}

QSize FlowLayout::__doLayout(const QRect &rect, bool testOnly) const
{
    QRect rectToUse = rect;

    if(_fixedWidth > 0) {
        rectToUse = QRect(rect.left(), rect.top(), _fixedWidth, 0);
    }

    DLog("Rect to use: ") << rectToUse;

    int left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);
    QRect effectiveRect = rectToUse.adjusted(+left, +top, -right, -bottom);
    int x = effectiveRect.x();
    int y = effectiveRect.y();
    int lineHeight = 0;

    int maxX = 0;

    QLayoutItem *item;
    Q_FOREACH (item, itemList)
    {
        QWidget *currItemWidget = item->widget();
        int spaceX = horizontalSpacing();
        if (spaceX == -1) {
            spaceX = currItemWidget->style()->layoutSpacing(QSizePolicy::PushButton, QSizePolicy::PushButton, Qt::Horizontal);
        }
        int spaceY = verticalSpacing();
        if (spaceY == -1) {
            spaceY = currItemWidget->style()->layoutSpacing(QSizePolicy::PushButton, QSizePolicy::PushButton, Qt::Vertical);
        }
        int nextX = x + item->sizeHint().width() + spaceX;
        if (nextX - spaceX > effectiveRect.right() && lineHeight > 0) {
            x = effectiveRect.x();
            y = y + lineHeight + spaceY;
            nextX = x + item->sizeHint().width() + spaceX;
            lineHeight = 0;
        }

        if (!testOnly) {
            item->setGeometry(QRect(QPoint(x, y), item->sizeHint()));
            DLog("Item geom: ") << item->geometry();
        }

        x = nextX;
        maxX = qMax(maxX, x);
        lineHeight = qMax(lineHeight, item->sizeHint().height());
    }

    QSize calculatedOptimalSize = QSize(maxX - rect.x(), y + lineHeight - rectToUse.y() + bottom);

    DLog("Optimal size: ") << calculatedOptimalSize;
    DLog("Is only testing: ") << testOnly;

    return calculatedOptimalSize;
}

int FlowLayout::smartSpacing(QStyle::PixelMetric pm) const
{
    QObject *parent = this->parent();
    if (!parent) {
        return -1;
    } else if (parent->isWidgetType()) {
        QWidget *pw = static_cast<QWidget *>(parent);
        return pw->style()->pixelMetric(pm, 0, pw);
    } else {
        return static_cast<QLayout *>(parent)->spacing();
    }
}
