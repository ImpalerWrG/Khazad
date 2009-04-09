#include <ConfigManager.h>
#include <Singleton.h>
#include <XMLManager.h>

DECLARE_SINGLETON(ConfigManager)

ConfigManager::ConfigManager()
{

}

ConfigManager::~ConfigManager()
{

}

bool ConfigManager::Init()
{
	XML->loadFile("Assets\\XML\\GlobalDefines.xml");

	XResolution = 400;
	XML->getUIntValue("GlobalDefines", "ScreenResolution", "X", XResolution);
	YResolution = 300;
	XML->getUIntValue("GlobalDefines", "ScreenResolution", "Y", YResolution);
	XMap = 20;
	XML->getUIntValue("GlobalDefines", "MapSize", "X", XMap);
	YMap = 30;
	XML->getUIntValue("GlobalDefines", "MapSize", "Y", YMap);
	ZMap = 10;
	XML->getUIntValue("GlobalDefines", "MapSize", "Z", ZMap);
	CellEdgeLength = 10;
	XML->getUIntValue("GlobalDefines", "CellSize", "EdgeLength", CellEdgeLength);

	return true;
}
