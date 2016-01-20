/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package Game;

import Core.Main;
import static Core.Main.app;
import static Core.Main.setProgress;

import Map.*;
import Map.Coordinates.*;
import Nifty.GUI;
import PathFinding.PathManager;

import Terrain.Geology;
import java.util.HashMap;
import java.util.concurrent.Callable;

/**
 *
 * @author Impaler
 */
public class MapGenerator implements Callable<Object>{

	Game game;
	short X, Y;
	String Name;
	int MasterSeed;

	public void setGame(Game game) {
		this.game = game;
	}

	public void setParameters(short X, short Y, String SeedString, String kingdomName) {
		this.X = X;  this.Y = Y;
		Name = kingdomName;

		this.MasterSeed = SeedString.hashCode();
	}

	public Object call() {
		game.setKingdomName(Name);
		game.setSaveGameFileName(null);
		game.PawnDice.seed(MasterSeed);

		game.MapGeology = new Geology();
		game.MapGeology.initialize(MasterSeed);

		game.MainMap = new GameMap(MasterSeed);

		game.GameWeather = new Weather();
		game.addTemporal(game.GameWeather);

		MapCoordinate SettlmentHeart = new MapCoordinate();
		SettlmentHeart.Chunk.X = ChunkCoordinate.SECTOR_EDGE_SIZE / 2;
		SettlmentHeart.Chunk.Y = ChunkCoordinate.SECTOR_EDGE_SIZE / 2;
		SettlmentHeart.Chunk.Z = ChunkCoordinate.SECTOR_EDGE_SIZE / 2;
		game.GameSettlement = new Settlement(SettlmentHeart);

		short Xorigin = (short) (SettlmentHeart.Chunk.X - (X / 2));
		short Yorigin = (short) (SettlmentHeart.Chunk.Y - (Y / 2));
		short Xend = (short) (SettlmentHeart.Chunk.X + (X / 2));
		short Yend = (short) (SettlmentHeart.Chunk.Y + (Y / 2));

		setProgress(.1f, "Building Chunk Geology");
		for (short x = Xorigin; x < Xend; x++) {
			for (short y = Yorigin; y < Yend; y++) {
				buildMapChunk(x, y, SettlmentHeart.getZ());
			}
		}

		setProgress(.15f, "Creating First Light");
		game.MainMap.generateFirstLight();

		setProgress(.2f, "Building Faces");
		for (Sector targetSector : game.MainMap.getSectorCollection()) {
			for (Chunk TargetChunk : targetSector.getChunkCollection()) {
				for (int i = 0; i < BlockCoordinate.CHUNK_DETAIL_LEVELS; i++) {
					TargetChunk.buildFaces(i);
				}
			}
		}

		setProgress(.6f, "Growing Grass");
		for (Sector targetSector : game.MainMap.getSectorCollection()) {
			for (Chunk TargetChunk : targetSector.getChunkCollection()) {
				TargetChunk.growGrass();
			}
		}

		game.Actors = new HashMap<Integer, Actor>();

		// PATHING
		setProgress(.7f, "Creating Pathfinding Abstraction");
		PathManager Pather = PathManager.getSingleton();
		Pather.initialize(Main.app.getStateManager(), Main.app);
		Pather.createMapAbstraction(game.getMap());
		Main.app.getStateManager().attach(Pather);

		setProgress(.9f, "Spawning Dwarves");
		short DwarfID = Data.DataManager.getDataManager().getLabelIndex("CREATURE_DWARF");
		for (int i = 0; i < 300; i++) {
			game.SpawnCitizen(DwarfID, Pather.Tester.getRandomPassableCoordinate());
		}

		Main.attachRenderers(game);
		GUI nifty = app.getStateManager().getState(GUI.class);
		nifty.gotoScreen("GameScreen");

		return null;
	}

	private boolean buildMapChunk(short x, short y, int baseHeight) {
		MapCoordinate targetCoordinates = new MapCoordinate();
		// Create and add Chunks with shape and material data

		targetCoordinates.Chunk.X = (short) x;
		targetCoordinates.Chunk.Y = (short) y;
		game.MapGeology.generateChunk(targetCoordinates, baseHeight);

		int zTop = game.MapGeology.getChunkTopZLevel() / BlockCoordinate.CHUNK_EDGE_SIZE;
		int zBottom = game.MapGeology.getChunkBottomZLevel() / BlockCoordinate.CHUNK_EDGE_SIZE;

		Sector targetSector = game.MainMap.getSector(new SectorCoordinate((byte)0, (byte)0));
		for (int z = zBottom; z <= zTop; z++) {
			ChunkCoordinate TargetChunkCoordinates = new ChunkCoordinate(x, y, z);
			Chunk targetChunk = targetSector.getChunk(TargetChunkCoordinates);
			game.MapGeology.loadChunkData(targetChunk);
		}

		return true;
	}
}
