/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package Map.Coordinates;

import java.io.Serializable;

/**
 *
 * @author Impaler
 */
public class BlockCoordinate implements Serializable {

	public static final float HALF_BLOCK = (float) 0.5;

	public static final int CHUNK_EDGE_SIZE = 32;
	public static final int BLOCK_BITMASK = 31;

	public static final int BLOCK_BITSHIFT_X = 2;
	public static final int BLOCK_BITSHIFT_Y = 1;
	public static final int BLOCK_BITSHIFT_Z = 0;
	
	public static final int BLOCKS_PER_CHUNK = 32768;
	public static final int CHUNK_DETAIL_LEVELS = 6;

	public short Data;  // Index bitpacking   0 YYYYY XXXXX ZZZZZ

	public byte DetailLevel;
	public short Size;
	public short Max;
	public short Mask;
	public short Shift;

	public BlockCoordinate() {
		this.DetailLevel = 0;
		setDetailLevel(DetailLevel);
	}

	public BlockCoordinate(byte DetailLevel) {
		this.DetailLevel = (byte) DetailLevel;
		setDetailLevel(DetailLevel);
	}

	public BlockCoordinate(byte DetailLevel, short Data) {
		this.Data = Data;
		setDetailLevel(DetailLevel);
	}

	public BlockCoordinate(BlockCoordinate CopySource) {
		this.Data = CopySource.Data;
		setDetailLevel(CopySource.DetailLevel);
	}

	public final void setDetailLevel(int DetailLevel) {
		this.DetailLevel = (byte) DetailLevel;
		this.Shift = (short) ((BlockCoordinate.CHUNK_DETAIL_LEVELS - DetailLevel) - 1);
		this.Size = (short) (1 << this.Shift);
		
		this.Mask = (short) (this.Size - 1);
		short Xcomponent = (short) (Mask << (this.Shift * BLOCK_BITSHIFT_X));
		short Ycomponent = (short) (Mask << (this.Shift * BLOCK_BITSHIFT_Y));
		short Zcomponent = (short) (Mask << (this.Shift * BLOCK_BITSHIFT_Z));
		
		this.Max = (short) (Xcomponent | Ycomponent | Zcomponent);
	}

	public void translate(Direction DirectionType) {
		short Xcomponent = (short) ((this.Data >> (this.Shift * BLOCK_BITSHIFT_X)) & Mask);
		short Ycomponent = (short) ((this.Data >> (this.Shift * BLOCK_BITSHIFT_Y)) & Mask);
		short Zcomponent = (short) ((this.Data >> (this.Shift * BLOCK_BITSHIFT_Z)) & Mask);

		Xcomponent += DirectionType.getValueonAxis(Axis.AXIS_X);
		Ycomponent += DirectionType.getValueonAxis(Axis.AXIS_Y);
		Zcomponent += DirectionType.getValueonAxis(Axis.AXIS_Z);
		
		if ((Xcomponent / Size) == 0) {
			Xcomponent = 0;
			Ycomponent++;
			if ((Ycomponent / Size) == 0) {
				Ycomponent = 0;
				Zcomponent++;
				if ((Zcomponent / Size) == 0) {
					Zcomponent = 0;
				}
			}	
		}

		Xcomponent = (short) (Xcomponent << (this.Shift * BLOCK_BITSHIFT_X));
		Ycomponent = (short) (Ycomponent << (this.Shift * BLOCK_BITSHIFT_Y));
		Zcomponent = (short) (Zcomponent << (this.Shift * BLOCK_BITSHIFT_Z));

		Data = (short) (Xcomponent | Ycomponent | Zcomponent);
	}

	public void translate(Direction DirectionType, int Length) {
		short Xcomponent = (short) ((this.Data >> (this.Shift * BLOCK_BITSHIFT_X)) & Mask);
		short Ycomponent = (short) ((this.Data >> (this.Shift * BLOCK_BITSHIFT_Y)) & Mask);
		short Zcomponent = (short) ((this.Data >> (this.Shift * BLOCK_BITSHIFT_Z)) & Mask);

		Xcomponent += DirectionType.getValueonAxis(Axis.AXIS_X) * Length;
		Ycomponent += DirectionType.getValueonAxis(Axis.AXIS_Y) * Length;
		Zcomponent += DirectionType.getValueonAxis(Axis.AXIS_Z) * Length;

		Xcomponent %= this.Size;
		Ycomponent %= this.Size;
		Zcomponent %= this.Size;
		
		Xcomponent = (short) (Xcomponent << (this.Shift * BLOCK_BITSHIFT_X));
		Ycomponent = (short) (Ycomponent << (this.Shift * BLOCK_BITSHIFT_Y));
		Zcomponent = (short) (Zcomponent << (this.Shift * BLOCK_BITSHIFT_Z));

		Data = (short) (Xcomponent | Ycomponent | Zcomponent);
	}

	public void set(short Data) {
		this.Data = Data;
	}

	public void set(int NewX, int NewY, int NewZ) {
		short Xcomponent = (short) (NewX & Mask);
		short Ycomponent = (short) (NewY & Mask);
		short Zcomponent = (short) (NewZ & Mask);

		Xcomponent = (short) (Xcomponent << (this.Shift * BLOCK_BITSHIFT_X));
		Ycomponent = (short) (Ycomponent << (this.Shift * BLOCK_BITSHIFT_Y));
		Zcomponent = (short) (Zcomponent << (this.Shift * BLOCK_BITSHIFT_Z));

		Data = (short) (Xcomponent | Ycomponent | Zcomponent);
	}

	public void set(Axis AxialComponent, int NewValue) {
		short Xcomponent = (short) ((this.Data >> (this.Shift * BLOCK_BITSHIFT_X)) & Mask);
		short Ycomponent = (short) ((this.Data >> (this.Shift * BLOCK_BITSHIFT_Y)) & Mask);
		short Zcomponent = (short) ((this.Data >> (this.Shift * BLOCK_BITSHIFT_Z)) & Mask);

		switch (AxialComponent) {
			case AXIS_Z:
				Zcomponent = (short) (NewValue & Mask);
				break;
			case AXIS_Y:
				Ycomponent = (short) (NewValue & Mask);
				break;
			case AXIS_X:
				Xcomponent = (short) (NewValue & Mask);
				break;

			default:
				break;
		}
		Xcomponent = (short) (Xcomponent << (this.Shift * BLOCK_BITSHIFT_X));
		Ycomponent = (short) (Ycomponent << (this.Shift * BLOCK_BITSHIFT_Y));
		Zcomponent = (short) (Zcomponent << (this.Shift * BLOCK_BITSHIFT_Z));

		Data = (short) (Xcomponent | Ycomponent | Zcomponent);		
	}

	public short getBlockIndex() {
		return Data;
	}
	
	public short getX() {
		return (short) ((this.Data >> (this.Shift * BLOCK_BITSHIFT_X)) & Mask);
	}

	public short getY() {
		return (short) ((this.Data >> (this.Shift * BLOCK_BITSHIFT_Y)) & Mask);
	}

	public short getZ() {
		return (short) ((this.Data >> (this.Shift * BLOCK_BITSHIFT_Z)) & Mask);
	}

	public short getXY() {
		return (short) (this.Data >> (this.Shift * (BLOCK_BITSHIFT_Z + 1)));
	}

	public void next() {
		Data++;
	}

	public void skipAlongAxis(Axis SkippingAxis) {
		short Xcomponent = (short) ((this.Data >> (this.Shift * BLOCK_BITSHIFT_X)) & Mask);
		short Ycomponent = (short) ((this.Data >> (this.Shift * BLOCK_BITSHIFT_Y)) & Mask);
		short Zcomponent = (short) ((this.Data >> (this.Shift * BLOCK_BITSHIFT_Z)) & Mask);

		short Skip = 0;
		switch (SkippingAxis) {
			case AXIS_X:
				Skip = (short) ((Size - Xcomponent) << (this.Shift * BLOCK_BITSHIFT_X));
				break;

			case AXIS_Y:
				Skip = (short) ((Size - Ycomponent) << (this.Shift * BLOCK_BITSHIFT_Y));
				break;

			case AXIS_Z:
				Skip = (short) ((Size - Zcomponent) << (this.Shift * BLOCK_BITSHIFT_Z));
				break;
		}
		Data += Skip;
	}

	public boolean end() {
		return (Data > this.Max || Data < 0);
	}

	public void copy(BlockCoordinate ArgumentCoordinates) {
		this.Data = ArgumentCoordinates.Data;
		setDetailLevel(ArgumentCoordinates.DetailLevel);
	}

	@Override
	public BlockCoordinate clone() {
		return new BlockCoordinate(DetailLevel, Data);
	}

	@Override
	public boolean equals(Object ArgumentCoordinates) {

		//if (ArgumentCoordinates == null)
		//return false;
		//if (ArgumentCoordinates == this)
		//return true;
		//if (!(ArgumentCoordinates instanceof MapCoordinate))
		//return false;

		BlockCoordinate Arg = (BlockCoordinate) ArgumentCoordinates;
		return (Arg.Data == this.Data && Arg.DetailLevel == this.DetailLevel);
	}

	public int getValueonAxis(Axis AxialComponent) {
		switch (AxialComponent) {
			case AXIS_X:
				return (short) ((this.Data >> (this.Shift * BLOCK_BITSHIFT_X)) & Mask);
			case AXIS_Y:
				return (short) ((this.Data >> (this.Shift * BLOCK_BITSHIFT_Y)) & Mask);
			case AXIS_Z:
				return (short) ((this.Data >> (this.Shift * BLOCK_BITSHIFT_Z)) & Mask);
			default:
				return 0;
		}
	}

	@Override
	public int hashCode() {
		//int hash = 3;
		//hash += 17 * X;
		//hash += 37 * Y;
		//hash += 5 * Z;
		return Data;
	}
}
