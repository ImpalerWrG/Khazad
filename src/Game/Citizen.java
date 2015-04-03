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

import Job.*;
import Map.MapCoordinate;
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
		setFirstName("Urist");
		setLastName(Integer.toString(id));
		setTask(new Task(null, TaskType.TASK_IDLE, SpawnLocation));
	}
	
	@Override
	public Task FindTask() {
		return super.FindTask();
	}
}
