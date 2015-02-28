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
import Data.Types.ColorData;
import Data.Types.MaterialData;
import Data.Types.TextureData;

import com.jme3.math.ColorRGBA;
import com.jme3.texture.Texture;
import com.jme3.texture.Image;
import com.jme3.texture.Image.Format;
import com.jme3.texture.image.ImageRaster;
import com.jme3.util.BufferUtils;
import java.nio.ByteBuffer;
import com.jme3.asset.AssetManager;

/**
 *
 * @author Impaler
 */
public class ImageManager {
	
	int ImageCounter = 0;
	private static ImageManager instance = null;
	AssetManager assetmanager = null;

	
	protected ImageManager() {

	}

	public static ImageManager getImageManager() {
		if(instance == null) {
			instance = new ImageManager();
		}
		return instance;
	}

	public void Initialize(AssetManager manager) {
		this.assetmanager = manager;
	}

	private Image convertImage(Image image, Format newFormat) {

		int width = image.getWidth();
		int height = image.getHeight();
		ByteBuffer data = BufferUtils.createByteBuffer( (int)Math.ceil(newFormat.getBitsPerPixel() / 8.0) * width * height);
		Image convertedImage = new Image(newFormat, width, height, data);

		ImageRaster sourceReader = ImageRaster.create(image);
		ImageRaster targetWriter = ImageRaster.create(convertedImage);
		for (int x = 0; x < width; x++) {
			for (int y = 0; y < height; y++) {
				ColorRGBA color = sourceReader.getPixel(x, y);
				targetWriter.setPixel(x, y, color);
			}
		}
		
		return convertedImage;
	}

	/*
	boolean Initialize() {

		int DefaultIndex = DATA->getLabelIndex("TEXTURE_DEFAULT");
		ILuint DefaultID = loadImage(DATA->getTextureData(DefaultIndex)->getPath());

		for(uint32_t i = 0; i < DATA->getNumTextures(); ++i)
		{
			if (DATA->getTextureData(i)->isLoneTexture())
			{
				ILuint DevilID = loadImage(DATA->getTextureData(i)->getPath(), false);
				if (DevilID != -1)
				{
					DATA->getTextureData(i)->setDevILID(DevilID);
				} else {
					DATA->getTextureData(i)->setDevILID(DefaultID);
				}
			}
		}

		for(uint32_t i = 0; i < DATA->getNumTextureGrids(); ++i)
		{
			TextureGridData* Grid = DATA->getTextureGridData(i);
			ILuint DevilID = loadImage(Grid->getPath(), false);

			uint16_t w = Grid->getTextureWidth();
			uint16_t h = Grid->getTextureHeight();

			std::vector<TextureData*> Textures = DATA->getTextureGridData(i)->TextureList;
			for (int j = 0; j < Textures.size(); j++)
			{
				if (DevilID != -1)
				{
					ILuint NewID = ClipImage(DevilID, Textures[j]->getX(), Textures[j]->getY(), w, h);
					Textures[j]->setDevILID(NewID);
				} else {
					Textures[j]->setDevILID(DefaultID);
				}
			}
		}


		for(uint32_t i = 0; i < DATA->getNumTextureSheets(); ++i)
		{
			TextureSheetData* Sheet = DATA->getTextureSheetData(i);
			ILuint DevilID = loadImage(Sheet->getPath(), false);

			std::vector<TextureData*> Textures = DATA->getTextureSheetData(i)->TextureList;
			for (int j = 0; j < Textures.size(); j++)
			{
				if (DevilID != -1)
				{
					ILuint NewID = ClipImage(DevilID, Textures[j]->getX(), Textures[j]->getY(), Textures[j]->getW(), Textures[j]->getH());
					Textures[j]->setDevILID(NewID);
				} else {
					Textures[j]->setDevILID(DefaultID);
				}
			}
		}

		return true;
	}
	*/
	
	Image loadImage(String filepath, boolean ColorKey) {
		Texture Tex = assetmanager.loadTexture(filepath);

		if (Tex == null) {
			//sprintf(buffer, "Failed to load Image file: %s", filepath);
			//Ogre::LogManager::getSingleton().getLog("Khazad.log")->logMessage(buffer);

			 return null;
		} else {
			//sprintf(buffer, "Loading Image file: %s", filepath);
			//Ogre::LogManager::getSingleton().getLog("Khazad.log")->logMessage(buffer);
		}

		//ilConvertImage(IL_BGRA, IL_UNSIGNED_BYTE);

		if (ColorKey) {
			//convert color key
		}

		int ImageID = ImageCounter;
		ImageCounter++;
		return Tex.getImage();
	}
	/*

	ILuint ImageManager::ClipImage(ILuint SourceID, ILuint X, ILuint Y, ILuint W, ILuint H)
	{
		ILuint ImageID;
		ilGenImages(1, &ImageID);
		ilBindImage(ImageID);
		ilTexImage(W, H, 1, 4, IL_BGRA, IL_UNSIGNED_BYTE, NULL);

		uint8_t* NewImageData = ilGetData();
		ilBindImage(SourceID);
		ilCopyPixels(X, Y, 0, W, H, 1, IL_BGRA, IL_UNSIGNED_BYTE, NewImageData);

		return ImageID;
	}
*/
	Image GenerateMaterialImage(short MaterialID, short TextureID) {
		DataManager Data = DataManager.getDataManager();
		MaterialData Material = Data.getMaterialData(MaterialID);
		TextureData Texture = Data.getTextureData(TextureID);

		//ILuint TextureDevILID = Texture->getDevILID();
		Image TextureImage = new Image();

		short PrimaryColorID = Material.PrimaryColorID;
		short SecondaryColorID = Material.SecondaryColorID;
		short BorderColorID = Material.BorderColorID;

		String colormode = Material.ColorMode;

		if(colormode.equals("gradientmap")) {
			return GenerateGradientImage(TextureImage, PrimaryColorID, SecondaryColorID, BorderColorID);
		}
		else if(colormode.equals("keepimage")) {
			return GenerateKeeperImage(TextureImage, BorderColorID);
		}
		else if(colormode.isEmpty() || colormode.equals("overlay")) {
			return GeneratedOverLayImage(TextureImage, PrimaryColorID, BorderColorID);
		}
		return null;
	}

	Image GenerateGradientImage(Image Original, short PrimaryColorID, short SecondaryColorID, short BorderColorID) {
		Image newImage = Original.clone();
		ImageRaster sourceReader = ImageRaster.create(Original);
		ImageRaster targetWriter = ImageRaster.create(newImage);

		int width = Original.getWidth();
		int height = Original.getHeight();

		ColorData PrimaryColor = DataManager.getDataManager().getColorData(PrimaryColorID);
		ColorData SecondaryColor = DataManager.getDataManager().getColorData(SecondaryColorID);

		Image OverlayImage = Original.clone();
		if (SecondaryColor != null) {
			for(int i = 0; i < width; i++) {
				for(int j = 0; j < height; j++) {
					//MaskImageData[(i * width * bpp) + (j * bpp) + 0] = SecondaryColor->getBlue();     // Blue
					//MaskImageData[(i * width * bpp) + (j * bpp) + 1] = SecondaryColor->getGreen();    // Green
					//MaskImageData[(i * width * bpp) + (j * bpp) + 2] = SecondaryColor->getRed();      // Red
					//MaskImageData[(i * width * bpp) + (j * bpp) + 3] = 255 - TextureImageData[(i * width) + j]; // Alpha
				}
			}
		}

		//Image OverlayImage = Original.clone();
		if(PrimaryColor != null) {
			ColorRGBA newColor = new ColorRGBA();
			newColor.r = PrimaryColor.Red / 255;
			newColor.g = PrimaryColor.Green / 255;
			newColor.b = PrimaryColor.Blue / 255;
			newColor.a = 1;
			
			for(int i = 0; i < width; i++) {
				for(int j = 0; j < height; j++) {
					targetWriter.setPixel(i, j, newColor);
				}
			}
		}

		//ilOverlayImage(MaskImageID, 0, 0, 0);

		if (BorderColorID != DataManager.INVALID_INDEX) {
			ApplyBorder(newImage, BorderColorID);
		}

		return newImage;
	}
 
	public Image GeneratedOverLayImage(Image Original, short PrimaryColorID, short BorderColorID) {
		Image newImage = Original.clone();
		ImageRaster sourceReader = ImageRaster.create(Original);
		ImageRaster targetWriter = ImageRaster.create(newImage);

		int width = Original.getWidth();
		int height = Original.getHeight();

		ColorData PrimaryColor = DataManager.getDataManager().getColorData(PrimaryColorID);
		
		if (PrimaryColor != null) {
			for (int i = 0; i < width; i++) {
				for (int j = 0; j < height; j++) {
					ColorRGBA sourceColor = sourceReader.getPixel(i, j);
					ColorRGBA newColor = new ColorRGBA();
					
					float Base = sourceColor.getRed() + sourceColor.getBlue() + sourceColor.getGreen();
					float Alpha = sourceColor.a;
					Base /= 3;

					float OriginalBlue = PrimaryColor.Blue / 65025f;
					float OriginalGreen = PrimaryColor.Green / 65025f;
					float OriginalRed = PrimaryColor.Red / 65025f;

					
					// coloring using overlay mode
					if (Base >= 0.5) {
						newColor.b = (float) (1.0 - 2.0 * (1.0 - OriginalBlue) * (1.0 - Base)) * 255;
						newColor.g = (float) (1.0 - 2.0 * (1.0 - OriginalGreen) * (1.0 - Base)) * 255;
						newColor.r = (float) (1.0 - 2.0 * (1.0 - OriginalRed) * (1.0 - Base)) * 255;
						newColor.a = Alpha;

						targetWriter.setPixel(i, j, newColor);
					} else {
						newColor.b = (float) (2.0 * OriginalBlue * Base) * 255;
						newColor.g = (float) (2.0 * OriginalGreen * Base) * 255;
						newColor.r = (float) (2.0 * OriginalRed * Base) * 255;
						newColor.a = Alpha;
						
						targetWriter.setPixel(i, j, newColor);
					}
				}
			}
		}

		if (BorderColorID != DataManager.INVALID_INDEX) {
			ApplyBorder(newImage, BorderColorID);
		}

		return newImage;
	}

	public Image GenerateKeeperImage(Image Original, short BorderColorID) {
		Image NewImage = Original.clone();
		if (BorderColorID != DataManager.INVALID_INDEX) {
			ApplyBorder(NewImage, BorderColorID);
		}
		return NewImage;
	}

	private void ApplyBorder(Image Original, short BorderColorID) {
		int width = Original.getWidth();
		int height = Original.getHeight();

		ImageRaster targetWriter = ImageRaster.create(Original);

		float Red, Green, Blue;
		ColorData BorderColor = DataManager.getDataManager().getColorData(BorderColorID);

		Red = BorderColor.Red / 255f;
		Green = BorderColor.Green / 255f;
		Blue = BorderColor.Blue / 255f;
		ColorRGBA color = new ColorRGBA(Red, Green, Blue, 1.0f);

		if(targetWriter != null) {
			for(int i = 0; i < width; i++) {
				targetWriter.setPixel(i, 0, color);
				targetWriter.setPixel(i, height - 1, color);
			}

			for(int j = 0; j < height; j++) {
				targetWriter.setPixel(width -1, j, color);
				targetWriter.setPixel(0, j, color);
			}
		}
	}
}
