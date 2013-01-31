#-------------------------------------------------
#
# Project created by QtCreator 2011-09-08T10:08:37
#
#-------------------------------------------------

QT       += core gui sql network webkit

TARGET = AppDatabaseUpdateChecker
TEMPLATE = app

CONFIG += precompile_header
PRECOMPILED_HEADER = Macros/macros.h

SOURCES += main.cpp\
        appdatabaseupdatecheckermainwindow.cpp \
    Common/Database/basedatabase.cpp \
    AppSpec/Database/websitelistdatabasemanager.cpp \
    AppSpec/Database/websitelistdatabaseitemmodel.cpp \
    AppSpec/websitecheckbackgroundworker.cpp \
    AppSpec/Dialogs/checkresultlistwindow.cpp \
    AppSpec/Dialogs/compareitemswindow.cpp \
    AppSpec/Dialogs/addnewitemdialog.cpp \
    Common/Helpers/qstringhelper.cpp

HEADERS  += appdatabaseupdatecheckermainwindow.h \
    Common/Database/basedatabase.h \
    AppSpec/Database/websitelistdatabasemanager.h \
    AppSpec/Database/websitelistdatabaseitemmodel.h \
    Macros/macros.h \
    AppSpec/websitecheckbackgroundworker.h \
    AppSpec/Dialogs/checkresultlistwindow.h \
    AppSpec/Dialogs/compareitemswindow.h \
    AppSpec/Dialogs/addnewitemdialog.h \
    Common/Helpers/qstringhelper.h

DEFINES += DEBUG























