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


#include <Geology.h>

#include <DataManager.h>
#include <Random.h>
#include <Coordinates.h>
#include <math.h>

#include <boost/functional/hash.hpp>


Geology::Geology()
{

}

Geology::~Geology()
{

}

bool Geology::Init(int32_t Seed)
{
    Ogre::Plane RockLayer = Ogre::Plane(Ogre::Vector3(0, 0, 1), 1);
    int RockType = 42;

    MasterSeed = Seed;

    RandGenerator = new RandomNumberGenerator();
    RandGenerator->Init();

    LayerBoundries.push_back(make_pair(RockLayer, RockType));

    return true;
}

void Geology::SetArea(int32_t X, int32_t Y)
{
    MapSizeX = X;
    MapSizeY = Y;
}

void Geology::Generate()
{

}

void Geology::GenerateCellHeight(int32_t X, int32_t Y, float heightScale, float Roughness)
{
    //float** Height = float[CELLEDGESIZE + 1][CELLEDGESIZE + 1];
    //bool** Seeded = bool[CELLEDGESIZE + 1][CELLEDGESIZE + 1];
    uint8_t size = CELLEDGESIZE + 1;

    // Mark edges as Seeded to prevent them from being overwritten
    for (uint16_t x = 0; x < size; x++)
    {
        for (uint16_t y = 0; y < size; y++)
        {
            Seeded[x][y] = false;
            Height[x][y] = 0.0f;
        }
    }

    // Edges Initialized to anchor values to create contiguousness
    for (uint16_t x = 0; x < size; x++)
    {
        Seeded[x][0] = true;
        Seeded[x][CELLEDGESIZE] = true;
    }
    for (uint16_t y = 0; y < size; y++)
    {
        Seeded[0][y] = true;
        Seeded[CELLEDGESIZE][y] = true;
    }

    /* initialize random number generator */

    boost::hash< std::pair< int32_t, int32_t > > CoordinateHash;
    int32_t Hash = CoordinateHash(std::make_pair(X, Y));

    RandGenerator->Seed(MasterSeed);
    int32_t FinalSeed = RandGenerator->Roll(0, 0x7fffffff) ^ Hash;

    RandGenerator->Seed(FinalSeed);


	/* Set up our roughness constants.
	   Random numbers are always generated in the range 0.0 to 1.0.
	   'scale' is multiplied by the randum number.
	   'ratio' is multiplied by 'scale' after each iteration
	   to effectively reduce the randum number range.
	   */
	float ratio = (float) pow (2.0, -Roughness);
	float scale = heightScale * ratio;

    /* Seed the first four values. For example, in a 4x4 array, we
       would initialize the data points indicated by '*':

           *   .   .   .   *

           .   .   .   .   .

           .   .   .   .   .

           .   .   .   .   .

           *   .   .   .   *

       In terms of the "diamond-square" algorithm, this gives us
       "squares".

       We want the four corners of the array to have the same
       point. This will allow us to tile the arrays next to each other
       such that they join seemlessly. */

    uint8_t stride = CELLEDGESIZE / 2;

    Height[0][0] = 0.0;
    Height[CELLEDGESIZE][0] = 0.0;
    Height[0][CELLEDGESIZE] = 0.0;
    Height[CELLEDGESIZE][CELLEDGESIZE] = 0.0;

    CellTopZ = CellBottomZ = Height[0][0];

    /* Now we add ever-increasing detail based on the "diamond" seeded
       values. We loop over stride, which gets cut in half at the
       bottom of the loop. Since it's an int, eventually division by 2
       will produce a zero result, terminating the loop. */

    while (stride)
    {
		/* Take the existing "square" data and produce "diamond"
		   data. On the first pass through with a 4x4 matrix, the
		   existing data is shown as "X"s, and we need to generate the
	       "*" now:

               X   .   .   .   X

               .   .   .   .   .

               .   .   *   .   .

               .   .   .   .   .

               X   .   .   .   X

	      It doesn't look like diamonds. What it actually is, for the
	      first pass, is the corners of four diamonds meeting at the
	      center of the array. */
		for (uint16_t x = stride; x < CELLEDGESIZE; x += stride)
		{
			for (uint16_t y = stride; y < CELLEDGESIZE; y += stride)
			{
			    if (!Seeded[x][y])
			    {
			        float Average = (Height[x - stride][y - stride] + Height[x - stride][y + stride] + Height[x + stride][y - stride] + Height[x + stride][y + stride]) * 0.25;
                    float RandomFactor = scale * RandGenerator->Roll(-1.0f, 1.0f);

                    Height[x][y] = RandomFactor + Average;

                    if (Height[x][y] > CellTopZ)
                    {
                        CellTopZ = Height[x][y];
                    }
                    if (Height[x][y] < CellBottomZ)
                    {
                        CellBottomZ = Height[x][y];
                    }
			    }

				y += stride;
			}
			x += stride;
		}

		/* Take the existing "diamond" data and make it into
	       "squares". Back to our 4X4 example: The first time we
	       encounter this code, the existing values are represented by
	       "X"s, and the values we want to generate here are "*"s:

               X   .   *   .   X

               .   .   .   .   .

               *   .   X   .   *

               .   .   .   .   .

               X   .   *   .   X

	       i and j represent our (x,y) position in the array. The
	       first value we want to generate is at (i=2,j=0), and we use
	       "oddline" and "stride" to increment j to the desired value.
	       */
		uint16_t oddline = 0;
		for (uint16_t x = 0; x < CELLEDGESIZE; x += stride)
		{
		    oddline = (oddline == 0);
			for (uint16_t y = 0; y < CELLEDGESIZE; y += stride)
			{
				if ((oddline) && !y)
				{
				    y += stride;
				}

			    if (!Seeded[x][y])
			    {
                    float Average = (Height[x - stride][y] + Height[x + stride][y] + Height[x][y - stride] + Height[x][y + stride]) * 0.25;
                    float RandomFactor = scale * RandGenerator->Roll(-1.0f, 1.0f);

                    Height[x][y] = Average + RandomFactor;

                    // Record Maximum and Minimum Height
                    if (Height[x][y] > CellTopZ)
                    {
                        CellTopZ = Height[x][y];
                    }
                    if (Height[x][y] < CellBottomZ)
                    {
                        CellBottomZ = Height[x][y];
                    }
			    }
				y += stride;
			}
		}

		/* reduce random number range. */
		scale *= ratio;
		stride >>= 1;
    }
}

int16_t Geology::getRockTypeAtCoordinates(MapCoordinates Target)
{
    static uint16_t RockType0 = DATA->getLabelIndex("MATERIAL_GRANITE");
    static uint16_t RockType1 = DATA->getLabelIndex("MATERIAL_SANDSTONE");
    static uint16_t RockType2 = DATA->getLabelIndex("MATERIAL_LIMESTONE");

    CubeCoordinates Cube = CubeCoordinates(Target);

    if (Target.Z  > Height[Cube.X][Cube.Y])
    {
        return INVALID_INDEX;
    }

    if (Target.Z > 0)
    {
        return RockType1;
    }
    if (Target.Z < 0)
    {
        return RockType2;
    }
    return RockType0;
}
