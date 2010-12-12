/* Copyright 2010 Kenneth Ferland

This file is part of Khazad.

Khazad is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Khazad is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Khazad.  If not, see <http://www.gnu.org/licenses/> */


/*------------------------GEOLOGY---------------------------

Geology is responsible for creating and Holding data about the
still un-initialized parts of the map, both deep underground and
beyond the embark area and then making that data available on request
through efficient run-time procedural generation.

Generation is done at the Cell level by first calling for Generation of
the X and Y Cell coordinate and then querrying the individuals blocks.

All data must be generated from the Master Seed in combination with
Coordinate data to ensure reproducability of the map.*/

// TODO

#ifndef GEOLOGY__HEADER
#define GEOLOGY__HEADER

#include <stdafx.h>

#include <Coordinates.h>

#include <OgrePlane.h>
#include <Random.h>

class Geology
{

public:

    Geology();
    ~Geology();

    bool Init(int32_t Seed);

    void SetArea(int32_t X, int32_t Y);

    void Generate();
    void GenerateCellHeight(int32_t X, int32_t Y, float heightScale = 1.0, float Roughness = 0.5);

    int16_t getRockTypeAtCoordinates(MapCoordinates Target);

    int16_t getCellBottomZLevel()  { return CellBottomZ; }
    int16_t getCellTopZLevel()     { return CellTopZ; }

private:

    int32_t MasterSeed;

    int32_t MapSizeX;
    int32_t MapSizeY;

    int16_t CellTopZ;
    int16_t CellBottomZ;

    float Height[CELLEDGESIZE + 1][CELLEDGESIZE + 1];
    bool Seeded[CELLEDGESIZE + 1][CELLEDGESIZE + 1];

    std::vector< std::pair< Ogre::Plane, int16_t > > LayerBoundries;

    RandomNumberGenerator* RandGenerator;
};

#endif // GEOLOGY__HEADER
