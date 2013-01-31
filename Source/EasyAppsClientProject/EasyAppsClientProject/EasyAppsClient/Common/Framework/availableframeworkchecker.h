#ifndef AVAILABLEFRAMEWORKCHECKER_H
#define AVAILABLEFRAMEWORKCHECKER_H

#include <QObject>
#include <QList>

#include "frameworkcheckmodel.h"

/*! Available framework(s) checker

    Checks for specified frameworks - checks whether the framework(s) are installed and available in the current system.
*/
class AvailableFrameworkChecker : public QObject
{
    Q_OBJECT
public:
    explicit AvailableFrameworkChecker(QObject *parent = 0);

    void checkForFrameworksAsync(QList<FrameworkCheckModel> frameworkCheckModels);

Q_SIGNALS:
    void frameworksAvailable(QList<FrameworkCheckModel> frameworksFoundAsAvailable);
    void frameworksNotFound(QList<FrameworkCheckModel> frameworksNotFound);
    void finishedWithChecking();

private Q_SLOTS:
    void _checkerFinished();
};

#endif // AVAILABLEFRAMEWORKCHECKER_H
