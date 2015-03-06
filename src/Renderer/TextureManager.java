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

import java.util.concurrent.ConcurrentHashMap;
import java.util.ArrayList;

import com.jme3.texture.Texture;
import com.jme3.texture.Image;
/**
 *
 * @author Impaler
 */

public class TextureManager {

	AssetManager assetmanager = null;
	
	ConcurrentHashMap<Integer, Texture> RawTextureMap;
	ConcurrentHashMap<Integer, Image> TextureMap;

	int ImageCounter = 0;
	private static TextureManager instance = null;

	
	protected TextureManager() {
		RawTextureMap = new ConcurrentHashMap<Integer, Texture>();
		TextureMap = new ConcurrentHashMap<Integer, Image>();
	}

	public static TextureManager getTextureManager() {
		if(instance == null) {
			instance = new TextureManager();
		}
		return instance;
	}

	public void Initialize(AssetManager manager) {
		this.assetmanager = manager;

		DataManager Data = DataManager.getDataManager();
		DataLibrary TextureLibrary = Data.getTextureDataLibrary();
		ArrayList<TextureData> Textures = TextureLibrary.getEntries();
		Texture texture = null;
		
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
					RawTextureMap.put(i, texture);
			}
		}
	}

	short PickImageTexture(short MaterialID, short SurfaceTypeID) {
		DataManager Data = DataManager.getDataManager();
		MaterialData Material = Data.getMaterialData(MaterialID);

		short TextureID = Material.getTexture(SurfaceTypeID);
		short MaterialClassID = Material.MaterialClassID;

		if (TextureID != DataManager.INVALID_INDEX) {
			return TextureID;
		} else {
			if (MaterialClassID != DataManager.INVALID_INDEX) {
				TextureID = Data.getMaterialClassData(MaterialClassID).getTexture(SurfaceTypeID);
				if (TextureID == DataManager.INVALID_INDEX) {
					//cerr << "bad material/surface combination, no texture. MaterialClassID: " << MaterialClassID << " SurfaceTypeID: " << SurfaceTypeID << endl;
					return  Data.getLabelIndex("TEXTURE_DEFAULT");
				} else {
					return TextureID;
				}
			}
			return  Data.getLabelIndex("TEXTURE_DEFAULT");
		}
	}

	Image MapTexture(short MaterialID, short TextureID) {
		DataManager Data = DataManager.getDataManager();
		if (MaterialID != DataManager.INVALID_INDEX && TextureID != DataManager.INVALID_INDEX) {
			int Key = MaterialID;
			Key = Key << 16;
			Key += TextureID;

			Image TargetImage = TextureMap.get(Key);
			if (TargetImage != null) {
				return TargetImage;
			} else {
				Image SelectedMaterial = new Image(); //makeStaticMaterial(makeImage(MaterialID, TextureID), getStaticTextureName(MaterialID, TextureID));
				if (SelectedMaterial != null) {
					TextureMap.put(Key, SelectedMaterial);
					return SelectedMaterial;
				}
			}
		}
		return null;
	}

	Image getOgreMaterial(short MaterialTypeID, short SurfaceTypeID) {
		short TextureID = PickImageTexture(MaterialTypeID, SurfaceTypeID);
		return MapTexture(MaterialTypeID, TextureID);
	}

	String getStaticMaterialName(short MaterialTypeID, short TextureID) {
		return new String(); //("StaticMat%i-%i",  MaterialTypeID, TextureID);
	}

	String getStaticTextureName(short MaterialTypeID, short TextureID){
		return new String(); //( "StaticTex%i-%i",  MaterialTypeID, TextureID);
	}

	Image ConvertToOgreImage(short ImageID) {
		//uint8_t* iData = IMAGE->getImageData(ImageID);
		//uint16_t Width = IMAGE->getImageWidth(ImageID);
		//uint16_t Height = IMAGE->getImageHeight(ImageID);

		Image NewImage = new Image(); // Delete?
		//NewImage->loadDynamicImage(iData, Width, Height, Ogre::PF_A8R8G8B8);

		return NewImage;
	}

	Image makeImage(short MaterialTypeID, short TextureID) {
		ImageManager Imaging = ImageManager.getImageManager();
		return Imaging.GenerateMaterialImage(MaterialTypeID, TextureID);
	}
/*
	Ogre::MaterialPtr makeStaticMaterial(Image NewImage, String MaterialName) {
		Ogre::MaterialPtr NewMaterial = Ogre::MaterialManager::getSingleton().create(MaterialName, "General", true);
		Ogre::Technique* FirstTechnique = NewMaterial->getTechnique(0);
		Ogre::Pass* FirstPass = FirstTechnique->getPass(0);

		Ogre::TexturePtr NewTex = Ogre::TextureManager::getSingleton().loadImage(MaterialName, "General", *NewImage, Ogre::TEX_TYPE_2D, Ogre::MIP_UNLIMITED, 1.0, true, Ogre::PF_A8R8G8B8);
		//FirstPass->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);

		Ogre::ColourValue lSelfIllumnationColour(0.0f, 0.0f, 0.0f, 1.0f);
		FirstPass->setSelfIllumination(lSelfIllumnationColour);

		Ogre::ColourValue lDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
		FirstPass->setDiffuse(lDiffuseColour);

		Ogre::ColourValue lAmbientColour(1.0f, 1.0f, 1.0f, 1.0f);
		FirstPass->setAmbient(lAmbientColour);

		Ogre::ColourValue lSpecularColour(0.0f, 0.0f, 0.0f, 1.0f);
		FirstPass->setSpecular(lSpecularColour);

		FirstPass->setShininess(0.0f);

		FirstPass->setAlphaRejectSettings(Ogre::CMPF_GREATER, 0, false);

		Ogre::TextureUnitState* TextureUnit = FirstPass->createTextureUnitState();
		TextureUnit->setTextureFiltering(Ogre::TFO_NONE);
		TextureUnit->setTextureName(MaterialName, Ogre::TEX_TYPE_2D);

		return NewMaterial;
	}

	/*
	Ogre::MaterialPtr TextureManager::makeLayeredTexture(int16_t* TextureID, int16_t* ColorID)
	{

	}

	const char* TextureManager::getAnimationTextureName(int16_t AnimationGroupID, int16_t ColorID, int16_t SlideNumber)
	{
		char buffer[64];
		sprintf(buffer, "Slide%i-%i-%i",  AnimationGroupID, ColorID, SlideNumber);
		return buffer;
	}

	Ogre::MaterialPtr TextureManager::makeAnimatedMaterial(int16_t AnimationGroupID, int16_t ColorID)
	{
		AnimationGroupData* Animation = DATA->getAnimationGroupData(AnimationGroupID);
		uint16_t Animations = Animation->getTextureCount();

		Ogre::String* TextureNames = new Ogre::String[Animations];

		char buffer[32];

		Ogre::MaterialPtr NewMaterial = Ogre::MaterialManager::getSingleton().create(getStaticMaterialName(buffer, AnimationGroupID, ColorID), "General", true);
		Ogre::Technique* FirstTechnique = NewMaterial->getTechnique(0);
		Ogre::Pass* FirstPass = FirstTechnique->getPass(0);
		//FirstPass->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
		FirstPass->setLightingEnabled(false);
		FirstPass->setAlphaRejectSettings(Ogre::CMPF_GREATER, 0, false);


		Ogre::TextureUnitState* TextureUnit = FirstPass->createTextureUnitState();
		TextureUnit->setAnimatedTextureName(TextureNames, Animations, 0);
		TextureUnit->setTextureFiltering(Ogre::TFO_NONE);

		for (uint16_t i = 0; i < Animations; i++)
		{
			TextureNames[i] = getAnimationTextureName(AnimationGroupID, ColorID, i);
			TextureData* Texture = DATA->getTextureData(Animation->getTexture(i));

			uint16_t ImageID = IMAGE->GenerateKeeperImage(Texture->getDevILID(), -1);
			Ogre::Image* NewImage = ConvertToOgreImage(ImageID);

			Ogre::TexturePtr NewTex = Ogre::TextureManager::getSingleton().loadImage(TextureNames[i], "General", *NewImage); //, Ogre::TEX_TYPE_2D, Ogre::MIP_UNLIMITED, 1.0, true, Ogre::PF_A8R8G8B8);
			TextureUnit->setFrameTextureName(TextureNames[i], i);
		}


		return NewMaterial;
	}

	/*
	Ogre::MaterialPtr TextureManager::makeAnimatedMaterial(Ogre::Image* Frames)
	{


	}

	Ogre::MaterialPtr TextureManager::makeLayeredMaterial(Ogre::Image* Layers, int16_t ColorID)
	{

	}
	*/
}

