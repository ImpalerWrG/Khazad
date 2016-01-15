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
import Core.Main;
import Data.DataManager;
import Game.Game;

import java.util.concurrent.ConcurrentHashMap;
import java.util.ArrayList;
import java.util.Collection;

import Interface.VolumeSelection;
import java.io.Serializable;

/**
 * Master Class for holding the playing Map, holds primaraly Chunk in HashMap
 * structure and pipes changes to the Chunk/Faces to abstract them away from the
 * rest of the code base
 *
 * @author Impaler
 */
public class GameMap implements Serializable {

	private static final long serialVersionUID = 1;
	boolean Initialized;
	boolean MapLoaded;

	ConcurrentHashMap<SectorCoordinate, Sector> Sectors;

	int Seed;
	ArrayList<Zone> Zones;

	public GameMap(int MasterSeed) {
		Seed = MasterSeed;

		Initialized = false;
		MapLoaded = false;

		Zones = new ArrayList<Zone>();
		Sectors = new ConcurrentHashMap<SectorCoordinate, Sector>();	
	}

	public static GameMap getMap() {
		Game game = Main.app.getStateManager().getState(Game.class);
		return game.getMap();
	}

	public boolean isBlockInitialized(MapCoordinate Coordinates) {
		Sector targetSector = Sectors.get(Coordinates.Sector);
		return targetSector != null ? targetSector.isBlockInitialized(Coordinates) : false;
	}

	public void getBlockShape(MapCoordinate Coordinates, BlockShape writeBlock) {
		Sector targetSector = Sectors.get(Coordinates.Sector);
	
		if (targetSector != null) {
			targetSector.getBlockShape(Coordinates, writeBlock);
		} else {
			writeBlock.setData(BlockShape.BELOW_CUBE_HEIGHT);
		}
	}

	public short getBlockMaterial(MapCoordinate Coordinates) {
		Sector targetSector = Sectors.get(Coordinates.Sector);
		return targetSector != null ? targetSector.getBlockMaterial(Coordinates) : DataManager.INVALID_INDEX;
	}

	public Sector getSector(SectorCoordinate Coordinates) {
		Sector targetSector = Sectors.get(Coordinates);
		return targetSector != null ? targetSector : initializeSector(Coordinates);
	}

	private Sector initializeSector(SectorCoordinate Coordinates) {
		Sector NewSector = new Sector(Seed);
		NewSector.setSectorCoordinates(Coordinates);
		Sectors.put(Coordinates, NewSector);
		return NewSector;
	}

	public MapCoordinate getMapCenter() {
		int Eastest = -100000;
		int Northest = -100000;
		int Westest = 100000;
		int Southest = 100000;

		for (Sector sector : Sectors.values()) {
			if (sector.EastestChunk > Eastest)
				Eastest = sector.EastestChunk;
			if (sector.WestestChunk < Westest)
				Westest = sector.WestestChunk;
			if (sector.SouthestChunk < Southest)
				Southest = sector.SouthestChunk;
			if (sector.NorthestChunk > Northest)
				Northest = sector.NorthestChunk;
		}

		int X = (Eastest - Westest) / 2;
		int Y = (Northest - Southest) / 2;

		byte size = BlockCoordinate.CHUNK_EDGE_SIZE / 2;
		BlockCoordinate block = new BlockCoordinate();
		block.set(size, size, size);
		return new MapCoordinate(new ChunkCoordinate(X, Y, 0), block);
	}

	public int getHighestFace() {
		int Highest = -1000000;
		for (Sector targetSector : Sectors.values()) {
			if (targetSector.getHighestFace() > Highest)
				Highest = targetSector.getHighestFace();
		}
		return Highest;
	}

	public int getLowestFace() {
		int Lowest = 1000000;
		for (Sector targetSector : Sectors.values()) {
			if (targetSector.getLowestFace() < Lowest)
				Lowest = targetSector.getLowestFace();
		}
		return Lowest;
	}

	public void generateFirstLight() {
		for (Sector targetSector : Sectors.values()) {
			targetSector.generateFirstLight();
		}
	}

	public Face addFace(MapCoordinate Coordinates, Direction DirectionType) {
		Sector targetSector = Sectors.get(Coordinates.Sector);
		if (targetSector == null)
			targetSector = initializeSector(Coordinates.Sector);

		return targetSector.addFace(Coordinates, DirectionType);
	}

	public boolean isBlockSunLit(MapCoordinate Coordinates) {
		Sector targetSector = Sectors.get(Coordinates.Sector);
		return targetSector != null ? targetSector.isBlockSunLit(Coordinates) : false;
	}

	public boolean excavateBlock(MapCoordinate Coordinates, BlockShape GoalShape) {
		Sector targetSector = Sectors.get(Coordinates.Sector);
		return targetSector != null ? targetSector.excavateBlock(Coordinates, GoalShape) : false;
	}

	public Collection<Sector> getSectorCollection() {
		return Sectors.values();
	}	
	
	public void releaseMap() {
		MapLoaded = false;

		Sectors.clear();
		Zones.clear();
	}

	public Zone createZone(ArrayList<VolumeSelection> Volumes) {
		Zone NewZone = new Zone(Volumes, Zones.size());
		Zones.add(NewZone);
		return NewZone;
	}

	public ArrayList<Zone> getZonesAt(MapCoordinate TestCoordinates) {
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
}