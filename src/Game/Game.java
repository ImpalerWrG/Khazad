/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package Game;

import Map.*;
import Terrain.Geology;
import Core.Clock;
import com.jme3.app.state.AbstractAppState;
import java.util.*;
/**
 *
 * @author Impaler
 */
public class Game extends AbstractAppState {

	//private static Game instance = null;

    boolean Zoneing;   // User is Designating a Zone

    //Clock GameTimer;

    GameMap MainMap;
    Geology MapGeology;
    //PathManager Path;
    //Settlment TheSettlment;

	ArrayList<Actor> Actors;
	int ActorIDcounter = 0;

    float ProgressAmount;

	public Game() {
		//GameTimer = new Clock(50);
		
		Actors = new ArrayList<Actor>();
	}

	public boolean InitializeGame(short X, short Y, String SeedString) {
		//TempManager = new TemporalManager();

		MapGeology = new Geology();
		MapGeology.Initialize(SeedString);
		MapGeology.GenerateWorldHeightMap(X, Y);

		MainMap = GameMap.getMap();
		//MainMap.Initialize();

		//Path = new PathManager();
		//Path.Initialize();

		//boost::thread_group Builder;
		//Builder.create_thread( boost::bind(&BuildMapChunk, this, 0, 0, X, Y));
		BuildMapChunk((short) 0, (short) 0, (byte) X, (byte) Y);

		Zoneing = false;
		//GameTimer.Start();

		return true;
	}

	boolean BuildMapChunk(short X, short Y, byte Width, byte Height) {
		short SizeX = (short) (X + Width);
		short SizeY = (short) (Y + Height);

		float ProgressSize = SizeX * SizeY;
		float ProgressCount = 0;

		// Create and add Cells with shape and material data
		for (int x = X; x < SizeX; x++)
		{
			for (int y = Y; y < SizeY; y++)
			{
				MapGeology.GenerateCellHeight(x, y, (float) 10.0, (float) 2.0);

				for (int z = MapGeology.getCellBottomZLevel() - 1; z <= MapGeology.getCellTopZLevel() + 1; z++)
				{
					CellCoordinate TargetCellCoordinates = new CellCoordinate(x, y, z);
					Cell NewCell = new Cell();

					NewCell.setCellPosition(TargetCellCoordinates);
					MainMap.insertCell(NewCell);
					MapGeology.LoadCellData(NewCell);
				}
			}
		}

		// Initialize Faces for the cells
		HashMap Cells = MainMap.getCellMap();
		ProgressSize = Cells.size();
		
		HashMap<CellCoordinate, Cell> CellMap = MainMap.getCellMap();
		for (Cell TargetCell : CellMap.values())
		{
			TargetCell.BuildFaceData();
			ProgressCount++;
			ProgressAmount = ProgressCount / ProgressSize;
			//GUI.DirtyActiveScreen();
		}

		//Path.CreateMapAbstraction(MainMap);
		//Path.InitializeTestingSuite();

		//RENDERER.FocusActiveCameraAt(Ogre::Vector3(X * CELLEDGESIZE / 2, Y * CELLEDGESIZE / 2, 0));
		//MainMap.RegisterWithRendering();
		//GUI.DirtyActiveScreen();

		return true;
	}

	public Actor SpawnActor(MapCoordinate SpawnCoordinates) {
		Actor NewActor = new Actor(ActorIDcounter, SpawnCoordinates);
		ActorIDcounter++;
		Actors.add(NewActor);
		return NewActor;
	}

	public Pawn SpawnPawn(MapCoordinate SpawnCoordinates) {
		Pawn NewPawn = new Pawn(ActorIDcounter, SpawnCoordinates);
		ActorIDcounter++;
		Actors.add(NewPawn);
		return NewPawn;
	}

	public GameMap getMap() {
		return MainMap;
	}
	
	public ArrayList<Actor> getActors() {
		return Actors;
	}
	/*
	void Save(boost::filesystem::basic_ofstream<char>& Stream) const
	{
		Stream.write((char*)&TickRate, sizeof(TickRate));
		Stream.write((char*)&Pause, sizeof(Pause));
		Stream.write((char*)&Zoneing, sizeof(Zoneing));

		MainMap.Save(Stream);
	}

	void Load(boost::filesystem::basic_ifstream<char>& Stream)
	{
		Stream.read((char*)&TickRate, sizeof(TickRate));
		Stream.read((char*)&Pause, sizeof(Pause));
		Stream.read((char*)&Zoneing, sizeof(Zoneing));

		MainMap = new Map();
		MainMap.Load(Stream);
	}
	* */

}
