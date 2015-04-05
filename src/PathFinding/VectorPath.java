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
import java.util.ArrayList;

/**
 * A more efficiently compressed Path implementation, parrellel Lists or
 * Directions and Magnitudes divide the path into segments, which is closer to
 * the kind of Path typically generated.
 *
 * @author Impaler
 */
public class VectorPath extends MapPath implements Serializable {

	private static final long serialVersionUID = 1;
	ArrayList<Direction> Directions;
	ArrayList<Integer> Magnitudes;

	VectorPath(float PathLength, ArrayList<Direction> RawDirections, MapCoordinate StartCoords, MapCoordinate GoalCoords) {
		Directions = new ArrayList<Direction>();
		Magnitudes = new ArrayList<Integer>();

		StartCoordinates = StartCoords;
		GoalCoordinates = GoalCoords;
		StepCount = RawDirections.size();

		// Compress the raw directions into legs
		if (RawDirections.size() > 0) {
			int MagnitudeCounter = 1;
			Direction CurrentDirection = RawDirections.get(0);

			for (int i = 1; i < RawDirections.size(); i++) {
				if (MagnitudeCounter == 255 || RawDirections.get(i) != CurrentDirection) {
					Directions.add(CurrentDirection);
					Magnitudes.add(MagnitudeCounter);

					CurrentDirection = RawDirections.get(i);
					MagnitudeCounter = 1;
				} else {
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
