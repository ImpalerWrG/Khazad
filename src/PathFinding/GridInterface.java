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

package PathFinding;

import java.util.BitSet;
import java.util.ArrayList;

import Map.MapCoordinate;
import Map.Direction;

/**
 * Basic interface for Pathfinding Grids, all PathAlgorithms operate on a Grid
 * and querry it for Edges. In addition before any PathAlgorithm is even called
 * the test for isPathPossible should be run to see if the start and goal points
 * even connect.
 *
 * Implementation will usually consist of HashMaps of blocks that
 * contain the Edge and Node information, some kind of connectivity mapping
 * is needed to provide quick answers to the PathPossible querry.
 *
 * The BitSet returned by getDirectionEdgeSet is sized to correspond to the
 * Direction Enum with true bits indicationg that a valid edge exists from
 * that Coordinate towards the neibhor in that direction, this helps eliminate
 * many checks for edge costs when no edge exists.
 *
 * @author Impaler
 */
public interface GridInterface {

	float getEdgeCost(MapCoordinate TestCoords, Direction DirectionType);

	BitSet getDirectionEdgeSet(MapCoordinate TargetCoords);

	public int getConnectivityZone(MapCoordinate TargetCoords);

	boolean isPathPossible(MovementModality MovementType, MapCoordinate StartCoords, MapCoordinate GoalCoords);

	abstract void dirtyMapCoordinate(MapCoordinate[] TargetCoords);

	boolean contains(MapCoordinate TestCoords);

	ArrayList<MapCoordinate> getPassableCoordinates();
}
