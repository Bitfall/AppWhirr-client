#ifndef SETTINGSBASE_H
#define SETTINGSBASE_H

#include <QObject>

QT_BEGIN_NAMESPACE
class QSettings;
class QSize;
class QRect;
class QString;
class QVariant;
QT_END_NAMESPACE

class SettingsBase {

public:
    void saveSettingsNow();

protected:
    SettingsBase();
    virtual ~SettingsBase();

    virtual void _initSettings() = 0;
    virtual void _setupDefaults() = 0;

    // -------------------------------
    // utility methods

    // const because this class doesn't hold any settings, only passes it
    // to QSettings
    void setSettingsValue(const QString& key, const QVariant& value) const;

    QString getQStringValue(const QString& key) const;
    bool getBooleanValue(const QString& key) const;
    int getIntegerValue(const QString& key) const;
    float getFloatValue(const QString& key) const;
    QRect getRectValue(const QString& key) const;
    QSize getSizeValue(const QString& key) const;
    QVariant getValue(const QString& key) const;

    QSettings*          qSettings;
};

#endif // SETTINGSBASE_H
