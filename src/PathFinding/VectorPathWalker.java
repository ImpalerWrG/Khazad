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
import Map.Direction;
import java.io.Serializable;

/**
 * Basic PathWalker for a VectorPath, it utilizes a simple double nested loop
 * to read out each 'leg' of the VectorPath.
 *
 * @author Impaler
 */
public class VectorPathWalker implements PathWalker, Serializable {

	private static final long serialVersionUID = 1;
	private MapCoordinate StepCoordinates;
	private int MagnitudeCountDown;
	private short LegCounter;
	private int CurrentStep;
	VectorPath TargetPath;

	VectorPathWalker(VectorPath SourcePath) {
		if (SourcePath != null) {
			TargetPath = SourcePath;
			reset();
		}
	}

	public void reset() {
		if (TargetPath != null) {
			if (TargetPath.Magnitudes.size() > 0) {
				MagnitudeCountDown = TargetPath.Magnitudes.get(0);  // Prime the counter with the first legs magnitude
			} else {
				MagnitudeCountDown = 0;
			}
			StepCoordinates = TargetPath.StartCoordinates;
		}
		LegCounter = 0;
		CurrentStep = 0;
	}

	public MapCoordinate nextCoordinate() {
		if (CurrentStep < TargetPath.StepCount) {
			if (MagnitudeCountDown == 0) {
				LegCounter++;
				MagnitudeCountDown = TargetPath.Magnitudes.get(LegCounter);
			}

			CurrentStep++;
			MagnitudeCountDown--;

			StepCoordinates.translate(TargetPath.Directions.get(LegCounter));

			return StepCoordinates;
		}
		return TargetPath.GoalCoordinates; // Keep returning the Goal if we've reached the end of the path
	}

	public Direction nextDirection() {
		if (CurrentStep < TargetPath.StepCount) {
			if (MagnitudeCountDown == 0) {
				LegCounter++;
				MagnitudeCountDown = TargetPath.Magnitudes.get(LegCounter);
			}

			CurrentStep++;
			MagnitudeCountDown--;

			return TargetPath.Directions.get(LegCounter);
		}
		return Direction.DIRECTION_DESTINATION;
	}

	public MapCoordinate peekCoordinate() {
		if (CurrentStep < TargetPath.StepCount) {
			Direction dir;
			if (MagnitudeCountDown == 0) {
				dir = TargetPath.Directions.get(LegCounter + 1);
			} else {
				dir = TargetPath.Directions.get(LegCounter);
			}

			StepCoordinates.translate(dir);
			return StepCoordinates;
		}
		return TargetPath.GoalCoordinates; // Keep returning the Goal if we've reached the end of the path
	}

	public Direction peekDirection() {
		if (CurrentStep < TargetPath.StepCount) {
			if (MagnitudeCountDown == 0) {
				return TargetPath.Directions.get(LegCounter + 1);
			} else {
				return TargetPath.Directions.get(LegCounter);
			}
		}
		return Direction.DIRECTION_DESTINATION;
	}

	public int getCurrentStep() {
		return CurrentStep;
	}
}
