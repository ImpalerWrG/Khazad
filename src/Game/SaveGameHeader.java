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
public class SaveGameHeader implements Serializable, Comparable<SaveGameHeader> {

	private static final long serialVersionUID = 1;
	public String version;
	public String kingdomName;
	public String timeString;
	public Date lastPlayed;
	public transient String fileName;

	@Override
	public String toString() {
		// displayed on load game screen list box
		return kingdomName;
	}

	public int compareTo(SaveGameHeader that) {
		// sort by lastPlayed descending on the load game screen
		return -this.lastPlayed.compareTo(that.lastPlayed);
	}
}
