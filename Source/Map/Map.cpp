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

std::pair< MapCoordinates, Direction > Map::FaceCoordinateConvertion(MapCoordinates TargetMapCoordinates, Direction DirectionType) const
{
    if (isDirectionPositive(DirectionType))  // True for East, South and Top some of which will require Translation of the Cube and Inversion of Direction
    {
        TargetMapCoordinates.TranslateMapCoordinates(DirectionType);
        return make_pair(TargetMapCoordinates, OppositeDirection(DirectionType));
    }
    else
    {
        return make_pair(TargetMapCoordinates, DirectionType);
    }
}

Face* Map::getFace(MapCoordinates TargetMapCoordinates, Direction DirectionType) const
{
    std::pair< MapCoordinates, Direction > ConvertedValues = FaceCoordinateConvertion(TargetMapCoordinates, DirectionType);

    Cell* TargetCell = getCell(CellCoordinates(ConvertedValues.first));
    return TargetCell != NULL ? TargetCell->getFace(FaceCoordinates(ConvertedValues.first.Cube(), ConvertedValues.second)) : NULL;
}

bool Map::hasFace(MapCoordinates TargetMapCoordinates, Direction DirectionType) const
{
    std::pair< MapCoordinates, Direction > ConvertedValues = FaceCoordinateConvertion(TargetMapCoordinates, DirectionType);

    Cell* TargetCell = getCell(CellCoordinates(ConvertedValues.first));
    return TargetCell != NULL ? TargetCell->hasFace(FaceCoordinates(ConvertedValues.first.Cube(), ConvertedValues.second)) : false;
}

bool Map::removeFace(MapCoordinates TargetMapCoordinates, Direction DirectionType)
{
    std::pair< MapCoordinates, Direction > ConvertedValues = FaceCoordinateConvertion(TargetMapCoordinates, DirectionType);

    Cell* TargetCell = getCell(CellCoordinates(ConvertedValues.first));
    return TargetCell != NULL ? TargetCell->removeFace(FaceCoordinates(ConvertedValues.first.Cube(), ConvertedValues.second)) : false;
}

Face* Map::addFace(MapCoordinates TargetMapCoordinates, Direction DirectionType)
{
    std::pair< MapCoordinates, Direction > ConvertedValues = FaceCoordinateConvertion(TargetMapCoordinates, DirectionType);

    Cell* TargetCell = getCell(CellCoordinates(ConvertedValues.first));
    if (TargetCell == NULL)
    {
        TargetCell = new Cell();

        // Properly Initialize?

        TargetCell->setCellPosition(CellCoordinates(ConvertedValues.first));
        insertCell(TargetCell, CellCoordinates(ConvertedValues.first));
    }

    return TargetCell->getFace(FaceCoordinates(ConvertedValues.first.Cube(), ConvertedValues.second));
}

bool Map::isCubeSloped(MapCoordinates Coordinates) const
{
    Cell* TargetCell = getCubeOwner(Coordinates);

    return TargetCell != NULL ? TargetCell->isCubeSloped(Coordinates.Cube()) : false;
}

void Map::setCubeShape(MapCoordinates Coordinates, TileShape NewShape)
{
    Cell* TargetCell = getCubeOwner(Coordinates);

    if (TargetCell != NULL)
    {
        TargetCell->setCubeShape(Coordinates.Cube(), NewShape);
    }
}

TileShape Map::getCubeShape(MapCoordinates Coordinates) const
{
    Cell* TargetCell = getCubeOwner(Coordinates);

    return TargetCell != NULL ? TargetCell->getCubeShape(Coordinates.Cube()) : NUM_TILESHAPES;
}

void Map::setCubeMaterial(MapCoordinates Coordinates, int16_t MaterialID)
{
    Cell* TargetCell = getCubeOwner(Coordinates);

    if (TargetCell != NULL)
    {
        TargetCell->setCubeMaterial(Coordinates.Cube(), MaterialID);
    }
}

inline int16_t Map::getCubeMaterial(MapCoordinates Coordinates) const
{
    Cell* TargetCell = getCubeOwner(Coordinates);

    return TargetCell != NULL ? TargetCell->getCubeMaterial(Coordinates.Cube()) : INVALID_INDEX;
}

void Map::setFaceMaterial(MapCoordinates TargetMapCoordinates, Direction DirectionType, int16_t MaterialID)
{
    std::pair< MapCoordinates, Direction > ConvertedValues = FaceCoordinateConvertion(TargetMapCoordinates, DirectionType);

    Cell* TargetCell = getCell(CellCoordinates(ConvertedValues.first));
    if (TargetCell != NULL)
    {
        TargetCell->setFaceMaterialType(FaceCoordinates(ConvertedValues.first.Cube(), ConvertedValues.second), MaterialID);
    }
}

inline int16_t Map::getFaceMaterial(MapCoordinates TargetMapCoordinates, Direction DirectionType) const
{
    Face* TargetFace = getFace(TargetMapCoordinates, DirectionType);

    return TargetFace != NULL ? TargetFace->getFaceMaterialType() : INVALID_INDEX;
}

void Map::setFaceSurfaceType(MapCoordinates TargetMapCoordinates, Direction DirectionType, int16_t SurfaceID)
{
    std::pair< MapCoordinates, Direction > ConvertedValues = FaceCoordinateConvertion(TargetMapCoordinates, DirectionType);

    Cell* TargetCell = getCell(CellCoordinates(ConvertedValues.first));
    if (TargetCell != NULL)
    {
        TargetCell->setFaceSurfaceType(FaceCoordinates(ConvertedValues.first.Cube(), ConvertedValues.second), SurfaceID);
    }
}

inline int16_t Map::getFaceSurfaceType(MapCoordinates TargetMapCoordinates, Direction DirectionType) const
{
    Face* TargetFace = getFace(TargetMapCoordinates, DirectionType);

    return TargetFace != NULL ? TargetFace->getFaceSurfaceType() : INVALID_INDEX;
}

bool Map::isCubeHidden(MapCoordinates Coordinates) const
{
    Cell* TargetCell = getCubeOwner(Coordinates);

    if(TargetCell != NULL)
    {
        return TargetCell->isCubeHidden(Coordinates.Cube());
    }
    return false;
}

void Map::setCubeHidden(MapCoordinates Coordinates, bool NewValue)
{
    Cell* TargetCell = getCubeOwner(Coordinates);

    if(TargetCell != NULL)
    {
        TargetCell->setCubeHidden(Coordinates.Cube(), NewValue);
    }
}

bool Map::isCubeSubTerranean(MapCoordinates Coordinates) const
{
    Cell* TargetCell = getCubeOwner(Coordinates);

    if(TargetCell != NULL)
    {
        return TargetCell->isCubeSubTerranean(Coordinates.Cube());
    }
    return false;
}

void Map::setCubeSubTerranean(MapCoordinates Coordinates, bool NewValue)
{
    Cell* TargetCell = getCubeOwner(Coordinates);

    if(TargetCell != NULL)
    {
        TargetCell->setCubeSubTerranean(Coordinates.Cube(), NewValue);
    }
}

bool Map::isCubeSkyView(MapCoordinates Coordinates) const
{
    Cell* TargetCell = getCubeOwner(Coordinates);

    if(TargetCell != NULL)
    {
        return TargetCell->isCubeSkyView(Coordinates.Cube());
    }
    return false;
}

void Map::setCubeSkyView(MapCoordinates Coordinates, bool NewValue)
{
    Cell* TargetCell = getCubeOwner(Coordinates);

    if(TargetCell != NULL)
    {
        TargetCell->setCubeSkyView(Coordinates.Cube(), NewValue);
    }
}

bool Map::isCubeSunLit(MapCoordinates Coordinates) const
{
    Cell* TargetCell = getCubeOwner(Coordinates);

    if(TargetCell != NULL)
    {
        return TargetCell->isCubeSunLit(Coordinates.Cube());
    }
    return false;
}

void Map::setCubeSunLit(MapCoordinates Coordinates, bool NewValue)
{
    Cell* TargetCell = getCubeOwner(Coordinates);

    if(TargetCell != NULL)
    {
        TargetCell->setCubeSunLit(Coordinates.Cube(), NewValue);
    }
}

void Map::Dig(MapCoordinates Coordinates)
{
    static int16_t FloorID = DATA->getLabelIndex("TILESHAPE_FLOOR");
    static int16_t RoughWallID = DATA->getLabelIndex("SURFACETYPE_ROUGH_WALL");
    static int16_t RoughFloorID = DATA->getLabelIndex("SURFACETYPE_ROUGH_FLOOR_1");

    if (isCubeInited(Coordinates))
    {
        TileShape Shape = getCubeShape(Coordinates);
        if (Shape != TILESHAPE_EMPTY)
        {
            for (Direction DirectionType = AXIAL_DIRECTIONS_START; DirectionType < NUM_AXIAL_DIRECTIONS; ++DirectionType)
            {
                MapCoordinates ModifiedCoordinates = Coordinates;
                ModifiedCoordinates.TranslateMapCoordinates(DirectionType);

                if (DirectionType == DIRECTION_DOWN)
                {
                    if (Shape != TILESHAPE_EMPTY)
                    {
                        Face* TargetFace = getFace(Coordinates, DirectionType);
                        if (TargetFace == NULL)
                        {
                            TargetFace = addFace(Coordinates, DIRECTION_DOWN);
                        }

                        TargetFace->setFaceMaterialType(getCubeMaterial(ModifiedCoordinates));
                        TargetFace->setFaceSurfaceType(RoughFloorID);
                    }
                    else
                    {
                        removeFace(Coordinates, DirectionType);
                    }
                }
                else
                {
                    if (Shape != TILESHAPE_EMPTY)
                    {
                        Face* TargetFace = getFace(Coordinates, DirectionType);
                        if (TargetFace == NULL)
                        {
                            TargetFace = addFace(Coordinates, DirectionType);
                        }

                        TargetFace->setFaceMaterialType(getCubeMaterial(ModifiedCoordinates));
                        TargetFace->setFaceSurfaceType(RoughWallID);
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
        TileShape Shape = getCubeShape(Coordinates);
        if (Shape != TILESHAPE_SOLID)
        {
            setCubeShape(Coordinates, TILESHAPE_SOLID);
            setCubeMaterial(Coordinates, MaterialID);

            for(Direction DirectionType = AXIAL_DIRECTIONS_START; DirectionType < NUM_AXIAL_DIRECTIONS; ++DirectionType)
            {
                MapCoordinates ModifiedCoordinates = Coordinates;
                ModifiedCoordinates.TranslateMapCoordinates(DirectionType);
                TileShape AdjacentShape = getCubeShape(ModifiedCoordinates);

                if (AdjacentShape != TILESHAPE_EMPTY || !isCubeInited(ModifiedCoordinates))  //((AxisFromDirection(DirectionType) == AXIS_Z) && (!)))
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
                            TargetFace->setFaceSurfaceType(RoughFloorID);
                        }
                        else
                        {
                            TargetFace->setFaceMaterialType(MaterialID);
                            TargetFace->setFaceSurfaceType(RoughWallID);
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
        TestPlane.d = (-i + HALFCUBE);
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
