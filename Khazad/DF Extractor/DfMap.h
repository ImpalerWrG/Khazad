#ifndef DFMAP_H_INCLUDED
#define DFMAP_H_INCLUDED

#define BLOCK_SIZE 16

#include <stdafx.h>
#include <DataStructures.h>

enum BiomeOffset
{
    eNorthWest,
    eNorth,
    eNorthEast,
    eWest,
    eHere,
    eEast,
    eSouthWest,
    eSouth,
    eSouthEast,
    eBiomeCount
};



/// TODO: research this further? consult DF hacker wizards?
union t_designation
{
    Uint32 whole;
    struct {
    unsigned int flow_size : 3; // how much liquid is here?
    unsigned int pile : 1; // stockpile?
/*
 * All the different dig designations... needs more info, probably an enum
 */
    unsigned int dig : 3;
    unsigned int detail : 1;///<- wtf
    unsigned int detail_event : 1;///<- more wtf
    unsigned int hidden :1;

/*
 * This one is rather involved, but necessary to retrieve the base layer matgloss index
 * see http://www.bay12games.com/forum/index.php?topic=608.msg253284#msg253284 for details
 */
    unsigned int geolayer_index :4;
    unsigned int light : 1;
    unsigned int subterranean : 1; // never seen the light of day?
    unsigned int skyview : 1; // sky is visible now, it rains in here when it rains

/*
 * Probably similar to the geolayer_index. Only with a different set of offsets and different data.
 * we don't use this yet
 */
    unsigned int biome : 4;
/*
0 = water
1 = magma
*/
    unsigned int liquid_type : 1;
    unsigned int water_table : 1; // srsly. wtf?
    unsigned int rained : 1; // does this mean actual rain (as in the blue blocks) or a wet tile?
    unsigned int traffic : 2; // needs enum
    unsigned int flow_forbid : 1; // idk wtf bbq
    unsigned int liquid_static : 1;
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
    // 7 = door
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

class Block
{
public:
    // where does the Block come from?
    Uint32 origin;
    // generic tile type. determines how the tile behaves ingame
    Uint16 tile_type[BLOCK_SIZE][BLOCK_SIZE];
    t_designation designation[BLOCK_SIZE][BLOCK_SIZE];
    t_occupancy occupancy[BLOCK_SIZE][BLOCK_SIZE];
    // veins
    vector <t_vein> veins;
    Uint16 material[BLOCK_SIZE][BLOCK_SIZE];
    void collapseVeins();
    /**
    // region offset modifiers... what a hack.
    // here we have double indexed offset into regions.
    // once inside t_designation, pointing into this, second time from here as a index modifier into region array (2d)
    // disassembled code where it's used follows. biome is biome from t_designation
    biome_stuffs = *(_BYTE *)((char)biome + offset_Block + 0x1D84);
    biome_stuffs_mod3 = biome_stuffs % 3;
    biome_stuffs_div3 = biome_stuffs / 3;
    biome_stuffs_mod3_ = biome_stuffs_mod3;
    if ( !biome_stuffs_mod3_ )
      --*(_WORD *)X_stuff;
    if ( biome_stuffs_mod3_ == 2 )
      ++*(_WORD *)X_stuff;
    if ( !biome_stuffs_div3 )
      --*(_WORD *)Y_stuff_;
    if ( biome_stuffs_div3 == 2 )
      ++*(_WORD *)Y_stuff_;
    */
    Uint8 RegionOffsets[16];// idk if the length is right here
};

class DfMap
{
private:

    Block **block;
    Uint32 blocks_read;
    bool valid;

    void convertToDfMapCoords(int x, int y, int &out_x, int &out_y, int &out_x2, int &out_y2);
    void allocBlockArray(int x,int y, int z);
    void updateCellCount();
    Uint32 getBlocksCount() const { return blocks_read; }

public:

/// TODO: refactor needed. next 4 lines are the proof
    Uint32 regionX;
    Uint32 regionY;
    Uint32 regionZ;
    vector<Uint16> geology[eBiomeCount];

    DfMap();
    DfMap(Uint32 x, Uint32 y, Uint32 z);
    DfMap(string file_name);
    ~DfMap();

    Uint16 getNumStoneMatGloss();
    string getStoneMatGlossString(Uint16 Index);
    vector<string> stone_matgloss;

    bool isValid();
    bool load(string FilePath);
    bool write(string FilePath);
    void clear();

    // fix external indexes into df_map. HACK
    void clamp(int &x,int &y, int &z);

    Block* getBlock(int x, int y, int z);
    Block* allocBlock(int x, int y, int z);
    bool   deallocBlock(int x, int y, int z);

    unsigned x_block_count, y_block_count, z_block_count; // block count
    unsigned x_cell_count, y_cell_count, z_cell_count;    // cell count


    void setBlocksCount(Uint32 p_blocks_read) { blocks_read = p_blocks_read; }

    unsigned int getXBlocks()        { return x_block_count; }
    unsigned int getYBlocks()        { return y_block_count; }
    unsigned int getZBlocks()        { return z_block_count; }

    bool isTileSky(int x, int y, int z, int blockX, int blockY);
    short int getTileType(int x, int y, int z);
    short int getTileType(int x, int y, int z, int blockX, int blockY);

    int getDesignations(int x, int y, int z);
    int getOccupancies(int x, int y, int z);

    // get tile material
    Sint16 getMaterialIndex (int x, int y, int z);
    string getMaterialString (int x, int y, int z);

    // get coords of region used for materials
    void getGeoRegion (int x, int y, int z, int& geoX, int& geoY);

    // matgloss part of the designation
    unsigned int getGeolayerIndex (int x, int y, int z);

    void getRegionCoords (int &x,int &y,int &z);
    void setRegionCoords (int x,int y,int z);

    // what kind of building is here?
    Uint16 getBuilding (int x, int y, int z);
    unsigned int getBiome (int x, int y, int z);

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


#endif // DFMAP_H_INCLUDED
