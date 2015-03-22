/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package Renderer;

import Game.Game;
import Interface.GameCameraState;
import Map.CellCoordinate;
import Map.GameMap;

import Map.MapCoordinate;
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
import com.jme3.scene.control.LodControl;

import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.Semaphore;

/**
 *
 * @author Impaler
 */
public class MapRenderer extends AbstractAppState{

	SimpleApplication app = null;
	AppStateManager state = null;
	AssetManager assetmanager = null;
	ImageManager imagemanager = null;

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
	boolean SunnyRendering = true;
	int Top; int Bottom;

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
		this.MapNode = new Node();
		this.app.getRootNode().attachChild(MapNode);

		darkterrainNode = new Node();
		sunnyterrainNode = new Node();
		MapNode.attachChild(darkterrainNode);
		MapNode.attachChild(sunnyterrainNode);

		ColorRGBA Suncolor = ColorRGBA.White;
		AmbientLight Darkglow = new AmbientLight();
		Darkglow.setColor(Suncolor.mult(1.8f));
		darkterrainNode.addLight(Darkglow);

		AmbientLight Lightglow = new AmbientLight();
		Lightglow.setColor(Suncolor.mult(1.8f));
		sunnyterrainNode.addLight(Lightglow);

		TargetGame.getWeather().AttatchSun(sunnyterrainNode);
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

					float x = (float) (TargetCoordinates.X * MapCoordinate.CELLEDGESIZE);
					float y = (float) (TargetCoordinates.Y * MapCoordinate.CELLEDGESIZE);
					CellNode.move(x, y, 0);

					getZNodeLight(TargetCoordinates.Z).attachChild(CellNode);
					LightCellNodeMap.put(TargetCoordinates, CellNode);
				}
			}	finally {
				semaphore.release();
			}
		} catch (final InterruptedException e) {
			e.printStackTrace();
		}
		return CellNode;
	}

	public Node getCellNodeDark(CellCoordinate TargetCell) {
		Node CellNode = DarkCellNodeMap.get(TargetCell);
		if (CellNode == null) {
			CellNode = new Node();	

			float x = (float) (TargetCell.X * MapCoordinate.CELLEDGESIZE);
			float y = (float) (TargetCell.Y * MapCoordinate.CELLEDGESIZE);

			CellNode.move(x, y, 0);

			getZNodeDark(TargetCell.Z).attachChild(CellNode);
			DarkCellNodeMap.put(TargetCell, CellNode);
		}
		return CellNode;
	}

	public Node getZNodeLight(int zlevel) {
		Node targetnode = ZMapLight.get(new Integer(zlevel));
		if (targetnode == null) {

			targetnode = new Node();
			targetnode.move(0, 0, zlevel);
			ZMapLight.put(new Integer(zlevel), targetnode);
		}
		return targetnode;
	}

	public Node getZNodeDark(int zlevel) {
		Node targetnode = ZMapDark.get(new Integer(zlevel));
		if (targetnode == null) {

			targetnode = new Node();
			targetnode.move(0, 0, zlevel);
			ZMapDark.put(new Integer(zlevel), targetnode);
		}
		return targetnode;
	}

	public void setSliceLevels(int top, int bottom) {
		Top = top; Bottom = bottom;

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

	public void setSunnyVisibility(boolean newValue) {
		if (SunnyRendering != newValue) {
			SunnyRendering = newValue;

			for (Node target : ZMapLight.values()) {
				float Z = target.getLocalTranslation().getZ();
				if (Z > Bottom && Z <= Top && SunnyRendering) {
					target.setCullHint(Spatial.CullHint.Dynamic);
				} else {
					target.setCullHint(Spatial.CullHint.Always);				
				}
			}
		}
	}

	public Node getMapNode()			{ return MapNode; }

	public Node getSunTerrainNode()		{ return sunnyterrainNode; }

	public Node getDarkTerrainNode()	{ return darkterrainNode; }

	@Override
	public void update(float tpf) {	
		if (this.game != null) {
			GameCameraState cam = state.getState(GameCameraState.class);
			setSliceLevels(cam.getSliceTop(), cam.getSliceBottom());

			GUI gui = state.getState(GUI.class);
			String TimeString = ("DAY " + (game.days) + "  -  " + (game.hours %24) + ":" + (game.minutes % 60) + ":" + (game.seconds % 60));
			gui.UpdateText("Timelabel", TimeString);
		}
	}
}
