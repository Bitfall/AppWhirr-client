#include "guiconfigurationmanagersingleton.h"



GuiConfigurationManager *GuiConfigurationManagerSingleton::_sharedObj = 0;

GuiConfigurationManagerSingleton::GuiConfigurationManagerSingleton()
{
}

GuiConfigurationManager *GuiConfigurationManagerSingleton::sharedGuiConfigurationManager()
{
    return _sharedObj;
}

void GuiConfigurationManagerSingleton::setSharedGuiConfigurationManager(GuiConfigurationManager *guiConfManager)
{
    _sharedObj = guiConfManager;
}
