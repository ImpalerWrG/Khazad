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

import java.util.Collection;
import java.util.concurrent.ExecutorService;

/**
 * Rendering class for Terrain, tracks all Scene Nodes that Terrain geometry
 * attaches too and rebuilds the geometry when Cells are dirty. Division of
 * Terrain into light/dark allows easy hiding of surface terrain and restriction
 * of directional sunlight to appropriate surfaces.
 *
 * @author Impaler
 */
public class TerrainRenderer extends AbstractAppState implements ActionListener {

	SimpleApplication app = null;
	AppStateManager state = null;
	AssetManager assetmanager = null;
	Game game = null;
	TileBuilder builder;
	LodControl TerrainLodControler;
	boolean SunnyRendering = true;
	boolean DarkRendering = true;
	boolean TerrainRendering = true;
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

	public void attachToGame(Game TargetGame) {
		this.game = TargetGame;
		this.TerrainLodControler = new LodControl();
	}

	public void onAction(String name, boolean keyPressed, float tpf) {
		if (this.isEnabled()) {
			if (name.equals("TerrainRenderToggle") && keyPressed) {
				TerrainRendering = !TerrainRendering;
			}
			if (name.equals("SunnyRenderToggle") && keyPressed) {
				SunnyRendering = !SunnyRendering;
			}
		}
	}

	public void registerWithInput(InputManager inputManager) {
		String[] inputs = {"TerrainRenderToggle", "SunnyRenderToggle"};

		inputManager.addMapping("TerrainRenderToggle", new KeyTrigger(KeyInput.KEY_T));
		inputManager.addMapping("SunnyRenderToggle", new KeyTrigger(KeyInput.KEY_L));
		inputManager.addListener(this, inputs);
	}

	public void rebuildDirtyCells(Collection<Cell> cells) {
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

	public void setTerrainRendering(Collection<Cell> cells) {
		Spatial.CullHint Sunnyhint = Spatial.CullHint.Always;
		Spatial.CullHint Darkhint = Spatial.CullHint.Always;

		if (SunnyRendering)
			Sunnyhint = Spatial.CullHint.Dynamic;

		if (DarkRendering)
			Darkhint = Spatial.CullHint.Dynamic;

		if (!TerrainRendering)
			Darkhint = Sunnyhint = Spatial.CullHint.Always;

		for (Cell target : cells) {
			CellCoordinate Coords = target.getCellCoordinates();

			MapRenderer Renderer = state.getState(MapRenderer.class);
			Node CellLight = Renderer.getCellNodeLight(Coords);
			Node CellDark = Renderer.getCellNodeDark(Coords);

			Spatial light = CellLight.getChild("LightGeometry Cell" + target.toString());
			Spatial dark = CellDark.getChild("DarkGeometry Cell" + target.toString());

			if (light != null)
				light.setCullHint(Sunnyhint);
			if (dark != null)
				dark.setCullHint(Darkhint);
		}
	}

	@Override
	public void update(float tpf) {
		if (this.game != null) {
			GameMap map = this.game.getMap();
			if (TerrainRendering) {
				setTerrainRendering(map.getCellCollection());
				rebuildDirtyCells(map.getCellCollection());
			} else {
				setTerrainRendering(map.getCellCollection());
			}
		}
	}
}