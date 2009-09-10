#include "DFCommon.h"
#include "DfVector.h"
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

// TODO: templating for vectors, simple copy constructor for stl vectors
// TODO: encapsulate access to multidimensional arrays.

SimpleAPI::SimpleAPI(const string path_to_xml)
{
    xml = path_to_xml;
    constructionsInited = false;
    buildingsInited = false;
    vegetationInited = false;
    pm = NULL;
}


/*-----------------------------------*
 *  Init the mapblock pointer array   *
 *-----------------------------------*/
bool SimpleAPI::InitMap()
{
    uint32_t    map_loc,   // location of the X array
                temp_loc,  // block location
                temp_locx, // iterator for the X array
                temp_locy, // iterator for the Y array
                temp_locz; // iterator for the Z array
    uint32_t map_offset = offset_descriptor->getAddress("map_data");;
    uint32_t x_count_offset = offset_descriptor->getAddress("x_count");
    uint32_t y_count_offset = offset_descriptor->getAddress("y_count");
    uint32_t z_count_offset = offset_descriptor->getAddress("z_count");
    
    // get the offsets once here
    tile_type_offset = offset_descriptor->getOffset("type");
    designation_offset = offset_descriptor->getOffset("designation");
    occupancy_offset = offset_descriptor->getOffset("occupancy");
    
    // get the map pointer
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

    //read the memory from the map blocks - x -> map slice
    for(uint32_t x = 0; x < x_block_count; x++)
    {
        temp_locx = map_loc + ( 4 * x );
        temp_locy = MreadDWord(temp_locx);
        
        // y -> map column
        for(uint32_t y = 0; y < y_block_count; y++)
        {
            temp_locz = MreadDWord(temp_locy);
            temp_locy += 4;
            
            // z -> map block (16x16)
            for(uint32_t z = 0; z < z_block_count; z++)
            {
                temp_loc = MreadDWord(temp_locz);
                temp_locz += 4;
                block[x*y_block_count*z_block_count + y*z_block_count + z] = temp_loc;
            }
        }
    }
    return true;
}

bool SimpleAPI::isValidBlock(uint32_t x, uint32_t y, uint32_t z)
{
    return block[x*y_block_count*z_block_count + y*z_block_count + z] != NULL;
}

// 256 * sizeof(uint16_t)
bool SimpleAPI::ReadTileTypes(uint32_t x, uint32_t y, uint32_t z, uint16_t *buffer)
{
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
    uint32_t addr = block[x*y_block_count*z_block_count + y*z_block_count + z];
    if (addr!=NULL)
    {
        Mread(addr+occupancy_offset, 256 * sizeof(uint32_t), (uint8_t *)buffer);
    }
    return true;
}


// 256 * sizeof(uint16_t)
bool SimpleAPI::WriteTileTypes(uint32_t x, uint32_t y, uint32_t z, uint16_t *buffer)
{
    uint32_t addr = block[x*y_block_count*z_block_count + y*z_block_count + z];
    if (addr!=NULL)
    {
        Mwrite(addr+tile_type_offset, 256 * sizeof(uint16_t), (uint8_t *)buffer);
    }
    return true;
}


// 256 * sizeof(uint32_t)
bool SimpleAPI::WriteDesignations(uint32_t x, uint32_t y, uint32_t z, uint32_t *buffer)
{
    uint32_t addr = block[x*y_block_count*z_block_count + y*z_block_count + z];
    if (addr!=NULL)
    {
        Mwrite(addr+designation_offset, 256 * sizeof(uint32_t), (uint8_t *)buffer);
    }
    return true;
}


// 256 * sizeof(uint32_t)
bool SimpleAPI::WriteOccupancy(uint32_t x, uint32_t y, uint32_t z, uint32_t *buffer)
{
    uint32_t addr = block[x*y_block_count*z_block_count + y*z_block_count + z];
    if (addr!=NULL)
    {
        Mwrite(addr+occupancy_offset, 256 * sizeof(uint32_t), (uint8_t *)buffer);
    }
    return true;
}


//16 of them? IDK... there's probably just 7. Reading more doesn't cause errors as it's an array nested inside a block
// 16 * sizeof(uint8_t)
bool SimpleAPI::ReadRegionOffsets(uint32_t x, uint32_t y, uint32_t z, uint8_t *buffer)
{
    uint32_t biome_stuffs = offset_descriptor->getOffset("biome_stuffs");
    uint32_t addr = block[x*y_block_count*z_block_count + y*z_block_count + z];
    if (addr!=NULL)
    {
        Mread(addr+biome_stuffs, 16 * sizeof(uint8_t), buffer);
    }
    return true;
}


// veins of a block, expects empty vein vector
bool SimpleAPI::ReadVeins(uint32_t x, uint32_t y, uint32_t z, vector <t_vein> & veins)
{
    uint32_t addr = block[x*y_block_count*z_block_count + y*z_block_count + z];
    int veinvector = offset_descriptor->getOffset("v_vein");
    int veinsize = offset_descriptor->getHexValue("v_vein_size");
    veins.clear();
    if(addr!=NULL && veinvector && veinsize)
    {
        assert(sizeof(t_vein) == veinsize);
        // veins are stored as a vector of pointers to veins
        /*pointer is 4 bytes! we work with a 32bit program here, no matter what architecture we compile khazad for*/
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


// getter for map size
void SimpleAPI::getSize(uint32_t& x, uint32_t& y, uint32_t& z)
{
    x = x_block_count;
    y = y_block_count;
    z = z_block_count;
}

bool SimpleAPI::ReadStoneMatgloss(vector<t_matgloss> & stones)
{
    int matgloss_address = offset_descriptor->getAddress("matgloss");
    int matgloss_offset = offset_descriptor->getHexValue("matgloss_skip");
    int matgloss_colors = offset_descriptor->getOffset("matgloss_stone_color");
    DfVector p_matgloss = dm->readVector(matgloss_address + matgloss_offset, 4);
    
    stones.clear();
    
    for (uint32_t i = 0; i< p_matgloss.getSize();i++)
    {
        uint32_t temp;
        // read the matgloss pointer from the vector into temp
        p_matgloss.read((uint32_t)i,(uint8_t *)&temp);
        // read the string pointed at by
        t_matgloss mat;
        mat.id = dm->readSTLString(temp); // reads a C string given an address
        mat.fore = MreadWord(temp + matgloss_colors);
        mat.back = MreadWord(temp + matgloss_colors + 2);
        mat.bright = MreadWord(temp + matgloss_colors + 4);
        stones.push_back(mat);
    }
    return true;
}


bool SimpleAPI::ReadMetalMatgloss(vector<t_matgloss> & metals)
{
    int matgloss_address = offset_descriptor->getAddress("matgloss");
    int matgloss_offset = offset_descriptor->getHexValue("matgloss_skip");
    int matgloss_colors = offset_descriptor->getOffset("matgloss_metal_color");
    DfVector p_matgloss = dm->readVector(matgloss_address + matgloss_offset*2, 4);
    
    metals.clear();
    
    for (uint32_t i = 0; i< p_matgloss.getSize();i++)
    {
        uint32_t temp;
        
        // read the matgloss pointer from the vector into temp
        p_matgloss.read((uint32_t)i,(uint8_t *)&temp);
        
        // read the string pointed at by
        t_matgloss mat;
        mat.id = dm->readSTLString(temp); // reads a C string given an address
        mat.fore = MreadWord(temp + matgloss_colors);
        mat.back = MreadWord(temp + matgloss_colors + 2);
        mat.bright = MreadWord(temp + matgloss_colors + 4);
        metals.push_back(mat);
    }
    return true;
}


bool SimpleAPI::ReadWoodMatgloss(vector<t_matgloss> & woods)
{
    int matgloss_address = offset_descriptor->getAddress("matgloss");
    // TODO: find flag for autumnal coloring?
    DfVector p_matgloss = dm->readVector(matgloss_address, 4);
    
    woods.clear();
    
    t_matgloss mat;
    // TODO: use brown?
    mat.fore = 7;
    mat.back = 0;
    mat.bright = 0;
    for (uint32_t i = 0; i< p_matgloss.getSize();i++)
    {
        uint32_t temp;
        
        // read the matgloss pointer from the vector into temp
        p_matgloss.read((uint32_t)i,(uint8_t *)&temp);
        
        // read the string pointed at by
        mat.id = dm->readSTLString(temp); // reads a C string given an address
        woods.push_back(mat);
    }
    return true;
}


bool SimpleAPI::ReadPlantMatgloss(vector<t_matgloss> & plants)
{
    int matgloss_address = offset_descriptor->getAddress("matgloss");
    int matgloss_offset = offset_descriptor->getHexValue("matgloss_skip");
    DfVector p_matgloss = dm->readVector(matgloss_address + matgloss_offset*3, 4);
    
    plants.clear();

    // TODO: use green?
    t_matgloss mat;
    mat.fore = 7;
    mat.back = 0;
    mat.bright = 0;
    for (uint32_t i = 0; i< p_matgloss.getSize();i++)
    {
        uint32_t temp;
        
        // read the matgloss pointer from the vector into temp
        p_matgloss.read((uint32_t)i,(uint8_t *)&temp);
        
        // read the string pointed at by
        mat.id = dm->readSTLString(temp); // reads a C string given an address
        plants.push_back(mat);
    }
    return true;
}

//vector<uint16_t> v_geology[eBiomeCount];
bool SimpleAPI::ReadGeology( vector < vector <uint16_t> >& assign )
{
    // get needed addresses and offsets
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
    
    uint32_t regionX, regionY, regionZ;
    uint16_t worldSizeX, worldSizeY;

    // check if we have 'em all
    if(
        !(
        region_x_offset && region_y_offset && region_z_offset && world_size_x && world_size_y
        && world_offset && world_regions_offset && world_geoblocks_offset && region_size
        && region_geo_index_offset && geolayer_geoblock_offset
        )
    )
    {
        // fail if we don't have them
        return false;
    }

    // read position of the region inside DF world
    MreadDWord(region_x_offset, regionX);
    MreadDWord(region_y_offset, regionY);
    MreadDWord(region_z_offset, regionZ);

    // get world size
    MreadWord(world_offset + world_size_x, worldSizeX);
    MreadWord(world_offset + world_size_y, worldSizeY);

    // get pointer to first part of 2d array of regions
    uint32_t regions = MreadDWord(world_offset + world_regions_offset);

    // read the geoblock vector
    DfVector geoblocks = dm->readVector(world_offset + world_geoblocks_offset,4);

    // iterate over 8 surrounding regions + local region
    for(int i = eNorthWest; i< eBiomeCount; i++)
    {
        // check bounds, fix them if needed
        int bioRX = regionX / 16 + (i%3) - 1;
        if( bioRX < 0) bioRX = 0;
        if( bioRX >= worldSizeX) bioRX = worldSizeX - 1;
        int bioRY = regionY / 16 + (i/3) - 1;
        if( bioRY < 0) bioRY = 0;
        if( bioRY >= worldSizeY) bioRY = worldSizeY - 1;
        
        // get pointer to column of regions
        uint32_t geoX;
        MreadDWord(regions + bioRX*4, geoX);
        
        // get index into geoblock vector
        uint16_t geoindex;
        MreadWord(geoX + bioRY*region_size + region_geo_index_offset, geoindex);
        
        // get the geoblock from the geoblock vector using the geoindex
        uint32_t geoblock_off;
        geoblocks.read(geoindex,(uint8_t *) &geoblock_off);
        
        // get the vector with pointer to layers
        DfVector geolayers = dm->readVector(geoblock_off + geolayer_geoblock_offset , 4); // let's hope
        // make sure we don't load crap
        assert(geolayers.getSize() > 0 && geolayers.getSize() <= 16);
        
        // finally, read the layer matgloss
        for(uint32_t j = 0;j< geolayers.getSize();j++)
        {
            int geol_offset;
            // read pointer to a layer
            geolayers.read(j, (uint8_t *) & geol_offset);
            // read word at pointer + 2, store in our geology vectors
            v_geology[i].push_back(MreadWord(geol_offset + 2));
        }
    }
    assign.clear();
    // TODO: clean this up
    for(int i = 0; i< eBiomeCount;i++)
    {
        assign.push_back(v_geology[i]);
    }
    return true;
}


// returns number of buildings, expects v_buildingtypes that will later map t_building.type to its name
uint32_t SimpleAPI::InitReadBuildings(vector <string> &v_buildingtypes)
{
    buildingsInited = true;
    int buildings = offset_descriptor->getAddress("buildings");
    assert(buildings);
    p_bld = new DfVector( dm->readVector(buildings,4));
    offset_descriptor->copyBuildings(v_buildingtypes);
    return p_bld->getSize();
}


// read one building
bool SimpleAPI::ReadBuilding(const uint32_t &index, t_building & building)
{
    assert(buildingsInited);
    uint32_t temp;
    t_building_df40d bld_40d;
    
    // read pointer from vector at position
    p_bld->read(index,(uint8_t *)&temp);
    
    //read building from memory
    Mread(temp, sizeof(t_building_df40d), (uint8_t *)&bld_40d);
    
    // transform
    int32_t type = -1;
    offset_descriptor->resolveClassId(temp, type);
    building.vtable = bld_40d.vtable;
    building.x1 = bld_40d.x1;
    building.x2 = bld_40d.x2;
    building.y1 = bld_40d.y1;
    building.y2 = bld_40d.y2;
    building.z = bld_40d.z;
    building.material = bld_40d.material;
    building.type = type;
}


void SimpleAPI::FinishReadBuildings()
{
    buildingsInited = false;
}


//TODO: maybe do construction reading differently - this could go slow with many of them.
// returns number of constructions, prepares a vector, returns total number of constructions
uint32_t SimpleAPI::InitReadConstructions()
{
    constructionsInited = true;
    int constructions = offset_descriptor->getAddress("buildings");
    assert(constructions);
    
    p_cons = new DfVector(dm->readVector(constructions,4));
    return p_cons->getSize();
}


bool SimpleAPI::ReadConstruction(const uint32_t &index, t_construction & construction)
{
    assert(constructionsInited);
    t_construction_df40d c_40d;
    uint32_t temp;
    
    // read pointer from vector at position
    p_cons->read((uint32_t)index,(uint8_t *)&temp);
    
    //read construction from memory
    Mread(temp, sizeof(t_construction_df40d), (uint8_t *)&c_40d);
    
    // transform
    construction.x = c_40d.x;
    construction.y = c_40d.y;
    construction.z = c_40d.z;
    construction.material = c_40d.material;
}


void SimpleAPI::FinishReadConstructions()
{
    constructionsInited = false;
}


uint32_t SimpleAPI::InitReadVegetation()
{
    vegetationInited = true;
    int vegetation = offset_descriptor->getAddress("vegetation");
    treeoffset = offset_descriptor->getOffset("tree_desc_offset");
    assert(vegetation && treeoffset);
    p_veg = new DfVector(dm->readVector(vegetation,4));
    return p_veg->getSize();
}


bool SimpleAPI::ReadVegetation(const uint32_t &index, t_tree_desc & shrubbery)
{
    assert(vegetationInited);
    uint32_t temp;
    // read pointer from vector at position
    p_veg->read(index,(uint8_t *)&temp);
    //read construction from memory
    Mread(temp + treeoffset, sizeof(t_tree_desc), (uint8_t *) &shrubbery);
    // fix matgloss nonsense
    if(shrubbery.material.type == 2) shrubbery.material.type = 3;
}


void SimpleAPI::FinishReadVegetation()
{
    vegetationInited = false;
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
    dm = p->getDataModel();
    // process is attached, everything went just fine... hopefully
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
    return true;
}
