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

import Map.Coordinates.*;
import Core.Dice;
import Core.Main;
import Data.DataManager;
import Game.Game;
import PathFinding.PathManager;

import java.util.concurrent.ConcurrentHashMap;
import java.util.ArrayList;
import java.util.Collection;

import Interface.VolumeSelection;
import java.io.Serializable;

import org.javatuples.Pair;

/**
 * Master Class for holding the playing Map, holds primaraly Cells in HashMap
 * structure and pipes changes to the Cells/Faces to abstract them away from the
 * rest of the code base
 *
 * @author Impaler
 */
public class GameMap implements Serializable {

	private static final long serialVersionUID = 1;
	boolean Initialized;
	boolean MapLoaded;
	// Storage of Cells
	ConcurrentHashMap<CellCoordinate, Cell> Cells;
	ConcurrentHashMap<CellCoordinate, Cell> WeatherCells;
	ConcurrentHashMap<CellCoordinate, Cell> BasementCells;

	int HighestCell;
	int LowestCell;
	int WestestCell;
	int EastestCell;
	int NorthestCell;
	int SouthestCell;

	CellCoordinate TestingCooords = new CellCoordinate();

	int Seed;
	Dice ExcavateDice = new Dice();
	ArrayList<Zone> Zones;

	public GameMap() {
		Initialized = false;
		MapLoaded = false;
		HighestCell = -100000000;
		LowestCell = 10000000;
		WestestCell = 10000000;
		EastestCell = -100000000;
		NorthestCell = -100000000;
		SouthestCell = 10000000;

		ExcavateDice.seed(Seed);
		Zones = new ArrayList<Zone>();
		Cells = new ConcurrentHashMap<CellCoordinate, Cell>();
		WeatherCells = new ConcurrentHashMap<CellCoordinate, Cell>();
		BasementCells = new ConcurrentHashMap<CellCoordinate, Cell>();
	}

	public static GameMap getMap() {
		Game game = Main.app.getStateManager().getState(Game.class);
		return game.getMap();
	}

	public void initialize(int MasterSeed) {
		Seed = MasterSeed;
		ExcavateDice.seed(Seed);
	}

	public Cell getCell(CellCoordinate TestCoords) {
		return Cells.get(TestCoords);
	}

	public boolean insertCell(Cell NewCell) {
		CellCoordinate TargetCoordinates = NewCell.getCellCoordinates();
		if (getCell(TargetCoordinates) == null) {
			Cells.put(TargetCoordinates, NewCell);

			CellCoordinate AboveCoords = TargetCoordinates.clone();
			AboveCoords.Z++;
			Cell AboveCell = Cells.get(AboveCoords);

			CellCoordinate BelowCoords = TargetCoordinates.clone();
			BelowCoords.Z--;
			Cell BelowCell = Cells.get(BelowCoords);

			if (TargetCoordinates.Z > HighestCell)
				HighestCell = TargetCoordinates.Z;

			if (AboveCell == null) {
				WeatherCells.put(TargetCoordinates, NewCell);
				//NewCell.WeatherCell = true;
				if (BelowCell != null) {
					WeatherCells.remove(BelowCoords);
					//BelowCell.WeatherCell = false;
				}
			}

			if (TargetCoordinates.Z < LowestCell)
				LowestCell = TargetCoordinates.Z;

			if (TargetCoordinates.X > EastestCell)
				EastestCell = TargetCoordinates.X;
			if (TargetCoordinates.X < WestestCell)
				WestestCell = TargetCoordinates.X;
			if (TargetCoordinates.Y < SouthestCell)
				SouthestCell = TargetCoordinates.Y;
			if (TargetCoordinates.Y > NorthestCell)
				NorthestCell = TargetCoordinates.Y;

			if (BelowCell == null) {
				BasementCells.put(TargetCoordinates, NewCell);
				//NewCell.BasementCell = true;
				if (AboveCell != null) {
					BasementCells.remove(AboveCoords);
					//AboveCell.BasementCell = false;
				}
			}

			return true;
		}
		return false;  // A Cell already exists at that spot
	}

	public Cell initializeCell(CellCoordinate Coords) {
		Cell TargetCell = getCell(Coords);
		if (TargetCell == null) {
			TargetCell = new Cell();
			TargetCell.setCellCoordinates(Coords);
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

	public CubeCoordinate getMapCenter() {
		int X = (EastestCell - WestestCell) / 2;
		int Y = (NorthestCell - SouthestCell) / 2;

		return new CubeCoordinate(X * CubeCoordinate.CELLEDGESIZE, Y * CubeCoordinate.CELLEDGESIZE, 0);
	}

	public Cell getCubeOwner(CubeCoordinate Coordinates) {
		TestingCooords.resolveCube(Coordinates);
		return getCell(TestingCooords);
	}

	public boolean isCubeInited(CubeCoordinate Coordinates) {
		return getCubeOwner(Coordinates) != null;
	}

	public boolean isCubeInited(MapCoordinate Coordinates) {
		return Cells.containsKey(Coordinates.Cell);
	}

	public CubeCoordinate getFacingCoordinates(CellCoordinate cellcoords, FaceCoordinate facecoords) {
		CubeCoordinate ModifiedCoordinates = new CubeCoordinate(cellcoords, facecoords.getCoordinates());
		ModifiedCoordinates.translate(facecoords.getFaceDirection());
		return ModifiedCoordinates;
	}

	public Face getFace(CubeCoordinate TargetMapCoordinates, Direction DirectionType) {
		Cell TargetCell = getCell(new CellCoordinate(TargetMapCoordinates));
		return TargetCell != null ? TargetCell.getFace(new FaceCoordinate(TargetMapCoordinates.getCubeIndex(), DirectionType), 0) : null;
	}

	public boolean hasFace(CubeCoordinate TargetMapCoordinates, Direction DirectionType) {
		Cell TargetCell = getCell(new CellCoordinate(TargetMapCoordinates));
		return TargetCell != null ? TargetCell.hasFace(new FaceCoordinate(TargetMapCoordinates.getCubeIndex(), DirectionType), 0) : false;
	}

	public boolean removeFace(CubeCoordinate TargetMapCoordinates, Direction DirectionType) {
		Cell TargetCell = getCell(new CellCoordinate(TargetMapCoordinates));
		return TargetCell != null ? TargetCell.removeFace(new FaceCoordinate(TargetMapCoordinates.getCubeIndex(), DirectionType), 0) : false;
	}

	public Face addFace(CubeCoordinate TargetMapCoordinates, Direction DirectionType, int LevelofDetail) {
		Cell TargetCell = getCell(new CellCoordinate(TargetMapCoordinates));
		if (TargetCell == null)
			TargetCell = initializeCell(new CellCoordinate(TargetMapCoordinates));

		return TargetCell.addFace(new FaceCoordinate(TargetMapCoordinates.getCubeIndex(), DirectionType, (byte) LevelofDetail));
	}

	public Face addFace(MapCoordinate TargetMapCoordinates, Direction DirectionType, int LevelofDetail) {
		Cell TargetCell = getCell(TargetMapCoordinates.Cell);
		if (TargetCell == null)
			TargetCell = initializeCell(TargetMapCoordinates.Cell);

		return TargetCell.addFace(new FaceCoordinate(TargetMapCoordinates.Cube, DirectionType, (byte) LevelofDetail));
	}

	public void setCubeShape(CubeCoordinate Coordinate, CubeShape NewShape) {
		Cell TargetCell = getCubeOwner(Coordinate);

		if (TargetCell != null) {
			TargetCell.setCubeShape(Coordinate.getCubeIndex(), NewShape);
			CubeCoordinate[] Coordinates = {Coordinate};
			PathManager.getSingleton().editMapAbstractions(Coordinates);
		}
	}

	public CubeShape getCubeShape(CubeCoordinate Coordinates) {
		Cell TargetCell = getCubeOwner(Coordinates);

		if (TargetCell != null) {
			return TargetCell.getCubeShape(Coordinates.getCubeIndex(), 0);
		} else {
			return new CubeShape(CubeShape.BELOW_CUBE_HEIGHT);
		}
	}

	public CubeShape getCubeShape(MapCoordinate Coordinates) {
		Cell TargetCell = Cells.get(Coordinates.Cell);

		if (TargetCell != null) {
			return TargetCell.getCubeShape(Coordinates.Cube.getCubeIndex(), 0);
		} else {
			return new CubeShape(CubeShape.BELOW_CUBE_HEIGHT);
		}
	}

	public void setCubeMaterial(CubeCoordinate Coordinates, short MaterialID) {
		Cell TargetCell = getCubeOwner(Coordinates);

		if (TargetCell != null) {
			TargetCell.setCubeMaterial(Coordinates.getCubeIndex(), MaterialID);
		}
	}

	public short getCubeMaterial(CubeCoordinate Coordinates) {
		Cell TargetCell = getCubeOwner(Coordinates);
		return TargetCell != null ? TargetCell.getCubeMaterial(Coordinates.getCubeIndex()) : DataManager.INVALID_INDEX;
	}

	public short getCubeMaterial(MapCoordinate Coordinates) {
		Cell TargetCell = Cells.get(Coordinates.Cell);
		return TargetCell != null ? TargetCell.getCubeMaterial(Coordinates.Cube.getCubeIndex()) : DataManager.INVALID_INDEX;
	}

	public void setFaceMaterial(CubeCoordinate TargetMapCoordinates, Direction DirectionType, short MaterialID) {
		Cell TargetCell = getCell(new CellCoordinate(TargetMapCoordinates));
		if (TargetCell != null) {
			TargetCell.setFaceMaterialType(new FaceCoordinate(TargetMapCoordinates.getCubeIndex(), DirectionType), MaterialID);
		}
	}

	public short getFaceMaterial(CubeCoordinate TargetMapCoordinates, Direction DirectionType) {
		Face TargetFace = getFace(TargetMapCoordinates, DirectionType);
		return TargetFace != null ? TargetFace.getFaceMaterialType() : DataManager.INVALID_INDEX;
	}

	public void setFaceSurfaceType(CubeCoordinate TargetMapCoordinates, Direction DirectionType, short SurfaceID) {
		Cell TargetCell = getCell(new CellCoordinate(TargetMapCoordinates));
		if (TargetCell != null) {
			TargetCell.setFaceSurfaceType(new FaceCoordinate(TargetMapCoordinates.getCubeIndex(), DirectionType), SurfaceID);
		}
	}

	public short getFaceSurfaceType(CubeCoordinate TargetMapCoordinates, Direction DirectionType) {
		Face TargetFace = getFace(TargetMapCoordinates, DirectionType);
		return TargetFace != null ? TargetFace.getFaceSurfaceType() : DataManager.INVALID_INDEX;
	}

	public boolean isCubeHidden(CubeCoordinate Coordinates) {
		Cell TargetCell = getCubeOwner(Coordinates);
		return TargetCell != null ? TargetCell.isCubeHidden(Coordinates.getCubeIndex()) : true;
	}

	public void setCubeHidden(CubeCoordinate Coordinates, boolean NewValue) {
		Cell TargetCell = getCubeOwner(Coordinates);
		if (TargetCell != null)
			TargetCell.setCubeHidden(Coordinates.getCubeIndex(), NewValue);
	}

	public void generateFirstLight() {
		for (Cell WeatherCell : WeatherCells.values()) {
			CellCoordinate TargetCoords = WeatherCell.getCellCoordinates();
			Cell TopCell = Cells.get(TargetCoords);
			TargetCoords.Z--;
			Cell BottomCell = Cells.get(TargetCoords);

			boolean LightRemains = false;

			for (int i = 0; i < CubeCoordinate.CUBESPERCELL; i++) {
				TopCell.setCubeSunLit((short) i, true);
			}

			do {
				for (int i = 0; i < CubeCoordinate.CUBESPERCELL; i++) {
					if (TopCell.isCubeSunLit((short) i) && !TopCell.getCubeShape((short) i, 0).hasFace(Direction.DIRECTION_NONE)) {
						BottomCell.setCubeSunLit((short) i, true);
						LightRemains = true;
					}
				}

				TopCell = BottomCell;
				TargetCoords.Z--;
				BottomCell = Cells.get(TargetCoords);
			} while (BottomCell != null && LightRemains);
		}
	}

	public boolean isCubeSubTerranean(CubeCoordinate Coordinates) {
		Cell TargetCell = getCubeOwner(Coordinates);
		return TargetCell != null ? TargetCell.isCubeSubTerranean(Coordinates.getCubeIndex()) : false;
	}

	public void setCubeSubTerranean(CubeCoordinate Coordinates, boolean NewValue) {
		Cell TargetCell = getCubeOwner(Coordinates);
		if (TargetCell != null)
			TargetCell.setCubeSubTerranean(Coordinates.getCubeIndex(), NewValue);
	}

	public boolean isCubeSkyView(CubeCoordinate Coordinates) {
		Cell TargetCell = getCubeOwner(Coordinates);
		return TargetCell != null ? TargetCell.isCubeSkyView(Coordinates.getCubeIndex()) : false;
	}

	public void setCubeSkyView(CubeCoordinate Coordinates, boolean NewValue) {
		Cell TargetCell = getCubeOwner(Coordinates);
		if (TargetCell != null)
			TargetCell.setCubeSkyView(Coordinates.getCubeIndex(), NewValue);
	}

	public boolean isCubeSunLit(CubeCoordinate Coordinates) {
		Cell TargetCell = getCubeOwner(Coordinates);
		return TargetCell != null ? TargetCell.isCubeSunLit(Coordinates.getCubeIndex()) : false;
	}

	public boolean isCubeSunLit(MapCoordinate Coordinates) {
		Cell TargetCell = Cells.get(Coordinates.Cell);
		return TargetCell != null ? TargetCell.isCubeSunLit(Coordinates.Cube.getCubeIndex()) : false;
	}

	public void setCubeSunLit(CubeCoordinate Coordinates, boolean NewValue) {
		Cell TargetCell = getCubeOwner(Coordinates);
		if (TargetCell != null)
			TargetCell.setCubeSunLit(Coordinates.getCubeIndex(), NewValue);
	}

	public void excavateCube(CubeCoordinate Coordinates, CubeShape GoalShape) {
		int Corner = ExcavateDice.roll(0, Direction.CARDINAL_DIRECTIONS.length - 1);
		CubeShape OldShape = getCubeShape(Coordinates);
		CubeShape IntermediateShape;

		switch (Corner) {
			case 0:
				if (GoalShape.getNorthEastCorner() < OldShape.getNorthEastCorner()) {
					IntermediateShape = OldShape.clone();
					IntermediateShape.setNorthEastCorner((byte) (OldShape.getNorthEastCorner() - 1));
					updateCubeShape(Coordinates, IntermediateShape);
					break;
				}
			case 1:
				if (GoalShape.getSouthEastCorner() < OldShape.getSouthEastCorner()) {
					IntermediateShape = OldShape.clone();
					IntermediateShape.setSouthEastCorner((byte) (OldShape.getSouthEastCorner() - 1));
					updateCubeShape(Coordinates, IntermediateShape);
					break;
				}
			case 2:
				if (GoalShape.getNorthWestCorner() < OldShape.getNorthWestCorner()) {
					IntermediateShape = OldShape.clone();
					IntermediateShape.setNorthWestCorner((byte) (OldShape.getNorthWestCorner() - 1));
					updateCubeShape(Coordinates, IntermediateShape);
					break;
				}
			case 3:
				if (GoalShape.getSouthWestCorner() < OldShape.getSouthWestCorner()) {
					IntermediateShape = OldShape.clone();
					IntermediateShape.setSouthWestCorner((byte) (OldShape.getSouthWestCorner() - 1));
					updateCubeShape(Coordinates, IntermediateShape);
					break;
				}
			default:
			//UpdateCubeShape(Coordinates, GoalShape);				
		}
		// Always set material to native cube if were excavating
		Face SloppedFace = getFace(Coordinates, Direction.DIRECTION_NONE);
		if (SloppedFace != null)
			SloppedFace.setFaceMaterialType(getCubeMaterial(Coordinates));
	}

	public void updateCubeShape(CubeCoordinate Coordinates, CubeShape NewShape) {
		if (isCubeInited(Coordinates)) {
			CubeShape Shape = getCubeShape(Coordinates);
			if (!Shape.equals(NewShape)) {

				// check bottoms
				CubeCoordinate belowCube = Coordinates.clone();
				belowCube.translate(Direction.DIRECTION_DOWN);
				if (!isCubeInited(belowCube)) {
					initializeCell(new CellCoordinate(belowCube));
				}
				CubeShape belowShape = getCubeShape(belowCube);

				if (belowShape.getNorthEastCorner() < CubeShape.CUBE_TOP_HEIGHT)
					NewShape.setNorthEastCorner(CubeShape.BELOW_CUBE_HEIGHT);

				if (belowShape.getNorthWestCorner() < CubeShape.CUBE_TOP_HEIGHT)
					NewShape.setNorthWestCorner(CubeShape.BELOW_CUBE_HEIGHT);

				if (belowShape.getSouthEastCorner() < CubeShape.CUBE_TOP_HEIGHT)
					NewShape.setSouthEastCorner(CubeShape.BELOW_CUBE_HEIGHT);

				if (belowShape.getSouthWestCorner() < CubeShape.CUBE_TOP_HEIGHT)
					NewShape.setSouthWestCorner(CubeShape.BELOW_CUBE_HEIGHT);


				setCubeShape(Coordinates, NewShape);
				if (NewShape.isEmpty()) {
					setCubeMaterial(Coordinates, DataManager.INVALID_INDEX);
				}

				for (Direction DirectionType : Direction.AXIAL_DIRECTIONS) {
					updateFace(Coordinates, DirectionType);
					CubeCoordinate AdjacentCoords = Coordinates.clone();
					AdjacentCoords.translate(DirectionType);
					updateFace(AdjacentCoords, DirectionType.invert());
				}
				updateFace(Coordinates, Direction.DIRECTION_NONE);
				setCubeHidden(Coordinates, false);

				// check and push changes above
				CubeCoordinate aboveCube = Coordinates.clone();
				aboveCube.translate(Direction.DIRECTION_UP);
				if (isCubeInited(aboveCube)) {
					CubeShape aboveShape = getCubeShape(aboveCube).clone();
					CubeShape NewAboveShape = aboveShape.clone();

					if (NewShape.getNorthEastCorner() < CubeShape.CUBE_TOP_HEIGHT) {
						if (aboveShape.getNorthEastCorner() == CubeShape.CUBE_BOTTOM_HEIGHT && aboveShape.getNorthWestCorner() == CubeShape.CUBE_BOTTOM_HEIGHT && aboveShape.getSouthEastCorner() == CubeShape.CUBE_BOTTOM_HEIGHT)
							NewAboveShape.setNorthEastCorner(CubeShape.BELOW_CUBE_HEIGHT);
					}

					if (NewShape.getSouthWestCorner() < CubeShape.CUBE_TOP_HEIGHT) {
						if (aboveShape.getSouthWestCorner() == CubeShape.CUBE_BOTTOM_HEIGHT && aboveShape.getNorthWestCorner() == CubeShape.CUBE_BOTTOM_HEIGHT && aboveShape.getSouthEastCorner() == CubeShape.CUBE_BOTTOM_HEIGHT)
							NewAboveShape.setSouthWestCorner(CubeShape.BELOW_CUBE_HEIGHT);
					}

					if (NewShape.getNorthWestCorner() < CubeShape.CUBE_TOP_HEIGHT) {
						if (aboveShape.getNorthEastCorner() == CubeShape.CUBE_BOTTOM_HEIGHT && aboveShape.getNorthWestCorner() == CubeShape.CUBE_BOTTOM_HEIGHT && aboveShape.getSouthWestCorner() == CubeShape.CUBE_BOTTOM_HEIGHT)
							NewAboveShape.setNorthWestCorner(CubeShape.BELOW_CUBE_HEIGHT);
					}

					if (NewShape.getSouthEastCorner() < CubeShape.CUBE_TOP_HEIGHT) {
						if (aboveShape.getNorthEastCorner() == CubeShape.CUBE_BOTTOM_HEIGHT && aboveShape.getSouthEastCorner() == CubeShape.CUBE_BOTTOM_HEIGHT && aboveShape.getSouthWestCorner() == CubeShape.CUBE_BOTTOM_HEIGHT)
							NewAboveShape.setSouthEastCorner(CubeShape.BELOW_CUBE_HEIGHT);
					}

					if (!aboveShape.equals(NewAboveShape))
						setCubeShape(aboveCube, NewAboveShape);
					if (NewAboveShape.isEmpty()) {
						setCubeMaterial(aboveCube, DataManager.INVALID_INDEX);
					}

					for (Direction DirectionType : Direction.AXIAL_DIRECTIONS) {
						updateFace(aboveCube, DirectionType);
					}
					updateFace(aboveCube, Direction.DIRECTION_NONE);
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

	public void updateFace(CubeCoordinate TargetCoordinates, Direction DirectionType) {
		DataManager Data = DataManager.getDataManager();
		final short RoughWallID = Data.getLabelIndex("SURFACETYPE_ROUGH_WALL");
		final short RoughFloorID = Data.getLabelIndex("SURFACETYPE_ROUGH_FLOOR_1");

		CubeCoordinate ModifiedCoordinates = TargetCoordinates.clone();
		ModifiedCoordinates.translate(DirectionType);

		if (!isCubeInited(ModifiedCoordinates)) {
			// Init it
			return;
		}

		CubeShape SourceShape = getCubeShape(TargetCoordinates);
		CubeShape AdjacentShape = getCubeShape(ModifiedCoordinates);
		Face TargetFace = getFace(TargetCoordinates, DirectionType);

		switch (DirectionType) {

			case DIRECTION_NONE:
				if (!SourceShape.isEmpty() && !SourceShape.isSolid()) {
					FaceShape NewShape = new FaceShape(getCubeShape(TargetCoordinates), null, Direction.DIRECTION_NONE);
					if (TargetFace == null) {
						TargetFace = addFace(TargetCoordinates, Direction.DIRECTION_NONE, 0);
						TargetFace.setFaceShapeType(NewShape);
						TargetFace.setFaceSurfaceType(RoughFloorID);
						TargetFace.setFaceMaterialType(getCubeMaterial(TargetCoordinates));
						// Dirty Cell if edge case
					} else {
						if (!TargetFace.getFaceShapeType().equals(NewShape)) {
							TargetFace.setFaceShapeType(NewShape);
							TargetFace.setFaceSurfaceType(RoughFloorID);
							TargetFace.setFaceMaterialType(getCubeMaterial(TargetCoordinates));
						}
					}
				} else {
					removeFace(TargetCoordinates, DirectionType);
				}
				break;

			case DIRECTION_DOWN:
				if (SourceShape.hasFloor()) {
					FaceShape NewShape = new FaceShape(SourceShape, AdjacentShape, DirectionType);
					if (TargetFace == null) {
						TargetFace = addFace(TargetCoordinates, DirectionType, 0);
						TargetFace.setFaceMaterialType(getCubeMaterial(ModifiedCoordinates));
						TargetFace.setFaceSurfaceType(RoughFloorID);
						TargetFace.setFaceShapeType(NewShape);
					} else {
						if (!TargetFace.getFaceShapeType().equals(NewShape)) {
							TargetFace.setFaceMaterialType(getCubeMaterial(ModifiedCoordinates));
							TargetFace.setFaceSurfaceType(RoughFloorID);
							TargetFace.setFaceShapeType(NewShape);
						}
					}
				} else {
					removeFace(TargetCoordinates, DirectionType);
				}
				break;

			case DIRECTION_UP:
				if (AdjacentShape.hasFloor()) {
					FaceShape NewShape = new FaceShape(SourceShape, AdjacentShape, DirectionType);
					if (TargetFace == null) {
						TargetFace = addFace(TargetCoordinates, DirectionType, 0);
						TargetFace.setFaceMaterialType(getCubeMaterial(TargetCoordinates));
						TargetFace.setFaceSurfaceType(RoughFloorID);
						TargetFace.setFaceShapeType(NewShape);
					} else {
						if (!TargetFace.getFaceShapeType().equals(NewShape)) {
							TargetFace.setFaceMaterialType(getCubeMaterial(TargetCoordinates));
							TargetFace.setFaceSurfaceType(RoughFloorID);
							TargetFace.setFaceShapeType(NewShape);
						}
					}
				} else {
					removeFace(TargetCoordinates, DirectionType);
				}
				break;

			case DIRECTION_EAST:
			case DIRECTION_WEST:
			case DIRECTION_NORTH:
			case DIRECTION_SOUTH:
				if (SourceShape.hasFace(DirectionType)) {
					FaceShape NewShape = new FaceShape(SourceShape, AdjacentShape, DirectionType);
					if (TargetFace == null) {
						TargetFace = addFace(TargetCoordinates, DirectionType, 0);
						TargetFace.setFaceMaterialType(getCubeMaterial(TargetCoordinates));
						TargetFace.setFaceShapeType(NewShape);
						TargetFace.setFaceSurfaceType(RoughWallID);
					} else {
						if (!TargetFace.getFaceShapeType().equals(NewShape)) {
							TargetFace.setFaceMaterialType(getCubeMaterial(TargetCoordinates));
							TargetFace.setFaceShapeType(NewShape);
							TargetFace.setFaceSurfaceType(RoughWallID);
						}
					}
				} else {
					removeFace(TargetCoordinates, DirectionType);
				}
				break;

			default:
				break;
		}
	}

	public void releaseMap() {
		MapLoaded = false;

		Cells.clear();
		Zones.clear();
	}

	public Zone createZone(ArrayList<VolumeSelection> Volumes) {
		Zone NewZone = new Zone(Volumes, Zones.size());
		Zones.add(NewZone);
		return NewZone;
	}

	public ArrayList<Zone> getZonesAt(CubeCoordinate TestCoordinates) {
		ArrayList<Zone> Collection = new ArrayList<Zone>();
		for (Zone Z : Zones) {
			if (Z.isCoordinateInZone(TestCoordinates))
				Collection.add(Z);
		}
		return Collection;
	}

	public ArrayList<Zone> getZones() {
		return Zones;
	}

	public Collection<Cell> getCellCollection() {
		return Cells.values();
	}
}