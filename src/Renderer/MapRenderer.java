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

import Game.Game;

import Map.Coordinates.ChunkCoordinate;
import Map.Coordinates.BlockCoordinate;
import Map.TileBuilder;

import com.jme3.app.Application;
import com.jme3.app.SimpleApplication;
import com.jme3.app.state.AbstractAppState;
import com.jme3.app.state.AppStateManager;
import com.jme3.asset.AssetManager;
import com.jme3.light.AmbientLight;

import com.jme3.math.ColorRGBA;
import com.jme3.scene.Node;
import com.jme3.scene.Spatial;

import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.Semaphore;

import gnu.trove.map.TIntObjectMap;
import gnu.trove.map.hash.TIntObjectHashMap;
import java.util.concurrent.ExecutorService;

/**
 *
 * @author Impaler
 */
public class MapRenderer extends AbstractAppState {

	SimpleApplication app = null;
	AppStateManager state = null;
	AssetManager assetmanager = null;
	ExecutorService Executor;

	Node MapNode = null;
	Node sunnyterrainNode = null;
	Node darkterrainNode = null;
	TileBuilder builder;
	//Game game;

	ConcurrentHashMap<ChunkCoordinate, Node> LightChunkNodeMap;
	ConcurrentHashMap<ChunkCoordinate, Node> DarkChunkNodeMap;
	TIntObjectMap<Node> ZMapLight;
	TIntObjectMap<Node> ZMapDark;

	Semaphore semaphore;
	boolean SunnyRendering, DarkRendering;

	int Top, Bottom;
	boolean DirtySlice;

	public MapRenderer(ExecutorService Threadpool) {
		Executor = Threadpool;

		LightChunkNodeMap = new ConcurrentHashMap<ChunkCoordinate, Node>();
		DarkChunkNodeMap = new ConcurrentHashMap<ChunkCoordinate, Node>();
		ZMapLight = new TIntObjectHashMap<Node>();
		ZMapDark = new TIntObjectHashMap<Node>();
		builder = new TileBuilder();

		semaphore = new Semaphore(1);
		SunnyRendering = DarkRendering = true;
	}

	@Override
	public void initialize(AppStateManager stateManager, Application app) {
		super.initialize(stateManager, app);
		this.app = (SimpleApplication) app;
		this.state = stateManager;
		this.assetmanager = app.getAssetManager();
	}

	public void attachToGame(Game TargetGame) {
		//this.game = TargetGame;
		this.MapNode = new Node("MapNode");
		this.app.getRootNode().attachChild(MapNode);

		darkterrainNode = new Node("DarkTerrainNode");
		sunnyterrainNode = new Node("SunnyTerrainNode");
		MapNode.attachChild(darkterrainNode);
		MapNode.attachChild(sunnyterrainNode);

		ColorRGBA Suncolor = ColorRGBA.White;
		AmbientLight Darkglow = new AmbientLight();
		Darkglow.setColor(Suncolor.mult(1.8f));
		darkterrainNode.addLight(Darkglow);

		AmbientLight Lightglow = new AmbientLight();
		Lightglow.setColor(Suncolor.mult(1.8f));
		sunnyterrainNode.addLight(Lightglow);

		TargetGame.getWeather().attatchSun(sunnyterrainNode);
	}

	public void detachFromGame() {
		//this.game = null;
		MapNode = null;
		darkterrainNode = null;
		sunnyterrainNode = null;

		LightChunkNodeMap.clear();
		DarkChunkNodeMap.clear();
		ZMapLight.clear();
		ZMapDark.clear();
	}

	public Node getChunkNodeLight(ChunkCoordinate TargetCoordinates) {
		Node ChunkNode = null;
		try {  // Semaphore prevents multiple copies of the same Chunk node from being created
			semaphore.acquire();
			try {
				ChunkNode = LightChunkNodeMap.get(TargetCoordinates);
				if (ChunkNode == null) {
					ChunkNode = new Node("LightNode" + TargetCoordinates.toString());
					ChunkNode.move(TargetCoordinates.getVector());

					getZNodeLight(TargetCoordinates.Z).attachChild(ChunkNode);
					LightChunkNodeMap.put(TargetCoordinates, ChunkNode);
				}
			} finally {
				semaphore.release();
			}
		} catch (final InterruptedException e) {
			System.err.println(e.getLocalizedMessage());
			System.err.println(e.getMessage());
			System.err.println(e.toString());
		}
		return ChunkNode;
	}

	public Node getChunkNodeDark(ChunkCoordinate TargetCoordinates) {
		Node ChunkNode = DarkChunkNodeMap.get(TargetCoordinates);
		try {  // Semaphore prevents multiple copies of the same Chunk node from being created
			semaphore.acquire();
			try {
				if (ChunkNode == null) {
					ChunkNode = new Node("DarkChunkNode");
					ChunkNode.move(TargetCoordinates.getVector());

					getZNodeDark(TargetCoordinates.Z).attachChild(ChunkNode);
					DarkChunkNodeMap.put(TargetCoordinates, ChunkNode);
				}
			} finally {
				semaphore.release();
			}
		} catch (final InterruptedException e) {
			System.err.println(e.getLocalizedMessage());
			System.err.println(e.getMessage());
			System.err.println(e.toString());
		}
		return ChunkNode;
	}

	public Node getZNodeLight(int zlevel) {
		//TODO use a primite equipped map to eliminate Integer
		Node targetnode = ZMapLight.get(zlevel);
		if (targetnode == null) {

			targetnode = new Node("ZMapLightNode");
			targetnode.move(0, 0, zlevel);
			ZMapLight.put(zlevel, targetnode);
			sunnyterrainNode.attachChild(targetnode);
		}
		return targetnode;
	}

	public Node getZNodeDark(int zlevel) {
		Node targetnode = ZMapDark.get(zlevel);
		if (targetnode == null) {

			targetnode = new Node("ZMapDarkNode");
			targetnode.move(0, 0, zlevel);
			ZMapDark.put(zlevel, targetnode);
			darkterrainNode.attachChild(targetnode);
		}
		return targetnode;
	}

	public void setSliceLevels(int top, int bottom) {
		try {  // Semaphore prevents multiple copies of the same Chunk node from being created
			semaphore.acquire();
			try {
				Top = top;
				Bottom = bottom;
				DirtySlice = true;
			} finally {
				semaphore.release();
			}
		} catch (final InterruptedException e) {
			System.err.println(e.getLocalizedMessage());
			System.err.println(e.getMessage());
			System.err.println(e.toString());
		}
	}

	public void setSunnyRendering(boolean SunnyRendering) {
		this.SunnyRendering = SunnyRendering;
		if (SunnyRendering) {
			sunnyterrainNode.setCullHint(Spatial.CullHint.Dynamic);
		} else {
			sunnyterrainNode.setCullHint(Spatial.CullHint.Always);
		}
	}

	public void setDarkRendering(boolean DarkRendering) {
		this.DarkRendering = DarkRendering;
		if (DarkRendering) {
			darkterrainNode.setCullHint(Spatial.CullHint.Dynamic);
		} else {
			darkterrainNode.setCullHint(Spatial.CullHint.Always);
		}
	}

	public Node getMapNode() {
		return MapNode;
	}

	public Node getSunTerrainNode() {
		return sunnyterrainNode;
	}

	public Node getDarkTerrainNode() {
		return darkterrainNode;
	}

	@Override
	public void update(float tpf) {
		if (DirtySlice) {
			DirtySlice = false;
			MapSlicer Slicer = new MapSlicer(app, this);
			Executor.submit(Slicer);
		}
	}
}
