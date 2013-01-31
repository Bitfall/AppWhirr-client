#ifndef DELEGATINGSCROLLAREA_H
#define DELEGATINGSCROLLAREA_H

#include <QScrollArea>

class DelegatingScrollArea : public QScrollArea
{
    Q_OBJECT
public:
    explicit DelegatingScrollArea(QWidget *parent = 0);

Q_SIGNALS:
    void scrollAreaResized(QResizeEvent *resizeEvent);

protected:
    void resizeEvent(QResizeEvent *e);
};

#endif // DELEGATINGSCROLLAREA_H
