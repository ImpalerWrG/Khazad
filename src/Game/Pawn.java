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
import Core.Dice;
import Data.*;
import Data.Types.*;

import Map.Direction;
import Map.MapCoordinate;

import PathFinding.MovementModality;
import PathFinding.Navigator;
import PathFinding.PathManager;

import java.io.Serializable;

/**
 * A moving creature, derived from Actor but with the addition of a Navigator object
 * linking it to the Pathfinding engine and a set of RPG style attributes.
 *
 * @author Impaler
 */
public class Pawn extends Actor implements Serializable {

	private static final long serialVersionUID = 1;
	short CreatureTypeID;
	// Action extension of 
	long ActionDuration = 1;
	long ActionStarted;
	// Movement and Pathfinding
	Navigator PathNavigator;
	boolean Moving;
	Direction CurrentMovementDirection;
	// Job/Task Managment
	private Task CurrentTask;
	public Job PrimaryJob;
	public Job BreakJob;
	public boolean onBreak;
	// The 14 Basic Attributes
	Dice AttributeDice;
	byte[] BasicAttributes;
	private String firstName;
	private String lastName;
	private Gender gender;

	public Pawn(short CreatureTypeID, int id, int Seed, MapCoordinate SpawnLocation) {
		super(id, SpawnLocation);

		this.CreatureTypeID = CreatureTypeID;
		Moving = false;
		PathNavigator = new Navigator(SpawnLocation, new MovementModality(MovementModality.MovementType.MOVEMENT_TYPE_WALK, 1, 1));
		CurrentMovementDirection = Direction.DIRECTION_NONE;

		AttributeDice = new Dice();
		AttributeDice.seed(id ^ Seed);

		DataManager Data = DataManager.getDataManager();
		CreatureData CreatureDataEntry = Data.getCreatureData(CreatureTypeID);
		CreatureSizeData CreatureSizeDataEntry = Data.getCreatureSizeData(CreatureDataEntry.SizeID);

		BasicAttributes = new byte[Data.getNumBaseAttributes()];
		for (int i = 0; i < BasicAttributes.length; i++) {
			BasicAttributes[i] = (byte) (AttributeDice.roll(1, 4) + AttributeDice.roll(1, 4));
			BasicAttributes[i] += CreatureDataEntry.AttributeModifierVales[i];  //Size class adjustment
			BasicAttributes[i] += CreatureSizeDataEntry.AttributeModifierVales[i];  //Size class adjustment
		}
		if (AttributeDice.roll(1,3) == 1) {
			gender = Gender.GENDER_FEMALE;
		} else {
			gender = Gender.GENDER_MALE;
		}
	}

	public Navigator getNavigator() {
		return PathNavigator;
	}

	public long attemptMove(Direction MovementDirection) {
		float EdgeCost = PathManager.getSingleton().getEdgeCost(LocationCoordinates, MovementDirection, PathNavigator.getMovementModality());
		final int speedIndex = DataManager.getDataManager().getLabelIndex("BASIC_ATTRIBUTE_SPEED");
		if (EdgeCost != -1) {
			return (int) (EdgeCost / ((float) BasicAttributes[speedIndex] / 7.0) * TICKS_PER_SECOND);
		} else {
			return 1;  // signal falure to job manager?
		}
	}

	public long updatePosition() {
		setLocation(new MapCoordinate(LocationCoordinates, CurrentMovementDirection));
		CurrentMovementDirection = PathNavigator.getNextStep();

		if (CurrentMovementDirection == Direction.DIRECTION_DESTINATION) {
			CurrentTask.Completed = true;
			return 1;
		} else {
			return attemptMove(CurrentMovementDirection);
		}
	}

	public boolean isDestinationReachable(MapCoordinate Destination) {
		return PathNavigator.isDestinationReachable(Destination);
	}

	@Override
	public final void setLocation(MapCoordinate NewLocation) {
		super.setLocation(NewLocation);
		PathNavigator.setLocation(NewLocation);
	}

	public Task getTask() {
		return CurrentTask;
	}

	public void setTask(Task NewTask) {
		if (CurrentTask != NewTask) {
			CurrentTask = NewTask;
		}
	}

	public float getActionFraction(long CurrentTick) {
		return (CurrentTick - ActionStarted) / ((float) ActionDuration);
	}

	public void setMovementDiretion(Direction newDirection) {
		CurrentMovementDirection = newDirection;
	}

	public Direction getMovementDirection() {
		return CurrentMovementDirection;
	}

	public MovementModality getMovementModality() {
		return PathNavigator.getMovementModality();
	}

	public byte getBasicAttribute(short AttributeID) {
		return BasicAttributes[AttributeID];
	}

	public Task findTask() {
		PrimaryJob = new WanderJob();
		CurrentTask = PrimaryJob.nextTask(this);
		return CurrentTask;
	}

	@Override
	long wake(long CurrentTick) {
		//super.Wake(CurrentTick);
		if (CurrentTask != null) {
			if (CurrentTask.Completed) {
				CurrentTask.finalizeTask(CurrentTick, this);
				// check for other needs, leave current Job if nessary
				setTask(CurrentTask.ParentJob.nextTask(this));
			}

			if (!CurrentTask.Begun) {
				ActionDuration = CurrentTask.beginTask(this);
			} else {
				ActionDuration = CurrentTask.continueTask(this);
				if (CurrentTask.Completed) {
					CurrentTask.finalizeTask(CurrentTick, this);
					setTask(CurrentTask.ParentJob.nextTask(this));
				}
			}
			ActionStarted = CurrentTick;
			WakeTick = CurrentTick + ActionDuration;
		} else {
			WakeTick = CurrentTick + 1;
		}

		return WakeTick;
	}

	public String getFirstName() {
		return firstName;
	}

	public void setFirstName(String firstName) {
		this.firstName = firstName;
	}

	public String getLastName() {
		return lastName;
	}

	public void setLastName(String lastName) {
		this.lastName = lastName;
	}
	
	public String getName() {
		return this.firstName + ' ' + this.lastName;
	}

	public Gender getGender() {
		return gender;
	}

	public void setGender(Gender gender) {
		this.gender = gender;
	}

	public String getGenderText() {
		if (gender == Gender.GENDER_MALE) {
			return "Male";
		} else if (gender == Gender.GENDER_FEMALE) {
			return "Female";
		}
		return "Unknown";
	}
}
