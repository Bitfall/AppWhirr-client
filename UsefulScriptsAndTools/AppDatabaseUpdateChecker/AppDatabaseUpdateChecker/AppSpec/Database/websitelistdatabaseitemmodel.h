#ifndef WEBSITELISTDATABASEITEMMODEL_H
#define WEBSITELISTDATABASEITEMMODEL_H

#include <QString>

class WebsiteListDatabaseItemModel
{
public:
    explicit WebsiteListDatabaseItemModel();
    explicit WebsiteListDatabaseItemModel(QString appname, QString url, QString downloadUrl, QString content, QString timestamp, QString timestampAsText, QString checkSectionBegin, QString checkSectionEnd, QString note);

//    WebsiteListDatabaseItemModel & operator=(const WebsiteListDatabaseItemModel &rhs) {
//        if(this == &rhs) {
//            return *this;
//        }

//        this->_appname = rhs._appname;
//        this->_url = rhs._url;
//        this->_content = rhs._content;
//        this->_timestamp = rhs._timestamp;
//        this->_timestampAsText = rhs._timestampAsText;

//        return *this;
//    }

    QString getAppName() const;
    QString getUrl() const;
    QString getDownloadUrl() const;
    QString getContent() const;
    QString getTimestamp() const;
    QString getTimestampAsText() const;
    QString getNote() const;
    QString getCheckSectionBegin() const;
    QString getCheckSectionEnd() const;

    void setContent(QString value);
    void setNote(QString value);
    void setCheckSectionBegin(QString value);
    void setCheckSectionEnd(QString value);

private:
    QString _appname;
    QString _url;
    QString _downloadUrl;
    QString _content;
    QString _timestamp;
    QString _timestampAsText;
    QString _note;
    QString _checkSectionBegin;
    QString _checkSectionEnd;
};

#endif // WEBSITELISTDATABASEITEMMODEL_H
