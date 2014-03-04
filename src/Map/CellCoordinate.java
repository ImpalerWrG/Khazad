/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package Map;

/**
 *
 * @author Impaler
 */

public class CellCoordinate  // Holds the relative Relative position of Map Cells, 6 Bytes
{
	public short X, Y, Z;
	
	public CellCoordinate() {
		X = Y = Z = 0;
	}

	public CellCoordinate(int NewX, int NewY, int NewZ) {
		X = (short) NewX;
		Y = (short) NewY;
		Z = (short) NewZ;
	}

	public CellCoordinate(MapCoordinate SourceCoordinates) {
        if (SourceCoordinates.X >= 0)
        {
            X = (short) (SourceCoordinates.X >> MapCoordinate.CELLBITSHIFT);
        } else {
            X = (short) ((SourceCoordinates.X >> MapCoordinate.CELLBITSHIFT) - 1); //truncate to negative infinity
        }
        if (SourceCoordinates.Y >= 0)
        {
            Y = (short) (SourceCoordinates.Y >> MapCoordinate.CELLBITSHIFT);
        } else {
            Y = (short) ((SourceCoordinates.Y >> MapCoordinate.CELLBITSHIFT) - 1); //truncate to negative infinity
        }
        Z = (short) SourceCoordinates.Z;
    }

	public void setequal(CellCoordinate ArgumentCoordinates) {
		X = ArgumentCoordinates.X;
		Y = ArgumentCoordinates.Y;
		Z = ArgumentCoordinates.Z;
	}

	public boolean notequal(CellCoordinate ArgumentCoordinates) {
		return (X != ArgumentCoordinates.X || Y != ArgumentCoordinates.Y || Z != ArgumentCoordinates.Z);
	}

	@Override
	public boolean equals(Object ArgumentCoordinates) {
		if (ArgumentCoordinates == null)
            return false;
        if (ArgumentCoordinates == this)
            return true;
        if (!(ArgumentCoordinates instanceof CellCoordinate))
            return false;
		
		CellCoordinate Arg = (CellCoordinate) ArgumentCoordinates;
		return (X == Arg.X && Y == Arg.Y && Z == Arg.Z);
	}
	
	@Override
	public int hashCode() {
		int Key = 0;

		Key += X;
		Key <<= 12;
		Key += Y;
		Key <<= 12;
		Key += Z;

		return Key;
	}
}