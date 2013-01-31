#ifndef INCONTENTNAVIGATIONWIDGET_H
#define INCONTENTNAVIGATIONWIDGET_H

#include <QWidget>

class InContentNavigationWidget : public QWidget
{
    Q_OBJECT
public:
    /*!
        @param backButtonText : if empty string then it will be tr("Back")
    */
    explicit InContentNavigationWidget(QString backButtonText = QString(), QWidget *parent = 0);

Q_SIGNALS:
    void backButtonClicked();
    
protected:
    void paintEvent(QPaintEvent *e);
    
};

#endif // INCONTENTNAVIGATIONWIDGET_H
