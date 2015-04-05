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
 * Abstract Base for the Path representations, all Paths implementations have a
 * corresponding PathWalker class that they will return and which will read
 * from only that type of Path.
 *
 * Implementations: CoordinatePath, VectorPath
 *
 * @author Impaler
 */
public abstract class MapPath implements Serializable {

	private static final long serialVersionUID = 1;
	float Length;   // The travel cost of the path
	int StepCount;  // The number of individual steps in the path
	MovementModality MovementType;
	MapCoordinate StartCoordinates, GoalCoordinates;

	public abstract PathWalker getPathWalker();
}
