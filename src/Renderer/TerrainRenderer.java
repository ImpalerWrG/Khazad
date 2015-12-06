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
import Interface.GameCameraState;

import com.jme3.app.Application;
import com.jme3.app.SimpleApplication;
import com.jme3.app.state.AbstractAppState;
import com.jme3.app.state.AppStateManager;

import com.jme3.asset.AssetManager;
import com.jme3.input.InputManager;
import com.jme3.input.KeyInput;
import com.jme3.input.controls.KeyTrigger;
import com.jme3.input.controls.ActionListener;

import com.jme3.bounding.BoundingBox;
import com.jme3.math.Vector3f;

import com.jme3.scene.control.LodControl;
import com.jme3.scene.Node;
import com.jme3.scene.Spatial;

import java.util.ArrayList;
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
public class TerrainRenderer extends AbstractAppState {

	SimpleApplication app = null;
	AppStateManager state = null;
	AssetManager assetmanager = null;
	GameCameraState CameraState = null;
	Game game = null;

	TileBuilder builder;
	int LevelofDetail;
	private boolean TerrainRendering = true;
	ExecutorService Executor;
	ArrayList<Cell> MeshedCells;

	public TerrainRenderer(ExecutorService Threadpool) {
		Executor = Threadpool;
		builder = new TileBuilder();
		MeshedCells = new ArrayList<Cell>();
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

	public void rebuildDirtyCells(Collection<Cell> cells) {
		BoundingBox CellBox = new BoundingBox();
		CellBox.setXExtent(CubeCoordinate.CELLEDGESIZE);
		CellBox.setYExtent(CubeCoordinate.CELLEDGESIZE);
		CellBox.setZExtent(1);
		CellBox.setCheckPlane(0);

		this.CameraState = state.getState(GameCameraState.class);
		MapRenderer Renderer = state.getState(MapRenderer.class);

		for (Cell targetCell : cells) {
			CellCoordinate Coords = targetCell.getCellCoordinates();
			Vector3f Center = new Vector3f(Coords.X * CubeCoordinate.CELLEDGESIZE, Coords.Y * CubeCoordinate.CELLEDGESIZE, Coords.Z);
			CellBox.setCenter(Center);
			if (this.CameraState.contains(CellBox)) {
				if (targetCell.isTerrainRenderingDirty()) {
					TerrainBuilder Builder = new TerrainBuilder(app, targetCell, builder, this.LevelofDetail);

					Builder.setNodes(Renderer.getCellNodeLight(Coords), Renderer.getCellNodeDark(Coords));
					Executor.submit(Builder);

					targetCell.setDirtyTerrainRendering(false);
				}
			} else { // Remove from the Scene graph
				targetCell.setDirtyTerrainRendering(true);
				for (int i = 0; i < CubeCoordinate.CELLDETAILLEVELS; i ++) {
					// remove mesh from scenegraph
					Renderer.getCellNodeLight(Coords).detachChildNamed("LightGeometry Cell " + targetCell.toString() + "DetailLevel " + i);
					Renderer.getCellNodeDark(Coords).detachChildNamed("DarkGeometry Cell " + targetCell.toString() + "DetailLevel " + i);
				}
			}
		}
	}

	public boolean getTerrainRendering() {
		return TerrainRendering;
	}

	/**
	 * @param TerrainRendering the TerrainRendering to set
	 */
	public void setTerrainRendering(boolean NewValue) {
		this.TerrainRendering = NewValue;
		Spatial.CullHint hint = Spatial.CullHint.Always;

		if (getTerrainRendering())
			hint = Spatial.CullHint.Dynamic;

		GameMap map = this.game.getMap();
		for (Cell target : map.getCellCollection()) {
			CellCoordinate Coords = target.getCellCoordinates();

			MapRenderer Renderer = state.getState(MapRenderer.class);
			Node CellLight = Renderer.getCellNodeLight(Coords);
			Node CellDark = Renderer.getCellNodeDark(Coords);

			Spatial light = CellLight.getChild("LightGeometry Cell " + target.toString() + "DetailLevel " + this.LevelofDetail);
			Spatial dark = CellDark.getChild("DarkGeometry Cell " + target.toString() + "DetailLevel " + this.LevelofDetail);

			if (light != null)
				light.setCullHint(hint);
			if (dark != null)
				dark.setCullHint(hint);
		}
	}

	public void setLevelofDetail(float ZoomLevel) {
		int NewDetailLevel = 0;
		
		if (ZoomLevel > 40)
			NewDetailLevel = 1;
		if (ZoomLevel > 80)
			NewDetailLevel = 2;
		if (ZoomLevel > 160)
			NewDetailLevel = 3;
		if (ZoomLevel > 320)
			NewDetailLevel = 4;
	
		if (NewDetailLevel != this.LevelofDetail) {
			this.LevelofDetail = NewDetailLevel;

			GameMap map = this.game.getMap();
			for (Cell target : map.getCellCollection()) {
				setCellDetailLevel(target, this.LevelofDetail);
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
		for (Cell target : map.getCellCollection()) {
			setCellDetailLevel(target, this.LevelofDetail);
		}
	}

	private void setCellDetailLevel(Cell TargetCell, int LevelofDetail) {
		CellCoordinate Coords = TargetCell.getCellCoordinates();

		MapRenderer Renderer = state.getState(MapRenderer.class);
		Node CellLight = Renderer.getCellNodeLight(Coords);
		Node CellDark = Renderer.getCellNodeDark(Coords);

		for (int i = 0; i < CubeCoordinate.CELLDETAILLEVELS; i++) {
			Spatial.CullHint hint = Spatial.CullHint.Always;
			if (i == LevelofDetail)
				hint = Spatial.CullHint.Dynamic;

			Spatial light = CellLight.getChild("LightGeometry Cell " + TargetCell.toString() + "DetailLevel " + i);
			Spatial dark = CellDark.getChild("DarkGeometry Cell " + TargetCell.toString() + "DetailLevel " + i);

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
			if (TerrainRendering)
				rebuildDirtyCells(map.getCellCollection());
		}
	}
}