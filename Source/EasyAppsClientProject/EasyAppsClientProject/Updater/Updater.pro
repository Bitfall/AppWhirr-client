    #-------------------------------------------------
#
# Project created by QtCreator 2011-06-20T20:18:22
#
#-------------------------------------------------

QT       += core
QT       += network

#QT       -= gui
QT       += gui
QT      += sql

TARGET = AppWhirrU
#CONFIG   += console
#CONFIG   -= console
#CONFIG   -= app_bundle

TEMPLATE = app

# 3rd-party and depending
INCLUDEPATH += ../3rdParty/quazip

win32:INCLUDEPATH += "$$(QT_SRC_REFERENCE_ROOT_DIR)/3rdparty/zlib"

CONFIG += precompile_header
PRECOMPILED_HEADER = ../EasyAppsClient/Developer/macros.h

SOURCES += main.cpp \
    ../EasyAppsClient/Common/Uncompressor/uncompressor.cpp \
    AppInstallation/AppInstaller/applicationinstaller.cpp \
    AppInstallation/AppInstaller/background_applicationuncompressor.cpp \
    ../EasyAppsClient/Common/Helpers/pathhelper.cpp \
    AppInstallation/appinstallmanager.cpp \
    ../EasyAppsClient/Common/Application/platformspecificprogramidentifierhelper.cpp \
    ../EasyAppsClient/Common/Communication/localmessagesender.cpp \
    ../EasyAppsClient/Application/User/usermanager.cpp \
    ../EasyAppsClient/Application/User/userdatabasemanager.cpp \
    ../EasyAppsClient/Common/Database/basedatabase.cpp \
    ../EasyAppsClient/Common/Database/simplelogdatabasemanager.cpp \
    appwhirrupdatermainwindow.cpp \
    appwhirrupdaterapplication.cpp \
    BackgroundClientInstaller/backgroundclientinstaller.cpp \
    ../EasyAppsClient/Common/Database/selfrepairingdatabase.cpp \
    ../EasyAppsClient/Common/Database/databasetableschemedescriptor.cpp \
    ../EasyAppsClient/Application/appspecificpathhelper.cpp \
    ../EasyAppsClient/Common/Helpers/databasehelper.cpp \
    ../EasyAppsClient/Common/Runnables/runnable_checkforrunningexecutable.cpp \
    ../EasyAppsClient/Common/Runnables/runnable_deletedirectoryrecursively.cpp \
    ../EasyAppsClient/Common/Application/macprocess.cpp \
    ../EasyAppsClient/Common/Helpers/qstringhelper.cpp \
    ../EasyAppsClient/Common/Json/json.cpp \
    AppInstallation/Runnables/runnable_appinstallcleanupaftererror.cpp \
    Helpers/mac_applescriptexecutor.cpp \
    AppInstallation/AppInstaller/background_appinstaller.cpp \
    ../EasyAppsClient/Common/awfilefolderbackup.cpp

HEADERS += \
    ../EasyAppsClient/Common/Uncompressor/uncompressor.h \
    ../EasyAppsClient/Common/Uncompressor/gojobzip.h \
    AppInstallation/AppInstaller/applicationinstaller.h \
    AppInstallation/AppInstaller/background_applicationuncompressor.h \
    ../EasyAppsClient/Common/Helpers/pathhelper.h \
    AppInstallation/appinstallmanager.h \
    ../EasyAppsClient/Developer/macros.h \
    ../EasyAppsClient/Common/Application/platformspecificprogramidentifierhelper.h \
    ../EasyAppsClient/Common/Communication/localmessagesender.h \
    ../EasyAppsClient/Common/Communication/localmessagecodesenum.h \
    ../EasyAppsClient/Application/User/usermanager.h \
    ../EasyAppsClient/Application/User/userdatabasemanager.h \
    ../EasyAppsClient/Common/Database/basedatabase.h \
    ../EasyAppsClient/Common/Database/simplelogdatabasemanager.h \
    updaterconfigs.h \
    appwhirrupdatermainwindow.h \
    appwhirrupdaterapplication.h \
    BackgroundClientInstaller/backgroundclientinstaller.h \
    BackgroundClientInstaller/installmajorstepsenum.h \
    ../EasyAppsClient/Common/Database/selfrepairingdatabase.h \
    ../EasyAppsClient/Common/Database/databasetableschemedescriptor.h \
    ../EasyAppsClient/Application/appspecificpathhelper.h \
    ../EasyAppsClient/Common/Helpers/databasehelper.h \
    ../EasyAppsClient/Common/Runnables/runnable_checkforrunningexecutable.h \
    ../EasyAppsClient/Common/Runnables/runnable_deletedirectoryrecursively.h \
    AppInstallation/installfallbackstatesenum.h \
    ../EasyAppsClient/Common/Application/macprocess.h \
    ../EasyAppsClient/Common/Helpers/qstringhelper.h \
    ../EasyAppsClient/Application/Managers/AppGrab/appinstallstates.h \
    ../EasyAppsClient/Common/Json/json.h \
    AppInstallation/Runnables/runnable_appinstallcleanupaftererror.h \
    Helpers/mac_applescriptexecutor.h \
    AppInstallation/AppInstaller/background_appinstaller.h \
    ../EasyAppsClient/Common/awfilefolderbackup.h

#
# platform specific
mac {
    OBJECTIVE_SOURCES += \
        Helpers/mac_applescriptexecutor_p.mm
}

# ===================
# -- MAC
mac {
# depending libs
    LIBS += -L"./../../EasyAppsClientProject-build-desktop/3rdParty/quazip" -lquazip
    LIBS += /System/Library/Frameworks/Carbon.framework/Carbon
    LIBS += -framework AppKit

#copy dynlib
# NOTE: every QMAKE_POST_LINK will be fired in one line!! here are the tips / tricks if you want to run more than 1 post-link action: http://www.qtforum.org/article/14932/qmake-post-link-in-pro.html
#    QMAKE_POST_LINK += ./../../../../UsefulScriptsAndTools/mac/dynlib/dynlib_updater_copy.sh
    QMAKE_POST_LINK += ./../../../../UsefulScriptsAndTools/mac/dynlib/deploy_updater.sh
    QMAKE_INFO_PLIST += ./MacResources/Info.plist
}

# ===================
# -- WIN
win32 {

# for more info to windows resource files check these
#   http://stackoverflow.com/questions/2784697/setting-application-info-in-qt
#   http://developer.qt.nokia.com/forums/viewthread/6116
#   http://stackoverflow.com/questions/1125349/how-do-i-set-the-executable-attributes-with-qmake-for-a-c-project
#   And for full details about the variables and about the parameters: http://msdn.microsoft.com/en-us/library/aa381058(VS.85).aspx
    RC_FILE = ./WindowsResources/WindowsOnlyResources.rc

# build (debug / release) specific
    CONFIG( debug, debug|release ) {
# debug mode

# depending libs
        LIBS += -L"./../../EasyAppsClientProject-build-desktop/BIN/debug" -lquazip

# destination / build dir
        DESTDIR = "./../../EasyAppsClientProject-build-desktop/BIN/debug"
    }

    CONFIG( release, debug|release ) {
# release mode

# depending libs
        LIBS += -L"./../../EasyAppsClientProject-build-desktop/BIN/release" -lquazip

# destination / build dir
        DESTDIR = "./../../EasyAppsClientProject-build-desktop/BIN/release"
    }

    SOURCES += ../EasyAppsClient/Common/Application/win_applicationprocessbyexedata.cpp

    HEADERS += ../EasyAppsClient/Common/Application/win_applicationprocessbyexedata.h

    QMAKE_POST_LINK = "$$(APPWHIRR_PROJECT_SOURCE_ROOT_DIR)\\Updater\\PostBuildScripts\\Windows\\windows_post_build_scripts_main.bat"
}

# ===================
# AllOS - debug / release (configuration) specific

# all
DEFINES += ENABLE_APPWHIRR_DEBUG_LOGGING
DEFINES += ENABLE_REVIEW_THIS_LOGGING
DEFINES += ENABLE_IGNORE_SSL_ERRORS_IN_SERVER_COMMUNICATION
# this won't effect normal not-implemented-warnings, only enables / disables some internal todo like warnings
DEFINES += ENABLE_INTERNAL_NOT_IMPLEMENTED_WARNING_LOGGING
DEFINES += ENABLE_DEPRECATED_WARNING_LOGGING

DEFINES += ENABLE_DYNAMIC_USER_RELATED_METHODS_IN_PATH_HELPER

# changing defines sometimes requires a manual/full clean!!
# probably there's no release {} section, and even !debug{} isn't working
# by the way, this is the "canonical" qmake form, and it works :)
CONFIG( debug, debug|release ) {
    # debug

# debug logging (required by every appwhirr logging)
    DEFINES += ENABLE_APPWHIRR_LOGGING

    #
    # enable these to allow to run a debug and a release version at the same time
    DEFINES += USE_DEBUG_LOCAL_FOLDER
    DEFINES += USE_DEBUG_LOCAL_SERVER_CONNECTION_NAMES

} else {
    # release
#   // this would completely turn-off every Qt logging (qDebug, qError, etc.)
#    DEFINES += QT_NO_DEBUG_OUTPUT

    # for debug logging enable this (required by every appwhirr logging)
    DEFINES += ENABLE_APPWHIRR_LOGGING

    #
    # logging enable defines - changing them sometimes require a full-rebuild

    DEFINES += ENABLE_LOCAL_APPLICATION_SCAN_LOGGING
    DEFINES += ENABLE_COMMUNICATION_CORE_LOGGING
    DEFINES += ENABLE_COMPRESSION_LOGGING
    DEFINES += ENABLE_APPLICATION_ACTIVATE_DEACTIVATE_STATE_LOGGING
    DEFINES += ENABLE_HOTKEY_EVENT_LOGGING
    DEFINES += ENABLE_MENU_OPERATION_LOGGING
    DEFINES += ENABLE_PATH_MANAGER_LOGGING

    DEFINES += ENABLE_PAGE_NAVIGATION_LOGGING
    DEFINES += ENABLE_SMART_LAYOUTING_SIZE_EVENT_LOGGING
    DEFINES += ENABLE_DYNAMIC_LAYOUTING_LOGGING
    DEFINES += ENABLE_SNAPSHOT_HANDLING_LOGGING
    DEFINES += ENABLE_PAGE_TRANSITION_LOGGING

    DEFINES += ENABLE_DATABASE_OPERATION_LOGGING
    DEFINES += ENABLE_LOCAL_APPLICATION_DATABASE_OPERATION_HANDLING
    DEFINES += ENABLE_SYSTEM_PROFILER_LOGGING

    #
    # other, utility logging enable/disable

}

FORMS += \
    appwhirrupdatermainwindow.ui

RESOURCES += \
    resources.qrc









