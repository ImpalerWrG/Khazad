/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package Map;

/**
 *
 * @author Impaler
 */

public class FaceCoordinate
{
	public byte Coordinates;
	public Direction FaceDirection;
	
	
	FaceCoordinate() {
		Coordinates = 0;
		FaceDirection = Direction.DIRECTION_DESTINATION;
	}
	
	FaceCoordinate(byte CubeCoordinates, Direction TargetDirection) {
		Coordinates = CubeCoordinates;
		FaceDirection = TargetDirection;		
	}

	public void set(FaceCoordinate ArgumentCoordinates) {
		Coordinates = ArgumentCoordinates.Coordinates;
		FaceDirection = ArgumentCoordinates.FaceDirection;
	}
		
	public void Set(byte TargetCoords, Direction DirectionComponent) {
		Coordinates = TargetCoords;
		FaceDirection = DirectionComponent;
	}

	public void Set(int NewX, int NewY, Direction DirectionComponent) {
		Coordinates = (byte) ((NewX << MapCoordinate.CELLBITSHIFT) + NewY);
		FaceDirection = DirectionComponent;
	}
	
	public int getX() {
		return (Coordinates >> MapCoordinate.CELLBITSHIFT) & 15;
	}

	public int getY() {
		return (Coordinates & MapCoordinate.CELLBITFLAG) & 15;
	}
	
	@Override
	public boolean equals(Object ArgumentCoordinates) {
		if (ArgumentCoordinates == null)
            return false;
        if (ArgumentCoordinates == this)
            return true;
        if (!(ArgumentCoordinates instanceof FaceCoordinate))
            return false;
		
		FaceCoordinate Arg = (FaceCoordinate) ArgumentCoordinates;

		return (Arg.Coordinates == Coordinates && Arg.FaceDirection == FaceDirection);
	}
	
	@Override
	public int hashCode() {
		int Key =  Coordinates;
		Key <<= 8;
		Key += FaceDirection.ordinal();

		return Key;
	}
}

