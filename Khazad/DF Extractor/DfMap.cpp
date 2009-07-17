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
/// TODO: check how this behaves with multiple overlapping veins
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
                if(set) material[k][j] = v.type;
            }
        }
    }
}

DfMap::~DfMap()
{
    clear();
}

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
}

void DfMap::getRegionCoords (int &x,int &y,int &z)
{
    x= regionX;
    y= regionY;
    z= regionZ;
}

void DfMap::setRegionCoords (int x,int y,int z)
{
    regionX = x;
    regionY = y;
    regionZ = z;
}

void DfMap::allocBlockArray(int x,int y, int z)
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

Block * DfMap::getBlock (int x,int y,int z)
{
    if(isValid())
    {
        return block[x*y_block_count*z_block_count + y*z_block_count + z];
    }
    return NULL;
}

Block * DfMap::allocBlock (int x,int y,int z)
{
    if(isValid())
    {
        if(block[x*y_block_count*z_block_count + y*z_block_count + z])
        {
            return block[x*y_block_count*z_block_count + y*z_block_count + z];
        }
        Block *b  = new Block;
        block[x*y_block_count*z_block_count + y*z_block_count + z] = b;
        return b;
    }
    return NULL;
}

bool DfMap::writeVersion1(FILE * SaveFile)
{
    uint32_t x, y, z;

    for (x = 0; x < x_block_count; x++ )
    {
        for (y = 0; y < y_block_count; y++ )
        {
            for (z = 0; z < z_block_count; z++ )
            {
                Block *b = getBlock(x,y,z);
                if(b != NULL)
                {
                    // which block it is
                    fwrite(&x, sizeof(uint32_t), 1, SaveFile);
                    fwrite(&y, sizeof(uint32_t), 1, SaveFile);
                    fwrite(&z, sizeof(uint32_t), 1, SaveFile);
                    // block data
                    fwrite(&b->tile_type, sizeof(uint16_t), BLOCK_SIZE*BLOCK_SIZE, SaveFile);
                    fwrite(&b->designation, sizeof(uint32_t), BLOCK_SIZE*BLOCK_SIZE, SaveFile);
                    fwrite(&b->occupancy, sizeof(uint32_t), BLOCK_SIZE*BLOCK_SIZE, SaveFile);
                }
            }
        }
    }
    return true;
}
bool DfMap::writeVersion2(FILE * SaveFile)
{
    uint32_t temp, temp2;
    uint32_t x, y, z;
    uint32_t numveins;
    // write matgloss vector
    uint32_t nummatgloss = stone_matgloss.size();
    fwrite(&nummatgloss, sizeof(uint32_t), 1, SaveFile);
    for(uint32_t i = 0; i< nummatgloss;i++)
    {
        const char *saveme = stone_matgloss[i].c_str();
        uint32_t length = stone_matgloss[i].size();
        fwrite(&length, sizeof(uint32_t),1,SaveFile);
        fwrite(saveme, sizeof(char), length, SaveFile);
    }
    // write region data known for version 2 format
    for(uint32_t i = eNorthWest; i< eBiomeCount;i++)
    {
        temp = geology[i].size();
        fwrite(&temp, sizeof(uint32_t), 1, SaveFile); // layer vector length
        for(uint32_t j = 0; j < temp;j++) // write all geolayers (just 16bit matgloss indices)
        {
            temp2 = geology[i][j];
            fwrite(&temp2, sizeof(uint16_t), 1, SaveFile);
        }
    }
    // write blocks
    for (x = 0; x < x_block_count; x++ )
    {
        for (y = 0; y < y_block_count; y++ )
        {
            for (z = 0; z < z_block_count; z++ )
            {
                Block *b = getBlock(x,y,z);
                if(b != NULL)
                {
                    // which block it is
                    fwrite(&x, sizeof(uint32_t), 1, SaveFile);
                    fwrite(&y, sizeof(uint32_t), 1, SaveFile);
                    fwrite(&z, sizeof(uint32_t), 1, SaveFile);
                    // block data
                    fwrite(&b->tile_type, sizeof(uint16_t), BLOCK_SIZE*BLOCK_SIZE, SaveFile);
                    fwrite(&b->designation, sizeof(uint32_t), BLOCK_SIZE*BLOCK_SIZE, SaveFile);
                    fwrite(&b->occupancy, sizeof(uint32_t), BLOCK_SIZE*BLOCK_SIZE, SaveFile);
                    fwrite(&b->RegionOffsets, sizeof(b->RegionOffsets), 1, SaveFile);
                    // write all veins
                    numveins = b->veins.size();
                    fwrite(&numveins, sizeof(uint32_t), 1, SaveFile);
                    for(uint32_t i = 0; i < numveins; i++)
                    {
                        fwrite(&b->veins[i],sizeof(t_vein),1,SaveFile);
                    }
                }
            }
        }
    }
    return true;
}

bool DfMap::write(string FilePath)
{
    FILE *SaveFile;
    SaveFile = fopen(FilePath.c_str(),"wb");
    DfMapHeader df_map_header;

    if(SaveFile == NULL)
    {
        printf("Can\'t create file for write.\n");
        return false;
    }
    else
    {
        // gather information to fill dfmapheader
        strcpy(df_map_header.identifier, dmh_id);
        df_map_header.version = dmh_ver;
        df_map_header.tile_block_count = getBlocksCount();
        df_map_header.x_block_count = getXBlocks();
        df_map_header.y_block_count = getYBlocks();
        df_map_header.z_block_count = getZBlocks();
        df_map_header.map_data_location = sizeof(DfMapHeader);

        // save map header
        fwrite(&df_map_header, sizeof(DfMapHeader), 1, SaveFile);
        switch(dmh_ver)
        {
            case 1:
                writeVersion1(SaveFile);
                break;
            case 2:
                writeVersion2(SaveFile);
                break;
        }
    }


    // reopen file for reading
    freopen (FilePath.c_str(),"rb",SaveFile);
    if(SaveFile == NULL)
    {
        printf("Can\'t create file for read.\n");
        return false;
    }


    FILE *SaveCompressedFile;
    string CompressedFilePath = FilePath + ".comp";

    SaveCompressedFile = fopen(CompressedFilePath.c_str(),"wb");
    if(SaveCompressedFile == NULL)
    {
        printf("Can\'t create a compressed file for write\n");
        return false;
    }

    // compress
    printf("Compressing... ");
    int ret = def(SaveFile, SaveCompressedFile, Z_BEST_COMPRESSION);

    printf("DONE\n");

    if (ret != Z_OK)
    {
        zerr(ret);
    }

    fclose(SaveFile);
    fclose(SaveCompressedFile);

    remove(FilePath.c_str());
    rename(CompressedFilePath.c_str(), FilePath.c_str());

    return true;
}

void DfMap::updateCellCount()
{
    x_cell_count = x_block_count * BLOCK_SIZE;
    y_cell_count = y_block_count * BLOCK_SIZE;
    z_cell_count = z_block_count;
}

void DfMap::applyMatgloss(Block * b)
{
    // load layer matgloss
    for(int x_b = 0; x_b < BLOCK_SIZE; x_b++)
    {
        for(int y_b = 0; y_b < BLOCK_SIZE; y_b++)
        {
            int geolayer = b->designation[x_b][y_b].bits.geolayer_index;
            int biome = b->designation[x_b][y_b].bits.biome;
            b->material[x_b][y_b] = geology[b->RegionOffsets[biome]][geolayer];
        }
    }
}

bool DfMap::loadVersion1(FILE * Decompressed,DfMapHeader & h)
{
    uint32_t x, y, z;
    // load new size information
    x_block_count = h.x_block_count;
    y_block_count = h.y_block_count;
    z_block_count = h.z_block_count;
    // make sure those size variables are in sync
    updateCellCount();
    // alloc new space for our new size
    allocBlockArray(x_block_count,y_block_count,z_block_count);

    fseek(Decompressed, h.map_data_location, SEEK_SET);

    for (uint32_t tile_block = 0U; tile_block < h.tile_block_count; ++tile_block)
    {
        fread(&x, sizeof(uint32_t), 1, Decompressed);
        fread(&y, sizeof(uint32_t), 1, Decompressed);
        fread(&z, sizeof(uint32_t), 1, Decompressed);

        Block * b = allocBlock(x,y,z);

        fread(&b->tile_type, sizeof(b->tile_type), 1, Decompressed);
        fread(&b->designation, sizeof(b->designation), 1, Decompressed);
        fread(&b->occupancy, sizeof(b->occupancy), 1, Decompressed);
        memset(b->material, -1, sizeof(b->material));
        // can't load offsets, substitute local biome everywhere
        memset(b->RegionOffsets,eHere,sizeof(b->RegionOffsets));
    }

    printf("Blocks read into memory: %d\n", h.tile_block_count);
    return true;
}
bool DfMap::loadVersion2(FILE * Decompressed,DfMapHeader & h)
{
    uint32_t x, y, z;
    uint32_t temp, temp2;
    uint32_t numveins;
    uint32_t nummatgloss;
    t_vein vein;
    char buffer [256];
    // load new size information
    x_block_count = h.x_block_count;
    y_block_count = h.y_block_count;
    z_block_count = h.z_block_count;
    // make sure those size variables are in sync
    updateCellCount();
    // alloc new space for our new size
    allocBlockArray(x_block_count,y_block_count,z_block_count);

    fseek(Decompressed, h.map_data_location, SEEK_SET);

    // read matgloss vector
    fread(&nummatgloss, sizeof(uint32_t), 1, Decompressed);
    ///FIXME: buffer overrun possible? probably not. but fix it anyway.
    for(uint32_t i = 0; i< nummatgloss;i++)
    {
        fread(&temp, sizeof(uint32_t), 1, Decompressed); // string length
        fread(&buffer, sizeof(char), temp, Decompressed); // string
        buffer[temp] = 0;
        stone_matgloss.push_back(buffer);
    }
    // read region data known for version 2 format
    for(uint32_t i = eNorthWest; i< eBiomeCount;i++)
    {
        fread(&temp, sizeof(uint32_t), 1, Decompressed); // layer vector length
        for(uint32_t j = 0; j < temp;j++) // load all geolayers into vectors (just 16bit matgloss indices)
        {
            fread(&temp2, sizeof(uint16_t), 1, Decompressed);
            geology[i].push_back(temp2);
        }
    }
    // read blocks
    for (uint32_t tile_block = 0U; tile_block < h.tile_block_count; ++tile_block)
    {
        fread(&x, sizeof(uint32_t), 1, Decompressed);
        fread(&y, sizeof(uint32_t), 1, Decompressed);
        fread(&z, sizeof(uint32_t), 1, Decompressed);

        Block * b = allocBlock(x,y,z);

        fread(&b->tile_type, sizeof(b->tile_type), 1, Decompressed);
        fread(&b->designation, sizeof(b->designation), 1, Decompressed);
        fread(&b->occupancy, sizeof(b->occupancy), 1, Decompressed);
        fread(&b->RegionOffsets,sizeof(b->RegionOffsets),1,Decompressed);
        // load all veins of this block
        fread(&numveins, sizeof(uint32_t), 1, Decompressed);
        if(nummatgloss)
        {
            applyMatgloss(b);
        }
        for(uint32_t i = 0; i < numveins; i++)
        {
            fread(&vein,sizeof(t_vein),1,Decompressed);
            b->veins.push_back(vein);
        }
        if(numveins)
            b->collapseVeins();
    }

    printf("Blocks read into memory: %d\n", h.tile_block_count);
    return true;
}

///TODO: this should use streams and filters
bool DfMap::load(string FilePath)
{
    string DecompressedFilePath = FilePath + ".decomp";
    FILE *ToDecompress;
    FILE *Decompressed;
    DfMapHeader df_map_header;

    // open target file for writing
    Decompressed = fopen(DecompressedFilePath.c_str(), "wb");
    if  (Decompressed == NULL)
    {
        printf("Can\'t open a decompressed file for write.\n");
        return false;
    }
    //decompress
    ToDecompress = fopen(FilePath.c_str(),"rb");
    if  (ToDecompress == NULL)
    {
        printf("Can\'t open file for read.\n");
        return false;
    }

    // Decompress
    printf("Decompressing... ");
    int ret = inf(/*source*/ToDecompress,/*destination*/Decompressed);

    printf("DONE\n");

    if (ret != Z_OK)
    {
        zerr(ret);
    }

    fclose(ToDecompress);
    // reopen decompressed file for reading
    freopen(DecompressedFilePath.c_str(), "rb", Decompressed);
    if  (Decompressed == NULL)
    {
        printf("Can\'t create decompressed file for read.\n");
        return false;
    }
    // delete all stuff before we change size
    clear();

    // check, if the file is big enough to contain the header
    fseek(Decompressed, 0, SEEK_END);

    if (ftell(Decompressed) < sizeof(DfMapHeader))
    {
    	printf("This Khazad map file is corrupted - file too small.\n");
    	return false;
    }

    // read the header
    fseek(Decompressed, 0, SEEK_SET);
    fread(&df_map_header, sizeof(DfMapHeader), 1, Decompressed);

    // check, if it's a Khazad map file
    if (strcmp(df_map_header.identifier,dmh_id) != 0)
    {
    	printf("This file is not a Khazad map file.\n");
    	return false;
    }

    switch(df_map_header.version)
    {
        /*
         * Basic format without matgloss. Kept for compatibility reasons.
         * Saved from version 0.0.5
         */
        case 1:
            loadVersion1(Decompressed, df_map_header);
            break;
        /*
         * Newer format
         * Saved from version 0.0.6
         */
        case 2:
            loadVersion2(Decompressed, df_map_header);
            break;
        default:
            printf("Unknown Khazad map file version(%3d).\n", df_map_header.version);
            return false;
    }
    // close reopened file
    fclose(Decompressed);
    // and delete it
    remove(DecompressedFilePath.c_str());
    return true;
}

int DfMap::getLiquidLevel(int x, int y, int z)
{
    assert(CheckBounds);

    int x2, y2;
    convertToDfMapCoords(x, y, x, y, x2, y2);
    Block *b = getBlock(x,y,z);
    if(b != NULL)
    {
        return b->designation[x2][y2].bits.flow_size;
    }
    return 0;
}

short int DfMap::getTileType(int x, int y, int z)
{
    assert(CheckBounds);

    int x2, y2;
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

short int DfMap::getTileType(int x, int y, int z, int blockX, int blockY)
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

int DfMap::getDesignations(int x, int y, int z)
{
    assert(CheckBounds);
    int x2, y2;
    convertToDfMapCoords(x, y, x, y, x2, y2);
    Block *b = getBlock(x,y,z);
    if(b != NULL)
    {
        return b->designation[x2][y2].whole;
    }
    return -1;
}

uint32_t DfMap::getGeolayerAddress (int x, int y, int z)
{
    assert(CheckBoundsXY);
    int x2, y2;
    convertToDfMapCoords(x, y, x, y, x2, y2);
    Block *b = getBlock(x,y,z);
    if(b != NULL)
    {
        int biome = b->designation[x2][y2].bits.biome;
        int BiomeOffset = b->RegionOffsets[biome];
        /*int16_t X_biomeB = (regionX / 16) + (BiomeOffset % 3) - 1;
        int16_t Y_biomeB = (regionY / 16) + (BiomeOffset / 3) - 1;
        if(X_biomeB < 0) X_biomeB = 0;
        if(Y_biomeB < 0) Y_biomeB = 0;*/
        ///FIXME: check and fix upper bounds here. this is important for embarks on the edge of the world! We need world size for that.
        int geoindex = b->designation[x2][y2].bits.geolayer_index;
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

uint32_t DfMap::getGeoblockAddress(int x, int y, int z)
{
    assert(CheckBoundsXY);
    int x2, y2;
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

uint32_t DfMap::getRegionAddress(int x, int y, int z)
{
    assert(CheckBoundsXY);
    int x2, y2;
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

bool DfMap::isBlockInitialized(int x, int y, int z)
{
    // because of the way DfMap is done, more than one check must be made.
    return getBlock(x,y,z) != NULL;
}

int DfMap::getOccupancies(int x, int y, int z)
{
    assert(CheckBounds);

    int x2, y2;
    convertToDfMapCoords(x, y, x, y, x2, y2);
    Block *b = getBlock(x,y,z);
    if(b != NULL)
    {
        return b->occupancy[x2][y2].whole;
    }
    return -1;
}

void DfMap::getGeoRegion (int x, int y, int z, int& geoX, int& geoY)
{
    assert(CheckBoundsXY);
    int x2, y2;
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
        ///FIXME: check and fix upper bounds here. this is important for embarks on the edge of the world! We need world size for that.
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
int16_t DfMap::getMaterialIndex (int x, int y, int z)
{
    assert(CheckBounds);

    int x2, y2;
    convertToDfMapCoords(x, y, x, y, x2, y2);
    Block *b = getBlock(x,y,z);
    if(b != NULL)
    {
        return b->material[x2][y2];
    }
    return -1;
}

// this is what the vein structures say it is
string DfMap::getMaterialString (int x, int y, int z)
{
    assert(CheckBounds);

    int x2, y2;
    convertToDfMapCoords(x, y, x, y, x2, y2);
    Block *b = getBlock(x,y,z);
    if(b != NULL)
    {
        // if matgloss loaded
        if(stone_matgloss.size() != 0)
        {
            // if it's a vein
            if(b->material[x2][y2] != 65535)
            {
                if(b->material[x2][y2] < stone_matgloss.size())
                {
                    return stone_matgloss[b->material[x2][y2]];
                }
                else
                {
                    string fallback = "ERROR";
                    return fallback;
                }
            }
        }
    }
    string fallback = "UNKNOWN";
    return fallback;
}

uint16_t DfMap::getNumStoneMatGloss()
{
    return stone_matgloss.size();
}

string DfMap::getStoneMatGlossString(uint16_t Index)
{
    if(Index < stone_matgloss.size())
    {
        return stone_matgloss[Index];
    }
}

// matgloss part of the designation
unsigned int DfMap::getGeolayerIndex (int x, int y, int z)
{
    assert(CheckBounds);

    int x2, y2;
    convertToDfMapCoords(x, y, x, y, x2, y2);
    Block *b = getBlock(x,y,z);
    if(b != NULL)
    {
        return b->designation[x2][y2].bits.geolayer_index;
    }
    return -1;
}
// matgloss part of the designation
unsigned int DfMap::getBiome (int x, int y, int z)
{
    assert(CheckBounds);

    int x2, y2;
    convertToDfMapCoords(x, y, x, y, x2, y2);
    Block *b = getBlock(x,y,z);
    if(b != NULL)
    {
        return b->designation[x2][y2].bits.biome;
    }
    return -1;
}

// what kind of building is here?
uint16_t DfMap::getBuilding (int x, int y, int z)
{
    assert(CheckBounds);
    int x2, y2;
    convertToDfMapCoords(x, y, x, y, x2, y2);
    Block *b = getBlock(x,y,z);
    if(b != NULL)
    {
        return b->occupancy[x2][y2].bits.building;
    }
    return -1;
}

bool DfMap::isHidden (int x, int y, int z)
{
    assert(CheckBounds);
    int x2, y2;
    convertToDfMapCoords(x, y, x, y, x2, y2);
    Block *b = getBlock(x,y,z);
    if(b != NULL)
    {
        return (b->designation[x2][y2].bits.hidden);
    }
    return false;
}

bool DfMap::isSubterranean (int x, int y, int z)
{
    assert(CheckBounds);
    int x2, y2;
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
bool DfMap::isTileSky(int x, int y, int z, int blockX, int blockY)
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
bool DfMap::isSkyView (int x, int y, int z)
{
    assert(CheckBounds);
    int x2, y2;
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
bool DfMap::isSunLit (int x, int y, int z)
{
    assert(CheckBounds);
    int x2, y2;
    convertToDfMapCoords(x, y, x, y, x2, y2);
    Block *b = getBlock(x,y,z);
    if(b != NULL)
    {
        return (b->designation[x2][y2].bits.light);
    }
    return false;
}

bool DfMap::isMagma (int x, int y, int z)
{
    assert(CheckBounds);
    int x2, y2;
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
void DfMap::convertToDfMapCoords(int x, int y, int &out_x, int &out_y, int &out_x2, int &out_y2)
{
    out_x2 = x % BLOCK_SIZE;
    out_y2 = y % BLOCK_SIZE;
    out_y = int(y / BLOCK_SIZE);
    out_x = int(x / BLOCK_SIZE);
}
