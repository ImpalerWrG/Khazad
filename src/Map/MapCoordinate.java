/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package Map;

/**
 *
 * @author Impaler
 */
public final class MapCoordinate {

	public static final int CELLEDGESIZE = 16;
	public static final int CELLBITSHIFT = 4;
	public static final int CELLBITFLAG = 15;
	public static final int CUBESPERCELL = 256;
	public static final float HALFCUBE = (float)0.5;
	  
	public int X, Y, Z;


	public MapCoordinate() {
		X = Y = Z = 0;
	};

	public MapCoordinate(CellCoordinate CellCoords, byte CubeCoords) {
		X = (CellCoords.X * CELLEDGESIZE) + ((CubeCoords >> CELLBITSHIFT) & CELLBITFLAG);
		Y = (CellCoords.Y * CELLEDGESIZE) + (CubeCoords & CELLBITFLAG);
		Z = CellCoords.Z;
	}

	public MapCoordinate(MapCoordinate SourceCoords, Direction DirectionType) {
		X = SourceCoords.X + DirectionType.X;
		Y = SourceCoords.Y + DirectionType.Y;
		Z = SourceCoords.Z + DirectionType.Z;
	}

	public void TranslateMapCoordinates(Direction DirectionType) {
		X += DirectionType.X;
		Y += DirectionType.Y;
		Z += DirectionType.Z;
	}

	public void TranslateMapCoordinates(Direction DirectionType, int Length) {
		X += DirectionType.ValueonAxis(Axis.AXIS_X) * Length;
		Y += DirectionType.ValueonAxis(Axis.AXIS_Y) * Length;
		Z += DirectionType.ValueonAxis(Axis.AXIS_Z) * Length;
	}

	public MapCoordinate(int NewX, int NewY, int NewZ) {
		Set(NewX, NewY, NewZ);
	}

	public void Set(int NewX, int NewY, int NewZ) {
		X = NewX;   Y = NewY;   Z = NewZ;
	};

	public void Set(Axis AxialComponent, int NewValue) {
		switch (AxialComponent)
		{
			case AXIS_Z:
				Z = NewValue;
				break;
			case AXIS_Y:
				Y = NewValue;
				break;
			case AXIS_X:
				X = NewValue;
				break;

			default:
				break;
		}
	}

	public byte Cube(){
		byte tempx = (byte) ((X & CELLBITFLAG) << CELLBITSHIFT);
		byte tempy = ((byte) (Y & CELLBITFLAG));
		return (byte) (tempx + tempy);
	}

	//public int Cube(){
	//	int tempx = (byte) ((X & CELLBITFLAG) << CELLBITSHIFT);
	//	int tempy = ((byte) (Y & CELLBITFLAG));
	//	return tempx + tempy;
	//}

	public void copy(MapCoordinate ArgumentCoordinates) {
		X = ArgumentCoordinates.X;
		Y = ArgumentCoordinates.Y;
		Z = ArgumentCoordinates.Z;
	}

	@Override
	public boolean equals(Object ArgumentCoordinates) {
		/*
		if (ArgumentCoordinates == null)
            return false;
        if (ArgumentCoordinates == this)
            return true;
        if (!(ArgumentCoordinates instanceof MapCoordinate))
            return false;*/
		
		MapCoordinate Arg = (MapCoordinate) ArgumentCoordinates;
		return (Arg.X == this.X && Arg.Y == this.Y && Arg.Z == this.Z);
	}

	public boolean greater(MapCoordinate ArgumentCoordinates) {
		if (Z < ArgumentCoordinates.Z)
		{
			return true;
		}
		if (Z > ArgumentCoordinates.Z)
		{
			return false;
		}
		if (Y < ArgumentCoordinates.Y)
		{
			return true;
		}
		if (Y > ArgumentCoordinates.Y)
		{
			return false;
		}
		if (X < ArgumentCoordinates.X)
		{
			return true;
		}
		if (X > ArgumentCoordinates.X)
		{
			return false;
		}

		return false;  // All values equal thus not less than
	};

	public int ValueonAxis(Axis AxialComponent) {
		switch (AxialComponent)
		{
			case AXIS_Z:
				return Z;
			case AXIS_Y:
				return Y;
			case AXIS_X:
				return X;
			default:
				return 0;
		}
	}

	@Override
	public int hashCode() {
		int hash = 3;
		hash += 17 * X;
		hash += 37 * Y;
		hash += 5 * Z;
		return hash;
	}
}

