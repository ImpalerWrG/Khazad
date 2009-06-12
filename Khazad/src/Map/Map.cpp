#include <stdafx.h>

#include <Map.h>
#include <Singleton.h>
#include <ConfigManager.h>
#include <TextureManager.h>
#include <DataManager.h>
#include <Extract.h>
#include <Cube.h>
#include <Face.h>
#include <Cell.h>
#include <Random.h>

DECLARE_SINGLETON(Map)

Map::Map()
{
    InitilizeTilePicker();

    Initialized = false;
}

Map::~Map()
{

}

bool Map::Init()
{
    InitilizeTilePicker();

    CellSizeX = CONFIG->getXMap();
	CellSizeY = CONFIG->getYMap();
	CellSizeZ = CONFIG->getZMap();

	CellArray = new Cell***[CellSizeX];

	for (Uint32 i = 0; i < CellSizeX; i++)
	{
		CellArray[i] = new Cell**[CellSizeY];

		for (Uint32 j = 0; j < CellSizeY; j++)
		{
			CellArray[i][j] = new Cell*[CellSizeZ];

			for (Uint32 k = 0; k < CellSizeZ; k++)
			{
				CellArray[i][j][k] = new Cell(i * CELLEDGESIZE, j * CELLEDGESIZE, k);
			}
		}
	}

    MapSizeX = CellSizeX * CELLEDGESIZE;
	MapSizeY = CellSizeY * CELLEDGESIZE;
	MapSizeZ = CellSizeZ;

    Initialized = true;

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
                if(!TargetCell->isInitalized())
                {
                    TargetCell->Init();
                }

                Acumulator = North + South + East + West;
                NewCube = getCube(i, j, Acumulator / 4);

                if (NewCube)
                {
                    if (NewCube->isInitalized() != true)
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

    Sint32 CellX = X / CELLEDGESIZE;
    Sint32 CellY = Y / CELLEDGESIZE;

    return getCell(CellX, CellY, Z);
}

Cube* Map::getCube(Sint32 X, Sint32 Y, Sint32 Z)
{
    Cell* TargetCell = getCubeOwner(X, Y, Z);

    if(TargetCell)
    {
        if(TargetCell->isInitalized())
        {
            Sint32 CubeX = X % CELLEDGESIZE;
            Sint32 CubeY = Y % CELLEDGESIZE;

            return TargetCell->getCube(CubeX, CubeY);
        }
        return NULL;
    }
    return NULL;
}

Face* Map::getFace(Sint32 X, Sint32 Y, Sint32 Z, Facet FaceType)
{
    Cell* TargetCell = getCubeOwner(X, Y, Z);

    if(TargetCell)
    {
        if(TargetCell->isInitalized())
        {
            Sint32 CubeX = X % CELLEDGESIZE;
            Sint32 CubeY = Y % CELLEDGESIZE;

            return TargetCell->getFace(CubeX, CubeY, FaceType);
        }
        return NULL;
    }
    return NULL;
}

void Map::LoadExtract()
{
    if(!EXTRACT->isMapLoaded())
    {
        return;
    }

    CellSizeX = EXTRACT->getXBlocks();
	CellSizeY = EXTRACT->getYBlocks();
	CellSizeZ = EXTRACT->getZBlocks();

	CellArray = new Cell***[CellSizeX];

	for (Uint32 i = 0; i < CellSizeX; i++)
	{
		CellArray[i] = new Cell**[CellSizeY];

		for (Uint32 j = 0; j < CellSizeY; j++)
		{
			CellArray[i][j] = new Cell*[CellSizeZ];

			for (Uint32 k = 0; k < CellSizeZ; k++)
			{
				CellArray[i][j][k] = new Cell(i * CELLEDGESIZE, j * CELLEDGESIZE, k);

				//if(k == 0)
				//{
				//    CellArray[i][j][k]->Init();
				//    CellArray[i][j][k]->SetBasment(true);
				//}
			}
		}
	}

    MapSizeX = CellSizeX * CELLEDGESIZE;
	MapSizeY = CellSizeY * CELLEDGESIZE;
	MapSizeZ = CellSizeZ;

    Cube* TargetCube = NULL;
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
                    int TileType = EXTRACT->getTileType(i, j, k);

                    bool IsFloor = EXTRACT->isFloorTerrain(TileType);
                    bool IsWall = EXTRACT->isWallTerrain(TileType);
                    bool IsOpen = EXTRACT->isOpenTerrain(TileType);
                    bool IsRamp = EXTRACT->isRampTerrain(TileType);
                    bool IsStairs = EXTRACT->isStairTerrain(TileType);

                    int Liquid = EXTRACT->getLiquidLevel(i, j, k);

                    if(IsFloor || IsWall || IsOpen || IsRamp || IsStairs)
                    {
                        if(!TargetCell->isInitalized())
                        {
                            TargetCell->Init();
                        }

                        TargetCube = getCube(i, j, k);

                        if(!TargetCube)
                        {
                            TargetCube = new Cube();
                            TargetCell->setCube(TargetCube, i % CELLEDGESIZE, j % CELLEDGESIZE);
                            TargetCube->setPosition((float) i, (float) j, (float) k);
                        }

                        Uint16 Material = PickTexture(TileType);

                        bool Hidden = EXTRACT->isDesignationFlag(DESIGNATION_HIDDEN, i, j, k);
                        TargetCube->setHidden(Hidden);

                        TargetCube->setSubTerranean(EXTRACT->isDesignationFlag(DESIGNATION_SUBTERRANEAN, i, j, k));
                        TargetCube->setSkyView(EXTRACT->isDesignationFlag(DESIGNATION_SKY_VIEW, i, j, k));
                        TargetCube->setSunLit(EXTRACT->isDesignationFlag(DESIGNATION_OPEN_TO_SUN, i, j, k));

                        if(IsWall)
                        {
                            TargetCube->Init(Material);
                        }

                        if(IsOpen)
                        {
                            TargetCube->Open();
                        }

                        if(IsRamp)
                        {
                            TargetCube->Init(Material);
                            TargetCube->Open();
                            TargetCube->SetSlope(SLOPE_FLAT);  // Prime the Slope, the type can not yet be determined
                        }

                        if(IsFloor)
                        {
                            TargetCube->InitConstructedFace(FACET_BOTTOM, Material);
                        }

                        if(IsStairs)
                        {
                            TargetCube->Init(Material);
                            TargetCube->Open();
                            TargetCube->SetSlope(SLOPE_FLAT);
                            //TODO render stairs differently
                        }

                        if(Liquid)
                        {
                            TargetCube->Open();
                            TargetCube->setLiquid((Uint8) Liquid);

                            if(EXTRACT->isDesignationFlag(DESIGNATION_LIQUID_TYPE, i, j, k))
                            {
                                TargetCube->InitConstructedFace(FACET_TOP, DATA->getLabelIndex("MATERIAL_LAVA"));
                            }
                            else
                            {

                                TargetCube->InitConstructedFace(FACET_TOP, DATA->getLabelIndex("MATERIAL_WATER"));
                            }
                        }

                        TargetCube->setVisible(true);
                    }
                }
            }
		}
	}

    // Initialize Faces
    for (Uint32 i = 0; i < MapSizeX; i++)
	{
		for (Uint32 j = 0; j < MapSizeY; j++)
		{
			for (Uint32 k = 0; k < MapSizeZ; k++)
			{
                TargetCube = getCube(i, j, k);
                if(TargetCube)
                {
                    if(!TargetCube->isSolid())
                    {
                        TargetCube->InitFacesOpen();
                    }

                    if(TargetCube->getSlope() != NULL)
                    {
                        TargetCube->DetermineSlope();
                    }
                }
			}
		}
	}

    // Initialize Drawlists
	for(Uint16 Zlevel = 0; Zlevel < getCellSizeZ(); Zlevel++)
	{
        for (Uint32 SizeX = 0; SizeX < getCellSizeX(); SizeX++)
        {
            for (Uint32 SizeY = 0; SizeY < getCellSizeY(); SizeY++)
            {
                Cell* LoopCell = getCell(SizeX, SizeY, Zlevel);

                if(LoopCell->isActive())
                {
                    if(LoopCell->DirtyDrawlist)
                    {
                        // Rebuild the new Drawlist
                        GLuint DrawListID = LoopCell->DrawListID;
                        glDeleteLists(DrawListID, 5);

                        for(CameraOrientation Orientation = CAMERA_DOWN; Orientation < NUM_ORIENTATIONS; ++Orientation)
                        {
                            SCREEN->RefreshDrawlist(LoopCell, DrawListID + (GLuint) Orientation, Orientation);
                        }
                        LoopCell->DirtyDrawlist = false;
                    }
                }
            }
        }
	}


    Initialized = true;
}

void Map::InitilizeTilePicker()
{
    for(int i = 0; i < 600; ++i)
    {
        TilePicker[i] = DATA->getLabelIndex("TEXTURE_NEHE");
    }

    for(int i = 0; i < DATA->getNumMaterials(); ++i)
    {
        for(int j = 0; j < DATA->getMaterialData(i)->TileTypes.size(); ++j)
        {
            int Tile = DATA->getMaterialData(i)->TileTypes[j];
            TilePicker[Tile] = i;
        }
    }
}

Uint32 Map::PickTexture(int TileType)
{
    return TilePicker[TileType];
}
