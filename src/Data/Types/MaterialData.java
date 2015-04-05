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
public class MaterialData extends DataBase implements Serializable {

	private static final long serialVersionUID = 1;
	// Colors used to define this Material
	transient private String PrimaryColorlabel, SecondaryColorlabel, BorderColorlabel;
	public short PrimaryColorID, SecondaryColorID, BorderColorID;
	// Class that this Material belongs too
	transient private String MaterialClasslabel;
	public short MaterialClassID;
	// Surface Texturing of this Material
	transient private String[] SurfaceTypeLabels;
	transient private String[] SurfaceTextureLabels;
	private short[] SurfaceTypeIDs;
	private short[] SufaceTextueIDs;
	// Other data
	public String ColorMode;
	public int PlantGrowthFactor;

	public MaterialData() {
		ColorMode = new String();
	}

	public boolean loadData(Element MaterialEntry, DataLibrary Library) {
		Element Name = MaterialEntry.getFirstChildElement("Name", MaterialEntry.getNamespaceURI());
		Library.indexEntry(Name.getAttributeValue("Label"), this);

		Element MaterialClassElement = MaterialEntry.getFirstChildElement("MaterialClass", MaterialEntry.getNamespaceURI());
		if (MaterialClassElement != null)
			MaterialClasslabel = MaterialClassElement.getAttributeValue("label");

		Element PrimaryColorElement = MaterialEntry.getFirstChildElement("PrimaryColor", MaterialEntry.getNamespaceURI());
		if (PrimaryColorElement != null)
			PrimaryColorlabel = PrimaryColorElement.getAttributeValue("label");

		Element SecondaryColorElement = MaterialEntry.getFirstChildElement("SecondaryColor", MaterialEntry.getNamespaceURI());
		if (SecondaryColorElement != null)
			SecondaryColorlabel = SecondaryColorElement.getAttributeValue("label");

		Element BorderColorElement = MaterialEntry.getFirstChildElement("BorderColor", MaterialEntry.getNamespaceURI());
		if (BorderColorElement != null)
			BorderColorlabel = BorderColorElement.getAttributeValue("label");

		Element ColorModeElement = MaterialEntry.getFirstChildElement("ColorMode", MaterialEntry.getNamespaceURI());
		if (ColorModeElement != null)
			ColorMode = ColorModeElement.getAttributeValue("mode");

		Element PlantGrowthElement = MaterialEntry.getFirstChildElement("PlantGrowth", MaterialEntry.getNamespaceURI());
		if (PlantGrowthElement != null)
			PlantGrowthFactor = Integer.parseInt(PlantGrowthElement.getAttributeValue("growthfactor"));

		Element SurfaceTexturesElement = MaterialEntry.getFirstChildElement("SurfaceTextures", MaterialEntry.getNamespaceURI());
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

		MaterialClassID = Data.getLabelIndex(MaterialClasslabel);
		PrimaryColorID = Data.getLabelIndex(PrimaryColorlabel);
		SecondaryColorID = Data.getLabelIndex(SecondaryColorlabel);
		BorderColorID = Data.getLabelIndex(BorderColorlabel);

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
