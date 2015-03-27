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

import java.io.Serializable;
import java.util.concurrent.Callable;

/**
 * Simple callable that pumps the game simulation for a specified number of 
 * simulated time ticks, used by game to put most simulation logic into a secondary
 * thread from the JME rendering thread.  A queue of temporal objects is used to
 * sort which objects are to be woken and updated on each tick, each object is 
 * then reinserted under it's next desired wakeup tick.
 * 
 * @author Impaler
 */
public class Ticker implements Callable<Object>, Serializable {
	private static final long serialVersionUID = 1;

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
		
		thegame.getSettlement().getJobManager().update();
		
		try {
		Temporal target;
		target = thegame.TemporalQueue.poll();
		if (target != null) {
			if (target.WakeTick <= thegame.CurrentGameTick) {
				do {	
					long RewakeTick = target.Wake(thegame.CurrentGameTick);
					if (RewakeTick != -1) {
						thegame.TemporalQueue.add(target);
					}
					target = thegame.TemporalQueue.poll();
				} while (target.WakeTick <= thegame.CurrentGameTick);	
			}
			thegame.TemporalQueue.add(target);
		}
		
		} catch (Exception e) {
			System.err.println(e.getLocalizedMessage());
			System.err.println(e.getMessage());
			System.err.println(e.toString());
		}
		// Other game logics here
	}
}
