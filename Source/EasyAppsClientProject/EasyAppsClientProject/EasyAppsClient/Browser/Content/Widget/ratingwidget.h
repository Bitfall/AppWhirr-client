#ifndef RATINGWIDGET_H
#define RATINGWIDGET_H

#include <QWidget>

class RatingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RatingWidget(float rating = 0.0, QWidget *parent = 0);
    ~RatingWidget();

Q_SIGNALS:

public Q_SLOTS:
    void updateRating(float rating);

private:
    QWidget*        filledStar;
};

#endif // RATINGWIDGET_H
