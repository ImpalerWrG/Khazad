#include <DfMap.h>

// zlib helper functions for de/compressing files
#include <ZlibHelper.h>

// asserts are fun
#include <assert.h>
#define CheckBounds x < x_cell_count && x >= 0 && y < y_cell_count && y >= 0 && z < z_block_count && z >= 0
#define CheckBlockBounds x < x_block_count && x >= 0 && y < y_block_count && y >= 0 && z < z_block_count && z >= 0

// this expands into lots of ugly switch statement functions
#include "TileTypes.h"

DfMap::~DfMap()
{
    if(isValid())
    {
        clear();
    }
}

void DfMap::clear()
{
    valid = false;
    for(int i = 0; i < x_block_count*y_block_count*z_block_count;i++)
    {
        Block * b = block[i];
        if(b)
        {
            delete b;
        }
    }
    delete block;
}

void DfMap::allocBlockArray(int x,int y, int z)
{
    if(isValid())
    {
        clear();
    }
    x_block_count = x;
    y_block_count = y;
    z_block_count = z;
    updateCellCount();
    block = new Block*[x_block_count*y_block_count*z_block_count];
    for (int i = 0; i < x_block_count*y_block_count*z_block_count; i++ )
    {
        block[i] = NULL;
    }

    valid = true;
}

DfMap::DfMap(Uint32 x, Uint32 y, Uint32 z)
{
    valid = false;
    allocBlockArray(x,y,z);
}

// fix external indexes into df_map. HACK
void DfMap::clamp(int &x,int &y, int &z)
{
    if(isValid())
    {
        if(x < 0)
        {
            fprintf(stderr,"clamp: X underflow prevented\n");
            x = 0;
        }
        if(x >= x_cell_count)
        {
            fprintf(stderr,"clamp: X overflow prevented\n");
            x = x_cell_count-1;
        }
        if(y < 0)
        {
            fprintf(stderr,"clamp: Y underflow prevented\n");
            y = 0;
        }
        if(y >= y_cell_count)
        {
            fprintf(stderr,"clamp: Y overflow prevented\n");
            y = y_cell_count-1;
        }
        if(z < 0)
        {
            fprintf(stderr,"clamp: Z underflow prevented\n");
            z = 0;
        }
        if(z >= z_cell_count)
        {
            fprintf(stderr,"clamp: Z overflow prevented\n");
            z = z_cell_count-1;
        }
    }
    else
    {
        x = y = z = 0;
    }
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

///TODO: this should use streams and filters
bool DfMap::write(string FilePath)
{
    FILE *SaveFile;
    SaveFile = fopen(FilePath.c_str(),"wb");

    if(SaveFile == NULL)
    {
        printf("Can\'t create file for write.\n");
        return false;
    }
    else
    {
        // save map size
        fwrite(&x_block_count, sizeof(x_block_count), 1, SaveFile);
        fwrite(&y_block_count, sizeof(y_block_count), 1, SaveFile);
        fwrite(&z_block_count, sizeof(z_block_count), 1, SaveFile);

        Uint32 x, y, z;

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
                        fwrite(&x, sizeof(int), 1, SaveFile);
                        fwrite(&y, sizeof(int), 1, SaveFile);
                        fwrite(&z, sizeof(int), 1, SaveFile);
                        // block data
                        fwrite(&b->tile_type, sizeof(Uint16), BLOCK_SIZE*BLOCK_SIZE, SaveFile);
                        fwrite(&b->designation, sizeof(Uint32), BLOCK_SIZE*BLOCK_SIZE, SaveFile);
                        fwrite(&b->occupancy, sizeof(Uint32), BLOCK_SIZE*BLOCK_SIZE, SaveFile);
                    }
                }
            }
        }

        x = y = z = -1;
        fwrite(&x, sizeof(Uint32), 1, SaveFile);
        fwrite(&y, sizeof(Uint32), 1, SaveFile);
        fwrite(&z, sizeof(Uint32), 1, SaveFile);

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

///TODO: this should use streams and filters
bool DfMap::load(string FilePath)
{
    string DecompressedFilePath = FilePath + ".decomp";
    FILE *ToDecompress;
    FILE *Decompressed;
    unsigned blocks_read = 0U;
    int x, y, z;

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
        printf("Can't create decompressed file for read.\n");
        return false;
    }

    fread(&x_block_count, sizeof(x_block_count), 1, Decompressed);
    fread(&y_block_count, sizeof(y_block_count), 1, Decompressed);
    fread(&z_block_count, sizeof(z_block_count), 1, Decompressed);

    printf("Read from file %s\nX block size: %d\nY block size: %d\nZ levels: %d\n", FilePath.c_str(), x_block_count, y_block_count, z_block_count);

    updateCellCount();

    // catch cases where we already have a map loaded
    if(isValid())
    {
        clear();
    }
    allocBlockArray(x_block_count,y_block_count,z_block_count);

    do{
        fread(&x, sizeof(Uint32), 1, Decompressed);
        fread(&y, sizeof(Uint32), 1, Decompressed);
        fread(&z, sizeof(Uint32), 1, Decompressed);

        if(x == -1 || y == -1 || z == -1)
        {
            break;
        }

        Block * b = allocBlock(x,y,z);

        fread(&b->tile_type, sizeof(Uint16), BLOCK_SIZE*BLOCK_SIZE, Decompressed);
        fread(&b->designation, sizeof(Uint32), BLOCK_SIZE*BLOCK_SIZE, Decompressed);
        fread(&b->occupancy, sizeof(Uint32), BLOCK_SIZE*BLOCK_SIZE, Decompressed);

        ++blocks_read;

    }while(true);


    printf("Blocks read into memory: %d\n", blocks_read);
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
