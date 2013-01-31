#ifndef PLAINERRORPRESENTERWIDGET_H
#define PLAINERRORPRESENTERWIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

class PlainErrorPresenterWidget : public QWidget
{
    Q_OBJECT

public:
    enum DefaultErrorMessagesEnum {
        DEM_ServerUnavailable = 1
    };

public:
    explicit PlainErrorPresenterWidget(QWidget *parent = 0);

    void setErrorMessage(QString errorMessage);
    void setErrorMessageByDefaultMessageEnum(DefaultErrorMessagesEnum defaultErrorMessage);

private:
    QLabel *_errorMessageLabel;
};

#endif // PLAINERRORPRESENTERWIDGET_H
