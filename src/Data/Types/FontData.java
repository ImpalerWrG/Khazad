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
public class FontData extends DataBase implements Serializable {

	private static final long serialVersionUID = 1;
	String FilePath;
	int Size;

	public FontData() {
	}

	public boolean loadData(Element FontEntry, DataLibrary Library) {
		Element Name = FontEntry.getFirstChildElement("Name", FontEntry.getNamespaceURI());
		Library.indexEntry(Name.getAttributeValue("Label"), this);

		Element FileElement = FontEntry.getFirstChildElement("File", FontEntry.getNamespaceURI());
		FilePath = FileElement.getAttributeValue("Path");

		Element SizeElement = FontEntry.getFirstChildElement("Size", FontEntry.getNamespaceURI());
		Size = Integer.parseInt(SizeElement.getAttributeValue("Int"));

		return true;
	}

	public boolean postProcessing() {
		return true;
	}
}
