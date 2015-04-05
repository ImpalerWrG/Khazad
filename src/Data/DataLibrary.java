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

package Data;

import java.io.Serializable;
import java.util.ArrayList;
import nu.xom.Element;

/**
 *
 * @author Impaler
 */
public class DataLibrary<T extends DataBase> implements Serializable {

	private static final long serialVersionUID = 1;
	ArrayList<T> Entries;
	private Class<T> DataClass;
	DataManager Data;

	DataLibrary(Class<T> Class, DataManager Parent) {
		Entries = new ArrayList<T>();
		DataClass = Class;
		Data = Parent;
	}

	void postProcessDataClass() {
		for (T Entry : Entries) {
			Entry.postProcessing();
		}
	}

	void loadElement(Element XMLEntry) {
		try {
			DataBase NewEntry = DataClass.newInstance();
			NewEntry.loadData(XMLEntry, this);
		} catch (InstantiationException ex) {
			System.err.println(ex.getMessage());
		} catch (IllegalAccessException ex) {
			System.err.println(ex.getMessage());
		}
	}

	public short indexEntry(String Name, T NewEntry) {
		if (Name != null)
			Data.addLabel(Name, (short) Entries.size());
		Entries.add((T) NewEntry);
		return (short) (Entries.size() - 1);
	}

	public ArrayList<T> getEntries() {
		return Entries;
	}
}
