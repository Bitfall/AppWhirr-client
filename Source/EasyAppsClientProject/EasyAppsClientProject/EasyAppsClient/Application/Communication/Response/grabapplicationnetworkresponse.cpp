#include "grabapplicationnetworkresponse.h"

#include <QString>
#include <QVariant>
#include <QUrl>

#include "Application/Communication/Entity/grabapplicationnetworkentity.h"

#include "Common/Json/json.h"

GrabApplicationNetworkResponse::GrabApplicationNetworkResponse(const QString& JSONResponse) :
    AW::Common::Communication::Background_BaseNetworkResponse(JSONResponse)
{
}

QList<AW::Common::Communication::BaseNetworkEntity*> GrabApplicationNetworkResponse::receivedResponseWithSuccess_DoParse(QString jsonString)
{

    QList<AW::Common::Communication::BaseNetworkEntity*> response;

    bool isParsingSuccess = true;
    QVariantMap resultMap = QtJson::Json::parse(jsonString, isParsingSuccess).toMap();
    if( !isParsingSuccess ) {
        WLog("Parsing error: ") << jsonString;
        return response;
    }

    QString givenUrl = resultMap.value("url").toString();
    if(!givenUrl.isEmpty()) {
        QUrl url(givenUrl);
        url.addEncodedQueryItem("AWSAccessKeyId", resultMap.value("accessKey").toString().toAscii());
        url.addEncodedQueryItem("Expires", resultMap.value("expires").toString().toAscii());
        url.addEncodedQueryItem("Signature", QUrl::toPercentEncoding(resultMap.value("signature").toString(),
                                                                     QByteArray("=")));

        GrabApplicationNetworkEntity* entity = new GrabApplicationNetworkEntity(url,
                                                                                resultMap.value("version").toString());

        // and save it to the list
        response.push_back(entity);
    }

    return response;
}
