/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package Game;

import java.util.concurrent.Callable;
/**
 *
 * @author Impaler
 */
public class Ticker implements Callable<Object> {

	Game thegame;
	int Tickdown;
	
	public Ticker(Game parent) {
		thegame = parent;
	}
	
	public void windup(int Ticks) {
		Tickdown = Ticks;
	}

	public Object call() {
		while (Tickdown > 0) {
			UpdateTick();
			Tickdown--;
		}
		return null;
	}
	
	public void UpdateTick() {
		thegame.CurrentGameTick++;   // Advance Tick count
		
		Temporal target;
		target = thegame.TemporalQueue.poll();
		if (target != null) {
			do {	
				long RewakeTick = target.Wake(thegame.CurrentGameTick);
				if (RewakeTick != -1) {
					thegame.TemporalQueue.add(target);
				}
				target = thegame.TemporalQueue.poll();
			} while (target.WakeTick <= thegame.CurrentGameTick);	
		
			thegame.TemporalQueue.add(target);
		}
	}
}
