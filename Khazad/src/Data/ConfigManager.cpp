#include <ConfigManager.h>
#include <Singleton.h>
#include <XMLManager.h>

DECLARE_SINGLETON(ConfigManager)

ConfigManager::ConfigManager()
{
	XResolution = 300;
	YResolution = 400;
	Full = 0;
	XMap = 10;
	YMap = 10;
	ZMap = 10;
	CellEdgeLength = 10;
}

ConfigManager::~ConfigManager()
{

}

bool ConfigManager::Init()
{
	XML->loadFile("Assets\\XML\\GlobalDefines.xml");

	XML->getUIntValue("GlobalDefines", "ScreenResolution", "X", XResolution);
	XML->getUIntValue("GlobalDefines", "ScreenResolution", "Y", YResolution);
	XML->getUIntValue("GlobalDefines", "ScreenResolution", "Full", Full);

	XML->getUIntValue("GlobalDefines", "MapSize", "X", XMap);
	XML->getUIntValue("GlobalDefines", "MapSize", "Y", YMap);
	XML->getUIntValue("GlobalDefines", "MapSize", "Z", ZMap);

	XML->getUIntValue("GlobalDefines", "CellSize", "EdgeLength", CellEdgeLength);

    XML->getUIntValue("GlobalDefines", "Zoom", "Min", ZoomMinimum);
	XML->getUIntValue("GlobalDefines", "Zoom", "Start", ZoomStartup);
	XML->getUIntValue("GlobalDefines", "Zoom", "Max", ZoomMaximum);

	return true;
}
