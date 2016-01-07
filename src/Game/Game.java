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


import Core.*;
import Job.ExcavateJob;
import Job.JobManager;
import Map.*;
import Map.Coordinates.*;
import Terrain.Geology;
import Interface.VolumeSelection;
import Nifty.GameScreenController;

import com.jme3.app.Application;
import com.jme3.app.state.AbstractAppState;
import com.jme3.app.state.AppStateManager;

import com.jme3.input.InputManager;
import com.jme3.input.controls.ActionListener;
import java.io.IOException;
import java.io.ObjectInputStream;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.PriorityQueue;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Future;

import java.lang.StringBuffer;

/**
 * Game holds all the objects (Map, Settlment, Weather etc) which together make
 * up the simulation and is were saving and loading of game files is initiate
 * before being piped to the particular sub-objects. Game is also responsible
 * for the simulation logic loop which is called by update each frame, a Ticker
 * instance is created and submitted to the thread pool to keep the core JME
 * thread fully devoted to rendering.
 *
 * @author Impaler
 */
public class Game extends AbstractAppState implements ActionListener, Serializable {

	private static final long serialVersionUID = 1;
	public static String version = "0.2.2a";
	transient Main app = null;
	transient AppStateManager state = null;
	int MasterSeed;
	Dice PawnDice = new Dice();
	GameMap MainMap;
	Geology MapGeology;
	public Settlement GameSettlement;
	Weather GameWeather;

	boolean Pause;
	int TickRate;
	long CurrentGameTick;
	float TickRounding;
	Ticker simulation;

	private float frameTimeAccululator;
	private float attainedTickRate;
	private float attainedTickRateAccumulator;
	private float attainedTickRateCounter;

	private long seconds;
	private long minutes;
	private long hours;
	private long days;

	int UniqueIDCounter;
	PriorityQueue<Temporal> TemporalQueue;
	Temporal[][] FastTemporalMatrix;
	int[] FastTemporalCounter;
	protected static final int FastArrySize = 1000;
	protected static final int FastTickLimit = (int) Temporal.TICKS_PER_SECOND * 2;

	HashMap<Integer, Actor> Actors;
	int ActorIDcounter = 0;
	transient ExecutorService Executor;
	transient Future lastUpdate;
	private transient String kingdomName;
	private transient String saveGameFileName;
	private transient GameScreenController gameScreenController;

	private StringBuffer GameClockStringBuffer;

	public Game() {

		TickRate = 1;
		TickRounding = 0;
		CurrentGameTick = Temporal.TICKS_PER_DAY / 2;
		UniqueIDCounter = 0;
		Pause = true;

		TemporalQueue = new PriorityQueue<Temporal>();
		FastTemporalMatrix = new Temporal[FastTickLimit][FastArrySize];
		FastTemporalCounter = new int[FastTickLimit];
		GameClockStringBuffer = new StringBuffer();
	}

	@Override
	public void initialize(AppStateManager stateManager, Application app) {
		super.initialize(stateManager, app);
		this.app = (Main) app;
		this.state = stateManager;
		Main core = (Main) app;
		Executor = core.getThreadPool();

		registerWithInput(app.getInputManager());
	}

	public boolean initializeGame(short X, short Y, String SeedString, String kingdomName) {
		MasterSeed = SeedString.hashCode();
		this.setKingdomName(kingdomName);
		setSaveGameFileName(null);
		PawnDice.seed(MasterSeed);

		MapGeology = new Geology();
		MapGeology.initialize(MasterSeed);
		MapGeology.generateWorldHeightMap(X, Y);

		MainMap = new GameMap(MasterSeed);

		GameWeather = new Weather();
		addTemporal(GameWeather);

		buildMapChunks((short) 0, (short) 0, (byte) X, (byte) Y);

		GameSettlement = new Settlement();
		Actors = new HashMap<Integer, Actor>();

		return true;
	}

	boolean buildMapChunks(short X, short Y, byte Width, byte Height) {
		short SizeX = (short) (X + Width);
		short SizeY = (short) (Y + Height);

		// Create and add Chunks with shape and material data
		for (int x = X; x < SizeX; x++) {
			for (int y = Y; y < SizeY; y++) {
				MapGeology.generateChunkHeight(x, y, (float) 10.0, (float) 1.5);

				int zBottom, zTop;
				if ((MapGeology.getChunkBottomZLevel() - 2) < 0) {
					zBottom = ((MapGeology.getChunkBottomZLevel() - 2) / BlockCoordinate.CHUNK_EDGE_SIZE) - 1;
				} else { 
					zBottom = ((MapGeology.getChunkBottomZLevel() - 2) / BlockCoordinate.CHUNK_EDGE_SIZE);
				}
				if ((MapGeology.getChunkTopZLevel() + 2) < 0) {
					zTop = ((MapGeology.getChunkTopZLevel() + 2) / BlockCoordinate.CHUNK_EDGE_SIZE) - 1;
				} else { 
					zTop = ((MapGeology.getChunkTopZLevel() + 2) / BlockCoordinate.CHUNK_EDGE_SIZE);
				}

				Sector targetSector = MainMap.getSector(new SectorCoordinate((byte)0, (byte)0));
				for (int z = zBottom; z <= zTop; z++) {
					ChunkCoordinate TargetChunkCoordinates = new ChunkCoordinate(x, y, z);
					Chunk targetChunk = targetSector.getChunk(TargetChunkCoordinates);
					MapGeology.loadChunkData(targetChunk);
				}
			}
		}

		MainMap.generateFirstLight();

		for (Sector targetSector : MainMap.getSectorCollection()) {
			for (Chunk TargetChunk : targetSector.getChunkCollection()) {
				for (int i = 0; i < BlockCoordinate.CHUNK_DETAIL_LEVELS; i++) {
					TargetChunk.buildFaces(i);
				}
				TargetChunk.growGrass();
			}
		}

		return true;
	}

	public Pawn spawnPawn(MapCoordinate SpawnCoordinates, short CreatureTypeID) {
		Pawn NewPawn = new Pawn(CreatureTypeID, ActorIDcounter, MasterSeed, SpawnCoordinates);
		ActorIDcounter++;
		Actors.put(NewPawn.getID(), NewPawn);
		addTemporal(NewPawn);
		return NewPawn;
	}

	public Citizen SpawnCitizen(short CreatureTypeID, MapCoordinate SpawnCoordinates) {
		if (SpawnCoordinates != null) {
			Citizen NewCitizen = new Citizen(CreatureTypeID, ActorIDcounter, PawnDice.roll(0, MasterSeed), SpawnCoordinates);
			ActorIDcounter++;
			Actors.put(NewCitizen.getID(), NewCitizen);
			GameSettlement.addCitizen(NewCitizen);
			addTemporal(NewCitizen);
			return NewCitizen;
		} else {
			return null;
		}
	}

	boolean addTemporal(Temporal NewTemporal) {
		TemporalQueue.add(NewTemporal);
		return true;
	}

	public GameMap getMap() {
		return MainMap;
	}

	public Settlement getSettlement() {
		return GameSettlement;
	}

	public Weather getWeather() {
		return GameWeather;
	}

	public void volumeSelectionCompleted(VolumeSelection newVolume) {
		// What dose it mean?  need some kind of priming knowlege

		JobManager jobs = GameSettlement.getJobManager();
		ExcavateJob newJob = new ExcavateJob(MainMap);
		ArrayList Volumes = new ArrayList<VolumeSelection>();
		Volumes.add(newVolume);
		Zone newZone = getMap().createZone(Volumes);

		newJob.addDesignations(newVolume, newZone, new BlockShape(BlockShape.CUBE_BOTTOM_HEIGHT));
		jobs.addJob(newJob);
	}

	public HashMap<Integer, Actor> getActors() {
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
		inputManager.addListener(this, inputs);
	}

	@Override
	public void update(float tpf) {
		if (!Pause && app.Focus) {
			frameTimeAccululator += tpf;
			if (lastUpdate == null || lastUpdate.isDone()) {
				float TargetTicks = TickRate * tpf * Temporal.TICKS_PER_SECOND;
				TargetTicks += TickRounding;
				int FullTicks = (int) TargetTicks;
				TickRounding = TargetTicks - FullTicks;

				attainedTickRateAccumulator += TargetTicks / (frameTimeAccululator * Temporal.TICKS_PER_SECOND);
				attainedTickRateCounter++;

				if (attainedTickRateCounter == 100) {
					attainedTickRate = attainedTickRateAccumulator / 100;
					attainedTickRateAccumulator = 0;
					attainedTickRateCounter = 0;
				}

				simulation = new Ticker(this);
				simulation.windup(FullTicks);
				lastUpdate = Executor.submit(simulation);

				seconds = CurrentGameTick / Temporal.TICKS_PER_SECOND;
				minutes = CurrentGameTick / Temporal.TICKS_PER_MINUTE;
				hours = CurrentGameTick / Temporal.TICKS_PER_HOUR;
				days = CurrentGameTick / Temporal.TICKS_PER_DAY;
				
				if (gameScreenController != null) {
					// update UI with any updated state, since some windows can be open while unpaused.
					gameScreenController.update();
				}
				frameTimeAccululator = 0;
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

	// this method is used by serialization
	private void readObject(ObjectInputStream ois) throws ClassNotFoundException, IOException {
		// default deserialization
		ois.defaultReadObject();
		// fix transients
		app = Main.app;
		state = Main.app.getStateManager();
		Executor = Main.app.getThreadPool();
		lastUpdate = null;
	}

	public String getTimeString() {
		GameClockStringBuffer.delete(0, GameClockStringBuffer.length());

		GameClockStringBuffer.append("DAY ");
		GameClockStringBuffer.append(days);
		GameClockStringBuffer.append("  -  ");

		GameClockStringBuffer.append(Utils.padLeadingZero(hours %24));
		GameClockStringBuffer.append(":");
		GameClockStringBuffer.append(Utils.padLeadingZero(minutes % 60));
		GameClockStringBuffer.append(":");
		GameClockStringBuffer.append(Utils.padLeadingZero(seconds % 60));

		GameClockStringBuffer.append("   ");
		GameClockStringBuffer.append(attainedTickRate);

		return GameClockStringBuffer.toString();
	}

	public String getKingdomName() {
		return kingdomName;
	}

	public void setKingdomName(String kingdomName) {
		this.kingdomName = kingdomName;
	}

	public String getSaveGameFileName() {
		return saveGameFileName;
	}

	public void setSaveGameFileName(String saveGameFileName) {
		this.saveGameFileName = saveGameFileName;
	}

	public GameScreenController getGameScreenController() {
		return gameScreenController;
	}

	public void setGameScreenController(GameScreenController gameScreenController) {
		this.gameScreenController = gameScreenController;
	}

	@Override
	public void cleanup() {
		super.cleanup();
		app.getInputManager().removeListener(this);
	}
}
