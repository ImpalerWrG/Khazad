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

import Map.Coordinates.MapCoordinate;
import java.io.Serializable;

/**
 * A collection of common and fast distance Heuristics, a Heuristic is a quick
 * guess as to the distance between two MapCoordinates.
 *
 * @author Impaler
 */
public interface Heuristic {

	public float estimate(MapCoordinate StartCoord, MapCoordinate GoalCoord);

	public class Manhatten implements Heuristic, Serializable {

		private static final long serialVersionUID = 1;

		public float estimate(MapCoordinate StartCoord, MapCoordinate GoalCoord) {
			return (float) Math.abs(StartCoord.getX() - GoalCoord.getX()) + Math.abs(StartCoord.getY() - GoalCoord.getY()) + Math.abs(StartCoord.getZ() - GoalCoord.getZ());
		}
	}

	public class Chebyshev implements Heuristic, Serializable {

		private static final long serialVersionUID = 1;

		public float estimate(MapCoordinate StartCoord, MapCoordinate GoalCoord) {
			float max = 0;
			float cost;

			cost = Math.abs(StartCoord.getX() - GoalCoord.getX());
			if (cost > max)
				max = cost;

			cost = Math.abs(StartCoord.getY() - GoalCoord.getY());
			if (cost > max)
				max = cost;

			cost = Math.abs(StartCoord.getZ() - GoalCoord.getZ());
			if (cost > max)
				max = cost;

			return max;
		}
	}

	public class Euclidean implements Heuristic, Serializable {

		public float estimate(MapCoordinate StartCoord, MapCoordinate GoalCoord) {
			float cost = StartCoord.getX() - GoalCoord.getX();
			float sum = cost * cost;

			cost = StartCoord.getY() - GoalCoord.getY();
			sum += cost * cost;

			// Use Z as well?  cuberoot?

			return (float) Math.sqrt(sum);
		}
	}

	public class Diagonal implements Heuristic, Serializable {
		final float SquareRootTwo = (float) Math.sqrt(2);

		public float estimate(MapCoordinate StartCoord, MapCoordinate GoalCoord) {
			float DiagonalsX = Math.abs(StartCoord.getX() - GoalCoord.getX());
			float DiagonalsY = Math.abs(StartCoord.getY() - GoalCoord.getY());
			float ZDifference = Math.abs(StartCoord.getZ() - GoalCoord.getZ());


			if (DiagonalsX < DiagonalsY) {
				return (SquareRootTwo * DiagonalsX) + (DiagonalsY - DiagonalsX) + (ZDifference * 2);
			} else {
				return (SquareRootTwo * DiagonalsY) + (DiagonalsX - DiagonalsY) + (ZDifference * 2);
			}
		}
	}

	public class Octile implements Heuristic, Serializable {
		final float DiagonalFactor = ((float) Math.sqrt(2)) -1;

		public float estimate(MapCoordinate StartCoord, MapCoordinate GoalCoord) {
			float DiagonalsX = Math.abs(StartCoord.getX() - GoalCoord.getX());
			float DiagonalsY = Math.abs(StartCoord.getY() - GoalCoord.getY());
			float ZDifference = Math.abs(StartCoord.getZ() - GoalCoord.getZ());

			float estimate = Math.max(DiagonalsX, DiagonalsY) + (DiagonalFactor * Math.min(DiagonalsX, DiagonalsY)) + (ZDifference * 2);
			return estimate;
		}
	}

	public class StraitLine implements Heuristic, Serializable {

		public float estimate(MapCoordinate StartCoord, MapCoordinate GoalCoord) {

			//float dx1 = current.x - goal.x
			//float dy1 = current.y - goal.y

			//float dx2 = start.x - goal.x
			//float dy2 = start.y - goal.y

			//float cross = abs(dx1*dy2 - dx2*dy1)
			//float heuristic += cross * 0.001

			return 0;
		}
	}

	public class Dijkstra implements Heuristic, Serializable {

		public float estimate(MapCoordinate StartCoord, MapCoordinate GoalCoord) {
			return 0;
		}
	}

}
