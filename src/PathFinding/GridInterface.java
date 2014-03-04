/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package PathFinding;

import java.util.BitSet;
import Map.MapCoordinate;
import Map.Direction;
import Map.Axis;

/**
 *
 * @author Impaler
 */
public interface GridInterface {
	
    float getEdgeCost(MapCoordinate TestCoords, Direction DirectionType);

    //int max(Axis AxialComponent);
    //int min(Axis AxialComponent);

    BitSet getDirectionFlags(MapCoordinate TargetCoords);
    boolean isPathPossible(MapCoordinate StartCoords, MapCoordinate GoalCoords);

    boolean contains(MapCoordinate TestCoords);
}
