/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package Data;

import nu.xom.*;
import java.io.IOException;

import java.io.File;

/**
 *
 * @author Impaler
 */
public class XMLManager {
	
	Document LoadFile(String Filename) {
		Document doc;
		try {
		  Builder parser = new Builder();
		  doc = parser.build(new File("assets\\" + Filename));
		}
		catch (ParsingException ex) {
		  System.err.println(Filename + " is malformed..." + ex.getMessage());
		  return null;
		}
		catch (IOException ex) {
		  System.err.println("Could not find file " + Filename + ex.getMessage());
		  return null;
		}
		return doc;
	}
}
