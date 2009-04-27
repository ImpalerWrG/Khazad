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
	CellEdgeLength = 16;

	Orbit = 100;
	Tilt = 100;
	Slide = 100;
	Zoom = 100;
}

ConfigManager::~ConfigManager()
{

}

bool ConfigManager::Init()
{
	TiXmlDocument* Document = XML->loadFile("Assets\\XML\\GlobalDefines.xml");
    TiXmlElement* Root = Document->RootElement();

	XML->QueryUIntValue(Root, "ScreenResolution", "X", XResolution);
	XML->QueryUIntValue(Root, "ScreenResolution", "Y", YResolution);
	XML->QueryUIntValue(Root, "ScreenResolution", "Full", Full);

	XML->QueryUIntValue(Root, "MapSize", "X", XMap);
	XML->QueryUIntValue(Root, "MapSize", "Y", YMap);
	XML->QueryUIntValue(Root, "MapSize", "Z", ZMap);

	XML->QueryUIntValue(Root, "CellSize", "EdgeLength", CellEdgeLength);

    XML->QueryUIntValue(Root, "Zoom", "Min", ZoomMinimum);
	XML->QueryUIntValue(Root, "Zoom", "Start", ZoomStartup);
	XML->QueryUIntValue(Root, "Zoom", "Max", ZoomMaximum);

    XML->QueryUIntValue(Root, "InterfaceSpeed", "Orbit", Orbit);
    XML->QueryUIntValue(Root, "InterfaceSpeed", "Tilt", Tilt);
    XML->QueryUIntValue(Root, "InterfaceSpeed", "Slide", Slide);
    XML->QueryUIntValue(Root, "InterfaceSpeed", "Zoom", Zoom);

    XML->QueryTextValue(Root, "MapLoading", "Path", Loading);
    XML->QueryTextValue(Root, "MapSaving", "Path", Saveing);

	return true;
}
