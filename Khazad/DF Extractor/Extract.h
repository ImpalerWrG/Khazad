/* Memory research
    http://dwarffortresswiki.net/index.php/User:Rick/memory.ini#A_table_of_available_settings
    http://dwarffortresswiki.net/index.php/User:Rick/Memory_research#Tile_Block
    http://dwarffortresswiki.net/index.php/User:AzureLightning/Memory_research
    http://dwarffortresswiki.net/index.php/User:Iluxan/Memory_research
 */

#ifndef EXTRACT_HEADER
#define EXTRACT_HEADER

#include <Singleton.h>
#include <stdafx.h>
#include <Process.h>

#define BLOCK_SIZE 16

// designation flags (tree,shub,lava,etc)
/// TODO: research this further? consult DF hacker wizards?
union t_designation
{
    Uint32 whole;
    struct {
    unsigned int flow_size : 3; // how much liquid is here?
    unsigned int pile : 1; // stockpile?
    unsigned int dig : 3;// needs more info...
    unsigned int detail : 1;
    unsigned int detail_event : 1;// what?
    unsigned int hidden :1;
    unsigned int matgloss :4;// how do we know which material it really is?
    unsigned int light : 1;
    unsigned int subterranean : 1; // never seen the light of day?
    unsigned int skyview : 1; // sky is visible now, it rains in here when it rains
    unsigned int biome : 4; // why four bits for maximum four biomes? is this some bitmap thing?
    unsigned int liquid_type : 1;
    unsigned int water_table : 1; // srsly. wtf?
    unsigned int rained : 1; // does this mean actual rain (as in the blue blocks) or a wet tile?
    unsigned int traffic : 2; // needs enum
    unsigned int flow_forbid : 1; // idk wtf bbq
    unsigned int liquid_static : 1;// good idea there... poor execution
    unsigned int moss : 1;// I LOVE MOSS
    unsigned int feature_present : 1; // another wtf... is this required for magma pipes to work?
    unsigned int liquid_character : 2; // those ripples on streams?
    } bits;
};

// occupancy flags (rat,dwarf,horse,built wall,not build wall,etc)
union t_occupancy
{
    Uint32 whole;
    struct {
    unsigned int building : 3;// building type... should be an enum?
    unsigned int unit : 1;
    unsigned int unit_grounded : 1;
    unsigned int item : 1;
    // splatter. everyone loves splatter.
    unsigned int mud : 1;
    unsigned int vomit :1;
    unsigned int debris1 :1;
    unsigned int debris2 :1;
    unsigned int debris3 :1;
    unsigned int debris4 :1;
    unsigned int blood_g : 1;
    unsigned int blood_g2 : 1;
    unsigned int blood_b : 1;
    unsigned int blood_b2 : 1;
    unsigned int blood_y : 1;
    unsigned int blood_y2 : 1;
    unsigned int blood_m : 1;
    unsigned int blood_m2 : 1;
    unsigned int blood_c : 1;
    unsigned int blood_c2 : 1;
    unsigned int blood_w : 1;
    unsigned int blood_w2 : 1;
    unsigned int blood_o : 1;
    unsigned int blood_o2 : 1;
    unsigned int slime : 1;
    unsigned int slime2 : 1;
    unsigned int blood : 1;
    unsigned int blood2 : 1;
    unsigned int debris5 : 1;
    unsigned int snow : 1;
    } bits;
};


struct Block
{
    // generic tile type. determines how the tile behaves ingame
    Uint16 tile_type[BLOCK_SIZE][BLOCK_SIZE];
    t_designation designation[BLOCK_SIZE][BLOCK_SIZE];
    t_occupancy occupancy[BLOCK_SIZE][BLOCK_SIZE];
};

struct DfMap
{
public:
    unsigned x_block_count, y_block_count, z_block_count; // block count
    unsigned x_cell_count, y_cell_count, z_cell_count;    // cell count
    Block *** *block;
};

class Extractor
{
DECLARE_SINGLETON_CLASS(Extractor)

protected:
    bool MapLoaded;
    DfMap df_map;   // DF extracted map structure

    bool testMapData(DfMap df_map);
    void convertToDfMapCoords(int x, int y, int &out_x, int &out_y, int &out_x2, int &out_y2);
    void allocateBlocks(int x, int y);
    inline bool checkXYZValid (int x, int y, int z); // for getters to check block validity

    int pe_offset;
    int pe_timestamp;
    int map_offset;
    int x_count_offset;
    int y_count_offset;
    int z_count_offset;
    int tile_type_offset;
    int designation_offset;
    int occupancy_offset;

public:

    bool Init();
    ~Extractor();

    bool isMapLoaded()      { return MapLoaded; }

    bool dumpMemory();
    bool loadMap(const char* FilePath);
    bool writeMap(const char* FilePath);
    bool FreeMap();

    unsigned int getXBlocks()        { return df_map.x_block_count; }
    unsigned int getYBlocks()        { return df_map.y_block_count; }
    unsigned int getZBlocks()        { return df_map.z_block_count; }

    short int getTileType(int x, int y, int z);
    short int getTileType(int x, int y, int z, int blockX, int blockY);

    int getDesignations(int x, int y, int z);
    int getOccupancies(int x, int y, int z);

    int picktexture(int);

    bool isOpenTerrain(int);
    bool isStairTerrain(int);
    bool isRampTerrain(int);
    bool isFloorTerrain(int);
    bool isWallTerrain(int);

    bool isBlockInitialized(int x, int y, int z);

    bool isHidden (int x, int y, int z);
    bool isSubterranean (int x, int y, int z);
    bool isSkyView (int x, int y, int z);
    bool isSunLit (int x, int y, int z);
    bool isMagma (int x, int y, int z);

    int getLiquidLevel(int x, int y, int z);
};


#define EXTRACT (Extractor::GetInstance())

#endif // EXTRACT_HEADER
