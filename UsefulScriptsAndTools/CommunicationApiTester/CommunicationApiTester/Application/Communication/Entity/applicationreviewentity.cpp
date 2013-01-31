#include "applicationreviewentity.h"

ApplicationReviewEntity::ApplicationReviewEntity(QObject *parent) :
    BaseNetworkEntity(parent),
    username(""),
    rating(0),
    review("")
{
}

void ApplicationReviewEntity::setUsername(QString username) {
    this->username = username;
}

void ApplicationReviewEntity::setRating(int rating) {
    this->rating = rating;
}

void ApplicationReviewEntity::setReview(QString review) {
    this->review = review;
}

QString ApplicationReviewEntity::getUsername() {
    return username;
}

int ApplicationReviewEntity::getRating() {
    return rating;
}

QString ApplicationReviewEntity::getReview() {
    return review;
}
