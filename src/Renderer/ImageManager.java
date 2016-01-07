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

import Data.DataLibrary;
import Data.DataManager;
import Data.Types.ColorData;
import Data.Types.MaterialData;
import Data.Types.TextureData;
import Data.Types.TextureGridData;
import Data.Types.TextureSheetData;

import com.jme3.math.ColorRGBA;
import com.jme3.texture.Texture;
import com.jme3.texture.Image;
import com.jme3.texture.Image.Format;
import com.jme3.texture.image.ImageRaster;
import com.jme3.util.BufferUtils;
import java.nio.ByteBuffer;
import com.jme3.asset.AssetManager;
import com.jme3.asset.AssetNotFoundException;
import com.jme3.system.JmeSystem;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.concurrent.ConcurrentHashMap;

/**
 * Manager for the material composite Image creation process, add Texture,
 * TextureGrid and TextureSheet Data Entries are processed and disected to
 * individual Images, upon request composits are made using the colors and methods
 * specified under the Materials Data Entry.
 *
 * @author Impaler
 */
public class ImageManager {

	int ImageCounter = 0;
	private static ImageManager instance = null;
	AssetManager assetmanager = null;
	ConcurrentHashMap<Integer, Image> RawTextureGridMap;
	ConcurrentHashMap<Integer, Image> RawTextureSheetMap;
	ConcurrentHashMap<Integer, Image> RawTextureMap;
	ConcurrentHashMap<Integer, Image> CompletedImageMap;

	final static short DefaultTextureID = DataManager.getLabelIndex("TEXTURE_DEFAULT");

	protected ImageManager() {
		RawTextureGridMap = new ConcurrentHashMap<Integer, Image>();
		RawTextureSheetMap = new ConcurrentHashMap<Integer, Image>();
		RawTextureMap = new ConcurrentHashMap<Integer, Image>();
		CompletedImageMap = new ConcurrentHashMap<Integer, Image>();
	}

	public static ImageManager getImageManager() {
		if (instance == null) {
			instance = new ImageManager();
		}
		return instance;
	}

	public void initialize(AssetManager manager) {
		this.assetmanager = manager;

		DataManager Data = DataManager.getDataManager();
		DataLibrary TextureLibrary = Data.getTextureDataLibrary();
		DataLibrary TextureGridLibrary = Data.getTextureGridDataLibrary();
		DataLibrary TextureSheetLibrary = Data.getTextureSheetDataLibrary();

		ArrayList<TextureData> Textures = TextureLibrary.getEntries();
		ArrayList<TextureGridData> TextureGrids = TextureGridLibrary.getEntries();
		ArrayList<TextureSheetData> TextureSheets = TextureSheetLibrary.getEntries();

		Texture texture = null;
		int DefaultIndex = Data.getLabelIndex("TEXTURE_DEFAULT");
		TextureData DefaultTexture = Data.getTextureData((short) DefaultIndex);
		try {
			texture = assetmanager.loadTexture(DefaultTexture.FilePath);
		} catch (AssetNotFoundException Exception) {
			System.err.println(Exception.getMessage());
		}
		Image DefaultImage = texture.getImage();
		texture = null;


		for (int i = 0; i < TextureGrids.size(); i++) {
			TextureGridData TextureGridEntry = TextureGrids.get(i);
			String File = TextureGridEntry.FilePath;
			if (File != null) {
				try {
					texture = assetmanager.loadTexture(File);
				} catch (AssetNotFoundException Exception) {
					System.err.println(Exception.getMessage());
				}
				if (texture != null) {
					RawTextureGridMap.put(i, texture.getImage());
					texture = null;
				}
			}
		}

		texture = null;
		for (int i = 0; i < TextureSheets.size(); i++) {
			TextureSheetData TextureSheetEntry = TextureSheets.get(i);
			String File = TextureSheetEntry.FilePath;
			if (File != null) {
				try {
					texture = assetmanager.loadTexture(File);
				} catch (AssetNotFoundException Exception) {
					System.err.println(Exception.getMessage());
				}
				if (texture != null)
					RawTextureSheetMap.put(i, texture.getImage());
			}
		}

		for (int i = 0; i < Textures.size(); i++) {
			TextureData TextureEntry = Textures.get(i);
			String File = TextureEntry.FilePath;
			if (File != null) {
				try {
					texture = assetmanager.loadTexture(File);
				} catch (AssetNotFoundException Exception) {
					System.err.println(Exception.getMessage());
				}
				if (texture != null)
					RawTextureMap.put(i, texture.getImage());
			} else {
				int GridID = TextureEntry.GridID;
				if (GridID != DataManager.INVALID_INDEX) {
					TextureGridData GridData = DataManager.getDataManager().getTextureGridData(TextureEntry.GridID);

					Image SourceImage = RawTextureGridMap.get(GridID);
					if (SourceImage != null) {
						RawTextureMap.put(i, clipImage(SourceImage, TextureEntry.X * GridData.TextureWidth, TextureEntry.Y * GridData.TextureHeight, GridData.TextureWidth, GridData.TextureHeight));
						//SaveImage(NewImage, new String("Grid" + i));
					} else {
						RawTextureMap.put(i, DefaultImage);
					}
				}
				int SheetID = TextureEntry.SheetID;
				if (TextureEntry.SheetID != DataManager.INVALID_INDEX) {
					Image SourceImage = RawTextureSheetMap.get(SheetID);
					if (SourceImage != null) {
						RawTextureMap.put(i, clipImage(SourceImage, TextureEntry.X, TextureEntry.Y, TextureEntry.Width, TextureEntry.Height));
						//SaveImage(NewImage, new String("Sheet" + i));
					} else {
						RawTextureMap.put(i, DefaultImage);
					}
				}
			}

		}
		RawTextureGridMap.clear();
		RawTextureSheetMap.clear();
	}

	public void saveImage(Image SavedImage, String fileName) {
		if (SavedImage != null) {

			ByteBuffer outBuf = SavedImage.getData(0);
			String filePath = new String();
			File file = new File(filePath + "Testing" + File.separator + fileName + ".png").getAbsoluteFile();

			OutputStream outStream = null;
			try {
				outStream = new FileOutputStream(file);
				JmeSystem.writeImageFile(outStream, "png", outBuf, SavedImage.getWidth(), SavedImage.getHeight());
			} catch (IOException ex) {
				//logger.log(Level.SEVERE, "Error while saving screenshot", ex);
			} finally {
				if (outStream != null) {
					try {
						outStream.close();
					} catch (IOException ex) {
						//logger.log(Level.SEVERE, "Error while saving screenshot", ex);
					}
				}
			}
		}
	}

	private Image convertImage(Image image, Format newFormat) {

		int width = image.getWidth();
		int height = image.getHeight();
		ByteBuffer data = BufferUtils.createByteBuffer((int) Math.ceil(newFormat.getBitsPerPixel() / 8.0) * width * height);
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

	Image loadImage(String filepath, boolean ColorKey) {
		Texture Tex = assetmanager.loadTexture(filepath);

		if (ColorKey) {
			//convert color key
		}

		int ImageID = ImageCounter;
		ImageCounter++;
		return Tex.getImage();
	}

	Image clipImage(Image SourceImage, int X, int Y, int W, int H) {
		ImageRaster SourceRaster = ImageRaster.create(SourceImage);

		byte[] data = new byte[W * H * SourceImage.getFormat().getBitsPerPixel() / 8];
		ByteBuffer Buffer = BufferUtils.createByteBuffer(data);
		Image NewImage = new Image(SourceImage.getFormat(), W, H, Buffer);
		ImageRaster DestinationRastor = ImageRaster.create(NewImage);

		for (int x = 0; x < H; x++) {
			for (int y = 0; y < W; y++) {
				DestinationRastor.setPixel(x, y, SourceRaster.getPixel(X + x, Y + y));
			}
		}
		return NewImage;
	}

	void pasteImage(Image SourceImage, Image DestinationImage, int X, int Y) {
		ImageRaster SourceRaster = ImageRaster.create(SourceImage);
		ImageRaster DestinationRastor = ImageRaster.create(DestinationImage);

		for (int x = 0; x < SourceImage.getWidth(); x++) {
			for (int y = 0; y < SourceImage.getHeight(); y++) {
				DestinationRastor.setPixel(X + x, Y + y, SourceRaster.getPixel(x, y));
			}
		}
	}

	Image getMaterialImage(short MaterialTypeID, short SurfaceTypeID) {
		short TextureID = pickImageTexture(MaterialTypeID, SurfaceTypeID);
		return mapTexture(MaterialTypeID, TextureID);
	}

	public short pickImageTexture(short MaterialID, short SurfaceTypeID) {
		DataManager Data = DataManager.getDataManager();
		final short DefaultTextureID = Data.getLabelIndex("TEXTURE_DEFAULT");

		if (MaterialID == DataManager.INVALID_INDEX)
			return DefaultTextureID;
		MaterialData Material = Data.getMaterialData(MaterialID);

		short TextureID = Material.getTexture(SurfaceTypeID);
		short MaterialClassID = Material.MaterialClassID;

		if (TextureID != DataManager.INVALID_INDEX) {
			return TextureID;
		} else {
			if (MaterialClassID != DataManager.INVALID_INDEX) {
				TextureID = Data.getMaterialClassData(MaterialClassID).getTexture(SurfaceTypeID);
				if (TextureID != DataManager.INVALID_INDEX)
					return TextureID;
			}
			return DefaultTextureID;
		}
	}

	public Image mapTexture(short MaterialID, short TextureID) {
		DataManager Data = DataManager.getDataManager();
		if (MaterialID != DataManager.INVALID_INDEX && TextureID != DataManager.INVALID_INDEX) {
			int Key = MaterialID;
			Key = Key << 16;
			Key += TextureID;

			Image TargetImage = CompletedImageMap.get(Key);
			if (TargetImage != null) {
				return TargetImage;
			} else {
				Image SelectedMaterial = generateMaterialImage(MaterialID, TextureID); //, getStaticTextureName(MaterialID, TextureID));
				if (SelectedMaterial != null) {
					CompletedImageMap.put(Key, SelectedMaterial);
					return SelectedMaterial;
				}
			}
		}
		return null;
	}

	private Image generateMaterialImage(short MaterialID, short TextureID) {
		DataManager Data = DataManager.getDataManager();
		MaterialData Material = Data.getMaterialData(MaterialID);
		TextureData Texture = Data.getTextureData(TextureID);

		Image TextureImage = RawTextureMap.get((int) TextureID);

		short PrimaryColorID = Material.PrimaryColorID;
		short SecondaryColorID = Material.SecondaryColorID;
		short BorderColorID = Material.BorderColorID;

		String colormode = Material.ColorMode;

		if (colormode.equals("gradientmap")) {
			return generateGradientImage(TextureImage, PrimaryColorID, SecondaryColorID, BorderColorID);
		} else if (colormode.equals("keepimage")) {
			return generateKeeperImage(TextureImage, BorderColorID);
		} else if (colormode.isEmpty() || colormode.equals("overlay")) {
			return generatedOverLayImage(TextureImage, PrimaryColorID, BorderColorID);
		}
		return null;
	}

	private Image generateGradientImage(Image Original, short PrimaryColorID, short SecondaryColorID, short BorderColorID) {
		Image newImage = Original.clone();
		ImageRaster sourceReader = ImageRaster.create(Original);
		ImageRaster targetWriter = ImageRaster.create(newImage);

		int width = Original.getWidth();
		int height = Original.getHeight();

		ColorData PrimaryColor = DataManager.getDataManager().getColorData(PrimaryColorID);
		ColorData SecondaryColor = DataManager.getDataManager().getColorData(SecondaryColorID);

		Image OverlayImage = Original.clone();
		if (SecondaryColor != null) {
			for (int i = 0; i < width; i++) {
				for (int j = 0; j < height; j++) {
					//MaskImageData[(i * width * bpp) + (j * bpp) + 0] = SecondaryColor->getBlue();     // Blue
					//MaskImageData[(i * width * bpp) + (j * bpp) + 1] = SecondaryColor->getGreen();    // Green
					//MaskImageData[(i * width * bpp) + (j * bpp) + 2] = SecondaryColor->getRed();      // Red
					//MaskImageData[(i * width * bpp) + (j * bpp) + 3] = 255 - TextureImageData[(i * width) + j]; // Alpha
				}
			}
		}

		//Image OverlayImage = Original.clone();
		if (PrimaryColor != null) {
			ColorRGBA newColor = new ColorRGBA();
			newColor.r = PrimaryColor.Red / 255;
			newColor.g = PrimaryColor.Green / 255;
			newColor.b = PrimaryColor.Blue / 255;
			newColor.a = 1;

			for (int i = 0; i < width; i++) {
				for (int j = 0; j < height; j++) {
					targetWriter.setPixel(i, j, newColor);
				}
			}
		}

		//ilOverlayImage(MaskImageID, 0, 0, 0);

		if (BorderColorID != DataManager.INVALID_INDEX) {
			applyBorder(newImage, BorderColorID);
		}

		return newImage;
	}

	private Image generatedOverLayImage(Image Original, short PrimaryColorID, short BorderColorID) {
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
			applyBorder(newImage, BorderColorID);
		}

		return newImage;
	}

	private Image generateKeeperImage(Image Original, short BorderColorID) {
		Image NewImage = Original.clone();
		if (BorderColorID != DataManager.INVALID_INDEX) {
			applyBorder(NewImage, BorderColorID);
		}
		return NewImage;
	}

	private void applyBorder(Image Original, short BorderColorID) {
		int width = Original.getWidth();
		int height = Original.getHeight();

		ImageRaster targetWriter = ImageRaster.create(Original);

		float Red, Green, Blue;
		ColorData BorderColor = DataManager.getDataManager().getColorData(BorderColorID);

		Red = BorderColor.Red / 255f;
		Green = BorderColor.Green / 255f;
		Blue = BorderColor.Blue / 255f;
		ColorRGBA color = new ColorRGBA(Red, Green, Blue, 1.0f);

		if (targetWriter != null) {
			for (int i = 0; i < width; i++) {
				targetWriter.setPixel(i, 0, color);
				targetWriter.setPixel(i, height - 1, color);
			}

			for (int j = 0; j < height; j++) {
				targetWriter.setPixel(width - 1, j, color);
				targetWriter.setPixel(0, j, color);
			}
		}
	}
}
