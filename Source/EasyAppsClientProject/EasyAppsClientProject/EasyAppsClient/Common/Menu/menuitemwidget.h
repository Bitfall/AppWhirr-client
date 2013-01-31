#ifndef MENUITEMWIDGET_H
#define MENUITEMWIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QHBoxLayout;
class QVBoxLayout;
class QLabel;
QT_END_NAMESPACE

class SimpleImagePresenterWidget;

class MenuItemWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MenuItemWidget(QString id, QString title, int badgeNumber = 0, QWidget *parent = 0);

    QString getId() const;

    void setTitle(QString title);

    // don't use this method - the MenuManager will use it to force this item to emit a top-active signal
    void _emitItemBecomeTheTopActiveOneSignal();

    void _activateAnimated();
    void _deactivateAnimated();

    void setBadgeNumber(int number);

Q_SIGNALS:
    void itemClick(QString relatedMenuItemId);

    // top-active signal, when this menu-item is selected
    void itemBecomeTheTopActiveOne(QString relatedMenuItemId);

protected:
    bool event(QEvent *e);

private:
    QString itemId;
    QHBoxLayout *mainLayout;
    QLabel *itemTitleLabel;
    int badgeNumber;
//    SimpleImagePresenterWidget* badge;
    QLabel* badge;
};

#endif // MENUITEMWIDGET_H
