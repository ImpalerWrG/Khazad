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
import nu.xom.Elements;
/**
 *
 * @author  Dallas
 */
public class GivenNameGroupData extends DataBase implements Serializable {
	private static final long serialVersionUID = 1;
	
	private String group;
	private String gender;
	private String[] prefixes;
	private String[] suffixes;
	

	public GivenNameGroupData() {
	
	}

	@Override
	public boolean loadData(Element GivenNameEntry, DataLibrary Library) {
		Element Name = GivenNameEntry.getFirstChildElement("Name", GivenNameEntry.getNamespaceURI());
		group = Name.getAttributeValue("Label");

		Element elementPrefixes = GivenNameEntry.getFirstChildElement("GivenNamePrefixes", GivenNameEntry.getNamespaceURI());
		gender = elementPrefixes.getAttributeValue("gender");
		Elements elementPrefixesChildren = elementPrefixes.getChildElements();
		prefixes = new String[elementPrefixesChildren.size()];
		for (int i = 0; i < elementPrefixesChildren.size(); i++) {
			prefixes[i] = elementPrefixesChildren.get(i).getAttributeValue("text");
		}
		Element elementSuffixes = GivenNameEntry.getFirstChildElement("GivenNameSuffixes", GivenNameEntry.getNamespaceURI());
		Elements elementSuffixesChildren = elementSuffixes.getChildElements();
		suffixes = new String[elementSuffixesChildren.size()];
		for (int i = 0; i < elementSuffixesChildren.size(); i++) {
			suffixes[i] = elementSuffixesChildren.get(i).getAttributeValue("text");
		}
		Library.indexEntry(getGroup() + "_" + getGender(), this);
		return false;
	}
	
	public boolean postProcessing() {
		return true;
	}

	public String getGroup() {
		return group;
	}

	public String getGender() {
		return gender;
	}

	public String[] getPrefixes() {
		return prefixes;
	}

	public String[] getSuffixes() {
		return suffixes;
	}
}
