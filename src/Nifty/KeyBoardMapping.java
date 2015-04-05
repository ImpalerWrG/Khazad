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
