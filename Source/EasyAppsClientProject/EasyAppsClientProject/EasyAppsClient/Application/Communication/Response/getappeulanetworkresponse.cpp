#include "getappeulanetworkresponse.h"

#include <QVariant>

#include "Application/Communication/Entity/getappeulanetworkentity.h"
#include "Common/Json/json.h"

GetAppEulaNetworkResponse::GetAppEulaNetworkResponse(const QString& JSONResponse) :
    AW::Common::Communication::Background_BaseNetworkResponse(JSONResponse)
{
}

QList<AW::Common::Communication::BaseNetworkEntity*> GetAppEulaNetworkResponse::receivedResponseWithSuccess_DoParse(QString jsonString) {

    DLog("--- do parse");

    QList<AW::Common::Communication::BaseNetworkEntity*> response;

    bool isParsingSuccess = true;
    QVariantMap resultMap = QtJson::Json::parse(jsonString, isParsingSuccess).toMap();
    if( !isParsingSuccess ) {
        WLog("Parsing error: ") << jsonString;
        return response;
    }

    QString eulaText = resultMap.value("eula").toString();
    if(!eulaText.isEmpty()) {
        GetAppEulaNetworkEntity *entity = new GetAppEulaNetworkEntity(eulaText);
        response.push_back(entity);
    }

//    Q_EMIT this->parsingFinished(response);
    return response;
}
