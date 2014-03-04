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
public class VectorPathWalker implements PathWalker  {

    private MapCoordinate StepCoordinates;
    private int MagnitudeCountDown;
    private short LegCounter;
	private int CurrentStep;

    VectorPath TargetPath;
	

	VectorPathWalker(VectorPath SourcePath) {
		if (SourcePath != null) {
			TargetPath = SourcePath;
			Reset();
		}
	}

	public void Reset() {
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

	public MapCoordinate NextCoordinate() {
		if(CurrentStep < TargetPath.StepCount) {
			if (MagnitudeCountDown == 0) {
				LegCounter++;
				MagnitudeCountDown = TargetPath.Magnitudes.get(LegCounter);
			}

			CurrentStep++;
			MagnitudeCountDown--;

			StepCoordinates.TranslateMapCoordinates(TargetPath.Directions.get(LegCounter));

			return StepCoordinates;
		}
		return TargetPath.GoalCoordinates; // Keep returning the Goal if we've reached the end of the path
	}

	public Direction NextDirection() {
		if(CurrentStep < TargetPath.StepCount)
		{
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
	
	public MapCoordinate PeekCoordinate() {
		if(CurrentStep < TargetPath.StepCount) {
			Direction dir;
			if (MagnitudeCountDown == 0) {
				dir = TargetPath.Directions.get(LegCounter + 1);
			} else {
				dir = TargetPath.Directions.get(LegCounter);
			}

			StepCoordinates.TranslateMapCoordinates(dir);
			return StepCoordinates;
		}
		return TargetPath.GoalCoordinates; // Keep returning the Goal if we've reached the end of the path
	}
	
    public Direction PeekDirection() {
		if(CurrentStep < TargetPath.StepCount) {
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
