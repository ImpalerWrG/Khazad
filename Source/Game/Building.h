#ifndef BUILDING_HEADER
#define BUILDING_HEADER

#include <stdafx.h>

#include <Actor.h>
#include <Map.h>

class Vector3;

class Building : public Actor
{

public:

    Building(MapCoordinates Position, uint8_t Xlength, uint8_t Ylength, int16_t MaterialID, int16_t BuildingID);
    ~Building();

    bool Init();
    CoolDown Update();

    //bool Draw(CameraOrientation Orientaion);

protected:

    MapCoordinates MapCoords;
    CubeCoordinates CubeCoords;

    uint8_t LengthX, LengthY;

    int16_t BuildingType;
    int16_t MaterialType;
    int16_t ModelType;

    //GLuint Texture;
};

#endif // BUILDING_HEADER
