#include "getappeulanetworkresponse.h"

#include <QScriptValueIterator>

#include "Application/Communication/Entity/getappeulanetworkentity.h"

GetAppEulaNetworkResponse::GetAppEulaNetworkResponse(const QString& JSONResponse) :
    Background_BaseNetworkResponse(JSONResponse)
{
}

QList<BaseNetworkEntity*> GetAppEulaNetworkResponse::receivedResponseWithSuccess_DoParse(const QScriptValue& JSONValue) {

    DLog("--- do parse");
    QScriptValueIterator it(JSONValue);

    QList<BaseNetworkEntity*> response;

    // Convert response to array
    while (it.hasNext()) {
        it.next();

        QString eulaText = it.value().property("eula").toString();
        if(!eulaText.isEmpty()) {
            GetAppEulaNetworkEntity *entity = new GetAppEulaNetworkEntity(eulaText);
            response.push_back(entity);
        }
    }

//    Q_EMIT this->parsingFinished(response);
    return response;
}
