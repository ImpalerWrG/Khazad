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

import java.util.ArrayList;
import java.util.BitSet;

import java.util.concurrent.Future;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.ConcurrentHashMap;

import Core.Clock;
import Core.Main;

import Map.GameMap;
import Map.Coordinates.MapCoordinate;
import Map.Coordinates.Direction;

import com.jme3.app.Application;
import com.jme3.app.state.AppStateManager;

import com.jme3.app.state.AbstractAppState;
import java.io.Serializable;

/**
 * Master Controller for all pathfinding operations. All pathfinding is done on
 * abstractions of the Map structure called Grids rather then the real map, these
 * abstractions are specialized for a specific Movement Modality. Map changes
 * have to be pushed here to keep the Grids accurate.
 *
 * Navagator objects which are embeded in moving entites which then
 * request paths from PathFinder. Each incoming Path request is matched to an
 * appropriate Grid instance and then an appropriate PathAlgorim instance is
 * created, given the Grid refrence and submitted to the threadPool.
 *
 * @author Impaler
 */
public class PathManager extends AbstractAppState {

	private static final long serialVersionUID = 1;
	static PathManager Singleton = null;
	Clock PathingTimer;
	//AStar AstarImplementation;
	PathAlgorithm HeriarchialAstarImplementation;
	//KhazadGrid MapGrid;
	ConcurrentHashMap<MovementModality, GridInterface> Grids;
	ArrayList<GridInterface> GridArray;

	Heuristic ManhattenHeuristic;
	Heuristic EuclideanHeuristic;
	Heuristic ChebyshevHeuristic;
	Heuristic DijkstraHeuristic;
	Heuristic DiagonalHeuristic;
	Heuristic OctileHeuristic;
	ArrayList<Pool> PoolList;
	ExecutorService Executor;
	public PathTester Tester;

	private PathManager() {
		ManhattenHeuristic = new Heuristic.Manhatten();
		EuclideanHeuristic = new Heuristic.Euclidean();
		ChebyshevHeuristic = new Heuristic.Chebyshev();
		DijkstraHeuristic = new Heuristic.Dijkstra();
		DiagonalHeuristic = new Heuristic.Diagonal();
		OctileHeuristic = new Heuristic.Octile();

		PoolList = new ArrayList<Pool>();
		Grids = new ConcurrentHashMap<MovementModality, GridInterface>();
		GridArray = new ArrayList<GridInterface>();
	}

	@Override
	public void initialize(AppStateManager stateManager, Application app) {
		super.initialize(stateManager, app);
		Main core = (Main) app;
		Executor = core.getThreadPool();
	}

	public static PathManager getSingleton() {
		if (Singleton == null)
			Singleton = new PathManager();

		return Singleton;
	}

	Pool ProvidePool() {  // Simple but effective
		for (Pool TargetPool : PoolList) {
			if (!TargetPool.isInUse()) {
				TargetPool.setInUse(true);
				return TargetPool;
			}
		}

		// No Pools available so Create a new one
		Pool NewPool = new Pool();
		NewPool.setInUse(true);
		PoolList.add(NewPool);
		return NewPool;
	}

	public void createMapAbstraction(GameMap TargetMap) {
		MovementModality BasicPawn = new MovementModality(MovementModality.MovementType.MOVEMENT_TYPE_WALK, 1, 1);
		KhazadGrid MainGrid = new KhazadGrid(TargetMap, BasicPawn);
		addGrid(MainGrid);

		Tester = new PathTester();
		Tester.Initialize(this);
	}

	public void addGrid(GridInterface NewGrid) {
		Grids.put(NewGrid.getModality(), NewGrid);
		GridArray.add(NewGrid);
	}

	public void editMapAbstractions(MapCoordinate[] Coordinates) {
		for (GridInterface Grid : Grids.values()) {
			Grid.dirtyMapCoordinate(Coordinates.clone());
		}
	}

	void deleteMapAbstractions() {
		Grids.clear();
		GridArray.clear();
	}

	public Future findFuturePath(MovementModality MovementType, MapCoordinate StartCoords, MapCoordinate GoalCoords) {
		GridInterface TargetGrid = getModalityGrid(MovementType);
		if (TargetGrid != null) {
			if (TargetGrid.contains(StartCoords) && TargetGrid.contains(GoalCoords)) {
				if (isPathPossible(MovementType, StartCoords, GoalCoords)) {
					AStar PathTask = new AStar(TargetGrid);
					PathTask.assignResources(ProvidePool(), new LinkedListDeque<AStarNode>(30));
					PathTask.setModality(MovementType);
					PathTask.setHeuristics(ManhattenHeuristic, EuclideanHeuristic);
					PathTask.setEndPoints(StartCoords, GoalCoords);

					return Executor.submit(PathTask);
				}
			}
		}

		return null;
	}

	public MapPath profilePath(MovementModality MovementType, MapCoordinate StartCoords, MapCoordinate GoalCoords, PathTester.Profile TargetProfile) {
		GridInterface TargetGrid = getModalityGrid(MovementType);
		if (TargetGrid != null) {
			if (TargetGrid.contains(StartCoords) && TargetGrid.contains(GoalCoords)) {
				if (isPathPossible(MovementType, StartCoords, GoalCoords)) {
					PathingTimer.start();
					AStar PathTask = new AStar(TargetGrid);
					PathTask.assignResources(ProvidePool(), new LinkedListDeque<AStarNode>(30));
					PathTask.setModality(MovementType);
					PathTask.setHeuristics(ManhattenHeuristic, EuclideanHeuristic);
					PathTask.setEndPoints(StartCoords, GoalCoords);
					MapPath FoundPath = PathTask.call();

					TargetProfile.PathTimeCost = PathingTimer.stop();

					TargetProfile.ProfiledPath = FoundPath;

					if (FoundPath == null) {
						TargetProfile.ResultCode = ProfileResultCode.PATH_CODE_FAILURE_UNKNOWN;
					} else {
						TargetProfile.ResultCode = ProfileResultCode.PATH_CODE_SUCCESS;
					}

					TargetProfile.PathGraphReads = PathTask.getGraphReads();
					TargetProfile.PathExpandedNodes = PathTask.getExpandedNodes();

					return FoundPath;
				} else {
					TargetProfile.ResultCode = ProfileResultCode.PATH_CODE_FAILURE_NO_CONNECTION;
					return null;
				}
			} else {
				TargetProfile.ResultCode = ProfileResultCode.PATH_CODE_FAILURE_INVALID_LOCATION;
				return null;
			}
		} else {
			TargetProfile.ResultCode = ProfileResultCode.PATH_CODE_FAILUTE_UNITIALIZED;
			return null;
		}
	}

	public boolean isPathPossible(MovementModality MovementType, MapCoordinate StartCoords, MapCoordinate GoalCoords) {
		GridInterface TargetGrid = getModalityGrid(MovementType);
		if (TargetGrid != null) {
			return TargetGrid.isPathPossible(MovementType, StartCoords, GoalCoords);
		}
		return false;
	}

	GridInterface getModalityGrid(MovementModality MovementType) {
		return Grids.get(MovementType);
	}

	GridInterface getModalityGrid(int GridIndex) {
		return GridArray.get(GridIndex);
	}

	int getModalityGridIndex(MovementModality MovementType) {
		for (int i = 0; i < GridArray.size(); i++) {
			GridInterface grid = GridArray.get(i);
			if (grid.getModality().equals(MovementType)) {
				return i;
			}
		}
		return -1;
	}

	public BitSet getDirectionFlags(MapCoordinate Coordinates, MovementModality Modality) {
		GridInterface TargetGrid = getModalityGrid(Modality);
		if (TargetGrid != null) {
			return TargetGrid.getDirectionEdgeSet(Coordinates);
		}
		return null;
	}

	boolean contains(MapCoordinate Coordinates, MovementModality Modality) {
		GridInterface TargetGrid = getModalityGrid(Modality);
		if (TargetGrid != null) {
			return TargetGrid.contains(Coordinates);
		}
		return false;
	}

	public float getEdgeCost(MapCoordinate TestCoords, Direction DirectionType, MovementModality Modality) {
		GridInterface TargetGrid = getModalityGrid(Modality);
		if (TargetGrid != null) {
			return TargetGrid.getEdgeCost(TestCoords, DirectionType);
		}
		return -1;
	}

	public float getEdgeCost(MapCoordinate TestCoords, Direction DirectionType, int ModalityIndex) {
		// TODO find more direct access to desired modality grid rather then hashmap
		GridInterface TargetGrid = getModalityGrid(ModalityIndex);
		if (TargetGrid != null) {
			return TargetGrid.getEdgeCost(TestCoords, DirectionType);
		}
		return -1;
	}

	public int getConnectivityZone(MapCoordinate TestCoords, MovementModality Modality) {
		GridInterface TargetGrid = getModalityGrid(Modality);
		if (TargetGrid != null) {
			return TargetGrid.getConnectivityZone(TestCoords);
		}
		return 0;
	}
}
