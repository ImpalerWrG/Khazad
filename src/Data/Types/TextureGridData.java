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

import Data.DataManager;
import Data.DataBase;
import Data.DataLibrary;

import nu.xom.Element;
import nu.xom.Elements;

import java.io.Serializable;

/**
 *
 * @author Impaler
 */


public class TextureGridData extends DataBase implements Serializable {

	private static final long serialVersionUID = 1;
	public String FilePath;
	// Used to clip images off the Grid
	public int TextureWidth, TextureHeight;
	public int GridWidth, GridHeight;

	public TextureGridData() {
	}

	public boolean loadData(Element TextureGridEntry, DataLibrary Library) {
		short GridID = Library.indexEntry(null, this);

		Element File = TextureGridEntry.getFirstChildElement("File", TextureGridEntry.getNamespaceURI());
		FilePath = File.getAttribute("Path").getValue();

		Element TextureSize = TextureGridEntry.getFirstChildElement("TextureSize", TextureGridEntry.getNamespaceURI());
		TextureHeight = Integer.parseInt(TextureSize.getAttribute("Height").getValue());
		TextureWidth = Integer.parseInt(TextureSize.getAttribute("Width").getValue());

		Element GridSize = TextureGridEntry.getFirstChildElement("GridSize", TextureGridEntry.getNamespaceURI());
		GridHeight = Integer.parseInt(GridSize.getAttribute("Height").getValue());
		GridWidth = Integer.parseInt(GridSize.getAttribute("Width").getValue());

		Element TexturesElement = TextureGridEntry.getFirstChildElement("Textures", TextureGridEntry.getNamespaceURI());
		Elements Textures = TexturesElement.getChildElements();
		DataLibrary TextureLibrary = DataManager.getDataManager().getTextureDataLibrary();

		for (int i = 0; i < Textures.size(); i++) {
			Element TextureEntry = Textures.get(i);

			TextureData NewTexture = new TextureData();
			NewTexture.loadData(TextureEntry, TextureLibrary);
			NewTexture.GridID = GridID;
		}
		return true;
	}

	public boolean postProcessing() {
		return true;
	}
}
