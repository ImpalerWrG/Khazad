#include "DFCommon.h"
// zlib helper functions for de/compressing files
#include "ZlibHelper.h"
#include "DfMapHeader.h"

// some bounds checking in debug mode. used in asserts
#define CheckBounds x < x_cell_count && x >= 0 && y < y_cell_count && y >= 0 && z < z_block_count && z >= 0
#define CheckBoundsXY x < x_cell_count && x >= 0 && y < y_cell_count && y >= 0
#define CheckBlockBounds x < x_block_count && x >= 0 && y < y_block_count && y >= 0 && z < z_block_count && z >= 0

// this expands into lots of ugly switch statement functions. some of them unused?, but kept for reference
#include "TileTypes.h"

// process vein vector into matgloss values...
void Block::collapseVeins()
{
    // iterate through assigned veins
    for( uint32_t i = 0; i < veins.size(); i++)
    {
        t_vein v = veins[i];
        //iterate through vein assignment bit arrays - one for every row
        for(uint32_t j = 0;j<16;j++)
        {
            //iterate through the bits... I wish I could use assembly here... especially the carry bits
            for (uint32_t k = 0; k< 16;k++)
            {
                // and the bit array with a one-bit mask, check if the bit is set
                bool set = ((1 << k) & v.assignment[j]) >> k;
                if(set)
                {
                    material[k][j].type = Mat_Stone;
                    material[k][j].index = v.type;
                }
            }
        }
    }
}

DfMap::~DfMap()
{
    clear();
}
/// TODO: make this sync
void DfMap::clear()
{
    if(valid)
    {
        valid = false;
        for(uint32_t i = 0; i < x_block_count*y_block_count*z_block_count;i++)
        {
            Block * b = block[i];
            if(b!=NULL)
            {
                delete b;
            }
        }
        delete[] block;
    }
    for (uint32_t i = eNorthWest; i< eBiomeCount;i++)
    {
        geology[i].clear();
        geodebug[i].clear();
        geoblockadresses[i] = 0;
        regionadresses[i] = 0;
    }
    for(uint32_t counter = Mat_Wood; counter < NUM_MATGLOSS_TYPES; counter++)
    {
        matgloss[counter].clear();
    }
    blocks_allocated = 0;
}

void DfMap::getRegionCoords (uint32_t &x,uint32_t &y,uint32_t &z)
{
    x= regionX;
    y= regionY;
    z= regionZ;
}

void DfMap::setRegionCoords (uint32_t x,uint32_t y,uint32_t z)
{
    regionX = x;
    regionY = y;
    regionZ = z;
}

void DfMap::allocBlockArray(uint32_t x,uint32_t y, uint32_t z)
{
    clear();
    x_block_count = x;
    y_block_count = y;
    z_block_count = z;
    updateCellCount();
    block = new Block*[x_block_count*y_block_count*z_block_count];
    for (uint32_t i = 0; i < x_block_count*y_block_count*z_block_count; i++ )
    {
        block[i] = NULL;
    }
    blocks_allocated = 0;
    valid = true;
}

DfMap::DfMap(uint32_t x, uint32_t y, uint32_t z)
{
    valid = false;
    allocBlockArray(x,y,z);
}

DfMap::DfMap(string FileName)
{
    valid = false;
    valid = load( FileName);
}

bool DfMap::isValid ()
{
    return valid;
}

Block * DfMap::getBlock (uint32_t x,uint32_t y,uint32_t z)
{
    if(isValid())
    {
        return block[x*y_block_count*z_block_count + y*z_block_count + z];
    }
    return NULL;
}

Block * DfMap::allocBlock (uint32_t x,uint32_t y,uint32_t z)
{
    if(isValid())
    {
        if(block[x*y_block_count*z_block_count + y*z_block_count + z])
        {
            return block[x*y_block_count*z_block_count + y*z_block_count + z];
        }
        Block *b  = new Block;
        block[x*y_block_count*z_block_count + y*z_block_count + z] = b;
        blocks_allocated++;
        return b;
    }
    return NULL;
}

void DfMap::updateCellCount()
{
    x_cell_count = x_block_count * BLOCK_SIZE;
    y_cell_count = y_block_count * BLOCK_SIZE;
    z_cell_count = z_block_count;
}

void DfMap::applyGeoMatgloss(Block * b)
{
    // load layer matgloss
    for(int x_b = 0; x_b < BLOCK_SIZE; x_b++)
    {
        for(int y_b = 0; y_b < BLOCK_SIZE; y_b++)
        {
            int geolayer = b->designation[x_b][y_b].bits.geolayer_index;
            int biome = b->designation[x_b][y_b].bits.biome;
            b->material[x_b][y_b].type = Mat_Stone;
            b->material[x_b][y_b].index = geology[b->RegionOffsets[biome]][geolayer];
        }
    }
}

uint8_t DfMap::getLiquidLevel(uint32_t x, uint32_t y, uint32_t z)
{
    assert(CheckBounds);

    uint32_t x2, y2;
    convertToDfMapCoords(x, y, x, y, x2, y2);
    Block *b = getBlock(x,y,z);
    if(b != NULL)
    {
        return b->designation[x2][y2].bits.flow_size;
    }
    return 0;
}

uint16_t DfMap::getTileType(uint32_t x, uint32_t y, uint32_t z)
{
    assert(CheckBounds);

    uint32_t x2, y2;
    convertToDfMapCoords(x, y, x, y, x2, y2);
    Block *b = getBlock(x,y,z);
    if(b != NULL)
    {
        return b->tile_type[x2][y2];
    }
    if(isTileSky(x,y,z,x2,y2))
        return 32;
    return -1;
}

uint16_t DfMap::getTileType(uint32_t x, uint32_t y, uint32_t z, uint32_t blockX, uint32_t blockY)
{
    assert(CheckBlockBounds);
    Block *b = getBlock(x,y,z);
    if(b != NULL)
    {
        return b->tile_type[blockX][blockY];
    }
    if(isTileSky(x,y,z,blockX,blockY))
        return 32;
    return -1;
}

uint32_t DfMap::getDesignations(uint32_t x, uint32_t y, uint32_t z)
{
    assert(CheckBounds);
    uint32_t x2, y2;
    convertToDfMapCoords(x, y, x, y, x2, y2);
    Block *b = getBlock(x,y,z);
    if(b != NULL)
    {
        return b->designation[x2][y2].whole;
    }
    return -1;
}

uint32_t DfMap::getGeolayerAddress (uint32_t x, uint32_t y, uint32_t z)
{
    assert(CheckBoundsXY);
    uint32_t x2, y2;
    convertToDfMapCoords(x, y, x, y, x2, y2);
    Block *b = getBlock(x,y,z);
    if(b != NULL)
    {
        int biome = b->designation[x2][y2].bits.biome;
        int BiomeOffset = b->RegionOffsets[biome];
        unsigned int geoindex = b->designation[x2][y2].bits.geolayer_index;
        if(geodebug[BiomeOffset].size() > geoindex)
        {
            return geodebug[BiomeOffset][geoindex];
        }
        else
        {
            return 0;
        }
    }
    else return 0;
}

uint32_t DfMap::getGeoblockAddress(uint32_t x, uint32_t y, uint32_t z)
{
    assert(CheckBoundsXY);
    uint32_t x2, y2;
    convertToDfMapCoords(x, y, x, y, x2, y2);
    Block *b = getBlock(x,y,z);
    if(b != NULL)
    {
        int biome = b->designation[x2][y2].bits.biome;
        int BiomeOffset = b->RegionOffsets[biome];

        return geoblockadresses[BiomeOffset];
    }
    return 0;
}

uint32_t DfMap::getRegionAddress(uint32_t x, uint32_t y, uint32_t z)
{
    assert(CheckBoundsXY);
    uint32_t x2, y2;
    convertToDfMapCoords(x, y, x, y, x2, y2);
    Block *b = getBlock(x,y,z);
    if(b != NULL)
    {
        int biome = b->designation[x2][y2].bits.biome;
        int BiomeOffset = b->RegionOffsets[biome];

        return regionadresses[BiomeOffset];
    }
    return 0;
}

bool DfMap::isBlockInitialized(uint32_t x, uint32_t y, uint32_t z)
{
    // because of the way DfMap is done, more than one check must be made.
    return getBlock(x,y,z) != NULL;
}

uint32_t DfMap::getOccupancies(uint32_t x, uint32_t y, uint32_t z)
{
    assert(CheckBounds);

    uint32_t x2, y2;
    convertToDfMapCoords(x, y, x, y, x2, y2);
    Block *b = getBlock(x,y,z);
    if(b != NULL)
    {
        return b->occupancy[x2][y2].whole;
    }
    return -1;
}

void DfMap::getGeoRegion (uint32_t x, uint32_t y, uint32_t z, int32_t& geoX, int32_t& geoY)
{
    assert(CheckBoundsXY);
    uint32_t x2, y2;
    convertToDfMapCoords(x, y, x, y, x2, y2);
    Block *b = getBlock(x,y,z);
    if(b != NULL)
    {
        int biome = b->designation[x2][y2].bits.biome;
        int BiomeOffset = b->RegionOffsets[biome];
        int16_t X_biomeB = (regionX / 16) + (BiomeOffset % 3) - 1;
        int16_t Y_biomeB = (regionY / 16) + (BiomeOffset / 3) - 1;
        if(X_biomeB < 0) X_biomeB = 0;
        if(Y_biomeB < 0) Y_biomeB = 0;
        if( (uint32_t)X_biomeB >= worldSizeX)
            X_biomeB = worldSizeX - 1;
        if( (uint32_t)Y_biomeB >= worldSizeY)
            Y_biomeB = worldSizeY - 1;
        geoX = X_biomeB;
        geoY = Y_biomeB;
    }
    else
    {
        geoX = regionX / 16;
        geoY = regionY / 16;
    }
}

// this is what the vein structures say it is
MatglossPair DfMap::getMaterialPair (uint32_t x, uint32_t y, uint32_t z)
{
    assert(CheckBounds);

    uint32_t x2, y2;
    convertToDfMapCoords(x, y, x, y, x2, y2);
    Block *b = getBlock(x,y,z);
    if(b != NULL)
    {
        return b->material[x2][y2];
    }
    MatglossPair fail = {-1,-1};
    return fail;
};


// this is what the vein structures say it is
string DfMap::getMaterialString (uint32_t x, uint32_t y, uint32_t z)
{
    assert(CheckBounds);

    uint32_t x2, y2;
    convertToDfMapCoords(x, y, x, y, x2, y2);
    Block *b = getBlock(x,y,z);
    if(b != NULL)
    {
        /*// if matgloss loaded
        if(stone_matgloss.size() != 0)
        {*/
        uint16_t type = b->material[x2][y2].type;
        uint16_t index = b->material[x2][y2].index;
        if(index != 65535)
        {
            if(index < matgloss[type].size())
            {
                return matgloss[type][index];
            }
            else
            {
                string fallback = "ERROR";
                return fallback;
            }
        }
        //}
    }
    string fallback = "UNKNOWN";
    return fallback;
}

uint16_t DfMap::getNumMatGloss(uint16_t type)
{
    return matgloss[type].size();
}

string DfMap::getMatGlossString(uint16_t type,uint16_t index)
{
    if(index < matgloss[type].size())
    {
        return matgloss[type][index];
    }
    return string("error");
}

// matgloss part of the designation
unsigned int DfMap::getGeolayerIndex (uint32_t x, uint32_t y, uint32_t z)
{
    assert(CheckBounds);

    uint32_t x2, y2;
    convertToDfMapCoords(x, y, x, y, x2, y2);
    Block *b = getBlock(x,y,z);
    if(b != NULL)
    {
        return b->designation[x2][y2].bits.geolayer_index;
    }
    return -1;
}
// matgloss part of the designation
unsigned int DfMap::getBiome (uint32_t x, uint32_t y, uint32_t z)
{
    assert(CheckBounds);

    uint32_t x2, y2;
    convertToDfMapCoords(x, y, x, y, x2, y2);
    Block *b = getBlock(x,y,z);
    if(b != NULL)
    {
        return b->designation[x2][y2].bits.biome;
    }
    return -1;
}

// what kind of building is here?
uint16_t DfMap::getBuilding (uint32_t x, uint32_t y, uint32_t z)
{
    assert(CheckBounds);
    uint32_t x2, y2;
    convertToDfMapCoords(x, y, x, y, x2, y2);
    Block *b = getBlock(x,y,z);
    if(b != NULL)
    {
        return b->occupancy[x2][y2].bits.building;
    }
    return -1;
}

bool DfMap::isHidden (uint32_t x, uint32_t y, uint32_t z)
{
    assert(CheckBounds);
    uint32_t x2, y2;
    convertToDfMapCoords(x, y, x, y, x2, y2);
    Block *b = getBlock(x,y,z);
    if(b != NULL)
    {
        return (b->designation[x2][y2].bits.hidden);
    }
    return false;
}

bool DfMap::isSubterranean (uint32_t x, uint32_t y, uint32_t z)
{
    assert(CheckBounds);
    uint32_t x2, y2;
    convertToDfMapCoords(x, y, x, y, x2, y2);
    Block *b = getBlock(x,y,z);
    if(b != NULL)
    {
        return (b->designation[x2][y2].bits.subterranean);
    }
    if(isTileSky( x, y, z, x2, y2))
        return false;
    return true;
}

// x,y,z - coords of block
// blockX,blockY - coords of tile inside block
bool DfMap::isTileSky(uint32_t x, uint32_t y, uint32_t z, uint32_t blockX, uint32_t blockY)
{
    assert(CheckBounds);
    Block *b;
    // trace down through blocks until we hit an inited one or the base
    for (int i = z; i>= 0;i--)
    {
        b = getBlock(x,y,i);
        if(b)
        {
            // is the inited block open to the sky?
            return b->designation[blockX][blockY].bits.skyview;
        }
    }
    // we hit base
    return false;
}

// is the sky above this tile visible?
bool DfMap::isSkyView (uint32_t x, uint32_t y, uint32_t z)
{
    assert(CheckBounds);
    uint32_t x2, y2;
    convertToDfMapCoords(x, y, x, y, x2, y2);
    Block *b = getBlock(x,y,z);
    if(b != NULL)
    {
        return (b->designation[x2][y2].bits.skyview);
    }
    if(isTileSky(x,y,z,x2,y2))
        return true;
    return false;
}

// is there light in this tile?
bool DfMap::isSunLit (uint32_t x, uint32_t y, uint32_t z)
{
    assert(CheckBounds);
    uint32_t x2, y2;
    convertToDfMapCoords(x, y, x, y, x2, y2);
    Block *b = getBlock(x,y,z);
    if(b != NULL)
    {
        return (b->designation[x2][y2].bits.light);
    }
    return false;
}

bool DfMap::isMagma (uint32_t x, uint32_t y, uint32_t z)
{
    assert(CheckBounds);
    uint32_t x2, y2;
    convertToDfMapCoords(x, y, x, y, x2, y2);
    Block *b = getBlock(x,y,z);
    if(b != NULL)
    {
        return (b->designation[x2][y2].bits.liquid_type);
    }
    return false;
}

// converts the (x,y,z) cell coords to internal coords
// out_y, out_x - block coords
// out_y2, out_x2 - cell coords in that block
void DfMap::convertToDfMapCoords(uint32_t x, uint32_t y, uint32_t &out_x, uint32_t &out_y, uint32_t &out_x2, uint32_t &out_y2)
{
    out_x2 = x % BLOCK_SIZE;
    out_y2 = y % BLOCK_SIZE;
    out_y = uint32_t(y / BLOCK_SIZE);
    out_x = uint32_t(x / BLOCK_SIZE);
}
