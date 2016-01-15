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

import Core.Main;

import de.lessvoid.nifty.Nifty;
import de.lessvoid.nifty.tools.SizeValue;
import de.lessvoid.nifty.elements.render.TextRenderer;

import de.lessvoid.nifty.elements.Element;
import de.lessvoid.nifty.input.NiftyInputEvent;
import de.lessvoid.nifty.screen.Screen;
import de.lessvoid.nifty.screen.ScreenController;
import de.lessvoid.xml.xpp3.Attributes;

import java.util.Properties;
import java.util.concurrent.Callable;

/**
 *
 * @author Impaler
 */
public class ProgressBarController implements ScreenController  {

	Nifty nifty;
	private Screen screen;
	private Element bar;
	private TextRenderer textRenderer;
	boolean ready = false;

	public void bind(Nifty nifty, Screen screen) {
		this.nifty = nifty;
		this.screen = screen;
	}

	public void onStartScreen() {
		this.bar = screen.findElementByName("progressbar");
		Element element = screen.findElementByName("loadingtext");
		this.textRenderer = element.getRenderer(TextRenderer.class);

		textRenderer.setText("Creating Game");
		ready = true;
	}

	public void onEndScreen() {
	}

	public void init(Properties parameter, Attributes controlDefinitionAttributes) {
	}

	public void onFocus(boolean getFocus) {
	}

	public boolean inputEvent(NiftyInputEvent inputEvent) {
		return false;
	}

	public void setProgress(final float progress, final String loadingText) {
		//since this method is called from another thread, we enqueue the changes to the progressbar to the update loop thread
		if (ready) {
			Main.getMain().enqueue(new Callable() {

				public Object call() throws Exception {
					final int MIN_WIDTH = 32;
					int pixelWidth = (int) (MIN_WIDTH + (bar.getParent().getWidth() - MIN_WIDTH) * progress);
					bar.setConstraintWidth(new SizeValue(pixelWidth + "px"));
					bar.getParent().layoutElements();

					textRenderer.setText(loadingText);
					return null;
				}
			});
		}
	}
}
