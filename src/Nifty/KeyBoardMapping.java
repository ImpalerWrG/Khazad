/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package Nifty;

import de.lessvoid.nifty.input.NiftyInputMapping;
import de.lessvoid.nifty.input.NiftyInputEvent;
import de.lessvoid.nifty.input.keyboard.KeyboardInputEvent;
/**
 *
 * @author Impaler
 */
public class KeyBoardMapping implements NiftyInputMapping {

	public NiftyInputEvent convert(final KeyboardInputEvent inputEvent) {
		if (inputEvent.isKeyDown()) {
			if (inputEvent.getKey() == KeyboardInputEvent.KEY_F1) {
				return NiftyInputEvent.ConsoleToggle;
			} else if (inputEvent.getKey() == KeyboardInputEvent.KEY_RETURN) {
				return NiftyInputEvent.Activate;
			} else if (inputEvent.getKey() == KeyboardInputEvent.KEY_ESCAPE) {
				return NiftyInputEvent.Escape;
				
			//} else if (inputEvent.getKey() == KeyboardInputEvent.KEY_SPACE) {
				//return NiftyInputEvent.Activate;
				//NiftyInputEvent event = NiftyInputEvent.Character;
				//char space = ' ';
				//event.setCharacter(space);
				//return event;
			} else if (inputEvent.getKey() == KeyboardInputEvent.KEY_TAB) {
				if (inputEvent.isShiftDown()) {
					return NiftyInputEvent.PrevInputElement;
				} else {
					return NiftyInputEvent.NextInputElement;
				}
			}
		}		
		
		return null;
	}
}

