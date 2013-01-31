#ifndef AWQPROCESSEXTENSION_H
#define AWQPROCESSEXTENSION_H

#include <QObject>

class AWQProcessExtension : public QObject
{
    Q_OBJECT
public:
    explicit AWQProcessExtension(QObject *parent = 0);

    /*! By default QProcess won't allow to run a process elevated if the source process is not elevated (on Windows).

        This extension method uses a different WinAPI for process execution and allows elevated process starting (if the app has the required manifest values which tells Windows it has to be run as elevated).

        On Mac OS this is the same as QProcess::startDetached.
    */
    static bool startDetachedAndAllowElevation(const QString &programPath, const QStringList &arguments, const QString &workingDirectoryPath);
    
};

#endif // AWQPROCESSEXTENSION_H
