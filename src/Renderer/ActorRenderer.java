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

package Renderer;

import Game.Actor;
import Game.Game;
import Game.Pawn;

import Map.CubeShape;
import Map.Direction;
import Map.GameMap;
import Map.MapCoordinate;

import com.jme3.app.Application;
import com.jme3.app.SimpleApplication;
import com.jme3.app.state.AbstractAppState;
import com.jme3.app.state.AppStateManager;
import com.jme3.asset.AssetManager;

import com.jme3.math.FastMath;
import com.jme3.math.Quaternion;
import com.jme3.math.Vector3f;

import com.jme3.scene.Node;
import com.jme3.scene.Spatial;
import com.jme3.scene.control.LodControl;

import java.util.ArrayList;
import java.util.concurrent.ConcurrentHashMap;

/**
 * Rendering class for Actors, used Nodes from the Terrain rendered to attach
 * Actors to the correct locations for rendering and lighting.
 *
 * @author Impaler
 */
public class ActorRenderer extends AbstractAppState {

	SimpleApplication app = null;
	AppStateManager state = null;
	AssetManager assetmanager = null;
	LodControl ActorLodControler;
	ConcurrentHashMap<Integer, Node> ActorNodeMap;
	boolean DisplayToggle = true;

	public ActorRenderer() {
		ActorNodeMap = new ConcurrentHashMap<Integer, Node>();
	}

	@Override
	public void initialize(AppStateManager stateManager, Application app) {
		super.initialize(stateManager, app);
		this.app = (SimpleApplication) app;
		this.state = stateManager;
		this.assetmanager = app.getAssetManager();

		//registerWithInput(app.getInputManager());
	}

	public void populateActors() {
		Game game = state.getState(Game.class);
		GameMap map = game.getMap();

		long CurrentTick = game.getCurrentTimeTick();

		ArrayList<Actor> actors = game.getActors();
		for (Actor target : actors) {
			if (target != null) {
				if (target.isDirty()) {
					Node actorNode = ActorNodeMap.get(target.getID());
					if (actorNode == null) {

						//Geometry = new Sphere();
						Spatial actorModel = assetmanager.loadModel("Models/Dwarf/Dwarf.j3o");
						actorModel.scale(0.25f, 0.25f, 0.25f);
						actorModel.rotate(1.5f, 0.0f, 0.0f);

						actorNode = new Node("ActorNode");
						actorNode.attachChild(actorModel);
						ActorNodeMap.put(new Integer(target.getID()), actorNode);
					}

					actorNode.setCullHint(Spatial.CullHint.Dynamic);
					MapCoordinate coords = target.getLocation();
					MapRenderer Renderer = state.getState(MapRenderer.class);
					Node z = Renderer.getZNodeLight(coords.Z);
					z.attachChild(actorNode);
					Vector3f Offset = new Vector3f();

					if (target instanceof Pawn) {
						Pawn PawnTarget = (Pawn) target;
						float MoveFraction = PawnTarget.getActionFraction(CurrentTick);
						Direction MovingDirection = PawnTarget.getMovementDirection();
						float Height = 0;

						if (MoveFraction <= 0.5) {
							CubeShape shape = map.getCubeShape(coords);
							float CenterHeight = shape.getCenterHeight();
							float EdgeHeight = shape.getDirectionEdgeHeight(MovingDirection);
							float CenterFraction = (MoveFraction * 2.0f);
							float EdgeFraction = 1.0f - CenterFraction;
							Height = (CenterHeight * CenterFraction) + (EdgeHeight * EdgeFraction);
						}

						if (MoveFraction > 0.5) {
							if (MoveFraction >= 1.0) {
								MoveFraction = 0;
							}

							MapCoordinate translated = new MapCoordinate(coords, MovingDirection);
							CubeShape shape = map.getCubeShape(translated);
							float CenterHeight = shape.getCenterHeight() + (translated.Z - coords.Z);
							float EdgeHeight = shape.getDirectionEdgeHeight(MovingDirection.invert()) + (translated.Z - coords.Z);
							float CenterFraction = ((MoveFraction - 0.5f) * 2.0f);
							float EdgeFraction = 1.0f - CenterFraction;
							Height = (CenterHeight * CenterFraction) + (EdgeHeight * EdgeFraction);
						}

						if (MovingDirection == Direction.DIRECTION_DESTINATION) {
							MoveFraction = 0;
						}

						Vector3f MoveVec = MovingDirection.toVector();
						Offset.addLocal(MoveVec.mult(MoveFraction));

						Quaternion rotation = actorNode.getLocalRotation();
						rotation.fromAngleAxis(MovingDirection.toDegree() * FastMath.DEG_TO_RAD, Vector3f.UNIT_Z);
						actorNode.setLocalRotation(rotation);

						actorNode.setLocalTranslation(coords.X + Offset.x, coords.Y + Offset.y, Height);

					} else {
						actorNode.setLocalTranslation(coords.X, coords.Y, 0);
					}
				}
			}
		}
	}

	public void hideActors() {
		for (Node target : ActorNodeMap.values()) {
			target.setCullHint(Spatial.CullHint.Always);
		}
	}

	@Override
	public void update(float tpf) {
		Game game = state.getState(Game.class);
		if (game != null) {
			GameMap map = game.getMap();

			if (game.getTickRate() <= 256) {
				populateActors();
			} else {
				hideActors();
			}
		}
	}
}
