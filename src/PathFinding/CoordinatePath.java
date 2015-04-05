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
import java.util.ArrayList;

/**
 * A simple path representation in the form of a list of MapCoordinates, the
 * simplest and least compressed form of path representation. Intended only
 * for temporary usage, any path intended for caching should be compressed.
 *
 * @author Impaler
 */
public class CoordinatePath extends MapPath implements Serializable {

	private static final long serialVersionUID = 1;
	ArrayList<MapCoordinate> PathCourse;

	CoordinatePath(float PathLength, ArrayList<MapCoordinate> Course) {
		Length = PathLength;
		PathCourse = Course;
		StepCount = Course.size() - 1;

		StartCoordinates = PathCourse.get(0);
		GoalCoordinates = PathCourse.get(StepCount);
	}

	public PathWalker getPathWalker() {
		return new CoordinatePathWalker(this);
	}
}