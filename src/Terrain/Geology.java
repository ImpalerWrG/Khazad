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

import Map.Coordinates.*;
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

	int GeologySeed;
	Dice RandomGenerator;
	Noise NoiseGenerator;
	float[][] WorldHeight;

	short ChunkTopZ;
	short ChunkBottomZ;

	float[] Edge;
	float[][] Height;
	boolean[][] Seeded;

	public final short RockType0;
	public final short RockType1;
	public final short RockType2;

	public Geology() {
		DataManager Data = DataManager.getDataManager();

		RockType0 = Data.getLabelIndex("MATERIAL_SOIL");
		RockType1 = Data.getLabelIndex("MATERIAL_SNOW");
		RockType2 = Data.getLabelIndex("MATERIAL_GRANITE");
	}

	public boolean initialize(int Seed) {
		GeologySeed = Seed;

		RandomGenerator = new Dice();
		NoiseGenerator = new Noise();

		return true;
	}

	public void generateWorldHeightMap(int X, int Y) {
		WorldHeight = new float[X + 1][Y + 1];

		Edge = new float[BlockCoordinate.CHUNK_EDGE_SIZE + 1];
		Height = new float[BlockCoordinate.CHUNK_EDGE_SIZE + 1][BlockCoordinate.CHUNK_EDGE_SIZE + 1];
		Seeded = new boolean[BlockCoordinate.CHUNK_EDGE_SIZE + 1][BlockCoordinate.CHUNK_EDGE_SIZE + 1];

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

	public short getChunkBottomZLevel() {
		return ChunkBottomZ;
	}

	public short getChunkTopZLevel() {
		return ChunkTopZ;
	}

	public void generateChunkEdge(float X, float Y, float heightScale, float Roughness) {
		ChunkCoordinate coords = new ChunkCoordinate((int) X, (int) Y, 0);
		coords.hashCode();
		RandomGenerator.seed(coords.hashCode());

		Edge[0] = X;
		Edge[BlockCoordinate.CHUNK_EDGE_SIZE] = Y;

		float ratio = (float) Math.pow(2.0, -Roughness);
		float scale = heightScale * ratio;

		/* Seed the endpoints of the array. To enable seamless wrapping,
		 * the endpoints need to be the same point. */
		short stride = BlockCoordinate.CHUNK_EDGE_SIZE / 2;

		while (stride > 0) {
			for (short i = stride; i < BlockCoordinate.CHUNK_EDGE_SIZE; i += stride) {
				Edge[i] = scale * RandomGenerator.roll(-1.0f, 1.0f) + ((Edge[i - stride] + Edge[i + stride]) * .5f);
				/* reduce random number range */
				scale *= ratio;
				i += stride;
			}
			stride >>= 1;
		}
	}

	public void generateChunkHeight(int X, int Y, float heightScale, float Roughness) {
		byte size = BlockCoordinate.CHUNK_EDGE_SIZE + 1;

		// Mark edges as Seeded to prevent them from being overwritten
		for (short x = 0; x < size; x++) {
			for (short y = 0; y < size; y++) {
				Seeded[x][y] = false;
				//Height[x][y] = 0.0f;
			}
		}

		// Edges Initialized to anchor values to create contiguousness
		generateChunkEdge(WorldHeight[X][Y], WorldHeight[X + 1][Y], heightScale, Roughness);

		for (short x = 0; x < size; x++) {
			Height[x][0] = Edge[x];
			Seeded[x][0] = true;
		}

		generateChunkEdge(WorldHeight[X][Y + 1], WorldHeight[X + 1][Y + 1], heightScale, Roughness);

		for (short x = 0; x < size; x++) {
			Height[x][BlockCoordinate.CHUNK_EDGE_SIZE] = Edge[x];
			Seeded[x][BlockCoordinate.CHUNK_EDGE_SIZE] = true;
		}

		generateChunkEdge(WorldHeight[X][Y], WorldHeight[X][Y + 1], heightScale, Roughness);

		for (short y = 0; y < size; y++) {
			Height[0][y] = Edge[y];
			Seeded[0][y] = true;
		}

		generateChunkEdge(WorldHeight[X + 1][Y], WorldHeight[X + 1][Y + 1], heightScale, Roughness);

		for (short y = 0; y < size; y++) {
			Height[BlockCoordinate.CHUNK_EDGE_SIZE][y] = Edge[y];
			Seeded[BlockCoordinate.CHUNK_EDGE_SIZE][y] = true;
		}

		// Reseed Random Number Generator
		ChunkCoordinate HashChunk = new ChunkCoordinate(X, Y, 0);

		RandomGenerator.seed(GeologySeed);
		int FinalSeed = RandomGenerator.roll(0, 100000) ^ HashChunk.hashCode();

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

		byte stride = BlockCoordinate.CHUNK_EDGE_SIZE / 2;

		Height[0][0] = WorldHeight[X][Y];
		Seeded[0][0] = true;

		Height[BlockCoordinate.CHUNK_EDGE_SIZE][0] = WorldHeight[X + 1][Y];
		Seeded[BlockCoordinate.CHUNK_EDGE_SIZE][0] = true;

		Height[0][BlockCoordinate.CHUNK_EDGE_SIZE] = WorldHeight[X][Y + 1];
		Seeded[0][BlockCoordinate.CHUNK_EDGE_SIZE] = true;

		Height[BlockCoordinate.CHUNK_EDGE_SIZE][BlockCoordinate.CHUNK_EDGE_SIZE] = WorldHeight[X + 1][Y + 1];
		Seeded[BlockCoordinate.CHUNK_EDGE_SIZE][BlockCoordinate.CHUNK_EDGE_SIZE] = true;


		// Set initial Fractal value range (scale) and the rate of decrese (ratio)
		float ratio = (float) Math.pow(2.0, -Roughness);
		float scale = heightScale * ratio;

		/* Now we add ever-increasing detail based on the "diamond" seeded
		 * values. We loop over stride, which gets cut in half at the
		 * bottom of the loop. Since it's an int, eventually division by 2
		 * will produce a zero result, terminating the loop. */

		while (stride > 0) {
			for (short x = stride; x < BlockCoordinate.CHUNK_EDGE_SIZE; x += stride) {
				for (short y = stride; y < BlockCoordinate.CHUNK_EDGE_SIZE; y += stride) {
					if (!Seeded[x][y]) {
						float Average = (float) ((Height[x - stride][y - stride] + Height[x - stride][y + stride] + Height[x + stride][y - stride] + Height[x + stride][y + stride]) * 0.25);
						float RandomFactor = scale * RandomGenerator.roll(-1.0f, 1.0f);

						Height[x][y] = RandomFactor + Average;

						if (((short) Height[x][y]) > ChunkTopZ)
							ChunkTopZ = (short) Height[x][y];
						if (((short) Height[x][y]) < ChunkBottomZ)
							ChunkBottomZ = (short) Height[x][y];
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
			for (short x = 0; x < BlockCoordinate.CHUNK_EDGE_SIZE; x += stride) {
				//oddline = (oddline == 0);
				if (oddline == 0)
					oddline = 1;
				else
					oddline = 0;
				for (short y = 0; y < BlockCoordinate.CHUNK_EDGE_SIZE; y += stride) {
					if ((y == 0) && oddline != 0) {
						y += stride;
					}

					if (!Seeded[x][y]) {
						float Average = (float) ((Height[x - stride][y] + Height[x + stride][y] + Height[x][y - stride] + Height[x][y + stride]) * 0.25);
						float RandomFactor = scale * RandomGenerator.roll(-1.0f, 1.0f);

						Height[x][y] = Average + RandomFactor;

						// Record Maximum and Minimum Height
						if (((short) Height[x][y]) > ChunkTopZ)
							ChunkTopZ = (short) Height[x][y];
						if (((short) Height[x][y]) < ChunkBottomZ)
							ChunkBottomZ = (short) Height[x][y];
					}
					y += stride;
				}
			}

			/* reduce random number range. */
			scale *= ratio;
			stride >>= 1;
		}
	}

	public short getRockTypeAtCoordinates(short Target, int Zlevel) {

		if (Zlevel > 3) {
			return RockType1;
		}
		if (Zlevel > 1) {
			return RockType2;
		}
		return RockType0;
	}

	public BlockShape getBlockShapeAtCoordinates(int West, int East, int South, int North, int Bottom, int Top) {
		float Remainder;
		float BlockSize = Top - Bottom;

		Remainder = Height[West][South] - ((float) Bottom);
		Remainder = Math.round((Remainder / BlockSize) * ((float) BlockShape.HEIGHT_FRACTIONS)) + BlockShape.CUBE_BOTTOM_HEIGHT;
		byte SWCornerHeight = (byte) Math.min(Math.max(Remainder, BlockShape.BELOW_CUBE_HEIGHT), BlockShape.CUBE_TOP_HEIGHT);

		Remainder = Height[East][South] - ((float) Bottom);
		Remainder = Math.round((Remainder / BlockSize) * ((float) BlockShape.HEIGHT_FRACTIONS)) + BlockShape.CUBE_BOTTOM_HEIGHT;
		byte SECornerHeight = (byte) Math.min(Math.max(Remainder, (float) BlockShape.BELOW_CUBE_HEIGHT), (float) BlockShape.CUBE_TOP_HEIGHT);

		Remainder = Height[West][North] - ((float) Bottom);
		Remainder = Math.round((Remainder / BlockSize) * ((float) BlockShape.HEIGHT_FRACTIONS)) + BlockShape.CUBE_BOTTOM_HEIGHT;
		byte NWCornerHeight = (byte) Math.min(Math.max(Remainder, (float) BlockShape.BELOW_CUBE_HEIGHT), (float) BlockShape.CUBE_TOP_HEIGHT);

		Remainder = Height[East][North] - ((float) Bottom);
		Remainder = Math.round((Remainder / BlockSize) * ((float) BlockShape.HEIGHT_FRACTIONS)) + BlockShape.CUBE_BOTTOM_HEIGHT;
		byte NECornerHeight = (byte) Math.min(Math.max(Remainder, (float) BlockShape.BELOW_CUBE_HEIGHT), (float) BlockShape.CUBE_TOP_HEIGHT);

		byte Split = (byte) RandomGenerator.roll(0, 1);

		if (NWCornerHeight == BlockShape.CUBE_TOP_HEIGHT && SECornerHeight == BlockShape.CUBE_TOP_HEIGHT) {
			Split = 1;
		}

		if (SWCornerHeight == BlockShape.CUBE_TOP_HEIGHT && NECornerHeight == BlockShape.CUBE_TOP_HEIGHT) {
			Split = 0;
		}

		if (SWCornerHeight == BlockShape.BELOW_CUBE_HEIGHT || NECornerHeight == BlockShape.BELOW_CUBE_HEIGHT) {
			Split = 1;
		}

		if (SECornerHeight == BlockShape.BELOW_CUBE_HEIGHT || NWCornerHeight == BlockShape.BELOW_CUBE_HEIGHT) {
			Split = 0;
		}

		return new BlockShape(SWCornerHeight, SECornerHeight, NWCornerHeight, NECornerHeight, (byte) Split);
	}

	public void loadChunkData(Chunk TargetChunk) {
		ChunkCoordinate ChunkCoords = TargetChunk.getChunkCoordinates();
		for (byte DetailLevel = 0; DetailLevel < BlockCoordinate.CHUNK_DETAIL_LEVELS; DetailLevel++) {
			for (BlockCoordinate Index = new BlockCoordinate(DetailLevel); !Index.end(); Index.next()) {
				int Offset = 1 << DetailLevel;
				short x = (short) (Index.getX() * Offset);
				short y = (short) (Index.getY() * Offset);
				short z = (short) ((Index.getZ() * Offset) + (ChunkCoords.Z * BlockCoordinate.CHUNK_EDGE_SIZE));

				BlockShape Shape = getBlockShapeAtCoordinates(x, x + Offset, y, y + Offset, z, z + Offset);
				short MaterialType = getRockTypeAtCoordinates(Index.getBlockIndex(), z);
				if (MaterialType != DataManager.INVALID_INDEX) {
					TargetChunk.setBlockMaterial(Index, MaterialType);	
					TargetChunk.setBlockShape(Index, Shape);
				} else {
					TargetChunk.setBlockMaterial(Index, DataManager.INVALID_INDEX);
					TargetChunk.setBlockShape(Index, new BlockShape());
				}
			}
		}
	}
}
