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

import Map.MapCoordinate;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.Serializable;

/**
 * Base class for any object that has a location on the map and is potentially
 * visible to the player.
 * 
 * Implementations:  Pawn
 * 
 * @author Impaler
 */
public abstract class Actor extends Temporal implements Serializable {

	// The location for gameplay logic purposes
	MapCoordinate LocationCoordinates;
	
	boolean Visible;
	boolean Hidden;
	transient boolean Dirty;

	public Actor(int id, MapCoordinate SpawnLocation) {
		this.ID = id;

		LocationCoordinates = SpawnLocation;
		Dirty = true;
	}

	public void setLocation(MapCoordinate NewPosition) {
		LocationCoordinates = NewPosition;
		Dirty = true;
	}
	
	private void readObject(ObjectInputStream ois) throws ClassNotFoundException, IOException {
		// default deserialization
		ois.defaultReadObject();
		// fix transients
		Dirty = true;
	}
		/*
		CellCoordinates OldCoordinates = CellCoordinates(LocationCoordinates);
		LocationCoordinates = NewPosition;

		CellCoordinates NewCellCoords = CellCoordinates(NewPosition);
		Cell NewCell = GAME->getMap()->getCell(NewCellCoords);
		CubeCoordinates Cube = NewPosition.Cube();

		float X = (float) (Cube >> Cube.CELLBITSHIFT) + (float)Cube.HALFCUBE - (float)(Cube.CELLEDGESIZE / 2);
		float Y = (float) (Cube & Cube.CELLBITFLAG) + (float)Cube.HALFCUBE - (float)(Cube.CELLEDGESIZE / 2);

		if (OldCoordinates != NewCell.getCellCoordinates())
		{
			Cell OldCell = GAME.getMap().getCell(OldCoordinates);
			if (OldCell != null)
			{
				OldCell.removeActor(this);
			}

			if (NewCell != null)
			{
				NewCell.addActor(this);
			}
		}
		ActorNode.setLocalTranslation(X, Y, 0);
	}


	void setRenderPosition(Vector3f NewPosition) {
		ActorNode.setPosition(NewPosition);
	}

	void MoveRenderPosition(Vector3f Translation) {
		ActorNode.move(Translation);
	}
	 */
	public boolean isDirty() {
		return Dirty;
	}

	public MapCoordinate getLocation() {
		return LocationCoordinates;
	}

	void setVisible(boolean NewValue) {
		if (Visible != NewValue) {
			Visible = NewValue; 
			//ActorNode. (NewValue);
			Dirty = true;
		}
	}
	
	//long Wake(long CurrentTick) {
	//	return CurrentTick + 1;
	//}
}

