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

package Terrain;

import Core.Dice;
import Map.*;
import Data.DataManager;
import java.io.Serializable;

/**
 *
 * @author Impaler
 */
public class Geology implements Serializable {
	private static final long serialVersionUID = 1;

	int RockType = 42;
	int GeologySeed;

	Dice RandomGenerator;
	Noise NoiseGenerator;

	float[][] WorldHeight;

	short CellTopZ;
	short CellBottomZ;

	float[] Edge;
	float[][] Height;
	boolean[][] Seeded;

	public boolean Initialize(int Seed) {

		//Ogre::Plane RockLayer = Ogre::Plane(Ogre::Vector3(0, 0, 1), 1);
		//LayerBoundries.push_back(make_pair(RockLayer, RockType));
		GeologySeed = Seed;

		RandomGenerator = new Dice();
		NoiseGenerator = new Noise();

		return true;
	}

	public void GenerateWorldHeightMap(int X, int Y) {
		int WorldSizeX = X;
		int WorldSizeY = Y;

		WorldHeight = new float[X + 1][Y + 1];

		Edge = new float[MapCoordinate.CELLEDGESIZE + 1];
		Height = new float[MapCoordinate.CELLEDGESIZE + 1][MapCoordinate.CELLEDGESIZE + 1];
		Seeded = new boolean[MapCoordinate.CELLEDGESIZE + 1][MapCoordinate.CELLEDGESIZE + 1];

		int Octives = 9;
		double lacunarity = 2.1379201;
		double persistence = 0.036281;

		float height;

		for (int x = 0; x < X + 1; x++) {
			for (int y = 0; y < Y + 1; y++) {

				double workingX = x;
				double workingY = y;
				double result = 0.0;

				for (int i = 0; i < Octives; i++) {

					double SpectralWeight = Math.pow(lacunarity, -persistence * i);
					result += NoiseGenerator.noise(workingX, workingY) * SpectralWeight;

					workingX *= lacunarity;
					workingY *= lacunarity;
				}
				WorldHeight[x][y] = (float) result;
			}
		}

		/* double PerlinNoise::Total(double i, double j) const
		{
			//properties of one octave (changing each loop)
			double t = 0.0f;
			double _amplitude = 1;
			double freq = frequency;

			for(int k = 0; k < octaves; k++) {
				t += GetValue(j * freq + randomseed, i * freq + randomseed) * _amplitude;
				_amplitude *= persistence;
				freq *= 2;
			}

			return t;
		}*/
	}

	public short getCellBottomZLevel() {
		return CellBottomZ;
	}

	public short getCellTopZLevel() {
		return CellTopZ;
	}

	public void GenerateCellEdge(float X, float Y, float heightScale, float Roughness) {
		CellCoordinate coords = new CellCoordinate((int) X, (int) Y, 0);
		coords.hashCode();
		RandomGenerator.Seed(coords.hashCode());

		Edge[0] = X;
		Edge[MapCoordinate.CELLEDGESIZE] = Y;

		float ratio = (float) Math.pow(2.0, -Roughness);
		float scale = heightScale * ratio;

		/* Seed the endpoints of the array. To enable seamless wrapping,
		 the endpoints need to be the same point. */
		byte stride = MapCoordinate.CELLEDGESIZE / 2;

		while (stride > 0) {
			for (byte i = stride; i < MapCoordinate.CELLEDGESIZE; i += stride) {
				Edge[i] = scale * RandomGenerator.Roll(-1.0f, 1.0f) + ((Edge[i - stride] + Edge[i + stride]) * .5f);
				/* reduce random number range */
				scale *= ratio;
				i += stride;
			}
			stride >>= 1;
		}
	}

	public void GenerateCellHeight(int X, int Y, float heightScale, float Roughness) {
		byte size = MapCoordinate.CELLEDGESIZE + 1;

		// Mark edges as Seeded to prevent them from being overwritten
		for (short x = 0; x < size; x++) {
			for (short y = 0; y < size; y++) {
				Seeded[x][y] = false;
				//Height[x][y] = 0.0f;
			}
		}

		// Edges Initialized to anchor values to create contiguousness
		GenerateCellEdge(WorldHeight[X][Y], WorldHeight[X + 1][Y], heightScale, Roughness);

		for (short x = 0; x < size; x++) {
			Height[x][0] = Edge[x];
			Seeded[x][0] = true;
		}

		GenerateCellEdge(WorldHeight[X][Y + 1], WorldHeight[X + 1][Y + 1], heightScale, Roughness);

		for (short x = 0; x < size; x++) {
			Height[x][MapCoordinate.CELLEDGESIZE] = Edge[x];
			Seeded[x][MapCoordinate.CELLEDGESIZE] = true;
		}

		GenerateCellEdge(WorldHeight[X][Y], WorldHeight[X][Y + 1], heightScale, Roughness);

		for (short y = 0; y < size; y++) {
			Height[0][y] = Edge[y];
			Seeded[0][y] = true;
		}

		GenerateCellEdge(WorldHeight[X + 1][Y], WorldHeight[X + 1][Y + 1], heightScale, Roughness);

		for (short y = 0; y < size; y++) {
			Height[MapCoordinate.CELLEDGESIZE][y] = Edge[y];
			Seeded[MapCoordinate.CELLEDGESIZE][y] = true;
		}

		// Reseed Random Number Generator
		CellCoordinate HashCell = new CellCoordinate(X, Y, 0);

		RandomGenerator.Seed(GeologySeed);
		int FinalSeed = RandomGenerator.Roll(0, 100000) ^ HashCell.hashCode();

		RandomGenerator.Seed(FinalSeed);


			/* Set up our roughness constants.
			   Random numbers are always generated in the range 0.0 to 1.0.
			   'scale' is multiplied by the randum number.
			   'ratio' is multiplied by 'scale' after each iteration
			   to effectively reduce the randum number range.
			   */

		/* Seed the first four values. For example, in a 4x4 array, we
		   would initialize the data points indicated by '*':

			   *   .   .   .   *

			   .   .   .   .   .

			   .   .   .   .   .

			   .   .   .   .   .

			   *   .   .   .   *

		   In terms of the "diamond-square" algorithm, this gives us
		   "squares".

		   We want the four corners of the array to have the same
		   point. This will allow us to tile the arrays next to each other
		   such that they join seemlessly. */

		byte stride = MapCoordinate.CELLEDGESIZE / 2;

		Height[0][0] = WorldHeight[X][Y];
		Seeded[0][0] = true;

		Height[MapCoordinate.CELLEDGESIZE][0] = WorldHeight[X + 1][Y];
		Seeded[MapCoordinate.CELLEDGESIZE][0] = true;

		Height[0][MapCoordinate.CELLEDGESIZE] = WorldHeight[X][Y + 1];
		Seeded[0][MapCoordinate.CELLEDGESIZE] = true;

		Height[MapCoordinate.CELLEDGESIZE][MapCoordinate.CELLEDGESIZE] = WorldHeight[X + 1][Y + 1];
		Seeded[MapCoordinate.CELLEDGESIZE][MapCoordinate.CELLEDGESIZE] = true;


		// Set initial Fractal value range (scale) and the rate of decrese (ratio)
			float ratio = (float) Math.pow(2.0, -Roughness);
			float scale = heightScale * ratio;

		/* Now we add ever-increasing detail based on the "diamond" seeded
		   values. We loop over stride, which gets cut in half at the
		   bottom of the loop. Since it's an int, eventually division by 2
		   will produce a zero result, terminating the loop. */

		while (stride > 0)
		{
					/* Take the existing "square" data and produce "diamond"
					   data. On the first pass through with a 4x4 matrix, the
					   existing data is shown as "X"s, and we need to generate the
				   "*" now:

				   X   .   .   .   X

				   .   .   .   .   .

				   .   .   *   .   .

				   .   .   .   .   .

				   X   .   .   .   X

				  It doesn't look like diamonds. What it actually is, for the
				  first pass, is the corners of four diamonds meeting at the
				  center of the array. */
					for (short x = stride; x < MapCoordinate.CELLEDGESIZE; x += stride)
					{
							for (short y = stride; y < MapCoordinate.CELLEDGESIZE; y += stride)
							{
								if (!Seeded[x][y])
								{
									float Average = (float) ((Height[x - stride][y - stride] + Height[x - stride][y + stride] + Height[x + stride][y - stride] + Height[x + stride][y + stride]) * 0.25);
									float RandomFactor = scale * RandomGenerator.Roll(-1.0f, 1.0f);

									Height[x][y] = RandomFactor + Average;

									if (((short) Height[x][y]) > CellTopZ)
										CellTopZ = (short) Height[x][y];
									if (((short) Height[x][y]) < CellBottomZ)
										CellBottomZ = (short) Height[x][y];
								}

									y += stride;
							}
							x += stride;
					}

					/* Take the existing "diamond" data and make it into
				   "squares". Back to our 4X4 example: The first time we
				   encounter this code, the existing values are represented by
				   "X"s, and the values we want to generate here are "*"s:

				   X   .   *   .   X

				   .   .   .   .   .

				   *   .   X   .   *

				   .   .   .   .   .

				   X   .   *   .   X

				   i and j represent our (x,y) position in the array. The
				   first value we want to generate is at (i=2,j=0), and we use
				   "oddline" and "stride" to increment j to the desired value.
				   */
					short oddline = 0;
					for (short x = 0; x < MapCoordinate.CELLEDGESIZE; x += stride)
					{
						//oddline = (oddline == 0);
						if (oddline == 0)
							oddline = 1;
						else
							oddline = 0;
						for (short y = 0; y < MapCoordinate.CELLEDGESIZE; y += stride)
						{
							if ((y == 0) && oddline != 0)
							{
								y += stride;
							}

							if (!Seeded[x][y])
							{
								float Average = (float) ((Height[x - stride][y] + Height[x + stride][y] + Height[x][y - stride] + Height[x][y + stride]) * 0.25);
								float RandomFactor = scale * RandomGenerator.Roll(-1.0f, 1.0f);

								Height[x][y] = Average + RandomFactor;

								// Record Maximum and Minimum Height
								if (((short) Height[x][y]) > CellTopZ)
									CellTopZ = (short) Height[x][y];
								if (((short) Height[x][y]) < CellBottomZ)
									CellBottomZ = (short) Height[x][y];
							}
							y += stride;
						}
					}

					/* reduce random number range. */
					scale *= ratio;
					stride >>= 1;
		}
	}

	public short getRockTypeAtCoordinates(byte Target, int Zlevel) {
		DataManager Data = DataManager.getDataManager();
		final short RockType0 = Data.getLabelIndex("MATERIAL_SOIL");
		final short RockType1 = Data.getLabelIndex("MATERIAL_SNOW");
		final short RockType2 = Data.getLabelIndex("MATERIAL_GRANITE");

		if (Zlevel > 3) {
			return RockType1;
		}
		if (Zlevel > 1) {
			return RockType2;
		}
		return RockType0;
	}

	public CubeShape getCubeShapeAtCoordinates(byte CubeTarget, int Zlevel) {
		float Remainder;

		int Target = CubeTarget & 0xFF;

		Remainder = Height[(Target >> MapCoordinate.CELLBITSHIFT)][(Target & MapCoordinate.CELLBITFLAG)] - ((float) Zlevel);
		byte SWCornerHeight = (byte) Math.min(Math.max(Math.round(Remainder * CubeShape.HEIGHT_FRACTIONS) + 1, (float) CubeShape.BELOW_CUBE_HEIGHT), (float) CubeShape.CUBE_TOP_HEIGHT);

		Remainder = Height[(Target >> MapCoordinate.CELLBITSHIFT) + 1][(Target & MapCoordinate.CELLBITFLAG)] - ((float) Zlevel);
		byte SECornerHeight = (byte) Math.min(Math.max(Math.round(Remainder * CubeShape.HEIGHT_FRACTIONS) + 1, (float) CubeShape.BELOW_CUBE_HEIGHT), (float) CubeShape.CUBE_TOP_HEIGHT);

		Remainder = Height[(Target >> MapCoordinate.CELLBITSHIFT)][(Target & MapCoordinate.CELLBITFLAG) + 1] - ((float) Zlevel);
		byte NWCornerHeight = (byte) Math.min(Math.max(Math.round(Remainder * CubeShape.HEIGHT_FRACTIONS) + 1, (float) CubeShape.BELOW_CUBE_HEIGHT), (float) CubeShape.CUBE_TOP_HEIGHT);

		Remainder = Height[(Target >> MapCoordinate.CELLBITSHIFT) + 1][(Target & MapCoordinate.CELLBITFLAG) + 1] - ((float) Zlevel);
		byte NECornerHeight = (byte) Math.min(Math.max(Math.round(Remainder * CubeShape.HEIGHT_FRACTIONS) + 1, (float) CubeShape.BELOW_CUBE_HEIGHT), (float) CubeShape.CUBE_TOP_HEIGHT);

		byte Split = (byte) RandomGenerator.Roll(0, 1);

		if (NWCornerHeight == CubeShape.CUBE_TOP_HEIGHT && SECornerHeight == CubeShape.CUBE_TOP_HEIGHT) {
			Split = 1;
		}

		if (SWCornerHeight == CubeShape.CUBE_TOP_HEIGHT && NECornerHeight == CubeShape.CUBE_TOP_HEIGHT) {
			Split = 0;
		}

		if (SWCornerHeight == CubeShape.BELOW_CUBE_HEIGHT || NECornerHeight == CubeShape.BELOW_CUBE_HEIGHT) {
			Split = 1;
		}

		if (SECornerHeight == CubeShape.BELOW_CUBE_HEIGHT || NWCornerHeight == CubeShape.BELOW_CUBE_HEIGHT) {
			Split = 0;
		}

		return new CubeShape(SWCornerHeight, SECornerHeight, NWCornerHeight, NECornerHeight, (byte) Split);
	}

	public void LoadCellData(Cell TargetCell) {
		CellCoordinate TargetCoordinates = TargetCell.getCellCoordinates();

		byte TargetCube = 0;
		do {
			CubeShape Shape = getCubeShapeAtCoordinates(TargetCube, TargetCoordinates.Z);

			if (!Shape.isEmpty()) {
				short MaterialType = getRockTypeAtCoordinates(TargetCube, TargetCoordinates.Z);
				TargetCell.setCubeMaterial(TargetCube, MaterialType);

				if (MaterialType != DataManager.INVALID_INDEX) {
					TargetCell.setCubeShape(TargetCube, Shape);
					//TargetCell->setCubeSurface(TargetCubeCoordinates, RoughWallID);
				}
			} else {
				TargetCell.setCubeShape(TargetCube, Shape);
				TargetCell.setCubeMaterial(TargetCube, DataManager.INVALID_INDEX);
			}
			TargetCube++;
		} while (TargetCube != 0);  // End Loop when Byte rolls over
	}
}
