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

import Map.Cell;
import Map.CubeCoordinate;

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

	Cell BuildCell;
	Application app;
	Node CellLight, CellDark;
	int DetailLevel;

	public TerrainDestroyer(Application Parentapp, Cell TargetCell, int LevelofDetail) {
		this.app = Parentapp;
		this.BuildCell = TargetCell;
		this.DetailLevel = LevelofDetail;
	}

	public void setNodes(Node LightCellNode, Node DarkCellNode) {
		this.CellLight = LightCellNode;
		this.CellDark = DarkCellNode;
	}

	public Void call() {
			app.enqueue(new Callable() {
			public Object call() throws Exception {
				for (int i = 0; i < CubeCoordinate.CELLDETAILLEVELS; i ++) {
					CellLight.detachChildNamed("LightGeometry Cell " + BuildCell.toString() + "DetailLevel " + i);
					CellDark.detachChildNamed("DarkGeometry Cell " + BuildCell.toString() + "DetailLevel " + i);
				}
				return null;
			}
		});
		return null;
	}
}
