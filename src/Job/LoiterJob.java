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

package Job;

import Game.Pawn;
import java.io.Serializable;

/**
 *
 * @author Impaler
 */
public class LoiterJob extends Job implements Serializable {

	private static final long serialVersionUID = 1;

	public LoiterJob() {
	}

	public boolean needsWorkers() {
		return false;
	}

	public Task nextTask(Pawn IdleCitizen) {
		return new Task(this, Task.TaskType.TASK_LOITER, null);
	}

	public float evaluatePawn(Pawn IdleCitizen) {
		return 0;
	}
}
