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

/**
 *
 * @author Impaler
 */
public class SurfaceTypeData extends DataBase implements Serializable {

	private static final long serialVersionUID = 1;
	transient private String TextureLabel;
	int TextureID;

	public SurfaceTypeData() {
	}

	public boolean loadData(Element SurfaceTypeEntry, DataLibrary Library) {
		Element Name = SurfaceTypeEntry.getFirstChildElement("Name", SurfaceTypeEntry.getNamespaceURI());
		Library.indexEntry(Name.getAttributeValue("Label"), this);

		Element TextureUsed = SurfaceTypeEntry.getFirstChildElement("TextureUsed", SurfaceTypeEntry.getNamespaceURI());
		if (TextureUsed != null) {
			TextureLabel = TextureUsed.getAttribute("label").getValue();
			return true;
		}
		return false;
	}

	public boolean postProcessing() {
		TextureID = DataManager.getDataManager().getLabelIndex(TextureLabel);
		return true;
	}
}
