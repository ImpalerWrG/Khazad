#ifndef DFMAP_H_INCLUDED
#define DFMAP_H_INCLUDED

#define BLOCK_SIZE 16

#include <stdafx.h>

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
    Uint32 origin;
    Uint16 tile_type[BLOCK_SIZE][BLOCK_SIZE];
    t_designation designation[BLOCK_SIZE][BLOCK_SIZE];
    t_occupancy occupancy[BLOCK_SIZE][BLOCK_SIZE];
};

class DfMap
{
private:
    Block **block;
    bool valid;
    void convertToDfMapCoords(int x, int y, int &out_x, int &out_y, int &out_x2, int &out_y2);
    void allocBlockArray(int x,int y, int z);
    void updateCellCount();
public:
    DfMap();
    DfMap(Uint32 x, Uint32 y, Uint32 z);
    DfMap(string file_name);
    ~DfMap();

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

    unsigned int getXBlocks()        { return x_block_count; }
    unsigned int getYBlocks()        { return y_block_count; }
    unsigned int getZBlocks()        { return z_block_count; }

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


#endif // DFMAP_H_INCLUDED
