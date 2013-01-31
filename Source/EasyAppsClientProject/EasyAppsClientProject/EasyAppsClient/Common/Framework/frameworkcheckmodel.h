#ifndef FRAMEWORKCHECKMODEL_H
#define FRAMEWORKCHECKMODEL_H

#include <QString>
#include <QStringList>

#include "../Framework/frameworkinfo.h"

class FrameworkCheckModel
{
public:
    /*! Default constructor - required by Qt meta object system
    */
    explicit FrameworkCheckModel();

    /*! Constructor

        @param frameworkId : internal Id of the framework
        @param frameworkName : the presentable frameworkName
        @param frameworkCheckerAppPath : path for the checker app
        @param frameworkResultFileDirPath : path of the \b directory will contain the result-file
        @param frameworkInstallerPath : path of the framework-installer
        @param optionalFrameworkCheckerArgs : will be passed to the checker as additional args
    */
    explicit FrameworkCheckModel(FrameworkInfo frameworkInfo, QString frameworkCheckerAppPath, QString frameworkCheckerUpdateTimestamp, QString frameworkResultFileDirPath, QString frameworkInstallerPath, QStringList optionalFrameworkCheckerArgs);

    FrameworkInfo getFrameworkInfo() const;
    QString getFrameworkCheckerAppPath() const;
    QString getFrameworkCheckerUpdateTimestamp() const;
    QString getFrameworkResultFileDirPath() const;
    QString getFrameworkInstallerPath() const;
    QStringList getOptionalFrameworkCheckerArgs() const;

    void _setFrameworkCheckerAppPath(QString value);
    void _setFrameworkCheckerUpdateTimestamp(QString value);
    void _setFrameworkInstallerPath(QString value);

    /*! Sets the optional framework checker args to the given list

        @note !!! This will overwrite the previous args !!!
        If you want to add a new arg use the \a addOptionalFrameworkCheckerArg() method
    */
    void replaceOptionalFrameworkCheckerArg(QStringList args);

    /*! Adds the given arg to the optional fw checker args

        @note It won't replace any previously added arg, will only append it to the list.
        If you want to overwrite previously added args you have to call the \a replaceOptionalFrameworkCheckerArg() method
    */
    void addOptionalFrameworkCheckerArg(QString arg);
    void addOptionalFrameworkCheckerArgs(QStringList argsToAdd);

    /*! Will remove the previously added args
    */
    void clearOptionalFrameworkCheckerArgs();

private:
    FrameworkInfo _frameworkInfo;
    QString _frameworkCheckerAppPath;
    QString _frameworkCheckerUpdateTimestamp;
    QString _frameworkResultFileDirPath;
    QString _frameworkInstallerPath;
    QStringList _optionalFrameworkCheckerArgs;
};

#endif // FRAMEWORKCHECKMODEL_H
