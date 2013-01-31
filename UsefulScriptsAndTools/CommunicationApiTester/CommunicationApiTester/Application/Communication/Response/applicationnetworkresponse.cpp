#include "applicationnetworkresponse.h"

#include <QString>
#include <QScriptValue>
#include <QScriptValueIterator>
#include <QUrl>

#include "Application/Communication/Entity/applicationnetworkentity.h"
#include "Application/Communication/Entity/applicationreviewentity.h"
#include "Application/Communication/Entity/applicationscreenshotentity.h"

ApplicationNetworkResponse::ApplicationNetworkResponse(const QString& JSONResponse) :
    Background_BaseNetworkResponse(JSONResponse)
{
}

QList<BaseNetworkEntity*> ApplicationNetworkResponse::receivedResponseWithSuccess_DoParse(const QScriptValue& JSONValue) {
//    DLog("-- do parse");
    QScriptValueIterator it(JSONValue);

    QList<BaseNetworkEntity*> response;

    // Convert response to array
    while (it.hasNext()) {
        it.next();
        if (!it.hasNext()) continue;

        if (it.value().property("application").isArray()) {
            QScriptValueIterator app_list_it(it.value().property("application"));
            while (app_list_it.hasNext()) {
                app_list_it.next();

                if (app_list_it.value().property("id").toString().isEmpty() ||
                    app_list_it.value().property("name").toString().isEmpty())
                    continue;

                ApplicationNetworkEntity* entity = new ApplicationNetworkEntity;
                DLog("-- new item created");

                // setup the entity
                ((ApplicationNetworkEntity*)entity)->setApplicationID(app_list_it.value().property("id").toString());
                ((ApplicationNetworkEntity*)entity)->setApplicationName(app_list_it.value().property("name").toString());
                ((ApplicationNetworkEntity*)entity)->setApplicationRating(float(app_list_it.value().property("rating").toNumber()));
                ((ApplicationNetworkEntity*)entity)->setApplicationRatingCount(app_list_it.value().property("ratingCount").toInteger());
                ((ApplicationNetworkEntity*)entity)->setApplicationReviewCount(app_list_it.value().property("commentsCount").toInteger());

                // get verion information
                if (app_list_it.value().property("version").isArray()) {
                    QScriptValueIterator app_version_it(app_list_it.value().property("version"));
                    while (app_version_it.hasNext()) {
                        app_version_it.next();

//                        DLog("-- before check");
                        if (app_version_it.value().property("icon").toString().isEmpty() ||
                            app_version_it.value().property("identifier").toString().isEmpty() ||
                            app_version_it.value().property("category").toString().isEmpty()
                                )
                        {
                            WLog("At least one of these required informations is missing: icon, version or category");
                            continue;
                        }

//                        DLog("-- after check");

                        // Get the url for the encoded string
                        QUrl url(app_version_it.value().property("icon").toString());

                        ((ApplicationNetworkEntity*)entity)->setApplicationVersion(app_version_it.value().property("identifier").toString());
                        ((ApplicationNetworkEntity*)entity)->setApplicationIconURL(QString("http://" + url.encodedHost() + url.encodedPath()));
                        ((ApplicationNetworkEntity*)entity)->setApplicationCategory(app_version_it.value().property("category").toString());
                        ((ApplicationNetworkEntity*)entity)->setApplicationDescription(app_version_it.value().property("description").toString());
                        ((ApplicationNetworkEntity*)entity)->setApplicationVersionSummedFlags(app_version_it.value().property("summedFlages").toInteger());

                        // media - video
                        if (app_version_it.value().property("video").isArray()) {
                            QScriptValueIterator video_it(app_version_it.value().property("video"));
                            while (video_it.hasNext()) {
                                video_it.next();
                                if (!video_it.hasNext()) continue;

                                ((ApplicationNetworkEntity*)entity)->addApplicationVideo(video_it.value().property("movie").toString());
                            }
                        }

                        // media - screenshot
                        if (app_version_it.value().property("image").isArray()) {
                            QScriptValueIterator image_it(app_version_it.value().property("image"));
                            while (image_it.hasNext()) {
                                image_it.next();
                                if (!image_it.hasNext()) continue;

                                ApplicationScreenshotEntity* screenshotEntity = new ApplicationScreenshotEntity;
                                screenshotEntity->setScreenshotUrl(image_it.value().property("screenshot").toString());
                                screenshotEntity->setThumbnailUrl(image_it.value().property("thumbnail").toString());
                                ((ApplicationNetworkEntity*)entity)->addApplicationImage(screenshotEntity);
                            }
                        }

                        // review
                        if (app_version_it.value().property("reviews").isArray()) {
                            QScriptValueIterator review_it(app_version_it.value().property("reviews"));
                            while (review_it.hasNext()) {
                                review_it.next();

                                if (review_it.value().property("user").toString().isEmpty() ||
                                    review_it.value().property("review").toString().isEmpty())
                                    continue;

                                ApplicationReviewEntity* applicationReview = new ApplicationReviewEntity;
                                applicationReview->setUsername(review_it.value().property("user").toString());
                                applicationReview->setRating(review_it.value().property("rating").toInteger());
                                applicationReview->setReview(review_it.value().property("review").toString());

                                ((ApplicationNetworkEntity*)entity)->addApplicationReview(applicationReview);
                            }
                        }
                    }
                }
                else {
                    WLog("Version is not an array. Should be.");
                }

                // and save it to the list
                response.push_back(entity);
//                DLog("-- new item stored with version") << ((ApplicationNetworkEntity*)entity)->getApplicationVersion();
            }
        }
    }

//    Q_EMIT this->parsingFinished(response);
    return response;
}
