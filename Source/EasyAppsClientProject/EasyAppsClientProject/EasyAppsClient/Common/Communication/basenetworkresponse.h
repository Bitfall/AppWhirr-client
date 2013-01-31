#ifndef BACKGROUND_BASENETWORKRESPONSE_H
#define BACKGROUND_BASENETWORKRESPONSE_H

#include <QObject>
#include <QRunnable>
#include <QList>
#include "Common/Communication/basenetworkentity.h"


namespace AW {
namespace Common {
namespace Communication {


/*! Parses the response received from the server */
/** <p>Parses the JSON response received from the server.
    If the response is not JSON or cannot be parsed, it fails with an error.</p>
    <p>More information about the communication and it's resonse can be found in the @ref communication "Communication" section<p>
 */
class Background_BaseNetworkResponse : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit Background_BaseNetworkResponse(QString JSONResponse);
    virtual ~Background_BaseNetworkResponse();

    /*! Starts the parsing in a background thread */
    /**
      */
    void run();

Q_SIGNALS:
    /*! Parsing is terminated because of an error */
    /** @param error Description of the error
      */
//    void errorOccuredWhileParsing(const QString& error);
    /*! Parsing is finished successfully */
    /** @param response The requested response
      */
    void parsingFinished(QList<AW::Common::Communication::BaseNetworkEntity*> response);
    /*! Parsing is finished, but we cannot generate the response */
    /** Request failed on the server
        @param error Description of the error
     */
    void parsingFailed(const QString& error);
    /*! Class did finish running in the background */
    /** */
    void finished();

protected:
    /*! Response can be parsed, so let's parse it */
    /** You have to implement this method, when subclassing this class
        @param jsonValue Generated JSON value
        @return: the parsed list of an empty list if nothing was parsed
      */
    virtual QList<AW::Common::Communication::BaseNetworkEntity*> receivedResponseWithSuccess_DoParse(QString jsonResponseString) = 0;

private:
    // Starts the parsing of the received response
    // Parsing is done in a background thread
    void parseResponse();

    /*! Response is rejected by server */
    /** You can overwrite this function
        @param jsonValue Generated JSON value
      */
    virtual void receivedResponseWithError(QString jsonResponseString);

private:

    // JSON response received from the communication class
    QString                 _jsonResponse;
};


} // namespace Communication
} // namespace Common
} // namespace AW

#endif // BACKGROUND_BASENETWORKRESPONSE_H
