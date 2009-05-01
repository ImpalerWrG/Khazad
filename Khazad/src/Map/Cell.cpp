#include <stdafx.h>

#include <Cell.h>
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
    Uint8 CubesPerCellSide = (Uint8) CONFIG->getCellEdgeLength();
    Cubes = new Cube*[CubesPerCellSide];

    DrawListID = glGenLists(5);

	//float X = Position.x - (CubesPerCellSide / 2);
	//float Y = Position.y - (CubesPerCellSide / 2);
	float HalfCell = CubesPerCellSide / 2;

	for (Uint8 i = 0; i < CubesPerCellSide; i++)
	{
		Cubes[i] = new Cube[CubesPerCellSide];

		for (Uint8 j = 0; j < CubesPerCellSide; j++)
		{
			Cubes[i][j].Position.x = (float) Position.x - HalfCell + i + 0.5;
			Cubes[i][j].Position.y = (float) Position.y - HalfCell + j + 0.5;
			Cubes[i][j].Position.z = (float) Position.z;
		}
	}

	return true;
}

Cell::Cell(Sint32 X, Sint32 Y, Sint32 Z)
{
	Active = false;
    Initalized = false;
	setType(CELL_ACTOR);
    Uint8 CubesPerCellSide = (Uint8) CONFIG->getCellEdgeLength();

	Position.x = (float) X + (CubesPerCellSide / 2) - 0.5;
	Position.y = (float) Y + (CubesPerCellSide / 2) - 0.5;
	Position.z = (float) Z;

    Basment = false;
	Active = true;
}

Cube* Cell::getCube(Uint16 x, Uint16 y)
{
	return &Cubes[x][y];
}

bool Cell::Update()
{
	return true;
}

bool Cell::Draw(CameraOrientation Orientation, bool DrawHidden)
{
	Uint16 CellEdgeLength = CONFIG->getCellEdgeLength();
    Cube* LoopCube = NULL;

    float HalfCell = CellEdgeLength / 2;

    if(Initalized)
    {
        for (Uint16 x = 0; x < CellEdgeLength; x++)
        {
            for (Uint16 y = 0; y < CellEdgeLength; y++)
            {
                LoopCube = getCube(x, y);
                if (LoopCube->isVisible())
                {
                    LoopCube->Draw(Orientation, x - HalfCell + 0.5, y - HalfCell + 0.5, DrawHidden);
                }
            }
        }
    }

	return true;
}
