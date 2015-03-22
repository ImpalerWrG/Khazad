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

/**
 * Base class for all objects that respond to time, all implementations need to 
 * return the future time tick that they wish to recive a future Wake call on as
 * the return value of Wake.  This can be done by simply adding the desired sleep
 * time to the current tick argument of Wake.
 * 
 * Implementations: Actor, Weather
 * 
 * @author Impaler
 */
public abstract class Temporal implements Comparable {

	public static final long TICKS_PER_SECOND = 12;
	public static final long TICKS_PER_MINUTE = 720;
	public static final long TICKS_PER_HOUR = 43200;
	public static final long TICKS_PER_DAY = 1036800;
	public static final long TICKS_PER_WEEK = 7257600;
	public static final long TICKS_PER_MONTH = 29030400;
	public static final long TICKS_PER_YEAR = 348364800;

	int ID;
	long WakeTick = 1;

	public Temporal() {
		
	}

	public void ResetWakeTick(long NewWakeTick) {
		WakeTick = NewWakeTick;
		// notify Temporal Manager?
	}

	public void Retire() {
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
