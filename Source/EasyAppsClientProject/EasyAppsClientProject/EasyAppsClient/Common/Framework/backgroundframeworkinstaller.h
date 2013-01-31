#ifndef BACKGROUNDFRAMEWORKINSTALLER_H
#define BACKGROUNDFRAMEWORKINSTALLER_H

#include <QObject>

#include "frameworkcheckmodel.h"

/*! Framework installer designed to be used with QThread in a background thread

    Specify the required installer information in the constructor, then you can start the install with the \a startInstall() method (slot) and it will emit the result.

    @note When the \a finished() signal emitted the object is safe to release. This signal will be emitted in every case the installer finishes. When the \a finishedWith..() signals emitted it's NOT safe to relese the object, only when the \a finished() signal emitted!

    @note Important!\n
    The \a finishedWithError() signal will be emitted only if the installer cannot be started! It won't check the installer's returned value, because it's not reliable! Check the framework after the install again!
*/
class BackgroundFrameworkInstaller : public QObject
{
    Q_OBJECT
public:
    explicit BackgroundFrameworkInstaller(FrameworkCheckModel frameworkInstallModel);

public Q_SLOTS:
    void startFrameworkInstall();

Q_SIGNALS:
    void finishedWithSuccess(FrameworkCheckModel frameworkInstallModel);
    void finishedWitError(FrameworkCheckModel frameworkInstallModel, QString error);
    void finished();

private:
    void _emitFailedWithError(QString errorMessage);
    
private:
    FrameworkCheckModel _frameworkInstallModel;
};

#endif // BACKGROUNDFRAMEWORKINSTALLER_H
