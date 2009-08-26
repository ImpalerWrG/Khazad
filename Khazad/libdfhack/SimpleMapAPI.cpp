#include "DFCommon.h"
#include "SimpleMapAPI.h"
#include "MemInfo.h"
/*
Cheers!

API wise what I need is fairly simple since I've got all of the processing code written to fit
internal DF formats already. After reading I convert from the DF format to a format optimised
for geometry generation and pack the data into one large array per z-level, border padded.

Here's a rough guide, but I'll fit it to whatever you think will work best:

    MatGloss:
    struct DFMatgloss {
       char[32] id;
       byte fore; // Annoyingly the offset for this differes between types
       byte back;
       byte bright;
    };
    DFMaterial *ReadStoneMatgloss(insigned int &count);
    DFMaterial *ReadMetalMatgloss(unsigned int &count);
Memory to be allocated in your library I guess, though I don't like it very much.
I could also pass in a std::vector to be filled if you think that would be better.

Constructions/Buildings:
Could be the same as a above. Or it would be nice if it was possible to read one building
at a time rather than all at once to reduce overhead, something like:
    bool BeginReadConstructions(); // Reads the pointer array, sets internal state to point to first item
    bool ReadNextBuilding(DFBuilding &buffer); // Reads current item (return false if out of bounds). Increments current pointer to next item.
- But if you don't like this that's cool.

Tiles:
    bool ReadTileTypes(unsigned int blockx, unsigned int blocky, word *buffer);
    bool ReadDesignations(unsigned int blockx, unsigned int blocky, dword *buffer);
    bool ReadOccupancy(unsigned int blockx, unsigned int blocky, dword *buffer);
Return false/0 on failure, buffer allocated by me, 256 long of course.

Veins:
I would prefer to have the veins returned in DF format (un-collapsed) as this is more efficient. I can do collapsed if I have to though
    DFVein *ReadVeins(unsigned int blockx, unsigned int blocky, unsigned int &count);

Biome data:
Not sure about this, you do it a slight different (and superior) way to me.
I just need the quickest way possible to convert between the DESIGNATION_MATGLOSS_BITS and the stone matgloss index.
    simple indexed table
*/
SimpleAPI::SimpleAPI(const string path_to_xml)
{
    xml = path_to_xml;
}

/*-----------------------------------*
 *  Init the mapblock pointer array  *
 *-----------------------------------*/
bool SimpleAPI::InitMap()
{
    uint32_t    map_loc, // location of the X array
                temp_loc, // block location
                temp_locx, // iterator for the X array
                temp_locy, // iterator for the Y array
                temp_locz; // iterator for the Z array
    uint32_t map_offset = offset_descriptor->getAddress("map_data");;
    uint32_t x_count_offset = offset_descriptor->getAddress("x_count");
    uint32_t y_count_offset = offset_descriptor->getAddress("y_count");
    uint32_t z_count_offset = offset_descriptor->getAddress("z_count");

    map_loc = MreadDWord(map_offset);
    if (!map_loc)
    {
        // bad stuffz happend
        return false;
    }
    // get the size
    x_block_count = MreadDWord(x_count_offset);
    y_block_count = MreadDWord(y_count_offset);
    z_block_count = MreadDWord(z_count_offset);
    // alloc array for pinters to all blocks
    block = new uint32_t[x_block_count*y_block_count*z_block_count];

    //read the memory from the map blocks
    // x -> slice
    for(uint32_t x = 0; x < x_block_count; x++)
    {
        temp_locx = map_loc + ( 4 * x );
        temp_locy = MreadDWord(temp_locx);
        // y -> column
        for(uint32_t y = 0; y < y_block_count; y++)
        {
            temp_locz = MreadDWord(temp_locy);
            temp_locy += 4;
            // z -> block
            for(uint32_t z = 0; z < z_block_count; z++)
            {
                temp_loc = MreadDWord(temp_locz);
                temp_locz += 4;
                if (temp_loc)
                {
                    //temp_loc is the address of the block
                    block[x*y_block_count*z_block_count + y*z_block_count + z] = temp_loc;
                }
                else
                {
                    block[x*y_block_count*z_block_count + y*z_block_count + z] = NULL;
                }
            }
        }
    }
    return true;
}

// 256 * sizeof(uint16_t)
bool SimpleAPI::ReadTileTypes(uint32_t x, uint32_t y, uint32_t z, uint16_t *buffer)
{
    uint32_t tile_type_offset = offset_descriptor->getOffset("type");
    uint32_t addr = block[x*y_block_count*z_block_count + y*z_block_count + z];
    if (addr!=NULL)
    {
        Mread(addr+tile_type_offset, 256 * sizeof(uint16_t), (uint8_t *)buffer);
    }
    return true;
}

// 256 * sizeof(uint32_t)
bool SimpleAPI::ReadDesignations(uint32_t x, uint32_t y, uint32_t z, uint32_t *buffer)
{
    uint32_t designation_offset = offset_descriptor->getOffset("designation");
    uint32_t addr = block[x*y_block_count*z_block_count + y*z_block_count + z];
    if (addr!=NULL)
    {
        Mread(addr+designation_offset, 256 * sizeof(uint32_t), (uint8_t *)buffer);
    }
    return true;
}

// 256 * sizeof(uint32_t)
bool SimpleAPI::ReadOccupancy(uint32_t x, uint32_t y, uint32_t z, uint32_t *buffer)
{
    uint32_t occupancy_offset = offset_descriptor->getOffset("occupancy");
    uint32_t addr = block[x*y_block_count*z_block_count + y*z_block_count + z];
    if (addr!=NULL)
    {
        Mread(addr+occupancy_offset, 256 * sizeof(uint32_t), (uint8_t *)buffer);
    }
    return true;
}

//16 of them? IDK... there's probably just 7. Reading more doesn't cause errors as it's an array nested inside a block
// 16 * sizeof(uint8_t)
bool SimpleAPI::ReadRegionOffsets(uint32_t x, uint32_t y, uint32_t z, uint32_t *buffer)
{
    uint32_t biome_stuffs = offset_descriptor->getOffset("biome_stuffs");
    uint32_t addr = block[x*y_block_count*z_block_count + y*z_block_count + z];
    if (addr!=NULL)
    {
        Mread(addr+biome_stuffs, 16 * sizeof(uint8_t), (uint8_t *)buffer);
    }
    return true;
}

// veins of a block, expects empty vein vector
bool SimpleAPI::ReadVeins(uint32_t x, uint32_t y, uint32_t z, vector <t_vein> & veins)
{
    uint32_t addr = block[x*y_block_count*z_block_count + y*z_block_count + z];
    int veinvector = offset_descriptor->getOffset("v_vein");
    int veinsize = offset_descriptor->getHexValue("v_vein_size");
    if(addr!=NULL && veinvector && veinsize)
    {
        assert(sizeof(t_vein) == veinsize);
        // veins are stored as a vector of pointers to veins
        /*pointer is 4 bytes! we work with a 32bit program here, no matter what architecture we compile khazad for*/
        /// FIXME: templating for vectors, simple copy constructor for stl vectors
        DfVector p_veins = dm->readVector(addr + veinvector, 4);
        // read all veins
        for (uint32_t i = 0; i< p_veins.getSize();i++)
        {
            t_vein v;
            uint32_t temp;
            // read the vein pointer from the vector
            p_veins.read((uint32_t)i,(uint8_t *)&temp);
            // read the vein data (dereference pointer)
            Mread(temp, veinsize, (uint8_t *)&v);
            // store it in the vector
            veins.push_back(v);
        }
    }
    return false;
}

void SimpleAPI::getSize(uint32_t& x, uint32_t& y, uint32_t& z)
{
    x = x_block_count;
    y = y_block_count;
    z = z_block_count;
}

/// TODO: turn next line into an XML entry, add more matgloss here (creature types for soap/leather/meat, etc.)
RawType matglossRawMapping[] = {Mat_Wood, Mat_Stone, Mat_Metal, Mat_Plant};

/// Matgloss
bool SimpleAPI::ReadMatgloss(vector< vector <t_matgloss> > &Matgloss)
{
    // matgloss
    int matgloss_address = offset_descriptor->getAddress("matgloss");
    int matgloss_skip = offset_descriptor->getHexValue("matgloss_skip");
    // read matgloss data from df if we can
    if(matgloss_address && matgloss_skip)
    {
        uint32_t addr = matgloss_address;
        for(uint32_t counter = Mat_Wood; counter < NUM_MATGLOSS_TYPES; addr += matgloss_skip, counter++)
        {
            DfVector p_matgloss = dm->readVector(addr, 4);
            // iterate over it
            for (uint32_t i = 0; i< p_matgloss.getSize();i++)
            {
                uint32_t temp;
                string tmpstr;
                // read the matgloss pointer from the vector into temp
                p_matgloss.read((uint32_t)i,(uint8_t *)&temp);
                // read the string pointed at by
                t_matgloss mat;
                mat.id = dm->readSTLString(temp); // reads a C string given an address
                /// FIXME: needs more offsets, all the tile/color/brightness stuff
                /// FIXME: we have to add vectors to the vector depending on if they are there or not...
                /// FIXME: unfinished
                Matgloss[matglossRawMapping[counter]].push_back(mat);
            }
        }
        return true;
    }
    return false;
}

bool SimpleAPI::ReadGeology( vector <uint16_t>*& assign)
{
    int region_x_offset = offset_descriptor->getAddress("region_x");
    int region_y_offset = offset_descriptor->getAddress("region_y");
    int region_z_offset =  offset_descriptor->getAddress("region_z");
    int world_offset =  offset_descriptor->getAddress("world");
    int world_regions_offset =  offset_descriptor->getOffset("w_regions_arr");
    int region_size =  offset_descriptor->getHexValue("region_size");
    int region_geo_index_offset =  offset_descriptor->getOffset("region_geo_index_off");
    int world_geoblocks_offset =  offset_descriptor->getOffset("w_geoblocks");
    int world_size_x = offset_descriptor->getOffset("world_size_x");
    int world_size_y = offset_descriptor->getOffset("world_size_y");
    int geolayer_geoblock_offset = offset_descriptor->getOffset("geolayer_geoblock_offset");

    uint32_t regionX;
    uint32_t regionY;
    uint32_t regionZ;

    ///FIXME: these belong to some world structure
    uint32_t worldSizeX;
    uint32_t worldSizeY;

    if(region_x_offset && region_y_offset && region_z_offset)
    {
        regionX = MreadDWord(region_x_offset);
        regionY = MreadDWord(region_y_offset);
        regionZ = MreadDWord(region_z_offset);
        //df_map->setRegionCoords(,MreadDWord(region_y_offset),MreadDWord(region_z_offset));
        // we have region offset
        // extract layer geology data. we need all these to do that
        if(world_size_x && world_size_y && world_offset && world_regions_offset && world_geoblocks_offset && region_size && region_geo_index_offset && geolayer_geoblock_offset)
        {
            // get world size
            int worldSizeX = MreadWord(world_offset + world_size_x);
            int worldSizeY = MreadWord(world_offset + world_size_y);
            printf("World size. X=%d Y=%d\n",worldSizeX,worldSizeY);
            // get pointer to first part of 2d array of regions
            uint32_t regions = MreadDWord(world_offset + world_regions_offset);
            printf("regions. Offset=%d\n",regions);
            // read the 9 surrounding regions
            DfVector geoblocks = dm->readVector(world_offset + world_geoblocks_offset,4);
            // iterate over surrounding biomes. make sure we don't fall off the world
            for(int i = eNorthWest; i< eBiomeCount; i++)
            {
                // check bounds, fix them if needed
                int bioRX = regionX / 16 + (i%3) - 1;
                if( bioRX < 0) bioRX = 0;
                if( bioRX >= worldSizeX) bioRX = worldSizeX - 1;
                int bioRY = regionY / 16 + (i/3) - 1;
                if( bioRY < 0) bioRY = 0;
                if( bioRY >= worldSizeY) bioRY = worldSizeY - 1;
                /// TODO: encapsulate access to multidimensional arrays.
                // load region stuff here
                uint32_t geoX = MreadDWord(regions + bioRX*4);// get pointer to column of regions
                // geoX = base
                // bioRY = index
                // region_size = size of array objects
                // region_geo_index_off = offset into the array object
                uint16_t geoindex = MreadWord(geoX + bioRY*region_size + region_geo_index_offset);
                uint32_t geoblock_off;
                // get the geoblock from the geoblock vector using the geoindex
                geoblocks.read(geoindex,(uint8_t *) &geoblock_off);
                // get the layer pointer vector :D
                DfVector geolayers = dm->readVector(geoblock_off + geolayer_geoblock_offset , 4); // let's hope
                // make sure we don't load crap
                assert(geolayers.getSize() > 0 && geolayers.getSize() <= 16);
                for(uint32_t j = 0;j< geolayers.getSize();j++)
                {
                    int geol_offset;
                    // read pointer to a layer
                    geolayers.read(j, (uint8_t *) & geol_offset);
                    // read word at pointer + 2, store in our geology vectors
                    v_geology[i].push_back(MreadWord(geol_offset + 2));
                }
            }
//            have_geology = true;
            assign = v_geology;
            return true;
        }
    }
    return false;
}

bool SimpleAPI::Attach()
{
    // detach all processes, destroy manager
    if(pm != NULL)
        delete pm;
    // find a process (ProcessManager can find multiple when used properly)
    pm = new ProcessManager(xml); // FIXME: handle bad XML better
    if(!pm->findProcessess())
        return false; // FIXME: throw exceptions to distinguish errors? provide error code?
    p = (*pm)[0]; // we just use the first found process
    if(!p->attach())
        return false; // couldn't attach to process, no go
    offset_descriptor = p->getDescriptor();
    // process is attached, everything went just fine
    return true;
}

bool SimpleAPI::Detach()
{
    p->detach();
    delete pm;
    pm = NULL;
    p = NULL;
    offset_descriptor = NULL;
    dm = NULL;
//    DestroyCache();
}
