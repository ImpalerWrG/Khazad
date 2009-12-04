#include <stdafx.h>

#include <Map.h>
#include <Singleton.h>
#include <TextureManager.h>
#include <DataManager.h>
#include <Gui.h>
#include <Extractor.h>

///FIXME: dfhack paths
#include "../../dfhack/library/DFTypes.h"
#include "../../dfhack/library/DFHackAPI.h"
//#include <string.h> // for memset

#include <Column.h>
#include <Cell.h>
#include <Random.h>
#include <Building.h>
#include <Tree.h>

#include <map>

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

uint64_t Map::GenerateCellKey(CellCoordinates KeyCoords) const
{
    uint64_t Key = KeyCoords.X;
    Key <<= 16;
    Key += KeyCoords.Y;
    Key <<= 16;
    Key += KeyCoords.Z;

    return Key;
}

Cell* Map::getCell(CellCoordinates TestCoords) const
{
    uint64_t Key = GenerateCellKey(TestCoords);

    if (Cells.find(Key) == Cells.end())
    {
        return NULL;
    }
    else
    {
        return Cells.find(Key)->second;
    }
}

bool Map::addCell(Cell* NewCell, CellCoordinates TargetCoordinates)
{
    if (getCell(TargetCoordinates) == NULL)
    {
        uint64_t Key = GenerateCellKey(TargetCoordinates);
        Cells[Key] = NewCell;
        return true;
    }
    return false;  // A Cell already exists at that spot
}

map<uint64_t, Cell*>* Map::getCellMap()
{
    return &Cells;
}

Cell* Map::getCubeOwner(MapCoordinates Coordinates) const
{
    if ((Coordinates.X > MapSizeX) || (Coordinates.Y > MapSizeY) || (Coordinates.Z > MapSizeZ)) //TODO better more flexible limit check
    {
        return NULL;
    }

    CellCoordinates TargetCellCoordinates = CellCoordinates(Coordinates);

    return getCell(TargetCellCoordinates);
}

bool Map::isCubeInited(MapCoordinates Coordinates) const
{
    return getCubeOwner(Coordinates) != NULL;
}

void Map::setCellNeedsReDraw(CellCoordinates Coordinates, bool NewValue)
{
    Cell* TargetCell = getCell(Coordinates);

    if (TargetCell != NULL)
    {
        TargetCell->setNeedsRedraw(NewValue);
    }
}

bool Map::Extract()
{
    if(!EXTRACTOR->Attach())
    {
        return false;
    }

    DFHack::API& DF = *EXTRACTOR->DFHack;

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

    MapSizeX = CellSizeX * CELLEDGESIZE;
    MapSizeY = CellSizeY * CELLEDGESIZE;
    MapSizeZ = CellSizeZ;

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


    CellCoordinates TargetCellCoordinates;

    for (TargetCellCoordinates.X = 0; TargetCellCoordinates.X < CellSizeX; TargetCellCoordinates.X += 1)
    {
        for (TargetCellCoordinates.Y = 0; TargetCellCoordinates.Y < CellSizeY; TargetCellCoordinates.Y += 1)
        {
            for (TargetCellCoordinates.Z = 0; TargetCellCoordinates.Z < CellSizeZ; TargetCellCoordinates.Z += 1)
            {
                if(DF.isValidBlock(TargetCellCoordinates.X, TargetCellCoordinates.Y, TargetCellCoordinates.Z))
                {
                    Cell* NewCell = new Cell();
                    NewCell->setPosition(TargetCellCoordinates);
                    LoadCellData(DF, layerassign, NewCell, constructionAssigner, plantAssigner, buildingAssigner, TargetCellCoordinates);

                    addCell(NewCell, TargetCellCoordinates);
                }
            }
        }
    }

    // Build Face Data and other Initializations, can only be done properly once all map data is loaded
    for (TargetCellCoordinates.X = 0; TargetCellCoordinates.X < CellSizeX; TargetCellCoordinates.X += 1)
    {
        for (TargetCellCoordinates.Y = 0; TargetCellCoordinates.Y < CellSizeY; TargetCellCoordinates.Y += 1)
        {
            for (TargetCellCoordinates.Z = 0; TargetCellCoordinates.Z < CellSizeZ; TargetCellCoordinates.Z += 1)
            {
                Cell* LoopCell = getCell(TargetCellCoordinates);
                if(LoopCell != NULL)
                {
                    LoopCell->Init();
                }
            }
        }
    }

    EXTRACTOR->Detach();

    MapLoaded = true;
    return true;
}

bool Map::Load(string filename)
{
    //DFExtractor->loadMap(filename);
    //ReparseExtract();
    return false;
}

void Map::Save(string filename)
{
    //DFExtractor->writeMap(filename);
    return;
}

Face* Map::getFace(MapCoordinates Coordinates, Direction DirectionType) const
{
    MapCoordinates TargetMapCoordinates = Coordinates;
    Direction TargetFace = DirectionType;

    if (isDirectionPositive(DirectionType)) // East, South and Top Directions get translated to adjacent Cells avoiding a bounce back call
    {
        // Do something for edge of Map cases??
        TargetMapCoordinates.TranslateMapCoordinates(DirectionType);
        TargetFace = OppositeDirection(TargetFace);
    }

    CellCoordinates TargetCellCoordinates = CellCoordinates(TargetMapCoordinates);
    Cell* TargetCell = getCell(TargetCellCoordinates);

    if (TargetCell != NULL)
    {
        return TargetCell->getFace(CubeCoordinates(TargetMapCoordinates), TargetFace);
    }
    return NULL;
}

bool Map::hasFace(MapCoordinates Coordinates, Direction DirectionType) const
{
    MapCoordinates TargetMapCoordinates = Coordinates;
    Direction TargetFace = DirectionType;

    if (isDirectionPositive(DirectionType)) // East, South and Top Directions get translated to adjacent Cells avoiding a bounce back call
    {
        // Do something for edge of Map cases??
        TargetMapCoordinates.TranslateMapCoordinates(DirectionType);
        TargetFace = OppositeDirection(TargetFace);
    }

    CellCoordinates TargetCellCoordinates = CellCoordinates(TargetMapCoordinates);
    Cell* TargetCell = getCell(TargetCellCoordinates);

    if (TargetCell != NULL)
    {
        return TargetCell->hasFace(CubeCoordinates(TargetMapCoordinates), TargetFace);
    }
    return false;
}

bool Map::removeFace(MapCoordinates Coordinates, Direction DirectionType)
{
    MapCoordinates TargetMapCoordinates = Coordinates;
    Direction TargetFace = DirectionType;

    if (isDirectionPositive(DirectionType)) // East, South and Top Directions get translated to adjacent Cells avoiding a bounce back call
    {
        // Do something for edge of Map cases??
        TargetMapCoordinates.TranslateMapCoordinates(DirectionType);
        TargetFace = OppositeDirection(TargetFace);
    }

    CellCoordinates TargetCellCoordinates = CellCoordinates(TargetMapCoordinates);
    Cell* TargetCell = getCell(TargetCellCoordinates);

    if (TargetCell != NULL)
    {
        return TargetCell->removeFace(CubeCoordinates(TargetMapCoordinates), TargetFace);
    }

    return false;
}

Face* Map::addFace(MapCoordinates Coordinates, Direction DirectionType)
{
    MapCoordinates TargetMapCoordinates = Coordinates;
    Direction TargetFace = DirectionType;

    if (isDirectionPositive(DirectionType)) // East, South and Top Directions get translated to adjacent Cells avoiding a bounce back call
    {
        // Do something for edge of Map cases??
        TargetMapCoordinates.TranslateMapCoordinates(DirectionType);
        TargetFace = OppositeDirection(TargetFace);
    }

    CellCoordinates TargetCellCoordinates = CellCoordinates(TargetMapCoordinates);
    Cell* TargetCell = getCell(TargetCellCoordinates);

    if (TargetCell != NULL)
    {
        return TargetCell->addFace(CubeCoordinates(TargetMapCoordinates), TargetFace);
    }

    return false;
}

bool Map::isCubeSloped(MapCoordinates Coordinates) const
{
    Cell* TargetCell = getCubeOwner(Coordinates);

    if(TargetCell != NULL)
    {
        return TargetCell->isCubeSloped(CubeCoordinates(Coordinates));
    }
    return false;
}

void Map::setCubeShape(MapCoordinates Coordinates, Sint16 TileShape)
{
    Cell* TargetCell = getCubeOwner(Coordinates);

    if (TargetCell != NULL)
    {
        TargetCell->setCubeShape(CubeCoordinates(Coordinates), TileShape);
    }
}

Sint16 Map::getCubeShape(MapCoordinates Coordinates) const
{
    Cell* TargetCell = getCubeOwner(Coordinates);

    if (TargetCell != NULL)
    {
        return TargetCell->getCubeShape(CubeCoordinates(Coordinates));
    }
    return -1;
}

void Map::setCubeMaterial(MapCoordinates Coordinates, Sint16 MaterialID)
{
    Cell* TargetCell = getCubeOwner(Coordinates);

    if (TargetCell != NULL)
    {
        TargetCell->setCubeMaterial(CubeCoordinates(Coordinates), MaterialID);
    }
}

inline Sint16 Map::getCubeMaterial(MapCoordinates Coordinates) const
{
    Cell* TargetCell = getCubeOwner(Coordinates);

    if (TargetCell != NULL)
    {
        return TargetCell->getCubeMaterial(CubeCoordinates(Coordinates));
    }
    return -1;
}

void Map::setCubeSurfaceType(MapCoordinates Coordinates, Sint16 SurfaceID)
{
    Cell* TargetCell = getCubeOwner(Coordinates);

    if (TargetCell != NULL)
    {
        TargetCell->setCubeSurface(CubeCoordinates(Coordinates), SurfaceID);
    }
}

inline Sint16 Map::getCubeSurfaceType(MapCoordinates Coordinates) const
{
    Cell* TargetCell = getCubeOwner(Coordinates);

    if (TargetCell != NULL)
    {
        return TargetCell->getCubeSurface(CubeCoordinates(Coordinates));
    }
    return -1;
}

void Map::setFaceMaterial(MapCoordinates Coordinates, Direction DirectionType, Sint16 MaterialID)
{
    Face* TargetFace = getFace(Coordinates, DirectionType);

    if (TargetFace != NULL)
    {
        TargetFace->MaterialTypeID = MaterialID;

        // TODO Set needs draw on Cell??
    }
}

inline Sint16 Map::getFaceMaterial(MapCoordinates Coordinates, Direction DirectionType) const
{
    Face* TargetFace = getFace(Coordinates, DirectionType);

    if (TargetFace != NULL)
    {
        return TargetFace->MaterialTypeID;
    }
    return -1;
}

void Map::setFaceSurfaceType(MapCoordinates Coordinates, Direction DirectionType, Sint16 SurfaceID)
{
    Face* TargetFace = getFace(Coordinates, DirectionType);

    if (TargetFace != NULL)
    {
        if (isDirectionPositive(DirectionType))
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

void Map::setBothFaceSurfaceTypes(MapCoordinates Coordinates, Direction DirectionType, Sint16 SurfaceID)
{
    Face* TargetFace = getFace(Coordinates, DirectionType);

    if (TargetFace != NULL)
    {
        TargetFace->NegativeAxisSurfaceTypeID = SurfaceID;
        TargetFace->PositiveAxisSurfaceTypeID = SurfaceID;
        // TODO Set needs draw on Cell??
    }
}

inline Sint16 Map::getFaceSurfaceType(MapCoordinates Coordinates, Direction DirectionType) const
{
    Face* TargetFace = getFace(Coordinates, DirectionType);

    if (TargetFace != NULL)
    {
        if (isDirectionPositive(DirectionType))
        {
            return TargetFace->NegativeAxisSurfaceTypeID;
        }
        else
        {
            return TargetFace->PositiveAxisSurfaceTypeID;
        }
    }
    return -1;
}

bool Map::isCubeHidden(MapCoordinates Coordinates) const
{
    Cell* TargetCell = getCubeOwner(Coordinates);

    if(TargetCell != NULL)
    {
        return TargetCell->isCubeHidden(CubeCoordinates(Coordinates));
    }
    return false;
}

void Map::setCubeHidden(MapCoordinates Coordinates, bool NewValue)
{
    Cell* TargetCell = getCubeOwner(Coordinates);

    if(TargetCell != NULL)
    {
        TargetCell->setCubeHidden(CubeCoordinates(Coordinates), NewValue);
    }
}

bool Map::isCubeSubTerranean(MapCoordinates Coordinates) const
{
    Cell* TargetCell = getCubeOwner(Coordinates);

    if(TargetCell != NULL)
    {
        return TargetCell->isCubeSubTerranean(CubeCoordinates(Coordinates));
    }
    return false;
}

void Map::setCubeSubTerranean(MapCoordinates Coordinates, bool NewValue)
{
    Cell* TargetCell = getCubeOwner(Coordinates);

    if(TargetCell != NULL)
    {
        TargetCell->setCubeSubTerranean(CubeCoordinates(Coordinates), NewValue);
    }
}

bool Map::isCubeSkyView(MapCoordinates Coordinates) const
{
    Cell* TargetCell = getCubeOwner(Coordinates);

    if(TargetCell != NULL)
    {
        return TargetCell->isCubeSkyView(CubeCoordinates(Coordinates));
    }
    return false;
}

void Map::setCubeSkyView(MapCoordinates Coordinates, bool NewValue)
{
    Cell* TargetCell = getCubeOwner(Coordinates);

    if(TargetCell != NULL)
    {
        TargetCell->setCubeSkyView(CubeCoordinates(Coordinates), NewValue);
    }
}

bool Map::isCubeSunLit(MapCoordinates Coordinates) const
{
    Cell* TargetCell = getCubeOwner(Coordinates);

    if(TargetCell != NULL)
    {
        return TargetCell->isCubeSunLit(CubeCoordinates(Coordinates));
    }
    return false;
}

void Map::setCubeSunLit(MapCoordinates Coordinates, bool NewValue)
{
    Cell* TargetCell = getCubeOwner(Coordinates);

    if(TargetCell != NULL)
    {
        TargetCell->setCubeSunLit(CubeCoordinates(Coordinates), NewValue);
    }
}

bool Map::isCubeSolid(MapCoordinates Coordinates) const
{
    Cell* TargetCell = getCubeOwner(Coordinates);

    if(TargetCell != NULL)
    {
        return TargetCell->isCubeSolid(CubeCoordinates(Coordinates));
    }
    return false;
}

void Map::setCubeSolid(MapCoordinates Coordinates, bool NewValue)
{
    Cell* TargetCell = getCubeOwner(Coordinates);

    if(TargetCell != NULL)
    {
        TargetCell->setCubeSolid(CubeCoordinates(Coordinates), NewValue);
    }
}

void Map::DigChannel(MapCoordinates Coordinates)
{
    Dig(Coordinates);
    setCubeShape(Coordinates, DATA->getLabelIndex("TILESHAPE_EMPTY"));

    // reveal tiles around
    for(Direction DirectionType = COMPASS_DIRECTIONS_START; DirectionType < NUM_COMPASS_DIRECTIONS; ++DirectionType)
    {
        setCubeHidden(MapCoordinates(Coordinates, DirectionType), false);
    }

    Dig(MapCoordinates(Coordinates, DIRECTION_DOWN));

    removeFace(Coordinates, DIRECTION_DOWN);
}

void Map::DigSlope(MapCoordinates Coordinates)
{
    static Sint16 RampID = DATA->getLabelIndex("TILESHAPE_RAMP");

    Dig(Coordinates);

    // reveal tiles around
    for(Direction DirectionType = COMPASS_DIRECTIONS_START; DirectionType < NUM_COMPASS_DIRECTIONS; ++DirectionType)
    {
        setCubeHidden(MapCoordinates(Coordinates, DirectionType), false);
    }

    DigChannel(MapCoordinates(Coordinates, DIRECTION_UP));
    setCubeShape(Coordinates, RampID);

    setCubeMaterial(Coordinates, getFaceMaterial(Coordinates, DIRECTION_DOWN));
    setCubeSurfaceType(Coordinates, getFaceSurfaceType(Coordinates, DIRECTION_DOWN));
}

void Map::Dig(MapCoordinates Coordinates)
{
    static Sint16 FloorID = DATA->getLabelIndex("TILESHAPE_FLOOR");
    static Sint16 RoughWallID = DATA->getLabelIndex("SURFACETYPE_ROUGH_WALL");
    static Sint16 RoughFloorID = DATA->getLabelIndex("SURFACETYPE_ROUGH_FLOOR_1");

    if (isCubeInited(Coordinates))
    {
        if(isCubeSolid(Coordinates) || isCubeSloped(Coordinates))
        {
            for(Direction DirectionType = AXIAL_DIRECTIONS_START; DirectionType < NUM_AXIAL_DIRECTIONS; ++DirectionType)
            {
                MapCoordinates ModifiedCoordinates = Coordinates;
                ModifiedCoordinates.TranslateMapCoordinates(DirectionType);

                if (DirectionType == DIRECTION_DOWN)
                {
                    if (MAP->isCubeSolid(ModifiedCoordinates))
                    {
                        Face* TargetFace = getFace(Coordinates, DirectionType);
                        if (TargetFace == NULL)
                        {
                            TargetFace = MAP->addFace(Coordinates, DIRECTION_DOWN);
                        }

                        TargetFace->MaterialTypeID = getCubeMaterial(ModifiedCoordinates);
                        TargetFace->NegativeAxisSurfaceTypeID = RoughFloorID;
                        TargetFace->PositiveAxisSurfaceTypeID = RoughFloorID;
                    }
                    else
                    {
                        MAP->removeFace(Coordinates, DirectionType);
                    }
                }
                else
                {
                    if (MAP->isCubeSolid(ModifiedCoordinates))
                    {
                        Face* TargetFace = getFace(Coordinates, DirectionType);
                        if (TargetFace == NULL)
                        {
                            TargetFace = MAP->addFace(Coordinates, DirectionType);
                        }

                        TargetFace->MaterialTypeID = getCubeMaterial(ModifiedCoordinates);
                        TargetFace->NegativeAxisSurfaceTypeID = getCubeSurfaceType(ModifiedCoordinates);
                        TargetFace->PositiveAxisSurfaceTypeID = getCubeSurfaceType(ModifiedCoordinates);
                    }
                    else
                    {
                        MAP->removeFace(Coordinates, DirectionType);
                    }
                }
            }

            setCubeHidden(Coordinates, false);
            setCubeShape(Coordinates, FloorID);

            MAP->setCubeMaterial(Coordinates, -1);
        }

        // reveal tiles around
        for(Direction DirectionType = COMPASS_DIRECTIONS_START; DirectionType < NUM_COMPASS_DIRECTIONS; ++DirectionType)
        {
            setCubeHidden(MapCoordinates(Coordinates, DirectionType), false);
        }
    }
}

void Map::LoadCellData(DFHack::API & DF,
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
            basemat[xx][yy] = layerassign[regionoffsets[designations[xx][yy].bits.biome]][designations[xx][yy].bits.geolayer_index];

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
            if(constructions.count(coord))
            {
                // store matgloss
                constmat[xx][yy] = constructions[coord].material;
            }

            // plants
            if(vegetation.count(coord))
            {
                t_tree_desc t = vegetation[coord];
                DFHack::TileClass Type = (DFHack::TileClass) DFHack::getVegetationType(tiletypes[xx][yy]);

                if (t.material.type == Mat_Wood)
                {
                    Sint16 TreeTypeID = ResolveMatGlossPair(t.material);

                    bool isAlive = false;

                    switch(Type)
                    {
                        case DFHack::TREE_DEAD:
                        case DFHack::SAPLING_DEAD:
                        case DFHack::SHRUB_DEAD:
                            isAlive = false;
                            break;

                        case DFHack::TREE_OK:
                        case DFHack::SAPLING_OK:
                        case DFHack::SHRUB_OK:
                            isAlive = true;
                            break;
                    }

                    Tree *tree = new Tree(TreeTypeID, MapCoordinates(t.x, t.y, t.z), isAlive);
                    TargetCell->addTree(tree);
                }
            }

            // buildings, FIXME: doesn't work with overlapping buildings
            if(buildings.count(coord))
            {
                t_building b = buildings[coord];

                for(Uint32 i = 0; i < DATA->getNumBuildings(); i++)
                {
                    if (DATA->getBuildingData(i)->getMatgloss() == b.type)
                    {
                        Sint16 MaterialID = ResolveMatGlossPair(b.material);
                        Building* NewBuilding = new Building(MapCoordinates(b.x1, b.y1, b.z), b.x2 - b.x1, b.y2 - b.y1, MaterialID, i);
                        TargetCell->addBuilding(NewBuilding);
                    }
                }
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
            TargetCell->setCubeSubTerranean(Coordinates, Designations.bits.subterranean);
            TargetCell->setCubeSkyView(Coordinates, Designations.bits.skyview);
            TargetCell->setCubeSunLit(Coordinates, Designations.bits.light);

            if(Designations.bits.flow_size)
            {
                TargetCell->setLiquid(Coordinates, Designations.bits.liquid_type, Designations.bits.flow_size);
            }
            //TargetCube->setVisible(true);
        }
    }
}

void Map::InitilizeTilePicker(DFHack::API & DF)
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
    vector<t_matgloss> planttypes;

    DF.ReadStoneMatgloss(stonetypes);
    DF.ReadPlantMatgloss(metaltypes);  // HACK, DF ReadPlantMatgloss and ReadMetalMatgloss are reversed
    DF.ReadWoodMatgloss(woodtypes);
    DF.ReadMetalMatgloss(planttypes);

    Sint16 uninitialized = -1;

    Uint32 NumStoneMats = stonetypes.size();
    for(Uint32 i = 0; i < NumStoneMats; i++)
    {
        bool Matchfound = false;
        for(Uint32 j = 0; j < DATA->getNumMaterials(); ++j)
        {
            if(DATA->getMaterialClassData(DATA->getMaterialData(j)->getMaterialClass())->getMatGlossIndex() == Mat_Stone)
            {
                if(DATA->getMaterialData(j)->getMatGloss() == stonetypes[i].id)
                {
                    StoneMatGloss.push_back(j);
                    Matchfound = true;
                    break;
                }
            }
        }
        if(!Matchfound)
        {
            StoneMatGloss.push_back(uninitialized);
        }
    }

    Uint32 NumMetalMats = metaltypes.size();
    for(Uint32 i = 0; i < NumMetalMats; i++)
    {
        bool Matchfound = false;
        for(Uint32 j = 0; j < DATA->getNumMaterials(); ++j)
        {
            if(DATA->getMaterialClassData(DATA->getMaterialData(j)->getMaterialClass())->getMatGlossIndex() == Mat_Metal)
            {
                if(DATA->getMaterialData(j)->getMatGloss() == metaltypes[i].id)
                {
                    MetalMatGloss.push_back(j);
                    Matchfound = true;
                    break;
                }
            }
        }
        if(!Matchfound)
        {
            MetalMatGloss.push_back(uninitialized);
        }
    }


    Uint32 NumTreeMats = woodtypes.size();
    for(Uint32 i = 0; i < NumTreeMats; i++)
    {
        /*
        bool Matchfound = false;
        int tree_end = DATA->getNumTrees();
        for(Uint32 j = 0; j < tree_end; ++j)
        {
            TreeData * td = DATA->getTreeData(j);
            if(td)
            if(td->getMatgloss() == woodtypes[i].id)
            {
                WoodMatGloss.push_back(j);
                Matchfound = true;
                break;
            }
        }
        if(!Matchfound)
        {*/
            WoodMatGloss.push_back(uninitialized);
        //}
    }
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

    Sint16 ConstructionMaterial = ResolveMatGlossPair(constructionmaterial);
    if(ConstructionMaterial != -1)
    {
        return ConstructionMaterial;
    }

    if (DefaultMaterial != -1 && DefaultMaterial < DATA->getNumMaterials())
    {
        return DefaultMaterial;
    }

    return Unknown;
}

Sint16 Map::ResolveMatGlossPair(t_matglossPair MatPair)
{
    static Uint16 Unknown = DATA->getLabelIndex("MATERIAL_UNINITIALIZED");

    if (MatPair.type != -1 && MatPair.index != -1)
    {
        Sint16 PotentialMaterial = -1;

        if (MatPair.type == Mat_Stone)
        {
            if (MatPair.index >= 0 && MatPair.index < StoneMatGloss.size())
            {
                PotentialMaterial = StoneMatGloss[MatPair.index];
            }
        }
        else if (MatPair.type == Mat_Metal)
        {
            if (MatPair.index >= 0 && MatPair.index < MetalMatGloss.size())
            {
                PotentialMaterial = MetalMatGloss[MatPair.index];
            }
        }
        else if (MatPair.type == Mat_Wood)
        {
            if (MatPair.index >= 0 && MatPair.index < WoodMatGloss.size())
            {
                PotentialMaterial = WoodMatGloss[MatPair.index];
            }
        }

        if(PotentialMaterial != -1)
        {
            return PotentialMaterial;
        }
        else
        {
            cerr << "material not in Materials.xml: "<< MatPair.type << "::" << MatPair.index << endl;
        }

        // Try to find a MateralClass default material
        for (Sint16 MaterialClass = 0; MaterialClass < DATA->getNumMaterialClasses(); MaterialClass++)
        {
            if (MatPair.type == DATA->getMaterialClassData(MaterialClass)->getMatGlossIndex())
            {
                PotentialMaterial = DATA->getMaterialClassData(MaterialClass)->getDefaultMaterial();
                break;
            }
        }

        if(PotentialMaterial != -1)
        {
            return PotentialMaterial;
        }
        else
        {
            cerr << "material not in Material Classes: "<< MatPair.type << "::" << MatPair.index << endl;
        }
    }

    return Unknown;
}

MapCoordinates Map::getMapCenter() const
{
    MapCoordinates CenterPoint;

    CenterPoint.X = getMapSizeX() / 2;
    CenterPoint.Y = getMapSizeY() / 2;
    CenterPoint.Z = getMapSizeZ();

    return CenterPoint;
}

void Map::ReleaseMap()
{
    MapLoaded = false;

    Cells.clear();
}
