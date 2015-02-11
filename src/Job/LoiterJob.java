/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package Job;

import Game.Pawn;

/**
 *
 * @author Impaler
 */
public class LoiterJob extends Job {

	public LoiterJob() {
	}

	public boolean needsWorkers() {
		return false;
	}

	public Task nextTask(Pawn IdleCitizen) {
		return new Task(this, TaskType.TASK_LOITER, null);
	}

	public float EvaluatePawn(Pawn IdleCitizen) {
		return 0;
	}
}
