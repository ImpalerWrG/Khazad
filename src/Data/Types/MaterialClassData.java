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

package Data.Types;

import Data.DataBase;
import Data.DataLibrary;
import Data.DataManager;
import java.io.Serializable;

import nu.xom.Element;
import nu.xom.Elements;

/**
 *
 * @author Impaler
 */
public class MaterialClassData extends DataBase implements Serializable {

	private static final long serialVersionUID = 1;
	transient String DefaultMaterialLabel;
	int DefaultMaterialID;
	transient String[] SurfaceTypeLabels;
	transient String[] SurfaceTextureLabels;
	short[] SurfaceTypeIDs;
	short[] SufaceTextueIDs;

	public MaterialClassData() {
	}

	public boolean loadData(Element MaterialClassEntry, DataLibrary Library) {
		Element Name = MaterialClassEntry.getFirstChildElement("Name", MaterialClassEntry.getNamespaceURI());
		Library.indexEntry(Name.getAttributeValue("Label"), this);

		Element DefaultMaterial = MaterialClassEntry.getFirstChildElement("DefaultMaterial", MaterialClassEntry.getNamespaceURI());
		if (DefaultMaterial != null) {
			DefaultMaterialLabel = DefaultMaterial.getAttribute("label").getValue();
		}

		Element SurfaceTexturesElement = MaterialClassEntry.getFirstChildElement("DefaultSurfaceTextures", MaterialClassEntry.getNamespaceURI());
		if (SurfaceTexturesElement != null) {
			Elements SurfaceTextures = SurfaceTexturesElement.getChildElements();

			SurfaceTypeLabels = new String[SurfaceTextures.size()];
			SurfaceTypeIDs = new short[SurfaceTextures.size()];
			SurfaceTextureLabels = new String[SurfaceTextures.size()];
			SufaceTextueIDs = new short[SurfaceTextures.size()];

			for (int i = 0; i < SurfaceTextures.size(); i++) {
				Element SurfaceTextureEntry = SurfaceTextures.get(i);
				SurfaceTextureLabels[i] = SurfaceTextureEntry.getAttribute("Texturelabel").getValue();
				SurfaceTypeLabels[i] = SurfaceTextureEntry.getAttribute("SurfaceTypelabel").getValue();
			}
		}
		return true;
	}

	public boolean postProcessing() {
		DataManager Data = DataManager.getDataManager();
		if (SurfaceTypeLabels != null) {
			for (int i = 0; i < SurfaceTypeLabels.length; i++) {
				SurfaceTypeIDs[i] = Data.getLabelIndex(SurfaceTypeLabels[i]);
			}
		}
		if (SurfaceTextureLabels != null) {
			for (int i = 0; i < SurfaceTextureLabels.length; i++) {
				SufaceTextueIDs[i] = Data.getLabelIndex(SurfaceTextureLabels[i]);
			}
		}
		DefaultMaterialID = Data.getLabelIndex(DefaultMaterialLabel);
		return true;
	}

	public short getTexture(short SurfaceType) {
		for (int i = 0; i < SurfaceTypeIDs.length; i++) {
			if (SurfaceTypeIDs[i] == SurfaceType) {
				return SufaceTextueIDs[i];
			}
		}
		return DataManager.INVALID_INDEX;
	}
}
