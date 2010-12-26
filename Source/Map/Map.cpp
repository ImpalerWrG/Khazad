/* Copyright 2010 Kenneth Ferland

This file is part of Khazad.

Khazad is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Khazad is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Khazad.  If not, see <http://www.gnu.org/licenses/> */


#include <Map.h>


#include <DataManager.h>
#include <Renderer.h>

#include <Geology.h>
#include <Cell.h>
#include <Face.h>
#include <Zone.h>
#include <TileShapes.h>


Map::Map()
{
    Initialized = false;
    CellNeedsRebuild = true;

    FaceCount = 0;

    HighestCell = 0;
    LowestCell = 0;

    CreateAllEntities();

    ActiveZone = NULL;
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

Cell* Map::getCell(CellCoordinates TestCoords) const
{
    std::map<uint64_t, Cell*>::const_iterator SearchResult = Cells.find(TestCoords.Key());

    return SearchResult == Cells.end() ? NULL : SearchResult->second;
}

void Map::RefreshCellGeometry()
{
    if (CellNeedsRebuild)
    {
        for (std::map<uint64_t, Cell*>::const_iterator it = Cells.begin(); it != Cells.end(); it++)
        {
            if (it->second->getNeedsReBuild())
            {
                it->second->BuildStaticGeometry();
            }
        }
    }
    setNeedsReBuild(false);
}

bool Map::insertCell(Cell* NewCell, CellCoordinates TargetCoordinates)
{
    if (getCell(TargetCoordinates) == NULL)
    {
        Cells[TargetCoordinates.Key()] = NewCell;

        if (TargetCoordinates.Z > HighestCell)
        {
            HighestCell = TargetCoordinates.Z;
        }
        if (TargetCoordinates.Z < LowestCell)
        {
            LowestCell = TargetCoordinates.Z;
        }

        return true;
    }
    return false;  // A Cell already exists at that spot
}

std::map<uint64_t, Cell*>* Map::getCellMap()
{
    return &Cells;
}

Cell* Map::getCubeOwner(MapCoordinates Coordinates) const
{
    CellCoordinates TargetCellCoordinates = CellCoordinates(Coordinates);

    return getCell(TargetCellCoordinates);
}

bool Map::isCubeInited(MapCoordinates Coordinates) const
{
    return getCubeOwner(Coordinates) != NULL;
}

void Map::setCellNeedsReBuild(CellCoordinates Coordinates, bool NewValue)
{
    Cell* TargetCell = getCell(Coordinates);

    if (TargetCell != NULL)
    {
        TargetCell->setNeedsReBuild(NewValue);

        if (NewValue)
        {
            setNeedsReBuild(true);
        }
    }
}

bool Map::Load(string filename)
{
    return false;
}

void Map::Save(string filename)
{
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
    else
    {
        Cell* NewCell = new Cell();
        NewCell->setCellPosition(TargetCellCoordinates);

        insertCell(NewCell, TargetCellCoordinates);
        //NewCell->LoadCellData(MapGeology);

        return NewCell->addFace(CubeCoordinates(TargetMapCoordinates), TargetFace);
    }

    return NULL;
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

void Map::setCubeShape(MapCoordinates Coordinates, TileShape NewShape)
{
    Cell* TargetCell = getCubeOwner(Coordinates);

    if (TargetCell != NULL)
    {
        TargetCell->setCubeShape(CubeCoordinates(Coordinates), NewShape);
    }
}

TileShape Map::getCubeShape(MapCoordinates Coordinates) const
{
    Cell* TargetCell = getCubeOwner(Coordinates);

    if (TargetCell != NULL)
    {
        return TargetCell->getCubeShape(CubeCoordinates(Coordinates));
    }
    return NUM_TILESHAPES;
}

void Map::setCubeMaterial(MapCoordinates Coordinates, int16_t MaterialID)
{
    Cell* TargetCell = getCubeOwner(Coordinates);

    if (TargetCell != NULL)
    {
        TargetCell->setCubeMaterial(CubeCoordinates(Coordinates), MaterialID);
    }
}

inline int16_t Map::getCubeMaterial(MapCoordinates Coordinates) const
{
    Cell* TargetCell = getCubeOwner(Coordinates);

    if (TargetCell != NULL)
    {
        return TargetCell->getCubeMaterial(CubeCoordinates(Coordinates));
    }
    return INVALID_INDEX;
}

void Map::setCubeSurfaceType(MapCoordinates Coordinates, int16_t SurfaceID)
{
    Cell* TargetCell = getCubeOwner(Coordinates);

    if (TargetCell != NULL)
    {
        TargetCell->setCubeSurface(CubeCoordinates(Coordinates), SurfaceID);
    }
}

inline int16_t Map::getCubeSurfaceType(MapCoordinates Coordinates) const
{
    Cell* TargetCell = getCubeOwner(Coordinates);

    if (TargetCell != NULL)
    {
        return TargetCell->getCubeSurface(CubeCoordinates(Coordinates));
    }
    return INVALID_INDEX;
}

void Map::setFaceMaterial(MapCoordinates Coordinates, Direction DirectionType, int16_t MaterialID)
{
    Face* TargetFace = getFace(Coordinates, DirectionType);

    if (TargetFace != NULL)
    {
        TargetFace->setFaceMaterialType(MaterialID);

        // TODO Set needs draw on Cell??
    }
}

inline int16_t Map::getFaceMaterial(MapCoordinates Coordinates, Direction DirectionType) const
{
    Face* TargetFace = getFace(Coordinates, DirectionType);

    if (TargetFace != NULL)
    {
        return TargetFace->getFaceMaterialType();
    }
    return INVALID_INDEX;
}

void Map::setFaceSurfaceType(MapCoordinates Coordinates, Direction DirectionType, int16_t SurfaceID)
{
    Face* TargetFace = getFace(Coordinates, DirectionType);

    if (TargetFace != NULL)
    {
        TargetFace->setFaceSurfaceType(SurfaceID, DirectionType);
        // TODO Set needs draw on Cell??
    }
}

void Map::setBothFaceSurfaceTypes(MapCoordinates Coordinates, Direction DirectionType, int16_t SurfaceID)
{
    Face* TargetFace = getFace(Coordinates, DirectionType);

    if (TargetFace != NULL)
    {
        TargetFace->setFaceSurfaceType(SurfaceID, DirectionType);
        TargetFace->setFaceSurfaceType(SurfaceID, OppositeDirection(DirectionType));

        // TODO Set needs draw on Cell??
    }
}

inline int16_t Map::getFaceSurfaceType(MapCoordinates Coordinates, Direction DirectionType) const
{
    Face* TargetFace = getFace(Coordinates, DirectionType);

    if (TargetFace != NULL)
    {
        if (isDirectionPositive(DirectionType))
        {
            return TargetFace->getNegativeAxisSurfaceType();
        }
        else
        {
            return TargetFace->getPositiveAxisSurfateType();
        }
    }
    return INVALID_INDEX;
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

void Map::Dig(MapCoordinates Coordinates)
{
    static int16_t FloorID = DATA->getLabelIndex("TILESHAPE_FLOOR");
    static int16_t RoughWallID = DATA->getLabelIndex("SURFACETYPE_ROUGH_WALL");
    static int16_t RoughFloorID = DATA->getLabelIndex("SURFACETYPE_ROUGH_FLOOR_1");

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
                    if (isCubeSolid(ModifiedCoordinates))
                    {
                        Face* TargetFace = getFace(Coordinates, DirectionType);
                        if (TargetFace == NULL)
                        {
                            TargetFace = addFace(Coordinates, DIRECTION_DOWN);
                        }

                        TargetFace->setFaceMaterialType(getCubeMaterial(ModifiedCoordinates));
                        TargetFace->setFaceSurfaceType(RoughFloorID, DirectionType);
                        TargetFace->setFaceSurfaceType(RoughFloorID, OppositeDirection(DirectionType));
                    }
                    else
                    {
                        removeFace(Coordinates, DirectionType);
                    }
                }
                else
                {
                    if (isCubeSolid(ModifiedCoordinates))
                    {
                        Face* TargetFace = getFace(Coordinates, DirectionType);
                        if (TargetFace == NULL)
                        {
                            TargetFace = addFace(Coordinates, DirectionType);
                        }

                        TargetFace->setFaceMaterialType(getCubeMaterial(ModifiedCoordinates));
                        TargetFace->setFaceSurfaceType(RoughWallID, DirectionType);
                        TargetFace->setFaceSurfaceType(RoughWallID, OppositeDirection(DirectionType));
                    }
                    else
                    {
                        removeFace(Coordinates, DirectionType);
                    }
                }
            }

            setCubeHidden(Coordinates, false);
            setCubeShape(Coordinates, TILESHAPE_FLOOR);

            setCubeMaterial(Coordinates, INVALID_INDEX);
        }

        // reveal tiles around
        for(Direction DirectionType = COMPASS_DIRECTIONS_START; DirectionType < NUM_COMPASS_DIRECTIONS; ++DirectionType)
        {
            setCubeHidden(MapCoordinates(Coordinates, DirectionType), false);
        }
    }
}

void Map::Fill(MapCoordinates Coordinates, int16_t MaterialID)
{
    static int16_t RoughWallID = DATA->getLabelIndex("SURFACETYPE_ROUGH_WALL");
    static int16_t RoughFloorID = DATA->getLabelIndex("SURFACETYPE_ROUGH_FLOOR_1");

    if (isCubeInited(Coordinates))
    {
        if (!isCubeSolid(Coordinates) && !isCubeSloped(Coordinates))
        {
            setCubeShape(Coordinates, TILESHAPE_SOLID);
            setCubeMaterial(Coordinates, MaterialID);

            for(Direction DirectionType = AXIAL_DIRECTIONS_START; DirectionType < NUM_AXIAL_DIRECTIONS; ++DirectionType)
            {
                MapCoordinates ModifiedCoordinates = Coordinates;
                ModifiedCoordinates.TranslateMapCoordinates(DirectionType);

                if (!isCubeSolid(ModifiedCoordinates) || !isCubeInited(ModifiedCoordinates))  //((AxisFromDirection(DirectionType) == AXIS_Z) && (!)))
                {
                    Face* TargetFace = getFace(Coordinates, DirectionType);
                    if (TargetFace == NULL)
                    {
                        TargetFace = addFace(Coordinates, DirectionType);
                    }

                    if (TargetFace != NULL)
                    {
                        if (DirectionType == DIRECTION_UP)
                        {
                            TargetFace->setFaceMaterialType(MaterialID);
                            TargetFace->setFaceSurfaceType(RoughFloorID, DirectionType);
                        }
                        else
                        {
                            TargetFace->setFaceMaterialType(MaterialID);
                            TargetFace->setFaceSurfaceType(RoughWallID, DirectionType);
                        }
                    }
                }
                else
                {
                    removeFace(Coordinates, DirectionType);
                }
            }
        }
    }
}

MapCoordinates Map::getRayIntersection(Ogre::Ray MouseRay)
{
    Ogre::Plane TestPlane;
    TestPlane.normal = Ogre::Vector3::UNIT_Z;
    TestPlane.d = -HighestCell;

    for (int32_t i = HighestCell; i > LowestCell; i--) // Drill down testing each Z level
    {
        TestPlane.d = (-i - HALFCUBE);
        std::pair<bool, Ogre::Real> result = MouseRay.intersects(TestPlane);

        if (result.first) // Was an intersection found
        {
            MapCoordinates TestCoords = MapCoordinates(MouseRay.getPoint(result.second));
            if (isCubeInited(TestCoords))
            {
                TileShape Shape = getCubeShape(TestCoords);
                if (Shape != TILESHAPE_EMPTY && Shape != TILESHAPE_SOLID)
                {
                    LastRayTestResult = TestCoords;
                    return TestCoords;
                }
            }
        }
    }
    return LastRayTestResult;
}

/*
MapCoordinates Map::getMapCenter() const
{
    MapCoordinates CenterPoint;

    CenterPoint.X = getMapSizeX() / 2;
    CenterPoint.Y = getMapSizeY() / 2;
    CenterPoint.Z = getMapSizeZ();

    return CenterPoint;
}
*/

void Map::ReleaseMap()
{
    MapLoaded = false;

    Cells.clear();
    Zones.clear();
}

void Map::addZone(Zone* NewZone)
{
    Zones.push_back(NewZone);
}

Zone* Map::getZoneAt(MapCoordinates TestCoordinates)
{
    for(uint32_t ZoneIndex = 0; ZoneIndex != Zones.size(); ZoneIndex++)
    {
        if (Zones[ZoneIndex]->isCoordinateInZone(TestCoordinates))
        {
            return Zones[ZoneIndex];
        }
    }
    return NULL;
}

void Map::setActiveZone(Zone* ActivatedZone)
{
    if(ActiveZone != NULL)
    {
        ActiveZone->setActive(false);
    }

    ActiveZone = ActivatedZone;

    if(ActiveZone != NULL)
    {
        ActivatedZone->setActive(true);
    }
}
