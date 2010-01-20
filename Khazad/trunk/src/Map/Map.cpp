#include <stdafx.h>

#include <Map.h>
#include <Singleton.h>
#include <TextureManager.h>
#include <DataManager.h>
#include <Gui.h>
#include <Extractor.h>


#include <Cell.h>
#include <Random.h>
#include <Building.h>
#include <Tree.h>
#include <Game.h>

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
    map<uint64_t, DFHack::t_construction> constructionAssigner;
    uint32_t numconstructions = DF.InitReadConstructions();
    DFHack::t_construction tempcon;
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
    map<uint64_t, DFHack::t_tree_desc> plantAssigner;
    uint32_t numtrees = DF.InitReadVegetation();
    DFHack::t_tree_desc temptree;
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
    map<uint64_t, DFHack::t_building> buildingAssigner;
    vector <string> v_buildingtypes;// FIXME: this is currently unused
    uint32_t numbuildings = DF.InitReadBuildings(v_buildingtypes);
    DFHack::t_building tempbld;
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

                    addCell(NewCell, TargetCellCoordinates);
                    EXTRACTOR->LoadCellData(DF, layerassign, NewCell, constructionAssigner, plantAssigner, buildingAssigner, TargetCellCoordinates);
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
