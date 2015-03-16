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

import Core.Dice;
import Data.DataManager;
import PathFinding.PathFinding;

import java.util.concurrent.ConcurrentHashMap;
import java.util.ArrayList;

import Interface.VolumeSelection;

import org.javatuples.Pair;


/**
 * Master Class for holding the playing Map, holds primaraly Cells in HashMap
 * structure and pipes changes to the Cells/Faces to abstract them away from the
 * rest of the code base
 * 
 * @author Impaler
 */
public class GameMap {

	private static GameMap instance = null;
	
    boolean Initialized;
    boolean MapLoaded;

    ConcurrentHashMap<CellCoordinate, Cell> Cells;
	
    int HighestCell;
    int LowestCell;

	int Seed;
	Dice ExcavateDice = new Dice();

    ArrayList<Zone> Zones;
	int ZoneCounter = 0;

    MapCoordinate LastRayTestResult;  // Used to smoothout Map Picking
	
	
	protected GameMap() {
		Initialized = false;
		MapLoaded = false;
		HighestCell = -100000000;
		LowestCell = 10000000;
		
		ExcavateDice.Seed(Seed);
		Zones = new ArrayList<Zone>();
		Cells = new ConcurrentHashMap<CellCoordinate, Cell>();
	}

	public static GameMap getMap() {
	   if(instance == null) {
		  instance = new GameMap();
	   }
	   return instance;
	}

	public void Initialize(int MasterSeed) {
		Seed = MasterSeed;
		ExcavateDice.Seed(Seed);
	}

	public Cell getCell(CellCoordinate TestCoords) {
		return Cells.get(TestCoords);
	}

	public boolean insertCell(Cell NewCell) {
		CellCoordinate TargetCoordinates = NewCell.getCellCoordinates();
		if (getCell(TargetCoordinates) == null) {
			Cells.put(TargetCoordinates, NewCell);

			if (TargetCoordinates.Z > HighestCell) {
				HighestCell = TargetCoordinates.Z;
			}
			if (TargetCoordinates.Z < LowestCell) {
				LowestCell = TargetCoordinates.Z;
			}

			return true;
		}
		return false;  // A Cell already exists at that spot
	}

	public Cell InitializeCell(CellCoordinate Coords) {
		Cell TargetCell = getCell(Coords);
		if (TargetCell == null) {
			TargetCell = new Cell();
			TargetCell.setCellPosition(Coords);
			insertCell(TargetCell);
			return TargetCell;
		}
		return TargetCell;  // A Cell already exists at that spot		
	}

	public int getHighestCell() {
		return HighestCell;
	}
	
	public int getLowestCell() {
		return LowestCell;
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
		return TargetCell != null ? TargetCell.getFace(new FaceCoordinate(ConvertedValues.getValue0().CubeByteIndex(), ConvertedValues.getValue1())) : null;
	}

	public boolean hasFace(MapCoordinate TargetMapCoordinates, Direction DirectionType) {
		Pair< MapCoordinate, Direction > ConvertedValues = FaceCoordinateConvertion(TargetMapCoordinates, DirectionType);

		Cell TargetCell = getCell(new CellCoordinate(ConvertedValues.getValue0()));
		return TargetCell != null ? TargetCell.hasFace(new FaceCoordinate(ConvertedValues.getValue0().CubeByteIndex(), ConvertedValues.getValue1())) : false;
	}

	public boolean removeFace(MapCoordinate TargetMapCoordinates, Direction DirectionType) {
		Pair< MapCoordinate, Direction > ConvertedValues = FaceCoordinateConvertion(TargetMapCoordinates, DirectionType);

		Cell TargetCell = getCell(new CellCoordinate(ConvertedValues.getValue0()));
		return TargetCell != null ? TargetCell.removeFace(new FaceCoordinate(ConvertedValues.getValue0().CubeByteIndex(), ConvertedValues.getValue1())) : false;
	}

	public Face addFace(MapCoordinate TargetMapCoordinates, Direction DirectionType) {
		Pair< MapCoordinate, Direction > ConvertedValues = FaceCoordinateConvertion(TargetMapCoordinates, DirectionType);

		Cell TargetCell = getCell(new CellCoordinate(ConvertedValues.getValue0()));
		if (TargetCell == null)
			TargetCell = InitializeCell(new CellCoordinate(ConvertedValues.getValue0()));

		return TargetCell.addFace(new FaceCoordinate(ConvertedValues.getValue0().CubeByteIndex(), ConvertedValues.getValue1()));
	}

	public void setCubeShape(MapCoordinate Coordinate, CubeShape NewShape) {
		Cell TargetCell = getCubeOwner(Coordinate);

		if (TargetCell != null) {
			TargetCell.setCubeShape(Coordinate.CubeByteIndex(), NewShape);
			MapCoordinate[] Coordinates = {Coordinate};
			PathFinding.getSinglton().EditMapAbstractions(Coordinates);
		}
	}

	public CubeShape getCubeShape(MapCoordinate Coordinates) {
		Cell TargetCell = getCubeOwner(Coordinates);

		if (TargetCell != null) {
			return TargetCell.getCubeShape(Coordinates.CubeByteIndex());
		} else {
			return new CubeShape(CubeShape.BELOW_CUBE_HEIGHT);
		}
	}

	public void setCubeMaterial(MapCoordinate Coordinates, short MaterialID) {
		Cell TargetCell = getCubeOwner(Coordinates);

		if (TargetCell != null) {
			TargetCell.setCubeMaterial(Coordinates.CubeByteIndex(), MaterialID);
		}
	}

	public short getCubeMaterial(MapCoordinate Coordinates) {
		Cell TargetCell = getCubeOwner(Coordinates);
		return TargetCell != null ? TargetCell.getCubeMaterial(Coordinates.CubeByteIndex()) : DataManager.INVALID_INDEX;
	}

	public void setFaceMaterial(MapCoordinate TargetMapCoordinates, Direction DirectionType, short MaterialID) {
		Pair< MapCoordinate, Direction > ConvertedValues = FaceCoordinateConvertion(TargetMapCoordinates, DirectionType);

		Cell TargetCell = getCell(new CellCoordinate(ConvertedValues.getValue0()));
		if (TargetCell != null) {
			TargetCell.setFaceMaterialType(new FaceCoordinate(ConvertedValues.getValue0().CubeByteIndex(), ConvertedValues.getValue1()), MaterialID);
		}
	}

	public short getFaceMaterial(MapCoordinate TargetMapCoordinates, Direction DirectionType) {
		Face TargetFace = getFace(TargetMapCoordinates, DirectionType);
		return TargetFace != null ? TargetFace.getFaceMaterialType() : DataManager.INVALID_INDEX;
	}

	public void setFaceSurfaceType(MapCoordinate TargetMapCoordinates, Direction DirectionType, short SurfaceID) {
		Pair< MapCoordinate, Direction > ConvertedValues = FaceCoordinateConvertion(TargetMapCoordinates, DirectionType);

		Cell TargetCell = getCell(new CellCoordinate(ConvertedValues.getValue0()));
		if (TargetCell != null) {
			TargetCell.setFaceSurfaceType(new FaceCoordinate(ConvertedValues.getValue0().CubeByteIndex(), ConvertedValues.getValue1()), SurfaceID);
		}
	}

	public short getFaceSurfaceType(MapCoordinate TargetMapCoordinates, Direction DirectionType) {
		Face TargetFace = getFace(TargetMapCoordinates, DirectionType);
		return TargetFace != null ? TargetFace.getFaceSurfaceType() : DataManager.INVALID_INDEX;
	}

	public boolean isCubeHidden(MapCoordinate Coordinates) {
		Cell TargetCell = getCubeOwner(Coordinates);
		return TargetCell != null ? TargetCell.isCubeHidden(Coordinates.CubeByteIndex()) : true;
	}

	public void setCubeHidden(MapCoordinate Coordinates, boolean NewValue) {
		Cell TargetCell = getCubeOwner(Coordinates);

		if(TargetCell != null)
			TargetCell.setCubeHidden(Coordinates.CubeByteIndex(), NewValue);	
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
	public void ExcavateCube(MapCoordinate Coordinates, CubeShape GoalShape) {
		int Corner = ExcavateDice.Roll(0, Direction.CARDINAL_DIRECTIONS.length - 1);
		CubeShape OldShape = getCubeShape(Coordinates);
		CubeShape IntermediateShape;
		
		switch (Corner) {
			case 0:
				if (GoalShape.NorthEastCorner() < OldShape.NorthEastCorner()) {
					IntermediateShape = OldShape.clone();
					IntermediateShape.setNorthEastCorner((byte) (OldShape.NorthEastCorner() - 1));
					UpdateCubeShape(Coordinates, IntermediateShape);
					break;
				}
			case 1:
				if (GoalShape.SouthEastCorner() < OldShape.SouthEastCorner()) {
					IntermediateShape = OldShape.clone();
					IntermediateShape.setSouthEastCorner((byte) (OldShape.SouthEastCorner() - 1));
					UpdateCubeShape(Coordinates, IntermediateShape);
					break;
				}
			case 2:
				if (GoalShape.NorthWestCorner() < OldShape.NorthWestCorner()) {
					IntermediateShape = OldShape.clone();
					IntermediateShape.setNorthWestCorner((byte) (OldShape.NorthWestCorner() - 1));
					UpdateCubeShape(Coordinates, IntermediateShape);
					break;
				}
			case 3:
				if (GoalShape.SouthWestCorner() < OldShape.SouthWestCorner()) {
					IntermediateShape = OldShape.clone();
					IntermediateShape.setSouthWestCorner((byte) (OldShape.SouthWestCorner() - 1));
					UpdateCubeShape(Coordinates, IntermediateShape);
					break;
				}
			default:
				//UpdateCubeShape(Coordinates, GoalShape);				
		}
	}

	public void UpdateCubeShape(MapCoordinate Coordinates, CubeShape NewShape) {
		if (isCubeInited(Coordinates)) {
			CubeShape Shape = getCubeShape(Coordinates);
			if (!Shape.equals(NewShape)) {
				
				// check bottoms
				MapCoordinate belowCube = Coordinates.clone();
				belowCube.TranslateMapCoordinates(Direction.DIRECTION_DOWN);
				if (!isCubeInited(belowCube)) {
					InitializeCell(new CellCoordinate(belowCube));
				}
				CubeShape belowShape = getCubeShape(belowCube);

				if (belowShape.NorthEastCorner() < CubeShape.CUBE_TOP_HEIGHT)
					NewShape.setNorthEastCorner(CubeShape.BELOW_CUBE_HEIGHT);

				if (belowShape.NorthWestCorner() < CubeShape.CUBE_TOP_HEIGHT)
					NewShape.setNorthWestCorner(CubeShape.BELOW_CUBE_HEIGHT);

				if (belowShape.SouthEastCorner() < CubeShape.CUBE_TOP_HEIGHT)
					NewShape.setSouthEastCorner(CubeShape.BELOW_CUBE_HEIGHT);

				if (belowShape.SouthWestCorner() < CubeShape.CUBE_TOP_HEIGHT)
					NewShape.setSouthWestCorner(CubeShape.BELOW_CUBE_HEIGHT);

						
				setCubeShape(Coordinates, NewShape);
				if (NewShape.isEmpty()) {
					setCubeMaterial(Coordinates, DataManager.INVALID_INDEX);
				}

				for (Direction DirectionType : Direction.AXIAL_DIRECTIONS) {
					UpdateFace(Coordinates, DirectionType);
					MapCoordinate AdjacentCoords = Coordinates.clone();
					AdjacentCoords.TranslateMapCoordinates(DirectionType);
					UpdateFace(AdjacentCoords, DirectionType.Invert());
				}
				UpdateFace(Coordinates, Direction.DIRECTION_NONE);
				setCubeHidden(Coordinates, false);

				// check and push changes above
				MapCoordinate aboveCube = Coordinates.clone();
				aboveCube.TranslateMapCoordinates(Direction.DIRECTION_UP);
				if (isCubeInited(aboveCube)) {
					CubeShape aboveShape = getCubeShape(aboveCube).clone();
					CubeShape NewAboveShape = aboveShape.clone();

					if (aboveShape.split()) {
						if (NewShape.NorthEastCorner() < CubeShape.CUBE_TOP_HEIGHT) {
							if (aboveShape.NorthEastCorner() == CubeShape.CUBE_BOTTOM_HEIGHT && aboveShape.NorthWestCorner() == CubeShape.CUBE_BOTTOM_HEIGHT && aboveShape.SouthEastCorner() == CubeShape.CUBE_BOTTOM_HEIGHT)
								NewAboveShape.setNorthEastCorner(CubeShape.BELOW_CUBE_HEIGHT);
						}

						if (NewShape.SouthWestCorner() < CubeShape.CUBE_TOP_HEIGHT) {
							if (aboveShape.SouthWestCorner() == CubeShape.CUBE_BOTTOM_HEIGHT && aboveShape.NorthWestCorner() == CubeShape.CUBE_BOTTOM_HEIGHT && aboveShape.SouthEastCorner() == CubeShape.CUBE_BOTTOM_HEIGHT)
								NewAboveShape.setSouthWestCorner(CubeShape.BELOW_CUBE_HEIGHT);
						}
					} else {
						if (NewShape.NorthWestCorner() < CubeShape.CUBE_TOP_HEIGHT) {
							if (aboveShape.NorthEastCorner() == CubeShape.CUBE_BOTTOM_HEIGHT && aboveShape.NorthWestCorner() == CubeShape.CUBE_BOTTOM_HEIGHT && aboveShape.SouthWestCorner() == CubeShape.CUBE_BOTTOM_HEIGHT)
								NewAboveShape.setNorthWestCorner(CubeShape.BELOW_CUBE_HEIGHT);
						}

						if (NewShape.SouthEastCorner() < CubeShape.CUBE_TOP_HEIGHT) {
						if (aboveShape.NorthEastCorner() == CubeShape.CUBE_BOTTOM_HEIGHT && aboveShape.SouthWestCorner() == CubeShape.CUBE_BOTTOM_HEIGHT && aboveShape.SouthEastCorner() == CubeShape.CUBE_BOTTOM_HEIGHT)
							NewAboveShape.setSouthEastCorner(CubeShape.BELOW_CUBE_HEIGHT);
						}
					}

					if (!aboveShape.equals(NewAboveShape))
						setCubeShape(aboveCube, aboveShape);
					if (NewAboveShape.isEmpty()) {
						setCubeMaterial(aboveCube, DataManager.INVALID_INDEX);
					}

					for (Direction DirectionType : Direction.AXIAL_DIRECTIONS) {
						UpdateFace(aboveCube, DirectionType);
					}
					UpdateFace(aboveCube, Direction.DIRECTION_NONE);
					setCubeHidden(aboveCube, false);
				}	
			}

			// reveal tiles around
			//for(Direction DirectionType = COMPASS_DIRECTIONS_START; DirectionType < NUM_COMPASS_DIRECTIONS; ++DirectionType)
			//{
			//    setCubeHidden(MapCoordinates(Coordinates, DirectionType), false);
			//}
		}
	}

	public void UpdateFace(MapCoordinate TargetCoordinates, Direction DirectionType) {
		DataManager Data = DataManager.getDataManager();
		final short RoughWallID = Data.getLabelIndex("SURFACETYPE_ROUGH_WALL");
		final short RoughFloorID = Data.getLabelIndex("SURFACETYPE_ROUGH_FLOOR_1");

		MapCoordinate ModifiedCoordinates = TargetCoordinates.clone();
		ModifiedCoordinates.TranslateMapCoordinates(DirectionType);

		if (!isCubeInited(ModifiedCoordinates)){
			// Init it
			return;
		}

		CubeShape SourceShape = getCubeShape(TargetCoordinates);
		CubeShape AdjacentShape = getCubeShape(ModifiedCoordinates);
		Face TargetFace = getFace(TargetCoordinates, DirectionType);

		switch (DirectionType) {

			case DIRECTION_NONE:
				if (!SourceShape.isEmpty() && !SourceShape.isSolid()) {
					if (TargetFace == null) {
						TargetFace = addFace(TargetCoordinates, Direction.DIRECTION_NONE);
						// Dirty Cell if edge case
					}

					TargetFace.setFaceMaterialType(getCubeMaterial(TargetCoordinates));
					TargetFace.setFaceShapeType(new FaceShape(getCubeShape(TargetCoordinates), null, Direction.DIRECTION_NONE));
					TargetFace.setFaceSurfaceType(RoughFloorID);

				} else {
					removeFace(TargetCoordinates, DirectionType);
				}
				break;

			case DIRECTION_DOWN:
				if (AdjacentShape.hasCeiling() || SourceShape.hasFloor()) {
					if (TargetFace == null) {
						TargetFace = addFace(TargetCoordinates, DirectionType);
					}

					TargetFace.setFaceMaterialType(getCubeMaterial(ModifiedCoordinates));
					TargetFace.setFaceSurfaceType(RoughFloorID);
					TargetFace.setFaceShapeType(new FaceShape(SourceShape, AdjacentShape, DirectionType));
				} else {
					removeFace(TargetCoordinates, DirectionType);
				}
				break;

			case DIRECTION_UP:
				if (AdjacentShape.hasFloor() || SourceShape.hasCeiling()) {
					if (TargetFace == null) {
						TargetFace = addFace(TargetCoordinates, DirectionType);
					}

					TargetFace.setFaceMaterialType(getCubeMaterial(TargetCoordinates));
					TargetFace.setFaceSurfaceType(RoughFloorID);
					TargetFace.setFaceShapeType(new FaceShape(SourceShape, AdjacentShape, DirectionType));
				} else {
					removeFace(TargetCoordinates, DirectionType);
				}
				break;

			case DIRECTION_EAST:
			case DIRECTION_WEST:
			case DIRECTION_NORTH:
			case DIRECTION_SOUTH:
				if (SourceShape.hasFace(DirectionType)) { // || AdjacentShape.hasFace(DirectionType.Invert())) {
					if (TargetFace == null) {
						TargetFace = addFace(TargetCoordinates, DirectionType);
					}

					TargetFace.setFaceMaterialType(getCubeMaterial(TargetCoordinates));
					TargetFace.setFaceShapeType(new FaceShape(SourceShape, AdjacentShape, DirectionType));
					TargetFace.setFaceSurfaceType(RoughWallID);
				} else {
					removeFace(TargetCoordinates, DirectionType);
				}
				break;

			default:
				break;
		}
	}

	public void ReleaseMap() {
		MapLoaded = false;

		Cells.clear();
		Zones.clear();
	}

	public Zone createZone(ArrayList<VolumeSelection> Volumes) {
		Zone NewZone = new Zone(Volumes, ZoneCounter++);
		Zones.add(NewZone);
		return NewZone;
	}

	public ArrayList<Zone> getZonesAt(MapCoordinate TestCoordinates) {
		ArrayList<Zone> Collection = new ArrayList<Zone>();
		for(Zone Z : Zones) {
			if (Z.isCoordinateInZone(TestCoordinates))
				Collection.add(Z);
		}
		return Collection;
	}
	
	public ArrayList<Zone> getZones() {
		return Zones;
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