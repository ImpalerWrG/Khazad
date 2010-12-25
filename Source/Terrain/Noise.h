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


/*------------------------NOISE---------------------------

Noise is an implementation of Perlin Simplex Noise in 3 dimensions
*/

#ifndef NOISE__HEADER
#define NOISE__HEADER

#include <stdafx.h>

#include <Coordinates.h>


class Noise
{
    float grad[12][3];

    int perm[512];

public:

    Noise();
    ~Noise();

	float dot(float x, float y, float z, float* g);

	float noise(float xin, float yin, float zin);

	float simplex_noise(int octaves, float x, float y, float z);

};

#endif // NOISE__HEADER
