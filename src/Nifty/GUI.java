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

import com.jme3.app.Application;
import com.jme3.niftygui.NiftyJmeDisplay;
import de.lessvoid.nifty.Nifty;

import com.jme3.app.state.AbstractAppState;
import de.lessvoid.nifty.controls.Label;

/**
 * Core initialization fo the Nifty GUI system
 * 
 * @author Impaler
 */
public class GUI extends AbstractAppState {

	private Nifty nifty;
	private Application app;

	public GUI(Application app) {
		this.app = app;

		NiftyJmeDisplay niftyDisplay = new NiftyJmeDisplay(app.getAssetManager(), app.getInputManager(), app.getAudioRenderer(), app.getGuiViewPort(), 1000, 1000);
		nifty = niftyDisplay.getNifty();

		//nifty.loadStyleFile("Dwarven.xml");

		nifty.addXml("Interface/Nifty/ChooseCitizenControl.xml");
		nifty.addXml("Interface/Nifty/CitizenWindow.xml");
		nifty.addXml("Interface/Nifty/ErrorPopup.xml");
		nifty.addXml("Interface/Nifty/GameScreenNifty.xml");
		nifty.addXml("Interface/Nifty/LoadScreenNifty.xml");
		nifty.addXml("Interface/Nifty/MenuPopup.xml");
		nifty.addXml("Interface/Nifty/PopulationPopupNifty.xml");
		nifty.addXml("Interface/Nifty/SetupScreenNifty.xml");
		nifty.addXml("Interface/Nifty/StartScreenNifty.xml");
		nifty.addXml("Interface/Nifty/TutorialPopup.xml");

		nifty.gotoScreen("StartScreen");

		app.getGuiViewPort().addProcessor(niftyDisplay);
	}
}
