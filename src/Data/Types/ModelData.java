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
import java.io.Serializable;
import nu.xom.Element;

/**
 *
 * @author Impaler
 */
public class ModelData extends DataBase implements Serializable {

	private static final long serialVersionUID = 1;
	String FilePath;

	public ModelData() {
	}

	public boolean loadData(Element ModelEntry, DataLibrary Library) {
		Element Name = ModelEntry.getFirstChildElement("Name", ModelEntry.getNamespaceURI());
		Library.indexEntry(Name.getAttributeValue("Label"), this);

		Element File = ModelEntry.getFirstChildElement("File", ModelEntry.getNamespaceURI());
		if (File != null)
			FilePath = File.getAttributeValue("Path");

		return true;
	}

	public boolean postProcessing() {
		return true;
	}
}
