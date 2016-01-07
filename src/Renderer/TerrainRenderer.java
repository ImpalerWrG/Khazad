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

package Renderer;

import Map.Coordinates.ChunkCoordinate;
import Map.*;
import Game.Game;
import Interface.GameCameraState;
import Map.Coordinates.BlockCoordinate;

import java.util.concurrent.ConcurrentHashMap;

import com.jme3.app.Application;
import com.jme3.app.SimpleApplication;
import com.jme3.app.state.AbstractAppState;
import com.jme3.app.state.AppStateManager;

import com.jme3.asset.AssetManager;

import com.jme3.bounding.BoundingBox;
import com.jme3.math.Vector3f;

import com.jme3.scene.control.LodControl;
import com.jme3.scene.Node;
import com.jme3.scene.Spatial;

import java.util.Collection;
import java.util.concurrent.ExecutorService;

/**
 * Rendering class for Terrain, tracks all Scene Nodes that Terrain geometry
 * attaches too and rebuilds the geometry when Chunks are dirty. Division of
 * Terrain into light/dark allows easy hiding of surface terrain and restriction
 * of directional sunlight to appropriate surfaces.
 *
 * @author Impaler
 */
public class TerrainRenderer extends AbstractAppState {

	SimpleApplication app = null;
	AppStateManager state = null;
	AssetManager assetmanager = null;
	GameCameraState CameraState = null;
	Game game = null;
	TileBuilder builder;
	int LevelofDetail;
	private boolean TerrainRenderingToggle = true;
	Spatial.CullHint TerrainHint = Spatial.CullHint.Never;
	ExecutorService Executor;
	ConcurrentHashMap<ChunkCoordinate, Chunk> MeshedChunks;

	public TerrainRenderer(ExecutorService Threadpool) {
		Executor = Threadpool;
		builder = new TileBuilder();
		MeshedChunks = new ConcurrentHashMap<ChunkCoordinate, Chunk>();
	}

	@Override
	public void initialize(AppStateManager stateManager, Application app) {
		super.initialize(stateManager, app);
		this.app = (SimpleApplication) app;
		this.state = stateManager;
		this.assetmanager = app.getAssetManager();
	}

	public void attachToGame(Game TargetGame) {
		this.game = TargetGame;
	}

	public void queueChunkBuild(Chunk targetChunk, int DetailLevel) {
		MapRenderer Renderer = state.getState(MapRenderer.class);
		ChunkCoordinate Coords = targetChunk.getChunkCoordinates();

		MeshedChunks.put(Coords, targetChunk);
		TerrainBuilder Builder = new TerrainBuilder(app, targetChunk, builder, DetailLevel);

		Builder.setNodes(Renderer.getChunkNodeLight(Coords), Renderer.getChunkNodeDark(Coords));
		Builder.setHint(TerrainHint);
		Executor.submit(Builder);

		targetChunk.setDirtyTerrainRendering(false);
	}

	public void queueChunkDestroy(Chunk targetChunk, int DetailLevel) {
		MapRenderer Renderer = state.getState(MapRenderer.class);
		ChunkCoordinate Coords = targetChunk.getChunkCoordinates();

		MeshedChunks.remove(Coords);
		TerrainDestroyer Destroyer = new TerrainDestroyer(app, targetChunk, DetailLevel);

		Destroyer.setNodes(Renderer.getChunkNodeLight(Coords), Renderer.getChunkNodeDark(Coords));
		Executor.submit(Destroyer);

		targetChunk.setDirtyTerrainRendering(true);
	}

	public void rebuildDirtyChunks(Collection<Chunk> cells) {
		for (Chunk targetChunk : MeshedChunks.values()) {
			if (targetChunk.isTerrainRenderingDirty())
				queueChunkBuild(targetChunk, this.LevelofDetail);
		}
	}

	public void SwapFrustrumChunks() {
		GameMap map = this.game.getMap();
		for (Sector targetSector : map.getSectorCollection()) {
			
			// DO bounding box test on whole sector
			Collection<Chunk> cells = targetSector.getChunkCollection();

			BoundingBox ChunkBox = new BoundingBox();
			ChunkBox.setXExtent(BlockCoordinate.CHUNK_EDGE_SIZE);
			ChunkBox.setYExtent(BlockCoordinate.CHUNK_EDGE_SIZE);
			ChunkBox.setZExtent(BlockCoordinate.CHUNK_EDGE_SIZE);
			ChunkBox.setCheckPlane(0);

			this.CameraState = state.getState(GameCameraState.class);

			// Add Chunks newly entering the Frustrum
			for (Chunk targetChunk : cells) {
				ChunkCoordinate Coords = targetChunk.getChunkCoordinates();
				Vector3f Center = Coords.getVector();
				ChunkBox.setCenter(Center);
				if (this.CameraState.contains(ChunkBox)) {
					if (targetChunk.isTerrainRenderingDirty()) {
						queueChunkBuild(targetChunk, this.LevelofDetail);
					}
				}
			}

			// Remove Chunks nolonger in the Frustrum
			for (Chunk targetChunk : MeshedChunks.values()) {
				ChunkCoordinate Coords = targetChunk.getChunkCoordinates();
				Vector3f Center = Coords.getVector();
				ChunkBox.setCenter(Center);
				if (this.CameraState.contains(ChunkBox) == false) {
					queueChunkDestroy(targetChunk, this.LevelofDetail);
				}
			}
		}
	}

	public boolean getTerrainRendering() {
		return TerrainRenderingToggle;
	}

	/**
	 * @param TerrainRendering the TerrainRendering to set
	 */
	public void setTerrainRendering(boolean NewValue) {
		this.TerrainRenderingToggle = NewValue;
		TerrainHint = Spatial.CullHint.Always;

		if (getTerrainRendering())
			TerrainHint = Spatial.CullHint.Dynamic;

		GameMap map = this.game.getMap();
		for (Sector targetSector : map.getSectorCollection()) {
			for (Chunk target : targetSector.getChunkCollection()) {
				ChunkCoordinate Coords = target.getChunkCoordinates();

				MapRenderer Renderer = state.getState(MapRenderer.class);
				Node ChunkLight = Renderer.getChunkNodeLight(Coords);
				Node ChunkDark = Renderer.getChunkNodeDark(Coords);

				Spatial light = ChunkLight.getChild("LightGeometry Chunk " + target.toString() + "DetailLevel " + this.LevelofDetail);
				Spatial dark = ChunkDark.getChild("DarkGeometry Chunk " + target.toString() + "DetailLevel " + this.LevelofDetail);

				if (light != null)
					light.setCullHint(TerrainHint);
				if (dark != null)
					dark.setCullHint(TerrainHint);
			}
		}
	}

	public void setLevelofDetail(float ZoomLevel) {
		int NewDetailLevel = 0;

		if (ZoomLevel > 60)
			NewDetailLevel = 1;
		if (ZoomLevel > 120)
			NewDetailLevel = 2;
		if (ZoomLevel > 240)
			NewDetailLevel = 3;
		if (ZoomLevel > 480)
			NewDetailLevel = 4;

		if (NewDetailLevel != this.LevelofDetail) {
			this.LevelofDetail = NewDetailLevel;

			GameMap map = this.game.getMap();
			for (Sector targetSector : map.getSectorCollection()) {
				for (Chunk target : targetSector.getChunkCollection()) {
					setChunkDetailLevel(target, this.LevelofDetail);
				}
			}
		}
	}

	public void changeLevelofDetal(int Change) {
		if (Change < 0 && this.LevelofDetail == 0)
			return;
		if (Change > 1 && this.LevelofDetail == 4)
			return;

		this.LevelofDetail += Change;

		if (this.LevelofDetail < 0)
			this.LevelofDetail = 0;

		if (this.LevelofDetail > 4)
			this.LevelofDetail = 4;

		GameMap map = this.game.getMap();
		for (Sector targetSector : map.getSectorCollection()) {
			for (Chunk target : targetSector.getChunkCollection()) {
				setChunkDetailLevel(target, this.LevelofDetail);
			}
		}
	}

	private void setChunkDetailLevel(Chunk TargetChunk, int LevelofDetail) {
		ChunkCoordinate Coords = TargetChunk.getChunkCoordinates();

		MapRenderer Renderer = state.getState(MapRenderer.class);
		Node ChunkLight = Renderer.getChunkNodeLight(Coords);
		Node ChunkDark = Renderer.getChunkNodeDark(Coords);

		for (int i = 0; i < BlockCoordinate.CHUNK_DETAIL_LEVELS; i++) {
			Spatial.CullHint hint = Spatial.CullHint.Always;
			if (i == LevelofDetail && TerrainRenderingToggle)
				hint = Spatial.CullHint.Dynamic;

			Spatial light = ChunkLight.getChild("LightGeometry Chunk " + TargetChunk.toString() + "DetailLevel " + i);
			Spatial dark = ChunkDark.getChild("DarkGeometry Chunk " + TargetChunk.toString() + "DetailLevel " + i);

			if (light != null)
				light.setCullHint(hint);
			if (dark != null)
				dark.setCullHint(hint);
		}
	}

	@Override
	public void update(float tpf) {
		if (this.game != null) {
			GameMap map = this.game.getMap();
			if (TerrainRenderingToggle) {
				for (Sector targetSector : map.getSectorCollection()) {
					rebuildDirtyChunks(targetSector.getChunkCollection());
				}
			}
		}
	}
}