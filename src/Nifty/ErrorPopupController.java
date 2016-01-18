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

import de.lessvoid.nifty.Nifty;
import de.lessvoid.nifty.controls.Controller;
import de.lessvoid.nifty.controls.Label;
import de.lessvoid.nifty.elements.Element;
import de.lessvoid.nifty.input.NiftyInputEvent;
import de.lessvoid.nifty.screen.Screen;
import de.lessvoid.xml.xpp3.Attributes;
import java.util.Properties;

/**
 *
 * @author Dallas
 */
public class ErrorPopupController implements Controller {

	Nifty nifty;
	Screen screen;
	Element popup;

	public void bind(Nifty nifty, Screen screen, Element element, Properties parameter, Attributes controlDefinitionAttributes) {
		this.nifty = nifty;
		this.screen = screen;
		this.popup = element;
	}

	public void init(Properties parameter, Attributes controlDefinitionAttributes) {
	}

	public void onStartScreen() {
	}

	public void onFocus(boolean getFocus) {
	}

	public boolean inputEvent(NiftyInputEvent inputEvent) {
		return false;
	}
	
	public void CloseErrorPopup() {
		nifty.closePopup(popup.getId());
	}
	
	public static void ShowErrorMessage(Nifty nifty, String title, String message) {
		Element popup = nifty.createPopup("ErrorPopup");
		Label errorTitleLabel = popup.findNiftyControl("ErrorTitleLabel", Label.class);
		if (errorTitleLabel != null) {
			errorTitleLabel.setText(title);
		}
		Label errorMessageLabel = popup.findNiftyControl("ErrorMessageLabel", Label.class);
		if (errorMessageLabel != null) {
			errorMessageLabel.setText(message);
		}
		nifty.showPopup(nifty.getCurrentScreen(), popup.getId(), null);
	}
}
