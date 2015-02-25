/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package Data.Types;

import Data.DataBase;
import Data.DataLibrary;
import nu.xom.Element;
/**
 *
 * @author Impaler
 */
public class ColorData extends DataBase {
	
	short Red, Green, Blue;

	public ColorData() {
	
	}

	@Override
	public boolean LoadData(Element ColorEntry, DataLibrary Library) {
		Element Name = ColorEntry.getFirstChildElement("Name", ColorEntry.getNamespaceURI());
		Library.IndexEntry(Name.getAttributeValue("Label"), this);

		Element Channels = ColorEntry.getFirstChildElement("Channels", ColorEntry.getNamespaceURI());
		if (Channels != null) {
			Red = (short) Integer.parseInt(Channels.getAttribute("Red").getValue());
			Green = (short) Integer.parseInt(Channels.getAttribute("Green").getValue());
			Blue = (short) Integer.parseInt(Channels.getAttribute("Blue").getValue());
			return true;
		}
		return false;
	}
	
	public boolean PostProcessing() {
		return true;
	}
}
