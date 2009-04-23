#include <stdafx.h>

#include <Map.h>
#include <Singleton.h>
#include <ConfigManager.h>
#include <TextureManager.h>
#include <Extract.h>
#include <Cube.h>
#include <Face.h>
#include <Cell.h>
#include <Random.h>

DECLARE_SINGLETON(Map)

Map::Map()
{

}

Map::~Map()
{

}

bool Map::Init()
{
    CellSizeX = CONFIG->getXMap();
	CellSizeY = CONFIG->getYMap();
	CellSizeZ = CONFIG->getZMap();

	CubesPerCellSide = (Uint8) CONFIG->getCellEdgeLength();

	CellArray = new Cell***[CellSizeX];

	for (Uint32 i = 0; i < CellSizeX; i++)
	{
		CellArray[i] = new Cell**[CellSizeY];

		for (Uint32 j = 0; j < CellSizeY; j++)
		{
			CellArray[i][j] = new Cell*[CellSizeZ];

			for (Uint32 k = 0; k < CellSizeZ; k++)
			{
				CellArray[i][j][k] = new Cell(i * CubesPerCellSide, j * CubesPerCellSide, k);
			}
		}
	}

    MapSizeX = CellSizeX * CubesPerCellSide;
	MapSizeY = CellSizeY * CubesPerCellSide;
	MapSizeZ = CellSizeZ;

	return true;
}

bool Map::Generate(Uint32 Seed)
{
    RANDOM->Seed(Seed);

    Uint32 TerrainX = MapSizeX + 1;
    Uint32 TerrainY = MapSizeY + 1;

    float** TerrainArray;
    TerrainArray = new float*[TerrainX];

    for(Uint32 i = 0; i < TerrainX; i++)
    {
        TerrainArray[i] = new float[TerrainY];

        for(Uint32 j = 0; j < TerrainY; j++)
        {
            TerrainArray[i][j] = RANDOM->Roll(0, 1);
        }
    }

    float Acumulator = 0.0;
    float North, South, East, West;
    Slopping Type;

    Cube* NewCube = NULL;
    Cell* TargetCell = NULL;

    for(Sint32 i = 0; i < MapSizeX; i++)
    {
        for(Sint32 j = 0; j < MapSizeY; j++)
        {
            North = TerrainArray[i][j];
            East = TerrainArray[i + 1][j];
            West = TerrainArray[i][j + 1];
            South = TerrainArray[i + 1][j + 1];

            if(North == East)
            {
                if(South == West)
                {
                    if(North > South)
                    {
                        Type = SLOPE_SOUTH_WEST;
                    }
                    else
                    {
                        if(South > North)
                        {
                            Type = SLOPE_NORTH_EAST;
                        }
                        else
                        {
                            Type = SLOPE_FLAT;
                        }
                    }
                }
                else
                {
                    Type = SLOPE_FLAT;
                }
            }
            else
            {
                if(East == South)
                {
                    if(North == West)
                    {
                        if(North > South)
                        {
                            Type = SLOPE_SOUTH_EAST;
                        }
                        else
                        {
                            if(South > North)
                            {
                                Type = SLOPE_NORTH_WEST;
                            }
                            else
                            {
                                Type = SLOPE_FLAT;
                            }
                        }
                    }
                    else
                    {
                        Type = SLOPE_FLAT;
                    }
                }
                else
                {
                    Type = SLOPE_FLAT;
                }
            }

            if(East == West)
            {
                if(North > East)
                {
                    if(South > East)
                    {
                        Type = SLOPE_CRESS_EAST_WEST;
                    }
                    else
                    {
                        Type = SLOPE_SMALL_SOUTH;
                    }
                }
                else
                {
                    if(North < East)
                    {
                        if(South < East)
                        {
                            Type = SLOPE_CRESS_NORTH_SOUTH;
                        }
                        else
                        {
                            Type = SLOPE_LARGE_NORTH;
                        }
                    }
                    else
                    {
                        if(South < East)
                        {
                            Type = SLOPE_LARGE_SOUTH;
                        }
                        else
                        {
                            if(South > East)
                            {
                                Type = SLOPE_SMALL_NORTH;
                            }
                            else
                            {
                                Type = SLOPE_FLAT;
                            }
                        }
                    }
                }
            }

            if(North == South)
            {
                if(East > South)
                {
                    if(West > South)
                    {
                        Type = SLOPE_CRESS_NORTH_SOUTH;
                    }
                    else
                    {
                        Type = SLOPE_SMALL_WEST;
                    }
                }
                else
                {
                    if(East < South)
                    {
                        if(West < South)
                        {
                            Type = SLOPE_CRESS_EAST_WEST;
                        }
                        else
                        {
                            Type = SLOPE_LARGE_EAST;
                        }
                    }
                    else
                    {
                        if(West < South)
                        {
                            Type = SLOPE_LARGE_WEST;
                        }
                        else
                        {
                            if(West > South)
                            {
                                Type = SLOPE_SMALL_EAST;
                            }
                            else
                            {
                                Type = SLOPE_FLAT;
                            }
                        }
                    }
                }
            }

            TargetCell = getCubeOwner(i, j, North);

            if(TargetCell)
            {
                if(!TargetCell->Initalized)
                {
                    TargetCell->Init();
                }

                Acumulator = North + South + East + West;
                NewCube = getCube(i, j, Acumulator / 4);

                if (NewCube)
                {
                    if (NewCube->Initalized != true)
                    {
                        NewCube->Init(RANDOM->Roll(0, 4));   // TEMPORARY RANDOMIZING OF TEXTURES HACK, must have atleast this many texture and XML material entries
                        if (Type != SLOPE_FLAT)
                        {
                            NewCube->SetSlope(Type);
                        }
                        NewCube->setVisible(true);
                    }
                }
            }
        }
    }

    for(Uint32 i = 0; i < TerrainX; i++)
    {
        delete TerrainArray[i];
    }

    delete TerrainArray;

    return true;
}

Cell* Map::getCell(Sint32 X, Sint32 Y, Sint32 Z)
{
	if (X >= 0 && X < CellSizeX && Y >= 0 && Y < CellSizeY && Z >= 0 && Z < CellSizeZ )
	{
		return CellArray[X][Y][Z];
	}
	return NULL;
}

Cell* Map::getCubeOwner(Sint32 X, Sint32 Y, Sint32 Z)
{
    if ((X > MapSizeX) || (Y > MapSizeY) || (Z > MapSizeZ))
    {
        return NULL;
    }

    Sint32 CellX = X / CubesPerCellSide;
    Sint32 CellY = Y / CubesPerCellSide;

    return getCell(CellX, CellY, Z);
}

Cube* Map::getCube(Sint32 X, Sint32 Y, Sint32 Z)
{
    Cell* TargetCell = getCubeOwner(X, Y, Z);

    if(TargetCell)
    {
        if(TargetCell->Initalized)
        {
            Sint32 CubeX = X % CubesPerCellSide;
            Sint32 CubeY = Y % CubesPerCellSide;
            Cube* TargetCube = TargetCell->getCube(CubeX, CubeY);

            return TargetCube;
        }
        return NULL;
    }
    return NULL;
}

void Map::LoadExtract()
{
    CellSizeX = EXTRACT->x_blocks;
	CellSizeY = EXTRACT->y_blocks;
	CellSizeZ = EXTRACT->z_levels + 1; // Extra level for Basement

	CubesPerCellSide = (Uint8) CONFIG->getCellEdgeLength();

	CellArray = new Cell***[CellSizeX];

	for (Uint32 i = 0; i < CellSizeX; i++)
	{
		CellArray[i] = new Cell**[CellSizeY];

		for (Uint32 j = 0; j < CellSizeY; j++)
		{
			CellArray[i][j] = new Cell*[CellSizeZ];

			for (Uint32 k = 0; k < CellSizeZ; k++)
			{
				CellArray[i][j][k] = new Cell(i * CubesPerCellSide, j * CubesPerCellSide, k);

				if(k == 0)
				{
				    CellArray[i][j][k]->Init();
				    CellArray[i][j][k]->SetBasment(true);
				}
			}
		}
	}

    MapSizeX = CellSizeX * CubesPerCellSide;
	MapSizeY = CellSizeY * CubesPerCellSide;
	MapSizeZ = CellSizeZ;

    Cube* NewCube = NULL;
    Cell* TargetCell = NULL;

    for (Uint32 i = 0; i < MapSizeX; i++)
	{
		for (Uint32 j = 0; j < MapSizeY; j++)
		{
			for (Uint32 k = 0; k < MapSizeZ; k++)
			{
                TargetCell = getCubeOwner(i, j, k);
                if(TargetCell)
                {
                    int TileType = EXTRACT->Tiles[i][j][k];

                    bool IsFloor = EXTRACT->isFloorTerrain(TileType);
                    bool IsWall = EXTRACT->isWallTerrain(TileType);
                    bool IsOpen = EXTRACT->isOpenTerrain(TileType);
                    bool IsRamp = EXTRACT->isRampTerrain(TileType);

                    if(IsFloor || IsWall || IsOpen || IsRamp)
                    {
                        if(!TargetCell->Initalized)
                        {
                            TargetCell->Init();
                        }

                        Uint16 Material = EXTRACT->picktexture(TileType);
                        NewCube = getCube(i, j, k);

                        if (NewCube)
                        {
                            if(IsWall)
                            {
                                NewCube->Init(Material);
                            }

                            if(IsOpen)
                            {
                                NewCube->Open();
                                NewCube->setVisible(true);
                                NewCube->setAllFacesVisiblity(true);
                            }

                            if(IsFloor)
                            {
                                if(Material == 31)
                                {
                                    int x = 666;
                                }
                                NewCube->InitConstructedFace(FACET_BOTTOM, Material);
                            }

                            if (IsRamp)
                            {
                                NewCube->Init(Material);
                                NewCube->SetSlope(SLOPE_SOUTH_WEST);
                                NewCube->setVisible(true);
                            }

                            NewCube->setVisible(true);
                            NewCube->setAllFacesVisiblity(true);
                        }
                    }
                }
            }
		}
	}


    for (Uint32 i = 0; i < MapSizeX; i++)
	{
		for (Uint32 j = 0; j < MapSizeY; j++)
		{
			for (Uint32 k = 1; k < MapSizeZ; k++)
			{
                NewCube = getCube(i, j, k);
                if(NewCube)
                {
                    NewCube->InitAllFaces();
                }
			}
		}
	}
}
