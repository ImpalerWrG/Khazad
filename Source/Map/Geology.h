#ifndef GEOLOGY__HEADER
#define GEOLOGY__HEADER

#include <stdafx.h>

#include <Coordinates.h>

#include <OgrePlane.h>


class Geology
{

public:

    Geology();
    ~Geology();

    bool Init();

    int16_t getRockTypeAtCoordinates(MapCoordinates Target);

private:

    std::vector< std::pair< Ogre::Plane, int16_t > > LayerBoundries;

};

#endif // GEOLOGY__HEADER
