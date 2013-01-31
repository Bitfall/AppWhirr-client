#ifndef INSTALLPROGRESSNOTIFICATIONWIDGET_H
#define INSTALLPROGRESSNOTIFICATIONWIDGET_H

#include "Browser/Notification/selftimeoutingnotificationwidgetbase.h"

QT_BEGIN_NAMESPACE
class QLabel;
class QTimer;
class QProgressBar;
QT_END_NAMESPACE

class InstallProgressNotificationWidget : public SelfTimeoutingNotificationWidgetBase
{
    Q_OBJECT

private:
    class InstallStateInformationModel {
    public:
        explicit InstallStateInformationModel() {}
        explicit InstallStateInformationModel(QString titleText, QString styleSheet, bool isShowProgressBar, int progressBarMin, int progressBarMax, int progressBarValue) : titleText(titleText), styleSheet(styleSheet), _isShowProgressBar(isShowProgressBar), _progressBarMin(progressBarMin), _progressBarMax(progressBarMax), _progressBarValue(progressBarValue) {}

        QString getTitleText() const { return titleText; }
        QString getStyleSheet() const { return styleSheet; }
        bool getIsShowProgressBar() const { return _isShowProgressBar; }
        int getProgressBarMin() const { return _progressBarMin; }
        int getProgressBarMax() const { return _progressBarMax; }
        int getProgressBarValue() const { return _progressBarValue; }

    private:
        QString titleText;
        QString styleSheet;
        bool _isShowProgressBar;
        int _progressBarMin;
        int _progressBarMax;
        int _progressBarValue;
    };

public:
    explicit InstallProgressNotificationWidget(QString appId, QString appName, QWidget *parent = 0);

public Q_SLOTS:
    void setGrabbed();
    void setDownloading();
    void setDownloadProgress(int progress);
    void setInstallStarted();
    void setInstalled();
    void setInstallProgressStateMessage(QString installProgressStateMessage);

    void setGrabFailedWithError(QString errorMessage);
    void setInstallFailedWithError(QString errorMessage);

private Q_SLOTS:
    void _finishTimerTimeout();
    void _doStateUpdate();

private:
    void _setNewState(QString title, QString styleSheet, bool isShowProgressBar, int progressBarMin, int progressBarMax, int progressBarValue);

private:
    // ui
    QLabel *_statusLabel;
    QProgressBar *_downloadProgressBar;

    QTimer *_finishedTimer;

    QString _appId;
    QString _appName;

    bool _isStateChanged;
    InstallStateInformationModel _currentStateInformation;
    QTimer *_stateUpdateTimer;
};

#endif // INSTALLPROGRESSNOTIFICATIONWIDGET_H
