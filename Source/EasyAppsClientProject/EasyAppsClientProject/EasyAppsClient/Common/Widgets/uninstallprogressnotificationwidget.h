#ifndef UNINSTALLPROGRESSNOTIFICATIONWIDGET_H
#define UNINSTALLPROGRESSNOTIFICATIONWIDGET_H

#include "Browser/Notification/selftimeoutingnotificationwidgetbase.h"

QT_BEGIN_NAMESPACE
class QLabel;
class QTimer;
QT_END_NAMESPACE

class UninstallProgressNotificationWidget : public SelfTimeoutingNotificationWidgetBase
{
    Q_OBJECT

private:
    class StateInformationModel {
    public:
        explicit StateInformationModel() {}
        explicit StateInformationModel(QString titleText, QString styleSheet) : titleText(titleText), styleSheet(styleSheet) {}

        QString getTitleText() const { return titleText; }
        QString getStyleSheet() const { return styleSheet; }

    private:
        QString titleText;
        QString styleSheet;
    };

public:
    explicit UninstallProgressNotificationWidget(QString appId, QString appName, QWidget *parent = 0);

public Q_SLOTS:
    void setProgressStateMessage(QString progressStateMessage);
    void setResolutionNeeded();
    void setUninstallSucceeded();
    void setUninstallFailedWithError(QString errorMessage);

private Q_SLOTS:
    void _finishTimerTimeout();
    void _doStateUpdate();

private:
    void _setNewState(QString title, QString styleSheet);

private:
    QLabel *_statusLabel;
    QTimer *_finishedTimer;

    QString _appId;
    QString _appName;

    bool _isStateChanged;
    StateInformationModel _currentStateInformation;
    QTimer *_stateUpdateTimer;
};

#endif // UNINSTALLPROGRESSNOTIFICATIONWIDGET_H
