/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package PathFinding;

import Map.MapCoordinate;
/**
 *
 * @author Impaler
 */
public abstract class PathAlgorithm<T> {

	int GraphReads;
    int ExpandedNodes;

    GridInterface SearchGraph;

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

    int getGraphReads() { 
		return GraphReads; 
	}

    int getExpandedNodes() {
		return ExpandedNodes; 
	}

    abstract boolean SearchPath(int NodesToExpand);       // Search but do not return Path
    abstract MapPath FindPath(int NodesToExpand);     // Search and return the best Path

	abstract T provide();
}
