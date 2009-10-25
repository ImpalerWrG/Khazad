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

CellCoordinates Map::TranslateMapToCell(MapCoordinates Coordinates)
{
    CellCoordinates NewCoordinates;

    NewCoordinates.X = Coordinates.X / CELLEDGESIZE;
    NewCoordinates.Y = Coordinates.Y / CELLEDGESIZE;
    NewCoordinates.Z = Coordinates.Z;

    return NewCoordinates;
}

CubeCoordinates Map::TranslateMapToCube(MapCoordinates Coordinates)
{
    CubeCoordinates NewCoordinates;

    NewCoordinates.X = Coordinates.X % CELLEDGESIZE;
    NewCoordinates.Y = Coordinates.Y % CELLEDGESIZE;

    return NewCoordinates;
}

Cell* Map::getCell(CellCoordinates Coordinates)
{
	if (Coordinates.X >= 0 && Coordinates.X < CellSizeX && Coordinates.Y >= 0 && Coordinates.Y < CellSizeY)
	{
	    if(Coordinates.Z >= ColumnMatrix[Coordinates.X][Coordinates.Y]->BottomLevel() && Coordinates.Z <= ColumnMatrix[Coordinates.X][Coordinates.Y]->TopLevel())
	    {
            return ColumnMatrix[Coordinates.X][Coordinates.Y]->getCell(Coordinates.Z);
	    }
	}
	return NULL;
}

Cell* Map::getCubeOwner(MapCoordinates Coordinates)
{
    if ((Coordinates.X > MapSizeX) || (Coordinates.Y > MapSizeY) || (Coordinates.Z > MapSizeZ)) //TODO better more flexible limit check
    {
        return NULL;
    }

    CellCoordinates TargetCellCoordinates = TranslateMapToCell(Coordinates);

    return getCell(TargetCellCoordinates);
}

void Map::setCellNeedsReDraw(CellCoordinates Coordinates)
{
    Cell* TargetCell = getCell(Coordinates);

    if (TargetCell != NULL)
    {
        TargetCell->setNeedsRedraw(true);
    }
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
    Uint32 X, Y, Z;
    DF.getSize(X, Y, Z);

    CellSizeX = X;
    CellSizeY = Y;
    CellSizeZ = Z;

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

    CellCoordinates TargetCellCoodinates;

    for (TargetCellCoodinates.X = 0; TargetCellCoodinates.X < CellSizeX; TargetCellCoodinates.X += 1)
    {
        ColumnMatrix[TargetCellCoodinates.X] = new Column*[CellSizeY];
        for (TargetCellCoodinates.Y = 0; TargetCellCoodinates.Y < CellSizeY; TargetCellCoodinates.Y += 1)
        {
            ColumnMatrix[TargetCellCoodinates.X][TargetCellCoodinates.Y] = new Column();
            ColumnMatrix[TargetCellCoodinates.X][TargetCellCoodinates.Y]->Init(TargetCellCoodinates.X, TargetCellCoodinates.Y);

            for (TargetCellCoodinates.Z = 0; TargetCellCoodinates.Z < CellSizeZ; TargetCellCoodinates.Z += 1)
            {
                if(DF.isValidBlock(TargetCellCoodinates.X, TargetCellCoodinates.Y, TargetCellCoodinates.Z))
                {
                    Cell* NewCell = new Cell();
                    NewCell->setPosition(TargetCellCoodinates);
                    LoadCellData(DF, layerassign, NewCell, constructionAssigner, plantAssigner, buildingAssigner, TargetCellCoodinates);
                    ColumnMatrix[TargetCellCoodinates.X][TargetCellCoodinates.Y]->PushCell(NewCell, TargetCellCoodinates.Z);
			    }
			}
		}
	}

    for (TargetCellCoodinates.X = 0; TargetCellCoodinates.X < CellSizeX; TargetCellCoodinates.X += 1)
    {
        for (TargetCellCoodinates.Y = 0; TargetCellCoodinates.Y < CellSizeY; TargetCellCoodinates.Y += 1)
        {
            for (TargetCellCoodinates.Z = 0; TargetCellCoodinates.Z < CellSizeZ; TargetCellCoodinates.Z += 1)
            {
                Cell* LoopCell = getCell(TargetCellCoodinates);
                if(LoopCell != NULL)
                {
                    LoopCell->Init();  // ?? still nessary
			    }
			}
		}
	}

    DF.Detach();
    delete pDF;

    // Initialize VBOs

    for (TargetCellCoodinates.X = 0; TargetCellCoodinates.X < CellSizeX; TargetCellCoodinates.X += 1)
    {
        for (TargetCellCoodinates.Y = 0; TargetCellCoodinates.Y < CellSizeY; TargetCellCoodinates.Y += 1)
        {
            for (TargetCellCoodinates.Z = 0; TargetCellCoodinates.Z < CellSizeZ; TargetCellCoodinates.Z += 1)
            {
                Cell* LoopCell = getCell(TargetCellCoodinates);
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

Face* Map::getFace(MapCoordinates Coordinates, Facet FacetType)
{
    MapCoordinates TargetMapCoordinates = Coordinates;
    Facet TargetFace = FacetType;

    if (FacetType & 1) // East, South and Top Facets get translated to adjacent Cells avoiding a bounce back call
    {
        // Do something for edge of Map cases??
        TranslateCoordinates(TargetMapCoordinates.X, TargetMapCoordinates.Y, TargetMapCoordinates.Z, FacetType);
        TargetFace = OppositeFacet(TargetFace);
    }

    CellCoordinates TargetCellCoordinates = TranslateMapToCell(TargetMapCoordinates);
    Cell* TargetCell = getCell(TargetCellCoordinates);

    if (TargetCell != NULL)
    {
        return TargetCell->getFace(TranslateMapToCube(TargetMapCoordinates), TargetFace);
    }
    return NULL;
}

bool Map::hasFace(MapCoordinates Coordinates, Facet FacetType)
{
    MapCoordinates TargetMapCoordinates = Coordinates;
    Facet TargetFace = FacetType;

    if (FacetType & 1) // East, South and Top Facets get translated to adjacent Cells avoiding a bounce back call
    {
        // Do something for edge of Map cases??
        TranslateCoordinates(TargetMapCoordinates.X, TargetMapCoordinates.Y, TargetMapCoordinates.Z, FacetType);
        TargetFace = OppositeFacet(TargetFace);
    }

    CellCoordinates TargetCellCoordinates = TranslateMapToCell(TargetMapCoordinates);
    Cell* TargetCell = getCell(TargetCellCoordinates);

    if (TargetCell != NULL)
    {
        return TargetCell->hasFace(TranslateMapToCube(TargetMapCoordinates), TargetFace);
    }
    return false;
}

bool Map::removeFace(MapCoordinates Coordinates, Facet FacetType)
{
    MapCoordinates TargetMapCoordinates = Coordinates;
    Facet TargetFace = FacetType;

    if (FacetType & 1) // East, South and Top Facets get translated to adjacent Cells avoiding a bounce back call
    {
        // Do something for edge of Map cases??
        TranslateCoordinates(TargetMapCoordinates.X, TargetMapCoordinates.Y, TargetMapCoordinates.Z, FacetType);
        TargetFace = OppositeFacet(TargetFace);
    }

    CellCoordinates TargetCellCoordinates = TranslateMapToCell(TargetMapCoordinates);
    Cell* TargetCell = getCell(TargetCellCoordinates);

    if (TargetCell != NULL)
    {
        return TargetCell->removeFace(TranslateMapToCube(TargetMapCoordinates), TargetFace);
    }

    return false;
}

Face* Map::addFace(MapCoordinates Coordinates, Facet FacetType)
{
    MapCoordinates TargetMapCoordinates = Coordinates;
    Facet TargetFace = FacetType;

    if (FacetType & 1) // East, South and Top Facets get translated to adjacent Cells avoiding a bounce back call
    {
        // Do something for edge of Map cases??
        TranslateCoordinates(TargetMapCoordinates.X, TargetMapCoordinates.Y, TargetMapCoordinates.Z, FacetType);
        TargetFace = OppositeFacet(TargetFace);
    }

    CellCoordinates TargetCellCoordinates = TranslateMapToCell(TargetMapCoordinates);
    Cell* TargetCell = getCell(TargetCellCoordinates);

    if (TargetCell != NULL)
    {
        return TargetCell->addFace(TranslateMapToCube(TargetMapCoordinates), TargetFace);
    }

    return false;
}

bool Map::isCubeSloped(MapCoordinates Coordinates)
{
    Cell* TargetCell = getCubeOwner(Coordinates);

    if(TargetCell != NULL)
    {
        return TargetCell->isCubeSloped(TranslateMapToCube(Coordinates));
    }
    return false;
}

void Map::setCubeShape(MapCoordinates Coordinates, Sint16 TileShape)
{
    Cell* TargetCell = getCubeOwner(Coordinates);

    if (TargetCell != NULL)
    {
        TargetCell->setCubeShape(TranslateMapToCube(Coordinates), TileShape);
    }
}

inline Sint16 Map::getCubeShape(MapCoordinates Coordinates)
{
    Cell* TargetCell = getCubeOwner(Coordinates);

    if (TargetCell != NULL)
    {
        return TargetCell->getCubeShape(TranslateMapToCube(Coordinates));
    }
    return -1;
}

void Map::setCubeMaterial(MapCoordinates Coordinates, Sint16 MaterialID)
{
    Cell* TargetCell = getCubeOwner(Coordinates);

    if (TargetCell != NULL)
    {
        TargetCell->setCubeMaterial(TranslateMapToCube(Coordinates), MaterialID);
    }
}

inline Sint16 Map::getCubeMaterial(MapCoordinates Coordinates)
{
    Cell* TargetCell = getCubeOwner(Coordinates);

    if (TargetCell != NULL)
    {
        return TargetCell->getCubeMaterial(TranslateMapToCube(Coordinates));
    }
    return -1;
}

void Map::setCubeSurfaceType(MapCoordinates Coordinates, Sint16 SurfaceID)
{
    Cell* TargetCell = getCubeOwner(Coordinates);

    if (TargetCell != NULL)
    {
        TargetCell->setCubeSurface(TranslateMapToCube(Coordinates), SurfaceID);
    }
}

inline Sint16 Map::getCubeSurfaceType(MapCoordinates Coordinates)
{
    Cell* TargetCell = getCubeOwner(Coordinates);

    if (TargetCell != NULL)
    {
        return TargetCell->getCubeSurface(TranslateMapToCube(Coordinates));
    }
    return -1;
}

void Map::setFaceMaterial(MapCoordinates Coordinates, Facet FacetType, Sint16 MaterialID)
{
    Face* TargetFace = getFace(Coordinates, FacetType);

    if (TargetFace != NULL)
    {
        TargetFace->MaterialTypeID = MaterialID;

        // TODO Set needs draw on Cell??
    }
}

inline Sint16 Map::getFaceMaterial(MapCoordinates Coordinates, Facet FacetType)
{
    Face* TargetFace = getFace(Coordinates, FacetType);

    if (TargetFace != NULL)
    {
        return TargetFace->MaterialTypeID;

        // TODO Set needs draw on Cell??
    }
    return -1;
}

void Map::setFaceSurfaceType(MapCoordinates Coordinates, Facet FacetType, Sint16 SurfaceID)
{
    Face* TargetFace = getFace(Coordinates, FacetType);

    if (TargetFace != NULL)
    {
        if (FacetType & 1)
        {
            TargetFace->NegativeAxisSurfaceTypeID = SurfaceID;
        }
        else
        {
            TargetFace->PositiveAxisSurfaceTypeID = SurfaceID;
        }
        // TODO Set needs draw on Cell??
    }
}

inline Sint16 Map::getFaceSurfaceType(MapCoordinates Coordinates, Facet FacetType)
{
    Face* TargetFace = getFace(Coordinates, FacetType);

    if (TargetFace != NULL)
    {
        if (FacetType & 1)
        {
            return TargetFace->NegativeAxisSurfaceTypeID;
        }
        else
        {
            return TargetFace->PositiveAxisSurfaceTypeID;
        }
        // TODO Set needs draw on Cell??
    }
    return -1;
}

bool Map::isCubeHidden(MapCoordinates Coordinates)
{
    Cell* TargetCell = getCubeOwner(Coordinates);

    if(TargetCell != NULL)
    {
        return TargetCell->isCubeHidden(TranslateMapToCube(Coordinates));
    }
    return false;
}

void Map::setCubeHidden(MapCoordinates Coordinates, bool NewValue)
{
    Cell* TargetCell = getCubeOwner(Coordinates);

    if(TargetCell != NULL)
    {
        TargetCell->setCubeHidden(TranslateMapToCube(Coordinates), NewValue);
    }
}

bool Map::isCubeSubTerranean(MapCoordinates Coordinates)
{
    Cell* TargetCell = getCubeOwner(Coordinates);

    if(TargetCell != NULL)
    {
        return TargetCell->isCubeSubTerranean(TranslateMapToCube(Coordinates));
    }
    return false;
}

void Map::setCubeSubTerranean(MapCoordinates Coordinates, bool NewValue)
{
    Cell* TargetCell = getCubeOwner(Coordinates);

    if(TargetCell != NULL)
    {
        TargetCell->setCubeSubTerranean(TranslateMapToCube(Coordinates), NewValue);
    }
}

bool Map::isCubeSkyView(MapCoordinates Coordinates)
{
    Cell* TargetCell = getCubeOwner(Coordinates);

    if(TargetCell != NULL)
    {
        return TargetCell->isCubeSkyView(TranslateMapToCube(Coordinates));
    }
    return false;
}

void Map::setCubeSkyView(MapCoordinates Coordinates, bool NewValue)
{
    Cell* TargetCell = getCubeOwner(Coordinates);

    if(TargetCell != NULL)
    {
        TargetCell->setCubeSkyView(TranslateMapToCube(Coordinates), NewValue);
    }
}

bool Map::isCubeSunLit(MapCoordinates Coordinates)
{
    Cell* TargetCell = getCubeOwner(Coordinates);

    if(TargetCell != NULL)
    {
        return TargetCell->isCubeSunLit(TranslateMapToCube(Coordinates));
    }
    return false;
}

void Map::setCubeSunLit(MapCoordinates Coordinates, bool NewValue)
{
    Cell* TargetCell = getCubeOwner(Coordinates);

    if(TargetCell != NULL)
    {
        TargetCell->setCubeSunLit(TranslateMapToCube(Coordinates), NewValue);
    }
}

bool Map::isCubeSolid(MapCoordinates Coordinates)
{
    Cell* TargetCell = getCubeOwner(Coordinates);

    if(TargetCell != NULL)
    {
        return TargetCell->isCubeSolid(TranslateMapToCube(Coordinates));
    }
    return false;
}

void Map::setCubeSolid(MapCoordinates Coordinates, bool NewValue)
{
    Cell* TargetCell = getCubeOwner(Coordinates);

    if(TargetCell != NULL)
    {
        TargetCell->setCubeSolid(TranslateMapToCube(Coordinates), NewValue);
    }
}

void Map::DigChannel(MapCoordinates Coordinates)
{
    setCubeHidden(Coordinates, true);
    setCubeShape(Coordinates, DATA->getLabelIndex("TILESHAPE_EMPTY"));

    // reveal tiles around, deig below
    for(Direction DirectionType = DIRECTIONS_START; DirectionType < NUM_DIRECTIONS; ++DirectionType)
    {
        MapCoordinates ModifiedCoordinates = Coordinates;
        TranslateCoordinates(ModifiedCoordinates.X, ModifiedCoordinates.Y, ModifiedCoordinates.Z, DirectionType);

        if(DirectionType != DIRECTION_DOWN && DirectionType != DIRECTION_UP)
        {
            setCubeHidden(ModifiedCoordinates, false);
        }
        if (DirectionType == DIRECTION_DOWN)
        {
            Dig(ModifiedCoordinates);
        }
    }
}

void Map::DigSlope(MapCoordinates Coordinates)
{
    static Sint16 FloorID = DATA->getLabelIndex("TILESHAPE_FLOOR");
    static Sint16 RampID = DATA->getLabelIndex("TILESHAPE_RAMP");
    static Sint16 EmptyID = DATA->getLabelIndex("TILESHAPE_EMPTY");

    if (isCubeSolid(Coordinates))
    {
        setCubeHidden(Coordinates, false);
        setCubeShape(Coordinates, FloorID);

        // reveal tiles around
        for(Direction DirectionType = DIRECTIONS_START; DirectionType < NUM_DIRECTIONS; ++DirectionType)
        {
            MapCoordinates ModifiedCoordinates = Coordinates;
            TranslateCoordinates(ModifiedCoordinates.X, ModifiedCoordinates.Y, ModifiedCoordinates.Z, DirectionType);

            if(DirectionType != DIRECTION_DOWN && DirectionType != DIRECTION_UP)
            {
                setCubeHidden(ModifiedCoordinates, false);
            }
            if(DirectionType == DIRECTION_UP)
            {
                DigChannel(ModifiedCoordinates);
            }
            if(DirectionType == DIRECTION_DOWN)
            {
                DigSlope(ModifiedCoordinates);
            }
        }
    }
    //else if (getCubeShape(Coordinates) != EmptyID)
    //{
        //Cube* NeighborCube = getNeighborCube(DIRECTION_DOWN);
        //if(NeighborCube)
        //{
        //    NeighborCube->
        //}
    //}
}

void Map::Dig(MapCoordinates Coordinates)
{
    static Sint16 FloorID = DATA->getLabelIndex("TILESHAPE_FLOOR");

    if(isCubeSolid(Coordinates) || isCubeSloped(Coordinates))
    {
        setCubeHidden(Coordinates, false);
        setCubeShape(Coordinates, FloorID);

        // reveal tiles around
        for(Direction DirectionType = DIRECTIONS_START; DirectionType < NUM_DIRECTIONS; ++DirectionType)
        {
            MapCoordinates ModifiedCoordinates = Coordinates;
            TranslateCoordinates(ModifiedCoordinates.X, ModifiedCoordinates.Y, ModifiedCoordinates.Z, DirectionType);

            if(DirectionType != DIRECTION_DOWN && DirectionType != DIRECTION_UP)
            {
                setCubeHidden(ModifiedCoordinates, false);
            }
        }
    }
}

void Map::LoadCellData(DFHackAPI & DF,
                       vector< vector <uint16_t> >& layerassign,
                       Cell* TargetCell,
                       map<uint64_t, t_construction> & constructions,
                       map<uint64_t, t_tree_desc> & vegetation,
                       map<uint64_t, t_building> & buildings, // FIXME: this is wrong for buildings. they can overlap
                       CellCoordinates NewCellCoordinates)
{
    uint16_t tiletypes[16][16];
    t_designation designations[16][16];
    t_occupancy occupancies[16][16];
    uint8_t regionoffsets[16];
    int16_t basemat [16][16];
    int16_t veinmat [16][16];
    t_matglossPair constmat [16][16];
    vector <t_vein> veins;

    DF.ReadTileTypes(NewCellCoordinates.X, NewCellCoordinates.Y, NewCellCoordinates.Z, (uint16_t *) tiletypes);
    DF.ReadDesignations(NewCellCoordinates.X, NewCellCoordinates.Y, NewCellCoordinates.Z, (uint32_t *) designations);
    DF.ReadOccupancy(NewCellCoordinates.X, NewCellCoordinates.Y, NewCellCoordinates.Z, (uint32_t *) occupancies);
    DF.ReadRegionOffsets(NewCellCoordinates.X, NewCellCoordinates.Y, NewCellCoordinates.Z, regionoffsets);

    memset(basemat, -1, sizeof(basemat));
    memset(veinmat, -1, sizeof(veinmat));
    memset(constmat, -1, sizeof(constmat));
    veins.clear();
    DF.ReadVeins(NewCellCoordinates.X, NewCellCoordinates.Y, NewCellCoordinates.Z, veins);

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
            uint64_t coord =  (uint64_t)NewCellCoordinates.Z + ((uint64_t)(NewCellCoordinates.Y *16 + yy) << 16) + ((uint64_t)(NewCellCoordinates.X *16 + xx) << 32);
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

    CubeCoordinates Coordinates;

    for (Coordinates.X = 0; Coordinates.X < CELLEDGESIZE; Coordinates.X += 1)
    {
        for (Coordinates.Y = 0; Coordinates.Y < CELLEDGESIZE; Coordinates.Y += 1)
        {
            t_designation Designations = designations[Coordinates.X][Coordinates.Y];
            Uint16 TileType = tiletypes[Coordinates.X][Coordinates.Y];
            t_occupancy Ocupancies = occupancies[Coordinates.X][Coordinates.Y];

            Sint16 TileShapeID = TileShapePicker[TileType];
            Sint16 TileSurfaceID = TileSurfacePicker[TileType];
            Sint16 TileMaterialID = PickMaterial(TileType, basemat[Coordinates.X][Coordinates.Y], veinmat[Coordinates.X][Coordinates.Y], constmat[Coordinates.X][Coordinates.Y], Ocupancies);

            TargetCell->setCubeShape(Coordinates, TileShapeID);
            TargetCell->setCubeSurface(Coordinates, TileSurfaceID);
            TargetCell->setCubeMaterial(Coordinates, TileMaterialID);

            TargetCell->setCubeHidden(Coordinates, Designations.bits.hidden);
            TargetCell->setCubeSubTerranean(Coordinates, Designations.bits.hidden);
            TargetCell->setCubeSkyView(Coordinates, Designations.bits.hidden);
            TargetCell->setCubeSunLit(Coordinates, Designations.bits.hidden);

            if(Designations.bits.flow_size)
            {
                TargetCell->setLiquid(Coordinates, Designations.bits.liquid_type, Designations.bits.flow_size);
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

