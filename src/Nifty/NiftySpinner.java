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
import de.lessvoid.nifty.controls.Button;
import de.lessvoid.nifty.controls.TextField;
import de.lessvoid.nifty.elements.Element;
import de.lessvoid.nifty.input.NiftyInputEvent;
import de.lessvoid.nifty.screen.Screen;

import de.lessvoid.xml.xpp3.Attributes;
import java.util.Properties;

/**
 * Implementations of a basic Spinner element for UI
 * 
 * @author Impaler
 */
public class NiftySpinner extends de.lessvoid.nifty.controls.AbstractController implements Spinner {
	int value = 0;
	int minimum = 0;
	int maximum = 0;
	int increment = 1;

	Button buttonUp = null;
	Button buttonDown = null;
	TextField field = null;

	public void onStartScreen() {
	}

	public boolean inputEvent(final NiftyInputEvent inputEvent) {
		System.out.println("Spinner input event");
		return false;
	}

	@Override
	public void init(Properties parameter, Attributes controlDefinitionAttributes) {
		super.init(parameter, controlDefinitionAttributes);
		field.setText(getValueString()); 
	}

	public void bind(Nifty nifty, Screen screen, Element element, Properties parameter, Attributes controlDefinitionAttributes) {
		buttonUp = element.findNiftyControl("spinnerUpButton", Button.class);
		buttonDown = element.findNiftyControl("spinnerDownButton", Button.class);
		field = element.findNiftyControl("spinnerField", TextField.class);

		value = Integer.parseInt(parameter.get("value").toString());
		minimum = Integer.parseInt(parameter.get("minimum").toString());
		maximum = Integer.parseInt(parameter.get("maximum").toString());
		increment = Integer.parseInt(parameter.get("increment").toString());
		if (increment == 0)
			increment = 1;
	}

	public void incresseValue() {
		setValue(value + increment); 
		field.setText(getValueString()); 
	}

	public void decreaseValue() {
		setValue(value - increment); 
		field.setText(getValueString()); 
	}

	public void setValue(int nvalue) {
		if(nvalue >= minimum && nvalue <= maximum) { 
			value = nvalue; 
		}
	}

	public int getValue() {
		return value; 
	}

	public String getValueString() {
		return Integer.toString(value); 
	}
}
