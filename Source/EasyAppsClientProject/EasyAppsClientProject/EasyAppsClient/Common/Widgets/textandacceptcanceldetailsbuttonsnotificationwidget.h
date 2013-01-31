#ifndef TEXTANDACCEPTCANCELDETAILSBUTTONSNOTIFICATIONWIDGET_H
#define TEXTANDACCEPTCANCELDETAILSBUTTONSNOTIFICATIONWIDGET_H

#include "Browser/Notification/selftimeoutingnotificationwidgetbase.h"

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

class TextAndAcceptCancelDetailsButtonsNotificationWidget : public SelfTimeoutingNotificationWidgetBase
{
    Q_OBJECT
public:
    explicit TextAndAcceptCancelDetailsButtonsNotificationWidget(QString titleText, QString acceptButtonText, QString cancelButtonText, QString detailsButtonText, bool isFinishNotificationWhenDetailsSelected, bool isRequestImmediateUpdateWhenAcceptCancelOrDetailesFired, QWidget *parent = 0);

Q_SIGNALS:
    void acceptOptionSelected();
    void cancelOptionSelected();
    void detailsOptionSelected();

private Q_SLOTS:
    void _acceptButtonClicked();
    void _cancelButtonClicked();
    void _detailsButtonClicked();

protected:
    void resizeEvent(QResizeEvent *e);

private:
    bool _isFinishNotificationWhenDetailsSelected;
    bool _isRequestImmediateUpdateWhenAcceptCancelOrDetailesFired;
};

#endif // TEXTANDACCEPTCANCELDETAILSBUTTONSNOTIFICATIONWIDGET_H
