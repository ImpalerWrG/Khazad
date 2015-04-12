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

import Renderer.TextureManager.TextureAtlasCoordinates;

import Map.FaceCoordinate;

import com.jme3.math.Vector2f;
import com.jme3.math.Vector3f;

import com.jme3.scene.Mesh;
import com.jme3.scene.VertexBuffer;

import static com.jme3.util.BufferUtils.createFloatBuffer;
import static com.jme3.util.BufferUtils.createIntBuffer;

import java.nio.FloatBuffer;
import java.nio.IntBuffer;
import java.util.ArrayList;

/**
 * Flexible class to organize data for a Manualy created Mesh, primarilly serves
 * terrain mesh generation
 * 
 * @author Impaler
 */
public class RawMeshData {

	ArrayList<Vector3f> Vertices;
	ArrayList<Vector3f> Normals;
	ArrayList<Vector2f> TextureCoords;
	ArrayList<Integer> Indexes;

	public RawMeshData() {
		this.Vertices = new ArrayList<Vector3f>();
		this.Normals = new ArrayList<Vector3f>();
		this.TextureCoords = new ArrayList<Vector2f>();
		this.Indexes = new ArrayList<Integer>();
	}

	public RawMeshData(ArrayList<Vector3f> Verts, ArrayList<Vector3f> Norms, ArrayList<Vector2f> Textures, ArrayList<Integer> Indexs) {
		this.Vertices = Verts;
		this.Normals = Norms;
		this.TextureCoords = Textures;
		this.Indexes = Indexs;
	}

	public RawMeshData(RawMeshData ArgumentData) {
		this.Vertices = new ArrayList<Vector3f>();
		this.Normals = new ArrayList<Vector3f>();
		this.TextureCoords = new ArrayList<Vector2f>();
		this.Indexes = new ArrayList<Integer>();

		if (ArgumentData != null) {
			for (Vector3f Vert : ArgumentData.Vertices) {
				this.Vertices.add(new Vector3f(Vert));
			}
			for (Vector3f Norm : ArgumentData.Normals) {
				this.Normals.add(new Vector3f(Norm));
			}
			for (Vector2f Tex : ArgumentData.TextureCoords) {
				this.TextureCoords.add(new Vector2f(Tex));
			}
			for (Integer Int : ArgumentData.Indexes) {
				this.Indexes.add(new Integer(Int));
			}
		}
	}

	public void convertTextureCoordinates(TextureAtlasCoordinates AtlasCoords) {
		for (Vector2f Vert : this.TextureCoords) {
			Vert.x = AtlasCoords.Left + ((AtlasCoords.Right - AtlasCoords.Left) * Vert.x);
			Vert.y = AtlasCoords.Top + ((AtlasCoords.Bottom - AtlasCoords.Top) * Vert.y);
		}
	}

	public void translateVertexs(FaceCoordinate Coords) {
		for (Vector3f Vert : this.Vertices) {
			Vert.x += Coords.getX();
			Vert.y += Coords.getY();
		}
	}

	public Mesh createMesh() {
		Mesh ManualObject = new Mesh();

		FloatBuffer Vertbuff = createFloatBuffer(3 * this.Vertices.size());
		for (Vector3f element : this.Vertices) {
			Vertbuff.put(element.x);
			Vertbuff.put(element.y);
			Vertbuff.put(element.z);
		}
		Vertbuff.flip();

		FloatBuffer Normbuff = createFloatBuffer(3 * this.Normals.size());
		for (Vector3f element : this.Normals) {
			Normbuff.put(element.x);
			Normbuff.put(element.y);
			Normbuff.put(element.z);
		}
		Normbuff.flip();


		FloatBuffer Texbuff = createFloatBuffer(2 * this.TextureCoords.size());
		for (Vector2f element : this.TextureCoords) {
			Texbuff.put(element.x);
			Texbuff.put(element.y);
		}
		Texbuff.flip();

		IntBuffer Indxbuff = createIntBuffer(this.Indexes.size());
		Indxbuff.clear();
		for (Integer element : this.Indexes) {
			Indxbuff.put(element.intValue());
		}
		Indxbuff.flip();

		ManualObject.setBuffer(VertexBuffer.Type.Position, 3, Vertbuff);
		ManualObject.setBuffer(VertexBuffer.Type.Normal, 3, Normbuff);
		ManualObject.setBuffer(VertexBuffer.Type.TexCoord, 2, Texbuff);
		ManualObject.setBuffer(VertexBuffer.Type.Index, 3, Indxbuff);

		ManualObject.updateBound();
		return ManualObject;
	}
}
