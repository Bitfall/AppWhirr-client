#include "appframeworkinfonetworkresponse.h"

#include <QString>
#include <QUrl>
#include <QVariant>

#include "Application/Communication/Entity/appframeworkinfonetworkentity.h"
#include "Common/Json/json.h"

AppFrameworkInfoNetworkResponse::AppFrameworkInfoNetworkResponse(const QString& JSONResponse) :
    AW::Common::Communication::Background_BaseNetworkResponse(JSONResponse)
{
}

QList<AW::Common::Communication::BaseNetworkEntity *> AppFrameworkInfoNetworkResponse::receivedResponseWithSuccess_DoParse(QString jsonString)
{
    QList<AW::Common::Communication::BaseNetworkEntity *> response;

    bool isParsingSuccess = true;
    QVariantMap resultMap = QtJson::Json::parse(jsonString, isParsingSuccess).toMap();
    if( !isParsingSuccess ) {
        WLog("Parsing error: ") << jsonString;
        return response;
    }

    QVariantList frameworksList = resultMap.value("frameworks").toList();

    if(frameworksList.isEmpty()) {
        WLog("No frameworks found.");
    }
    else {

        Q_FOREACH(QVariant frameworkVar, frameworksList)
        {
            QVariantMap frameworkMap = frameworkVar.toMap();

            QString frameworkId;
            QString frameworkName;
            QString frameworkCheckerUpdateTimestamp;
            QUrl frameworkDownloadUrl;
            QUrl frameworkCheckerDownloadUrl;

            frameworkId = frameworkMap.value("id").toString();
            frameworkName = frameworkMap.value("name").toString();
            frameworkCheckerUpdateTimestamp = frameworkMap.value("timestamp").toString();
            DLog(" -!- timestamp: ") << frameworkCheckerUpdateTimestamp;

            if(frameworkId.isEmpty()) {
                WLog("Invalid (empty) framework id.");
                continue;
            }
            if(frameworkName.isEmpty()) {
                WLog("Invalid (empty) framework name.");
                continue;
            }
            if(frameworkCheckerUpdateTimestamp.isEmpty()) {
                WLog("Invalid (empty) framework checker update timestamp.");
                continue;
            }

            // framework-download-url
            QVariantList fwUrlList = frameworkMap.value("frameworkURL").toList();
            if(fwUrlList.isEmpty()) {
                WLog("No URL found for framework");
            }
            else {
                Q_FOREACH(QVariant fwUrlVar, fwUrlList)
                {
                    QVariantMap fwUrlMap = fwUrlVar.toMap();

                    QString baseFwUrlString = fwUrlMap.value("url").toString();

                    if(!baseFwUrlString.isEmpty()) {
                        frameworkDownloadUrl = QUrl(baseFwUrlString);
                        frameworkDownloadUrl.addEncodedQueryItem("AWSAccessKeyId", fwUrlMap.value("accessKey").toString().toAscii());
                        frameworkDownloadUrl.addEncodedQueryItem("Expires", fwUrlMap.value("expires").toString().toAscii());
                        frameworkDownloadUrl.addEncodedQueryItem("Signature", QUrl::toPercentEncoding(fwUrlMap.value("signature").toString(), QByteArray("=")));

                        DLog("FW Url found: ") << frameworkDownloadUrl;

                        //                    GrabApplicationNetworkEntity* entity = new GrabApplicationNetworkEntity(url, it.value().property("version").toString());

                        // and save it to the list
                        //                    response.push_back(entity);
                    }
                    else {
                        WLog("Invalid 'url' in 'frameworkURL' - empty.");
                    }
                }
            }

            // framework-checker-download-url
            QVariantList fwCheckerUrlList = frameworkMap.value("frameworkCheckerURL").toList();
            if(fwCheckerUrlList.isEmpty()) {
                WLog("No URL found for frameworkChecker");
            }
            else {
                Q_FOREACH(QVariant fwCheckerUrlVar, fwCheckerUrlList)
                {
                    QVariantMap fwCheckerUrlMap = fwCheckerUrlVar.toMap();

                    QString baseFwCheckerUrlString = fwCheckerUrlMap.value("url").toString();

                    if(!baseFwCheckerUrlString.isEmpty()) {
                        frameworkCheckerDownloadUrl = QUrl(baseFwCheckerUrlString);
                        frameworkCheckerDownloadUrl.addEncodedQueryItem("AWSAccessKeyId", fwCheckerUrlMap.value("accessKey").toString().toAscii());
                        frameworkCheckerDownloadUrl.addEncodedQueryItem("Expires", fwCheckerUrlMap.value("expires").toString().toAscii());
                        frameworkCheckerDownloadUrl.addEncodedQueryItem("Signature", QUrl::toPercentEncoding(fwCheckerUrlMap.value("signature").toString(), QByteArray("=")));

                        DLog("FW checker url found: ") << frameworkCheckerDownloadUrl;

                        //                    GrabApplicationNetworkEntity* entity = new GrabApplicationNetworkEntity(url, it.value().property("version").toString());

                        // and save it to the list
                        //                    response.push_back(entity);
                    }
                    else {
                        WLog("Invalid 'url' in 'frameworkCheckerURL' - empty.");
                    }
                }
            }

            // check validity
            if(frameworkDownloadUrl.isEmpty() || frameworkCheckerDownloadUrl.isEmpty()) {
                // invalid
                WLog("Invalid 'frameworkULR' or 'frameworkCheckerURL'");
            }
            else {
                // valid
                AppFrameworkInfoNetworkEntity *entity = new AppFrameworkInfoNetworkEntity(frameworkId, frameworkName, frameworkCheckerUpdateTimestamp, frameworkDownloadUrl, frameworkCheckerDownloadUrl);
                response.push_back(entity);
            }
        }
    }
    DLog("parse finished.");

    return response;
}
