/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package Nifty;

import Core.Main;
import Game.Citizen;
import Game.Game;
import de.lessvoid.nifty.Nifty;
import de.lessvoid.nifty.controls.Controller;
import de.lessvoid.nifty.controls.NiftyControl;
import de.lessvoid.nifty.controls.dynamic.CustomControlCreator;
import de.lessvoid.nifty.elements.Element;
import de.lessvoid.nifty.input.NiftyInputEvent;
import de.lessvoid.nifty.screen.Screen;
import de.lessvoid.xml.xpp3.Attributes;
import java.util.Properties;

/**
 *
 * @author Dallas
 */
public class PopulationPopupController implements Controller {

	Nifty nifty;
	Screen screen;
	Element popup;
	Element citizenPanel;

	public void bind(Nifty nifty, Screen screen, Element element, Properties parameter, Attributes controlDefinitionAttributes) {
		this.nifty = nifty;
		this.screen = screen;
		this.popup = element;
		this.citizenPanel = popup.findElementByName("CitizenPanel");
		refreshPopulation();
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

	public void refreshPopulation() {
		Game game = Main.app.getStateManager().getState(Game.class);
		for (Citizen citizen : game.GameSettlement.Citizens) {
			addChooseCitizenControl(citizen);
		}
	}

	public void addChooseCitizenControl(Citizen citizen) {
		String citizenId = String.valueOf(citizen.getID());
		CustomControlCreator chooseCitizenControlCreator = new CustomControlCreator(citizenId, "ChooseCitizenControl");
		chooseCitizenControlCreator.parameter("citizenId", citizenId);
		Element chooseCitizenControl = chooseCitizenControlCreator.create(nifty, screen, citizenPanel);
	}
}
