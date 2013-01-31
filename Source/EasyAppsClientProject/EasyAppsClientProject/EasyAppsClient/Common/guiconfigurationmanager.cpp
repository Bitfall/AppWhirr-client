#include "guiconfigurationmanager.h"

#include <QFile>
#include <QTextStream>

#include <QVariant>
#include <QDataStream>
#include <QStringBuilder>

// config
#define ID_SPECIFIER_LINE_PREFIX        QString("@|")
#define COMMENT_LINE_PREFIX             QString("//")
#define ID_CONTENT_COPY_LINE_PREFIX     QString(".@|")
#define PLACEHOLDER_DEFINITION_LINE_PREFIX  QString("@")

GuiConfigurationManager::GuiConfigurationManager(QString configFilePath, QObject *parent) :
    QObject(parent)
{
    this->loadFromFile(configFilePath);
}

void GuiConfigurationManager::loadFromFile(QString filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        WLog("Cannot open the given file: ") << filePath;
        return;
    }

    //
    // read and process
    QTextStream in(&file);
    QString textBuffer;
    QString currentItemId;
    while (!in.atEnd()) {
        QString line = in.readLine().simplified();

        bool isLineShouldBeProcessedAsContent = false;
        if(line.isEmpty()) {
            // # skip
            continue;
        }
        else if(line.startsWith(COMMENT_LINE_PREFIX)) {
            // # comment, skip
            continue;
        }
        else if(line.startsWith(ID_SPECIFIER_LINE_PREFIX)) {
            // # id specifier line
            int idEndIdx = line.indexOf("|", ID_SPECIFIER_LINE_PREFIX.length());
            if(idEndIdx < 0) {
                WLog(QString("Invalid id line found. Starts with the required prefix (%1), but id end indicator not found").arg(ID_SPECIFIER_LINE_PREFIX));
                continue;
            }
            QString foundItemId = line.mid(ID_SPECIFIER_LINE_PREFIX.length(), idEndIdx - ID_SPECIFIER_LINE_PREFIX.length());
//            DLog("Id found: ") << foundItemId;
            if(foundItemId.isEmpty()) {
                WLog("Id found, but it's empty. Cannot do anything with it.");
                continue;
            }

            if(!currentItemId.isEmpty() && !textBuffer.isEmpty()) {
                // store the previous one
                _configMap[currentItemId] = textBuffer;
//                DLog("Stored item: ") << currentItemId << textBuffer;
            }
            // clear previous item's data
            currentItemId = foundItemId;
            textBuffer.clear();
        }
        else if(line.startsWith(ID_CONTENT_COPY_LINE_PREFIX)) {
            // # copy identifier
//            DLog("Copy identifier.");

            // # id specifier line
            int idEndIdx = line.indexOf("|", ID_CONTENT_COPY_LINE_PREFIX.length());
            if(idEndIdx < 0) {
                WLog(QString("Invalid copy-id line found. Starts with the required prefix (%1), but copy-id end indicator not found").arg(ID_CONTENT_COPY_LINE_PREFIX));
                continue;
            }
            QString foundItemId = line.mid(ID_CONTENT_COPY_LINE_PREFIX.length(), idEndIdx - ID_CONTENT_COPY_LINE_PREFIX.length());
//            DLog("Copy-Id found: ") << foundItemId;
            if(foundItemId.isEmpty()) {
                WLog("Copy-Id found, but it's empty. Cannot do anything with it.");
                continue;
            }

            if(_configMap.contains(foundItemId)) {
//                DLog("original id found - copy it's content");
                textBuffer += _configMap.value(foundItemId);
            }
            else {
                WLog("Original ID not found - won't copy anything.");
            }
        }
        else if(line.startsWith(PLACEHOLDER_DEFINITION_LINE_PREFIX)) {
            // # placeholder definition
//            DLog("Placeholder definition.");

            int partEndIdx = line.indexOf("=", PLACEHOLDER_DEFINITION_LINE_PREFIX.length());
            if(partEndIdx < 0) {
//                WLog(QString("Invalid placeholder-definition line found. Starts with the required prefix (%1), but end indicator not found").arg(PLACEHOLDER_DEFINITION_LINE_PREFIX));
//                continue;
                DLog("Line starts with placeholder-definition prefix but '=' not found - will be processed as a content-line.");
                isLineShouldBeProcessedAsContent = true;
            }

            else {
                QString foundPlaceholderKey = "@" + line.mid(PLACEHOLDER_DEFINITION_LINE_PREFIX.length(), partEndIdx - PLACEHOLDER_DEFINITION_LINE_PREFIX.length()).trimmed();
                if(foundPlaceholderKey.isEmpty()) {
                    WLog("Placeholder-key found, but it's empty. Cannot do anything with it.");
                    continue;
                }
//              DLog("Placeholder-key found: ") << foundPlaceholderKey;

                QString foundPlaceholderValue = line.mid(partEndIdx+1).trimmed();
//              DLog("Placeholder-value found: ") << foundPlaceholderValue;
                _placeholderMap[foundPlaceholderKey] = foundPlaceholderValue;
            }
        }
        else {
            // # content
            isLineShouldBeProcessedAsContent = true;
        }

        if(isLineShouldBeProcessedAsContent)
        {
            QString processedLine = line;

            // check for placeholders
            QMapIterator<QString, QString> i(_placeholderMap);
            while(i.hasNext()) {
                i.next();
                processedLine.replace(i.key(), i.value());
            }

            textBuffer += processedLine;
        }
    }

    // store the last item as well
    if(!currentItemId.isEmpty() && !textBuffer.isEmpty()) {
        // store the last one
        _configMap[currentItemId] = textBuffer;
//        DLog("Stored item: ") << currentItemId << textBuffer;
    }
}

QString GuiConfigurationManager::_getConfigContentForKey(QString configKey)
{
    if(_configMap.contains(configKey)) {
        return _configMap.value(configKey);
    }
    else {
        WLog("GuiConf ID/key not found: ") << configKey;
    }

    return QString();
}

QString GuiConfigurationManager::getStyleForId(QString configId)
{
    return this->_getConfigContentForKey(configId);
}

QSize GuiConfigurationManager::getSizeForId(QString configId)
{
    QString contentForId = this->_getConfigContentForKey(configId);
    if(contentForId.isEmpty()) {
        WLog("Content not found for id: ") << configId;
        return QSize();
    }

//    QSize testSize(200, 300);
//    QColor testColor(200, 100, 100);

//    QVariant tmpTest(testColor);


//    QString stringVersion;
//    QTextStream testTextStream;

//    DLog(tmpTest.convert(QVariant::String));
////    testTextStream << testSize;
////    stringVersion << testTextStream;
////    stringVersion << testColor;
//    DLog("testtest: ") << tmpTest << testColor.name();

//    QVariant tmpVariantForConvert(contentForId);
//    return tmpVariantForConvert.toSize();

    contentForId = contentForId.simplified();
    QStringList parts = contentForId.split("x");
    if(parts.size() != 2) {
        WLog("Invalid size format. Exactly 1 'x' separator character required.");
        return QSize();
    }

    return QSize(parts[0].toInt(), parts[1].toInt());
}

int GuiConfigurationManager::getIntForId(QString configId)
{
    QString contentForId = this->_getConfigContentForKey(configId);
    if(contentForId.isEmpty()) {
        WLog("Content not found for id: ") << configId;
        return 0;
    }

    return contentForId.toInt();
}

QColor GuiConfigurationManager::getColorForId(QString configId)
{
    QString contentForId = this->_getConfigContentForKey(configId);
    if(contentForId.isEmpty()) {
        WLog("Content not found for id: ") << configId;
        return QColor();
    }

    return QColor(contentForId);
}

QMargins GuiConfigurationManager::getMarginsForId(QString configId)
{
    QString contentForId = this->_getConfigContentForKey(configId);
    if(contentForId.isEmpty()) {
        WLog("Content not found for id: ") << configId;
        return QMargins();
    }

    contentForId = contentForId.simplified();
    QStringList parts = contentForId.split(";", QString::SkipEmptyParts);
    if(parts.size() != 4) {
        WLog("Invalid Margins format. Exactly 4 non-empty ';' separated parts required. Example: 5;5;5;5");
        return QMargins();
    }

    return QMargins(parts[0].toInt(), parts[1].toInt(), parts[2].toInt(), parts[3].toInt());
}
