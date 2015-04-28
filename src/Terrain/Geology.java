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

	public boolean initialize(int Seed) {

		//Ogre::Plane RockLayer = Ogre::Plane(Ogre::Vector3(0, 0, 1), 1);
		//LayerBoundries.push_back(make_pair(RockLayer, RockType));
		GeologySeed = Seed;

		RandomGenerator = new Dice();
		NoiseGenerator = new Noise();

		return true;
	}

	public void generateWorldHeightMap(int X, int Y) {
		WorldHeight = new float[X + 1][Y + 1];

		Edge = new float[CubeCoordinate.CELLEDGESIZE + 1];
		Height = new float[CubeCoordinate.CELLEDGESIZE + 1][CubeCoordinate.CELLEDGESIZE + 1];
		Seeded = new boolean[CubeCoordinate.CELLEDGESIZE + 1][CubeCoordinate.CELLEDGESIZE + 1];

		int Octives = 9;
		double lacunarity = 2.1379201;
		double persistence = 0.036281;


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
		 * {
		 * //properties of one octave (changing each loop)
		 * double t = 0.0f;
		 * double _amplitude = 1;
		 * double freq = frequency;
		 *
		 * for(int k = 0; k < octaves; k++) {
		 * t += GetValue(j * freq + randomseed, i * freq + randomseed) * _amplitude;
		 * _amplitude *= persistence;
		 * freq *= 2;
		 * }
		 *
		 * return t;
		 * } */
	}

	public short getCellBottomZLevel() {
		return CellBottomZ;
	}

	public short getCellTopZLevel() {
		return CellTopZ;
	}

	public void generateCellEdge(float X, float Y, float heightScale, float Roughness) {
		CellCoordinate coords = new CellCoordinate((int) X, (int) Y, 0);
		coords.hashCode();
		RandomGenerator.seed(coords.hashCode());

		Edge[0] = X;
		Edge[CubeCoordinate.CELLEDGESIZE] = Y;

		float ratio = (float) Math.pow(2.0, -Roughness);
		float scale = heightScale * ratio;

		/* Seed the endpoints of the array. To enable seamless wrapping,
		 * the endpoints need to be the same point. */
		byte stride = CubeCoordinate.CELLEDGESIZE / 2;

		while (stride > 0) {
			for (byte i = stride; i < CubeCoordinate.CELLEDGESIZE; i += stride) {
				Edge[i] = scale * RandomGenerator.roll(-1.0f, 1.0f) + ((Edge[i - stride] + Edge[i + stride]) * .5f);
				/* reduce random number range */
				scale *= ratio;
				i += stride;
			}
			stride >>= 1;
		}
	}

	public void generateCellHeight(int X, int Y, float heightScale, float Roughness) {
		byte size = CubeCoordinate.CELLEDGESIZE + 1;

		// Mark edges as Seeded to prevent them from being overwritten
		for (short x = 0; x < size; x++) {
			for (short y = 0; y < size; y++) {
				Seeded[x][y] = false;
				//Height[x][y] = 0.0f;
			}
		}

		// Edges Initialized to anchor values to create contiguousness
		generateCellEdge(WorldHeight[X][Y], WorldHeight[X + 1][Y], heightScale, Roughness);

		for (short x = 0; x < size; x++) {
			Height[x][0] = Edge[x];
			Seeded[x][0] = true;
		}

		generateCellEdge(WorldHeight[X][Y + 1], WorldHeight[X + 1][Y + 1], heightScale, Roughness);

		for (short x = 0; x < size; x++) {
			Height[x][CubeCoordinate.CELLEDGESIZE] = Edge[x];
			Seeded[x][CubeCoordinate.CELLEDGESIZE] = true;
		}

		generateCellEdge(WorldHeight[X][Y], WorldHeight[X][Y + 1], heightScale, Roughness);

		for (short y = 0; y < size; y++) {
			Height[0][y] = Edge[y];
			Seeded[0][y] = true;
		}

		generateCellEdge(WorldHeight[X + 1][Y], WorldHeight[X + 1][Y + 1], heightScale, Roughness);

		for (short y = 0; y < size; y++) {
			Height[CubeCoordinate.CELLEDGESIZE][y] = Edge[y];
			Seeded[CubeCoordinate.CELLEDGESIZE][y] = true;
		}

		// Reseed Random Number Generator
		CellCoordinate HashCell = new CellCoordinate(X, Y, 0);

		RandomGenerator.seed(GeologySeed);
		int FinalSeed = RandomGenerator.roll(0, 100000) ^ HashCell.hashCode();

		RandomGenerator.seed(FinalSeed);


		/* Set up our roughness constants.
		 * Random numbers are always generated in the range 0.0 to 1.0.
		 * 'scale' is multiplied by the randum number.
		 * 'ratio' is multiplied by 'scale' after each iteration
		 * to effectively reduce the randum number range.
		 */

		/* Seed the first four values at the corners
		 *
		 * In terms of the "diamond-square" algorithm, this gives us
		 * "squares".
		 *
		 * We want the four corners of the array to have the same
		 * point. This will allow us to tile the arrays next to each other
		 * such that they join seemlessly. */

		byte stride = CubeCoordinate.CELLEDGESIZE / 2;

		Height[0][0] = WorldHeight[X][Y];
		Seeded[0][0] = true;

		Height[CubeCoordinate.CELLEDGESIZE][0] = WorldHeight[X + 1][Y];
		Seeded[CubeCoordinate.CELLEDGESIZE][0] = true;

		Height[0][CubeCoordinate.CELLEDGESIZE] = WorldHeight[X][Y + 1];
		Seeded[0][CubeCoordinate.CELLEDGESIZE] = true;

		Height[CubeCoordinate.CELLEDGESIZE][CubeCoordinate.CELLEDGESIZE] = WorldHeight[X + 1][Y + 1];
		Seeded[CubeCoordinate.CELLEDGESIZE][CubeCoordinate.CELLEDGESIZE] = true;


		// Set initial Fractal value range (scale) and the rate of decrese (ratio)
		float ratio = (float) Math.pow(2.0, -Roughness);
		float scale = heightScale * ratio;

		/* Now we add ever-increasing detail based on the "diamond" seeded
		 * values. We loop over stride, which gets cut in half at the
		 * bottom of the loop. Since it's an int, eventually division by 2
		 * will produce a zero result, terminating the loop. */

		while (stride > 0) {
			for (short x = stride; x < CubeCoordinate.CELLEDGESIZE; x += stride) {
				for (short y = stride; y < CubeCoordinate.CELLEDGESIZE; y += stride) {
					if (!Seeded[x][y]) {
						float Average = (float) ((Height[x - stride][y - stride] + Height[x - stride][y + stride] + Height[x + stride][y - stride] + Height[x + stride][y + stride]) * 0.25);
						float RandomFactor = scale * RandomGenerator.roll(-1.0f, 1.0f);

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

			/* Take the existing "diamond" data and make it into "squares"
			 * i and j represent our (x,y) position in the array. The
			 * first value we want to generate is at (i=2,j=0), and we use
			 * "oddline" and "stride" to increment j to the desired value.
			 */
			short oddline = 0;
			for (short x = 0; x < CubeCoordinate.CELLEDGESIZE; x += stride) {
				//oddline = (oddline == 0);
				if (oddline == 0)
					oddline = 1;
				else
					oddline = 0;
				for (short y = 0; y < CubeCoordinate.CELLEDGESIZE; y += stride) {
					if ((y == 0) && oddline != 0) {
						y += stride;
					}

					if (!Seeded[x][y]) {
						float Average = (float) ((Height[x - stride][y] + Height[x + stride][y] + Height[x][y - stride] + Height[x][y + stride]) * 0.25);
						float RandomFactor = scale * RandomGenerator.roll(-1.0f, 1.0f);

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

	public CubeShape getCubeShapeAtCoordinates(int West, int East, int South, int North, int Zlevel) {
		float Remainder;

		Remainder = Height[West][South] - ((float) Zlevel);
		byte SWCornerHeight = (byte) Math.min(Math.max(Math.round(Remainder * CubeShape.HEIGHT_FRACTIONS) + 1, (float) CubeShape.BELOW_CUBE_HEIGHT), (float) CubeShape.CUBE_TOP_HEIGHT);

		Remainder = Height[East][South] - ((float) Zlevel);
		byte SECornerHeight = (byte) Math.min(Math.max(Math.round(Remainder * CubeShape.HEIGHT_FRACTIONS) + 1, (float) CubeShape.BELOW_CUBE_HEIGHT), (float) CubeShape.CUBE_TOP_HEIGHT);

		Remainder = Height[West][North] - ((float) Zlevel);
		byte NWCornerHeight = (byte) Math.min(Math.max(Math.round(Remainder * CubeShape.HEIGHT_FRACTIONS) + 1, (float) CubeShape.BELOW_CUBE_HEIGHT), (float) CubeShape.CUBE_TOP_HEIGHT);

		Remainder = Height[East][North] - ((float) Zlevel);
		byte NECornerHeight = (byte) Math.min(Math.max(Math.round(Remainder * CubeShape.HEIGHT_FRACTIONS) + 1, (float) CubeShape.BELOW_CUBE_HEIGHT), (float) CubeShape.CUBE_TOP_HEIGHT);

		byte Split = (byte) RandomGenerator.roll(0, 1);

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

	public void loadCellData(Cell TargetCell) {
		CellCoordinate TargetCoordinates = TargetCell.getCellCoordinates();

		for (int i = 0; i < CubeCoordinate.CELLDETAILLEVELS; i++) {
			int SizeFactor = (CubeCoordinate.CELLDETAILLEVELS - i) - 1;
			int Size = 1 << SizeFactor;
			int Count = Size * Size;
			int Mask = Size - 1;
			for (int j = 0; j < Count; j++) {
				int Row = j >> SizeFactor;
				int Column = j & Mask;
				int Offset = 1 << i;
				
				CubeShape Shape = getCubeShapeAtCoordinates(Row, Row + Offset, Column, Column + Offset, TargetCoordinates.Z);	
				TargetCell.setCubeShape((byte) j, Shape, i);

				if (i == 0) {
					short MaterialType = getRockTypeAtCoordinates((byte) j, TargetCoordinates.Z);
					if (MaterialType != DataManager.INVALID_INDEX) {
						TargetCell.setCubeMaterial((byte) j, MaterialType);	
						TargetCell.setCubeShape((byte) j, Shape, i);
					} else {
						TargetCell.setCubeMaterial((byte) j, DataManager.INVALID_INDEX);
						//TargetCell.setCubeShape((byte) j, CubeShape.EMPTY_CUBE_DATA);
					}
				}
			}
		}
	}
}
