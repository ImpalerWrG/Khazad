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
public interface Heuristic {
	
    public float Estimate(MapCoordinate StartCoord, MapCoordinate GoalCoord);
	
	public class Manhatten implements Heuristic {
		public float Estimate(MapCoordinate StartCoord, MapCoordinate GoalCoord) {
			return (float) Math.abs(StartCoord.X - GoalCoord.X) + Math.abs(StartCoord.Y - GoalCoord.Y) +  Math.abs(StartCoord.Z - GoalCoord.Z);
		}
	}
	
	public class MaxDimension implements Heuristic {
		public float Estimate(MapCoordinate StartCoord, MapCoordinate GoalCoord) {
			float max = 0;
			float cost = 0;

			cost = Math.abs(StartCoord.X - GoalCoord.X);
			if (cost > max)
				max = cost;

			cost = Math.abs(StartCoord.Y - GoalCoord.Y);
			if (cost > max)
				max = cost;

			cost = Math.abs(StartCoord.Z - GoalCoord.Z);
			if (cost > max)
				max = cost;

			return max;
		}
	}

	public class Euclidean implements Heuristic {
		public float Estimate(MapCoordinate StartCoord, MapCoordinate GoalCoord) {
			float cost = StartCoord.X - GoalCoord.X;
			float sum = cost * cost;

			cost = StartCoord.Y - GoalCoord.Y;
			sum += cost * cost;

			// Use Z as well?  cuberoot?

			return (float) Math.sqrt(sum);
		}
	}

	public class Diagonal implements Heuristic {
		public float Estimate(MapCoordinate StartCoord, MapCoordinate GoalCoord)
		{
			float DiagonalsX = Math.abs(StartCoord.X - GoalCoord.X);
			float DiagonalsY = Math.abs(StartCoord.Y - GoalCoord.Y);
			float ZDifference = Math.abs(StartCoord.Z - GoalCoord.Z);

			final float SquareRootTwo = (float) Math.sqrt(2);
			
			if(DiagonalsX < DiagonalsY) {
				return (SquareRootTwo * DiagonalsX) + (DiagonalsY - DiagonalsX) + (ZDifference * 2);
			} else {
				return (SquareRootTwo * DiagonalsY) + (DiagonalsX - DiagonalsY) + (ZDifference * 2);
			}
		}
	}

	public class StraitLine implements Heuristic {
		public float Estimate(MapCoordinate StartCoord, MapCoordinate GoalCoord) {
			
			//float dx1 = current.x - goal.x
			//float dy1 = current.y - goal.y

			//float dx2 = start.x - goal.x
			//float dy2 = start.y - goal.y

			//float cross = abs(dx1*dy2 - dx2*dy1)
			//float heuristic += cross * 0.001

			return 0;
		}
	}

	public class Dijkstra implements Heuristic {
		public float Estimate(MapCoordinate StartCoord, MapCoordinate GoalCoord) {
			return 0;
		}
	}
}


