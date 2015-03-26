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

import Map.Direction;
import Map.MapCoordinate;
import java.io.Serializable;

/**
 * The walker class for a FullPath, like all walkers it allows multiple Navigators
 * to simultaniously use the same path without interfering with each other.  As 
 * Full Paths are just MapCoordinate lists and Navigators querry for Directions
 * the overhead of converting the Coordinate into a Direction means this Walker
 * will be slower then other PathWalkers implementations.
 * 
 * @author Impaler
 */
public class CoordinatePathWalker implements PathWalker, Serializable {
	
	CoordinatePath TargetPath = null;
	int CurrentStep = 0;
	
	CoordinatePathWalker(CoordinatePath SourcePath) {
		TargetPath = SourcePath;
		Reset();
	}

	public void Reset() {
		CurrentStep = 0;
	}

	public MapCoordinate NextCoordinate() {
		if(CurrentStep < TargetPath.StepCount)
		{
			CurrentStep++;
			return TargetPath.PathCourse.get(CurrentStep);
		}
		return TargetPath.GoalCoordinates; // Keep returning the Goal if we've reached the end of the path
	}
	
	public Direction NextDirection() {
		return Direction.DIRECTION_NONE;
	}

	public MapCoordinate PeekCoordinate() {
		if(CurrentStep < TargetPath.StepCount)
		{
			//CurrentStep++;
			return TargetPath.PathCourse.get(CurrentStep);
		}
		return TargetPath.GoalCoordinates; // Keep returning the Goal if we've reached the end of the path
	}
	
	public Direction PeekDirection() {
		return Direction.DIRECTION_NONE;
	}

	public int getCurrentStep() {
		return CurrentStep;
	}
}
