#ifndef WELCOMEVIEW_H
#define WELCOMEVIEW_H

#include <QWidget>
QT_BEGIN_NAMESPACE
class QPushButton;
QT_END_NAMESPACE
class CheckboxButton;

class WelcomeView : public QWidget
{
    Q_OBJECT
public:
    explicit WelcomeView(QWidget *parent = 0);

    void setIsRemoveAppWhirrAndLocalDataCheckboxChecked(bool value);
    void setIsRemoveInstalledAppsCheckboxChecked(bool value);

    void setStartUninstallButtonEnabled(bool value);
    
Q_SIGNALS:
    void isRemoveAppWhirrAndLocalDataCheckboxValueChanged(bool newValue);
    void isRemoveAppsAsWellCheckboxValueChanged(bool newValue);
    void startUninstall();
    
private:
    CheckboxButton *_removeAppWhirrAndLocalDataCheckboxButton;
    CheckboxButton *_removeAppsAsWellCheckboxButton;
    QPushButton *_startUninstallButton;
};

#endif // WELCOMEVIEW_H
