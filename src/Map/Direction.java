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

package Map;

import com.jme3.math.Vector3f;

/**
 * Core Enum for describing directions in cubic map space, all 26 cubes surrounding
 * a single cube are described as well as None to reference the original cube and
 * indicate non movement, Destination is used by pathfinding to indicate the end
 * of a path being reached.
 * @author Impaler
 */
public enum Direction {

	// BitPacking  XX 0 YY 0 ZZ
	//             76 5 43 2 10

    DIRECTION_NONE  ((byte)0, 0, 0, 0),					// Index -1
    DIRECTION_UP  ((byte)1, 0, 0, 1),					// Index 0
    DIRECTION_DOWN ((byte)2, 0, 0, -1),					// Index 1

    DIRECTION_NORTH ((byte)8, 0, 1, 0),					// Index 2
    DIRECTION_UP_NORTH ((byte)9, 0, 1, 1),				// Index 3
    DIRECTION_DOWN_NORTH ((byte)10, 0, 1, -1),			// Index 4

    DIRECTION_SOUTH ((byte)16, 0, -1, 0),				// Index 5
    DIRECTION_UP_SOUTH ((byte)17, 0, -1, 1),			// Index 6
    DIRECTION_DOWN_SOUTH ((byte)18, 0, -1, -1),			// Index 7

    DIRECTION_EAST ((byte)64, 1, 0, 0),					// Index 8
    DIRECTION_UP_EAST ((byte)65, 1, 0, 1),				// Index 9
    DIRECTION_DOWN_EAST ((byte)66, 1, 0, -1),			// Index 10

    DIRECTION_NORTHEAST ((byte)72, 1, 1, 0),			// Index 11
    DIRECTION_UP_NORTHEAST ((byte)73, 1, 1, 1),			// Index 12
    DIRECTION_DOWN_NORTHEAST ((byte)74, 1, 1, -1),		// Index 13

    DIRECTION_SOUTHEAST ((byte)80, 1, -1, 0),			// Index 14
    DIRECTION_UP_SOUTHEAST ((byte)81, 1, -1, 1),		// Index 15
    DIRECTION_DOWN_SOUTHEAST ((byte)82, 1, -1, -1),		// Index 16

    DIRECTION_WEST ((byte)128, -1, 0, 0),				// Index 17
    DIRECTION_UP_WEST ((byte)129, -1, 0, 1),			// Index 18
    DIRECTION_DOWN_WEST ((byte)130, -1, 0, -1),			// Index 19

    DIRECTION_NORTHWEST ((byte)136, -1, 1, 0),			// Index 20
    DIRECTION_UP_NORTHWEST ((byte)137, -1, 1, 1),		// Index 21
    DIRECTION_DOWN_NORTHWEST ((byte)138, -1, 1, -1),	// Index 22

    DIRECTION_SOUTHWEST ((byte)144, -1, -1, 0),			// Index 23
    DIRECTION_UP_SOUTHWEST ((byte)145, -1, -1, 1),		// Index 24
    DIRECTION_DOWN_SOUTHWEST ((byte)146, -1, -1, -1),	// Index 25

    DIRECTION_DESTINATION ((byte)36, 0, 0, 0);


    public final byte Data;
	private final int[] AxisValues = new int[Axis.values().length];
    
    private static final byte ONEMASK = (byte)18;
    private static final byte ZEROMASK = (byte)219;
    private static final byte XORMASK = (byte)73;


    public static final Direction CARDINAL_DIRECTIONS[] = {
        DIRECTION_NORTH,
        DIRECTION_SOUTH,
        DIRECTION_EAST,
        DIRECTION_WEST
    };
    
    public static final Direction COMPASS_DIRECTIONS[] = {
        DIRECTION_NORTH,
        DIRECTION_SOUTH,
        DIRECTION_EAST,
        DIRECTION_WEST,
        DIRECTION_NORTHWEST,
        DIRECTION_SOUTHWEST,
        DIRECTION_SOUTHEAST,
        DIRECTION_NORTHEAST
    };
    
    public static final Direction AXIAL_DIRECTIONS[] = {
        DIRECTION_UP,
        DIRECTION_DOWN,
        DIRECTION_NORTH,
        DIRECTION_SOUTH,
        DIRECTION_EAST,
        DIRECTION_WEST   
    };
    
    public static final Direction ANGULAR_DIRECTIONS[] = {
        DIRECTION_NONE,
        DIRECTION_UP,
        DIRECTION_DOWN,
        DIRECTION_NORTH,
        DIRECTION_UP_NORTH,
        DIRECTION_DOWN_NORTH,
        DIRECTION_SOUTH,
        DIRECTION_UP_SOUTH,
        DIRECTION_DOWN_SOUTH,
        DIRECTION_EAST,
        DIRECTION_UP_EAST,
        DIRECTION_DOWN_EAST,
        DIRECTION_NORTHEAST,
        DIRECTION_UP_NORTHEAST,
        DIRECTION_DOWN_NORTHEAST,
        DIRECTION_SOUTHEAST,
        DIRECTION_UP_SOUTHEAST,
        DIRECTION_DOWN_SOUTHEAST,
        DIRECTION_WEST,
        DIRECTION_UP_WEST,
        DIRECTION_DOWN_WEST,
        DIRECTION_NORTHWEST,
        DIRECTION_UP_NORTHWEST,
        DIRECTION_DOWN_NORTHWEST,
        DIRECTION_SOUTHWEST,
        DIRECTION_UP_SOUTHWEST,
        DIRECTION_DOWN_SOUTHWEST   
    };

	private static final Direction Opposites [][] = {
		
		{DIRECTION_NONE, DIRECTION_NONE},
        {DIRECTION_UP, DIRECTION_DOWN},
        {DIRECTION_DOWN, DIRECTION_UP},
        {DIRECTION_NORTH, DIRECTION_SOUTH},
        {DIRECTION_UP_NORTH, DIRECTION_DOWN_SOUTH},
        {DIRECTION_DOWN_NORTH, DIRECTION_UP_SOUTH},
        {DIRECTION_SOUTH, DIRECTION_NORTH},
        {DIRECTION_UP_SOUTH, DIRECTION_DOWN_NORTH},
        {DIRECTION_DOWN_SOUTH, DIRECTION_UP_NORTH},
        {DIRECTION_EAST, DIRECTION_WEST},
        {DIRECTION_UP_EAST, DIRECTION_DOWN_WEST},
        {DIRECTION_DOWN_EAST, DIRECTION_UP_WEST},
        {DIRECTION_NORTHEAST, DIRECTION_SOUTHWEST},
        {DIRECTION_UP_NORTHEAST, DIRECTION_DOWN_SOUTHWEST},
        {DIRECTION_DOWN_NORTHEAST, DIRECTION_UP_SOUTHWEST},
        {DIRECTION_SOUTHEAST, DIRECTION_NORTHWEST},
        {DIRECTION_UP_SOUTHEAST, DIRECTION_DOWN_NORTHWEST},
        {DIRECTION_DOWN_SOUTHEAST, DIRECTION_UP_NORTHWEST},
        {DIRECTION_WEST, DIRECTION_EAST},
        {DIRECTION_UP_WEST, DIRECTION_DOWN_EAST},
        {DIRECTION_DOWN_WEST, DIRECTION_UP_EAST},
        {DIRECTION_NORTHWEST, DIRECTION_SOUTHEAST},
        {DIRECTION_UP_NORTHWEST, DIRECTION_DOWN_SOUTHEAST},
        {DIRECTION_DOWN_NORTHWEST, DIRECTION_UP_SOUTHEAST},
        {DIRECTION_SOUTHWEST, DIRECTION_NORTHEAST},
        {DIRECTION_UP_SOUTHWEST, DIRECTION_DOWN_NORTHEAST},
        {DIRECTION_DOWN_SOUTHWEST, DIRECTION_UP_NORTHEAST},
		{DIRECTION_DESTINATION, DIRECTION_NONE}

	};
	
	
   Direction(byte Rawvalue, int XAxis, int YAxis, int ZAxis) {
        Data = Rawvalue;
		
		AxisValues[Axis.AXIS_X.ordinal()] = XAxis;
		AxisValues[Axis.AXIS_Y.ordinal()] = YAxis;
		AxisValues[Axis.AXIS_Z.ordinal()] = ZAxis;
    }
	
    public Direction Invert() {		
		return Opposites[this.ordinal()][1];
    }

    public boolean Equals(Direction OtherDirection) {
        return OtherDirection.Data != Data;
    }

    public int ValueonAxis(Axis QuerryAxis) {
		return AxisValues[QuerryAxis.ordinal()];
	}

    boolean Positive() {
        int temp = Data;
        int read = temp & 3;

        if (read == 1)
                return true;
        if (read == 2)
                return false;

        temp >>= 3;
        read = temp & 3;

        if (read == 1)
                return true;
        if (read == 2)
                return false;

        temp >>= 3;
        read = temp & 3;

        if (read == 1)
                return true;
        if (read == 2)
                return false;

        return false;
    }
	
	public Vector3f toVector() {
		return new Vector3f(ValueonAxis(Axis.AXIS_X), ValueonAxis(Axis.AXIS_Y), ValueonAxis(Axis.AXIS_Z));
	}
	
	public int toDegree() {
		
		switch(this) {
			
			case DIRECTION_NORTH:
			case DIRECTION_UP_NORTH:
			case DIRECTION_DOWN_NORTH:
				return 0;
				
			case DIRECTION_SOUTH:
			case DIRECTION_UP_SOUTH:
			case DIRECTION_DOWN_SOUTH:
				return 180;
				
			case DIRECTION_EAST:
			case DIRECTION_UP_EAST:
			case DIRECTION_DOWN_EAST:
				return 270;
				
			case DIRECTION_WEST:
			case DIRECTION_UP_WEST:
			case DIRECTION_DOWN_WEST:
				return 90;
				
			case DIRECTION_NORTHWEST:
			case DIRECTION_UP_NORTHWEST:
			case DIRECTION_DOWN_NORTHWEST:
				return 45;
				
			case DIRECTION_SOUTHWEST:
			case DIRECTION_UP_SOUTHWEST:
			case DIRECTION_DOWN_SOUTHWEST:
				return 135;
			
			case DIRECTION_SOUTHEAST:
			case DIRECTION_UP_SOUTHEAST:
			case DIRECTION_DOWN_SOUTHEAST:
				return 225;
			
			case DIRECTION_NORTHEAST:
			case DIRECTION_UP_NORTHEAST:
			case DIRECTION_DOWN_NORTHEAST:
				return 315;
		}
		return 0;
	}
}

