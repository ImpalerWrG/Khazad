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

package Game;

import Job.JobManager;
import java.io.Serializable;
import java.util.ArrayList;

/**
 * Object representing the players settlement, holds all Citizens, history and
 * work tasks (in the form of the JobManager) being ordered by the player.
 *
 * @author Impaler
 */
public class Settlement implements Serializable {

	private static final long serialVersionUID = 1;
	// Settlment data, wealth, honor, diplomatic status etc
	public ArrayList<Citizen> Citizens;
	JobManager JobSystem;

	public Settlement() {
		Citizens = new ArrayList<Citizen>();
		JobSystem = new JobManager();
	}

	public void addCitizen(Citizen NewCitizen) {
		Citizens.add(NewCitizen);
		JobSystem.addCitizen(NewCitizen);
	}

	public JobManager getJobManager() {
		return JobSystem;
	}
}
