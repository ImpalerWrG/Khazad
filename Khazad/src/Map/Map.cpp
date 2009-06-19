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
    Initialized = false;

	MapSizeX = 0;
	MapSizeY = 0;
	MapSizeZ = 0;

	CellSizeX = 0;
	CellSizeY = 0;
	CellSizeZ = 0;

    CellCount = 0;
    CubeCount = 0;
    FaceCount = 0;
    SlopeCount = 0;

    InitedCellCount = 0;
    InitedCubeCount = 0;
    InitedFaceCount = 0;
    InitedSlopeCount = 0;

    CellArray = NULL;
}

Map::~Map()
{

}

bool Map::Init()
{
    InitilizeTilePicker();

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

    Slopping Type;

    Cube* NewCube = NULL;
    Cell* TargetCell = NULL;

    for(Sint32 i = 0; i < MapSizeX; i++)
    {
        for(Sint32 j = 0; j < MapSizeY; j++)
        {
            TargetCell = getCubeOwner(i, j, 0);

            if(TargetCell)
            {
                if(!TargetCell->isInitalized())
                {
                    TargetCell->Init();
                }

                NewCube = getCube(i, j, 0);

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

    if(TargetCell != NULL)
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
			    if(EXTRACT->isBlockInitialized(i, j, k))
			    {
                    CellArray[i][j][k] = new Cell(i * CELLEDGESIZE, j * CELLEDGESIZE, k);
                    CellArray[i][j][k]->Init();

                    for(Uint32 l = 0; l < CELLEDGESIZE; l++)
                    {
                        for(Uint32 m = 0; m < CELLEDGESIZE; m++)
                        {
                            LoadCubeData(CellArray[i][j][k], i, j, k, l, m);
                        }
                    }
			    }
			    else
			    {
                    CellArray[i][j][k] = NULL;
			    }
			}
		}
	}

    MapSizeX = CellSizeX * CELLEDGESIZE;
	MapSizeY = CellSizeY * CELLEDGESIZE;
	MapSizeZ = CellSizeZ;

    // Initialize Faces
    for (Uint32 i = 0; i < MapSizeX; i++)
	{
		for (Uint32 j = 0; j < MapSizeY; j++)
		{
			for (Uint32 k = 0; k < MapSizeZ; k++)
			{
                Cube* TargetCube = getCube(i, j, k);
                if(TargetCube != NULL)
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

                if(LoopCell != NULL && LoopCell->isActive())
                {
                    if(LoopCell->isDirtyDrawList())
                    {
                        // Rebuild the new Drawlist
                        GLuint DrawListID = LoopCell->getDrawListID();
                        glDeleteLists(DrawListID, 5);

                        for(CameraOrientation Orientation = CAMERA_DOWN; Orientation < NUM_ORIENTATIONS; ++Orientation)
                        {
                            SCREEN->RefreshDrawlist(LoopCell, DrawListID + (GLuint) Orientation, Orientation);
                        }
                        LoopCell->setDirtyDrawList(false);
                    }
                }
            }
        }
	}

    MapLoaded = true;
}

void Map::LoadCubeData(Cell* TargetCell, Uint32 CellX, Uint32 CellY, Uint32 CellZ, Uint32 CubeX, Uint32 CubeY)
{
    Uint32 MapX = (CellX * CELLEDGESIZE) + CubeX;
	Uint32 MapY = (CellY * CELLEDGESIZE) + CubeY;
	Uint32 MapZ = CellZ;

    int TileType = EXTRACT->getTileType(CellX, CellY, CellZ, CubeX, CubeY);

    bool IsFloor = EXTRACT->isFloorTerrain(TileType);
    bool IsWall = EXTRACT->isWallTerrain(TileType);
    bool IsOpen = EXTRACT->isOpenTerrain(TileType);
    bool IsRamp = EXTRACT->isRampTerrain(TileType);
    bool IsStairs = EXTRACT->isStairTerrain(TileType);

    int Liquid = EXTRACT->getLiquidLevel(MapX, MapY, MapZ);

    if(IsFloor || IsWall || IsOpen || IsRamp || IsStairs)
    {
        Cube* TargetCube = TargetCell->getCube(CubeX, CubeY);

        TargetCube = new Cube();
        TargetCell->setCube(TargetCube, CubeX, CubeY);
        TargetCube->setPosition((float) MapX, (float) MapY, (float) MapZ);

        Uint16 Material = PickTexture(TileType);

        bool Hidden = EXTRACT->isDesignationFlag(DESIGNATION_HIDDEN, MapX, MapY, MapZ);
        TargetCube->setHidden(Hidden);

        TargetCube->setSubTerranean(EXTRACT->isDesignationFlag(DESIGNATION_SUBTERRANEAN, MapX, MapY, MapZ));
        TargetCube->setSkyView(EXTRACT->isDesignationFlag(DESIGNATION_SKY_VIEW, MapX, MapY, MapZ));
        TargetCube->setSunLit(EXTRACT->isDesignationFlag(DESIGNATION_OPEN_TO_SUN, MapX, MapY, MapZ));

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

            if(EXTRACT->isDesignationFlag(DESIGNATION_LIQUID_TYPE, MapX, MapY, MapZ))
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
