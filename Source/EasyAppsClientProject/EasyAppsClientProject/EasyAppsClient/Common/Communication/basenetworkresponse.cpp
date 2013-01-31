#include "basenetworkresponse.h"

//#include <QScriptEngine>
//#include <QScriptValue>
//#include <QScriptValueIterator>

#include "Common/Json/json.h"

namespace AW {
namespace Common {
namespace Communication {

Background_BaseNetworkResponse::Background_BaseNetworkResponse(QString JSONResponse) :
    QObject(0),
    _jsonResponse(JSONResponse)
{
#ifdef ENABLE_COMMUNICATION_DEBUG_LOGGING
    DLog("BaseNetworkResponse created");
#endif

    // Delete automatically, after calling run()
    setAutoDelete(true);
}

Background_BaseNetworkResponse::~Background_BaseNetworkResponse() {
#ifdef ENABLE_COMMUNICATION_DEBUG_LOGGING
    DLog("BaseNetworkResponse deleted");
#endif

    this->disconnect();
}

void Background_BaseNetworkResponse::run()
{
    parseResponse();

    // Finished running, kill me!
    Q_EMIT finished();
}

void Background_BaseNetworkResponse::parseResponse()
{
#ifdef ENABLE_COMMUNICATION_DEBUG_LOGGING
    DLog("Given JSON response: ") << _jsonResponse;
#endif

    bool isParseOk = false;
    QVariantMap parsedMap = QtJson::Json::parse(_jsonResponse, isParseOk).toMap();
    if( !isParseOk ) {
        WLog("Cannot parse the JSon response: ") << _jsonResponse;
        return;
    }

    if(parsedMap.contains("return"))
    {
        // Check for error
        if (parsedMap.value("return").toString() == QString("success")) {
            QList<BaseNetworkEntity*> parserResultList = this->receivedResponseWithSuccess_DoParse(_jsonResponse);
            if(parserResultList.isEmpty()) {
                //                        Q_EMIT parsingFailed("Nothing was parsed.");
                DLog("Empty response - nothing to parse. But it's probably not an error.");
                Q_EMIT parsingFinished(parserResultList);
            }
            else {
                DLog("Item cnt: ") << parserResultList.size();
                Q_EMIT parsingFinished(parserResultList);
            }
        } else {
            receivedResponseWithError(_jsonResponse);
        }

        return; // nothing more to do
    }
    else {
        WLog("No 'return' defined - parsing failed!");
    }

    Q_EMIT parsingFailed("Cannot parse response");
}

void Background_BaseNetworkResponse::receivedResponseWithError(QString jsonString) {
    bool isParseOk = false;
    QVariantMap parsedMap = QtJson::Json::parse(_jsonResponse, isParseOk).toMap();
    if( !isParseOk ) {
        WLog("Cannot parse the JSon response: ") << _jsonResponse;
        return;
    }

    if(!parsedMap.contains("return")) {
        WLog("No 'return' found in response!");
    }
    else {
        // Get the error message
        QString returnMessage = parsedMap.value("return").toString();
        if(returnMessage == "failed") {
            QString errorMessage = parsedMap.value("error_str").toString();
            QString errorCode = parsedMap.value("error_code").toString();
            WLog(" >-< Response error. Code: ") << errorCode << " | message: " << errorMessage;

            if(!errorMessage.isEmpty()) {
                Q_EMIT parsingFailed(errorMessage);
                return;
            }
        }

        Q_EMIT parsingFailed(returnMessage);

        return;
    }

    Q_EMIT parsingFailed("Server response was an error, but no error message received from server");
}

} // namespace Communication
} // namespace Common
} // namespace AW
