#include <stdafx.h>

#include <Map.h>
#include <Singleton.h>
#include <TextureManager.h>
#include <DataManager.h>
///FIXME: dfhack paths
#include <Extract.h>
#include <DFTypes.h>
#include <DfMap.h>
#include <DFTileTypes.h>

#include <Column.h>
#include <Cube.h>
#include <Face.h>
#include <Cell.h>
#include <Random.h>
#include <Building.h>
#include <Tree.h>

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

    ColumnMatrix = NULL;
    DFExtractor = new Extractor();
}

Map::~Map()
{
    if(MapLoaded)
    {
        ReleaseMap();
    }
    delete DFExtractor;
}

bool Map::Init()
{
    BuildVertexArray();

    Initialized = true;
    MapLoaded = false;

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

    Slopping Type = SLOPE_FLAT;

    Cube* NewCube = NULL;
    Cell* TargetCell = NULL;

    for(Uint32 i = 0; i < MapSizeX; i++)
    {
        for(Uint32 j = 0; j < MapSizeY; j++)
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
                        NewCube->Init(0);
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
	if (X >= 0 && X < CellSizeX && Y >= 0 && Y < CellSizeY)
	{
	    if(Z >= ColumnMatrix[X][Y]->BottomLevel() && Z <= ColumnMatrix[X][Y]->TopLevel())
	    {
            return ColumnMatrix[X][Y]->getCell(Z);
	    }
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

DfMap *Map::getDFMap()
{
    return DFExtractor->getMap();
}

void Map::ReparseExtract()
{
    if(!DFExtractor->isMapLoaded())
    {
        return;
    }

    if(MapLoaded)
    {
        ReleaseMap();
    }

    InitilizeTilePicker();

    DfMap* ExtractedMap = DFExtractor->getMap();

    // Initialize Cells and Cubes
    CellSizeX = ExtractedMap->getXBlocks();
	CellSizeY = ExtractedMap->getYBlocks();
	CellSizeZ = ExtractedMap->getZBlocks();


    ColumnMatrix = new Column**[CellSizeX];

	for (Uint16 x = 0; x < CellSizeX; x++)
	{
		ColumnMatrix[x] = new Column*[CellSizeY];

		for (Uint16 y = 0; y < CellSizeY; y++)
		{
			ColumnMatrix[x][y] = new Column();
			ColumnMatrix[x][y]->Init(x, y);

			for (Uint32 z = 0; z < CellSizeZ; z++)
			{
/*			    if(x == 0 && y == (CellSizeY - 1) && z == 25)
			    {
			        bool Debug = true;
			    }*/

			    if(ExtractedMap->isBlockInitialized(x, y, z))
			    {
                    Cell* NewCell = new Cell(x * CELLEDGESIZE, y * CELLEDGESIZE, z);
                    NewCell->Init();

                    for(Uint32 l = 0; l < CELLEDGESIZE; l++)
                    {
                        for(Uint32 m = 0; m < CELLEDGESIZE; m++)
                        {
                            LoadCubeData(NewCell, x, y, z, l, m);
                        }
                    }
                    vector<t_building *> * bldvect = ExtractedMap->getBlockBuildingsVector(x,y,z);
                    for(int i = 0; i< bldvect->size(); i++)
                    {
                        t_building * b = (*bldvect)[i];
                        Building *bld = new Building(b->x1,b->y1,b->x2,b->y2,b->z,b->material,b->type);
                        ///FIXME: destroy buildings when destroying map
                        NewCell->addBuilding(bld);
                    }
                    vector<t_tree_desc *> * treevect = ExtractedMap->getBlockVegetationVector(x,y,z);
                    for(int i = 0; i< treevect->size(); i++)
                    {
                        t_tree_desc* t = (*treevect)[i];
                        Tree *tree = new Tree(t->material,t->x,t->y,t->z);
                        ///FIXME: destroy trees when destroying map
                        NewCell->addTree(tree);
                    }

                    ColumnMatrix[x][y]->PushCell(NewCell, z);
			    }
			}
		}
	}

    MapSizeX = CellSizeX * CELLEDGESIZE;
	MapSizeY = CellSizeY * CELLEDGESIZE;
	MapSizeZ = CellSizeZ;

	for (Uint32 i = 0; i < CellSizeX; i++)
	{
		for (Uint32 j = 0; j < CellSizeY; j++)
		{
			for (Sint32 k = ColumnMatrix[i][j]->BottomLevel(); k < ColumnMatrix[i][j]->TopLevel(); k++)
			{
			    if(getCell(i, j, k) != NULL)
			    {
                    for(Uint32 l = 0; l < CELLEDGESIZE; l++)
                    {
                        for(Uint32 m = 0; m < CELLEDGESIZE; m++)
                        {
                            Cube* TargetCube = getCube((i * CELLEDGESIZE) + l, (j * CELLEDGESIZE) + m, k);
                            if(TargetCube != NULL && TargetCube->isInitalized())
                            {
                                if(TargetCube->isSolid())
                                {
                                    TargetCube->InitFacesSolid();
                                }
                                else
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

                if(LoopCell != NULL)
                {
                    GLuint DrawListID = LoopCell->getDrawListID();
                    if(LoopCell->isActive())
                    {
                        if(LoopCell->isDirtyDrawList())
                        {
                            // Rebuild the new Drawlist
                            glDeleteLists(DrawListID, 5);

                            for(CameraOrientation Orientation = CAMERA_DOWN; Orientation < NUM_ORIENTATIONS; ++Orientation)
                            {
                                SCREEN->RefreshDrawlist(LoopCell, DrawListID + (GLuint) Orientation, Orientation);
                            }
                            LoopCell->setDirtyDrawList(false);
                        }
                    }
                    if(LoopCell->isLiquidActive())
                    {
                        glDeleteLists(DrawListID + 5,1);
                        SCREEN->RefreshTransparentDrawlist(LoopCell, DrawListID + 5);
                    }
                }
            }
        }
	}
    MapLoaded = true;
}

// extract from DF
void Map::Load()
{
    Path path_to_xml("Assets/XML/Memory.xml");
    DFExtractor->dumpMemory(path_to_xml);
    ReparseExtract();
}

// load from file
void Map::Load(string filename)
{
    DFExtractor->loadMap(filename);
    ReparseExtract();
}

// save to file
void Map::Save(string filename)
{
    DFExtractor->writeMap(filename);
}

void Map::LoadCubeData(Cell* TargetCell, Uint32 CellX, Uint32 CellY, Uint32 CellZ, Uint32 CubeX, Uint32 CubeY)
{
    Uint32 MapX = (CellX * CELLEDGESIZE) + CubeX;
	Uint32 MapY = (CellY * CELLEDGESIZE) + CubeY;
	Uint32 MapZ = CellZ;

    DfMap *df_map = DFExtractor->getMap();

    int TileType = df_map->getTileType(CellX, CellY, CellZ, CubeX, CubeY);

    bool IsFloor = isFloorTerrain(TileType);
    bool IsWall = isWallTerrain(TileType);
    bool IsOpen = isOpenTerrain(TileType);
    bool IsRamp = isRampTerrain(TileType);
    bool IsStairs = isStairTerrain(TileType);

    int Liquid = df_map->getLiquidLevel(MapX, MapY, MapZ);

    if(IsFloor || IsWall || IsOpen || IsRamp || IsStairs)
    {
        Cube* TargetCube = TargetCell->getCube(CubeX, CubeY);

        TargetCube = new Cube();
        TargetCell->setCube(TargetCube, CubeX, CubeY);
        TargetCube->setPosition((float) MapX, (float) MapY, (float) MapZ);

        Uint16 Material = PickTexture(MapX, MapY, MapZ);

        bool Hidden = df_map->isHidden(MapX, MapY, MapZ);
        TargetCube->setHidden(Hidden);

        TargetCube->setSubTerranean(df_map->isSubterranean( MapX, MapY, MapZ));
        TargetCube->setSkyView(df_map->isSkyView( MapX, MapY, MapZ));
        TargetCube->setSunLit(df_map->isSunLit( MapX, MapY, MapZ));

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
            //TargetCube->Open();
            TargetCube->setLiquid(df_map->isMagma( MapX, MapY, MapZ),(Uint8) Liquid);
/*
            if(df_map->isMagma( MapX, MapY, MapZ))
            {
                TargetCube->InitConstructedFace(FACET_TOP, DATA->getLabelIndex("MATERIAL_LAVA"));
            }
            else
            {
                TargetCube->InitConstructedFace(FACET_TOP, DATA->getLabelIndex("MATERIAL_WATER"));
            }*/
        }
        TargetCube->setVisible(true);
    }
}

void Map::InitilizeTilePicker()
{
    for(int i = 0; i < 600; ++i)
    {
        TilePicker[i] = DATA->getLabelIndex("MATERIAL_UNINITIALIZED");
    }

    for(Uint32 i = 0; i < DATA->getNumMaterials(); ++i)
    {
        for(Uint32 j = 0; j < DATA->getMaterialData(i)->TileTypes.size(); ++j)
        {
            int Tile = DATA->getMaterialData(i)->TileTypes[j];
            TilePicker[Tile] = i;
        }
    }

    DfMap* df_map = DFExtractor->getMap();

    Uint32 NumStoneMats = df_map->getNumMatGloss(Mat_Stone);
    StoneMatGloss = new Sint16[NumStoneMats];

    for(Uint32 i = 0; i < NumStoneMats; i++)
    {
        StoneMatGloss[i] = DATA->getLabelIndex("MATERIAL_UNINITIALIZED");
    }

    for(Uint32 i = 0; i < NumStoneMats; i++)
    {
        for(Uint32 j = 0; j < DATA->getNumMaterials(); ++j)
        {
            if(strcmp(DATA->getMaterialData(j)->getMatGloss().c_str(), df_map->getMatGlossString(Mat_Stone,i).c_str()) == 0)
            {
                StoneMatGloss[i] = j;
            }
        }
    }

}

Uint32 Map::PickTexture(Uint16 MapX, Uint16 MapY, Uint16 MapZ)
{
    DfMap *df_map = DFExtractor->getMap();
    t_matglossPair mat = df_map->getMaterialPair(MapX, MapY, MapZ);
    //Sint16 StoneType = df_map->getMaterialIndex(MapX, MapY, MapZ);
    Sint16 TileType = df_map->getTileType(MapX, MapY, MapZ);

    static Uint16 Sand = DATA->getLabelIndex("MATERIAL_SAND");
    //static Uint16 Stone = DATA->getLabelIndex("MATERIAL_ROUGH_STONE");
    static Uint16 Ramp = DATA->getLabelIndex("MATERIAL_RAMP_STONE");
    //static Uint16 LavaStone = DATA->getLabelIndex("MATERIAL_OBSIDIAN");
    static Uint16 Unknown = DATA->getLabelIndex("MATERIAL_UNINITIALIZED");
    static Uint16 Vein = DATA->getLabelIndex("MATERIAL_VEIN_STONE");
    static Uint16 VeinFloor = DATA->getLabelIndex("MATERIAL_VEIN_STONE_FLOOR");
    static Uint16 Soil = DATA->getLabelIndex("MATERIAL_SOIL");
    static Uint16 Layer1 = DATA->getLabelIndex("MATERIAL_ROUGH_LAYER_STONE");
    static Uint16 Layer2 = DATA->getLabelIndex("MATERIAL_SMOOTH_LAYER_STONE");
    static Uint16 Layer3 = DATA->getLabelIndex("MATERIAL_ROUGH_STONE");

    Uint16 TileTexture = TilePicker[TileType];
    if(mat.type == Mat_Stone)
    {
        Uint16 MatGlossTexture = StoneMatGloss[mat.index];
    //    bool IsFloor = df_map->isFloorTerrain(TileType);
        //bool IsWall = df_map->isWallTerrain(TileType);
        //bool IsOpen = df_map->isOpenTerrain(TileType);
        //bool IsRamp = df_map->isRampTerrain(TileType);
        //bool IsStairs = df_map->isStairTerrain(TileType);

        // use matgloss for textures on some *very* specific tile types
        if(TileTexture == Vein ||
           TileTexture == VeinFloor ||
           TileTexture == Soil ||
           TileTexture == Sand ||
           TileTexture == Layer1 ||
           TileTexture == Layer2 ||
           TileTexture == Layer3 ||
           TileTexture == Ramp
           )
        {
            // and only if it's properly defined
            if(MatGlossTexture != Unknown)
            {
                return MatGlossTexture;
            }
        }
        // use tile texture otherwise
        if(TileType != 65535)
        {
            return TileTexture;
        }
        // fallback for undefined values of tile types and matgloss
        return Unknown;
    }
    // matgloss not loaded -> use tile texture
    else if(mat.type == 65535)
    {
        return TileTexture;
    }
    else
    {
        return Unknown;
    }
}

void Map::BuildVertexArray()
{
    float XSize = CELLEDGESIZE;
    float YSize = CELLEDGESIZE;
    float ZSize = 1.0;

    Uint16 Counter = 0;
    Uint16 ArraySize = ((Uint16)XSize + 1) * ((Uint16)YSize + 1) * ((Uint16)ZSize + 1) * 3;
    VertexArray = new float[ArraySize];

    for(Uint16 x = 0; x <= XSize; x++)
    {
        for(Uint16 y = 0; y <= YSize; y++)
        {
            for(Uint16 z = 0; z <= ZSize; z++)
            {
                VertexArray[Counter++] = x - (XSize / 2);
                VertexArray[Counter++] = y - (YSize / 2);
                VertexArray[Counter++] = z - (ZSize / 2);
            }
        }
    }
}

Vector3 Map::getMapCenter()
{
    Vector3 CenterPoint;

    CenterPoint.x = getMapSizeX() / 2;
    CenterPoint.y = getMapSizeY() / 2;
    CenterPoint.z = getMapSizeZ();

    return CenterPoint;
}

void Map::ReleaseMap()
{
    MapLoaded = false;

    if (ColumnMatrix != NULL)
    {
        for (Uint32 x = 0; x < CellSizeX; ++x)
        {
            if (ColumnMatrix[x] != NULL)
            {
                for (Uint32 y = 0; y < CellSizeY; ++y)
                {
                    if (ColumnMatrix[x][y] != NULL)
                    {
                        delete ColumnMatrix[x][y];
                    }
                }
                delete[] ColumnMatrix[x];
            }
        }
        delete[] ColumnMatrix;
        ColumnMatrix = NULL;
    }

    delete[] StoneMatGloss;
    StoneMatGloss = NULL;
}

