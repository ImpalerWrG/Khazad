/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package Data;

import java.util.ArrayList;
import nu.xom.Element;
/**
 *
 * @author Impaler
 */
public class DataLibrary<T extends DataBase> {
	
	ArrayList<T> Entries;
	private Class<T> DataClass;
	DataManager Data;

	DataLibrary(Class<T> Class, DataManager Parent) {
		Entries = new ArrayList<T>();
		DataClass = Class;
		Data = Parent;
	}

	void PostProcessDataClass() {
		for (T Entry : Entries) {
			Entry.PostProcessing();
		}
	}

	void LoadElement(Element XMLEntry) {
		try {
			DataBase NewEntry = DataClass.newInstance();
			NewEntry.LoadData(XMLEntry, this);
		}
		catch (InstantiationException ex) {
			System.err.println(ex.getMessage());
		}
		catch (IllegalAccessException ex) {
		  System.err.println(ex.getMessage());
		}
	}
	
	public void IndexEntry(String Name, T NewEntry) {
		Data.addLabel(Name, (short) Entries.size());
		Entries.add((T) NewEntry);		
	}
}
