#ifndef MAP_H
#define MAP_H

#include <Singleton.h>
#include <stdafx.h>

#include <boost/multi_array.hpp>



#define CUBE_LEFT     1
#define CUBE_FORWARD  2
#define CUBE_RIGHT    4
#define CUBE_BACK     8
#define CUBE_UP       16
#define CUBE_DOWN     32

#define FLOOR_LEFT    64
#define FLOOR_FORWARD 128
#define FLOOR_RIGHT   256
#define FLOOR_BACK    512
#define FLOOR_DOWN    1024

#define RAMP_FLAG     4096
#define WALL_FLAG     8192
#define FLOOR_FLAG    16384

//#define RAMP_CENTER
#define RAMP_NW       2
#define RAMP_N        4
#define RAMP_NE       8
#define RAMP_E        16
#define RAMP_SE       32
#define RAMP_S        2048
#define RAMP_SW       32768
#define RAMP_W        1
#define RAMP_MASK     34879


class memory_info
{
public:
    char version[20];
    int pe_timestamp;
    int pe_timestamp_offset;
    int map_offset;
    int x_count_offset;
    int y_count_offset;
    int z_count_offset;
    int tile_type_offset;
    int designation_offset;
    int occupancy_offset;
};

class Extractor
{
DECLARE_SINGLETON_CLASS(Extractor)

public:

    int toggle_isloaded;

    short int*** Tiles;

    boost::multi_array<int, 3> map_blocks;
    boost::multi_array<short int, 3> tile_types;
//    boost::multi_array<short int, 3> cube_sides;
//    boost::multi_array<char, 3> cube_textures;

    std::vector<memory_info> meminfo;

    int x_blocks;
    int y_blocks;
    int z_levels;
    int z_active_levels;
    short int *z_level_active;

    bool Init();
    ~Extractor();
    int dumpMemory(/*scene**/);
    int loadMap(char*/*,scene**/);
    int createTerrain();
    int readMemoryFile();

    int picktexture(int);
    int isOpenTerrain(int);
    int isStairTerrain(int);
    int isRampTerrain(int);
    int isFloorTerrain(int);
    int isWallTerrain(int);

};


#define EXTRACT (Extractor::GetInstance())

#endif // MAP_H
