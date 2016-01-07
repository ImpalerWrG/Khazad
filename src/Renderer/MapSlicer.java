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

import Map.Coordinates.BlockCoordinate;
import com.jme3.app.Application;
import com.jme3.scene.Node;
import com.jme3.scene.Spatial;
import gnu.trove.map.TIntObjectMap;
import java.util.concurrent.Callable;

/**
 *
 * @author Impaler
 */
public class MapSlicer implements Callable<Void>{

	Application app;
	MapRenderer renderer;
		
	int Top, Bottom;
	boolean DirtySlice;
	
	public MapSlicer(Application Parentapp, MapRenderer Renderer) {
		this.app = Parentapp;
		this.renderer = Renderer;
	}
	
	public Void call() {
		
		app.enqueue(new Callable() {
			public Object call() throws Exception {
				Top = renderer.Top;
				Bottom = renderer.Bottom;

				TIntObjectMap<Node> ZMapLight = renderer.ZMapLight;
				TIntObjectMap<Node> ZMapDark =  renderer.ZMapDark;

				for (Node targetnode : ZMapLight.valueCollection()) {
					float Z = targetnode.getLocalTranslation().getZ();
					float ChunkBottom = (Z * BlockCoordinate.CHUNK_EDGE_SIZE);
					float ChunkTop = ChunkBottom + BlockCoordinate.CHUNK_EDGE_SIZE;
					if (ChunkBottom <= Top && Bottom <= ChunkTop && renderer.SunnyRendering) {
						targetnode.setCullHint(Spatial.CullHint.Inherit);
					} else {
						targetnode.setCullHint(Spatial.CullHint.Always);
					}
				}

				for (Node targetnode : ZMapDark.valueCollection()) {
					float Z = targetnode.getLocalTranslation().getZ();
					float ChunkBottom = (Z * BlockCoordinate.CHUNK_EDGE_SIZE);
					float ChunkTop = ChunkBottom + BlockCoordinate.CHUNK_EDGE_SIZE;
					if (ChunkBottom <= Top && Bottom <= ChunkTop) {
						targetnode.setCullHint(Spatial.CullHint.Inherit);
					} else {
						targetnode.setCullHint(Spatial.CullHint.Always);
					}
				}
				return null;
			}
		});
		
		return null;
	}
}
