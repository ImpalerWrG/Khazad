#ifndef CONFIG__HEADER
#define CONFIG__HEADER

#include <stdafx.h>
#include <Singleton.h>
#include <Paths.h>

class ConfigManager
{
DECLARE_SINGLETON_CLASS(ConfigManager)

public:

	~ConfigManager();
	bool Init();

	int16_t getXResolution()     { return XResolution; }
	int16_t getYResolution()     { return YResolution; }
	int16_t FullScreen()         { return Full; }

	int16_t getXMap()            { return XMap; }
	int16_t getYMap()            { return YMap; }
	int16_t getZMap()            { return ZMap; }

	//int16_t getCellEdgeLength()  { return CellEdgeLength; }

	int16_t ZoomMin()            { return ZoomMinimum; }
	int16_t ZoomStart()          { return ZoomStartup; }
	int16_t ZoomMax()            { return ZoomMaximum; }

    int16_t OrbitSpeed()         { return Orbit; }
    int16_t TiltSpeed()          { return Tilt; }
    int16_t SlideSpeed()         { return Slide; }
    int16_t ZoomSpeed()          { return Zoom; }

    const char* LoadPath()            { return Loading.c_str(); }
    const char* SavePath()            { return Saveing.c_str(); }

protected:

	int16_t XResolution;
	int16_t YResolution;
    int16_t Full;

	int16_t XMap;
	int16_t YMap;
	int16_t ZMap;

	int16_t CellEdgeLength;

	int16_t ZoomMinimum;
    int16_t ZoomStartup;
	int16_t ZoomMaximum;

    int16_t Orbit;
    int16_t Tilt;
    int16_t Slide;
    int16_t Zoom;

    string Loading;
    string Saveing;
};

#define CONFIG (ConfigManager::GetInstance())

#endif // CONFIG__HEADER
