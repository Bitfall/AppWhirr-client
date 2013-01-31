#ifndef BACKGROUNDFRAMEWORKCHECK_H
#define BACKGROUNDFRAMEWORKCHECK_H

#include <QObject>
#include <QList>

#include "frameworkcheckmodel.h"

/*! Framework checker designed to be used with QThread in a background thread

    You can start the checking with the \a startChecking() method and it will emit the results. First it will emit the \a frameworksAvailable() and the \a frameworksNotFound() signals, then the \a finishedWithChecking() signal.

    @note When (and only when!) the \a finishedWithChecking() signal emitted the object is safe to release.

    @note Return codes for framework-checkers (the framework checker apps \b have to return these codes):
    * 0: success
    * 1: not enough parameter
    * 2: required parameter not found
    * 3: the given 'output result file' is not writeable
*/
class BackgroundFrameworkCheck : public QObject
{
    Q_OBJECT
public:
    explicit BackgroundFrameworkCheck(QList<FrameworkCheckModel> frameworkModelsForCheck, QObject *parent=0);

public Q_SLOTS:
    void startChecking();

Q_SIGNALS:
    void frameworksAvailable(QList<FrameworkCheckModel> frameworksFoundAsAvailable);
    void frameworksNotFound(QList<FrameworkCheckModel> frameworksNotFound);
    void finishedWithChecking();

private:
    bool isThisFrameworkAvailable(FrameworkCheckModel frameworkCheckModel);

private:
    QList<FrameworkCheckModel> _frameworkModelsForCheck;
};

#endif // BACKGROUNDFRAMEWORKCHECK_H
