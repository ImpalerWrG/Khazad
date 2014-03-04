/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package PathFinding;

import Map.MapCoordinate;
import java.util.ArrayList;

/**
 *
 * @author Impaler
 */
public class FullPath extends MapPath {
	
    ArrayList<MapCoordinate> PathCourse;

	FullPath(float PathLength, ArrayList<MapCoordinate> Course) {
		Length = PathLength;
		PathCourse = Course;
		StepCount = Course.size() - 1;

		StartCoordinates = PathCourse.get(0);
		GoalCoordinates = PathCourse.get(StepCount);
	}

	public PathWalker getPathWalker() {
		return new FullPathWalker(this);
	}
}