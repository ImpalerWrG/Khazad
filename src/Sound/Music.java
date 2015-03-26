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

package Sound;

import Core.Dice;
import com.jme3.app.Application;
import com.jme3.app.SimpleApplication;
import com.jme3.app.state.AbstractAppState;
import com.jme3.app.state.AppStateManager;
import com.jme3.asset.AssetManager;
import com.jme3.audio.AudioNode;
import com.jme3.audio.AudioSource;
import java.io.Serializable;
import java.util.ArrayList;

/**
 *
 * @author Impaler
 */
public class Music extends AbstractAppState implements Serializable {
	
	ArrayList<String> Tracks = null;
	transient SimpleApplication app = null;
	AssetManager assets = null;
	
	Dice RandomGenerator = null;
	private AudioNode audio_music;
	float Volume = 0.15f;
	
	public Music() {
		
	}
	
	@Override
    public void initialize(AppStateManager stateManager, Application app) {
		super.initialize(stateManager, app);
        this.app = (SimpleApplication) app;
		this.assets = app.getAssetManager();
		
		RandomGenerator = new Dice();
		RandomGenerator.Seed(42);

		Tracks = new ArrayList<String>();
		LoadTrackLocations();
		PlayNewTrack();
	}

	public void LoadTrackLocations() {
		Tracks.add("Sounds/Music/The Fall of Arcana.ogg");
		Tracks.add("Sounds/Music/Stepping Stones.ogg");
		Tracks.add("Sounds/Music/Around the Mountain.ogg");
		//Tracks.add("Sounds/Music/Crystal Cave.ogg");
		Tracks.add("Sounds/Music/Dark Ambience.ogg");
		Tracks.add("Sounds/Music/Second Organ.ogg");
	}

	public String SelectTrack() {
		String Track = new String();
		int choice = RandomGenerator.Roll(0, Tracks.size() - 1);
		return Tracks.get(choice);
	}

	public void PlayNewTrack() {
		audio_music = new AudioNode(assets, SelectTrack(), false);
		audio_music.setPositional(false); 
		audio_music.setVolume(Volume);
		
		this.app.getRootNode().attachChild(audio_music);
		audio_music.play();
	}

	@Override
    public void update(float tpf) {
		if (audio_music.getStatus() == AudioSource.Status.Stopped) {
			PlayNewTrack();
		}
    }
}
