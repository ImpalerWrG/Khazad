/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package Data;

import nu.xom.Element;
/**
 *
 * @author Impaler
 */
public abstract class DataBase {

	public abstract boolean PostProcessing();
	public abstract boolean LoadData(Element XMLElement, DataLibrary Library);
    String Name;
}
