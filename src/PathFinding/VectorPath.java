/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package PathFinding;

import Map.MapCoordinate;
import Map.Direction;
import java.util.ArrayList;

/**
 *
 * @author Impaler
 */
public class VectorPath extends MapPath {
	
	ArrayList<Direction> Directions;
    ArrayList<Integer> Magnitudes;

	VectorPath(float PathLength, ArrayList<Direction> RawDirections, MapCoordinate StartCoords, MapCoordinate GoalCoords)
	{
		Directions = new ArrayList<Direction>();
		Magnitudes = new ArrayList<Integer>();

		StartCoordinates = StartCoords;
		GoalCoordinates = GoalCoords;
		StepCount = RawDirections.size();

		// Compress the raw directions into legs
		if (RawDirections.size() > 0)
		{
			int MagnitudeCounter = 1;
			Direction CurrentDirection = RawDirections.get(0);

			for (int i = 1; i < RawDirections.size(); i++)
			{
				if (MagnitudeCounter == 255 || RawDirections.get(i) != CurrentDirection)
				{
					Directions.add(CurrentDirection);
					Magnitudes.add(MagnitudeCounter);

					CurrentDirection = RawDirections.get(i);
					MagnitudeCounter = 1;
				}
				else
				{
					MagnitudeCounter++;
				}
			}
			Directions.add(CurrentDirection);
			Magnitudes.add(MagnitudeCounter);
		}
	}

	public PathWalker getPathWalker() {
		return new VectorPathWalker(this);
	}
}
