#ifndef DFMAP_H_INCLUDED
#define DFMAP_H_INCLUDED

#define BLOCK_SIZE 16

struct t_vein
{
    uint32_t VTable;
    int16_t type;
    int16_t assignment[16];
    int16_t unknown;
    uint32_t flags;
};

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

class DfMapHeader;

/// TODO: research this further? consult DF hacker wizards?
union t_designation
{
    uint32_t whole;
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
    uint32_t whole;
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
    uint32_t origin;
    // generic tile type. determines how the tile behaves ingame
    uint16_t tile_type[BLOCK_SIZE][BLOCK_SIZE];
    t_designation designation[BLOCK_SIZE][BLOCK_SIZE];
    t_occupancy occupancy[BLOCK_SIZE][BLOCK_SIZE];
    // veins
    vector <t_vein> veins;
    uint16_t material[BLOCK_SIZE][BLOCK_SIZE];
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
    uint8_t RegionOffsets[16];// idk if the length is right here
};
/**
 * This class can load and save DF maps
 */
class DfMap
{
private:

    Block **block;
    uint32_t blocks_read;
    bool valid;

    void convertToDfMapCoords(int x, int y, int &out_x, int &out_y, int &out_x2, int &out_y2);
    void allocBlockArray(int x,int y, int z);
    void updateCellCount();
    uint32_t getBlocksCount() const { return blocks_read; }
    bool loadVersion1(FILE * Decompressed,DfMapHeader & h);
    bool loadVersion2(FILE * Decompressed,DfMapHeader & h);
    bool writeVersion1(FILE * SaveFile);
    bool writeVersion2(FILE * SaveFile);
public:

/// TODO: refactor *REALLY* needed. Time to go back to drawing board
    void applyMatgloss(Block * b);
    uint32_t regionX;
    uint32_t regionY;
    uint32_t regionZ;
    vector<uint16_t> geology[eBiomeCount];
    vector<uint32_t> geodebug[eBiomeCount];
    uint32_t geoblockadresses[eBiomeCount];
    uint32_t regionadresses[eBiomeCount];

    DfMap();
    DfMap(uint32_t x, uint32_t y, uint32_t z);
    DfMap(string file_name);
    ~DfMap();

    uint16_t getNumStoneMatGloss();
    string getStoneMatGlossString(uint16_t Index);
    vector<string> stone_matgloss;

    bool isValid();
    bool load(string FilePath);
    bool write(string FilePath);
    void clear();

    Block* getBlock(int x, int y, int z);
    Block* allocBlock(int x, int y, int z);
    bool   deallocBlock(int x, int y, int z);

    unsigned x_block_count, y_block_count, z_block_count; // block count
    unsigned x_cell_count, y_cell_count, z_cell_count;    // cell count


    void setBlocksCount(uint32_t p_blocks_read) { blocks_read = p_blocks_read; }

    unsigned int getXBlocks()        { return x_block_count; }
    unsigned int getYBlocks()        { return y_block_count; }
    unsigned int getZBlocks()        { return z_block_count; }

    bool isTileSky(int x, int y, int z, int blockX, int blockY);
    short int getTileType(int x, int y, int z);
    short int getTileType(int x, int y, int z, int blockX, int blockY);

    int getDesignations(int x, int y, int z);
    int getOccupancies(int x, int y, int z);

    // get tile material
    int16_t getMaterialIndex (int x, int y, int z);
    string getMaterialString (int x, int y, int z);

    // get coords of region used for materials
    void getGeoRegion (int x, int y, int z, int& geoX, int& geoY);

    // matgloss part of the designation
    unsigned int getGeolayerIndex (int x, int y, int z);

    // terrible
    uint32_t getGeolayerAddress (int x, int y, int z);
    uint32_t getGeoblockAddress (int x, int y, int z);
    uint32_t getRegionAddress(int x, int y, int z);

    void getRegionCoords (int &x,int &y,int &z);
    void setRegionCoords (int x,int y,int z);

    // what kind of building is here?
    uint16_t getBuilding (int x, int y, int z);
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
