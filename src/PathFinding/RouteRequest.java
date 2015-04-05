/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package PathFinding;

import Map.MapCoordinate;
import Map.Zone;
import java.io.Serializable;

/**
 * Encapsulation of all the data needed for all types of routes that might be
 * needed, basicly a struct with all public members
 *
 * @author Impaler
 */
public class RouteRequest implements Serializable {

	private static final long serialVersionUID = 1;
	MapCoordinate Start;
	MapCoordinate Goal;
	Zone GoalZone;
}
