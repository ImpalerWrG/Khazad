#ifndef DFMAP_H_INCLUDED
#define DFMAP_H_INCLUDED

#define BLOCK_SIZE 16

struct t_vein
{
    uint32_t vtable;
    int16_t type;
    int16_t assignment[16];
    int16_t unknown;
    uint32_t flags;
};

struct MatglossPair
{
    uint16_t type;
    uint16_t index;
};

// raw
struct t_construction_df40d
{
    int16_t x;
    int16_t y;
    int16_t z;
    int16_t unk1;
    int16_t unk2;
    int16_t mat_type;
    int16_t mat_idx;
    // not complete
};

// cooked
struct t_construction
{
    uint16_t x;
    uint16_t y;
    uint16_t z;
    int16_t mat_type;
    int16_t mat_idx;
};

//raw
struct t_building_df40d
{
    uint32_t vtable;
    uint32_t x1;
    uint32_t y1;
    uint32_t unk1;
    uint32_t x2;
    uint32_t y2;
    uint32_t unk2;
    uint32_t z;
    uint32_t some_flag;
    int16_t mat_type;
    int16_t mat_idx;
    // not complete
};

//cooked
struct t_building
{
    uint32_t type;
    uint32_t x1;
    uint32_t y1;
    uint32_t x2;
    uint32_t y2;
    uint32_t z;
    int16_t mat_type;
    int16_t mat_idx;
    // not complete
};

struct t_tree_desc
{
    int16_t mat_type;
    int16_t mat_idx;
    uint16_t x;
    uint16_t y;
    uint16_t z;
};

// in order in which the raw vectors appear in df memory
enum RawType
{
    Mat_Wood,
    Mat_Stone,
    Mat_Plant,
    Mat_Metal,
    NUM_MATGLOSS_TYPES
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
    MatglossPair material[BLOCK_SIZE][BLOCK_SIZE];
    vector<t_building*> v_buildings;
    vector<t_tree_desc*> v_trees;
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
    // allow extractor direct access to our data, avoid call lag and lots of self-serving methods
    friend class Extractor;

    Block **block;
    uint32_t blocks_allocated;
    bool valid;

    // converts the (x,y,z) cell coords to internal coords
    // out_y, out_x - block coords
    // out_y2, out_x2 - cell coords in that block
    inline void convertToDfMapCoords(uint32_t x, uint32_t y, uint32_t &out_x, uint32_t &out_y, uint32_t &out_x2, uint32_t &out_y2)
    {
        out_x = x / BLOCK_SIZE;
        out_x2 = x % BLOCK_SIZE;
        out_y = y / BLOCK_SIZE;
        out_y2 = y % BLOCK_SIZE;
    };

    void allocBlockArray(uint32_t x,uint32_t y, uint32_t z);
    void updateCellCount();

    bool loadVersion1(FILE * Decompressed,DfMapHeader & h);
    bool writeVersion1(FILE * SaveFile);

    bool loadMatgloss2(FILE * Decompressed);
    bool loadBlocks2(FILE * Decompressed,DfMapHeader & h);
    bool loadRegion2(FILE * Decompressed);
    bool loadVersion2(FILE * Decompressed,DfMapHeader & h);

    void writeMatgloss2(FILE * SaveFile);
    void writeBlocks2(FILE * SaveFile);
    void writeRegion2(FILE * SaveFile);
    bool writeVersion2(FILE * SaveFile);

    uint32_t regionX;
    uint32_t regionY;
    uint32_t regionZ;

    ///FIXME: these belong to some world structure
    uint32_t worldSizeX;
    uint32_t worldSizeY;

    vector<uint16_t> v_geology[eBiomeCount];
    vector<string> v_matgloss[NUM_MATGLOSS_TYPES];
    vector<string> v_buildingtypes;
    vector<t_construction> v_constructions;
    vector<t_building*> v_buildings;
    vector<t_tree_desc*> v_trees;
    unsigned x_block_count, y_block_count, z_block_count; // block count
    unsigned x_cell_count, y_cell_count, z_cell_count;    // cell count

public:

    DfMap();
    DfMap(uint32_t x, uint32_t y, uint32_t z);
    DfMap(string file_name);
    ~DfMap();

    /// TODO: rework matgloss
    void applyGeoMatgloss(Block * b);
    // accessing vectors of materials
    uint16_t getNumMatGloss(uint16_t type);
    string getMatGlossString(uint16_t type, uint16_t index);
    // accessing vectors of building types
    uint32_t getNumBuildingTypes();
    string getBuildingTypeName(uint32_t index);

    bool isValid();
    bool load(string FilePath);
    bool write(string FilePath);
    void clear();

    Block* getBlock(uint32_t x, uint32_t y, uint32_t z);
    Block* allocBlock(uint32_t x, uint32_t y, uint32_t z);
    bool   deallocBlock(uint32_t x, uint32_t y, uint32_t z);

    vector<t_building *> * getBlockBuildingsVector(uint32_t x,uint32_t y,uint32_t z);
    vector<t_tree_desc *> * getBlockVegetationVector(uint32_t x,uint32_t y,uint32_t z);

    inline unsigned int getXBlocks()        { return x_block_count; }
    inline unsigned int getYBlocks()        { return y_block_count; }
    inline unsigned int getZBlocks()        { return z_block_count; }

    bool isTileSky(uint32_t x, uint32_t y, uint32_t z, uint32_t blockX, uint32_t blockY);
    uint16_t getTileType(uint32_t x, uint32_t y, uint32_t z);
    uint16_t getTileType(uint32_t x, uint32_t y, uint32_t z, uint32_t blockX, uint32_t blockY);

    uint32_t getDesignations(uint32_t x, uint32_t y, uint32_t z);
    uint32_t getOccupancies(uint32_t x, uint32_t y, uint32_t z);

    // get tile material
    MatglossPair getMaterialPair (uint32_t x, uint32_t y, uint32_t z);
    string getGeoMaterialString (uint32_t x, uint32_t y, uint32_t z);
    string getMaterialString (uint32_t type, uint32_t index);

    // get coords of region used for materials
    void getGeoRegion (uint32_t x, uint32_t y, uint32_t z, int32_t& geoX, int32_t& geoY);

    // matgloss part of the designation
    uint32_t getGeolayerIndex (uint32_t x, uint32_t y, uint32_t z);

    void getRegionCoords (uint32_t &x,uint32_t &y,uint32_t &z);
    void setRegionCoords (uint32_t x,uint32_t y,uint32_t z);

    // what kind of building is here?
    //uint16_t getBuilding (uint32_t x, uint32_t y, uint32_t z);
    t_building *getBuilding (uint32_t x, uint32_t y, uint32_t z);
    t_tree_desc *getTree (uint32_t x, uint32_t y, uint32_t z);

    unsigned int getBiome (uint32_t x, uint32_t y, uint32_t z);

    int picktexture(int);

    bool isOpenTerrain(int);
    bool isStairTerrain(int);
    bool isRampTerrain(int);
    bool isFloorTerrain(int);
    bool isWallTerrain(int);

    bool isBlockInitialized(uint32_t x, uint32_t y, uint32_t z);

    bool isHidden (uint32_t x, uint32_t y, uint32_t z);
    bool isSubterranean (uint32_t x, uint32_t y, uint32_t z);
    bool isSkyView (uint32_t x, uint32_t y, uint32_t z);
    bool isSunLit (uint32_t x, uint32_t y, uint32_t z);
    bool isMagma (uint32_t x, uint32_t y, uint32_t z);

    uint8_t getLiquidLevel(uint32_t x, uint32_t y, uint32_t z);
};


#endif // DFMAP_H_INCLUDED
