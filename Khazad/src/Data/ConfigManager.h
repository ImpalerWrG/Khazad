#ifndef CONFIG__HEADER
#define CONFIG__HEADER

#include <stdafx.h>
#include <Singleton.h>

class ConfigManager
{
DECLARE_SINGLETON_CLASS(ConfigManager)

public:

	~ConfigManager();
	bool Init();

	Uint16 getXResolution()     { return XResolution; }
	Uint16 getYResolution()     { return YResolution; }
	Uint16 FullScreen()         { return Full; }

	Uint16 getXMap()            { return XMap; }
	Uint16 getYMap()            { return YMap; }
	Uint16 getZMap()            { return ZMap; }

	Uint16 getCellEdgeLength()  { return CellEdgeLength; }

	Uint16 ZoomMin()            { return ZoomMinimum; }
	Uint16 ZoomStart()          { return ZoomStartup; }
	Uint16 ZoomMax()            { return ZoomMaximum; }

    Uint16 OrbitSpeed()         { return Orbit; }
    Uint16 TiltSpeed()          { return Tilt; }
    Uint16 SlideSpeed()         { return Slide; }
    Uint16 ZoomSpeed()          { return Zoom; }

    char* LoadPath()            { return Loading; }
    char* SavePath()            { return Saveing; }

protected:

	Uint16 XResolution;
	Uint16 YResolution;
    Uint16 Full;

	Uint16 XMap;
	Uint16 YMap;
	Uint16 ZMap;

	Uint16 CellEdgeLength;

	Uint16 ZoomMinimum;
    Uint16 ZoomStartup;
	Uint16 ZoomMaximum;

    Uint16 Orbit;
    Uint16 Tilt;
    Uint16 Slide;
    Uint16 Zoom;

    char* Loading;
    char* Saveing;
};

#define CONFIG (ConfigManager::GetInstance())

#endif // CONFIG__HEADER
