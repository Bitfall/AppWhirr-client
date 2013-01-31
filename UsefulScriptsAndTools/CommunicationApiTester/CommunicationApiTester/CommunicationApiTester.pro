#-------------------------------------------------
#
# Project created by QtCreator 2011-11-29T18:40:35
#
#-------------------------------------------------

QT       += core gui network script webkit

TARGET = CommunicationApiTester
TEMPLATE = app

CONFIG += precompile_header
CONFIG += no_keywords

PRECOMPILED_HEADER = Developer/macros.h

DEFINES += DEBUG
DEFINES += ENABLE_COMMUNICATION_LOGGING


SOURCES += main.cpp\
        mainwindow.cpp \
    Common/Communication/localmessagesender.cpp \
    Common/Communication/basenetworkresponse.cpp \
    Common/Communication/basenetworkrequest.cpp \
    Common/Communication/basenetworkentity.cpp \
    Application/Communication/Entity/grablistnetworkentity.cpp \
    Application/Communication/Entity/grabapplicationnetworkentity.cpp \
    Application/Communication/Entity/getappeulanetworkentity.cpp \
    Application/Communication/Entity/applicationscreenshotentity.cpp \
    Application/Communication/Entity/applicationreviewentity.cpp \
    Application/Communication/Entity/applicationnetworkentity.cpp \
    Application/Communication/Entity/applicationlistwithsectionsnetworkentity.cpp \
    Application/Communication/Entity/applicationbaseinfosnetworkentitymodel.cpp \
    Application/Communication/Entity/applicationbaseinfosnetworkentity.cpp \
    Application/Communication/Gateway/communicationresourcegateway.cpp \
    Application/Communication/Gateway/communicationdescriptiongateway.cpp \
    Application/Communication/Request/userloginnetworkrequest.cpp \
    Application/Communication/Request/storemainpagewithsectionsnetworkrequest.cpp \
    Application/Communication/Request/resourcedownloadnetworkrequest.cpp \
    Application/Communication/Request/grablistnetworkrequest.cpp \
    Application/Communication/Request/grabclientupdatenetworkrequest.cpp \
    Application/Communication/Request/grabapplicationnetworkrequest.cpp \
    Application/Communication/Request/getappeulanetworkrequest.cpp \
    Application/Communication/Request/clientupdatenetworkrequest.cpp \
    Application/Communication/Request/applicationsearchnetworkrequest.cpp \
    Application/Communication/Request/applicationnetworkrequest.cpp \
    Application/Communication/Request/applicationlistnetworkrequest.cpp \
    Application/Communication/Response/grabapplicationnetworkresponse.cpp \
    Application/Communication/Response/getappeulanetworkresponse.cpp \
    Application/Communication/Response/applicationnetworkresponse.cpp \
    Application/Communication/Response/applicationlistwithsectionsnetworkresponse.cpp \
    Application/Communication/Response/applicationlistnetworkresponse.cpp \
    Common/Security/authorization.cpp \
    Common/System/systemprofiler.cpp \
    Application/appspecificpathhelper.cpp \
    Application/Settings/configmanager.cpp \
    Common/Helpers/pathhelper.cpp \
    Common/Helpers/platformflagshelper.cpp \
    Application/Communication/Request/apitestnetworkrequest.cpp \
    Application/Communication/Response/apitestnetworkresponse.cpp \
    Application/Communication/Entity/plainresponsenetworkentity.cpp \
    Common/Widget/simplewebviewwindow.cpp \
    Common/Communication/simplesslclient.cpp

HEADERS  += mainwindow.h \
    Developer/macros.h \
    Common/Communication/localmessagesender.h \
    Common/Communication/localmessagecodesenum.h \
    Common/Communication/basenetworkresponse.h \
    Common/Communication/basenetworkrequest.h \
    Common/Communication/basenetworkentity.h \
    Application/Communication/Entity/grablistnetworkentity.h \
    Application/Communication/Entity/grabapplicationnetworkentity.h \
    Application/Communication/Entity/getappeulanetworkentity.h \
    Application/Communication/Entity/applicationscreenshotentity.h \
    Application/Communication/Entity/applicationreviewentity.h \
    Application/Communication/Entity/applicationnetworkentity.h \
    Application/Communication/Entity/applicationlistwithsectionsnetworkentity.h \
    Application/Communication/Entity/applicationbaseinfosnetworkentitymodel.h \
    Application/Communication/Entity/applicationbaseinfosnetworkentity.h \
    Application/Communication/Gateway/communicationresourcegateway.h \
    Application/Communication/Gateway/communicationdescriptiongateway.h \
    Application/Communication/Request/userloginnetworkrequest.h \
    Application/Communication/Request/storemainpagewithsectionsnetworkrequest.h \
    Application/Communication/Request/resourcedownloadnetworkrequest.h \
    Application/Communication/Request/grablistnetworkrequest.h \
    Application/Communication/Request/grabclientupdatenetworkrequest.h \
    Application/Communication/Request/grabapplicationnetworkrequest.h \
    Application/Communication/Request/getappeulanetworkrequest.h \
    Application/Communication/Request/clientupdatenetworkrequest.h \
    Application/Communication/Request/applicationsearchnetworkrequest.h \
    Application/Communication/Request/applicationnetworkrequest.h \
    Application/Communication/Request/applicationlistnetworkrequest.h \
    Application/Communication/Response/grabapplicationnetworkresponse.h \
    Application/Communication/Response/getappeulanetworkresponse.h \
    Application/Communication/Response/applicationnetworkresponse.h \
    Application/Communication/Response/applicationlistwithsectionsnetworkresponse.h \
    Application/Communication/Response/applicationlistnetworkresponse.h \
    Common/Security/authorization.h \
    Common/System/systemprofiler.h \
    Application/appspecificpathhelper.h \
    Application/Settings/configmanager.h \
    Common/Helpers/pathhelper.h \
    Common/Helpers/platformflagshelper.h \
    Application/ApplicationDescription/applicationstatesenum.h \
    Application/Communication/Request/apitestnetworkrequest.h \
    Application/Communication/Response/apitestnetworkresponse.h \
    Application/Communication/Entity/plainresponsenetworkentity.h \
    Common/Widget/simplewebviewwindow.h \
    Common/Communication/simplesslclient.h

mac {
    LIBS+=/System/Library/Frameworks/Carbon.framework/Carbon
}

RESOURCES += \
    resources.qrc




































