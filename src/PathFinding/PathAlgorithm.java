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

import Map.MapCoordinate;
import java.io.Serializable;

/**
 * Abstract base for all PathFinding Algorithms, all Algorithms need a Grid
 * to work upon, Heristics, Start/Goal points to operate upon.
 *
 * Implementations: AStar
 *
 * @author Impaler
 */
public abstract class PathAlgorithm<T> implements Serializable {

	private static final long serialVersionUID = 1;
	int GraphReads;
	int ExpandedNodes;
	GridInterface SearchGraph;
	MovementModality Modality;
	MapCoordinate StartCoordinates;
	MapCoordinate GoalCoordinates;
	Heuristic MainHeuristic;
	Heuristic TieBreakerHeuristic;
	MapPath FinalPath;

	void setEndPoints(MapCoordinate StartCoords, MapCoordinate GoalCoords) {
		StartCoordinates = StartCoords;
		GoalCoordinates = GoalCoords;
		GraphReads = ExpandedNodes = 0;
		FinalPath = null;
	}

	void setHeuristics(Heuristic PrimaryHeuristic, Heuristic SecondaryHeuristic) {
		MainHeuristic = PrimaryHeuristic;
		TieBreakerHeuristic = SecondaryHeuristic;
	}

	void setModality(MovementModality MovementType) {
		Modality = MovementType;
	}

	int getGraphReads() {
		return GraphReads;
	}

	int getExpandedNodes() {
		return ExpandedNodes;
	}

	abstract boolean searchPath(int NodesToExpand);       // Search but do not return Path

	abstract MapPath findPath(int NodesToExpand);     // Search and return the best Path

	abstract T provide();
}
