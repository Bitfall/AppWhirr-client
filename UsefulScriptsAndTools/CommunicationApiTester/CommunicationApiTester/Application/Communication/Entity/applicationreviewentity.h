#ifndef APPLICATIONREVIEWENTITY_H
#define APPLICATIONREVIEWENTITY_H

#include <QObject>
#include "Common/Communication/basenetworkentity.h"

QT_BEGIN_NAMESPACE
class QString;
QT_END_NAMESPACE

class ApplicationReviewEntity : public BaseNetworkEntity
{
    Q_OBJECT
public:
    explicit ApplicationReviewEntity(QObject *parent = 0);

    void setUsername(QString username);
    void setRating(int rating);
    void setReview(QString review);

    QString getUsername();
    int getRating();
    QString getReview();

private:
    QString username;
    int rating;
    QString review;
};

#endif // APPLICATIONREVIEWENTITY_H
