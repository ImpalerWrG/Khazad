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

import Interface.GameCameraState;
import Interface.VolumeSelection;

import Map.Coordinates.MapCoordinate;
import Map.Coordinates.BlockCoordinate;
import Game.Game;
import Map.Coordinates.ChunkCoordinate;
import Map.Zone;

import com.jme3.app.Application;
import com.jme3.app.SimpleApplication;
import com.jme3.app.state.AbstractAppState;
import com.jme3.app.state.AppStateManager;
import com.jme3.asset.AssetManager;
import com.jme3.font.BitmapFont;
import com.jme3.font.BitmapText;
import com.jme3.material.Material;
import com.jme3.math.ColorRGBA;
import com.jme3.math.Vector3f;
import com.jme3.scene.Geometry;
import com.jme3.scene.Mesh;
import com.jme3.scene.VertexBuffer;
import com.jme3.scene.shape.Box;
import com.jme3.scene.Node;
import com.jme3.scene.Spatial.CullHint;
import com.jme3.util.BufferUtils;

import java.util.ArrayList;
import java.util.BitSet;
import java.util.HashMap;
import java.util.Map;

/**
 *
 * @author Impaler
 */
public class SelectionRenderer extends AbstractAppState {

	SimpleApplication app = null;
	AppStateManager state = null;
	AssetManager assetmanager = null;
	private Geometry CursorBox;
	private Geometry SelectionBox;
	private HashMap<Integer, Node> ZoneGeometries;
	private VolumeSelection Selection;
	BitmapText hudText;

	@Override
	public void initialize(AppStateManager stateManager, Application app) {
		super.initialize(stateManager, app);
		this.app = (SimpleApplication) app;
		this.state = stateManager;
		this.assetmanager = app.getAssetManager();

		ZoneGeometries = new HashMap<Integer, Node>();

		buildCursorBox();
		buildText();
	}

	@Override
	public void cleanup() {
		super.cleanup();
		this.app.getGuiNode().detachChild(hudText);
	}

	public void buildCursorBox() {

		Material CursorMaterial = new Material(app.getAssetManager(), "Common/MatDefs/Misc/Unshaded.j3md");
		CursorMaterial.setColor("Color", ColorRGBA.Blue);

		Mesh WireBoxMesh = new Mesh();
		WireBoxMesh.setMode(Mesh.Mode.Lines);
		WireBoxMesh.setLineWidth(3);

		Vector3f[] vertices = new Vector3f[8];
		vertices[0] = new Vector3f(BlockCoordinate.HALF_BLOCK, BlockCoordinate.HALF_BLOCK, BlockCoordinate.HALF_BLOCK);
		vertices[1] = new Vector3f(-BlockCoordinate.HALF_BLOCK, BlockCoordinate.HALF_BLOCK, BlockCoordinate.HALF_BLOCK);
		vertices[2] = new Vector3f(-BlockCoordinate.HALF_BLOCK, -BlockCoordinate.HALF_BLOCK, BlockCoordinate.HALF_BLOCK);
		vertices[3] = new Vector3f(BlockCoordinate.HALF_BLOCK, -BlockCoordinate.HALF_BLOCK, BlockCoordinate.HALF_BLOCK);

		vertices[4] = new Vector3f(BlockCoordinate.HALF_BLOCK, BlockCoordinate.HALF_BLOCK, -BlockCoordinate.HALF_BLOCK);
		vertices[5] = new Vector3f(-BlockCoordinate.HALF_BLOCK, BlockCoordinate.HALF_BLOCK, -BlockCoordinate.HALF_BLOCK);
		vertices[6] = new Vector3f(-BlockCoordinate.HALF_BLOCK, -BlockCoordinate.HALF_BLOCK, -BlockCoordinate.HALF_BLOCK);
		vertices[7] = new Vector3f(BlockCoordinate.HALF_BLOCK, -BlockCoordinate.HALF_BLOCK, -BlockCoordinate.HALF_BLOCK);

		int[] indexes = {0, 1, 1, 2, 2, 3, 3, 0, 0, 4, 1, 5, 2, 6, 3, 7, 4, 5, 5, 6, 6, 7, 7, 4};

		WireBoxMesh.setBuffer(VertexBuffer.Type.Position, 3, BufferUtils.createFloatBuffer(vertices));
		WireBoxMesh.setBuffer(VertexBuffer.Type.Index, 3, BufferUtils.createIntBuffer(indexes));
		WireBoxMesh.updateBound();

		CursorBox = new Geometry("Camera Mouse Box", WireBoxMesh);
		CursorBox.setMaterial(CursorMaterial);
		app.getRootNode().attachChild(CursorBox);
	}

	public void buildSelectionBox() {
		Material mark_mat = new Material(app.getAssetManager(), "Common/MatDefs/Misc/Unshaded.j3md");
		mark_mat.setColor("Color", ColorRGBA.Orange);

		Mesh WireBoxMesh = new Mesh();
		WireBoxMesh.setMode(Mesh.Mode.Lines);
		WireBoxMesh.setLineWidth(3);

		int maxX = Math.max(Selection.OriginLocation.getX(), Selection.TerminalLocation.getX());
		int maxY = Math.max(Selection.OriginLocation.getY(), Selection.TerminalLocation.getY());
		int maxZ = Math.max(Selection.OriginLocation.getZ(), Selection.TerminalLocation.getZ());

		int minX = Math.min(Selection.OriginLocation.getX(), Selection.TerminalLocation.getX());
		int minY = Math.min(Selection.OriginLocation.getY(), Selection.TerminalLocation.getY());
		int minZ = Math.min(Selection.OriginLocation.getZ(), Selection.TerminalLocation.getZ());

		Vector3f[] vertices = new Vector3f[8];
		vertices[0] = new Vector3f(maxX - minX + BlockCoordinate.HALF_BLOCK, maxY - minY + BlockCoordinate.HALF_BLOCK, maxZ - minZ + BlockCoordinate.HALF_BLOCK);
		vertices[1] = new Vector3f(-BlockCoordinate.HALF_BLOCK, maxY - minY + BlockCoordinate.HALF_BLOCK, maxZ - minZ + BlockCoordinate.HALF_BLOCK);
		vertices[2] = new Vector3f(-BlockCoordinate.HALF_BLOCK, -BlockCoordinate.HALF_BLOCK, maxZ - minZ + BlockCoordinate.HALF_BLOCK);
		vertices[3] = new Vector3f(maxX - minX + BlockCoordinate.HALF_BLOCK, -BlockCoordinate.HALF_BLOCK, maxZ - minZ + BlockCoordinate.HALF_BLOCK);

		vertices[4] = new Vector3f(maxX - minX + BlockCoordinate.HALF_BLOCK, maxY - minY + BlockCoordinate.HALF_BLOCK, -BlockCoordinate.HALF_BLOCK);
		vertices[5] = new Vector3f(-BlockCoordinate.HALF_BLOCK, maxY - minY + BlockCoordinate.HALF_BLOCK, -BlockCoordinate.HALF_BLOCK);
		vertices[6] = new Vector3f(-BlockCoordinate.HALF_BLOCK, -BlockCoordinate.HALF_BLOCK, -BlockCoordinate.HALF_BLOCK);
		vertices[7] = new Vector3f(maxX - minX + BlockCoordinate.HALF_BLOCK, -BlockCoordinate.HALF_BLOCK, -BlockCoordinate.HALF_BLOCK);

		int[] indexes = {0, 1, 1, 2, 2, 3, 3, 0, 0, 4, 1, 5, 2, 6, 3, 7, 4, 5, 5, 6, 6, 7, 7, 4};

		WireBoxMesh.setBuffer(VertexBuffer.Type.Position, 3, BufferUtils.createFloatBuffer(vertices));
		WireBoxMesh.setBuffer(VertexBuffer.Type.Index, 2, BufferUtils.createIntBuffer(indexes));
		WireBoxMesh.updateBound();

		SelectionBox = new Geometry("Camera Mouse Box", WireBoxMesh);
		SelectionBox.setMaterial(mark_mat);
	}

	public void buildText() {
		BitmapFont font = this.app.getAssetManager().loadFont("Interface/Fonts/Default.fnt");

		hudText = new BitmapText(font, false);
		hudText.setSize(font.getCharSet().getRenderedSize());      // font size
		hudText.setColor(ColorRGBA.Blue);                             // font color
		hudText.setText("You can write any string here");             // the text
		hudText.setLocalTranslation(300, hudText.getLineHeight(), 0); // position
		this.app.getGuiNode().attachChild(hudText);
	}

	public Node buildZone(Zone TargetZone) {
		Node ZoneNode = new Node("ZoneNode");

		HashMap<ChunkCoordinate, BitSet> ZoneMap = TargetZone.getZoneMap();
		for (Map.Entry<ChunkCoordinate, BitSet> entry : ZoneMap.entrySet()) {
			BitSet ChunkBitSet = entry.getValue();
			ChunkCoordinate Coords = entry.getKey();

			for (BlockCoordinate Index = new BlockCoordinate(); !Index.end(); Index.next()) {
				if (ChunkBitSet.get(Index.getBlockIndex())) {
					Box newBox = new Box(BlockCoordinate.HALF_BLOCK, BlockCoordinate.HALF_BLOCK, BlockCoordinate.HALF_BLOCK);
					MapCoordinate MapCoords = new MapCoordinate(Coords, Index);

					Geometry ZoneBlock = new Geometry("Box", newBox);
					ZoneBlock.setLocalTranslation(MapCoords.getVector());

					Material BrownMat = new Material(app.getAssetManager(), "Common/MatDefs/Misc/Unshaded.j3md");
					BrownMat.setColor("Color", ColorRGBA.Brown);
					ZoneBlock.setMaterial(BrownMat);

					ZoneNode.attachChild(ZoneBlock);
				}
			}
		}

		return ZoneNode;
	}

	@Override
	public void update(float tpf) {
		GameCameraState cam = state.getState(GameCameraState.class);
		if (cam == null) {
			// not initialised yet
			return;
		}
		MapCoordinate mouse = cam.getMouseLocation();
		if (cam.getSelectedActor() == null) {
			CursorBox.setLocalTranslation(mouse.getVector());
			CursorBox.setCullHint(CullHint.Dynamic);
		} else {
			// hide the cursor if over an actor
			CursorBox.setCullHint(CullHint.Always);
		}

		Selection = cam.Volume;
		if (Selection != null) {
			if (Selection.Dirty) {
				if (SelectionBox != null)
					app.getRootNode().detachChild(SelectionBox);

				buildSelectionBox();

				int minX = Math.min(Selection.OriginLocation.getX(), Selection.TerminalLocation.getX());
				int minY = Math.min(Selection.OriginLocation.getY(), Selection.TerminalLocation.getY());
				int minZ = Math.min(Selection.OriginLocation.getZ(), Selection.TerminalLocation.getZ());

				SelectionBox.setLocalTranslation(new Vector3f(minX, minY, minZ));
				app.getRootNode().attachChild(SelectionBox);
				Selection.Dirty = false;
			}
		}
		//cam.SelectionOrigin
		//cam.SelectionTerminus;

		Game game = state.getState(Game.class);
		ArrayList<Zone> zones = game.getMap().getZones();
		if (zones != null) {
			for (Zone targetZone : zones) {
				if (targetZone.Dirty) {
					int ID = targetZone.getID();
					Node ZoneNode = ZoneGeometries.get(ID);

					if (ZoneNode != null)
						app.getRootNode().detachChild(ZoneNode);

					//ZoneNode = BuildZone(targetZone);
					//app.getRootNode().attachChild(ZoneNode);					
					//ZoneGeometries.put(ID, ZoneNode);
					//targetZone.Dirty = false;
				}
			}
		}

		hudText.setText("X: " + mouse.getX() + "  Y: " + mouse.getY() + "  Z: " + mouse.getZ());
	}
}
