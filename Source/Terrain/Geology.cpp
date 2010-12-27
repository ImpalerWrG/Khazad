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
#include <Cell.h>
#include <math.h>

#include <boost/functional/hash.hpp>


Geology::Geology()
{

}

Geology::~Geology()
{

}

bool Geology::Init(const char* SeedString)
{
    boost::hash< const char* > SeedHash;

    Ogre::Plane RockLayer = Ogre::Plane(Ogre::Vector3(0, 0, 1), 1);
    int RockType = 42;

    MasterSeed = 42; //SeedHash(SeedString);

    RandGenerator = new RandomNumberGenerator();
    RandGenerator->Init();

    NoiseGenerator = new Noise();

    LayerBoundries.push_back(make_pair(RockLayer, RockType));

    return true;
}

void Geology::GenerateWorldHeightMap(int32_t X, int32_t Y)
{
    WorldSizeX = X;
    WorldSizeY = Y;

    WorldHeight = new float*[X + 1];

    for (int32_t x = 0; x < X + 1; x++)
    {
        WorldHeight[x] = new float[Y + 1];

        for (int32_t y = 0; y < Y + 1; y++)
        {
            WorldHeight[x][y] = (NoiseGenerator->simplex_noise(2, x, y, 0) * 4);
            //WorldHeight[x][y] = x + y;
        }
    }
}

void Geology::GenerateCellEdge(float X, float Y, float heightScale, float Roughness)
{
    boost::hash< std::pair< float, float > > CoordinateHash;
    int32_t Hash = CoordinateHash(std::make_pair(Y, X));

    RandGenerator->Seed(MasterSeed);
    int32_t FinalSeed = RandGenerator->Roll(0, 0x7fffffff) ^ Hash;

    RandGenerator->Seed(FinalSeed);


    Edge[0] = X;     Edge[CELLEDGESIZE] = Y;

	float ratio = (float) pow (2.0, -Roughness);
	float scale = heightScale * ratio;

    /* Seed the endpoints of the array. To enable seamless wrapping,
       the endpoints need to be the same point. */
    uint8_t stride = CELLEDGESIZE / 2;

    while (stride)
    {
		for (int8_t i = stride; i < CELLEDGESIZE; i += stride)
		{
			Edge[i] = scale * RandGenerator->Roll(-1.0f, 1.0f) + ((Edge[i - stride] + Edge[i + stride]) * .5f);

			/* reduce random number range */
			scale *= ratio;

			i += stride;
		}
		stride >>= 1;
    }
}

void Geology::GenerateCellHeight(int32_t X, int32_t Y, float heightScale, float Roughness)
{
    uint8_t size = CELLEDGESIZE + 1;

    // Mark edges as Seeded to prevent them from being overwritten
    for (uint16_t x = 0; x < size; x++)
    {
        for (uint16_t y = 0; y < size; y++)
        {
            Seeded[x][y] = false;
            //Height[x][y] = 0.0f;
        }
    }

    // Edges Initialized to anchor values to create contiguousness
    GenerateCellEdge(WorldHeight[X][Y], WorldHeight[X + 1][Y], heightScale, Roughness);

    for (uint16_t x = 0; x < size; x++)
    {
        Height[x][0] = Edge[x];
        Seeded[x][0] = true;
    }

    GenerateCellEdge(WorldHeight[X][Y + 1], WorldHeight[X + 1][Y + 1], heightScale, Roughness);

    for (uint16_t x = 0; x < size; x++)
    {
        Height[x][CELLEDGESIZE] = Edge[x];
        Seeded[x][CELLEDGESIZE] = true;
    }

    GenerateCellEdge(WorldHeight[X][Y], WorldHeight[X][Y + 1], heightScale, Roughness);

    for (uint16_t y = 0; y < size; y++)
    {
        Height[0][y] = Edge[y];
        Seeded[0][y] = true;
    }

    GenerateCellEdge(WorldHeight[X + 1][Y], WorldHeight[X + 1][Y + 1], heightScale, Roughness);

    for (uint16_t y = 0; y < size; y++)
    {
        Height[CELLEDGESIZE][y] = Edge[y];
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

    Height[0][0] = WorldHeight[X][Y];
    Seeded[0][0] = true;

    Height[CELLEDGESIZE][0] = WorldHeight[X + 1][Y];
    Seeded[CELLEDGESIZE][0] = true;

    Height[0][CELLEDGESIZE] = WorldHeight[X][Y + 1];
    Seeded[0][CELLEDGESIZE] = true;

    Height[CELLEDGESIZE][CELLEDGESIZE] = WorldHeight[X + 1][Y + 1];
    Seeded[CELLEDGESIZE][CELLEDGESIZE] = true;

    CellTopZ = max(max(Height[0][0], Height[CELLEDGESIZE][0]), max(Height[0][CELLEDGESIZE], Height[CELLEDGESIZE][CELLEDGESIZE]));
    CellBottomZ = min(min(Height[0][0], Height[CELLEDGESIZE][0]), min(Height[0][CELLEDGESIZE], Height[CELLEDGESIZE][CELLEDGESIZE]));


    // Set initial Fractal value range (scale) and the rate of decrese (ratio)
	float ratio = (float) pow (2.0, -Roughness);
	float scale = heightScale * ratio;

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

int16_t Geology::getRockTypeAtCoordinates(CubeCoordinates Target, int32_t Zlevel)
{
    static uint16_t RockType0 = DATA->getLabelIndex("MATERIAL_BRIGHT_GRASS");
    static uint16_t RockType1 = DATA->getLabelIndex("MATERIAL_SNOW");
    static uint16_t RockType2 = DATA->getLabelIndex("MATERIAL_GRANITE");

    if (Zlevel > 8)
    {
        return RockType1;
    }
    if (Zlevel > 7)
    {
        return RockType2;
    }
    return RockType0;
}

TileShape Geology::getTileShapeAtCoordinates(CubeCoordinates CubeTarget, int32_t Zlevel)
{
    if (CubeTarget.X == 7 && CubeTarget.Y == 6 && Zlevel == 0)
    {
        bool Debug = true;
    }


    float Remainder;

    Remainder = Height[CubeTarget.X][CubeTarget.Y] - ((float) Zlevel);
    int8_t SWCornerHeight = min(max(roundf(Remainder * HEIGHT_FRACTIONS) + 1, 0.0f), (float) HEIGHT_FRACTIONS + 1);

    Remainder = Height[CubeTarget.X + 1][CubeTarget.Y] - ((float) Zlevel);
    int8_t SECornerHeight = min(max(roundf(Remainder * HEIGHT_FRACTIONS) + 1, 0.0f), (float) HEIGHT_FRACTIONS + 1);

    Remainder = Height[CubeTarget.X][CubeTarget.Y + 1] - ((float) Zlevel);
    int8_t NWCornerHeight = min(max(roundf(Remainder * HEIGHT_FRACTIONS) + 1, 0.0f), (float) HEIGHT_FRACTIONS + 1);

    Remainder = Height[CubeTarget.X + 1][CubeTarget.Y + 1] - ((float) Zlevel);
    int8_t NECornerHeight = min(max(roundf(Remainder * HEIGHT_FRACTIONS) + 1, 0.0f), (float) HEIGHT_FRACTIONS + 1);


    if (NECornerHeight == 0 || SWCornerHeight == 0)
    {
        return getTileShapeFromCornerHeight(SWCornerHeight, SECornerHeight, NWCornerHeight, NECornerHeight, true);
    }

    if (SECornerHeight == 0 || NWCornerHeight == 0)
    {
        return getTileShapeFromCornerHeight(SWCornerHeight, SECornerHeight, NWCornerHeight, NECornerHeight, false);
    }

    return getTileShapeFromCornerHeight(SWCornerHeight, SECornerHeight, NWCornerHeight, NECornerHeight, true);
}

void Geology::LoadCellData(Cell* TargetCell)
{
    CellCoordinates TargetCoordinates = TargetCell->getCellCoordinates();

    for (CubeCoordinates TargetCubeCoordinates; TargetCubeCoordinates.Index < (CUBESPERCELL - 1); ++TargetCubeCoordinates)
    {
        TileShape Shape = getTileShapeAtCoordinates(TargetCubeCoordinates, TargetCoordinates.Z);

        if (Shape != TILESHAPE_EMPTY)
        {
            int16_t MaterialType = getRockTypeAtCoordinates(TargetCubeCoordinates, TargetCoordinates.Z);
            TargetCell->setCubeMaterial(TargetCubeCoordinates, MaterialType);

            if (MaterialType != INVALID_INDEX)
            {
                TargetCell->setCubeShape(TargetCubeCoordinates, Shape);
                //TargetCell->setCubeSurface(TargetCubeCoordinates, RoughWallID);
            }
        }
        else
        {
            TargetCell->setCubeShape(TargetCubeCoordinates, Shape);
            TargetCell->setCubeMaterial(TargetCubeCoordinates, INVALID_INDEX);
        }
    }
}
