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

import Data.DataManager;
import Data.DataLibrary;
import Data.Types.MaterialData;
import Data.Types.TextureData;

import com.jme3.asset.AssetManager;
import com.jme3.asset.AssetNotFoundException;
import com.jme3.math.ColorRGBA;

import java.util.concurrent.ConcurrentHashMap;

import com.jme3.material.Material;
import com.jme3.texture.Texture;
import com.jme3.texture.Texture2D;
import com.jme3.texture.Image;
import com.jme3.util.BufferUtils;
import java.nio.ByteBuffer;


/**
 *
 * @author Impaler
 */

public class TextureManager {

	Image TerrainImage;
	Texture TerrainTexture;
	Material TerrainMaterial;

	ConcurrentHashMap<Integer, float[]> CoordinateMap;

	int AtlastWidth, AtlasHeight = 512;

	private static TextureManager instance = null;

	class TextureCoordinates {
		float[] Coordinates = {0.0f, 0.0f, 0.0f, 0.0f};
	}

	protected TextureManager() {
		CoordinateMap = new ConcurrentHashMap<Integer, float[]>();
	}

	public static TextureManager getTextureManager() {
		if(instance == null) {
			instance = new TextureManager();
		}
		return instance;
	}

	public void Initialize(AssetManager manager) {
		TerrainMaterial = new Material(manager,"Common/MatDefs/Light/Lighting.j3md");

		int Bytes = Image.Format.RGBA8.getBitsPerPixel() / 8;
		byte[] data = new byte[AtlastWidth * AtlasHeight * Bytes];
		ByteBuffer Buffer = BufferUtils.createByteBuffer(data);
		Image NewImage = new Image(Image.Format.RGBA8, AtlastWidth, AtlasHeight, Buffer);

		TerrainTexture = new Texture2D(NewImage);
		TerrainTexture.setMagFilter(Texture.MagFilter.Nearest);
		
		TerrainMaterial.setTexture("DiffuseMap", TerrainTexture);
	}

	float[] getTextureCoordinates(short MaterialID, short SurfaceTypeID) {
		int Key = MaterialID;
		Key = Key << 16;
		Key += SurfaceTypeID;

		float[] Target = CoordinateMap.get(Key);
		if (Target != null) {
			return Target;
		} else {
			ImageManager Imaging = ImageManager.getImageManager();
			Image NewImage = Imaging.getMaterialImage(MaterialID, SurfaceTypeID);
			// Find spot on Atlas insert Image
			int Index = CoordinateMap.size();
			
			
			CoordinateMap.put(Key, Target);
			return Target;
		}
	}
}

