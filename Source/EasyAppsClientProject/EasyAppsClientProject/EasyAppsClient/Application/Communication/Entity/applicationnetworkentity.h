#ifndef APPLICATIONDETAILINFOSNETWORKENTITY_H
#define APPLICATIONDETAILINFOSNETWORKENTITY_H

#include <QObject>
#include "Application/Communication/Entity/applicationbaseinfosnetworkentity.h"
#include <QString>
#include <QList>

class ApplicationReviewEntity;
class ApplicationScreenshotEntity;

class ApplicationDetailInfosNetworkEntity : public ApplicationBaseInfosNetworkEntity
{
    Q_OBJECT
public:
    explicit ApplicationDetailInfosNetworkEntity(QObject *parent = 0);

    void setApplicationDescription(QString applicationDescription);
    void addApplicationImage(ApplicationScreenshotEntity* image);
    void addApplicationVideo(QString video);
    void addApplicationReview(ApplicationReviewEntity* applicationReview);

    void setTwitterQuery(QString twitterQuery);
    void setDeveloperWebpageUrl(QString developerWebpageUrl);
    void setSupportUrl(QString supportUrl);
    void setPrice(QString price);
    void setRequiredHdd(QString requiredHdd);

    QString getApplicationDescription();
    QList<ApplicationScreenshotEntity*> getApplicationImages();
    QList<QString> getApplicationVideos();
    QList<ApplicationReviewEntity*> getApplicationReviews();

    QString getTwitterQuery() const;
    QString getDeveloperWebpageUrl() const;
    QString getSupportUrl() const;
    QString getPrice() const;
    QString getRequiredHdd() const;

private:
    QString                             applicationDescription;
    QList<QString>                      applicationVideos;
    QList<ApplicationScreenshotEntity*> applicationImages;
    QList<ApplicationReviewEntity*>     applicationReviews;

    QString _twitterQuery;
    QString _developerWebpageUrl;
    QString _supportUrl;
    QString _price;

    QString _requiredHdd;
};

#endif // APPLICATIONDETAILINFOSNETWORKENTITY_H
