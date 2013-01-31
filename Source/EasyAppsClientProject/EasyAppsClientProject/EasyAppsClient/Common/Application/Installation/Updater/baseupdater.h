#ifndef BASEUPDATER_H
#define BASEUPDATER_H

#include <QObject>

class BaseUpdater : public QObject
{
    Q_OBJECT
public:
    explicit BaseUpdater(QObject *parent = 0);

    /** Starts checking for updates.
        Only call it manually, when isAutomaticallyCheckingForUpdates() is false
      */
    void checkForUpdates();

    /** Sets the url where update information is handled on the server
      */
    void setURLForUpdateInformation(const QString& url);
    /** Gets url for update informations
      */
    QString urlForUpdateInformation();

    /** Sets update checking method
      */
    void setAutomaticallyCheckForUpdates(bool checkAutomatically);
    bool isAutomaticallyCheckingForUpdates();

    /** Sets the interval for checking for updates when isAutomaticallyCheckingForUpdates() is true
      */
    void setUpdateCheckInterval(int interval);
    int updateCheckInterval();

Q_SIGNALS:
    /** You're on the right ship!
      */
    void appIsUpToDate();

    /** It's time to download something fress
      */
    void newVersionIsAvailable();
public Q_SLOTS:

protected:
    /** Pure virtual function. Must be overwritten!
        @return Should return update information for requested file
      */
    virtual void updateInfoForFile(const QString& file) = 0;
};

#endif // BASEUPDATER_H
