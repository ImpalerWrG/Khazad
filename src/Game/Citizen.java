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

import Core.Utils;
import Data.DataManager;
import Data.Types.GivenNameGroupData;
import Job.*;
import Map.Coordinates.MapCoordinate;
import java.io.Serializable;

/**
 * The class representing the actually Dwarves and their livestock which are
 * responsive to the players Job commands.
 *
 * @author Impaler
 */
public class Citizen extends Pawn implements Serializable {

	private static final long serialVersionUID = 1;

	// Family/clan relationships
	// Happyness/ moods/ hunger/ thirst/ wealth/ honor / other mutable status
	// personality/ tastes/ other fixed status
	// Proffessions / levels/ skills
	public Citizen(short CreatureTypeID, int id, int seed, MapCoordinate SpawnLocation) {
		super(CreatureTypeID, id, seed, SpawnLocation);
		// TODO randomly generated name
		generateName("GIVEN_NAMEGROUP_TOLKEINESQUE");
		//setFirstName("Urist");
		//setLastName(Integer.toString(id));
		setTask(new Task(null, Task.TaskType.TASK_IDLE, SpawnLocation));
	}

	@Override
	public Task findTask() {
		return super.findTask();
	}

	private void generateName(String givenNameGroup) {
		DataManager data = DataManager.getDataManager();
		short givenNameGroupId;
		if (getGender() == Gender.GENDER_MALE) {
			givenNameGroupId = Data.DataManager.getDataManager().getLabelIndex(givenNameGroup + "_GENDER_MALE");
		} else {
			givenNameGroupId = Data.DataManager.getDataManager().getLabelIndex(givenNameGroup + "_GENDER_FEMALE");
		}
		GivenNameGroupData givenNameGroupData = data.getGivenNameGroupData(givenNameGroupId);
		String prefix = givenNameGroupData.getPrefixes()[AttributeDice.roll(0, givenNameGroupData.getPrefixes().length - 1)];
		String suffix = givenNameGroupData.getSuffixes()[AttributeDice.roll(0, givenNameGroupData.getSuffixes().length - 1)];
		setFirstName(Utils.upperCaseFirst(prefix + suffix)); // upper case the first letter in case the prefix is an empty string.
		// TODO last names
		setLastName("");
	}
}
