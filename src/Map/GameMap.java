/* Copyright 2010 Kenneth 'Impaler' Ferland

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

package Map;

import java.util.concurrent.ConcurrentHashMap;
import java.util.ArrayList;

import Interface.VolumeSelection;

import org.javatuples.Pair;

import Data.DataTypes;

/**
 * Master Class for holding the playing Map, holds primaricly Cells in HashMap
 * structure and pipes changes to the Cells to abstract them away from the rest
 * of the code base, 
 * @author Impaler
 */
public class GameMap {

	private static GameMap instance = null;
	
    boolean Initialized;
    boolean MapLoaded;

    ConcurrentHashMap<CellCoordinate, Cell> Cells;
	
    int HighestCell;
    int LowestCell;

    ArrayList<Zone> Zones;

    MapCoordinate LastRayTestResult;  // Used to smoothout Map Picking
	
	
	protected GameMap() {
		Initialized = false;
		MapLoaded = false;
		HighestCell = 0;
		LowestCell = 0;
		
		Cells = new ConcurrentHashMap<CellCoordinate, Cell>();
	}

	public static GameMap getMap() {
	   if(instance == null) {
		  instance = new GameMap();
	   }
	   return instance;
	}

	public Cell getCell(CellCoordinate TestCoords) {
		return Cells.get(TestCoords);
	}

	public boolean insertCell(Cell NewCell) {
		CellCoordinate TargetCoordinates = NewCell.getCellCoordinates();
		if (getCell(TargetCoordinates) == null)
		{
			Cells.put(TargetCoordinates, NewCell);

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

	public Cell getCubeOwner(MapCoordinate Coordinates) {
		CellCoordinate TargetCellCoordinates = new CellCoordinate(Coordinates);

		return getCell(TargetCellCoordinates);
	}

	public boolean isCubeInited(MapCoordinate Coordinates) {
		return getCubeOwner(Coordinates) != null;
	}

	public MapCoordinate getFacingCoordinates(CellCoordinate cellcoords, FaceCoordinate facecoords) {
		MapCoordinate ModifiedCoordinates = new MapCoordinate(cellcoords, facecoords.Coordinates);
		ModifiedCoordinates.TranslateMapCoordinates(facecoords.FaceDirection);
		return ModifiedCoordinates;
	}

	Pair< MapCoordinate, Direction > FaceCoordinateConvertion(MapCoordinate TargetMapCoordinates, Direction DirectionType) {
		if (DirectionType.Positive())  // True for East, South and Top some of which will require Translation of the Cube and Inversion of Direction
		{
			//TargetMapCoordinates.TranslateMapCoordinates(DirectionType);
			//return new Pair(TargetMapCoordinates, DirectionType.Invert());
			return new Pair(TargetMapCoordinates, DirectionType);
		}
		else
		{
			return new Pair(TargetMapCoordinates, DirectionType);
		}
	}

	public Face getFace(MapCoordinate TargetMapCoordinates, Direction DirectionType) {
		Pair< MapCoordinate, Direction > ConvertedValues = FaceCoordinateConvertion(TargetMapCoordinates, DirectionType);

		Cell TargetCell = getCell(new CellCoordinate(ConvertedValues.getValue0()));
		return TargetCell != null ? TargetCell.getFace(new FaceCoordinate(ConvertedValues.getValue0().CubeIndex(), ConvertedValues.getValue1())) : null;
	}

	public boolean hasFace(MapCoordinate TargetMapCoordinates, Direction DirectionType) {
		Pair< MapCoordinate, Direction > ConvertedValues = FaceCoordinateConvertion(TargetMapCoordinates, DirectionType);

		Cell TargetCell = getCell(new CellCoordinate(ConvertedValues.getValue0()));
		return TargetCell != null ? TargetCell.hasFace(new FaceCoordinate(ConvertedValues.getValue0().CubeIndex(), ConvertedValues.getValue1())) : false;
	}

	public boolean removeFace(MapCoordinate TargetMapCoordinates, Direction DirectionType) {
		Pair< MapCoordinate, Direction > ConvertedValues = FaceCoordinateConvertion(TargetMapCoordinates, DirectionType);

		Cell TargetCell = getCell(new CellCoordinate(ConvertedValues.getValue0()));
		return TargetCell != null ? TargetCell.removeFace(new FaceCoordinate(ConvertedValues.getValue0().CubeIndex(), ConvertedValues.getValue1())) : false;
	}

	public Face addFace(MapCoordinate TargetMapCoordinates, Direction DirectionType) {
		Pair< MapCoordinate, Direction > ConvertedValues = FaceCoordinateConvertion(TargetMapCoordinates, DirectionType);

		Cell TargetCell = getCell(new CellCoordinate(ConvertedValues.getValue0()));
		if (TargetCell == null)
		{
			TargetCell = new Cell();

			// Properly Initialize?

			TargetCell.setCellPosition(new CellCoordinate(ConvertedValues.getValue0()));
			insertCell(TargetCell);
		}

		return TargetCell.addFace(new FaceCoordinate(ConvertedValues.getValue0().CubeIndex(), ConvertedValues.getValue1()));
	}

	public void setCubeShape(MapCoordinate Coordinates, CubeShape NewShape) {
		Cell TargetCell = getCubeOwner(Coordinates);

		if (TargetCell != null)
		{
			TargetCell.setCubeShape(Coordinates.CubeIndex(), NewShape);
		}
	}

	public CubeShape getCubeShape(MapCoordinate Coordinates) {
		Cell TargetCell = getCubeOwner(Coordinates);

		if (TargetCell != null) {
			return TargetCell.getCubeShape(Coordinates.CubeIndex());
		} else {
			return new CubeShape(CubeShape.BELOW_CUBE_HEIGHT);
		}
	}

	public void setCubeMaterial(MapCoordinate Coordinates, short MaterialID) {
		Cell TargetCell = getCubeOwner(Coordinates);

		if (TargetCell != null)
		{
			TargetCell.setCubeMaterial(Coordinates.CubeIndex(), MaterialID);
		}
	}

	public short getCubeMaterial(MapCoordinate Coordinates) {
		Cell TargetCell = getCubeOwner(Coordinates);

		return TargetCell != null ? TargetCell.getCubeMaterial(Coordinates.CubeIndex()) : DataTypes.INVALID_INDEX;
	}

	public void setFaceMaterial(MapCoordinate TargetMapCoordinates, Direction DirectionType, short MaterialID) {
		Pair< MapCoordinate, Direction > ConvertedValues = FaceCoordinateConvertion(TargetMapCoordinates, DirectionType);

		Cell TargetCell = getCell(new CellCoordinate(ConvertedValues.getValue0()));
		if (TargetCell != null)
		{
			TargetCell.setFaceMaterialType(new FaceCoordinate(ConvertedValues.getValue0().CubeIndex(), ConvertedValues.getValue1()), MaterialID);
		}
	}

	public short getFaceMaterial(MapCoordinate TargetMapCoordinates, Direction DirectionType) {
		Face TargetFace = getFace(TargetMapCoordinates, DirectionType);

		return TargetFace != null ? TargetFace.getFaceMaterialType() : DataTypes.INVALID_INDEX;
	}

	public void setFaceSurfaceType(MapCoordinate TargetMapCoordinates, Direction DirectionType, short SurfaceID) {
		Pair< MapCoordinate, Direction > ConvertedValues = FaceCoordinateConvertion(TargetMapCoordinates, DirectionType);

		Cell TargetCell = getCell(new CellCoordinate(ConvertedValues.getValue0()));
		if (TargetCell != null)
		{
			TargetCell.setFaceSurfaceType(new FaceCoordinate(ConvertedValues.getValue0().CubeIndex(), ConvertedValues.getValue1()), SurfaceID);
		}
	}

	public short getFaceSurfaceType(MapCoordinate TargetMapCoordinates, Direction DirectionType) {
		Face TargetFace = getFace(TargetMapCoordinates, DirectionType);

		return TargetFace != null ? TargetFace.getFaceSurfaceType() : DataTypes.INVALID_INDEX;
	}

	public boolean isCubeHidden(MapCoordinate Coordinates) {
		Cell TargetCell = getCubeOwner(Coordinates);

		if(TargetCell != null)
		{
			return TargetCell.isCubeHidden(Coordinates.CubeIndex());
		}
		return false;
	}

	public void setCubeHidden(MapCoordinate Coordinates, boolean NewValue) {
		Cell TargetCell = getCubeOwner(Coordinates);

		if(TargetCell != null)
			TargetCell.setCubeHidden(Coordinates.CubeIndex(), NewValue);	
	}
	/*
	public boolean isCubeSubTerranean(MapCoordinates Coordinates) {
		Cell TargetCell = getCubeOwner(Coordinates);

		if(TargetCell != null)
		{
			return TargetCell.isCubeSubTerranean(Coordinates.Cube());
		}
		return false;
	}

	public void setCubeSubTerranean(MapCoordinates Coordinates, boolean NewValue) {
		Cell TargetCell = getCubeOwner(Coordinates);

		if(TargetCell != null)
		{
			TargetCell.setCubeSubTerranean(Coordinates.Cube(), NewValue);
		}
	}

	public boolean isCubeSkyView(MapCoordinates Coordinates) {
		Cell TargetCell = getCubeOwner(Coordinates);

		if(TargetCell != null)
		{
			return TargetCell.isCubeSkyView(Coordinates.Cube());
		}
		return false;
	}

	public void setCubeSkyView(MapCoordinates Coordinates, boolean NewValue) {
		Cell TargetCell = getCubeOwner(Coordinates);

		if(TargetCell != null)
		{
			TargetCell.setCubeSkyView(Coordinates.Cube(), NewValue);
		}
	}

	public boolean isCubeSunLit(MapCoordinates Coordinates) {
		Cell TargetCell = getCubeOwner(Coordinates);

		if(TargetCell != null)
		{
			return TargetCell.isCubeSunLit(Coordinates.Cube());
		}
		return false;
	}

	public void setCubeSunLit(MapCoordinates Coordinates, boolean NewValue) {
		Cell TargetCell = getCubeOwner(Coordinates);

		if(TargetCell != null)
		{
			TargetCell.setCubeSunLit(Coordinates.Cube(), NewValue);
		}
	}
	*/

	public void UpdateCubeShape(MapCoordinate Coordinates, CubeShape NewShape) {
		if (isCubeInited(Coordinates))
		{
			CubeShape Shape = getCubeShape(Coordinates);
			if (Shape != NewShape)
			{
				setCubeShape(Coordinates, NewShape);
				if (NewShape.isEmpty())
				{
					setCubeMaterial(Coordinates, DataTypes.INVALID_INDEX);
				}

				for (Direction DirectionType : Direction.AXIAL_DIRECTIONS)
				{
					UpdateFace(Coordinates, DirectionType);
				}
				UpdateFace(Coordinates, Direction.DIRECTION_NONE);

				setCubeHidden(Coordinates, false);
			}

			// reveal tiles around
			//for(Direction DirectionType = COMPASS_DIRECTIONS_START; DirectionType < NUM_COMPASS_DIRECTIONS; ++DirectionType)
			//{
			//    setCubeHidden(MapCoordinates(Coordinates, DirectionType), false);
			//}
		}
	}

	public void UpdateFace(MapCoordinate TargetCoordinates, Direction DirectionType) {
		final short RoughWallID = 0; //DATA.getLabelIndex("SURFACETYPE_ROUGH_WALL");
		final short RoughFloorID = 1; //DATA.getLabelIndex("SURFACETYPE_ROUGH_FLOOR_1");

		MapCoordinate ModifiedCoordinates = TargetCoordinates;
		ModifiedCoordinates.TranslateMapCoordinates(DirectionType);

		if (!isCubeInited(ModifiedCoordinates))
		{
			// Init it
			return;
		}

		CubeShape SourceShape = getCubeShape(TargetCoordinates);
		CubeShape AdjacentShape = getCubeShape(ModifiedCoordinates);
		Face TargetFace = getFace(TargetCoordinates, DirectionType);

		switch (DirectionType) {

			case DIRECTION_NONE:
				if (!SourceShape.isEmpty() && !SourceShape.isSolid())
				{
					if (TargetFace == null)
					{
						TargetFace = addFace(TargetCoordinates, Direction.DIRECTION_NONE);
						// Dirty Cell if edge case
					}

					TargetFace.setFaceMaterialType(getCubeMaterial(TargetCoordinates));
					TargetFace.setFaceShapeType(new FaceShape(getCubeShape(TargetCoordinates), Direction.DIRECTION_NONE));
					TargetFace.setFaceSurfaceType(RoughFloorID);

				} else {
					removeFace(TargetCoordinates, DirectionType);
				}
				break;

			case DIRECTION_DOWN:
				if (!AdjacentShape.isEmpty() && SourceShape.hasFloor())
				{
					if (TargetFace == null)
					{
						TargetFace = addFace(TargetCoordinates, DirectionType);
					}

					TargetFace.setFaceMaterialType(getCubeMaterial(ModifiedCoordinates));
					TargetFace.setFaceSurfaceType(RoughFloorID);
					TargetFace.setFaceShapeType(new FaceShape(SourceShape, DirectionType));
				} else {
					removeFace(TargetCoordinates, DirectionType);
				}
				break;

			case DIRECTION_UP:
				if (AdjacentShape.isEmpty() && SourceShape.hasCeiling())
				{
					if (TargetFace == null)
					{
						TargetFace = addFace(TargetCoordinates, DirectionType);
					}

					TargetFace.setFaceMaterialType(getCubeMaterial(TargetCoordinates));
					TargetFace.setFaceSurfaceType(RoughFloorID);
					TargetFace.setFaceShapeType(new FaceShape(SourceShape, DirectionType));
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
					if (TargetFace == null)
					{
						TargetFace = addFace(TargetCoordinates, DirectionType);
					}

					TargetFace.setFaceMaterialType(getCubeMaterial(ModifiedCoordinates));
					TargetFace.setFaceShapeType(new FaceShape(AdjacentShape, DirectionType.Invert()));
					TargetFace.setFaceSurfaceType(RoughWallID);
				} else {
					removeFace(TargetCoordinates, DirectionType);
				}
				break;

			default:
				break;
		}
	}

	/*
	public MapCoordinates getRayIntersection(Ogre::Ray MouseRay, short Top, short Bottom) {
		Ogre::Plane TestPlane;
		TestPlane.normal = Ogre::Vector3::UNIT_Z;
		TestPlane.d = -HighestCell;

		for (int i = Top; i >= Bottom; i--) // Drill down testing each Z level
		{
			TestPlane.d = (-i + HALFCUBE);
			std::pair<boolean, Ogre::Real> result = MouseRay.intersects(TestPlane);

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
	}*/

	public void ReleaseMap() {
		MapLoaded = false;

		Cells.clear();
		Zones.clear();
	}

	public void addZone(Zone NewZone) {
		Zones.add(NewZone);
	}

	public Zone createZone(ArrayList< VolumeSelection > Volumes) {
		Zone NewZone = new Zone(Volumes);
		Zones.add(NewZone);
		return NewZone;
	}

	public ArrayList<Zone> getZonesAt(MapCoordinate TestCoordinates) {
		ArrayList<Zone> Collection = new ArrayList<Zone>();
		for(Zone Z : Zones)
		{
			if (Z.isCoordinateInZone(TestCoordinates))
			{
				Collection.add(Z);
			}
		}
		return Collection;
	}

	public ConcurrentHashMap getCellMap() {
		return Cells;
	}
/*
	void Save(boost::filesystem::basic_ofstream<char>& Stream)
	{
		Stream.write((char*)&Initialized, sizeof(Initialized));
		Stream.write((char*)&MapLoaded, sizeof(MapLoaded));

		Stream.write((char*)&HighestCell, sizeof(HighestCell));
		Stream.write((char*)&LowestCell, sizeof(LowestCell));

		uint32_t CellCount = Cells.size();
		Stream.write((char*)&CellCount, sizeof(CellCount));
		for (boost::unordered_map<uint64_t, Cell>::const_iterator it = Cells.begin(); it != Cells.end(); it++)
		{
			it.second.Save(Stream);
		}
	}

	void Load(boost::filesystem::basic_ifstream<char>& Stream)
	{
		Stream.read((char*)&Initialized, sizeof(Initialized));
		Stream.read((char*)&MapLoaded, sizeof(MapLoaded));

		Stream.read((char*)&HighestCell, sizeof(HighestCell));
		Stream.read((char*)&LowestCell, sizeof(LowestCell));

		uint32_t CellCount;
		Stream.read((char*)&CellCount, sizeof(CellCount));
		for (int i = 0; i < CellCount; i++)
		{
			Cell NewCell = new Cell();
			NewCell.Load(Stream);
			insertCell((NewCell));
		}
	}*/
}


