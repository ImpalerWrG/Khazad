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
import Interface.GameCameraState;
import Map.CellCoordinate;

import Map.CubeCoordinate;
import Map.TileBuilder;
import Nifty.GUI;
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

/**
 *
 * @author Impaler
 */
public class MapRenderer extends AbstractAppState {

	SimpleApplication app = null;
	AppStateManager state = null;
	AssetManager assetmanager = null;

	Node MapNode = null;
	Node sunnyterrainNode = null;
	Node darkterrainNode = null;
	TileBuilder builder;
	Game game;

	ConcurrentHashMap<CellCoordinate, Node> LightCellNodeMap;
	ConcurrentHashMap<CellCoordinate, Node> DarkCellNodeMap;
	ConcurrentHashMap<Integer, Node> ZMapLight;
	ConcurrentHashMap<Integer, Node> ZMapDark;

	Semaphore semaphore;
	boolean SunnyRendering, DarkRendering = true;

	int Top;
	int Bottom;

	public MapRenderer() {
		LightCellNodeMap = new ConcurrentHashMap<CellCoordinate, Node>();
		DarkCellNodeMap = new ConcurrentHashMap<CellCoordinate, Node>();
		ZMapLight = new ConcurrentHashMap<Integer, Node>();
		ZMapDark = new ConcurrentHashMap<Integer, Node>();
		builder = new TileBuilder();

		semaphore = new Semaphore(1);
	}

	@Override
	public void initialize(AppStateManager stateManager, Application app) {
		super.initialize(stateManager, app);
		this.app = (SimpleApplication) app;
		this.state = stateManager;
		this.assetmanager = app.getAssetManager();

		//registerWithInput(app.getInputManager());
	}

	public void attachToGame(Game TargetGame) {
		this.game = TargetGame;
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
		this.game = null;
		MapNode = null;
		darkterrainNode = null;
		sunnyterrainNode = null;

		LightCellNodeMap.clear();
		DarkCellNodeMap.clear();
		ZMapLight.clear();
		ZMapDark.clear();
	}

	public Node getCellNodeLight(CellCoordinate TargetCoordinates) {
		Node CellNode = null;
		try {  // Semaphore prevents multiple copies of the same Cell node from being created
			semaphore.acquire();
			try {
				CellNode = LightCellNodeMap.get(TargetCoordinates);
				if (CellNode == null) {
					CellNode = new Node("LightNode" + TargetCoordinates.toString());

					float x = (float) (TargetCoordinates.X * CubeCoordinate.CELLEDGESIZE);
					float y = (float) (TargetCoordinates.Y * CubeCoordinate.CELLEDGESIZE);
					CellNode.move(x, y, 0);

					getZNodeLight(TargetCoordinates.Z).attachChild(CellNode);
					LightCellNodeMap.put(TargetCoordinates, CellNode);
				}
			} finally {
				semaphore.release();
			}
		} catch (final InterruptedException e) {
			System.err.println(e.getLocalizedMessage());
			System.err.println(e.getMessage());
			System.err.println(e.toString());
		}
		return CellNode;
	}

	public Node getCellNodeDark(CellCoordinate TargetCell) {
		Node CellNode = DarkCellNodeMap.get(TargetCell);
		try {  // Semaphore prevents multiple copies of the same Cell node from being created
			semaphore.acquire();
			try {
				if (CellNode == null) {
					CellNode = new Node("DarkCellNode");

					float x = (float) (TargetCell.X * CubeCoordinate.CELLEDGESIZE);
					float y = (float) (TargetCell.Y * CubeCoordinate.CELLEDGESIZE);

					CellNode.move(x, y, 0);

					getZNodeDark(TargetCell.Z).attachChild(CellNode);
					DarkCellNodeMap.put(TargetCell, CellNode);
				}
			} finally {
				semaphore.release();
			}
		} catch (final InterruptedException e) {
			System.err.println(e.getLocalizedMessage());
			System.err.println(e.getMessage());
			System.err.println(e.toString());
		}
		return CellNode;
	}

	public Node getZNodeLight(int zlevel) {
		//TODO use a primite equipped map to eliminate Integer
		Node targetnode = ZMapLight.get(new Integer(zlevel));
		if (targetnode == null) {

			targetnode = new Node("ZMapLightNode");
			targetnode.move(0, 0, zlevel);
			ZMapLight.put(new Integer(zlevel), targetnode);
			sunnyterrainNode.attachChild(targetnode);
		}
		return targetnode;
	}

	public Node getZNodeDark(int zlevel) {
		Node targetnode = ZMapDark.get(new Integer(zlevel));
		if (targetnode == null) {

			targetnode = new Node("ZMapDarkNode");
			targetnode.move(0, 0, zlevel);
			ZMapDark.put(new Integer(zlevel), targetnode);
			darkterrainNode.attachChild(targetnode);
		}
		return targetnode;
	}

	public void setSliceLevels(int top, int bottom) {
		Top = top;
		Bottom = bottom;

		for (Node targetnode : ZMapLight.values()) {
			float Z = targetnode.getLocalTranslation().getZ();
			if (Z <= Top && SunnyRendering) {
				sunnyterrainNode.attachChild(targetnode);
			} else {
				sunnyterrainNode.detachChild(targetnode);
			}
		}

		for (Node targetnode : ZMapDark.values()) {
			float Z = targetnode.getLocalTranslation().getZ();
			if (Z <= Top) {
				darkterrainNode.attachChild(targetnode);
			} else {
				darkterrainNode.detachChild(targetnode);
			}
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
	}
}
