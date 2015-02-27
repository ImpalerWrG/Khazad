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

		if (Tex == null)
		{
			//sprintf(buffer, "Failed to load Image file: %s", filepath);
			//Ogre::LogManager::getSingleton().getLog("Khazad.log")->logMessage(buffer);

			 return null;
		} else {
			//sprintf(buffer, "Loading Image file: %s", filepath);
			//Ogre::LogManager::getSingleton().getLog("Khazad.log")->logMessage(buffer);
		}		

		//ilConvertImage(IL_BGRA, IL_UNSIGNED_BYTE);

		if(ColorKey) {
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

	ILuint ImageManager::GenerateMaterialImage(int16_t MaterialID, int16_t TextureID)
	{
		MaterialData* Material = DATA->getMaterialData(MaterialID);
		TextureData* Texture = DATA->getTextureData(TextureID);

		ILuint TextureDevILID = Texture->getDevILID();

		int16_t PrimaryColorID = Material->getPrimaryColorID();
		int16_t SecondaryColorID = Material->getSecondaryColorID();
		int16_t BorderColorID = Material->getBorderColorID();

		string colormode = DATA->getMaterialData(MaterialID)->getColorMode();

		if(colormode == "gradientmap")
		{
			return GenerateGradientImage(TextureDevILID, PrimaryColorID, SecondaryColorID, BorderColorID);
		}
		else if(colormode.empty() || colormode == "overlay")
		{
			return GeneratedOverLayImage(TextureDevILID, PrimaryColorID, BorderColorID);
		}
		else if(colormode == "keepimage")
		{
			return GenerateKeeperImage(TextureDevILID, BorderColorID);
		}
	}
	
	Image GenerateGradientImage(Image Original, int PrimaryColorID, int SecondaryColorID, int BorderColorID)
	{
		ILuint TextureImageID;
		ilGenImages(1, &TextureImageID);
		ilBindImage(TextureImageID);
		ilCopyImage(TextureDevILID);
		ilConvertImage(IL_LUMINANCE, IL_UNSIGNED_BYTE);  //Load as IL_LUMINANCE to avoid convertion?

		uint8_t* TextureImageData = ilGetData();
		uint32_t width = ilGetInteger(IL_IMAGE_WIDTH);
		uint32_t height = ilGetInteger(IL_IMAGE_HEIGHT);

		ILuint MaskImageID;
		ilGenImages(1, &MaskImageID);
		ilBindImage(MaskImageID);
		ilTexImage(width, height, 1, 4, IL_BGRA, IL_UNSIGNED_BYTE, NULL);
		uint8_t* MaskImageData = ilGetData();

		ColorData* PrimaryColor = DATA.getColorData(PrimaryColorID);
		ColorData* SecondaryColor = DATA.getColorData(SecondaryColorID);

		uint32_t bpp = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
		if(SecondaryColor != NULL)
		{
			for(uint32_t i = 0; i < width; i++)
			{
				for(uint32_t j = 0; j < height; j++)
				{
					MaskImageData[(i * width * bpp) + (j * bpp) + 0] = SecondaryColor->getBlue();     // Blue
					MaskImageData[(i * width * bpp) + (j * bpp) + 1] = SecondaryColor->getGreen();    // Green
					MaskImageData[(i * width * bpp) + (j * bpp) + 2] = SecondaryColor->getRed();      // Red
					MaskImageData[(i * width * bpp) + (j * bpp) + 3] = 255 - TextureImageData[(i * width) + j]; // Alpha
				}
			}
		}

		ILuint NewImageID;
		ilGenImages(1, &NewImageID);
		ilBindImage(NewImageID);
		ilTexImage(width, height, 1, 4, IL_BGRA, IL_UNSIGNED_BYTE, NULL);
		uint8_t* NewImageData = ilGetData();

		if(PrimaryColor != NULL)
		{
			for(uint32_t i = 0; i < width; i++)
			{
				for(uint32_t j = 0; j < height; j++)
				{
					NewImageData[(i * width * bpp) + (j * bpp) + 0] = PrimaryColor->getBlue(); // Blue
					NewImageData[(i * width * bpp) + (j * bpp) + 1] = PrimaryColor->getGreen(); // Green
					NewImageData[(i * width * bpp) + (j * bpp) + 2] = PrimaryColor->getRed(); // Red
					NewImageData[(i * width * bpp) + (j * bpp) + 3] = 255; // Alpha
				}
			}
		}

		ilOverlayImage(MaskImageID, 0, 0, 0);

		if (BorderColorID != DataTypes.INVALID_INDEX)
		{
			ApplyBorder(NewImageID, BorderColorID);
		}

		return NewImageID;
	}*/
 
	public Image GeneratedOverLayImage(Image Original, int PrimaryColorID, int BorderColorID) {
		Image newImage = Original.clone();
		ImageRaster sourceReader = ImageRaster.create(Original);
        ImageRaster targetWriter = ImageRaster.create(newImage);

		int width = Original.getWidth();
		int height = Original.getHeight();

		//ColorData PrimaryColor = DATA.getColorData(PrimaryColorID);
		ColorRGBA PrimaryColor = ColorRGBA.Green;

		if(PrimaryColor != null)
		{
			for(int i = 0; i < width; i++)
			{
				for(int j = 0; j < height; j++)
				{
					ColorRGBA sourceColor = sourceReader.getPixel(i, j);
					ColorRGBA newColor = new ColorRGBA();
					
					float Base = sourceColor.getRed() + sourceColor.getBlue() + sourceColor.getGreen();
					float Alpha = sourceColor.a;
					Base /= 3;

					float OriginalBlue = PrimaryColor.getBlue();
					OriginalBlue /= 255.0;
					
					float OriginalGreen = PrimaryColor.getGreen();
					OriginalGreen /= 255.0;

					float OriginalRed = PrimaryColor.getRed();
					OriginalRed /= 255.0;

					// coloring using overlay mode
					if(Base >= 0.5)
					{
						newColor.b = (float) (1.0 - 2.0 * (1.0 - OriginalBlue) * (1.0 - Base)) * 255;
						newColor.g = (float) (1.0 - 2.0 * (1.0 - OriginalGreen) * (1.0 - Base)) * 255;
						newColor.r = (float) (1.0 - 2.0 * (1.0 - OriginalRed) * (1.0 - Base)) * 255;
						newColor.a = Alpha;

						targetWriter.setPixel(i, j, newColor);
					}
					else
					{
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

	public Image GenerateKeeperImage(Image Original, int BorderColorID) {
		Image NewImage = Original.clone();
		if (BorderColorID != DataManager.INVALID_INDEX) {
			ApplyBorder(NewImage, BorderColorID);
		}
		return NewImage;
	}

	private void ApplyBorder(Image Original, int BorderColorID) {
        int width = Original.getWidth();
        int height = Original.getHeight();

        ImageRaster targetWriter = ImageRaster.create(Original);

		byte Red, Green, Blue;
		//ColorData BorderColor = DATA.getColorData(BorderColorID);
		ColorRGBA color = ColorRGBA.Black;
		//Red = BorderColor->getRed();
		//Green = BorderColor->getGreen();
		//Blue = BorderColor->getBlue();

		if(color != null)
		{
			if(targetWriter != null)
			{
				for(int i = 0; i < width; i++)
				{
					targetWriter.setPixel(i, 0, color);
					targetWriter.setPixel(i, height - 1, color);
				}

				for(int j = 0; j < height; j++)
				{
					targetWriter.setPixel(width -1, j, color);
					targetWriter.setPixel(0, j, color);
				}
			}
		}
	}
}
