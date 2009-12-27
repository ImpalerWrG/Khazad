#ifndef ENUM__HEADER
#define ENUM__HEADER

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
