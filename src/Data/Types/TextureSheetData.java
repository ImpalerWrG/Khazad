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
public class TextureSheetData extends DataBase implements Serializable {

	private static final long serialVersionUID = 1;
	public String FilePath;

	public TextureSheetData() {
	}

	public boolean loadData(Element TextureSheetEntry, DataLibrary Library) {
		short SheetID = Library.indexEntry(null, this);

		Element File = TextureSheetEntry.getFirstChildElement("File", TextureSheetEntry.getNamespaceURI());
		FilePath = File.getAttribute("Path").getValue();

		Element TexturesElement = TextureSheetEntry.getFirstChildElement("Textures", TextureSheetEntry.getNamespaceURI());
		Elements Textures = TexturesElement.getChildElements();
		DataLibrary TextureLibrary = DataManager.getDataManager().getTextureDataLibrary();

		for (int i = 0; i < Textures.size(); i++) {
			Element TextureEntry = Textures.get(i);

			TextureData NewTexture = new TextureData();
			NewTexture.loadData(TextureEntry, TextureLibrary);

			NewTexture.SheetID = SheetID;
		}
		return true;

	}

	public boolean postProcessing() {
		return true;
	}
}
