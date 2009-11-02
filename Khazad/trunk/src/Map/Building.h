#ifndef BUILDING_HEADER
#define BUILDING_HEADER

#include <stdafx.h>
#include <Actor.h>
#include <Map.h>


class Building : public Actor
{

public:

    Building(MapCoordinates Position, Uint8 Xlength, Uint8 Ylength, Sint16 MaterialID, Sint16 BuildingID);
    ~Building();

    bool Init();
    bool Draw();

protected:

    MapCoordinates MapCoords;
    CubeCoordinates CubeCoords;

    Uint8 LengthX, LengthY;

    Sint16 BuildingType;
    Sint16 MaterialType;
    Sint16 ModelType;

    GLuint Texture;
};

#endif // BUILDING_HEADER
