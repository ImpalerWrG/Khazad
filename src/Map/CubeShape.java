/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package Map;

/**
 *
 * @author Impaler
 */
public class CubeShape {

	public static final byte BELOW_CUBE_HEIGHT = 0;
	public static final byte CUBE_BOTTOM_HEIGHT = 1;
	public static final byte HEIGHT_FRACTIONS = 4;
	public static final byte CUBE_TOP_HEIGHT = 5;

	// BitPacking  SWW SEE NWW NEE FLAG
	//             FED CBA 987 654 3210

	public static final short SWMASK = (short) 57344;
	public static final short SEMASK = 7168;
	public static final short NWMASK = 896;
	public static final short NEMASK = 112;

	public static final short FLAGMASK = 15;

	public static final short SWSHIFT = 13;
	public static final short SESHIFT = 10;
	public static final short NWSHIFT = 7;
	public static final short NESHIFT = 4;

	short Data;		// Bit compressed heights of each corner and flags



	public CubeShape(byte SWCornerHeight, byte SECornerHeight, byte NWCornerHeight, byte NECornerHeight, byte NewFlags) {
		Data = (short) (((SWCornerHeight << SWSHIFT) & SWMASK) | ((SECornerHeight << SESHIFT) & SEMASK) | ((NWCornerHeight << NWSHIFT) & NWMASK) | ((NECornerHeight << NESHIFT) & NEMASK) | (NewFlags & FLAGMASK));
    }
	
    public CubeShape(byte NewHeight, byte flags) {
		this(NewHeight, NewHeight, NewHeight, NewHeight, flags);
    }

    public CubeShape(byte NewHeight) {
		this(NewHeight, NewHeight, NewHeight, NewHeight, (byte) 0);
    }

	public CubeShape() {
        this(CUBE_TOP_HEIGHT);
    }
	
	public CubeShape(short newData) {
		Data = newData;
	}

	public short getData() {
		return Data;
	}
		
    public boolean isSolid() {
        return (SouthWestCorner() >= CUBE_TOP_HEIGHT) && (SouthEastCorner() >= CUBE_TOP_HEIGHT) && (NorthWestCorner() >= CUBE_TOP_HEIGHT) && (NorthEastCorner() >= CUBE_TOP_HEIGHT);
    }

    public boolean isEmpty() {
        return (SouthWestCorner() <= CUBE_BOTTOM_HEIGHT) && (SouthEastCorner() <= CUBE_BOTTOM_HEIGHT) && (NorthWestCorner() <= CUBE_BOTTOM_HEIGHT) && (NorthEastCorner() <= CUBE_BOTTOM_HEIGHT);
    }

    public boolean isSky() {
        int Counter = 0;

        if (SouthWestCorner() < CUBE_BOTTOM_HEIGHT)
            Counter++;
        if (SouthEastCorner() < CUBE_BOTTOM_HEIGHT)
            Counter++;
        if (NorthEastCorner() < CUBE_BOTTOM_HEIGHT)
            Counter++;
        if (NorthWestCorner() < CUBE_BOTTOM_HEIGHT)
            Counter++;

        if (Counter >= 2) {
            return true;
        }

        return false;
    }

    public boolean hasFace(Direction DirectionType) {
        if (DirectionType == Direction.DIRECTION_DOWN)
            return hasFloor();
        if (DirectionType == Direction.DIRECTION_UP)
            return hasCeiling();
        if (DirectionType == Direction.DIRECTION_EAST)
            return (SouthEastCorner() > CUBE_BOTTOM_HEIGHT || NorthEastCorner() > CUBE_BOTTOM_HEIGHT);
        if (DirectionType == Direction.DIRECTION_WEST)
            return (SouthWestCorner() > CUBE_BOTTOM_HEIGHT || NorthWestCorner() > CUBE_BOTTOM_HEIGHT);
        if (DirectionType == Direction.DIRECTION_NORTH)
            return (NorthEastCorner() > CUBE_BOTTOM_HEIGHT || NorthWestCorner() > CUBE_BOTTOM_HEIGHT);
        if (DirectionType == Direction.DIRECTION_SOUTH)
            return (SouthEastCorner() > CUBE_BOTTOM_HEIGHT || SouthWestCorner() > CUBE_BOTTOM_HEIGHT);
		
		return false;
    }

    public boolean hasFloor() {
       if (split())
       {
            if (SouthEastCorner() == CUBE_BOTTOM_HEIGHT && NorthEastCorner() == CUBE_BOTTOM_HEIGHT && NorthWestCorner() == CUBE_BOTTOM_HEIGHT)
            {
                return true;
            }
            if (NorthWestCorner() == CUBE_BOTTOM_HEIGHT && SouthWestCorner() == CUBE_BOTTOM_HEIGHT && SouthEastCorner() == CUBE_BOTTOM_HEIGHT)
            {
                return true;
            }
            return false;
       }
       else
       {
            if (NorthEastCorner() == CUBE_BOTTOM_HEIGHT && NorthWestCorner() == CUBE_BOTTOM_HEIGHT && SouthWestCorner() == CUBE_BOTTOM_HEIGHT)
            {
                return true;
            }
            if (SouthWestCorner() == CUBE_BOTTOM_HEIGHT && SouthEastCorner() == CUBE_BOTTOM_HEIGHT && NorthEastCorner() == CUBE_BOTTOM_HEIGHT)
            {
                return true;
            }
            return false;
        }
    }

    public boolean hasCeiling() {
       if (split())
       {
            if (SouthEastCorner() == CUBE_TOP_HEIGHT && NorthEastCorner() == CUBE_TOP_HEIGHT && NorthWestCorner() == CUBE_TOP_HEIGHT)
            {
                return true;
            }
            if (NorthWestCorner() == CUBE_TOP_HEIGHT && SouthWestCorner() == CUBE_TOP_HEIGHT && SouthEastCorner() == CUBE_TOP_HEIGHT)
            {
                return true;
            }
            return false;
       }
       else
       {
            if (NorthEastCorner() == CUBE_TOP_HEIGHT && NorthWestCorner() == CUBE_TOP_HEIGHT && SouthWestCorner() == CUBE_TOP_HEIGHT)
            {
                return true;
            }
            if (SouthWestCorner() == CUBE_TOP_HEIGHT && SouthEastCorner() == CUBE_TOP_HEIGHT && NorthEastCorner() == CUBE_TOP_HEIGHT)
            {
                return true;
            }
            return false;
        }
    }

    public byte SouthWestCorner() {
		return (byte) (((Data & SWMASK) >> SWSHIFT) & 7);
    }

    public byte SouthEastCorner() {
    	return (byte) (((Data & SEMASK) >> SESHIFT) & 7);
    }

    public byte NorthWestCorner() {
    	return (byte) (((Data & NWMASK) >> NWSHIFT) & 7);
    }

    public byte NorthEastCorner() {
    	return (byte) (((Data & NEMASK) >> NESHIFT) & 7);
    }

	void setSouthWestCorner(byte Height) {
		Data |= SWMASK;
		Data &= ((Height << SWSHIFT) & SWMASK);
	}

	public void setSouthEastCorner(byte Height) {
		Data |= SEMASK;
		Data &= ((Height << SESHIFT) & SEMASK);
	}

	public void setNorthWestCorner(byte Height) {
		Data |= NWMASK;
		Data &= ((Height << NWSHIFT) & NWMASK);
	}

	public void setNorthEastCorner(byte Height) {
		Data |= NEMASK;
		Data &= ((Height << NESHIFT) & NEMASK);
	}

    public boolean split() {
		return (Data & 1) > 0 ? true : false ;
    }

    public boolean equals(CubeShape ArgumentShape) {
		return Data == ArgumentShape.Data;
    }

    public boolean notequal(CubeShape ArgumentShape) {
		return Data != ArgumentShape.Data;
    }

}
