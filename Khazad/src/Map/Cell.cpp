#include <stdafx.h>

#include <Cell.h>
#include <Face.h>
#include <Cube.h>
#include <ConfigManager.h>
#include <Random.h>
#include <Game.h>

/*
Cell::Cell()
{
	Active = false;
	setType(CELL_ACTOR);
    Initalized = false;
}*/

Cell::~Cell()
{

}

bool Cell::Init()
{
    Initalized = true;

    Cubes = new Cube*[CellEdgeSize];
    Facets = new Face**[CellEdgeSize];

	float HalfCell = CellEdgeSize / 2;

	for(Uint8 i = 0; i < CellEdgeSize; i++)
	{
		Cubes[i] = new Cube[CellEdgeSize];
		Facets[i] = new Face*[CellEdgeSize];

		for(Uint8 j = 0; j < CellEdgeSize; j++)
		{
			Cubes[i][j].Position.x = (float) Position.x - HalfCell + i + 0.5;
			Cubes[i][j].Position.y = (float) Position.y - HalfCell + j + 0.5;
			Cubes[i][j].Position.z = (float) Position.z;

            Facets[i][j] = new Face[NUM_FACETS];
		}
	}

	return true;
}

Cell::Cell(Sint32 X, Sint32 Y, Sint32 Z)
{
	Active = false;
    Initalized = false;

    DrawListID = glGenLists(5);

	setType(CELL_ACTOR);

	Position.x = (float) X + (CellEdgeSize / 2) - 0.5;
	Position.y = (float) Y + (CellEdgeSize / 2) - 0.5;
	Position.z = (float) Z;

    //Basment = false;
}

Cube* Cell::getCube(Uint16 x, Uint16 y)
{
	return &Cubes[x][y];
}

bool Cell::Update()
{
	return true;
}

bool Cell::Draw(CameraOrientation Orientation, bool DrawHidden, bool DrawSubTerranian, bool DrawSkyView, bool DrawSunLit)
{
    Cube* LoopCube = NULL;

    float HalfCell = CellEdgeSize / 2;

    if(Initalized)
    {
        for (Uint16 x = 0; x < CellEdgeSize; x++)
        {
            for (Uint16 y = 0; y < CellEdgeSize; y++)
            {
                LoopCube = getCube(x, y);
                if (LoopCube->isVisible())
                {
                    LoopCube->Draw(Orientation, x - HalfCell + 0.5, y - HalfCell + 0.5, DrawHidden, DrawSubTerranian, DrawSkyView, DrawSunLit);
                }
            }
        }
    }

	return true;
}
