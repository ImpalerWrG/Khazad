/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package Game;

/**
 * Base class for all objects that respond to time
 * @author Impaler
 */
public abstract class Temporal implements Comparable {
	
	int ID;
	long WakeTick = 1;

	Temporal() {
		
	}

	void ResetWakeTick(long NewWakeTick) {
		WakeTick = NewWakeTick;
		// notify Temporal Manager?
	}

	void Retire() {
		//GAME->getTemporal()->RetireTemporal(this);
	}

	public int getID() {
		return ID;
	}

	public int compareTo(Object TargetObject) {
		Temporal Target = (Temporal) TargetObject;
		if (WakeTick < Target.WakeTick) {
			return -1;
		} else if (WakeTick == Target.WakeTick) {
			return 0;
		} else {
			return 1;
		}
	}

	abstract long Wake(long CurrentTick);
}
