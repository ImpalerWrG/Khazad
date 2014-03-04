/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package PathFinding;

import Map.MapCoordinate;
import Map.Direction;

/**
 *
 * @author Impaler
 */
public class AStarNode implements Comparable {

    float TotalCost;
    float TieBreakerValue;

	MapCoordinate LocationCoordinates;

    float PathLengthFromStart;
    float MinimumCostToGoal;

    AStarNode Parent;
    Direction ParentDirection;
	
    AStarNode() {
	
	}

    void Set(MapCoordinate TargetCoordinates, AStarNode ParentNode, Direction SourceDirection, float DistanceFromStart, float MinimumCost, float TieBreaker) {
        Parent = ParentNode;
        ParentDirection = SourceDirection;
        LocationCoordinates = TargetCoordinates;
        PathLengthFromStart = DistanceFromStart;
        MinimumCostToGoal = MinimumCost;
        TieBreakerValue = TieBreaker;
        TotalCost = PathLengthFromStart + MinimumCostToGoal;
    }

	public int compareTo(Object TargetObject) {
		//if (TargetObject instanceof AStarNode) {
			AStarNode TargetNode = (AStarNode) TargetObject;
			if (TotalCost < TargetNode.TotalCost) {
				return -1;
			} else if (TotalCost == TargetNode.TotalCost) {
				if (TieBreakerValue < TargetNode.TieBreakerValue) {
					return -1;
				} else {
					return 1;
				}
			} else {
				return 1;
			}
		//}
		//return 0;
	}

    boolean equals(AStarNode other) {
        return LocationCoordinates.equals(other.LocationCoordinates);
    }

    MapCoordinate getCoordinates() {
        return LocationCoordinates;
    }
}
