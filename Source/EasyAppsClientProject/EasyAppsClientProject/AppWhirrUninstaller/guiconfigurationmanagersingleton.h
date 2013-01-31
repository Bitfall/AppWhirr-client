#ifndef GUICONFIGURATIONMANAGERSINGLETON_H
#define GUICONFIGURATIONMANAGERSINGLETON_H

#include "../EasyAppsClient/Common/guiconfigurationmanager.h"

class GuiConfigurationManagerSingleton
{
private:
    GuiConfigurationManagerSingleton();
public:
    static GuiConfigurationManager *sharedGuiConfigurationManager();
    static void setSharedGuiConfigurationManager(GuiConfigurationManager *guiConfManager);

private:
    static GuiConfigurationManager *_sharedObj;
};

#endif // GUICONFIGURATIONMANAGERSINGLETON_H
