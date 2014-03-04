/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package Game;

import com.jme3.app.Application;
import com.jme3.app.SimpleApplication;

import java.util.ArrayList;
import java.util.HashMap;

import com.jme3.app.state.AbstractAppState;
import com.jme3.app.state.AppStateManager;
import com.jme3.font.BitmapFont;
import com.jme3.font.BitmapText;

import com.jme3.input.InputManager;
import com.jme3.input.KeyInput;
import com.jme3.input.controls.ActionListener;
import com.jme3.input.controls.KeyTrigger;

import com.jme3.math.ColorRGBA;

/**
 *
 * @author Impaler
 */
public class TemporalManager extends AbstractAppState implements ActionListener{
	
	class Bucket {
		ArrayList<Temporal> Contents;
		void add(Temporal NewTemporal)
		{
			Contents.add(NewTemporal);
		}
	}

	class Carrosel {
	
		ArrayList<Bucket> Buckets;
		int BucketCount;
		
		Carrosel(int CarroselSize) {
			BucketCount = CarroselSize;         // Saved for destructor
			Buckets = new ArrayList<Bucket>(CarroselSize);

			for (Bucket target: Buckets) {
				target = new Bucket();
			}
		}
	}

	private static TemporalManager Singleton = null;
	private SimpleApplication app;
	private AppStateManager state;

	BitmapText hudText;

	boolean Pause;
	int TickRate;
	long CurrentGameTick;
	float TickRounding;
	HashMap<Integer, Carrosel> CarroselMap;

	ArrayList<Temporal> ReIndexedTemporalBuffer;
	ArrayList<Integer> ReIndexedTemporalCoolDown;
	ArrayList<Temporal> RetiredTemporalBuffer;

	int UniqueIDCounter;

	ArrayList<Temporal> TemporaryTemps; // Temporary Hack
	
	private TemporalManager() {
		TickRate = 100;
		TickRounding = 0;
		CurrentGameTick = 0;
		UniqueIDCounter = 0;
		Pause = true;
		
		TemporaryTemps = new ArrayList<Temporal>();
	}

	public static TemporalManager getSingleton() {
		if (Singleton == null)
			Singleton = new TemporalManager();
			
		return Singleton;
	}

	@Override
    public void initialize(AppStateManager stateManager, Application app) {
        super.initialize(stateManager, app);
        this.app = (SimpleApplication) app;
		this.state = stateManager;
		
		BuildText();
		registerWithInput(app.getInputManager());
	}

	public void onAction(String name, boolean keyPressed, float tpf) {
        if (this.isEnabled()) {
			if (name.equals("Pause")) {
				if (keyPressed) {
					Pause = !Pause;
				}
			}
			if (name.equals("Faster")) {
				if (keyPressed) {
					TickRate *= 1.3f;
				}
			}
			if (name.equals("Slower")) {
				if (keyPressed) {
					TickRate /= 1.3f;
				}
			}

		}
	}

	public void registerWithInput(InputManager inputManager) {
        String[] inputs = {"Pause", "Faster", "Slower"};

		inputManager.addMapping("Pause", new KeyTrigger(KeyInput.KEY_SPACE)); 
		inputManager.addMapping("Faster", new KeyTrigger(KeyInput.KEY_ADD)); 
		inputManager.addMapping("Slower", new KeyTrigger(KeyInput.KEY_MINUS)); 
        inputManager.addListener(this, inputs);
	}

	public long getCurrentTimeTick() {
		return CurrentGameTick;
	}

	public void UpdateTick() {
		CurrentGameTick++;   // Advance Tick count
		
		for (Temporal target: TemporaryTemps) {
			target.Wake();
		}
	}
	
	public void changeTickRate(int RateChange) {
		TickRate += RateChange;

		if (TickRate < 1) {
			TickRate = 1;
		}
	}

	public void setTickRate(int NewRate) {
		TickRate = NewRate;
	}

	@Override
    public void update(float tpf) {
		if (!Pause) {
			float TargetTicks = TickRate * tpf;
			TargetTicks += TickRounding;
			int FullTicks = (int) TargetTicks;
			TickRounding = TargetTicks - FullTicks;

			for (int i = 0; i < FullTicks; i++) {
				UpdateTick();
			}
			hudText.setText("Tick" + CurrentGameTick);
		}
    }

	public void BuildText() {
		BitmapFont font = this.app.getAssetManager().loadFont("Interface/Fonts/Default.fnt");
		
		hudText = new BitmapText(font, false);          
		hudText.setSize(font.getCharSet().getRenderedSize());      // font size
		hudText.setColor(ColorRGBA.Blue);                             // font color
		hudText.setText("You can write any string here");             // the text
		hudText.setLocalTranslation(300, hudText.getLineHeight() * 2, 0); // position
		this.app.getGuiNode().attachChild(hudText);
	}

	/*
	void TemporalManager::UpdateTick()
	{
		std::map<CoolDown, Carrosel*>::iterator CarroselIterator;

		for (CarroselIterator = CarroselMap.begin(); CarroselIterator != CarroselMap.end(); CarroselIterator++)
		{
			CoolDown CurrentCoolDown = CarroselIterator->first;        // CoolDown is the size of this Carosel and how long it takes to 'revolve'
			Carrosel* TargetCarosel = CarroselIterator->second;

			Bucket* TargetBucket = TargetCarosel->Buckets[(CurrentGameTick + CurrentCoolDown) % CurrentCoolDown];  // Modulus advances to the next bucket each update
			uint16_t BucketShorteningCounter = 0;

			for (uint32_t i = 0; i < TargetBucket->TemporalVector.size(); i++)
			{
				if (TargetBucket->TemporalVector[i] != NULL)  // should Null be allowed?
				{
					CoolDown DesiredCoolDown = TargetBucket->TemporalVector[i]->Update();  // The amount of Cooldown the Temporal desires untill its next update

					if (DesiredCoolDown != CurrentCoolDown)  // ReIndex the Temporal
					{
						ReIndexedTemporalBuffer.push_back(TargetBucket->TemporalVector[i]);
						ReIndexedTemporalCoolDown.push_back(DesiredCoolDown);

						TargetBucket->TemporalVector[i] = TargetBucket->TemporalVector.back();  i--; //  Move Last Object into the current slot and remain on it for lext loop
						TargetBucket->TemporalVector.back() = NULL;
						BucketShorteningCounter++;
					}
				}
				else
				{
					//BucketShorteningCounter++; // No Nulls should slip through
				}
			}
			TargetBucket->TemporalVector.resize(TargetBucket->TemporalVector.size() - BucketShorteningCounter);  //Shrink the Bucket to save space
		}


		// Extract and Destroy Retired



		// Insert New and Changed
		for (int i = 0; i < ReIndexedTemporalBuffer.size(); i++)
		{
			AddTemporal(ReIndexedTemporalBuffer[i], ReIndexedTemporalCoolDown[i]);
		}

		// Clear the lists now that all Reinsertions are done
		ReIndexedTemporalBuffer.clear();
		ReIndexedTemporalCoolDown.clear();


		CurrentGameTick++;   // Advance Tick count
	}

	Carrosel getCarosel(CoolDown DesiredCoolDown)
	{
		std::map<CoolDown, Carrosel*>::iterator CarroselIterator = CarroselMap.find(DesiredCoolDown);

		if (CarroselIterator != CarroselMap.end())
		{
			return CarroselIterator->second;
		}
		else   // Create and Insert a new Carosel
		{
			Carrosel* NewCarosel = new Carrosel(DesiredCoolDown);
			CarroselMap[DesiredCoolDown] = NewCarosel;
			return NewCarosel;
		}
	}
*/
	boolean AddTemporal(Temporal NewTemporal, int NewCoolDown) {
		/*
		if (NewCoolDown < 1) {
			NewCoolDown = 1;
		}
		Carrosel TargetCarosel = getCarosel(NewCoolDown);
		int Bucket = ((CurrentGameTick + NewCoolDown) % NewCoolDown);   // Ensures small Tick count dose not produce false modulus

		TargetCarosel->Buckets[Bucket]->TemporalVector.push_back(NewTemporal);
		NewTemporal->CurrentBucket = TargetCarosel->Buckets[Bucket];

		* */
		TemporaryTemps.add(NewTemporal);
		return true;
	}

	boolean RetireTemporal(TemporalManager TargetTemporal) {
		//Bucket* TargetBucket = TargetTemporal->CurrentBucket;

		//for
		// TODP, retreave and remove pawn
		return false;
	}

}
