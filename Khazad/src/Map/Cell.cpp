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

    DrawListID = glGenLists(1);

	float X = Position.x - (CubesPerCellSide / 2);
	float Y = Position.y - (CubesPerCellSide / 2);

	for (Uint8 i = 0; i < CubesPerCellSide; i++)
	{
		Cubes[i] = new Cube[CubesPerCellSide];

		for (Uint8 j = 0; j < CubesPerCellSide; j++)
		{
			Cubes[i][j].Position.x = (float) X + i;
			Cubes[i][j].Position.y = (float) Y + j;
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

	Position.x = (float) X + (CubesPerCellSide / 2);
	Position.y = (float) Y + (CubesPerCellSide / 2);
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

bool Cell::Draw()
{
    float Shading = SCREEN->getShading(Position.z);  // Atleast one gl call must occure in each Drawlist to prevent weird Segfault in Atioglx1.dll
    glColor3f(Shading, Shading, Shading);

    if(Basment) // Nothing to draw for these Cells  (Sky | Rock | Air)
    {
        return true;
    }

	Uint16 CellEdgeLenth = CONFIG->getCellEdgeLength();

    Cube* LoopCube = NULL;

    if(Initalized && Shading > 0)
    {
        for (Uint16 x = 0; x < CellEdgeLenth; x++)
        {
            for (Uint16 y = 0; y < CellEdgeLenth; y++)
            {
                LoopCube = getCube(x, y);
                if (LoopCube->Visible)
                {
                    LoopCube->Draw();
                }
            }
        }
    }
	return true;
}
