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
public class ColorData extends DataBase implements Serializable {

	private static final long serialVersionUID = 1;
	public short Red, Green, Blue;

	public ColorData() {
	}

	@Override
	public boolean loadData(Element ColorEntry, DataLibrary Library) {
		Element Name = ColorEntry.getFirstChildElement("Name", ColorEntry.getNamespaceURI());
		Library.indexEntry(Name.getAttributeValue("Label"), this);

		Element Channels = ColorEntry.getFirstChildElement("Channels", ColorEntry.getNamespaceURI());
		if (Channels != null) {
			Red = (short) Integer.parseInt(Channels.getAttribute("Red").getValue());
			Green = (short) Integer.parseInt(Channels.getAttribute("Green").getValue());
			Blue = (short) Integer.parseInt(Channels.getAttribute("Blue").getValue());
			return true;
		}
		return false;
	}

	public boolean postProcessing() {
		return true;
	}
}
