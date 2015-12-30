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

import Map.Chunk;
import Map.Coordinates.BlockCoordinate;

import com.jme3.app.Application;
import com.jme3.scene.Node;

import java.util.concurrent.Callable;

/**
 * Callable class that removes a target Mesh from
 * the Scene graph.
 *
 * @author Impaler
 */

public class TerrainDestroyer implements Callable<Void>{

	Chunk BuildChunk;
	Application app;
	Node ChunkLight, ChunkDark;
	int DetailLevel;

	public TerrainDestroyer(Application Parentapp, Chunk TargetChunk, int LevelofDetail) {
		this.app = Parentapp;
		this.BuildChunk = TargetChunk;
		this.DetailLevel = LevelofDetail;
	}

	public void setNodes(Node LightChunkNode, Node DarkChunkNode) {
		this.ChunkLight = LightChunkNode;
		this.ChunkDark = DarkChunkNode;
	}

	public Void call() {
			app.enqueue(new Callable() {
			public Object call() throws Exception {
				for (int i = 0; i < BlockCoordinate.CHUNK_DETAIL_LEVELS; i ++) {
					ChunkLight.detachChildNamed("LightGeometry Chunk " + BuildChunk.toString() + "DetailLevel " + i);
					ChunkDark.detachChildNamed("DarkGeometry Chunk " + BuildChunk.toString() + "DetailLevel " + i);
				}
				return null;
			}
		});
		return null;
	}
}
