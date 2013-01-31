#-------------------------------------------------
#
# Project created by QtCreator 2012-06-22T13:45:11
#
#-------------------------------------------------

QT       += core gui

TARGET = AppWhirrUninstaller
TEMPLATE = app

CONFIG += no_keywords
CONFIG += precompile_header
PRECOMPILED_HEADER = ../EasyAppsClient/Developer/macros.h

SOURCES += main.cpp\
        mainwindow.cpp \
    View/welcomeview.cpp \
    ../EasyAppsClient/Common/Widgets/checkboxbutton.cpp \
    Controller/welcomeviewcontroller.cpp \
    uninstallconfigs.cpp \
    Controller/uninstallingviewcontroller.cpp \
    View/uninstallingview.cpp \
    Controller/goodbyeviewcontroller.cpp \
    View/goodbyeview.cpp \
    awuninstallperformertask.cpp \
    ../EasyAppsClient/Common/Runnables/runnable_checkforrunningexecutable.cpp \
    ../EasyAppsClient/Application/appspecificpathhelper.cpp \
    ../EasyAppsClient/Common/Helpers/pathhelper.cpp \
    ../EasyAppsClient/Common/Application/platformspecificprogramidentifierhelper.cpp \
    ../EasyAppsClient/Common/awfilefolderbackup.cpp \
    View/goodbyeerrorlistitemwidget.cpp \
    ../EasyAppsClient/Common/guiconfigurationmanager.cpp \
    guiconfigurationmanagersingleton.cpp \
    ../EasyAppsClient/Common/Application/macprocess.cpp \
    ../EasyAppsClient/Common/Helpers/qstringhelper.cpp

HEADERS  += mainwindow.h \
    Controller/welcomeviewcontroller.h \
    View/welcomeview.h \
    ../EasyAppsClient/Common/Widgets/checkboxbutton.h \
    ../EasyAppsClient/Developer/macros.h \
    Controller/viewcontrollerinterface.h \
    uninstallconfigs.h \
    Controller/uninstallingviewcontroller.h \
    View/uninstallingview.h \
    Controller/goodbyeviewcontroller.h \
    View/goodbyeview.h \
    uninstalldeletesteps.h \
    awuninstallperformertask.h \
    ../EasyAppsClient/Common/awtask.h \
    ../EasyAppsClient/Common/Runnables/runnable_checkforrunningexecutable.h \
    ../EasyAppsClient/Application/appspecificpathhelper.h \
    ../EasyAppsClient/Common/Helpers/pathhelper.h \
    ../EasyAppsClient/Common/Application/platformspecificrunningexecutabledatainterface.h \
    ../EasyAppsClient/Common/Application/platformspecificprogramidentifierhelper.h \
    ../EasyAppsClient/Common/awfilefolderbackup.h \
    View/goodbyeerrorlistitemwidget.h \
    ../EasyAppsClient/Common/guiconfigurationmanager.h \
    guiconfigurationmanagersingleton.h \
    ../EasyAppsClient/Common/Application/macprocess.h \
    ../EasyAppsClient/Common/Helpers/qstringhelper.h


# -------------------
# platform and build type specific configurations

#
# platform specific

# ===================
# -- MAC
mac {
# depending libs
    LIBS += /System/Library/Frameworks/Carbon.framework/Carbon
    LIBS += -framework AppKit

#copy dynlib
# NOTE: every QMAKE_POST_LINK will be fired in one line!! here are the tips / tricks if you want to run more than 1 post-link action: http://www.qtforum.org/article/14932/qmake-post-link-in-pro.html
#    QMAKE_POST_LINK += ./../../../../UsefulScriptsAndTools/mac/dynlib/dynlib_updater_copy.sh
    QMAKE_POST_LINK += ./../../../../UsefulScriptsAndTools/mac/dynlib/deploy_uninstaller.sh
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

# destination / build dir
        DESTDIR = "./../../EasyAppsClientProject-build-desktop/BIN/debug"
    }

    CONFIG( release, debug|release ) {
# release mode

# destination / build dir
        DESTDIR = "./../../EasyAppsClientProject-build-desktop/BIN/release"
    }

    SOURCES += ../EasyAppsClient/Common/Application/win_applicationprocessbyexedata.cpp
    HEADERS += ../EasyAppsClient/Common/Application/win_applicationprocessbyexedata.h

    QMAKE_POST_LINK = "$$(APPWHIRR_PROJECT_SOURCE_ROOT_DIR)\\AppWhirrUninstaller\\PostBuildScripts\\Windows\\windows_post_build_scripts_main.bat"
}

# ===================
# AllOS - debug / release (configuration) specific

# all
DEFINES += ENABLE_APPWHIRR_LOGGING
DEFINES += ENABLE_REVIEW_THIS_LOGGING
DEFINES += ENABLE_IGNORE_SSL_ERRORS_IN_SERVER_COMMUNICATION
# this won't effect normal not-implemented-warnings, only enables / disables some internal todo like warnings
DEFINES += ENABLE_INTERNAL_NOT_IMPLEMENTED_WARNING_LOGGING
DEFINES += ENABLE_DEPRECATED_WARNING_LOGGING

# changing defines sometimes requires a manual/full clean!!
# probably there's no release {} section, and even !debug{} isn't working
# by the way, this is the "canonical" qmake form, and it works :)
CONFIG( debug, debug|release ) {
    # debug

# debug logging (required by every appwhirr logging)
    DEFINES += ENABLE_APPWHIRR_DEBUG_LOGGING

    #
    # enable these to allow to run a debug and a release version at the same time
    DEFINES += USE_DEBUG_LOCAL_FOLDER
    DEFINES += USE_DEBUG_LOCAL_SERVER_CONNECTION_NAMES
    DEFINES += ENABLE_FILE_SYSTEM_LOGGING

} else {
    # release
#   // this would completely turn-off every Qt logging (qDebug, qError, etc.)
#    DEFINES += QT_NO_DEBUG_OUTPUT

    # for debug logging enable this (required by every appwhirr logging)
    DEFINES += ENABLE_APPWHIRR_DEBUG_LOGGING

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

OTHER_FILES +=

RESOURCES += \
    ../awuninstaller-resources.qrc
