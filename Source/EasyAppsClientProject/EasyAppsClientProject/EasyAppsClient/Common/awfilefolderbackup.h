#ifndef AWFILEFOLDERBACKUP_H
#define AWFILEFOLDERBACKUP_H

#include <QObject>
#include <QFileInfo>
#include <QString>

class AWFileFolderBackup : public QObject
{
    Q_OBJECT
public:
    explicit AWFileFolderBackup(QFileInfo fileOrFolderInfo, QString backupRenamePrefix = QString("bak."), QObject *parent = 0);

    bool doBackup();
    bool rollbackBackup();
    bool deleteBackup();

private:
    QFileInfo _originalFileOrFolderInfo;
    QString _backupRenamePrefix;
};

#endif // AWFILEFOLDERBACKUP_H
