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

import Core.Dice;

import Map.MapCoordinate;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.Serializable;

/**
 * A data agragating class used to statisticly sample the speed and efficency
 * of Pathfinding algorithms.
 *
 * @author Impaler
 */
enum ProfileResultCode {

	PATH_CODE_NO_DATA,
	PATH_CODE_SUCCESS,
	PATH_CODE_FAILUTE_UNITIALIZED,
	PATH_CODE_FAILURE_INVALID_LOCATION,
	PATH_CODE_FAILURE_NO_CONNECTION,
	PATH_CODE_FAILURE_UNKNOWN;
}

public class PathTester implements Serializable {

	private static final long serialVersionUID = 1;

	protected class Profile implements Serializable { // stores data from profiling

		private static final long serialVersionUID = 1;
		ProfileResultCode ResultCode = ProfileResultCode.PATH_CODE_NO_DATA;
		long PathTimeCost = 0;
		int PathGraphReads = 0;
		int PathExpandedNodes = 0;
		boolean CacheHit = false;
		MapPath ProfiledPath = null;
	}

	protected class GroupProfile implements Serializable {

		private static final long serialVersionUID = 1;
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

				if (Profiles.get(i).ProfiledPath != null && Profiles.get(i).ResultCode == ProfileResultCode.PATH_CODE_SUCCESS) {
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

	transient PathManager ParentManager;
	Dice PathDice;
	MapCoordinate ManualStartCoords, ManualGoalCoords;  // Used for manual testing
	MapPath ManualPath;
	GroupProfile ManualProfileGroup;
	GroupProfile CurrentProfileGroup;
	GroupProfile[] ProfileGroupList;
	ArrayList<MapCoordinate> TestCoords, StartCoordsList, GoalCoordsList;
	int TestingIterations;
	MovementModality Basic;

	boolean Initialize(PathManager Parent) {
		// TODO get Modality from the Manager
		Basic = new MovementModality(MovementModality.MovementType.MOVEMENT_TYPE_WALK, 1, 1);

		StartCoordsList = new ArrayList<MapCoordinate>();
		GoalCoordsList = new ArrayList<MapCoordinate>();

		CurrentProfileGroup = null;
		ManualProfileGroup = null;
		ManualPath = null;

		ParentManager = Parent;
		PathDice = new Dice();

		collectTestCoords();
		return true;
	}

	// this method is used by serialization
	private void readObject(ObjectInputStream ois) throws ClassNotFoundException, IOException {
		// default deserialization
		ois.defaultReadObject();
		// fix transients
		ParentManager = PathManager.getSingleton();
	}

	void collectTestCoords() {
		GridInterface BasicGrid = ParentManager.Grids.get(Basic);
		TestCoords = BasicGrid.getPassableCoordinates();
	}

	void createTestSuite(int Seed, int Iterations) {
		TestingIterations = Iterations;
		PathDice.seed(Seed);

		// Prepare a set of Start Goal pairs for the test
		for (int i = 0; i < Iterations; i++) {
			StartCoordsList.add(TestCoords.get(PathDice.roll(0, TestCoords.size() - 1)));
			GoalCoordsList.add(TestCoords.get(PathDice.roll(0, TestCoords.size() - 1)));
			// Identical Start and Goal points are possible, the PathManager should handle them
		}
	}

	public MapCoordinate getRandomPassableCoordinate() {
		return TestCoords.get(PathDice.roll(0, TestCoords.size() - 1));
	}

	void profileManualPath() {
		if (ManualProfileGroup == null) {
			ManualProfileGroup = new GroupProfile();
		} else {
			//delete ManualProfileGroup;
			ManualProfileGroup = new GroupProfile();
		}

		Profile NewProfile = new Profile();
		MovementModality MovementType = new MovementModality(MovementModality.MovementType.MOVEMENT_TYPE_WALK, 1, 1);
		ParentManager.profilePath(MovementType, ManualStartCoords, ManualGoalCoords, NewProfile);

		ManualProfileGroup.Profiles.add(NewProfile);
		ManualPath = NewProfile.ProfiledPath;

		ManualProfileGroup.Analyze();
		CurrentProfileGroup = ManualProfileGroup;
	}

	void runPathTestSuites(int[] TestSystems) {
		ProfileGroupList = new GroupProfile[TestSystems.length];
		for (int i = 0; i < TestSystems.length; i++) {
			ProfileGroupList[TestSystems[i]] = new GroupProfile();
			testSuite(TestingIterations, Basic, ProfileGroupList[TestSystems[i]], StartCoordsList, GoalCoordsList);
			CurrentProfileGroup = ProfileGroupList[TestSystems[i]];
		}
	}

	void testSuite(int Iterations, MovementModality MovementType, GroupProfile SystemProfileGroup, ArrayList<MapCoordinate> StartCoordsList, ArrayList<MapCoordinate> GoalCoordsList) {
		for (int i = 0; i < Iterations; ++i) {
			Profile NewProfile = new Profile();
			ParentManager.profilePath(MovementType, StartCoordsList.get(i), GoalCoordsList.get(i), NewProfile);
			SystemProfileGroup.Profiles.add(NewProfile);
		}
		SystemProfileGroup.Analyze();
	}

	void runHuristicTestSuite(int Interations, ArrayList<Integer> PathSystems) {
	}
}
