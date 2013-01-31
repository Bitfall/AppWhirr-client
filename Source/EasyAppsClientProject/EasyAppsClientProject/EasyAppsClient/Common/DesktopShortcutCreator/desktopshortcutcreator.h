#ifndef DESKTOPSHORTCUTCREATOR_H
#define DESKTOPSHORTCUTCREATOR_H

#include <QString>

class DesktopShortcutCreator
{
private:
    DesktopShortcutCreator();

public:

    /* Creates a shortcut / symlink

      @param pathToSaveShortcutFile : the shortcut / symlink will be saved to this path
      @param pathToShortcutTarget : the target of the shortcut - where it will point to
      @param iconPath : can be empty, in this case it won't set the icon location -> windows will automatically try to get an icon from the target pathToShortcutTarget file
      @param shortcutDescription : optional description of the shortcut - windows only
      @param isNormalizePathForCurrentPlatform : if \b true it will normalize the given \a pathToShortcutTarget \a pathToSaveShortcutFile and \a iconPath paths - will make it platform dependent e.g. replacing / to \ on Windows. If \b false the pathes will be used as provided
    */
    static bool createShortcut(QString pathToSaveShortcutFile, QString pathToShortcutTarget, QString iconPath, QString shortcutDescription, bool isNormalizePathesForCurrentPlatform);

};

#endif // DESKTOPSHORTCUTCREATOR_H
