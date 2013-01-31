#include "websitelistdatabaseitemmodel.h"

WebsiteListDatabaseItemModel::WebsiteListDatabaseItemModel()
{

}

WebsiteListDatabaseItemModel::WebsiteListDatabaseItemModel(QString appname, QString url, QString downloadUrl, QString content, QString timestamp, QString timestampAsText, QString checkSectionBegin, QString checkSectionEnd, QString note) :
    _appname(appname), _url(url), _downloadUrl(downloadUrl), _content(content), _timestamp(timestamp), _timestampAsText(timestampAsText), _note(note), _checkSectionBegin(checkSectionBegin), _checkSectionEnd(checkSectionEnd)
{}

QString WebsiteListDatabaseItemModel::getAppName() const {
    return this->_appname;
}

QString WebsiteListDatabaseItemModel::getUrl() const {
    return this->_url;
}

QString WebsiteListDatabaseItemModel::getDownloadUrl() const {
    return this->_downloadUrl;
}

QString WebsiteListDatabaseItemModel::getContent() const {
    return this->_content;
}

QString WebsiteListDatabaseItemModel::getTimestamp() const {
    return this->_timestamp;
}

QString WebsiteListDatabaseItemModel::getTimestampAsText() const {
    return this->_timestampAsText;
}

QString WebsiteListDatabaseItemModel::getNote() const {
    return this->_note;
}

QString WebsiteListDatabaseItemModel::getCheckSectionBegin() const {
    return this->_checkSectionBegin;
}

QString WebsiteListDatabaseItemModel::getCheckSectionEnd() const {
    return this->_checkSectionEnd;
}

//
// --- setters ---

void WebsiteListDatabaseItemModel::setContent(QString value) {
    this->_content = value;
}

void WebsiteListDatabaseItemModel::setNote(QString value) {
    this->_note = value;
}

void WebsiteListDatabaseItemModel::setCheckSectionBegin(QString value) {
    this->_checkSectionBegin = value;
}

void WebsiteListDatabaseItemModel::setCheckSectionEnd(QString value) {
    this->_checkSectionEnd = value;
}
