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
    FaceCount = 0;

    InitedCellCount = 0;

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
    Initialized = true;
    MapLoaded = false;

	return true;
}

bool Map::Generate(Uint32 Seed)
{
    RANDOM->Seed(Seed);

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

    return getCell(X / CELLEDGESIZE, Y / CELLEDGESIZE, Z);
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
    DF.getSize(CellSizeX, CellSizeY, CellSizeZ);
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

    MapSizeX = CellSizeX * CELLEDGESIZE;
	MapSizeY = CellSizeY * CELLEDGESIZE;
	MapSizeZ = CellSizeZ;

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
                    Cell* NewCell = new Cell();
                    NewCell->setPosition(x * CELLEDGESIZE, y * CELLEDGESIZE, z);
                    LoadCellData(DF, layerassign, NewCell, constructionAssigner, plantAssigner, buildingAssigner, x, y, z);
                    ColumnMatrix[x][y]->PushCell(NewCell, z);
			    }
			}
		}
	}

	for(Uint16 Zlevel = 0; Zlevel < getCellSizeZ(); Zlevel++)
	{
        for (Uint32 SizeX = 0; SizeX < getCellSizeX(); SizeX++)
        {
            for (Uint32 SizeY = 0; SizeY < getCellSizeY(); SizeY++)
            {
                Cell* LoopCell = getCell(SizeX, SizeY, Zlevel);
                if(LoopCell != NULL)
                {
                    LoopCell->Init();
			    }
			}
		}
	}

    DF.Detach();

    // Initialize VBOs

	for(Uint16 Zlevel = 0; Zlevel < getCellSizeZ(); Zlevel++)
	{
        for (Uint32 SizeX = 0; SizeX < getCellSizeX(); SizeX++)
        {
            for (Uint32 SizeY = 0; SizeY < getCellSizeY(); SizeY++)
            {
                Cell* LoopCell = getCell(SizeX, SizeY, Zlevel);
                if(LoopCell != NULL && /*LoopCell->isActive() &&*/ LoopCell->getNeedsRedraw())
                {
                    // Rebuild the new Drawlist
                    LoopCell->BuildFaceData();
                    LoopCell->UpdateRenderLists();
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

Face* Map::getFace(Sint32 MapX, Sint32 MapY, Sint32 MapZ, Facet FacetType)
{
    return NULL;
}

bool Map::hasFace(Sint32 MapX, Sint32 MapY, Sint32 MapZ, Facet FacetType)
{
    return false;
}

bool Map::removeFace(Sint32 MapX, Sint32 MapY, Sint32 MapZ, Facet FacetType)
{
    return true;
}

Face* Map::addFace(Sint32 MapX, Sint32 MapY, Sint32 MapZ, Facet FacetType)
{
    return NULL;
}

bool Map::isCubeSloped(Sint32 MapX, Sint32 MapY, Sint32 MapZ)
{
    Cell* TargetCell = getCubeOwner(MapX, MapY, MapZ);

    if(TargetCell != NULL)
    {
        return TargetCell->isCubeSloped(MapX % CELLEDGESIZE, MapY % CELLEDGESIZE);
    }
    return false;
}

void Map::setCubeShape(Sint32 MapX, Sint32 MapY, Sint32 MapZ, Sint16 TileShape)
{

}

inline Sint16 Map::getCubeShape(Sint32 MapX, Sint32 MapY, Sint32 MapZ)
{
    return -1;
}

void Map::setCubeMaterial(Sint32 MapX, Sint32 MapY, Sint32 MapZ, Sint16 MaterialID)
{

}

inline Sint16 Map::getCubeMaterial(Sint32 MapX, Sint32 MapY, Sint32 MapZ)
{
    return -1;
}

void Map::setCubeSurfaceType(Sint32 MapX, Sint32 MapY, Sint32 MapZ, Sint16 SurfaceID)
{
}

inline Sint16 Map::getCubeSurfaceType(Sint32 MapX, Sint32 MapY, Sint32 MapZ)
{
    return -1;
}

void Map::setFaceMaterial(Sint32 MapX, Sint32 MapY, Sint32 MapZ, Facet FacetType, Sint16 MaterialID)
{
}

inline Sint16 Map::getFaceMaterial(Sint32 MapX, Sint32 MapY, Sint32 MapZ, Facet FacetType)
{
    return -1;
}

void Map::setFaceSurfaceType(Sint32 MapX, Sint32 MapY, Sint32 MapZ, Facet FacetType, Sint16 SurfaceID)
{
}

inline Sint16 Map::getFaceSurfaceType(Sint32 MapX, Sint32 MapY, Sint32 MapZ, Facet FacetType)
{
    return -1;
}

bool Map::isCubeHidden(Sint32 MapX, Sint32 MapY, Sint32 MapZ)
{
    Cell* TargetCell = getCubeOwner(MapX, MapY, MapZ);

    if(TargetCell != NULL)
    {
        return TargetCell->isCubeHidden(MapX % CELLEDGESIZE, MapY % CELLEDGESIZE);
    }
    return false;
}

void Map::setCubeHidden(Sint32 MapX, Sint32 MapY, Sint32 MapZ, bool NewValue)
{
}

bool Map::isCubeSubTerranean(Sint32 MapX, Sint32 MapY, Sint32 MapZ)
{
    Cell* TargetCell = getCubeOwner(MapX, MapY, MapZ);

    if(TargetCell != NULL)
    {
        return TargetCell->isCubeSubTerranean(MapX % CELLEDGESIZE, MapY % CELLEDGESIZE);
    }
    return false;
}

void Map::setCubeSubTerranean(Sint32 MapX, Sint32 MapY, Sint32 MapZ, bool NewValue)
{
}

bool Map::isCubeSkyView(Sint32 MapX, Sint32 MapY, Sint32 MapZ)
{
    Cell* TargetCell = getCubeOwner(MapX, MapY, MapZ);

    if(TargetCell != NULL)
    {
        return TargetCell->isCubeSkyView(MapX % CELLEDGESIZE, MapY % CELLEDGESIZE);
    }
    return false;
}

void Map::setCubeSkyView(Sint32 MapX, Sint32 MapY, Sint32 MapZ, bool NewValue)
{
}

bool Map::isCubeSunLit(Sint32 MapX, Sint32 MapY, Sint32 MapZ)
{
    Cell* TargetCell = getCubeOwner(MapX, MapY, MapZ);

    if(TargetCell != NULL)
    {
        return TargetCell->isCubeSunLit(MapX % CELLEDGESIZE, MapY % CELLEDGESIZE);
    }
    return false;
}

void Map::setCubeSunLit(Sint32 MapX, Sint32 MapY, Sint32 MapZ, bool NewValue)
{
}

bool Map::isCubeSolid(Sint32 MapX, Sint32 MapY, Sint32 MapZ)
{
    Cell* TargetCell = getCubeOwner(MapX, MapY, MapZ);

    if(TargetCell != NULL)
    {
        return TargetCell->isCubeSolid(MapX % CELLEDGESIZE, MapY % CELLEDGESIZE);
    }
    return false;
}

void Map::setCubeSolid(Sint32 MapX, Sint32 MapY, Sint32 MapZ, bool NewValue)
{
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
    for(uint32_t xx = 0; xx < 16; xx++)
    {
        for (uint32_t yy = 0; yy < 16; yy++)
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

    for(Uint32 CubeX = 0; CubeX < CELLEDGESIZE; CubeX++)
    {
        for(Uint32 CubeY = 0; CubeY < CELLEDGESIZE; CubeY++)
        {
            t_designation Designations = designations[CubeX][CubeY];
            Uint16 TileType = tiletypes[CubeX][CubeY];
            t_occupancy Ocupancies = occupancies[CubeX][CubeY];

            Sint16 TileShapeID = TileShapePicker[TileType];
            Sint16 TileSurfaceID = TileSurfacePicker[TileType];
            Sint16 TileMaterialID = PickMaterial(TileType, basemat[CubeX][CubeY], veinmat[CubeX][CubeY], constmat[CubeX][CubeY], Ocupancies);

            TargetCell->setCubeShape(CubeX, CubeY, TileShapeID);
            TargetCell->setCubeSurface(CubeX, CubeY, TileSurfaceID);
            TargetCell->setCubeMaterial(CubeX, CubeY, TileMaterialID);

            TargetCell->setCubeHidden(CubeX, CubeY, Designations.bits.hidden);
            TargetCell->setCubeSubTerranean(CubeX, CubeY, Designations.bits.hidden);
            TargetCell->setCubeSkyView(CubeX, CubeY, Designations.bits.hidden);
            TargetCell->setCubeSunLit(CubeX, CubeY, Designations.bits.hidden);

            if(Designations.bits.flow_size)
            {
                TargetCell->setLiquid(CubeX, CubeY, Designations.bits.liquid_type, Designations.bits.flow_size);
            }
            //TargetCube->setVisible(true);
        }
    }
}

void Map::InitilizeTilePicker(DFHackAPI & DF)
{
    for(int i = 0; i < 600; ++i)  // Exact number of possible DF tile types isn't know
    {
        TileShapePicker[i] = -1;
        TileSurfacePicker[i] = -1;
        TileMaterialPicker[i] = -1;
        TileMaterialClassPicker[i] = -1;
    }

    // Index the Shape, Surface and Material values of each tile
    for(Uint32 i = 0; i < DATA->getNumTileGroups(); ++i)
    {
        for(Uint32 j = 0; j < DATA->getTileGroupData(i)->TileValues.size(); ++j)
        {
            int Tile = DATA->getTileGroupData(i)->TileValues[j];

            TileShapePicker[Tile] = DATA->getTileGroupData(i)->TileShapeID[j];
            TileSurfacePicker[Tile] = DATA->getTileGroupData(i)->SurfaceTypeID[j];

            TileMaterialPicker[Tile] = DATA->getTileGroupData(i)->getMaterialID();
            TileMaterialClassPicker[Tile] = DATA->getTileGroupData(i)->getMaterialClassID();
        }
    }

    // FIXME: move to .h, so that it can be saved/loaded
    vector<t_matgloss> stonetypes;
    vector<t_matgloss> metaltypes;
    vector<t_matgloss> woodtypes;

    DF.ReadStoneMatgloss(stonetypes);
    DF.ReadMetalMatgloss(metaltypes);
    DF.ReadWoodMatgloss(woodtypes);

    Uint32 NumStoneMats = stonetypes.size();
    int16_t uninitialized = -1;

    for(Uint32 i = 0; i < NumStoneMats; i++)
    {
        bool hit = false;
        for(Uint32 j = 0; j < DATA->getNumMaterials(); ++j)
        {
            if(DATA->getMaterialData(j)->getMatGloss() == stonetypes[i].id)
            {
                StoneMatGloss.push_back(j);
                hit = true;
            }
        }
        if(!hit)
        {
            StoneMatGloss.push_back(uninitialized);
        }
    }
    
    Uint32 NumMetalMats = metaltypes.size();
    for(Uint32 i = 0; i < NumMetalMats; i++)
    {
        bool hit = false;
        for(Uint32 j = 0; j < DATA->getNumMaterials(); ++j)
        {
            if(DATA->getMaterialData(j)->getMatGloss() == metaltypes[i].id)
            {
                MetalMatGloss.push_back(j);
                hit = true;
            }
        }
        if(!hit)
        {
            MetalMatGloss.push_back(uninitialized);
        }
    }

    if(TreeMan)
    {
        delete TreeMan;
    }
    TreeMan = new TreeManager(woodtypes);
}

//FIXME: the ugly hack
Sint16 Map::PickMaterial(Sint16 TileType, Sint16 basematerial, Sint16 veinmaterial, t_matglossPair constructionmaterial, t_occupancy occupancy)
{
    static Uint16 Unknown = DATA->getLabelIndex("MATERIAL_UNINITIALIZED");
    static Uint16 LayerStone = DATA->getLabelIndex("MATERIALCLASS_LAYER_STONE");
    static Uint16 Soil = DATA->getLabelIndex("MATERIALCLASS_SOIL");
    static Uint16 VeinStone = DATA->getLabelIndex("MATERIALCLASS_VEIN_STONE");

    //return Unknown;

    Sint16 TileMaterial = TileMaterialPicker[TileType];
    if (TileMaterial != -1 && TileMaterial < DATA->getNumMaterials())
    {
        return TileMaterial;
    }

    Sint16 TileMaterialClass = TileMaterialClassPicker[TileType];
    Sint16 DefaultMaterial = -1;
    if(TileMaterialClass != -1)
    {
        DefaultMaterial = DATA->getMaterialClassData(TileMaterialClass)->getDefaultMaterial();
    }

    if ( TileMaterialClass == LayerStone || TileMaterialClass == Soil )
    {
        return StoneMatGloss[basematerial];
    }
    else if (TileMaterialClass == VeinStone)
    {
        if (veinmaterial >= 0 && veinmaterial < StoneMatGloss.size() && StoneMatGloss[veinmaterial] != -1)
        {
            return StoneMatGloss[veinmaterial];
        }
        else // Probably a Modded material
        {
            cerr << "bad or unknown vein matgloss:" << veinmaterial << endl;
            if(DefaultMaterial != -1 && DefaultMaterial < DATA->getNumMaterials())
            {
                return DefaultMaterial;
            }
        }
    }

    if (constructionmaterial.type != -1)
    {
        int16_t construction_ret = -1;
        if (constructionmaterial.type == Mat_Stone)
        {
            if (constructionmaterial.index >= 0 && constructionmaterial.index < StoneMatGloss.size())
            {
                construction_ret = StoneMatGloss[constructionmaterial.index];
                //return StoneMatGloss[constructionmaterial.index];
            }
        }
        else if (constructionmaterial.type == Mat_Metal)
        {
            if (constructionmaterial.index >= 0 && constructionmaterial.index < MetalMatGloss.size())
            {
                construction_ret = MetalMatGloss[constructionmaterial.index];
                //return MetalMatGloss[constructionmaterial.index];
            }
        }
        
        if(construction_ret != -1)
        {
            return construction_ret;
        }
        else
        {
            cerr << "construction material not in Materials.xml: "
                 << constructionmaterial.type 
                 << "::" 
                 << constructionmaterial.index 
                 << endl;
        }
        
        for (Sint16 MaterialClass = 0; MaterialClass < DATA->getNumMaterialClasses(); MaterialClass++)
        {
            if (constructionmaterial.type == DATA->getMaterialClassData(MaterialClass)->getMatGlossIndex())
            {
                construction_ret = DATA->getMaterialClassData(MaterialClass)->getDefaultMaterial();
                //return DATA->getMaterialClassData(MaterialClass)->getDefaultMaterial();
            }
        }
        
        if(construction_ret != -1)
        {
            return construction_ret;
        }
        else
        {
            cerr << "construction material not in Material Classes: "
            << constructionmaterial.type 
            << "::" 
            << constructionmaterial.index 
            << endl;
        }
    }

    if (DefaultMaterial != -1 && DefaultMaterial < DATA->getNumMaterials())
    {
        return DefaultMaterial;
    }

    return Unknown;
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

