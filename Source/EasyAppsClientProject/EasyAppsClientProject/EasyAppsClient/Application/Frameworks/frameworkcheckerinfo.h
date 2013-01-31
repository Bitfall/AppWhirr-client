#ifndef FRAMEWORKCHECKERINFO_H
#define FRAMEWORKCHECKERINFO_H

#include <QString>

/*! Framework-checker info model.

    The \a frameworkCheckerUpdateTimestamp is the timestamp of the checker it was last updated in the server.
*/
class FrameworkCheckerInfo
{
public:
    explicit FrameworkCheckerInfo();
    explicit FrameworkCheckerInfo(QString frameworkCheckerId, QString relatedFrameworkName, QString frameworkCheckerPath, QString frameworkCheckerUpdateTimestamp);

    QString getFrameworkCheckerId() const;
    void setFrameworkCheckerId(QString value);

    QString getRelatedFrameworkName() const;
    void setRelatedFrameworkName(QString value);

    QString getFrameworkCheckerPath() const;
    void setFrameworkCheckerPath(QString value);

    QString getFrameworkCheckerUpdateTimestamp() const;
    void setFrameworkCheckerUpdateTimestamp(QString value);

private:
    QString _frameworkCheckerId;
    QString _relatedFrameworkName;
    QString _frameworkCheckerPath;
    QString _frameworkCheckerUpdateTimestamp;
};

#endif // FRAMEWORKCHECKERINFO_H
