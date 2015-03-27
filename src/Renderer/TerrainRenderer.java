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

import Core.Main;
import Map.*;
import Game.Game;

import com.jme3.app.Application;
import com.jme3.app.SimpleApplication;
import com.jme3.app.state.AbstractAppState;
import com.jme3.app.state.AppStateManager;

import com.jme3.asset.AssetManager;
import com.jme3.input.InputManager;
import com.jme3.input.KeyInput;
import com.jme3.input.controls.KeyTrigger;
import com.jme3.input.controls.ActionListener;


import com.jme3.scene.control.LodControl;
import com.jme3.scene.Node;
import com.jme3.scene.Spatial;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.Serializable;
import java.util.Collection;

import java.util.concurrent.ExecutorService;

/**
 * Rendering class for Terrain, tracks all Scene Nodes that Terrain geometry
 * attaches too and rebuilds the geometry when Cells are dirty.  Division of
 * Terrain into light/dark allows easy hiding of surface terrain and restriction
 * of directional sunlight to appropriate surfaces.
 * 
 * @author Impaler
 */
public class TerrainRenderer extends AbstractAppState implements ActionListener {

	transient SimpleApplication app = null;
	transient AppStateManager state = null;
	transient AssetManager assetmanager = null;

	transient Game game = null;

	TileBuilder builder;
	transient LodControl TerrainLodControler;

	boolean SunnyRendering = true;
	boolean DisplayToggle = true;

	ExecutorService Executor;

	public TerrainRenderer(ExecutorService Threadpool) {
		Executor = Threadpool;
		builder = new TileBuilder();
	}

	@Override
	public void initialize(AppStateManager stateManager, Application app) {
		super.initialize(stateManager, app);
		this.app = (SimpleApplication) app;
		this.state = stateManager;
		this.assetmanager = app.getAssetManager();

		registerWithInput(app.getInputManager());
	}
	
	// this method is used by serialization
	private void readObject(ObjectInputStream ois) throws ClassNotFoundException, IOException {
		// default deserialization
		ois.defaultReadObject();
		// fix transients
		app = Main.app;
		state = Main.app.getStateManager();
		assetmanager = Main.app.getAssetManager();
	}

	public void attachToGame(Game TargetGame) {
		this.game = TargetGame;
		this.TerrainLodControler = new LodControl();
	}

	public void onAction(String name, boolean keyPressed, float tpf) {
		if (this.isEnabled()) {
			if (name.equals("TerrainRenderToggle") && keyPressed) {
				DisplayToggle = !DisplayToggle;
			}
		}
	}

	public void registerWithInput(InputManager inputManager) {
		String[] inputs = {"TerrainRenderToggle"};

		inputManager.addMapping("TerrainRenderToggle", new KeyTrigger(KeyInput.KEY_T));		
		inputManager.addListener(this, inputs);
	}

	public void RebuildDirtyCells(Collection<Cell> cells) {
		for (Cell target : cells) {
			if (target.isTerrainRenderingDirty()) {
				CellCoordinate Coords = target.getCellCoordinates();
				TerrainBuilder Builder = new TerrainBuilder(app, target, builder, TerrainLodControler);
				MapRenderer Renderer = state.getState(MapRenderer.class);

				Builder.setNodes(Renderer.getCellNodeLight(Coords), Renderer.getCellNodeDark(Coords));
				Executor.submit(Builder);

				target.setDirtyTerrainRendering(false);
			}
		}
	}

	public void SetTerrainRendering(Collection<Cell> cells, boolean show) {
		Spatial.CullHint hint = Spatial.CullHint.Always;
		if (show == true)
			hint = Spatial.CullHint.Dynamic;
		
		for (Cell target : cells) {
			CellCoordinate Coords = target.getCellCoordinates();

			MapRenderer Renderer = state.getState(MapRenderer.class);
			Node CellLight = Renderer.getCellNodeLight(Coords);
			Node CellDark = Renderer.getCellNodeDark(Coords);
			
			Spatial light = CellLight.getChild("LightGeometry Cell" + target.toString());
			Spatial dark = CellDark.getChild("DarkGeometry Cell" + target.toString());

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
			if (DisplayToggle) {
				SetTerrainRendering(map.getCellCollection(), true);
				RebuildDirtyCells(map.getCellCollection());
			} else {
				SetTerrainRendering(map.getCellCollection(), false);
			}
		}
	}
}