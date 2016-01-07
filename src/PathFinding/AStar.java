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

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashSet;
import java.util.BitSet;
import java.util.PriorityQueue;

import Map.Coordinates.Direction;
import Map.Coordinates.MapCoordinate;
import java.io.Serializable;

import java.util.concurrent.Callable;

/**
 * A simple uni-direction A* implementation, it utilizes it's own Node type
 * AStarNode which is optimized for this search methodology. The data structures
 * are highly optimized, PriorityQueue for Fringe and HashSet for visited Nodes
 *
 * Pathing can be done for a limited number of nodes, or with a zero argument
 * searching will continue until their is either a complete path or the
 * Fringe Queue is exhausted which indicates that no path is possible. As all
 * attempts to path should have been proceeded by a connectivity check an
 * exhaustion is probably indicative of a flaw in connectivity checking.
 *
 * @author Impaler
 */
public class AStar extends PathAlgorithm implements Callable, Serializable {

	private static final long serialVersionUID = 1;
	// Core storage sturctures of AStar
	PriorityQueue<AStarNode> FringeHeap;
	LinkedListDeque<AStarNode> FringeDeque;
	HashSet<MapCoordinate> VisitedCoordinates;
	MapCoordinate NeiboringCoordinates;

	// Values used in iteration loop
	AStarNode CurrentNode;
	boolean FringeExausted;
	// Memory optimizing pool for Node supply
	Pool<AStarNode> NodePool;

	AStar(GridInterface TargetSearchGraph) {
		FringeHeap = new PriorityQueue<AStarNode>(100);
		VisitedCoordinates = new HashSet<MapCoordinate>(100);
		NeiboringCoordinates = new MapCoordinate();

		SearchGraph = TargetSearchGraph;
		FinalPath = null;
	}

	public void assignResources(Pool TargetPool, LinkedListDeque<AStarNode> List) {
		NodePool = TargetPool;
		FringeDeque = List;
		NodePool.setFactory(this);
	}

	@Override
	void setEndPoints(MapCoordinate StartCoords, MapCoordinate GoalCoords) {
		StartCoordinates = StartCoords;
		GoalCoordinates = GoalCoords;
		GraphReads = ExpandedNodes = 0;
		FinalPath = null;
		FringeExausted = false;

		FringeHeap.clear();
		VisitedCoordinates.clear();

		AStarNode StartNode = NodePool.provide();
		StartNode.set(StartCoordinates, null, Direction.DIRECTION_NONE, 0, MainHeuristic.estimate(StartCoords, GoalCoords), TieBreakerHeuristic.estimate(StartCoords, GoalCoords));

		FringeDeque.insertFront(StartNode);
	}

	boolean searchPath(int NodesToExpand) {
		if (FringeExausted) {
			return false; // No more searching can be done
		}

		if (FinalPath == null) {
			boolean GoalFound;
			if (NodesToExpand > 0) { // Search for a limited time
				for (int RemainingNodes = NodesToExpand; RemainingNodes > 0; RemainingNodes--) {
					if (FringeDeque.isEmpty() && FringeHeap.isEmpty()) {
						FringeExausted = true; // Path could not be found
						return false;
					}

					if (expandNode()) {
						return true; // Path found, skip to finish
					}
				}
				return false; // Path not yet found
			} else { // Search untill Path is found or Fringe is exhausted
				while (!FringeDeque.isEmpty()) {
					if (expandNode()) {
						return true;
					}
				}
				FringeExausted = true;
				return false;
			}
		}
		return true;  // Final Path already found don't do any more searching
	}

	MapPath findPath(int NodesToExpand) {
		if (FringeExausted) {
			return null; // Fringe Exhastion, don't return a useless path
		}

		boolean FinalPathFound = searchPath(NodesToExpand);

		if (FinalPath == null) {
			MapPath CurrentPath = generateVectorPath();

			if (FinalPathFound)
				FinalPath = CurrentPath;

			NodePool.release();	 // Nodes can be released now that a final path has been found
			return CurrentPath;
		}
		NodePool.release();
		FringeDeque.clear();
		return FinalPath;
	}

	boolean expandNode() {
		CurrentNode = FringeDeque.removeFirst();
		if (CurrentNode == null)
			CurrentNode = FringeHeap.poll();

		MapCoordinate TestCoordinates = CurrentNode.LocationCoordinates;
		Direction ParentDirection = CurrentNode.ParentDirection.invert();

		if (VisitedCoordinates.contains(TestCoordinates))
			return false;

		if (TestCoordinates.equals(GoalCoordinates))
			return true;

		// mark as VisitedCoordinates if not already Visited
		VisitedCoordinates.add(TestCoordinates);

		BitSet TestDirections = SearchGraph.getDirectionEdgeSet(TestCoordinates);

		// Check all Neibors
		for (int i = TestDirections.nextSetBit(0); i >= 0; i = TestDirections.nextSetBit(i + 1)) {
			Direction DirectionType = Direction.ANGULAR_DIRECTIONS[i];
			if (DirectionType == Direction.DIRECTION_NONE || DirectionType == ParentDirection)
				continue;

			NeiboringCoordinates.copy(TestCoordinates);
			NeiboringCoordinates.translate(DirectionType);

			// If Coordinate is not already on the VisitedCoordinates list
			if (VisitedCoordinates.contains(NeiboringCoordinates) == false) {
				float EdgeCost = SearchGraph.getEdgeCost(TestCoordinates, DirectionType);
				GraphReads++;

				AStarNode NewNode = NodePool.provide();
				NewNode.set(NeiboringCoordinates, CurrentNode, DirectionType, CurrentNode.PathLengthFromStart + EdgeCost, MainHeuristic.estimate(NeiboringCoordinates, GoalCoordinates), TieBreakerHeuristic.estimate(NeiboringCoordinates, GoalCoordinates));
				FringeDeque.insertFront(NewNode);
			} // modify existing node with new distance?
		}

		while(FringeDeque.size() > 1) {
			FringeHeap.add(FringeDeque.removeLast());
		}

		return false; // Goal was not found
	}

	CoordinatePath generateFullPath() {
		ExpandedNodes = VisitedCoordinates.size();

		float PathLength = CurrentNode.PathLengthFromStart;
		ArrayList<MapCoordinate> Course = new ArrayList();

		while (CurrentNode != null) {
			Course.add(CurrentNode.LocationCoordinates);
			CurrentNode = CurrentNode.Parent;
		}
		Course.add(StartCoordinates);

		Collections.reverse(Course);
		return new CoordinatePath(PathLength, Course);
	}

	VectorPath generateVectorPath() {
		ExpandedNodes = VisitedCoordinates.size();

		float PathLength = CurrentNode.PathLengthFromStart;
		ArrayList<Direction> Course = new ArrayList();

		while (CurrentNode != null) {
			Course.add(CurrentNode.ParentDirection);
			CurrentNode = CurrentNode.Parent;
		}
		Course.remove(Course.size() - 1);

		Collections.reverse(Course);
		return new VectorPath(PathLength, Course, StartCoordinates, GoalCoordinates);
	}

	AStarNode provide() {
		return new AStarNode();
	}

	public MapPath call() {
		return findPath(0);
	}
}
