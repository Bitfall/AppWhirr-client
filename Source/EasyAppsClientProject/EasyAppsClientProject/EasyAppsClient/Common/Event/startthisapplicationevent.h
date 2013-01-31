#ifndef STARTTHISAPPLICATIONEVENT_H
#define STARTTHISAPPLICATIONEVENT_H

#include <QEvent>
#include <QString>

class StartThisApplicationEvent: public QEvent
{
public:
    static inline Type getCustomEventType()
    {
        static Type sEventType = static_cast<QEvent::Type>(QEvent::registerEventType());
        return sEventType;
    }

    //
    // activeFilter: the filter which was active when this event created
    explicit StartThisApplicationEvent(QString applicationPath,
                                       QString applicationWorkingDirectory,
                                       QString applicationTitle,
                                       QString relatedFilter);

    QString getApplicationPath() const;
    QString getApplicationWorkingDirectory() const;
    QString getApplicationTitle() const;
    QString getRelatedFilter() const;

private:
    QString applicationPath;
    QString applicationWorkingDirectory;
    QString applicationTitle;
    QString relatedFilter;
};

#endif // STARTTHISAPPLICATIONEVENT_H
