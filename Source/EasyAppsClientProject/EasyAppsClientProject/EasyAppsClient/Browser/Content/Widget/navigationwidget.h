#ifndef NAVIGATIONWIDGET_H
#define NAVIGATIONWIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QMouseEvent;
class QPoint;
class QLabel;
QT_END_NAMESPACE

class NavigationWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NavigationWidget(QWidget *parent = 0);

    void setUserNameText(QString userNameText);

Q_SIGNALS:
    void minimizeButtonClicked();
    void maximizeButtonClicked();
    void closeButtonClicked();

protected:
    void mousePressEvent (QMouseEvent * event);
    void mouseMoveEvent (QMouseEvent * event);
    void mouseReleaseEvent (QMouseEvent * event);

    void paintEvent(QPaintEvent *e);

private Q_SLOTS:
    // for test
    void _minimizeButtonClicked();

private:
    QPoint          offset;
    bool            moving;

    QLabel *_userNameText;
};

#endif // NAVIGATIONWIDGET_H
