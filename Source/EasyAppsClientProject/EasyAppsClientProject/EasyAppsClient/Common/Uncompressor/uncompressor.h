#ifndef UNCOMPRESSOR_H
#define UNCOMPRESSOR_H

#include <QString>
#include <QStringList>

class Uncompressor
{
private:
    Uncompressor();

public:
    /*! Uncompress the given zip to the target-output-directory

        @param isCleanupTargetOutputDirectoryBeforeUncompress : if true it will delete the target output directory before uncompress
        @param ignoreTheseRootDirectories : a list of root directory names which should be skipped (useful for special dirs like the mac __MACOSX dir
        @param isCopyOnlyContentsOfRootDir : if true only contents of the zip's root directory will be uncompressed. Example: the zip contains a root dir 'ziproot'. If isCopyOnlyContentsOfRootDir is true only the content of 'ziproot' will be uncompressed and 'ziproot' won't be added to the targetOutputDirectoryPath as a path component (will be ignored completely). !NOTE: don't use this for zips without a special root dir - in this case every file which is not in a directory won't be uncompressed!
        @param isApplyFilePermissions : if true file permissions stored in the zip will be applied on each file, otherwhise will set 0777.
    */
    static bool uncompress(QString targetZipFilePath, QString targetOutputDirectoryPath, bool isCleanupTargetOutputDirectoryBeforeUncompress, QStringList ignoreTheseRootDirectories = QStringList(), bool isCopyOnlyContentsOfRootDir = false, bool isApplyFilePermissions = false);
};

#endif // UNCOMPRESSOR_H
