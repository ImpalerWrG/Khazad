/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package PathFinding;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.concurrent.ConcurrentHashMap;

import Core.Dice;

import Map.MapCoordinate;
import Map.CellCoordinate;

/**
 *
 * @author Impaler
 */

enum ProfileResultCode
{
    PATH_CODE_NO_DATA,
    PATH_CODE_SUCCESS,
    PATH_CODE_FAILUTE_UNITIALIZED,
    PATH_CODE_FAILURE_INVALID_LOCATION,
    PATH_CODE_FAILURE_NO_CONNECTION,
    PATH_CODE_FAILURE_UNKNOWN;
}

public class PathTester {
	
	protected class Profile { // stores data from profiling
		
		ProfileResultCode ResultCode = ProfileResultCode.PATH_CODE_NO_DATA;

		long PathTimeCost = 0;
		int PathGraphReads = 0;
		int PathExpandedNodes = 0;
		boolean CacheHit = false;

		MapPath ProfiledPath = null;
	}
	
	protected class GroupProfile {
		
		ArrayList<Profile> Profiles;

		int TotalPathSteps = 0;
		float TotalPathLength = 0;
		int TotalGraphReads = 0;
		int TotalNodesExpanded = 0;
		int TotalCacheHits = 0;
		int TotalTimeCost = 0;

		int[] Results = new int[ProfileResultCode.values().length];

		float GraphReadEfficency;
		float NodeSearchEfficency;
		float CacheEfficency;
		float TimeCostEfficency;

		void Analyze() {
			TotalGraphReads = TotalNodesExpanded = TotalCacheHits = TotalPathSteps = TotalTimeCost = 0;
			TotalPathLength = 0f;
					 
			for (int i = 0; i < ProfileResultCode.values().length; i++) {
				Results[i] = 0;
			}

			for (int i = 0; i < Profiles.size(); i++) {
				Results[Profiles.get(i).ResultCode.ordinal()] += 1;

				if (Profiles.get(i).ProfiledPath != null && Profiles.get(i).ResultCode == ProfileResultCode.PATH_CODE_SUCCESS)
				{
					TotalPathSteps += Profiles.get(i).ProfiledPath.StepCount;
					TotalPathLength += Profiles.get(i).ProfiledPath.Length;
				}

				TotalTimeCost += Profiles.get(i).PathTimeCost;
				TotalGraphReads += Profiles.get(i).PathGraphReads;
				TotalNodesExpanded += Profiles.get(i).PathExpandedNodes;
				if (Profiles.get(i).CacheHit) {
					TotalCacheHits++;
				}
			}

			GraphReadEfficency = TotalGraphReads / (float) TotalPathSteps;
			NodeSearchEfficency = TotalNodesExpanded / (float) TotalPathSteps;
			CacheEfficency = TotalCacheHits / (float) Profiles.size();
			TimeCostEfficency = TotalTimeCost / (float) TotalPathSteps;
		}
	}
	
	
	PathManager ParentManager;
    Dice PathDice;

    MapCoordinate ManualStartCoords, ManualGoalCoords;  // Used for manual testing
    MapPath ManualPath;

    GroupProfile ManualProfileGroup;
    GroupProfile CurrentProfileGroup;
    GroupProfile[] ProfileGroupList;

    ArrayList<MapCoordinate> TestCoords, StartCoordsList, GoalCoordsList;
    int TestingIterations;

	
	boolean Initialize(PathManager Parent) {
		//ProfileGroupList[0] = null;
		//ProfileGroupList[1] = null;

		TestCoords = new ArrayList<MapCoordinate>(); 
		StartCoordsList = new ArrayList<MapCoordinate>();  
		GoalCoordsList = new ArrayList<MapCoordinate>(); 
	
		CurrentProfileGroup = null;
		ManualProfileGroup = null;
		ManualPath = null;

		ParentManager = Parent;
		PathDice = new Dice();

		CollectTestCoords();
		return true;
	}

	void CollectTestCoords() {
		ConcurrentHashMap<CellCoordinate, KhazadGrid.GridCell> cells = ParentManager.MapGrid.GridCells;
		
		for (KhazadGrid.GridCell TargetCell : cells.values()) {
			if (TargetCell != null)
			{
				CellCoordinate CellCoords = TargetCell.getCellCoordinates();

				byte TargetCube = 0;
				do
				{
					MapCoordinate TestCoordinates = new MapCoordinate(CellCoords, TargetCube);

					if (ParentManager.contains(TestCoordinates) && ParentManager.getDirectionFlags(TestCoordinates).cardinality() != 0) {
						TestCoords.add(TestCoordinates);
					}
					TargetCube++;
				}
				while (TargetCube != 0);  // End Loop when Byte rolls over
			}
		}
	}

	void CreateTestSuite(int Seed, int Iterations) {
		TestingIterations = Iterations;
		PathDice.Seed(Seed);

		// Prepare a set of Start Goal pairs for the test
		for (int i = 0; i < Iterations; i++)
		{
			StartCoordsList.add(TestCoords.get(PathDice.Roll(0, TestCoords.size() - 1)));
			GoalCoordsList.add(TestCoords.get(PathDice.Roll(0, TestCoords.size() - 1)));
			// Identical Start and Goal points are possible, the PathManager should handle them
		}
	}

	public MapCoordinate getRandomPassableCoordinate() {
		return TestCoords.get(PathDice.Roll(0, TestCoords.size() - 1));
	}

	void ProfileManualPath() {
		if (ManualProfileGroup == null) {
			ManualProfileGroup = new GroupProfile();
		} else {
			//delete ManualProfileGroup;
			ManualProfileGroup = new GroupProfile();
		}

		Profile NewProfile = new Profile();
		ParentManager.ProfilePath(0, ManualStartCoords, ManualGoalCoords, NewProfile);

		ManualProfileGroup.Profiles.add(NewProfile);
		ManualPath = NewProfile.ProfiledPath;

		ManualProfileGroup.Analyze();
		CurrentProfileGroup = ManualProfileGroup;
	}

	void RunPathTestSuites(int[] TestSystems) {
		ProfileGroupList = new GroupProfile[TestSystems.length];
		for (int i = 0; i < TestSystems.length; i++) {
			ProfileGroupList[TestSystems[i]] = new GroupProfile();
			TestSuite(TestingIterations, TestSystems[i], ProfileGroupList[TestSystems[i]], StartCoordsList, GoalCoordsList);
			CurrentProfileGroup = ProfileGroupList[TestSystems[i]];
		}
	}

	void TestSuite(int Iterations, int SystemIndex, GroupProfile SystemProfileGroup, ArrayList<MapCoordinate> StartCoordsList, ArrayList<MapCoordinate> GoalCoordsList) {
		for (int i = 0; i < Iterations; ++i) {
			Profile NewProfile = new Profile();
			ParentManager.ProfilePath(SystemIndex, StartCoordsList.get(i), GoalCoordsList.get(i), NewProfile);
			SystemProfileGroup.Profiles.add(NewProfile);
		}

		SystemProfileGroup.Analyze();
	}

	void RunHuristicTestSuite(int Interations, ArrayList<Integer> PathSystems) {

	}

}
