#include "apitestnetworkresponse.h"

#include <QScriptValueIterator>

#include "Application/Communication/Entity/plainresponsenetworkentity.h"

ApiTestNetworkResponse::ApiTestNetworkResponse(const QString& JSONResponse) :
    Background_BaseNetworkResponse(JSONResponse)
{
}

QList<BaseNetworkEntity*> ApiTestNetworkResponse::receivedResponseWithSuccess_DoParse(const QScriptValue& JSONValue) {
    QList<BaseNetworkEntity*> response;

    response.append(new PlainResponseNetworkEntity(JSONValue.toString()));

//    QScriptValueIterator it(JSONValue);

//    // Convert response to array
//    while (it.hasNext()) {
//        it.next();

//        response.append(new PlainResponseNetworkEntity(it.name()));
//        response.append(new PlainResponseNetworkEntity(it.value().toString()));
//    }

    return response;
}
