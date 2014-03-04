/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package PathFinding;

import java.util.BitSet;

import Map.Axis;
import Map.MapCoordinate;
import Map.Direction;

/**
 *
 * @author Impaler
 */
public interface Grid<T> {

    float getEdgeCost(MapCoordinate TestCoords, Direction DirectionType);

    int max(Axis AxialComponent);
    int min(Axis AxialComponent);

    BitSet getDirectionFlags(MapCoordinate TargetCoords);
    boolean isPathPossible(MapCoordinate StartCoords, MapCoordinate GoalCoords);

    boolean contains(MapCoordinate TestCoords);
}
