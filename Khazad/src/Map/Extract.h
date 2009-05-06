#ifndef EXTRACT_HEADER
#define EXTRACT_HEADER

#include <Singleton.h>
#include <stdafx.h>

#define BlockSize 16

#define DESIGNATION_LIQUID_AMOUNT_BIT1 0
#define DESIGNATION_LIQUID_AMOUNT_BIT2 1
#define DESIGNATION_LIQUID_AMOUNT_BIT3 2
#define DESIGNATION_PILE 3
#define DESIGNATION_DIG_BIT1 4
#define DESIGNATION_DIG_BIT2 5
#define DESIGNATION_DIG_BIT3 6
#define DESIGNATION_DETAIL 7
#define DESIGNATION_DETAIL_EVENT 8
#define DESIGNATION_HIDDEN 9
#define DESIGNATION_MATGLOSS_BIT1 10
#define DESIGNATION_MATGLOSS_BIT2 11
#define DESIGNATION_MATGLOSS_BIT3 12
#define DESIGNATION_MATGLOSS_BIT4 13
#define DESIGNATION_OPEN_TO_SUN 14
#define DESIGNATION_SUBTERRANEAN 15
#define DESIGNATION_SKY_VIEW 16
#define DESIGNATION_BIOME_BIT1 17
#define DESIGNATION_BIOME_BIT2 18
#define DESIGNATION_BIOME_BIT3 19
#define DESIGNATION_BIOME_BIT4 20
#define DESIGNATION_LIQUID_TYPE 21
#define DESIGNATION_WATER_TABLE 22
#define DESIGNATION_RAINED 23
#define DESIGNATION_TRAFFIC_BIT1 24
#define DESIGNATION_TRAFFIC_BIT2 25
#define DESIGNATION_FLOW_FORBID 26
#define DESIGNATION_LIQUID_STATIC 27
#define DESIGNATION_MOSS 28
#define DESIGNATION_FEATURE_PRESENT 29
#define DESIGNATION_LIQUID_CHARACTER_BIT1 30
#define DESIGNATION_LIQUID_CHARACTER_BIT2 31


#define OCCUPANCY_BUILDING_BIT1 0
#define OCCUPANCY_BUILDING_BIT2 1
#define OCCUPANCY_BUILDING_BIT3 2
#define OCCUPANCY_UNIT 3
#define OCCUPANCY_GROUNDED_UNIT 4
#define OCCUPANCY_ITEM 5
#define OCCUPANCY_MUD 6
#define OCCUPANCY_VOMIT 7
#define OCCUPANCY_DEBRIS1 8
#define OCCUPANCY_DEBRIS2 9
#define OCCUPANCY_DEBRIS3 10
#define OCCUPANCY_DEBRIS4 11
#define OCCUPANCY_BLOOD_G 12
#define OCCUPANCY_BLOOD_G2 13
#define OCCUPANCY_BLOOD_B 14
#define OCCUPANCY_BLOOD_B2 15
#define OCCUPANCY_BLOOD_Y 16
#define OCCUPANCY_BLOOD_Y2 17
#define OCCUPANCY_BLOOD_M 18
#define OCCUPANCY_BLOOD_M2 19
#define OCCUPANCY_BLOOD_C 20
#define OCCUPANCY_BLOOD_C2 21
#define OCCUPANCY_BLOOD_W 22
#define OCCUPANCY_BLOOD_W2 23
#define OCCUPANCY_BLOOD_0 24
#define OCCUPANCY_BLOOD_02 25
#define OCCUPANCY_SLIME 26
#define OCCUPANCY_SLIME2 27
#define OCCUPANCY_BLOOD 28
#define OCCUPANCY_BLOOD2 29
#define OCCUPANCY_DEBRIS5 30
#define OCCUPANCY_SNOW 31


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
    unsigned int MapSizeZ;

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

    int DesignationBitBlock(unsigned int Start, unsigned int Size, int x, int y, int z);
    int OccupancyBitBlock(unsigned int Start, unsigned int Size, int x, int y, int z);
};


#define EXTRACT (Extractor::GetInstance())

#endif // EXTRACT_HEADER
