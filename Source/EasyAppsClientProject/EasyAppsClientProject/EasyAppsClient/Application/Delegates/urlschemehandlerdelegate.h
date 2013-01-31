#ifndef URLSCHEMEHANDLERDELEGATE_H
#define URLSCHEMEHANDLERDELEGATE_H

#include <QObject>

/*! Url scheme handling delegate

    It can register itself for supported custom OS schemes. After the registration this app will get the registered scheme calls.
    Ex: registering the 'appwhirr://' scheme will result that after the registration the OS will delegate every 'appwhirr://' scheme-d url calls to this app.
*/
class UrlSchemeHandlerDelegate : public QObject
{
    Q_OBJECT
public:
    explicit UrlSchemeHandlerDelegate(QObject *parent = 0);

    void registerUrlSchemeHandler();
    void unregisterUrlSchemeHandler();

    //
    // the scheme prefix is the first part of the scheme. If the url is like: 'appwhirr://apps/15' then the prefix is 'appwhirr'
    static QString getSchemePrefix();
    //
    // if the url is 'appwhirr://runapp/3' then the fix-first-part is: 'appwhirr://'
    static QString getSchemeFixFirstPart();
    //
    static QString getShowAppDetailSchemeContentPart();
    static QString getRunAppSchemeContentPart();
    //
    static QString getSearchInStoreSchemeContentPart();
    //
    static QString createSchemeUrlForRunApp(QString appId);

Q_SIGNALS:
    void openAppDetailPageForAppId(QString appId);
    void runAppForAppId(QString appId);
    void searchInStore(QString searchFilterText);

public: // INTERNAL
    //
    // the platform specific implementation have to call this when it get's a scheme url
    //
    // [!] the url has to be the full url, containing the scheme-prefix as well -> example url: 'appwhirr://apps/15'
    void __schemeCalledWithURL(QString url);

private:

#ifdef Q_WS_MAC
    void setupMacEvents();
    void removeMacEvents();
#endif

#ifdef Q_WS_WIN
    void win_registerUrlSchemeHandler();
    void win_unregisterUrlSchemeHandler();

    bool win_RegisterCustomUrlSchemeToRegistry(QString urlScheme, QString targetAppPathToCall, QString iconPath);
#endif

private:
#ifdef Q_WS_MAC
    class MacEventProcessor;
    MacEventProcessor* macEventProcessor;
#endif
};

#endif // URLSCHEMEHANDLERDELEGATE_H
