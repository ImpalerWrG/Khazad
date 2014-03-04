/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package PathFinding;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashSet;
import java.util.BitSet;
import java.util.PriorityQueue;

import Map.MapCoordinate;
import Map.Direction;

import java.util.concurrent.Callable;
/**
 *
 * @author Impaler
 */
public class AStar extends PathAlgorithm implements Callable {

    PriorityQueue<AStarNode> FringeNodes;
    HashSet<MapCoordinate> VisitedCoordinates;
    AStarNode CurrentNode;
    boolean FringeExausted;
    Pool<AStarNode> NodePool;
	

	AStar(GridInterface TargetSearchGraph) {
		FringeNodes = new PriorityQueue<AStarNode>();
		VisitedCoordinates = new HashSet<MapCoordinate>();

		SearchGraph = TargetSearchGraph;
		FinalPath = null;
	}

	public void AssignPoll(Pool TargetPool) {
		NodePool = TargetPool;
		NodePool.setFactory(this);	
	}

	@Override
	void setEndPoints(MapCoordinate StartCoords, MapCoordinate GoalCoords) {
		StartCoordinates = StartCoords;
		GoalCoordinates = GoalCoords;
		GraphReads = ExpandedNodes = 0;
		FinalPath = null;
		FringeExausted = false;

		FringeNodes.clear();
		VisitedCoordinates.clear();

		AStarNode StartNode = NodePool.ProvideObject();
		StartNode.Set(StartCoordinates, null, Direction.DIRECTION_NONE, 0, MainHeuristic.Estimate(StartCoords, GoalCoords), TieBreakerHeuristic.Estimate(StartCoords, GoalCoords));

		FringeNodes.add(StartNode);
	}

	boolean SearchPath(int NodesToExpand) {
		if (FringeExausted) {
			return false; // No more searching can be done
		}

		if (FinalPath == null) {
			boolean GoalFound;
			if (NodesToExpand > 0) { // Search for a limited time
				for(int RemainingNodes = NodesToExpand; RemainingNodes > 0; RemainingNodes--) {
					if (FringeNodes.size() == 0) {
						FringeExausted = true; // Path could not be found
						return false;
					}

					if (ExpandNode()) {
						return true; // Path found, skip to finish
					}
				}
				return false; // Path not yet found
			} else { // Search untill Path is found or Fringe is exhausted
				while (FringeNodes.size() != 0) {
					if (ExpandNode()) {
						return true;
					}
				}
				FringeExausted = true;
				return false;
			}
		}
		return true;  // Final Path already found don't do any more searching
	}

	MapPath FindPath(int NodesToExpand) {
		if (FringeExausted) {
			return null; // Fringe Exhastion, don't return a useless path
		}

		boolean FinalPathFound = SearchPath(NodesToExpand);

		if (FinalPath == null) {
			MapPath CurrentPath = GenerateVectorPath();

			if (FinalPathFound)
				FinalPath = CurrentPath;
			
			NodePool.Release();  // Nodes can be released now that a final path has been found
			return CurrentPath;
		}
		NodePool.Release();
		return FinalPath;
	}

	boolean ExpandNode() {
		CurrentNode = FringeNodes.poll();
		MapCoordinate TestCoordinates = CurrentNode.LocationCoordinates;
		
		if (VisitedCoordinates.contains(TestCoordinates))
			return false;

		if (TestCoordinates.equals(GoalCoordinates))
			return true; //GenerateBestPath();

		// mark as VisitedCoordinates if not already Visited
		VisitedCoordinates.add(TestCoordinates);

		MapCoordinate NeiboringCoordinates;
		BitSet TestDirections = SearchGraph.getDirectionFlags(TestCoordinates);

		// Check all Neibors
		for (int i = TestDirections.nextSetBit(0); i >= 0; i = TestDirections.nextSetBit(i + 1)) {
			Direction DirectionType = Direction.ANGULAR_DIRECTIONS[i];
			NeiboringCoordinates = new MapCoordinate(TestCoordinates, DirectionType);

			// If Coordinate is not already on the VisitedCoordinates list
			if (VisitedCoordinates.contains(NeiboringCoordinates) == false) {
				float EdgeCost = SearchGraph.getEdgeCost(TestCoordinates, DirectionType);
				GraphReads++;

				AStarNode NewNode = NodePool.ProvideObject();
				NewNode.Set(NeiboringCoordinates, CurrentNode, DirectionType, CurrentNode.PathLengthFromStart + EdgeCost, MainHeuristic.Estimate(NeiboringCoordinates, GoalCoordinates), TieBreakerHeuristic.Estimate(NeiboringCoordinates, GoalCoordinates));

				// Add the new Node to the Fringe
				FringeNodes.add(NewNode);
			}
		}

		return false; // Goal was not found
	}

	FullPath GenerateFullPath() {
		ExpandedNodes = VisitedCoordinates.size();

		float PathLength = CurrentNode.PathLengthFromStart;
		ArrayList<MapCoordinate> Course = new ArrayList();

		while (CurrentNode != null) {
			Course.add(CurrentNode.LocationCoordinates);
			CurrentNode = CurrentNode.Parent;
		}
		Course.add(StartCoordinates);

		Collections.reverse(Course);
		return new FullPath(PathLength, Course);
	}

	VectorPath GenerateVectorPath() {
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
		return FindPath(0);
	}
}
