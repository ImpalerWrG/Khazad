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

import Map.Coordinates.Axis;
import Map.Chunk;
import Map.Coordinates.ChunkCoordinate;
import Map.Coordinates.Direction;
import Map.GameMap;
import Map.Coordinates.BlockCoordinate;
import Map.Coordinates.MapCoordinate;
import Map.Coordinates.BlockCoordinate;
import Map.Sector;

import PathFinding.PathManager;
import PathFinding.MovementModality;

import com.jme3.app.Application;
import com.jme3.app.SimpleApplication;
import com.jme3.app.state.AbstractAppState;
import com.jme3.app.state.AppStateManager;
import com.jme3.asset.AssetManager;

import com.jme3.input.KeyInput;
import com.jme3.input.controls.ActionListener;
import com.jme3.input.controls.KeyTrigger;
import com.jme3.input.InputManager;

import com.jme3.material.Material;
import com.jme3.math.ColorRGBA;
import com.jme3.math.Vector3f;

import com.jme3.scene.Spatial;
import com.jme3.scene.Geometry;
import com.jme3.scene.Mesh;
import com.jme3.scene.Node;
import com.jme3.scene.VertexBuffer;
import com.jme3.util.BufferUtils;

import java.util.HashMap;
import java.util.BitSet;
import java.util.ArrayList;
import java.util.Collection;

import jme3tools.optimize.GeometryBatchFactory;

/**
 *
 * @author Impaler
 */
public class PathingRenderer extends AbstractAppState {

	SimpleApplication app = null;
	AppStateManager state = null;
	AssetManager assetmanager = null;
	Game game = null;
	PathManager Pathing;
	Node PathingNode;
	HashMap<ChunkCoordinate, Node> chunks;
	HashMap<Integer, Material> ZoneMaterials;
	private boolean DisplayToggle = false;
	Vector3f[] vertices;

	@Override
	public void initialize(AppStateManager stateManager, Application app) {
		super.initialize(stateManager, app);
		this.app = (SimpleApplication) app;
		this.state = stateManager;
		this.assetmanager = app.getAssetManager();

		this.vertices = new Vector3f[Direction.ANGULAR_DIRECTIONS.length];

		for (Direction dir : Direction.ANGULAR_DIRECTIONS) {
			vertices[dir.ordinal()] = new Vector3f(dir.getValueonAxis(Axis.AXIS_X) * BlockCoordinate.HALF_BLOCK, dir.getValueonAxis(Axis.AXIS_Y) * BlockCoordinate.HALF_BLOCK, dir.getValueonAxis(Axis.AXIS_Z) * BlockCoordinate.HALF_BLOCK);
		}
	}

	public void attachToGame(Game TargetGame) {
		this.game = TargetGame;

		PathingNode = new Node("PathingNode");
		this.app.getRootNode().attachChild(PathingNode);

		this.Pathing = PathManager.getSingleton();
		ZoneMaterials = new HashMap<Integer, Material>();
	}

	public Node buildRendering(Chunk TargetChunk) {

		Node PathRenderingNode = new Node("PathRenderingNode");
		MovementModality Mod = new MovementModality(MovementModality.MovementType.MOVEMENT_TYPE_WALK, 1, 1);

		ChunkCoordinate ChunkCoords = TargetChunk.getChunkCoordinates();

		for (BlockCoordinate Index = new BlockCoordinate(); !Index.end(); Index.next()) {
			MapCoordinate TargetCoords = new MapCoordinate(ChunkCoords, Index);
			BitSet Connectivity = Pathing.getDirectionFlags(TargetCoords, Mod);

			int Zone = Pathing.getConnectivityZone(TargetCoords, Mod);
			Material mat = ZoneMaterials.get(Zone);
			if (mat == null) {
				mat = new Material(app.getAssetManager(), "Common/MatDefs/Misc/Unshaded.j3md");
				mat.setColor("Color", ColorRGBA.randomColor());
				ZoneMaterials.put(Zone, mat);
			}

			if (Connectivity.cardinality() > 0) {
				Mesh EdgeWires = new Mesh();
				EdgeWires.setMode(Mesh.Mode.Lines);
				EdgeWires.setLineWidth(5);

				ArrayList<Integer> Indexes = new ArrayList<Integer>();
				for (Direction dir : Direction.ANGULAR_DIRECTIONS) {
					if (Pathing.getEdgeCost(TargetCoords, dir, Mod) != -1) {
						Indexes.add(0);
						Indexes.add(dir.ordinal());
					}
				}

				int[] indexes = new int[Indexes.size()];
				for (int i = 0; i < Indexes.size(); i++) {
					indexes[i] = Indexes.get(i).intValue();
				}

				EdgeWires.setBuffer(VertexBuffer.Type.Position, 3, BufferUtils.createFloatBuffer(vertices));
				EdgeWires.setBuffer(VertexBuffer.Type.Index, 2, BufferUtils.createIntBuffer(indexes));
				EdgeWires.updateBound();

				Geometry Wires = new Geometry("Connection Wires", EdgeWires);
				Wires.setMaterial(mat);
				Wires.setLocalTranslation(TargetCoords.Block.getX(), TargetCoords.Block.getY(), TargetCoords.Block.getZ());
				PathRenderingNode.attachChild(Wires);
			}
		}

		GeometryBatchFactory.optimize(PathRenderingNode, true);
		if (PathRenderingNode.getQuantity() > 0) {
			Spatial ConnectionRendering = PathRenderingNode.getChild(0);
			PathRenderingNode.setName("Connection Rendering" + ChunkCoords.toString());
			return PathRenderingNode;
		} else {
			return null;
		}
	}

	public void rebuildDirtyChunks(Collection<Chunk> chunks) {
		for (Chunk target : chunks) {
			ChunkCoordinate Coords = target.getChunkCoordinates();

			MapRenderer Renderer = state.getState(MapRenderer.class);
			Node ChunkNode = Renderer.getChunkNodeLight(Coords);
			Spatial ConnectivityNode = ChunkNode.getChild("Connection Rendering" + Coords.toString());

			if (target.isPathingRenderingDirty()) {
				Node NewConnectivity = buildRendering(target);
				if (ConnectivityNode != null)
					ChunkNode.detachChild(ConnectivityNode);
				if (NewConnectivity != null) {
					ChunkNode.attachChild(NewConnectivity);
				}
				target.setDirtyPathingRendering(false);
			}

			if (ConnectivityNode != null) {
				ConnectivityNode.setCullHint(Spatial.CullHint.Dynamic);
			}
		}
	}

	void hideConnectivityRendering(Collection<Chunk> chunks) {
		for (Chunk target : chunks) {
			ChunkCoordinate Coords = target.getChunkCoordinates();
			MapRenderer Renderer = state.getState(MapRenderer.class);
			Node ChunkNode = Renderer.getChunkNodeLight(Coords);
			Spatial ConnectivityNode = ChunkNode.getChild("Connection Rendering" + Coords.toString());

			if (ConnectivityNode != null) {
				ConnectivityNode.setCullHint(Spatial.CullHint.Always);
			}
		}
	}

	@Override
	public void update(float tpf) {
		if (this.game != null) {
			GameMap map = this.game.getMap();
			if (getDisplayToggle()) {
				for (Sector targetSector : map.getSectorCollection()) {
					rebuildDirtyChunks(targetSector.getChunkCollection());
				}
			}
		}
	}

	/**
	 * @return the DisplayToggle
	 */
	public boolean getDisplayToggle() {
		return DisplayToggle;
	}

	/**
	 * @param DisplayToggle the DisplayToggle to set
	 */
	public void setDisplayToggle(boolean DisplayToggle) {
		this.DisplayToggle = DisplayToggle;
		if (this.game != null) {
			GameMap map = this.game.getMap();
			if (DisplayToggle == false) {
				for (Sector targetSector : map.getSectorCollection()) {
					rebuildDirtyChunks(targetSector.getChunkCollection());
				}
			}
		}
	}
}
