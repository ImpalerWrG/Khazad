#ifndef ENUM__HEADER
#define ENUM__HEADER

enum ActorType
{
	FACE_ACTOR,
	SLOPE_ACTOR,
	CUBE_ACTOR,
	CELL_ACTOR,
	PAWN_ACTOR,

	NUM_ACTORS,
	ACTORS_START = 0
};

enum Facet
{
	FACET_TOP,
	FACET_BOTTOM,
	FACET_NORTH_EAST,
	FACET_SOUTH_EAST,
	FACET_SOUTH_WEST,
	FACET_NORTH_WEST,

	NUM_FACETS,
	FACETS_START = 0
};

inline Facet &operator++ (Facet &OldFace)      { return OldFace = Facet(OldFace + 1); }

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

enum Direction
{
    DIRECTION_UP,
    DIRECTION_DOWN,
	DIRECTION_NORTH,
	DIRECTION_NORTHEAST,
	DIRECTION_EAST,
	DIRECTION_SOUTHEAST,
	DIRECTION_SOUTH,
	DIRECTION_SOUTHWEST,
	DIRECTION_WEST,
	DIRECTION_NORTHWEST,

	NUM_DIRECTIONS,
    DIRECTIONS_START = 0
};

inline Direction &operator++ (Direction &OldDirection)      { return OldDirection = Direction(OldDirection + 1); }

enum CameraOrientation
{
    CAMERA_DOWN,
	CAMERA_NORTH,
	CAMERA_EAST,
	CAMERA_SOUTH,
	CAMERA_WEST,

	NUM_ORIENTATIONS,
    ORIENTATIONS_START = 0
};

inline CameraOrientation &operator++ (CameraOrientation &Orientation)      { return Orientation = CameraOrientation(Orientation + 1); }


#endif // ENUM_HEADER
