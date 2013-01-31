#ifndef GUICONFIGURATIONMANAGER_H
#define GUICONFIGURATIONMANAGER_H

#include <QObject>
#include <QMap>

#include <QSize>
#include <QColor>
#include <QMargins>

class GuiConfigurationManager : public QObject
{
    Q_OBJECT
public:
    explicit GuiConfigurationManager(QString configFilePath, QObject *parent = 0);

    //
    // returns and empty string if not found
    QString getStyleForId(QString configId);

    //
    // some utility, converter methods

    QSize getSizeForId(QString configId);
    int getIntForId(QString configId);
    QColor getColorForId(QString configId);
    QMargins getMarginsForId(QString configId);

private:
    void loadFromFile(QString filePath);
    QString _getConfigContentForKey(QString configKey);

private:
    QMap<QString, QString> _configMap;
    QMap<QString, QString> _placeholderMap;
};

#endif // GUICONFIGURATIONMANAGER_H
