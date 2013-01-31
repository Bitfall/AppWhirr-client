#ifndef BACKGROUND_APPLICATIONRUNNER_H
#define BACKGROUND_APPLICATIONRUNNER_H

#include <QObject>
#include <QRunnable>
#include <QStringList>

class Background_ApplicationRunner : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit Background_ApplicationRunner(QString absoluteExecutablePath, QString absoluteWorkingDirectory, QString appName, QStringList appArgs = QStringList(), QObject *parent = 0);

    void run();

Q_SIGNALS:
    void successWithApp(QString appName);
    void failedWithError(QString appName, QString errorMessage);

#ifdef Q_OS_WIN
private:
    /*! This method uses the ShellExecute(Ex) WinAPI instead of Qt's QProcess's API

        This is required to run apps elevated under Windows.
    */
    void win_runAppWithShellExecute();
#endif

private:
    QString _absoluteExecutablePath;
    QString _absoluteWorkingDirectory;
    QString _appName;
    QStringList _appArgs;
};

#endif // BACKGROUND_APPLICATIONRUNNER_H
