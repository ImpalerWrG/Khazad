#ifndef EXTRACT_HEADER
#define EXTRACT_HEADER

#include <Singleton.h>
#include <stdafx.h>

#define BlockSize 16

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

protected:

    bool MapLoaded;

    short int*** Tiles;
    int*** Designations;
    int*** Ocupancy;

    int*** Blocks;

    std::vector<memory_info> meminfo;

    unsigned int x_blocks;
    unsigned int y_blocks;
    unsigned int z_levels;

    unsigned int MapSizeX;
    unsigned int MapSizeY;

public:

    bool Init();
    ~Extractor();

    bool isMapLoaded()      { return MapLoaded; }

    bool dumpMemory();
    bool loadMap(char* FilePath);
    bool writeMap(char* FilePath);
    bool FreeMap();

    unsigned int getXBlocks()        { return x_blocks; }
    unsigned int getYBlocks()        { return y_blocks; }
    unsigned int getZBlocks()        { return z_levels; }

    short int getTileType(int x, int y, int z);
    int getDesignations(int x, int y, int z);
    int getOccupancies(int x, int y, int z);

    int readMemoryFile();

    int picktexture(int);

    int getLiquidLevel(int x, int y, int z);
    bool isOpenTerrain(int);
    bool isStairTerrain(int);
    bool isRampTerrain(int);
    bool isFloorTerrain(int);
    bool isWallTerrain(int);

    bool isDesignationFlag(unsigned int flag, int x, int y, int z);
    bool isOcupancyFlag(unsigned int flag, int x, int y, int z);
};


#define EXTRACT (Extractor::GetInstance())

#endif // EXTRACT_HEADER
