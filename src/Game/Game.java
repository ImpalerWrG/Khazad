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

import Core.Main;
import Map.*;
import Terrain.Geology;

import com.jme3.app.Application;
import com.jme3.app.SimpleApplication;
import com.jme3.app.state.AbstractAppState;
import com.jme3.app.state.AppStateManager;

import com.jme3.input.InputManager;
import com.jme3.input.KeyInput;
import com.jme3.input.controls.ActionListener;
import com.jme3.input.controls.KeyTrigger;

import java.util.concurrent.ConcurrentHashMap;
import java.util.ArrayList;
import java.util.PriorityQueue;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Future;

/**
 * Game holds all the objects (Map, Settlment, Weather etc) which together make
 * up the simulation and is were saving and loading of game files is initiate
 * before being piped to the particular sub-objects.  Game is also responsible
 * for the simulation logic loop which is called by update each frame, a Ticker
 * instance is created and submitted to the thread pool to keep the core JME
 * thread fully devoted to rendering.
 * 
 * @author Impaler
 */
public class Game extends AbstractAppState implements ActionListener {

	SimpleApplication app = null;
	AppStateManager state = null;

    GameMap MainMap;
    Geology MapGeology;
	Settlment GameSettlment;
	Weather GameWeather;

	boolean Pause;
	int TickRate;
	long CurrentGameTick;
	float TickRounding;
	
	public long seconds;
	public long minutes;
	public long hours;
	public long days;

	int UniqueIDCounter;
	PriorityQueue<Temporal> TemporalQueue;

	ArrayList<Actor> Actors;
	int ActorIDcounter = 0;

	ExecutorService Executor;
	Future lastUpdate;

	public Game() {
		GameSettlment = new Settlment();
		Actors = new ArrayList<Actor>();
		
		TickRate = 1;
		TickRounding = 0;
		CurrentGameTick = 0;
		UniqueIDCounter = 0;
		Pause = true;

		TemporalQueue = new PriorityQueue<Temporal>();
	}

	@Override
    public void initialize(AppStateManager stateManager, Application app) {
		super.initialize(stateManager, app);
        this.app = (SimpleApplication) app;
		this.state = stateManager;
		Main core = (Main) app;
		Executor = core.getThreadPool();

		registerWithInput(app.getInputManager());
	}

	public boolean InitializeGame(short X, short Y, String SeedString) {

		MapGeology = new Geology();
		MapGeology.Initialize(SeedString);
		MapGeology.GenerateWorldHeightMap(X, Y);

		MainMap = GameMap.getMap();

		GameWeather = new Weather();
		AddTemporal(GameWeather);
		
		//MainMap.Initialize();

		//Path = new PathManager();
		//Path.Initialize();

		BuildMapChunk((short) 0, (short) 0, (byte) X, (byte) Y);

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
		ConcurrentHashMap Cells = MainMap.getCellMap();
		ProgressSize = Cells.size();
		
		ConcurrentHashMap<CellCoordinate, Cell> CellMap = MainMap.getCellMap();
		for (Cell TargetCell : CellMap.values())
		{
			TargetCell.BuildFaceData();
			ProgressCount++;
		}

		//Path.CreateMapAbstraction(MainMap);
		//Path.InitializeTestingSuite();

		//RENDERER.FocusActiveCameraAt(Ogre::Vector3(X * CELLEDGESIZE / 2, Y * CELLEDGESIZE / 2, 0));
		//MainMap.RegisterWithRendering();
		//GUI.DirtyActiveScreen();

		return true;
	}

	public Pawn SpawnPawn(MapCoordinate SpawnCoordinates) {
		Pawn NewPawn = new Pawn(ActorIDcounter, SpawnCoordinates);
		ActorIDcounter++;
		Actors.add(NewPawn);
		AddTemporal(NewPawn);
		return NewPawn;
	}

	public Citizen SpawnCitizen(MapCoordinate SpawnCoordinates) {
		Citizen NewCitizen = new Citizen(ActorIDcounter, SpawnCoordinates);
		ActorIDcounter++;
		Actors.add(NewCitizen);
		GameSettlment.addCitizen(NewCitizen);
		AddTemporal(NewCitizen);
		return NewCitizen;
	}

	boolean AddTemporal(Temporal NewTemporal) {
		TemporalQueue.add(NewTemporal);	
		return true;
	}

	public GameMap getMap() {
		return MainMap;
	}
	
	public Settlment getSettlment() {
		return GameSettlment;
	}

	public Weather getWeather() {
		return GameWeather;
	}
	
	public ArrayList<Actor> getActors() {
		return Actors;
	}
	
	public void onAction(String name, boolean keyPressed, float tpf) {
        if (this.isEnabled()) {
			if (name.equals("Pause")) {
				if (keyPressed)
					Pause = !Pause;
			}
			if (name.equals("Faster")) {
				if (keyPressed) 
					TickRate *= 4;
			}
			if (name.equals("Slower")) {
				if (keyPressed)
					TickRate /= 4;
			}
		}
	}

	public void registerWithInput(InputManager inputManager) {
        String[] inputs = {"Pause", "Faster", "Slower"};

		inputManager.addMapping("Pause", new KeyTrigger(KeyInput.KEY_SPACE)); 
		inputManager.addMapping("Faster", new KeyTrigger(KeyInput.KEY_ADD)); 
		inputManager.addMapping("Faster", new KeyTrigger(KeyInput.KEY_EQUALS)); 
		inputManager.addMapping("Slower", new KeyTrigger(KeyInput.KEY_MINUS)); 
		inputManager.addMapping("Slower", new KeyTrigger(KeyInput.KEY_SUBTRACT)); 
        inputManager.addListener(this, inputs);
	}

	@Override
    public void update(float tpf) {
		if (!Pause) {
			if(lastUpdate == null || lastUpdate.isDone()) {
				float TargetTicks = TickRate * tpf * 12;
				TargetTicks += TickRounding;
				int FullTicks = (int) TargetTicks;
				TickRounding = TargetTicks - FullTicks;

				Ticker simulation = new Ticker(this);
				simulation.windup(FullTicks);
				lastUpdate = Executor.submit(simulation);

				seconds = CurrentGameTick / 12;
				minutes = seconds / 60;
				hours = minutes / 60;
				days = hours / 24;		
			}
		}
    }

	public long getCurrentTimeTick() {
		return CurrentGameTick;
	}

	public void setTickRate(int NewRate) {
		TickRate = NewRate;
	}

	public int getTickRate() {
		return TickRate;
	}

	public void Pause(boolean NewPause) {
		Pause = NewPause;
	}

	public boolean isPaused() {
		return Pause;
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
