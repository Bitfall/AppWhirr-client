#ifndef DATASOURCEELEMENTWIDGET_H
#define DATASOURCEELEMENTWIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QMouseEvent;
QT_END_NAMESPACE

class DataSourceElementWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DataSourceElementWidget(QWidget *parent = 0);

Q_SIGNALS:
    void widgetSelected(QWidget* widget);
    void widgetDeselected();

public Q_SLOTS:
    virtual void widgetSelectedSlot(QWidget* widget);
    virtual void widgetDeselectedSlot();

protected:
    void mousePressEvent (QMouseEvent* event);

};

#endif // DATASOURCEELEMENTWIDGET_H
