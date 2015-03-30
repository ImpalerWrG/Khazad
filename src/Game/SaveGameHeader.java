/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package Game;

import java.io.Serializable;
import java.util.Date;

/**
 *
 * @author Dallas
 */
public class SaveGameHeader implements Serializable {
	public String version;
	public String kingdomName;
	public String timeString;
	public Date lastPlayed;
	public transient String fileName;
	
	public String toString() {
		return kingdomName;
	}
}
