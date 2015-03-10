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

import com.jme3.asset.AssetManager;

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

	ConcurrentHashMap<Integer, TextureCoordinates> CoordinateMap;
	boolean[][] AtlasOccupiedMatrix;

	int AtlasWidth, AtlasHeight, MinimumUnitSize;

	private static TextureManager instance = null;

	class TextureCoordinates {
		float Top, Bottom, Left, Right;
	}

	protected TextureManager() {
		AtlasWidth = 512;
		AtlasHeight = 512;
		MinimumUnitSize = 16;

		CoordinateMap = new ConcurrentHashMap<Integer, TextureCoordinates>();
		AtlasOccupiedMatrix = new boolean[AtlasWidth / MinimumUnitSize][AtlasHeight / MinimumUnitSize];
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
		byte[] data = new byte[AtlasWidth * AtlasHeight * Bytes];
		ByteBuffer Buffer = BufferUtils.createByteBuffer(data);
		TerrainImage = new Image(Image.Format.RGBA8, AtlasWidth, AtlasHeight, Buffer);

		TerrainTexture = new Texture2D(TerrainImage);
		TerrainTexture.setMagFilter(Texture.MagFilter.Nearest);

		TerrainMaterial.setTexture("DiffuseMap", TerrainTexture);
	}

	TextureCoordinates getTextureCoordinates(short MaterialID, short SurfaceTypeID) {
		int Key = MaterialID;
		Key = Key << 16;
		Key += SurfaceTypeID;

		TextureCoordinates Target = CoordinateMap.get(Key);
		if (Target != null) {
			return Target;
		} else {
			ImageManager Imaging = ImageManager.getImageManager();
			Image NewImage = Imaging.getMaterialImage(MaterialID, SurfaceTypeID);
			TextureCoordinates NewCoords = insertImage(NewImage);

			CoordinateMap.put(Key, NewCoords);
			return NewCoords;
		}
	}
	
	TextureCoordinates insertImage(Image NewImage) {
		int ImageWidth = NewImage.getWidth() / MinimumUnitSize;
		int ImageHeight = NewImage.getHeight() / MinimumUnitSize;
		boolean Reject;

		for (int x = 0; x < AtlasOccupiedMatrix.length; x++) {
			for (int y = 0; y < AtlasOccupiedMatrix[0].length; y++) {
				Reject = false;
				if (!AtlasOccupiedMatrix[x][y]) { // Not occupied
					for (int w = 0; w < ImageWidth && !Reject; w++) {
						for (int h = 0; h < ImageHeight && !Reject; h++) {
							if (AtlasOccupiedMatrix[x + w][y + h])
								Reject = true;  // Space occupied, reject candidate
						}
					}
					// Candidate Accepted
					TextureCoordinates NewCoords = new TextureCoordinates();
					NewCoords.Top = (y * MinimumUnitSize) / (float) AtlasHeight;
					NewCoords.Bottom = ((y + ImageHeight) * MinimumUnitSize) / (float) AtlasHeight;
					NewCoords.Right = ((x + ImageWidth) * MinimumUnitSize) / (float) AtlasWidth;
					NewCoords.Left = (x * MinimumUnitSize) / (float) AtlasWidth;

					// Copy pixels
					ImageManager Imaging = ImageManager.getImageManager();
					Imaging.PasteImage(NewImage, TerrainImage, x * MinimumUnitSize, y * MinimumUnitSize);

					// Mark as Occupied
					for (int w = 0; w < ImageWidth; w++) {
						for (int h = 0; h < ImageHeight; h++) {
							AtlasOccupiedMatrix[x + w][y + h] = true;
						}
					}

					return NewCoords;
				}
			}
		}
		return null;  // No valid location
	}
}
