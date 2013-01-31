#ifndef APPLICATIONNETWORKENTITY_H
#define APPLICATIONNETWORKENTITY_H

#include <QObject>
#include "Application/Communication/Entity/applicationbaseinfosnetworkentity.h"
#include <QString>
#include <QList>

class ApplicationReviewEntity;
class ApplicationScreenshotEntity;

class ApplicationNetworkEntity : public ApplicationBaseInfosNetworkEntity
{
    Q_OBJECT
public:
    explicit ApplicationNetworkEntity(QObject *parent = 0);

    void setApplicationDescription(QString applicationDescription);
    void addApplicationImage(ApplicationScreenshotEntity* image);
    void addApplicationVideo(QString video);
    void addApplicationReview(ApplicationReviewEntity* applicationReview);

    QString getApplicationDescription();
    QList<ApplicationScreenshotEntity*> getApplicationImages();
    QList<QString> getApplicationVideos();
    QList<ApplicationReviewEntity*> getApplicationReviews();

private:
    QString                             applicationDescription;
    QList<QString>                      applicationVideos;
    QList<ApplicationScreenshotEntity*> applicationImages;
    QList<ApplicationReviewEntity*>     applicationReviews;
};

#endif // APPLICATIONNETWORKENTITY_H
