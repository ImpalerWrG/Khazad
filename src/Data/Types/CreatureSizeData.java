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
public class CreatureSizeData extends DataBase implements Serializable {

	private static final long serialVersionUID = 1;
	transient byte[] ModifierValues;
	transient String[] AttributeLabels;
	public byte[] AttributeModifierVales;

	public CreatureSizeData() {
	}

	@Override
	public boolean loadData(Element CreatureSizeEntry, DataLibrary Library) {
		Element Name = CreatureSizeEntry.getFirstChildElement("Name", CreatureSizeEntry.getNamespaceURI());
		Library.indexEntry(Name.getAttributeValue("Label"), this);

		Element ModifiersElement = CreatureSizeEntry.getFirstChildElement("BasicAttributeModifiers", CreatureSizeEntry.getNamespaceURI());
		if (ModifiersElement != null) {
			Elements AttributeModifiers = ModifiersElement.getChildElements();
			AttributeLabels = new String[AttributeModifiers.size()];
			ModifierValues = new byte[AttributeModifiers.size()];

			for (int i = 0; i < AttributeModifiers.size(); i++) {
				Element SurfaceTextureEntry = AttributeModifiers.get(i);
				AttributeLabels[i] = SurfaceTextureEntry.getAttribute("label").getValue();
				ModifierValues[i] = (byte) Integer.parseInt(SurfaceTextureEntry.getAttribute("Modifier").getValue());
			}
		}

		return false;
	}

	public boolean postProcessing() {
		DataManager Data = DataManager.getDataManager();
		AttributeModifierVales = new byte[Data.getNumBaseAttributes()];

		for (int i = 0; i < AttributeLabels.length; i++) {
			short Index = Data.getLabelIndex(AttributeLabels[i]);
			AttributeModifierVales[Index] = ModifierValues[i];
		}
		return true;
	}
}
