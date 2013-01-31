#include "basenetworkresponse.h"

#include <QScriptEngine>
#include <QScriptValue>
#include <QScriptValueIterator>

Background_BaseNetworkResponse::Background_BaseNetworkResponse(const QString& JSONResponse) :
    QObject(0),
    JSONResponse(JSONResponse)
{
#ifdef ENABLE_COMMUNICATION_LOGGING
    DLog("BaseNetworkResponse created");
#endif

    // Delete automatically, after calling run()
    setAutoDelete(true);
}

Background_BaseNetworkResponse::~Background_BaseNetworkResponse() {
#ifdef ENABLE_COMMUNICATION_LOGGING
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
#ifdef ENABLE_COMMUNICATION_LOGGING
    DLog("Given JSON response: ") << JSONResponse;
#endif
    QScriptEngine scriptEngine;
    QScriptValue jsonValue = scriptEngine.evaluate(JSONResponse);

    if (jsonValue.isArray()) {
        QScriptValueIterator it(jsonValue);
        while (it.hasNext()) {
            it.next();

            // Check for error
            QScriptValue returnValue = it.value().property("return");
            if (!returnValue.isNull())
            {
                if (returnValue.toString() == QString("success")) {
                    QList<BaseNetworkEntity*> parserResultList = this->receivedResponseWithSuccess_DoParse(jsonValue);
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
                    receivedResponseWithError(jsonValue);
                }

                return; // nothing more to do
            }
        }
    }

    Q_EMIT parsingFailed("Cannot parse response, no array found");
}

void Background_BaseNetworkResponse::receivedResponseWithError(const QScriptValue& JSONValue) {
    if (JSONValue.isArray()) {
        QScriptValueIterator it(JSONValue);
        while (it.hasNext()) {
            it.next();

            // Get the error message
            QScriptValue returnValue = it.value().property("return");
            if (!returnValue.isNull()) {
                QString returnMessage = returnValue.toString();
                if(returnMessage == "failed") {
                    QString errorMessage = it.value().property("error_str").toString();
//                    QString errorCode = it.value().property("error_code");
                    if(!errorMessage.isEmpty()) {
                        Q_EMIT parsingFailed(errorMessage);
                        return;
                    }
                }

                Q_EMIT parsingFailed(returnValue.toString());

                return;
            }
        }
    }

    Q_EMIT parsingFailed("Server response was an error, but no error message received from server");
}
