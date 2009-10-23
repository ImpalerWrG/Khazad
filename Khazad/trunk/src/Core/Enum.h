#ifndef ENUM__HEADER
#define ENUM__HEADER

enum ActorType
{
	FACE_ACTOR,
	SLOPE_ACTOR,
	CUBE_ACTOR,
	CELL_ACTOR,
	PAWN_ACTOR,

	NUM_ACTORS
};

enum Facet
{
	FACET_WEST,    // -X axis
	FACET_EAST,    // +X axis
	FACET_NORTH,   // -Y axis
	FACET_SOUTH,   // +Y axis
	FACET_BOTTOM,  // -Z axis
	FACET_TOP,     // +Z axis

	NUM_FACETS,
	FACETS_START = 0
};

inline Facet &operator++ (Facet &OldFacet)      { return OldFacet = Facet(OldFacet + 1); }
inline Facet &operator-- (Facet &OldFacet)      { return OldFacet = Facet(OldFacet - 1); }

inline Facet OppositeFacet(Facet FacetType)
{
    return  (Facet) ((int) FacetType ^ 1);  // Flips the last bit
}

inline void TranslateCordinates(Sint32* X, Sint32* Y, Sint32* Z, Facet TestFacet)
{
    //TOD some bit shift magic to speed this up and use less if statements

    if(TestFacet == FACET_WEST)
    {
        *X -= 1;
    }
    if (TestFacet == FACET_EAST)
    {
        *X += 1;
    }
    if(TestFacet == FACET_NORTH)
    {
        *Y -= 1;
    }
    if (TestFacet == FACET_SOUTH)
    {
        *Y += 1;
    }
    if(TestFacet == FACET_BOTTOM)
    {
        *Z -= 1;
    }
    if (TestFacet == FACET_TOP)
    {
        *Z += 1;
    }
}
/*
enum Slopping
{
    SLOPE_FLAT,

    SLOPE_NORTH_EAST,
    SLOPE_SOUTH_EAST,
    SLOPE_SOUTH_WEST,
    SLOPE_NORTH_WEST,

    SLOPE_LARGE_NORTH,
    SLOPE_LARGE_EAST,
    SLOPE_LARGE_SOUTH,
    SLOPE_LARGE_WEST,

    SLOPE_SMALL_NORTH,
    SLOPE_SMALL_EAST,
    SLOPE_SMALL_SOUTH,
    SLOPE_SMALL_WEST,

    SLOPE_CRESS_NORTH_SOUTH,
    SLOPE_CRESS_EAST_WEST,

    NUM_SLOPES,
    SLOPES_START = 0
};

inline Slopping &operator++ (Slopping &OldSlope)      { return OldSlope = Slopping(OldSlope + 1); }
*/
enum Direction
{
    DIRECTION_NORTHWEST,
	DIRECTION_NORTH,
	DIRECTION_NORTHEAST,
	DIRECTION_EAST,
	DIRECTION_SOUTHEAST,
	DIRECTION_SOUTH,
	DIRECTION_SOUTHWEST,
	DIRECTION_WEST,
	DIRECTION_UP,
    DIRECTION_DOWN,

	NUM_DIRECTIONS,
    DIRECTIONS_START = 0
};

inline void TranslateCordinates(Sint32* X, Sint32* Y, Sint32* Z, Direction TestDirection)
{
    if(TestDirection == DIRECTION_NORTHWEST ||  TestDirection == DIRECTION_NORTH || TestDirection == DIRECTION_NORTHEAST)
    {
        *Y -= 1;
    }
    else if (TestDirection == DIRECTION_SOUTHEAST ||  TestDirection == DIRECTION_SOUTH || TestDirection == DIRECTION_SOUTHWEST)
    {
        *Y += 1;
    }

    if(TestDirection == DIRECTION_NORTHEAST ||  TestDirection == DIRECTION_EAST || TestDirection == DIRECTION_SOUTHEAST)
    {
        *X += 1;
    }
    else if (TestDirection == DIRECTION_SOUTHWEST ||  TestDirection == DIRECTION_WEST || TestDirection == DIRECTION_NORTHWEST)
    {
        *X -= 1;
    }

    if(TestDirection == DIRECTION_UP)
    {
        *Z += 1;
    }
    else if (TestDirection == DIRECTION_DOWN)
    {
        *Z -= 1;
    }
}

inline Direction &operator++ (Direction &OldDirection)      { return OldDirection = Direction(OldDirection + 1); }
inline Direction &operator-- (Direction &OldDirection)      { return OldDirection = Direction(OldDirection - 1); }

enum CameraOrientation
{
    CAMERA_DOWN,
	CAMERA_NORTH_WEST,
	CAMERA_NORTH_EAST,
	CAMERA_SOUTH_WEST,
	CAMERA_SOUTH_EAST,

	NUM_ORIENTATIONS,
    ORIENTATIONS_START = 0
};

inline CameraOrientation &operator++ (CameraOrientation &Orientation)      { return Orientation = CameraOrientation(Orientation + 1); }
inline CameraOrientation &operator-- (CameraOrientation &Orientation)      { return Orientation = CameraOrientation(Orientation - 1); }

enum SpacialPoint
{
    SPACIAL_POINT_CENTER,

    SPACIAL_POINT_NORTH_TOP,
    SPACIAL_POINT_EAST_TOP,
    SPACIAL_POINT_SOUTH_TOP,
    SPACIAL_POINT_WEST_TOP,

    SPACIAL_POINT_NORTH_BOTTOM,
    SPACIAL_POINT_EAST_BOTTOM,
    SPACIAL_POINT_SOUTH_BOTTOM,
    SPACIAL_POINT_WEST_BOTTOM,

    NUM_SPACIAL_POINTS
};

enum WallDisplayMode
{
    DISPLAY_MODE_FLOOR_ONLY,
    DISPLAY_MODE_BACK_FACET,
    DISPLAY_MODE_BOTH_FACETS,
    DISPLAY_MODE_THICK_WALLS,

    NUM_DISPLAY_MODES,
    DISPLAY_MODES_START = 0
};

inline WallDisplayMode &operator++ (WallDisplayMode &DisplayMode)      { return DisplayMode = WallDisplayMode(DisplayMode + 1); }
inline WallDisplayMode &operator-- (WallDisplayMode &DisplayMode)      { return DisplayMode = WallDisplayMode(DisplayMode - 1); }

#endif // ENUM_HEADER
