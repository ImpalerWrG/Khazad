/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package Nifty;

import Core.Main;
import Game.Citizen;
import Game.Game;
import de.lessvoid.nifty.Nifty;
import de.lessvoid.nifty.NiftyIdCreator;
import de.lessvoid.nifty.controls.Controller;
import de.lessvoid.nifty.controls.Label;
import de.lessvoid.nifty.controls.ScrollPanel;
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
	Element citizenTableBody;
	ScrollPanel citizenScrollPanel;
	Label idleCitizensLabel;

	public void bind(Nifty nifty, Screen screen, Element element, Properties parameter, Attributes controlDefinitionAttributes) {
		this.nifty = nifty;
		this.screen = screen;
		this.popup = element;
		this.citizenTableBody = popup.findElementByName("CitizenTableBody");
		this.citizenScrollPanel = popup.findNiftyControl("CitizenScrollPanel", ScrollPanel.class);
		this.idleCitizensLabel = popup.findNiftyControl("IdleCitizensLabel", Label.class);		
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
		setUpScrollPanel(0, 30, 0, citizenScrollPanel.getHeight() - 30);
		idleCitizensLabel.setText("Idle citizens: " + game.GameSettlement.getJobManager().getIdleCitizens().size());
	}
	
	private void setUpScrollPanel(float stepSizeX, float stepSizeY, float pageSizeX, float pageSizeY) {
		citizenScrollPanel.setUp(stepSizeX, stepSizeY, pageSizeX, pageSizeY, ScrollPanel.AutoScroll.OFF);	
	}
			

	public void addChooseCitizenControl(Citizen citizen) {
		CustomControlCreator chooseCitizenControlCreator = new CustomControlCreator(NiftyIdCreator.generate(), "ChooseCitizenControl");
		Element chooseCitizenControl = chooseCitizenControlCreator.create(nifty, screen, citizenTableBody);
		ChooseCitizenControlController controller = chooseCitizenControl.getControl(ChooseCitizenControlController.class);
		controller.setCitizen(citizen);
	}
}
