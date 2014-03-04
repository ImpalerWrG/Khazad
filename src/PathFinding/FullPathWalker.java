/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package PathFinding;

import Map.Direction;
import Map.MapCoordinate;

/**
 *
 * @author Impaler
 */
public class FullPathWalker implements PathWalker {
	
	FullPath TargetPath = null;
	int CurrentStep = 0;
	
	FullPathWalker(FullPath SourcePath)
	{
		TargetPath = SourcePath;
		Reset();
	}

	public void Reset()
	{
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
