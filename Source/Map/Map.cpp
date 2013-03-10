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

#include <Geology.h>
#include <Cell.h>
#include <Face.h>
#include <Zone.h>
#include <TileShapes.h>


Map::Map()
{
    Initialized = false;
    HighestCell = 0;
    LowestCell = 0;
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

Cell* Map::getCubeOwner(MapCoordinates Coordinates) const
{
    CellCoordinates TargetCellCoordinates = CellCoordinates(Coordinates);

    return getCell(TargetCellCoordinates);
}

bool Map::isCubeInited(MapCoordinates Coordinates) const
{
    return getCubeOwner(Coordinates) != NULL;
}

std::pair< MapCoordinates, Direction > Map::FaceCoordinateConvertion(MapCoordinates TargetMapCoordinates, Direction DirectionType) const
{
    if (DirectionType.Positive())  // True for East, South and Top some of which will require Translation of the Cube and Inversion of Direction
    {
        TargetMapCoordinates.TranslateMapCoordinates(DirectionType);
        return make_pair(TargetMapCoordinates, DirectionType.Invert());
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

    return TargetCell->addFace(FaceCoordinates(ConvertedValues.first.Cube(), ConvertedValues.second));
}

void Map::setCubeShape(MapCoordinates Coordinates, CubeShape NewShape)
{
    Cell* TargetCell = getCubeOwner(Coordinates);

    if (TargetCell != NULL)
    {
        TargetCell->setCubeShape(Coordinates.Cube(), NewShape);
    }
}

CubeShape Map::getCubeShape(MapCoordinates Coordinates) const
{
    Cell* TargetCell = getCubeOwner(Coordinates);

    return TargetCell != NULL ? TargetCell->getCubeShape(Coordinates.Cube()) : CubeShape(BELOW_CUBE_HEIGHT);
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

void Map::UpdateCubeShape(MapCoordinates Coordinates, CubeShape NewShape)
{
    if (isCubeInited(Coordinates))
    {
        CubeShape Shape = getCubeShape(Coordinates);
        if (Shape != NewShape)
        {
            setCubeShape(Coordinates, NewShape);
            if (NewShape.isEmpty())
            {
                setCubeMaterial(Coordinates, INVALID_INDEX);
            }

            for (uint8_t i = 0, DirectionType = Direction::AXIAL_DIRECTIONS[i]; i < NUM_AXIAL_DIRECTIONS; ++i, DirectionType = Direction::AXIAL_DIRECTIONS[i])
            {
                UpdateFace(Coordinates, DirectionType);
            }
            UpdateFace(Coordinates, DIRECTION_NONE);

            setCubeHidden(Coordinates, false);
        }

        // reveal tiles around
        //for(Direction DirectionType = COMPASS_DIRECTIONS_START; DirectionType < NUM_COMPASS_DIRECTIONS; ++DirectionType)
        //{
        //    setCubeHidden(MapCoordinates(Coordinates, DirectionType), false);
        //}
    }
}

void Map::UpdateFace(MapCoordinates TargetCoordinates, Direction DirectionType)
{
    static int16_t RoughWallID = DATA->getLabelIndex("SURFACETYPE_ROUGH_WALL");
    static int16_t RoughFloorID = DATA->getLabelIndex("SURFACETYPE_ROUGH_FLOOR_1");

    MapCoordinates ModifiedCoordinates = TargetCoordinates;
    ModifiedCoordinates.TranslateMapCoordinates(DirectionType);

    if (!isCubeInited(ModifiedCoordinates))
    {
        // Init it
        return;
    }

    CubeShape SourceShape = getCubeShape(TargetCoordinates);
    CubeShape AdjacentShape = getCubeShape(ModifiedCoordinates);
    Face* TargetFace = getFace(TargetCoordinates, DirectionType);

    switch (DirectionType.Data) {

        case DIRECTION_NONE:
            if (!SourceShape.isEmpty() && !SourceShape.isSolid())
            {
                if (TargetFace == NULL)
                {
                    TargetFace = addFace(TargetCoordinates, DIRECTION_NONE);
                    // Dirty Cell if edge case
                }

                TargetFace->setFaceMaterialType(getCubeMaterial(TargetCoordinates));
                TargetFace->setFaceShapeType(FaceShape(getCubeShape(TargetCoordinates), DIRECTION_NONE));
                TargetFace->setFaceSurfaceType(RoughFloorID);

            } else {
                removeFace(TargetCoordinates, DirectionType);
            }
            break;

        case DIRECTION_DOWN:
            if (!AdjacentShape.isEmpty() && SourceShape.hasFloor())
            {
                if (TargetFace == NULL)
                {
                    TargetFace = addFace(TargetCoordinates, DirectionType);
                }

                TargetFace->setFaceMaterialType(getCubeMaterial(ModifiedCoordinates));
                TargetFace->setFaceSurfaceType(RoughFloorID);
                TargetFace->setFaceShapeType(FaceShape(SourceShape, DirectionType));
            } else {
                removeFace(TargetCoordinates, DirectionType);
            }
            break;

        case DIRECTION_UP:
            if (AdjacentShape.isEmpty() && SourceShape.hasCeiling())
            {
                if (TargetFace == NULL)
                {
                    TargetFace = addFace(TargetCoordinates, DirectionType);
                }

                TargetFace->setFaceMaterialType(getCubeMaterial(TargetCoordinates));
                TargetFace->setFaceSurfaceType(RoughFloorID);
                TargetFace->setFaceShapeType(FaceShape(SourceShape, DirectionType));
            } else {
                removeFace(TargetCoordinates, DirectionType);
            }
            break;

        case DIRECTION_EAST:
        case DIRECTION_WEST:
        case DIRECTION_NORTH:
        case DIRECTION_SOUTH:
            if (SourceShape.isEmpty() && !AdjacentShape.isEmpty())
            {
                if (TargetFace == NULL)
                {
                    TargetFace = addFace(TargetCoordinates, DirectionType);
                }

                TargetFace->setFaceMaterialType(getCubeMaterial(ModifiedCoordinates));
                TargetFace->setFaceShapeType(FaceShape(AdjacentShape, DirectionType.Invert()));
                TargetFace->setFaceSurfaceType(RoughWallID);
            } else {
                removeFace(TargetCoordinates, DirectionType);
            }
            break;

        default:
            break;
    }
}

MapCoordinates Map::getRayIntersection(Ogre::Ray MouseRay, uint16_t Top, uint16_t Bottom)
{
    Ogre::Plane TestPlane;
    TestPlane.normal = Ogre::Vector3::UNIT_Z;
    TestPlane.d = -HighestCell;

    for (int32_t i = Top; i >= Bottom; i--) // Drill down testing each Z level
    {
        TestPlane.d = (-i + HALFCUBE);
        std::pair<bool, Ogre::Real> result = MouseRay.intersects(TestPlane);

        if (result.first) // Was an intersection found
        {
            Ogre::Vector3 Intersection = MouseRay.getPoint(result.second);
            MapCoordinates TestCoords = MapCoordinates(Intersection.x, Intersection.y, Intersection.z);
            if (isCubeInited(TestCoords))
            {
                CubeShape Shape = getCubeShape(TestCoords);
                if (!Shape.isSky())
                {
                    LastRayTestResult = TestCoords;
                    return TestCoords;
                }
            }
        }
    }
    return LastRayTestResult;
}

void Map::InvokeRendering()
{
	for (std::map<uint64_t, Cell*>::iterator it = Cells.begin(); it != Cells.end(); it++)
	{
		//it->second->setNeedsReRendering();
	}
}

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

Zone* Map::createZone(std::vector< VolumeSelection* > Volumes)
{
	Zone* NewZone = new Zone(Volumes);
    Zones.push_back(NewZone);
    return NewZone;
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
