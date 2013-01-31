#ifndef DELEGATINGDWIDGET_H
#define DELEGATINGDWIDGET_H

#include <QWidget>

class DelegatingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DelegatingWidget(QWidget *parent = 0);

Q_SIGNALS:
    void resizeHappened(QResizeEvent *e);

protected:
    void resizeEvent(QResizeEvent *e);
    void paintEvent(QPaintEvent *e);
};

#endif // DELEGATINGDWIDGET_H
