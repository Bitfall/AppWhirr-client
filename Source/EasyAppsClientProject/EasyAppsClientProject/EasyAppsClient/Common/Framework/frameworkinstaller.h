#ifndef FRAMEWORKINSTALLER_H
#define FRAMEWORKINSTALLER_H

#include <QObject>

#include "frameworkcheckmodel.h"

class FrameworkInstaller : public QObject
{
    Q_OBJECT
public:
    explicit FrameworkInstaller(QObject *parent = 0);

    void startFrameworkInstallAsync(FrameworkCheckModel frameworkInstallModel);

Q_SIGNALS:
    void frameworkInstalled(FrameworkCheckModel frameworkInstallModel);
    void frameworkInstallFailed(FrameworkCheckModel frameworkInstallModel, QString errorMessage);

private Q_SLOTS:
    void _installFinished();
};

#endif // FRAMEWORKINSTALLER_H
