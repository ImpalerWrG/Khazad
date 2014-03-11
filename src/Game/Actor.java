/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package Game;

import Map.MapCoordinate;
import com.jme3.scene.Node;

/**
 *
 * @author Impaler
 */
public class Actor extends Temporal {

	// The location for gameplay logic purposes
	MapCoordinate LocationCoordinates;

	Node ActorNode;
	//Ogre::BillboardSet* ActorBillboard;

	// TODO Use bitvector in the future
	
	boolean Visible;
	boolean Hidden;
	boolean Dirty;

	public Actor(int id, MapCoordinate SpawnLocation) {
		this.ID = id;
		//ActorBillboard = RENDERER->getSceneManager()->createBillboardSet(1);

		//ActorBillboard->setDefaultDimensions(Width, Height);
		//ActorBillboard->createBillboard(0, 0, ((Height - 1.0) / 2));

		//ActorBillboard->setMaterialName(Mat->getName());
		ActorNode = new Node();

		LocationCoordinates = SpawnLocation;
		Dirty = true;
	}

	public void setLocation(MapCoordinate NewPosition) {
		LocationCoordinates = NewPosition;
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
	
	long Wake(long CurrentTick) {
		return CurrentTick + 100;
	}
}

