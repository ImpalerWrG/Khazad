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

    bool Init(uint32_t Seed);

    int16_t getRockTypeAtCoordinates(MapCoordinates Target);

private:

    std::vector< std::pair< Ogre::Plane, int16_t > > LayerBoundries;

    RandomNumberGenerator* RandGenerator;
};

#endif // GEOLOGY__HEADER
