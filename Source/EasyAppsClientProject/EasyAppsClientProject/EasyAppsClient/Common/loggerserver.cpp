#include "loggerserver.h"

LoggerServer::LoggerServer(QObject *parent) :
    QObject(parent)
{
}

LoggerServer::~LoggerServer()
{
}

bool LoggerServer::installAndInitializeQtLogHandler(QString logFilePath)
{
//    qInstallMsgHandler(SimpleLoggingHandler);
    return true;
}
