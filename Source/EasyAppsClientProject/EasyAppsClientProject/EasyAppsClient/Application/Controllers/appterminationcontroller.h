#ifndef APPTERMINATIONCONTROLLER_H
#define APPTERMINATIONCONTROLLER_H

#include <QObject>
#include <QQueue>
#include <QList>

QT_BEGIN_NAMESPACE
class QTimer;
QT_END_NAMESPACE

class PlatformspecificRunningExecutableDataInterface;


//
// information:
//  * this is a controller class, does the termination, but you have to connect to it's _presenter_Xx signals and slots a presenter -> connect only 1 presenter to these
//  * has interface signal/slots as well -> you can connect as many items to these as many you want
//
// usage:
//  * connect a presenter to the _resenter_Xx signals and slots (and of course add it to layout / present it somewhere then this controller tells it)
//  * then whenever you need a termination call terminateAppUnderDirectory()
//  * the result will be an interface signal
class AppTerminationController : public QObject
{
    Q_OBJECT
    Q_ENUMS(AppTerminationController::TerminationReasonCodeEnum)

public:
    enum TerminationReasonCodeEnum {
        TRC_TerminateForInstallOrUpdate = 0,
        TRC_TerminateForUninstall = 1
    };

private:
    class AppTerminationQueueItem {
    public:
        // don't use this constructor - needed for internal mechanism
        explicit AppTerminationQueueItem() {}

        explicit AppTerminationQueueItem(QString appId, QString appName, QString rootDirectoryToScanForRunningApps, AppTerminationController::TerminationReasonCodeEnum terminationReasonCode) : _appId(appId), _appName(appName), _rootDirectoryToScanForRunningApps(rootDirectoryToScanForRunningApps), _terminationReasonCode(terminationReasonCode)
        {}

        QString getAppId() const { return _appId; }
        QString getAppName() const { return _appName; }
        QString getRootDirectoryToScanForRunningApps() const { return _rootDirectoryToScanForRunningApps; }
        AppTerminationController::TerminationReasonCodeEnum getTerminationReasonCode() const { return _terminationReasonCode; }

    private:
        QString _appId;
        QString _appName;
        QString _rootDirectoryToScanForRunningApps;
        AppTerminationController::TerminationReasonCodeEnum _terminationReasonCode;
    };

public:
    explicit AppTerminationController(QObject *parent = 0);

    // -----------------
    // --- interface ---

    //
    // async method -> will add this item to termination queue and will emit related interface signals
    void terminateAppUnderDirectory(QString appId, QString appName, QString rootDirectoryToScanForRunningApps, AppTerminationController::TerminationReasonCodeEnum terminationReasonCode);

Q_SIGNALS:
    void applicationTerminatedSuccesfully(QString appId, QString appName);
    void cannotTerminateApplication(QString appId, QString appName);
    void nothingToTerminate(QString appId, QString appName);

    // -----------------
    // --- presenter ---

public Q_SLOTS: // these are the presenter UI connection slots -> the presenter have to be connected to these
    void _presenter_StartTermination();
    void _presenter_AbortTermination();

Q_SIGNALS: // presenter UI connection signals
    void _presenter_ShowWithInfo(QString appId, QString appName, AppTerminationController::TerminationReasonCodeEnum terminationReasonCode);
    void _presenter_Hide();


    // ----------------
    // --- internal ---
Q_SIGNALS:
    void __addThisRootDirectoryToQueueSignal(AppTerminationQueueItem queueItem);

private Q_SLOTS: // internal communication
    void _addThisRootDirectoryToQueueAndTryToStart(AppTerminationQueueItem queueItem);
    void _currentItemRunningExecutableCheckFinished(QList<PlatformspecificRunningExecutableDataInterface *> result);
    void _removeCurrentItemFromQueueAndTryStartTheNextOne();
    void _checkIfCurrentApplicationIsStillRunning();

private:
    void _tryNextScanAndTerminationFromQueue();
    void _terminateCurrentApplication();
    void _currentApplicationTerminated();
    void _currentApplicationTerminationFailed();

private:
    QQueue<AppTerminationQueueItem> _queueItems;
    QList<PlatformspecificRunningExecutableDataInterface *> _currRunningExecutableDataList;
    QTimer *_checkForRunningInstanceTimer;
    bool _isInTerminationProcess;
};

#endif // APPTERMINATIONCONTROLLER_H
