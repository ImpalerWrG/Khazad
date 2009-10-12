#include <stdafx.h>

#include <Map.h>
#include <Singleton.h>
#include <TextureManager.h>
#include <DataManager.h>
#include <TreeManager.h>
#include <Gui.h>

///FIXME: dfhack paths
#include "../../dfhack/library/DFTypes.h"
#include "../../dfhack/library/DFTileTypes.h"
#include "../../dfhack/library/DFHackAPI.h"
#include <string.h> // for memset

#include <Column.h>
#include <Cube.h>
//#include <Face.h>
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
    TreeMan = NULL;
}

Map::~Map()
{
    if(MapLoaded)
    {
        ReleaseMap();
    }
}

bool Map::Init()
{
    BuildVertexArray();

    Initialized = true;
    MapLoaded = false;

	return true;
}
/*
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
                        NewCube->setGeometry(GEOM_WALL);
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
}*/

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
        Sint32 CubeX = X % CELLEDGESIZE;
        Sint32 CubeY = Y % CELLEDGESIZE;
        return TargetCell->getCube(CubeX, CubeY);
    }
    return NULL;
}

bool Map::getIsInBounds(Sint32 x, Sint32 y, Sint32 z)
{
    if (x > 0 && x < MapSizeX)
    {
        if (y > 0 && y < MapSizeY)
        {
            if (z > 0 && z < MapSizeZ)
            {
                return true;
            }
        }
    }
    return false;
}

bool Map::hasFace(Sint32 X, Sint32 Y, Sint32 Z, Facet FaceType)
{
    Cell* TargetCell = getCubeOwner(X, Y, Z);

    if(TargetCell)
    {
//        if(TargetCell->isInitalized())
        {
            Sint32 CubeX = X % CELLEDGESIZE;
            Sint32 CubeY = Y % CELLEDGESIZE;

            return TargetCell->hasFace(CubeX, CubeY, FaceType);
        }
        return false;
    }
    return false;
}

bool Map::Extract()
{
    Path path_to_xml("Assets/XML/Memory.xml");
    DFHackAPI *pDF = CreateDFHackAPI(path_to_xml);
    DFHackAPI &DF = *pDF;

    if(!DF.Attach())
    {
        return false;
    }
    if(!DF.InitMap())
    {
        return false;
    }
    if(MapLoaded)
    {
        ReleaseMap();
    }
    vector< vector <uint16_t> > layerassign;
    // get region geology
    if(!DF.ReadGeology( layerassign ))
    {
        cerr << "Can't get region geology." << endl;
        return false;
    }

    InitilizeTilePicker(DF);

    // Initialize Cells and Cubes
    DF.getSize(CellSizeX,CellSizeY,CellSizeZ);
    // update depth slider
    //UI->setZSliderRange(CellSizeZ);
    //UI->setZSliders(0, CellSizeZ);

    // read constructions
    map<uint64_t, t_construction> constructionAssigner;
    uint32_t numconstructions = DF.InitReadConstructions();
    t_construction tempcon;
    uint32_t index = 0;
    while(index < numconstructions)
    {
        DF.ReadConstruction(index, tempcon);
        uint64_t coord =  ((uint64_t)tempcon.z) + (((uint64_t)tempcon.y) << 16) + (((uint64_t)tempcon.x) << 32);
        constructionAssigner[coord] = tempcon;
        index ++;
    }
    DF.FinishReadConstructions();

    // read trees
    map<uint64_t, t_tree_desc> plantAssigner;
    uint32_t numtrees = DF.InitReadVegetation();
    t_tree_desc temptree;
    index = 0;
    while(index < numtrees)
    {
        DF.ReadVegetation(index, temptree);
        uint64_t coord =  ((uint64_t)temptree.z) + (((uint64_t)temptree.y) << 16) + (((uint64_t)temptree.x) << 32);
        plantAssigner[coord] = temptree;
        index ++;
    }
    DF.FinishReadVegetation();

    // read buildings
    map<uint64_t, t_building> buildingAssigner;
    vector <string> v_buildingtypes;// FIXME: this is currently unused
    uint32_t numbuildings = DF.InitReadBuildings(v_buildingtypes);
    t_building tempbld;
    index = 0;
    while(index < numbuildings)
    {
        DF.ReadBuilding(index, tempbld);
        if(tempbld.type < v_buildingtypes.size())
        {
            string strtype = v_buildingtypes[tempbld.type];
            if( strtype == "stockpile" || strtype == "zone" ||strtype == "construction_blueprint" )
            {
                index++;
                continue;
            }
            uint64_t coord =  ((uint64_t)tempbld.z) + (((uint64_t)tempbld.y1) << 16) + (((uint64_t)tempbld.x1) << 32);
            buildingAssigner[coord] = tempbld;
        }
        else
        {
            printf ("building at %d %d %d, unknown type %d, vtable %x\n",tempbld.x1,tempbld.y1,tempbld.z,tempbld.type, tempbld.vtable);
        }
        index ++;
    }
    DF.FinishReadBuildings();

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
                if(DF.isValidBlock(x, y, z))
                {
                    Cell* NewCell = new Cell(x * CELLEDGESIZE, y * CELLEDGESIZE, z);
                    NewCell->Init();
                    LoadCellData(DF,layerassign,NewCell,constructionAssigner,plantAssigner,buildingAssigner,x,y,z);
                    ColumnMatrix[x][y]->PushCell(NewCell, z);
			    }
			}
		}
	}
    DF.Detach();

    MapSizeX = CellSizeX * CELLEDGESIZE;
	MapSizeY = CellSizeY * CELLEDGESIZE;
	MapSizeZ = CellSizeZ;
    // Initialize VBOs

	for(Uint16 Zlevel = 0; Zlevel < getCellSizeZ(); Zlevel++)
	{
        for (Uint32 SizeX = 0; SizeX < getCellSizeX(); SizeX++)
        {
            for (Uint32 SizeY = 0; SizeY < getCellSizeY(); SizeY++)
            {
                Cell* LoopCell = getCell(SizeX, SizeY, Zlevel);
                if(LoopCell != NULL && LoopCell->isActive() && LoopCell->getNeedsRedraw())
                {
                    // Rebuild the new Drawlist
                    LoopCell->UpdateLists();
                    LoopCell->setNeedsRedraw(false);
                }
            }
        }
	}
    delete pDF;
    MapLoaded = true;
    return true;
}

// load from file
bool Map::Load(string filename)
{
    //DFExtractor->loadMap(filename);
    //ReparseExtract();
    return false;
}

// save to file
void Map::Save(string filename)
{
    //DFExtractor->writeMap(filename);
    return;
}

void Map::LoadCellData(DFHackAPI & DF,
                       vector< vector <uint16_t> >& layerassign,
                       Cell* TargetCell,
                       map<uint64_t, t_construction> & constructions,
                       map<uint64_t, t_tree_desc> & vegetation,
                       map<uint64_t, t_building> & buildings, // FIXME: this is wrong for buildings. they can overlap
                       Uint32 CellX, Uint32 CellY, Uint32 CellZ)
{
    uint16_t tiletypes[16][16];
    t_designation designations[16][16];
    t_occupancy occupancies[16][16];
    uint8_t regionoffsets[16];
    int16_t basemat [16][16];
    int16_t veinmat [16][16];
    t_matglossPair constmat [16][16];
    vector <t_vein> veins;

    DF.ReadTileTypes(CellX, CellY, CellZ, (uint16_t *) tiletypes);
    DF.ReadDesignations(CellX, CellY, CellZ, (uint32_t *) designations);
    DF.ReadOccupancy(CellX, CellY, CellZ, (uint32_t *) occupancies);
    DF.ReadRegionOffsets(CellX,CellY,CellZ, regionoffsets);
    memset(basemat, -1, sizeof(basemat));
    memset(veinmat, -1, sizeof(veinmat));
    memset(constmat, -1, sizeof(constmat));
    veins.clear();
    DF.ReadVeins(CellX,CellY,CellZ,veins);

    // get the materials, buildings, trees
    for(uint32_t xx = 0;xx<16;xx++)
    {
        for (uint32_t yy = 0; yy< 16;yy++)
        {
            // base rock layers
            basemat[xx][yy] =
            layerassign
            [regionoffsets[designations[xx][yy].bits.biome]]
            [designations[xx][yy].bits.geolayer_index];

            // veins
            for(int i = 0; i < veins.size();i++)
            {
                // and the bit array with a one-bit mask, check if the bit is set
                bool set = ((1 << xx) & veins[i].assignment[yy]) >> xx;
                if(set)
                {
                    // store matgloss
                    veinmat[xx][yy] = veins[i].type;
                }
            }

            // constructions
            uint64_t coord =  (uint64_t)CellZ + ((uint64_t)(CellY*16 + yy) << 16) + ((uint64_t)(CellX*16 + xx) << 32);
            //uint64_t coord =  (uint64_t)tempcon.z + ((uint64_t)tempcon.y << 16) + ((uint64_t)tempcon.x << 32);
            if(constructions.count(coord))
            {
                // store matgloss
                constmat[xx][yy] = constructions[coord].material;
            }

            // plants
            if(vegetation.count(coord))
            {
                t_tree_desc t = vegetation[coord];
                Tree *tree = new Tree(t.material,t.x,t.y,t.z, tiletypes[xx][yy]);
                TargetCell->addTree(tree);
            }

            // buildings, FIXME: doesn't work with overlapping buildings
            if(buildings.count(coord))
            {
                t_building b = buildings[coord];
                Building *bld = new Building(b.x1,b.y1,b.x2,b.y2,b.z,b.material,b.type);
                TargetCell->addBuilding(bld);
            }
        }
    }

    static Sint16 FloorID = DATA->getLabelIndex("TILESHAPE_FLOOR");
    static Sint16 WallID = DATA->getLabelIndex("TILESHAPE_WALL");
    static Sint16 RampID = DATA->getLabelIndex("TILESHAPE_RAMP");
    static Sint16 StairID = DATA->getLabelIndex("TILESHAPE_STAIR");
    static Sint16 FortificationID = DATA->getLabelIndex("TILESHAPE_FORTIFICATION");
    static Sint16 EmptyID = DATA->getLabelIndex("TILESHAPE_EMPTY");

    for(Uint32 CubeX = 0; CubeX < CELLEDGESIZE; CubeX++)
    {
        for(Uint32 CubeY = 0; CubeY < CELLEDGESIZE; CubeY++)
        {
            Uint32 MapX = (CellX * CELLEDGESIZE) + CubeX;
            Uint32 MapY = (CellY * CELLEDGESIZE) + CubeY;
            Uint32 MapZ = CellZ;

            t_designation Designations = designations[CubeX][CubeY];
            uint16_t TileType = tiletypes[CubeX][CubeY];
            t_occupancy Ocupancies = occupancies[CubeX][CubeY];
            Sint16 TileShapeID = TileShapePicker[TileType];

            bool IsFloor = (TileShapeID == FloorID);
            bool IsWall = (TileShapeID == WallID) || (TileShapeID == FortificationID);
            bool IsEmpty = (TileShapeID == EmptyID);
            bool IsRamp = (TileShapeID == RampID);
            bool IsStairs = (TileShapeID == StairID);
            
            int Liquid = Designations.bits.flow_size;

            // Create Cubes and load data, skip empty cubes unless they have liquid in them
            if(!IsEmpty || IsEmpty && Liquid)
            {
                Cube* TargetCube = TargetCell->getCube(CubeX, CubeY);

                TargetCube = new Cube();
                TargetCell->setCube(TargetCube, CubeX, CubeY);
                TargetCube->setPosition((float) MapX, (float) MapY, (float) MapZ);

                Uint16 Material = PickTexture(TileType, basemat[CubeX][CubeY], veinmat[CubeX][CubeY], constmat[CubeX][CubeY], Ocupancies);

                TargetCube->setHidden(Designations.bits.hidden);
                TargetCube->setSubTerranean(Designations.bits.subterranean);
                TargetCube->setSkyView(Designations.bits.skyview);
                TargetCube->setSunLit(Designations.bits.light);
                
                TargetCube->setMaterial(Material);
                TargetCube->setShape(TileShapeID);

                if(Designations.bits.flow_size)
                {
                    TargetCube->setLiquid(Designations.bits.liquid_type, Designations.bits.flow_size);
                }

                TargetCube->setVisible(true);
            }
        }
    }
}

void Map::InitilizeTilePicker(DFHackAPI & DF)
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

    // Index the Shape of each tile for determining Render Geometry
    for(Uint32 i = 0; i < DATA->getNumTileGroups(); ++i)
    {
        for(Uint32 j = 0; j < DATA->getTileGroupData(i)->TileValues.size(); ++j)
        {
            int Tile = DATA->getTileGroupData(i)->TileValues[j];
            TileShapePicker[Tile] = i;

            TileShapePicker[Tile] = DATA->getTileGroupData(i)->TileShapeID[j];
        }
    }

    // FIXME: move to .h, so that it can be saved/loaded
    vector<t_matgloss> stonetypes;
    vector<t_matgloss> woodtypes;
    DF.ReadStoneMatgloss(stonetypes);
    DF.ReadWoodMatgloss(woodtypes);

    Uint32 NumStoneMats = stonetypes.size();
    int16_t uninitialized = DATA->getLabelIndex("MATERIAL_UNINITIALIZED");
    for(Uint32 i = 0; i < NumStoneMats; i++)
    {
        bool hit = 0;
        for(Uint32 j = 0; j < DATA->getNumMaterials(); ++j)
        {
            if(DATA->getMaterialData(j)->getMatGloss() == stonetypes[i].id)
            {
                StoneMatGloss.push_back(j);
                hit = 1;
            }
        }
        if(!hit)
        {
            StoneMatGloss.push_back(uninitialized);
        }
    }
    if(TreeMan) delete TreeMan;
    TreeMan = new TreeManager(woodtypes);
}
/*
string DfMap::getMaterialTypeString (uint32_t type)
{
    string ret = "";
    switch (type)
    {
        case 0:
            ret += "wood";
            break;
        case 1:
            ret += "stone/soil";
            break;
        case 2:
            ret += "metal";
            break;
        case 3:
            ret += "plant";
            break;
        case 10:
            ret += "leather";
            break;
        case 11:
            ret += "silk cloth";
            break;
        case 12:
            ret += "plant thread cloth";
            break;
        case 13: // green glass
            ret += "green glass";
            break;
        case 14: // clear glass
            ret += "clear glass";
            break;
        case 15: // crystal glass
            ret += "crystal glass";
            break;
        case 17:
            ret += "ice";
            break;
        case 18:
            ret += "charcoal";
            break;
        case 19:
            ret += "potash";
            break;
        case 20:
            ret += "ashes";
            break;
        case 21:
            ret += "pearlash";
            break;
        case 24:
            ret += "soap";
            break;
        default:
            ret += "unknown";
            break;
    }
    return ret;
}*/

//FIXME: the ugly hack
Uint32 Map::PickTexture(Sint16 TileType, Sint16 basematerial, Sint16 veinmaterial,t_matglossPair constructionmaterial, t_occupancy occupancy)
{
    /*
    // debug texture orientation on terrain
    return DATA->getLabelIndex("MATERIAL_UP");
    */
    static Uint16 Sand = DATA->getLabelIndex("MATERIAL_SAND");
    //static Uint16 Stone = DATA->getLabelIndex("MATERIAL_ROUGH_STONE");
    static Uint16 Ramp = DATA->getLabelIndex("MATERIAL_RAMP_STONE");
    //static Uint16 LavaStone = DATA->getLabelIndex("MATERIAL_OBSIDIAN");
    static Uint16 Unknown = DATA->getLabelIndex("MATERIAL_UNINITIALIZED");
    static Uint16 Vein = DATA->getLabelIndex("MATERIAL_VEIN_STONE");
    static Uint16 VeinFloor = DATA->getLabelIndex("MATERIAL_VEIN_STONE_FLOOR");
    static Uint16 Soil = DATA->getLabelIndex("MATERIAL_SOIL");
    static Uint16 Snow = DATA->getLabelIndex("MATERIAL_SNOW");
    static Uint16 Layer1 = DATA->getLabelIndex("MATERIAL_ROUGH_LAYER_STONE");
    static Uint16 Layer2 = DATA->getLabelIndex("MATERIAL_SMOOTH_LAYER_STONE");
    static Uint16 Layer3 = DATA->getLabelIndex("MATERIAL_ROUGH_STONE");
    static Uint16 ConstructedWall = DATA->getLabelIndex("MATERIAL_CONSTRUCTED_WALL");
    static Uint16 ConstructedFloor = DATA->getLabelIndex("MATERIAL_SMOOTH_FLOOR");

    static Uint16 GreenGlass = DATA->getLabelIndex("MATERIAL_GREEN_GLASS");
    static Uint16 ClearGlass = DATA->getLabelIndex("MATERIAL_CLEAR_GLASS");
    static Uint16 CrystalGlass = DATA->getLabelIndex("MATERIAL_CRYSTAL_GLASS");
    static Uint16 Ice = DATA->getLabelIndex("MATERIAL_ICE");
    static Uint16 WoodW = DATA->getLabelIndex("MATERIAL_WOOD_WALL");
    static Uint16 WoodF = DATA->getLabelIndex("MATERIAL_WOOD_FLOOR");
    // TODO: missing constructed ramps
    /*
    Mat_GreenGlass = 13,
    Mat_ClearGlass = 14,
    Mat_CrystalGlass = 15,
    Mat_Ice = 17,
    Mat_Charcoal =18,
    Mat_Potash = 20,
    Mat_Ashes = 20,
    Mat_PearlAsh = 21,
    Mat_Soap = 24,
    */
    Uint16 TileTexture = TilePicker[TileType];
    if(occupancy.bits.snow) return Snow;
    if(occupancy.bits.mud) return Soil;

    Uint16 BaseMatGlossTexture = StoneMatGloss[basematerial];
    Uint16 VeinMatGlossTexture = 0;
    //
    if( veinmaterial > 0 && veinmaterial < StoneMatGloss.size())
    {
        VeinMatGlossTexture = StoneMatGloss[veinmaterial];
    }
    else if( veinmaterial > 0 && veinmaterial >= StoneMatGloss.size())
    {
        cout << "invalid vein? " << veinmaterial << endl;
    }
    // FIXME: add more material types so that we can use soap and other such terrible things
    Uint16 ContructionMatGlossTexture = -1;
    switch(constructionmaterial.type)
    {
        case Mat_Wood:
            if(isWallTerrain(TileType))
                ContructionMatGlossTexture = WoodW;
            else
                ContructionMatGlossTexture = WoodF;
            break;
        case Mat_Stone:
            // ignore stone types for constructions for now
            ContructionMatGlossTexture = TileTexture;//StoneMatGloss[constructionmaterial.index];
            break;
        case Mat_GreenGlass:
            ContructionMatGlossTexture = GreenGlass;
            break;
        case Mat_ClearGlass:
            ContructionMatGlossTexture = ClearGlass;
            break;
        case Mat_CrystalGlass:
            ContructionMatGlossTexture = CrystalGlass;
            break;
        case Mat_Ice:
            ContructionMatGlossTexture = Ice;
            break;
        default:
            ContructionMatGlossTexture = TileTexture;
    }
    // use matgloss for veins
    if(TileTexture == Vein ||
       TileTexture == VeinFloor)
    {
        if(VeinMatGlossTexture != Unknown)
        {
            return VeinMatGlossTexture;
        }
        return TileTexture;
    }
    // soil
    else if(TileTexture == Soil)
    {
        if(BaseMatGlossTexture != Unknown)
        {
            return BaseMatGlossTexture;
        }
        return TileTexture;
    }
    else if(TileTexture == ConstructedWall ||
            TileTexture == ConstructedFloor) // ConstructedRamp, etc...
    {
        // and only if it's properly defined
        if(ContructionMatGlossTexture != Unknown)
        {
            return ContructionMatGlossTexture;
        }
        return TileTexture;
    }
    // use tile texture otherwise
    else if(TileType != -1)
    {
        return TileTexture;
    }
    // fallback for undefined values of tile types and matgloss
    return Unknown;
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
}

