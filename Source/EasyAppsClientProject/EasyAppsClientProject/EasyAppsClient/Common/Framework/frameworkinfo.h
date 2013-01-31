#ifndef FRAMEWORKINFO_H
#define FRAMEWORKINFO_H

#include <QString>

class FrameworkInfo
{
public:
    /*! Default constructor - required by Qt meta info system
    */
    explicit FrameworkInfo();
    explicit FrameworkInfo(QString frameworkId, QString frameworkPrintableName);

    QString getFrameworkId() const;
    QString getFrameworkPrintableName() const;
    
private:
    QString _frameworkId;
    QString _frameworkPrintableName;
};

#endif // FRAMEWORKINFO_H
