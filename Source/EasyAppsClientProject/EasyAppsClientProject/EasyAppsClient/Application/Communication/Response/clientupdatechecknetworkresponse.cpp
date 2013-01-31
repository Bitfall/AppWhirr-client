#include "clientupdatechecknetworkresponse.h"

#include <QVariant>

#include "Application/Communication/Entity/appversionnetworkentity.h"
#include "Common/Json/json.h"

ClientUpdateCheckNetworkResponse::ClientUpdateCheckNetworkResponse(const QString& JSONResponse) :
    AW::Common::Communication::Background_BaseNetworkResponse(JSONResponse)
{
}

QList<AW::Common::Communication::BaseNetworkEntity*> ClientUpdateCheckNetworkResponse::receivedResponseWithSuccess_DoParse(QString jsonString) {

    DLog("--- do parse");

    QList<AW::Common::Communication::BaseNetworkEntity*> response;

    bool isParsingSuccess = true;
    QVariantMap resultMap = QtJson::Json::parse(jsonString, isParsingSuccess).toMap();
    if( !isParsingSuccess ) {
        WLog("Parsing error: ") << jsonString;
        return response;
    }

    QString versionInfoString = resultMap.value("version").toString();
    if(!versionInfoString.isEmpty()) {
        AppVersionNetworkEntity *entity = new AppVersionNetworkEntity(versionInfoString);
        response.push_back(entity);
    }

//    Q_EMIT this->parsingFinished(response);
    return response;
}
