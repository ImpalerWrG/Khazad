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
public abstract class MapPath {
	   
	float Length;   // The travel cost of the path
    int StepCount;  // The number of individual steps in the path

    int SizeLimit;      // Largest size of pawn that can use this path
    int MovementFlags;  // Booleans flags for terrain passable flags

    public abstract PathWalker getPathWalker();

    MapCoordinate StartCoordinates, GoalCoordinates;
}

