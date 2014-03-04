/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package PathFinding;

import Map.Direction;
import Map.MapCoordinate;

/**
 *
 * @author Impaler
 */
public interface PathWalker {
		
    public MapCoordinate NextCoordinate();
    public Direction NextDirection();
	
    public MapCoordinate PeekCoordinate();
    public Direction PeekDirection();
	
    void Reset();
	public int getCurrentStep();
	//void setCurrentStep();

    //int CurrentStep;

}
