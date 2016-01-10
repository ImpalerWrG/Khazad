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

		Height = new float[BlockCoordinate.CHUNK_EDGE_SIZE + 1][BlockCoordinate.CHUNK_EDGE_SIZE + 1];
		return true;
	}

	public short getChunkBottomZLevel() {
		return ChunkBottomZ;
	}

	public short getChunkTopZLevel() {
		return ChunkTopZ;
	}
	
	public void generateChunk(MapCoordinate targetCoordinates) {
		int X = targetCoordinates.getX() + GeologySeed;
		int Y = targetCoordinates.getY() + GeologySeed;

		int Octives = 4;
		double persistence = 0.4;

		for (int x = 0; x < BlockCoordinate.CHUNK_EDGE_SIZE + 1; x++) {
			for (int y = 0; y < BlockCoordinate.CHUNK_EDGE_SIZE + 1; y++) {

				double workingX = (x + X) / 200f;
				double workingY = (y + Y) / 200f;
				Height[x][y] = (float) OctavePerlin(workingX, workingY, Octives, persistence);
			}
		}
	}

	private double bias(double input, double bias) {
		return input + bias;
	}

	private double SmoothNoise(double x, double y) {
		double corners = ( NoiseGenerator.noise(x-1, y-1) + NoiseGenerator.noise(x+1, y-1) + NoiseGenerator.noise(x-1, y+1) + NoiseGenerator.noise(x+1, y+1) ) / 16;
		double sides   = ( NoiseGenerator.noise(x-1, y) + NoiseGenerator.noise(x+1, y) + NoiseGenerator.noise(x, y-1) + NoiseGenerator.noise(x, y+1) ) /  8;
		double center  =  NoiseGenerator.noise(x, y) / 4;

		return corners + sides + center;
	}

	public double OctavePerlin(double x, double y, int octaves, double persistence) {
		double total = 0;
		double raw;
		double frequency = 1;
		double amplitude = 20;

		for(int i = 0 ; i < octaves; i++) {
			raw = NoiseGenerator.noise(x * frequency, y * frequency);
			total += raw * amplitude;
			persistence *= raw + 1;
			amplitude *= persistence;
			frequency *= 2;
		}

		return total;
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
