/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package PathFinding;

import java.util.ArrayList;
import java.util.BitSet;

import java.util.concurrent.Future;
import java.util.concurrent.Executor;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledThreadPoolExecutor;
import java.util.concurrent.ConcurrentLinkedQueue;

import Core.Clock;
import Core.Main;

import Map.GameMap;
import Map.MapCoordinate;
import Map.Direction;

import com.jme3.app.Application;
import com.jme3.app.state.AppStateManager;

import com.jme3.app.state.AbstractAppState;

/**
 *
 * @author Impaler
 */
public class PathManager extends AbstractAppState {
	
	static PathManager Singlton = null;
	
	Clock PathingTimer;

    AStar AstarImplementation;
    PathAlgorithm HeriarchialAstarImplementation;

    ArrayList<Navigator> Navigators;

    KhazadGrid MapGrid;

    Heuristic ManhattenHeuristic;
    Heuristic EuclideanHeuristic;
    Heuristic MaxDimensionHeuristic;
    Heuristic DijkstraHeuristic;
    Heuristic DiagonalHeuristic;

    //CentralPool<AStarNode> NodeCentralPool;
	ArrayList<Pool> PoolList;
	
	ScheduledThreadPoolExecutor Executor;
	
	
    public PathTester Tester;

	private PathManager() {
		ManhattenHeuristic = new Heuristic.Manhatten();
		EuclideanHeuristic = new Heuristic.Euclidean();
		MaxDimensionHeuristic = new Heuristic.MaxDimension();
		DijkstraHeuristic = new Heuristic.Dijkstra();
		DiagonalHeuristic = new Heuristic.Diagonal();

		PoolList = new ArrayList<Pool>();
	}

	@Override
    public void initialize(AppStateManager stateManager, Application app) {
		super.initialize(stateManager, app);
		Main core = (Main) app;
		Executor = core.getThreadPool();
	}

	public static PathManager getSinglton() {
		if (Singlton == null)
			Singlton = new PathManager();
		
		return Singlton;
	}

	Pool ProvidePool() {  // Simple but effective
        for (Pool TargetPool: PoolList) {
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

	public void CreateMapAbstraction(GameMap TargetMap) {
		MapGrid = new KhazadGrid(TargetMap);

		AstarImplementation = new AStar(MapGrid);
		AstarImplementation.AssignPoll(ProvidePool());
		AstarImplementation.setHeuristics(DiagonalHeuristic, EuclideanHeuristic);
		
		Tester = new PathTester();
		Tester.Initialize(this);		
	}

	public void EditMapAbstraction() {

	}

	void DeleteMapAbstraction() {
		MapGrid = null;
		AstarImplementation = null;
	}

	public MapPath FindPath(int PathSystem, MapCoordinate StartCoords, MapCoordinate GoalCoords) {
		if (AstarImplementation != null) {
			if (MapGrid.contains(StartCoords) && MapGrid.contains(GoalCoords)) {
				if (isPathPossible(PathSystem, StartCoords, GoalCoords)) {
					AstarImplementation.AssignPoll(ProvidePool());
					AstarImplementation.setEndPoints(StartCoords, GoalCoords);
					
					return AstarImplementation.FindPath(0);
				}
			}
		}
		return null;
	}

	public Future FindFuturePath(int PathSystem, MapCoordinate StartCoords, MapCoordinate GoalCoords) {
		if (MapGrid.contains(StartCoords) && MapGrid.contains(GoalCoords)) {	
			if (isPathPossible(PathSystem, StartCoords, GoalCoords)) {	
				AStar PathTask = new AStar(MapGrid);
				PathTask.AssignPoll(ProvidePool());
				PathTask.setHeuristics(ManhattenHeuristic, EuclideanHeuristic);
				PathTask.setEndPoints(StartCoords, GoalCoords);

				return Executor.submit(PathTask);
			}
		}
		
		return null;
	}

	public MapPath ProfilePath(int PathSystem, MapCoordinate StartCoords, MapCoordinate GoalCoords, PathTester.Profile TargetProfile) {
		if (AstarImplementation != null) {
			if (MapGrid.contains(StartCoords) && MapGrid.contains(GoalCoords)) {
				if (isPathPossible(PathSystem, StartCoords, GoalCoords)) {
					PathingTimer.Start();
						AstarImplementation.setEndPoints(StartCoords, GoalCoords);
						MapPath FoundPath = AstarImplementation.FindPath(0);
					TargetProfile.PathTimeCost = PathingTimer.Stop();

					TargetProfile.ProfiledPath = FoundPath;

					if (FoundPath == null) {
						TargetProfile.ResultCode = ProfileResultCode.PATH_CODE_FAILURE_UNKNOWN;
					} else {
						TargetProfile.ResultCode = ProfileResultCode.PATH_CODE_SUCCESS;
					}

					TargetProfile.PathGraphReads = AstarImplementation.getGraphReads();
					TargetProfile.PathExpandedNodes = AstarImplementation.getExpandedNodes();

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

	public boolean isPathPossible(int PathSystem, MapCoordinate StartCoords, MapCoordinate GoalCoords) {
		if (MapGrid != null) {
			return MapGrid.isPathPossible(StartCoords, GoalCoords);
		}
		return false;
	}

	public float EstimatePathLength(int PathSystem, MapCoordinate StartCoords, MapCoordinate GoalCoords) {
		return -1;
	}

	public BitSet getDirectionFlags(MapCoordinate Coordinates) {
		if (MapGrid != null) {
			return MapGrid.getDirectionFlags(Coordinates);
		}
		return null;
	}

	boolean contains(MapCoordinate Coordinates) {
		if (MapGrid != null)
		{
			return MapGrid.contains(Coordinates);
		}
		return false;
	}

	public float getEdgeCost(MapCoordinate TestCoords, Direction DirectionType) {
		if (MapGrid != null)
		{
			return MapGrid.getEdgeCost(TestCoords, DirectionType);
		}
		return -1;
	}

	int getZone(MapCoordinate TargetCoords){ // Superflous? 
		if (MapGrid != null)
		{
			return MapGrid.getConnectivityZone(TargetCoords);
		}
		return 0;
	}

	int getZoneEquivilency(MapCoordinate TargetCoords) {
		if (MapGrid != null) {
			return MapGrid.getZoneEquivilency(TargetCoords);
		}
		return 0;
	}
}

