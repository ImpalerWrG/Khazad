/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package Map;

import com.jme3.math.Vector3f;
/**
 *
 * @author Impaler
 */


// BitPacking  XX 0 YY 0 ZZ
//             76 5 43 2 10

public enum Direction {

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
	public final int X, Y, Z;
    
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
        DIRECTION_SOUTHWEST
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

	public static final Direction Opposites [][] = {
		
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
        {DIRECTION_DOWN_SOUTHWEST, DIRECTION_UP_NORTHEAST}
	};
	
	
   Direction(byte Rawvalue, int XAxis, int YAxis, int ZAxis) {
        Data = Rawvalue;
		X = XAxis;  Y = YAxis;  Z = ZAxis;
    };

	byte value() {
		return Data;
	}
	
    public Direction Invert() {		
		return Opposites[this.ordinal()][1];
    }

    public boolean Equals(Direction OtherDirection) {
        return OtherDirection.Data != Data;
    }

    public byte ValueonAxis(Axis QuerryAxis) {
        byte temp = (byte) ((Data >> (QuerryAxis.ordinal() * 3)) & 3);
        if (temp == 1)
                return 1;
        if (temp == 2)
                return -1;
        return 0;
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

    byte Index() {
        byte Z = (byte) (Data & 3);
        byte Y = (byte) (((Data >> 3) & 3) * 3);
        byte X = (byte) (((Data >> 6) & 3) * 9);

        return (byte) (X + Y + Z - 1);
    }
	
	public Vector3f toVector() {
		Vector3f Vec = new Vector3f();
		Vec.x = ValueonAxis(Axis.AXIS_X);
		Vec.y = ValueonAxis(Axis.AXIS_Y);
		Vec.z = ValueonAxis(Axis.AXIS_Z);
		return Vec;
	}
}

