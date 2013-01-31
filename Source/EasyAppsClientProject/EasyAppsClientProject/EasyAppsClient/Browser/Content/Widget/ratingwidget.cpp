#include "ratingwidget.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QString>

RatingWidget::RatingWidget(float rating, QWidget *parent) :
    QWidget(parent)
{
    this->setFixedSize(115, 16);

    QHBoxLayout* mainLayout = new QHBoxLayout;
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    this->setLayout(mainLayout);

    QLabel* ratingLabel = new QLabel;
    ratingLabel->setStyleSheet("* { font: 12pt Arial sans-serif; color: #8e8e8e; text-align: left; }");
    ratingLabel->setText("rating");
    ratingLabel->setFixedSize(30, 16);

    QWidget* emptyStar = new QWidget;
    emptyStar->setFixedSize(80, 16);
    emptyStar->setContentsMargins(0, 0, 0, 0);
    emptyStar->setStyleSheet("* {  background: url(':rating/star') left top; }");

    mainLayout->addWidget(ratingLabel);
    mainLayout->addStretch();
    mainLayout->addWidget(emptyStar);

    QHBoxLayout* ratingLayout = new QHBoxLayout;
    ratingLayout->setContentsMargins(0, 0, 0, 0);
    ratingLayout->setSpacing(0);

    emptyStar->setLayout(ratingLayout);

    filledStar = new QWidget;

    filledStar->setStyleSheet("* { background: url(':rating/star') left bottom; }");
    filledStar->setContentsMargins(0, 0, 0, 0);

    ratingLayout->addWidget(filledStar);
    ratingLayout->addStretch();

    this->updateRating(rating);
}

RatingWidget::~RatingWidget() {
    AW_QOBJECT_SAFE_DELETE(filledStar);
}

void RatingWidget::updateRating(float rating) {
    // Max width: 16 * 5 = 80px
    // Max height: 16px

    if (rating < 0.0) rating = 0.0;
    if (rating > 5.0) rating = 5.0;

    filledStar->setFixedSize(16 * rating, 16);
}
