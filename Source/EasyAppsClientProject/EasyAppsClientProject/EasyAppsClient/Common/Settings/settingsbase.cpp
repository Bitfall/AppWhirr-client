#include "settingsbase.h"

#include <QSettings>
#include <QSize>
#include <QRect>
#include <QString>
#include <QVariant>

SettingsBase::SettingsBase()
{
}

SettingsBase::~SettingsBase()
{
    delete qSettings;
}

// -------------------------------
// utility methods

void SettingsBase::setSettingsValue(const QString& key, const QVariant& value) const {
    qSettings->setValue(key, value);
}

QString SettingsBase::getQStringValue(const QString& key) const {
    return qSettings->value(key).toString();
}

bool SettingsBase::getBooleanValue(const QString& key) const {
    return qSettings->value(key).toBool();
}

int SettingsBase::getIntegerValue(const QString& key) const {
    return qSettings->value(key).toInt();
}

float SettingsBase::getFloatValue(const QString& key) const {
    return qSettings->value(key).toFloat();
}

QRect SettingsBase::getRectValue(const QString& key) const {
    return qSettings->value(key).toRect();
}

QSize SettingsBase::getSizeValue(const QString& key) const {
    return qSettings->value(key).toSize();
}

QVariant SettingsBase::getValue(const QString& key) const {
    return qSettings->value(key);
}

void SettingsBase::saveSettingsNow()
{
    qSettings->sync();
}
