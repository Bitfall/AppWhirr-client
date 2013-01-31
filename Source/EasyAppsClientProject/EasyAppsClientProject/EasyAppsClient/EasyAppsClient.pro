#-------------------------------------------------
#
# Project created by QtCreator 2011-01-19T11:04:55
#
#-------------------------------------------------

QT += core gui
QT += svg
QT += xml
QT += network
QT += script
QT += sql
QT += webkit

TARGET = AppWhirr
TEMPLATE = app

CONFIG += precompile_header
CONFIG += no_keywords

PRECOMPILED_HEADER = Developer/macros.h

# enable this for final, public production compilation
#   this will modify some networking configurations (like the frequency of client-self-update check)
#   the communication API's url (non-production vs production server)
#   and will turn on/off some debugging as well
VAR_IS_COMPILE_PUBLIC_PRODUCT = true

# ----------------
# includes and sources

# 3rd-party and depending
INCLUDEPATH += ../3rdParty/quazip
win32:INCLUDEPATH += "$$(QT_SRC_REFERENCE_ROOT_DIR)/3rdparty/zlib"

# sources
SOURCES += main.cpp\
    Common/Event/confirmapplicationdeactivaterequestevent.cpp \
    Common/Event/confirmapplicationdeactivateresponseevent.cpp \
    Settings/guiconfigurations.cpp \
    Common/Event/ensureapplicationpresentedevent.cpp \
    Common/Event/forcehideorminimizeevent.cpp \
    Common/Event/startthisapplicationevent.cpp \
    Settings/settings.cpp \
    Browser/browserwindow.cpp \
    Common/Sorting/sortresultchangeditem.cpp \
    Application/Delegates/globalhotkeydelegate.cpp \
    Application/Delegates/systemtrayicondelegate.cpp \
    Application/Delegates/autostartupdelegate.cpp \
    Common/Sorting/sortingdatabasemanager.cpp \
    Common/Event/forcehideorminimizebrowserevent.cpp \
    Common/Layouts/layoutinginformations.cpp \
    Browser/Sidebar/browsersidebarcontainer.cpp \
    Browser/Featured/browserfeaturedmainwidget.cpp \
    Common/Datasource/datasourcewidget.cpp \
    Browser/Sidebar/browsersidebartextwidget.cpp \
    Common/Datasource/datasourceelementwidget.cpp \
    Browser/Content/Page/optionscontentpage.cpp \
    Common/Widgets/faderwidget.cpp \
    Common/Widgets/crossfaderwidget.cpp \
    Browser/Content/Page/suggestanapppage.cpp \
    Settings/menuitemidcenter.cpp \
    Settings/pageidcenter.cpp \
    Browser/Content/Page/feedbackpage.cpp \
    Common/Widgets/simpleimagepresenterwidget.cpp \
    Common/Helpers/widgetinlayouthelper.cpp \
    Common/WidgetReplacers/animatedwidgetreplacer.cpp \
    Common/Helpers/qsizehelper.cpp \
    Common/WidgetReplacers/parallelanimatedwidgetreplacer.cpp \
    Common/WidgetReplacers/parallelanimatedwidgetreplaceritem.cpp \
    Browser/Content/Page/errorpage.cpp \
    Browser/Content/Page/browserpagebase.cpp \
    Common/Layouts/dynamicdelegatebasedhighperformancesamesizelayoutbase.cpp \
    Browser/Notification/notificationmanager.cpp \
    Browser/Notification/notificationwidget.cpp \
    Common/Layouts/dynamicsamesizeverticallinelayout.cpp \
    Common/Application/Installation/Updater/baseupdater.cpp \
    Common/Application/applicationterminator.cpp \
    Common/Database/databasetableschemedescriptor.cpp \
    Common/Database/basedatabase.cpp \
    Common/Event/selectfiltersearchguiitemtextevent.cpp \
    Common/Application/platformspecificprogramidentifierhelper.cpp \
    Common/Application/win_applicationprocessbyexedata.cpp \
    Application/Communication/Response/applicationlistnetworkresponse.cpp \
    Application/Communication/Request/applicationlistnetworkrequest.cpp \
    Application/Communication/Gateway/communicationdescriptiongateway.cpp \
    Application/Communication/Request/applicationnetworkrequest.cpp \
    Application/Communication/Response/applicationnetworkresponse.cpp \
    Application/Communication/Entity/applicationnetworkentity.cpp \
    Browser/Content/Page/applicationstoremainpage.cpp \
    Common/mousegrabscrolldelegate.cpp \
    Common/Widgets/idbasedselectableandhighlightablewidgetbase.cpp \
    Common/Widgets/idbasedwidgetbase.cpp \
    Common/Widgets/iconimagewidget.cpp \
    Common/IdBasedWidgetCollection/idbasedwidgetcollection.cpp \
    Browser/Content/Page/applicationstoreapplicationpage.cpp \
    Application/Communication/Request/grabapplicationnetworkrequest.cpp \
    Application/Communication/Entity/grabapplicationnetworkentity.cpp \
    Application/Communication/Response/grabapplicationnetworkresponse.cpp \
    Browser/Content/Page/helppage.cpp \
    Common/Widgets/helpitemwidget.cpp \
    Common/Sorting/cleverapplicationiconpresentersortingdelegate.cpp \
    Common/Menu/proposehidemenuitemelementevent.cpp \
    Common/Menu/menumanager.cpp \
    Common/Menu/menuitemwidget.cpp \
    Common/Page/pagewidgetbase.cpp \
    Common/Page/pagenavigationpagestack.cpp \
    Common/Page/pagenavigationmanager.cpp \
    Common/Page/Delegate/simplefadepagetransitionmanagerdelegate.cpp \
    Common/Menu/plainverticalmenupresenterwidget.cpp \
    Common/Settings/settingsbase.cpp \
    Common/Menu/menuitemholderwidgetbase.cpp \
    Common/Menu/menuitemholderwithleftinsetwidget.cpp \
    Common/Menu/proposehideanddeleteaftermenuitemelementevent.cpp \
    Common/Layouts/dynamicsamesizehorizontalboxlayout.cpp \
    Common/Layouts/dynamicsamesizehorizontallinelayout.cpp \
    Common/Widgets/dynamicsamesizehorizontalboxlayoutbasedsmartscrollarea.cpp \
    Common/Helpers/qstringhelper.cpp \
    Browser/Content/Widget/applicationthumbnailwidget.cpp \
    Common/Widgets/intelligentseparatedmultifunctionalbuttonwidget.cpp \
    Common/Widgets/intelligentseparatedmultifunctionalbuttonoptionitemwidget.cpp \
    Common/Widgets/intelligentseparatedmultifunctionalbuttonoptionitemmodel.cpp \
    Common/Widgets/animatedtwofixedsizewidgetbase.cpp \
    Common/SvgToPixmapRenderer/svgtopixmaprenderer.cpp \
    Common/Helpers/textsizehelper.cpp \
    Common/BorderImageRenderer/borderimagerenderer.cpp \
    Browser/Content/Widget/appiconandbuttonpresenterwidget.cpp \
    Browser/Content/Widget/myappsmanagementsidebar.cpp \
    Application/ApplicationDescription/applicationdescriptionmodel.cpp \
    Browser/Content/Page/myappspage.cpp \
    Common/Sorting/widgetlistsnapshothandlerdelegate.cpp \
    Common/Sorting/widgetlistsnapshot.cpp \
    Common/Layouts/SnapshotChangeDelegates/samesizelayoutsnapshotchangedelegatebase.cpp \
    Common/Event/showstorepageforapplicationevent.cpp \
    Common/ApplicationRunner/applicationrunner.cpp \
	Common/Security/authorization.cpp \
    Application/Communication/Request/userloginnetworkrequest.cpp \
    Common/Widgets/installprogressnotificationwidget.cpp \
    Common/DesktopShortcutCreator/desktopshortcutcreator.cpp \
    Common/System/systemprofiler.cpp \
    Common/Layouts/SnapshotChangeDelegates/samesizelayoutsnapshotchangedelegatewithfading.cpp \
    Common/FadingItemBase/fadingitembase.cpp \
    Browser/Content/Widget/userloginsidebarwidget.cpp \
    Application/Communication/Request/grablistnetworkrequest.cpp \
    Application/Communication/Entity/grablistnetworkentity.cpp \
    Browser/Content/Widget/navigationwidget.cpp \
    Common/Widgets/textandacceptcancelbuttonsnotificationwidget.cpp \
    Common/Database/simplelogdatabasemanager.cpp \
    Common/Widgets/textandacceptcanceldetailsbuttonsnotificationwidget.cpp \
    Application/Communication/Entity/applicationreviewentity.cpp \
    Application/LocalAppDatabaseServer/staticappinfofilehandler.cpp \
    Common/Helpers/platformflagshelper.cpp \
    Application/Communication/Request/applicationsearchnetworkrequest.cpp \
    Common/Helpers/bestapplicationiconimageselectionhelper.cpp \
    Common/imagewithtypeinfo.cpp \
    Browser/Content/Widget/applicationstoreappwidget.cpp \
    Browser/Content/Widget/ratingwidget.cpp \
    Application/Communication/Entity/applicationscreenshotentity.cpp \
    Common/ClientSelfUpdateManager/clientselfupdatemanager.cpp \
    Common/Uncompressor/uncompressor.cpp \
    Common/InterProcessCommunication/interprocesscommunicatorserver.cpp \
    Application/Communication/Request/clientupdatenetworkrequest.cpp \
    Application/Communication/Request/grabclientupdatenetworkrequest.cpp \
    Common/InterProcessCommunication/interprocesscommunicator.cpp \
    Browser/Content/Page/clientselfupdatepage.cpp \
    Common/Helpers/delayedretryhelper.cpp \
    Browser/Content/Widget/helpitempresenterwindow.cpp \
    Common/Widgets/fadingwidgetbase.cpp \
    Common/Widgets/fadingbutton.cpp \
    Common/fadingitemdelegate.cpp \
    Common/Database/selfrepairingdatabase.cpp \
    Browser/Content/Page/whatsnewpage.cpp \
    Application/applicationglobalstateregistry.cpp \
    Application/appspecificpathhelper.cpp \
    Common/Helpers/pathhelper.cpp \
    Application/globalobjectregistry.cpp \
    Common/Helpers/databasehelper.cpp \
    Application/LocalAppDatabaseServer/localappdatabaseworkperformer.cpp \
    Common/Database/databaseworkperformerbase.cpp \
    Application/LocalAppDatabaseServer/Agent/getdynamicappinfoagent.cpp \
    Application/LocalAppDatabaseServer/Agent/getstaticappinfoagent.cpp \
    Application/LocalAppDatabaseServer/Agent/getallappdescriptionmodelsagent.cpp \
    Application/LocalAppDatabaseServer/Agent/getappstateagent.cpp \
    Application/LocalAppDatabaseServer/Agent/getwholeappdescriptionagent.cpp \
    Application/LocalAppDatabaseServer/Agent/addorupdatewholeappdescription.cpp \
    Application/LocalAppDatabaseServer/Agent/updateappstateagent.cpp \
    Common/Database/databaseserver.cpp \
    Application/LocalAppDatabaseServer/localappdatabasemanager.cpp \
    Application/LocalAppDatabaseServer/Agent/updateappishiddenagent.cpp \
    Application/LocalAppDatabaseServer/Agent/updateappisstartwhenclientstartsagent.cpp \
    Browser/BackgroundWorkers/background_grabsyncperformer.cpp \
    Application/LocalAppDatabaseServer/Agent/appdataqueryhelpers.cpp \
    Common/Widgets/delegatingwidget.cpp \
    Common/ApplicationRunner/background_applicationrunner.cpp \
    Common/ClientSelfUpdateManager/background_clientuncompressor.cpp \
    Browser/AppUninstall/appuninstaller.cpp \
    Common/Runnables/runnable_copydirectory.cpp \
    Common/Runnables/runnable_deletedirectoryrecursively.cpp \
    Common/Widgets/uninstallprogressnotificationwidget.cpp \
    Browser/Content/Widget/appterminatorsidebarwidget.cpp \
    Application/Controllers/appterminationcontroller.cpp \
    Application/appwhirrapplication.cpp \
    Application/Delegates/urlschemehandlerdelegate.cpp \
    Browser/specialpageactioncontroller.cpp \
    Common/Runnables/runnable_checkforrunningexecutable.cpp \
    Browser/Content/Page/licencepresenterpage.cpp \
    Browser/AppStore/appstoresearchcontroller.cpp \
    Browser/Content/Page/appstoresearchresultpage.cpp \
    Common/Widgets/busyindicatorwidget.cpp \
    Common/Widgets/contentcoverstatusmessagewidget.cpp \
    Application/User/usermanager.cpp \
    Application/User/userdatabasemanager.cpp \
    Application/CriticalAction/criticalactionmanager.cpp \
    Common/Widgets/checkboxbutton.cpp \
    Browser/Notification/selftimeoutingnotificationwidgetbase.cpp \
    Common/Menu/menupresenterwidgetbase.cpp \
    Common/guiconfigurationmanager.cpp \
    Common/Helpers/qcolorhelper.cpp \
    Common/Layouts/flowlayout.cpp \
    Common/Layouts/samesizelayoutbase.cpp \
    Common/Layouts/samesizehorizontalflowlayout.cpp \
    Common/Widgets/delegatingscrollarea.cpp \
    Common/Widgets/samesizehorizontalflowlayoutbasedscrollarea.cpp \
    Common/Widgets/multisectionsamesizehorizontalflowlayoutbasedscrollarea.cpp \
    Browser/Content/Widget/appsubstatespresenterwidget.cpp \
    Common/Widgets/clientselfupdateavailablenotificationwidget.cpp \
    Application/Communication/Entity/applicationbaseinfosnetworkentity.cpp \
    Application/Communication/Entity/applicationlistwithsectionsnetworkentity.cpp \
    Application/Communication/Request/storemainpagewithsectionsnetworkrequest.cpp \
    Application/Communication/Response/applicationlistwithsectionsnetworkresponse.cpp \
    Application/Communication/Entity/applicationbaseinfosnetworkentitymodel.cpp \
    Application/Communication/Entity/getappeulanetworkentity.cpp \
    Application/Communication/Request/getappeulanetworkrequest.cpp \
    Application/Communication/Response/getappeulanetworkresponse.cpp \
    Application/speciallocalappdatabaseoperationids.cpp \
    Settings/configmanager.cpp \
    Application/Widgets/Store/sectionwithfeatureditemiconbackgroundwidget.cpp \
    Common/Helpers/qpixmaphelper.cpp \
    Browser/Content/Page/appwhirrlicencepage.cpp \
    Common/Widgets/coverwithcustomwidget.cpp \
    Application/Widgets/networkunreachablewidget.cpp \
    Common/referencecountingidbasedcollection.cpp \
    Common/multicontentmanager.cpp \
    Browser/Content/Widget/plainerrorpresenterwidget.cpp \
    Application/Communication/Response/clientupdatechecknetworkresponse.cpp \
    Application/Communication/Entity/appversionnetworkentity.cpp \
    Common/Json/json.cpp \
    Application/Communication/Request/applistversioninfonetworkrequest.cpp \
    Common/WidgetResizerDelegate/widgetresizerdelegate.cpp \
    Common/Widgets/urlbutton.cpp \
    Browser/Notification/simpleselftimeoutwithtextnotificationwidget.cpp \
    Application/appdatabasesyncmanager.cpp \
    Common/Widgets/popupwebviewwidget.cpp \
    Common/Security/Login/userloginwidget3.cpp \
    Common/Application/macprocess.cpp \
    Application/uifactory.cpp \
    Application/Widgets/incontentnavigationwidget.cpp \
    Application/Communication/Request/userfeedbackrequest.cpp \
    Application/Communication/Request/usersuggestapprequest.cpp \
    Common/Widgets/layeredimagepresenterwidget.cpp \
    Common/Helpers/qimagehelper.cpp \
    Common/Widgets/magnetwidget.cpp \
    Common/Widgets/bettertextedit.cpp \
    Common/Security/removefromquarantine.cpp \
    Application/Managers/AppGrab/grabapplicationmanager.cpp \
    Application/Managers/AppGrab/appgrabdelegate.cpp \
    Application/Managers/AppGrab/grabapplicationqueueitem.cpp \
    Application/Managers/AppGrab/appdownloaddelegate.cpp \
    Application/Managers/AppGrab/appinstalldelegate.cpp \
    Common/Framework/frameworkinstaller.cpp \
    Common/Framework/frameworkinfo.cpp \
    Common/Framework/frameworkcheckmodel.cpp \
    Common/Framework/backgroundframeworkinstaller.cpp \
    Common/Framework/backgroundframeworkcheck.cpp \
    Common/Framework/availableframeworkchecker.cpp \
    Application/Managers/AppGrab/appframeworkdelegate.cpp \
    Application/Frameworks/frameworkcheckerdatabase.cpp \
    Application/Frameworks/frameworkcheckerinfo.cpp \
    Application/Communication/Request/appframeworkinfonetworkrequest.cpp \
    Application/Communication/Response/appframeworkinfonetworkresponse.cpp \
    Application/Communication/Entity/appframeworkinfonetworkentity.cpp \
    Browser/Content/Page/applicationstoremorepage.cpp \
    Common/Widgets/multisectiondata.cpp \
    Common/widgetanimatedtwosizeresizer.cpp \
    Common/Widgets/coverwidgetwithautofade.cpp \
    Application/Widgets/autostartappsnotificationwidget.cpp \
    Common/Runnables/runnable_saveresourcetofile.cpp \
    Common/Communication/resourcedownloadstreamtomemory.cpp \
    Common/Communication/resourcedownloadstreamtofile.cpp \
    Common/Communication/localmessagesender.cpp \
    Common/Communication/basenetworkresponse.cpp \
    Common/Communication/basenetworkrequest.cpp \
    Common/File/streamtofilemanager.cpp \
    Common/File/streamtofile.cpp \
    Common/bufferedtransmitter.cpp \
    Common/backgroundworkerqueue.cpp \
    Application/Communication/awresourcedownloadstreamtofile.cpp \
    Common/Communication/communicationresourcegateway.cpp \
    Application/Communication/awdefaultcommunicationresourcegatewaycachedelegate.cpp \
    Application/Communication/awdefaultnetworkrequestcachedelegate.cpp \
    Application/Communication/Request/awbasenetworkrequest.cpp \
    Application/Communication/awresourcedownloadstreamtomemoryfactory.cpp \
    Application/Utils/awapplicationrunner.cpp \
    Application/Utils/awshortcuthandler.cpp \
    Application/Utils/localapppathhelper.cpp \
    Common/Runnables/runnable_qimagefromresourceloader.cpp \
    Common/loggerserver.cpp \
    Application/Widgets/offclientnotificationwindowview.cpp \
    Application/Widgets/offclientnotificationwindowvc.cpp \
    Common/Helpers/awqprocessextension.cpp \
    Common/awfilefolderbackup.cpp \
    Browser/Content/Page/uninstallconfirmandfeedbackpage.cpp

# headers
HEADERS  += \
    Common/Event/confirmapplicationdeactivaterequestevent.h \
    Common/Event/confirmapplicationdeactivateresponseevent.h \
    Settings/guiconfigurations.h \
    Common/Event/ensureapplicationpresentedevent.h \
    Common/Event/forcehideorminimizeevent.h \
    Common/Event/startthisapplicationevent.h \
    Settings/settings.h \
    Browser/browserwindow.h \
    Common/Sorting/sortresultchangeditem.h \
    Application/Delegates/globalhotkeydelegate.h \
    Application/Delegates/systemtrayicondelegate.h \
    Application/Delegates/autostartupdelegate.h \
    Common/Sorting/sortingdatabasemanager.h \
    Common/Event/forcehideorminimizebrowserevent.h \
    Common/Layouts/layoutinginformations.h \
    Browser/Sidebar/browsersidebarcontainer.h \
    Browser/Featured/browserfeaturedmainwidget.h \
    Common/Datasource/datasourcewidget.h \
    Browser/Sidebar/browsersidebartextwidget.h \
    Common/Datasource/datasourceelementwidget.h \
    Browser/Content/Page/optionscontentpage.h \
    Common/Widgets/faderwidget.h \
    Common/Widgets/crossfaderwidget.h \
    Browser/Content/Page/suggestanapppage.h \
    Settings/menuitemidcenter.h \
    Settings/pageidcenter.h \
    Browser/Content/Page/feedbackpage.h \
    Common/Widgets/simpleimagepresenterwidget.h \
    Common/Helpers/widgetinlayouthelper.h \
    Common/WidgetReplacers/animatedwidgetreplacer.h \
    Common/Helpers/qsizehelper.h \
    Common/WidgetReplacers/parallelanimatedwidgetreplacer.h \
    Common/WidgetReplacers/parallelanimatedwidgetreplaceritem.h \
    Browser/Content/Page/errorpage.h \
    Browser/Content/Page/browserpagebase.h \
    Common/Layouts/SnapshotChangeDelegates/snapshotqueueitem.h \
    Common/Layouts/SnapshotChangeDelegates/itemmoveddata.h \
    Common/Layouts/dynamicdelegatebasedhighperformancesamesizelayoutbase.h \
    Browser/Notification/notificationmanager.h \
    Browser/Notification/notificationwidget.h \
    Common/Layouts/dynamicsamesizeverticallinelayout.h \
    Common/Application/Installation/Updater/baseupdater.h \
    Common/Application/Installation/Compression/gojobzip.h \
    Common/Application/applicationterminator.h \
    Common/Database/databasetableschemedescriptor.h \
    Common/Database/basedatabase.h \
    Developer/macros.h \
    Common/Event/selectfiltersearchguiitemtextevent.h \
    Common/Application/platformspecificprogramidentifierhelper.h \
    Common/Application/win_applicationprocessbyexedata.h \
    Application/Communication/Response/applicationlistnetworkresponse.h \
    Application/Communication/Request/applicationlistnetworkrequest.h \
    Application/Communication/Gateway/communicationdescriptiongateway.h \
    Application/Communication/Request/applicationnetworkrequest.h \
    Application/Communication/Response/applicationnetworkresponse.h \
    Application/Communication/Entity/applicationnetworkentity.h \
    Browser/Content/Page/applicationstoremainpage.h \
    Common/mousegrabscrolldelegate.h \
    Common/Widgets/idbasedselectableandhighlightablewidgetbase.h \
    Common/Widgets/idbasedwidgetbase.h \
    Common/Widgets/iconimagewidget.h \
    Common/imagetypesenum.h \
    Common/IdBasedWidgetCollection/idbasedwidgetcollection.h \
    Browser/Content/Page/applicationstoreapplicationpage.h \
    Application/Communication/Request/grabapplicationnetworkrequest.h \
    Application/Communication/Entity/grabapplicationnetworkentity.h \
    Application/Communication/Response/grabapplicationnetworkresponse.h \
    Browser/Content/Page/helppage.h \
    Common/Widgets/helpitemwidget.h \
    Common/Sorting/cleverapplicationiconpresentersortingdelegate.h \
    Common/Menu/proposehidemenuitemelementevent.h \
    Common/Menu/menumanager.h \
    Common/Menu/menuitemwidget.h \
    Common/Page/pagewidgetbase.h \
    Common/Page/pagenavigationpagestack.h \
    Common/Page/pagenavigationmanager.h \
    Common/Page/Delegate/simplefadepagetransitionmanagerdelegate.h \
    Common/Page/Delegate/pagetransitionmanagerdelegateinterface.h \
    Common/Menu/plainverticalmenupresenterwidget.h \
    Common/Settings/settingsbase.h \
    Common/Menu/menuitemholderwidgetbase.h \
    Common/Menu/menuitemholderwithleftinsetwidget.h \
    Common/Menu/proposehideanddeleteaftermenuitemelementevent.h \
    Common/Layouts/dynamicsamesizehorizontalboxlayout.h \
    Common/Layouts/dynamicsamesizehorizontallinelayout.h \
    Common/Widgets/dynamicsamesizehorizontalboxlayoutbasedsmartscrollarea.h \
    Common/Helpers/qstringhelper.h \
    Browser/Content/Widget/applicationthumbnailwidget.h \
    Common/Widgets/intelligentseparatedmultifunctionalbuttonwidget.h \
    Common/Widgets/intelligentseparatedmultifunctionalbuttonoptionitemwidget.h \
    Common/Widgets/intelligentseparatedmultifunctionalbuttonoptionitemmodel.h \
    Common/Widgets/animatedtwofixedsizewidgetbase.h \
    Common/SvgToPixmapRenderer/svgtopixmaprenderer.h \
    Common/Helpers/textsizehelper.h \
    Common/BorderImageRenderer/borderimagerenderer.h \
    Browser/Content/Widget/appiconandbuttonpresenterwidget.h \
    Browser/Content/Widget/myappsmanagementsidebar.h \
    Application/ApplicationDescription/applicationdescriptionmodel.h \
    Browser/Content/Page/myappspage.h \
    Common/Sorting/widgetlistsnapshot.h \
    Common/Sorting/widgetlistsnapshothandlerdelegate.h \
    Common/Layouts/SnapshotChangeDelegates/samesizelayoutsnapshotchangedelegatebase.h \
    Common/Event/showstorepageforapplicationevent.h \
    Common/ApplicationRunner/applicationrunner.h \
	Common/Security/authorization.h \
    Application/Communication/Request/userloginnetworkrequest.h \
    Common/Widgets/installprogressnotificationwidget.h \
    Browser/Notification/selftimeoutingnotificationwidgetbase.h \
    Common/DesktopShortcutCreator/desktopshortcutcreator.h \
    Common/System/systemprofiler.h \
    Common/Layouts/SnapshotChangeDelegates/samesizelayoutsnapshotchangedelegatewithfading.h \
    Common/FadingItemBase/fadingitembase.h \
    Browser/Content/Widget/userloginsidebarwidget.h \
    Application/Communication/Request/grablistnetworkrequest.h \
    Application/Communication/Entity/grablistnetworkentity.h \
    Browser/Content/Widget/navigationwidget.h \
    Common/Widgets/textandacceptcancelbuttonsnotificationwidget.h \
    Common/Database/simplelogdatabasemanager.h \
    Common/Widgets/textandacceptcanceldetailsbuttonsnotificationwidget.h \
    Application/Communication/Entity/applicationreviewentity.h \
    Application/LocalAppDatabaseServer/staticappinfofilehandler.h \
    Common/Helpers/platformflagshelper.h \
    Application/Communication/Request/applicationsearchnetworkrequest.h \
    Common/Helpers/bestapplicationiconimageselectionhelper.h \
    Common/imagewithtypeinfo.h \
    Application/ApplicationDescription/applicationstatesenum.h \
    Browser/Content/Widget/applicationstoreappwidget.h \
    Browser/Content/Widget/ratingwidget.h \
    Application/Communication/Entity/applicationscreenshotentity.h \
    Common/ClientSelfUpdateManager/clientselfupdatemanager.h \
    Common/Menu/menupresenterwidgetbase.h \
    Common/Uncompressor/uncompressor.h \
    Common/Uncompressor/gojobzip.h \
    Application/Communication/Request/clientupdatenetworkrequest.h \
    Application/Communication/Request/grabclientupdatenetworkrequest.h \
    Common/InterProcessCommunication/interprocesscommunicatorserver.h \
    Common/InterProcessCommunication/interprocesscommunicator.h \
    Browser/Content/Page/clientselfupdatepage.h \
    Common/Helpers/delayedretryhelper.h \
    Browser/Content/Widget/helpitempresenterwindow.h \
    Common/Widgets/fadingwidgetbase.h \
    Common/Widgets/fadingbutton.h \
    Common/fadingitemdelegate.h \
    Common/fadingiteminterface.h \
    Common/Database/selfrepairingdatabase.h \
    Browser/Content/Page/whatsnewpage.h \
    Application/applicationglobalstateregistry.h \
    Application/appspecificpathhelper.h \
    Common/Helpers/pathhelper.h \
    Application/globalobjectregistry.h \
    Common/Database/databaseworkagentinterface.h \
    Common/Helpers/databasehelper.h \
    Application/LocalAppDatabaseServer/localappdatabaseworkperformer.h \
    Common/Database/databaseworkperformerbase.h \
    Application/LocalAppDatabaseServer/Agent/getdynamicappinfoagent.h \
    Application/LocalAppDatabaseServer/Agent/getstaticappinfoagent.h \
    Application/LocalAppDatabaseServer/Agent/getallappdescriptionmodelsagent.h \
    Application/LocalAppDatabaseServer/Agent/getappstateagent.h \
    Application/LocalAppDatabaseServer/Agent/getwholeappdescriptionagent.h \
    Application/LocalAppDatabaseServer/Agent/addorupdatewholeappdescription.h \
    Application/LocalAppDatabaseServer/Agent/updateappstateagent.h \
    Common/Database/databaseserver.h \
    Common/Database/databaseserverdelegate.h \
    Application/LocalAppDatabaseServer/localappdatabasemanager.h \
    Application/LocalAppDatabaseServer/Agent/updateappishiddenagent.h \
    Application/LocalAppDatabaseServer/Agent/updateappisstartwhenclientstartsagent.h \
    Browser/BackgroundWorkers/background_grabsyncperformer.h \
    Application/LocalAppDatabaseServer/addorupdateconditionflags.h \
    Application/LocalAppDatabaseServer/Agent/appdataqueryhelpers.h \
    Common/Widgets/delegatingwidget.h \
    Common/ApplicationRunner/background_applicationrunner.h \
    Common/ClientSelfUpdateManager/background_clientuncompressor.h \
    Browser/AppUninstall/appuninstaller.h \
    Common/Runnables/runnable_copydirectory.h \
    Common/Runnables/runnable_deletedirectoryrecursively.h \
    Common/Widgets/uninstallprogressnotificationwidget.h \
    Common/Application/platformspecificrunningexecutabledatainterface.h \
    Browser/Content/Widget/appterminatorsidebarwidget.h \
    Application/Controllers/appterminationcontroller.h \
    Application/appwhirrapplication.h \
    Application/Delegates/urlschemehandlerdelegate.h \
    Application/AfterLoginActions/afterloginactioninterface.h \
    Application/AfterLoginActions/grabsyncafterloginaction.h \
    Application/AfterLoginActions/showappdetailpageforappidafterloginaction.h \
    Browser/specialpageactioncontroller.h \
    Application/AfterLoginActions/runappafterloginaction.h \
    Common/Runnables/runnable_checkforrunningexecutable.h \
    Browser/Content/Page/licencepresenterpage.h \
    Browser/AppStore/appstoresearchcontroller.h \
    Browser/Content/Page/appstoresearchresultpage.h \
    Common/Widgets/busyindicatorwidget.h \
    Common/Widgets/contentcoverstatusmessagewidget.h \
    Application/User/usermanager.h \
    Application/User/userdatabasemanager.h \
    Application/CriticalAction/criticalactionmanager.h \
    Application/CriticalAction/criticalactioncodes.h \
    Common/Widgets/checkboxbutton.h \
    Common/guiconfigurationmanager.h \
    Common/Helpers/qcolorhelper.h \
    Common/Layouts/flowlayout.h \
    Common/Layouts/samesizelayoutbase.h \
    Common/Layouts/samesizehorizontalflowlayout.h \
    Common/Widgets/delegatingscrollarea.h \
    Common/Widgets/sectionbasedhorizontalflowlayoutpresenterdelegate.h \
    Common/Widgets/samesizehorizontalflowlayoutbasedscrollarea.h \
    Common/Widgets/multisectionsamesizehorizontalflowlayoutbasedscrollarea.h \
    Browser/Content/Widget/appsubstatespresenterwidget.h \
    Common/Widgets/clientselfupdateavailablenotificationwidget.h \
    Application/Communication/Entity/applicationbaseinfosnetworkentity.h \
    Application/Communication/Entity/applicationlistwithsectionsnetworkentity.h \
    Application/Communication/Request/storemainpagewithsectionsnetworkrequest.h \
    Application/Communication/Response/applicationlistwithsectionsnetworkresponse.h \
    Application/Communication/Entity/applicationbaseinfosnetworkentitymodel.h \
    Application/Communication/Entity/getappeulanetworkentity.h \
    Application/Communication/Request/getappeulanetworkrequest.h \
    Application/Communication/Response/getappeulanetworkresponse.h \
    Application/speciallocalappdatabaseoperationids.h \
    Settings/configmanager.h \
    Application/Widgets/Store/sectionwithfeatureditemiconbackgroundwidget.h \
    Common/Helpers/qpixmaphelper.h \
    Browser/Content/Page/appwhirrlicencepage.h \
    Common/Widgets/coverwithcustomwidget.h \
    Application/Widgets/networkunreachablewidget.h \
    Common/idbasediteminterface.h \
    Common/referencecountingidbasedcollection.h \
    Common/multicontentmanager.h \
    Browser/Content/Widget/plainerrorpresenterwidget.h \
    Application/Communication/Response/clientupdatechecknetworkresponse.h \
    Application/Communication/Entity/appversionnetworkentity.h \
    Common/Json/json.h \
    Application/Communication/Request/applistversioninfonetworkrequest.h \
    Application/AfterLoginActions/offlineuserapplistupdatecheckafterloginaction.h \
    Common/WidgetResizerDelegate/widgetresizerdelegate.h \
    Common/Widgets/urlbutton.h \
    Browser/Notification/simpleselftimeoutwithtextnotificationwidget.h \
    Application/appdatabasesyncmanager.h \
    Common/Widgets/popupwebviewwidget.h \
    Common/Security/Login/userloginwidget3.h \
    Common/Application/macprocess.h \
    Application/uifactory.h \
    Application/Widgets/incontentnavigationwidget.h \
    Application/Communication/Request/userfeedbackrequest.h \
    Application/Communication/Request/usersuggestapprequest.h \
    Common/Widgets/layeredimagepresenterwidget.h \
    Common/Helpers/qimagehelper.h \
    Common/Widgets/magnetwidget.h \
    Common/Widgets/bettertextedit.h \
    Common/Security/removefromquarantine.h \
    Application/Managers/AppGrab/grabapplicationmanager.h \
    Application/Managers/AppGrab/appgrabdelegate.h \
    Application/Managers/AppGrab/grabapplicationqueueitem.h \
    Application/Managers/AppGrab/appdownloaddelegate.h \
    Application/Managers/AppGrab/appinstalldelegate.h \
    Common/Framework/frameworkinstaller.h \
    Common/Framework/frameworkinfo.h \
    Common/Framework/frameworkcheckmodel.h \
    Common/Framework/backgroundframeworkinstaller.h \
    Common/Framework/backgroundframeworkcheck.h \
    Common/Framework/availableframeworkchecker.h \
    Application/Managers/AppGrab/appframeworkdelegate.h \
    Application/Frameworks/frameworkcheckerdatabase.h \
    Application/Frameworks/frameworkcheckerinfo.h \
    Application/Communication/Request/appframeworkinfonetworkrequest.h \
    Application/Communication/Response/appframeworkinfonetworkresponse.h \
    Application/Communication/Entity/appframeworkinfonetworkentity.h \
    Browser/Content/Page/applicationstoremorepage.h \
    Common/Widgets/multisectiondata.h \
    Common/widgetanimatedtwosizeresizer.h \
    Common/Widgets/coverwidgetwithautofade.h \
    Application/AfterLoginActions/showsearchinstorepageandsearchafterloginaction.h \
    Application/Widgets/autostartappsnotificationwidget.h \
    Common/Runnables/runnable_saveresourcetofile.h \
    Common/Communication/resourcedownloadstreamtomemory.h \
    Common/Communication/resourcedownloadstreamtofile.h \
    Common/Communication/resourcedownloadnetworkrequest.h \
    Common/Communication/networkrequestcachedelegate.h \
    Common/Communication/localmessagesender.h \
    Common/Communication/localmessagecodesenum.h \
    Common/Communication/communicationresourcegateway.h \
    Common/Communication/basenetworkresponse.h \
    Common/Communication/basenetworkrequest.h \
    Common/Communication/basenetworkentity.h \
    Common/File/streamtofilemanager.h \
    Common/File/streamtofile.h \
    Common/runnableitem.h \
    Common/bufferedtransmitter.h \
    Common/backgroundworkerqueue.h \
    Application/Communication/awresourcedownloadstreamtofile.h \
    Common/Communication/communicationresourcegatewaydelegate.h \
    Common/Communication/communicationresourcegatewaycachedelegate.h \
    Application/Communication/awdefaultcommunicationresourcegatewaycachedelegate.h \
    Application/Communication/awdefaultnetworkrequestcachedelegate.h \
    Application/Communication/Request/awbasenetworkrequest.h \
    Application/Communication/awresourcedownloadstreamtomemoryfactory.h \
    Application/Utils/awapplicationrunner.h \
    Application/Utils/awshortcuthandler.h \
    Application/Utils/localapppathhelper.h \
    Common/Runnables/runnable_qimagefromresourceloader.h \
    Common/loggerserver.h \
    Application/Managers/AppGrab/appinstallstates.h \
    Application/Widgets/offclientnotificationwindowview.h \
    Application/Widgets/offclientnotificationwindowvc.h \
    Common/awtask.h \
    Common/Helpers/awqprocessextension.h \
    Common/awfilefolderbackup.h \
    Browser/Content/Page/uninstallconfirmandfeedbackpage.h

# forms
FORMS += \
    Browser/Content/Page/suggestanapppage.ui \
    Browser/Content/Page/feedbackpage.ui \
    Browser/Content/Page/errorpage.ui \
    Common/Application/applicationterminator.ui \
    Browser/Content/Page/helppage.ui \
    Common/Widgets/helpitemwidget.ui \
    Common/Security/Login/userloginwidget.ui

# resources
RESOURCES += \
    resources.qrc

mac {
    OBJECTIVE_SOURCES += \
        Application/Delegates/urlschemehandlerdelegate_objc.mm \
        Application/Delegates/autostartupdelegate_objc.mm \
        Common/Security/removefromquarantine_objc.mm
}


# -------------------
# platform and build type specific configurations

#
# platform specific

# ===================
# -- MAC
mac {
# icon
    ICON = ApplicationLauncher.icns
    LIBS+=/System/Library/Frameworks/Carbon.framework/Carbon
    LIBS += -framework AppKit
    LIBS+=/System/Library/Frameworks/ApplicationServices.framework/ApplicationServices

# depending libs
    LIBS += -L"./../../EasyAppsClientProject-build-desktop/3rdParty/quazip" -lquazip

#copy dynlib
# NOTE: every QMAKE_POST_LINK will be fired in one line!! here are the tips / tricks if you want to run more than 1 post-link action: http://www.qtforum.org/article/14932/qmake-post-link-in-pro.html
    QMAKE_POST_LINK += ./../../../../UsefulScriptsAndTools/mac/dynlib/deploy.sh
    QMAKE_INFO_PLIST += ./MacResources/Info.plist
# TODO
#   DESTDIR = "./../../EasyAppsClientProject-build-desktop/EasyAppsClient"
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

        # !! for MinGW !!
#        LIBS += user32.lib
#        LIBS += advapi32.lib
#        LIBS += shell32.lib
#        LIBS += Ole32.lib

# destination / build dir
        DESTDIR = "./../../EasyAppsClientProject-build-desktop/BIN/release"
    }

    # NOTE: every QMAKE_POST_LINK will be fired in one line!! here are the tips / tricks if you want to run more than 1 post-link action: http://www.qtforum.org/article/14932/qmake-post-link-in-pro.html
    # this does not work for some reason...
    QMAKE_POST_LINK = "$$(APPWHIRR_PROJECT_SOURCE_ROOT_DIR)\\EasyAppsClient\\PostBuildScripts\\Windows\\windows_post_build_scripts_main.bat"
}


# ===================
# AllOS, debug and release (configuration) specific

# all
#   logging (required by every appwhirr logging)
DEFINES += ENABLE_APPWHIRR_LOGGING
DEFINES += ENABLE_REVIEW_THIS_LOGGING
DEFINES += ENABLE_IGNORE_SSL_ERRORS_IN_SERVER_COMMUNICATION
# this won't effect normal not-implemented-warnings, only enables / disables some internal todo like warnings
DEFINES += ENABLE_INTERNAL_NOT_IMPLEMENTED_WARNING_LOGGING
DEFINES += ENABLE_DEPRECATED_WARNING_LOGGING

DEFINES += ENABLE_DYNAMIC_USER_RELATED_METHODS_IN_PATH_HELPER

# production only
contains(VAR_IS_COMPILE_PUBLIC_PRODUCT, true) {
    DEFINES += PUBLIC_PRODUCTION_READY_VERSION
    DEFINES += USE_PUBLIC_PRODUCTION_SERVER
# !!!remove!!!
#    DEFINES += USE_NETWORK_REFRESH_TIMES_FOR_TESTING
} else {
    DEFINES += USE_NETWORK_REFRESH_TIMES_FOR_TESTING
}

# changing defines sometimes requires a manual/full clean!!
# probably there's no release {} section, and even !debug{} isn't working
# by the way, this is the "canonical" qmake form, and it works :)
CONFIG( debug, debug|release ) {
    # debug

# g++ specific
*-g++* {
# -Wall forces maximum warning level for G++
    QMAKE_CXXFLAGS_DEBUG += -Wall
# -pedantic forces ANSI compliance and will fail if the project is not ANSI compliant
# Qt is not ANSI compliant (ANSI C++ does not allow 'long long' for example)
#    QMAKE_CXXFLAGS_DEBUG += -pedantic
}

# debug logging (additional, debug logging)
    DEFINES += ENABLE_APPWHIRR_DEBUG_LOGGING

#    DEFINES += ENABLE_DEBUG_ASSERT_LOG_ASSERTATION

    #
    # enable these to allow to run a debug and a release version at the same time
    DEFINES += USE_DEBUG_LOCAL_FOLDER
    DEFINES += USE_DEBUG_LOCAL_SERVER_CONNECTION_NAMES

    #
    # logging enable defines - changing them sometimes require a full-rebuild

#    DEFINES += ENABLE_LOCAL_APPLICATION_SCAN_LOGGING
    DEFINES += ENABLE_COMMUNICATION_CORE_LOGGING
    DEFINES += ENABLE_COMMUNICATION_DEBUG_LOGGING
    DEFINES += ENABLE_COMPRESSION_LOGGING
##    DEFINES += ENABLE_APPLICATION_ACTIVATE_DEACTIVATE_STATE_LOGGING
    DEFINES += ENABLE_HOTKEY_EVENT_LOGGING
#    DEFINES += ENABLE_MENU_OPERATION_LOGGING
#    DEFINES += ENABLE_PATH_MANAGER_LOGGING
    DEFINES += ENABLE_FILE_SYSTEM_LOGGING

#    DEFINES += ENABLE_PAGE_NAVIGATION_LOGGING
#    DEFINES += ENABLE_SMART_LAYOUTING_SIZE_EVENT_LOGGING
##    DEFINES += ENABLE_DYNAMIC_LAYOUTING_LOGGING
#    DEFINES += ENABLE_SNAPSHOT_HANDLING_LOGGING
#    DEFINES += ENABLE_PAGE_TRANSITION_LOGGING

    #DEFINES += ENABLE_DATABASE_OPERATION_LOGGING
    DEFINES += ENABLE_LOCAL_APPLICATION_DATABASE_OPERATION_HANDLING
#    DEFINES += ENABLE_SYSTEM_PROFILER_LOGGING
    DEFINES += ENABLE_CRITICAL_ACTION_LOGGING

    #
    # other, utility logging enable/disable

#    DEFINES += USE_NETWORK_REFRESH_TIMES_FOR_TESTING

    #
    # non-production ready feature hiding
    DEFINES += ENABLE_DEVELOPER_PREVIEW_FEATURES

} else {
    # release
#    DEFINES += QT_NO_DEBUG
#   // this would completely turn-off every Qt logging (qDebug, qError, etc.)
#    DEFINES += QT_NO_DEBUG_OUTPUT

    # enable these for debug-logging (required by every appwhirr logging) in release mode
    DEFINES += TRANSFER_LOG_TO_FILE

    DEFINES += ENABLE_COMMUNICATION_CORE_LOGGING
    DEFINES += ENABLE_COMMUNICATION_DEBUG_LOGGING

    DEFINES += ENABLE_LOCAL_APPLICATION_DATABASE_OPERATION_HANDLING
    DEFINES += ENABLE_SYSTEM_PROFILER_LOGGING

    DEFINES += ENABLE_FILE_SYSTEM_LOGGING
}


message(Qt version: $$[QT_VERSION])
