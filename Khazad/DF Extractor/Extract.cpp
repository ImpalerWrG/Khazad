// Extractor
#include <Extract.h>
#include <DfVector.h>

DECLARE_SINGLETON(Extractor)

Extractor::Extractor()
{
    df_map = NULL; // important, null pointer means we don't have a map loaded
}

bool Extractor::Init()
{
    df_map = NULL;
    return true;
}

Extractor::~Extractor()
{
    if(df_map !=NULL )
    {
        delete df_map;
    }
}

bool Extractor::dumpMemory()
{
    // attach to process
    printf("Attempting to Attach Process\n");
    Process * p = PROCESS;
    if(!p->attach())
    {
        printf("Could not Attach Process, Aborting\n");
        return false; // couldn't attach to process, no go
    }
    printf("Process succesfully Attached\n");
    memory_info* offset_descriptor = p->getDescriptor();

    Uint32 map_loc, // location of the X array
           temp_loc, // block location
           temp_locx, // iterator for the X array
           temp_locy, // iterator for the Y array
           temp_locz; // iterator for the Z array
    unsigned blocks_read = 0U;

    // Read Map Data Blocks
    int map_offset = offset_descriptor->getOffset("map_data");;
    int x_count_offset = offset_descriptor->getOffset("x_count");
    int y_count_offset = offset_descriptor->getOffset("y_count");
    int z_count_offset = offset_descriptor->getOffset("z_count");
    int tile_type_offset = offset_descriptor->getOffset("type");
    int designation_offset = offset_descriptor->getOffset("designation");
    int occupancy_offset = offset_descriptor->getOffset("occupancy");
    int veinvector = offset_descriptor->getOffset("v_vein");
    int veinsize = offset_descriptor->getOffset("v_vein_size");

    map_loc = p->readDWord(map_offset);

    if (!map_loc)
    {
        printf("Could not find DF map information in memory, Aborting\n");
        return false;
    }
    printf("Map data Found at: 0x%.8X\n", map_loc);

    if(df_map != NULL)
    {
        delete df_map;
    }
    df_map = new DfMap(p->readDWord(x_count_offset),p->readDWord(y_count_offset),p->readByte(z_count_offset));
    //read the memory from the map blocks
    for(int x = 0; x < df_map->x_block_count; x++)
    {
        temp_locx = map_loc + ( 4 * x );
        temp_locy = p->readDWord(temp_locx);
        for(int y = 0; y < df_map->y_block_count; y++)
        {
            temp_locz = p->readDWord(temp_locy);
            temp_locy += 4;
            for(int z = 0; z < df_map->z_block_count; z++)
            {
                temp_loc = p->readDWord(temp_locz);
                temp_locz += 4;
                if (temp_loc)
                {
                    Block * b = df_map->allocBlock(x,y,z);
                    b->origin = temp_loc; // save place of block in DF's memory for later
                    //
                    p->read(
                    /*Uint32 offset*/ temp_loc + tile_type_offset,
                    /*Uint32 size*/   sizeof(short int)*BLOCK_SIZE*BLOCK_SIZE,
                    /*void *target*/  (Uint8 *)&b->tile_type
                           );
                    p->read(
                    /*Uint32 offset*/ temp_loc + designation_offset,
                    /*Uint32 size*/   sizeof(int)*BLOCK_SIZE*BLOCK_SIZE,
                    /*void *target*/  (Uint8 *)&b->designation
                           );
                    p->read(
                    /*Uint32 offset*/ temp_loc + occupancy_offset,
                    /*Uint32 size*/   sizeof(int)*BLOCK_SIZE*BLOCK_SIZE,
                    /*void *target*/  (Uint8 *)&b->occupancy
                           );
                    // load veins from the game
                    if(veinvector && veinsize)
                    {
                        assert(sizeof(t_vein) == veinsize);
                        // veins are stored as a vector of pointers to veins .. at least in df 40d11 on linux
                        /*pointer is 4 bytes! we work with a 32bit program here, no matter what architecture we compile khazad for*/
                        DfVector p_veins = p->readVector(temp_loc + veinvector, 4);
                        // read all veins
                        for (int i = 0; i< p_veins.getSize();i++)
                        {
                            t_vein v;
                            Uint32 temp;
                            // read the vein pointer from the vector
                            p_veins.read((Uint32)i,(Uint8 *)&temp);
                            // read the vein data (dereference pointer)
                            p->read(temp, veinsize, (Uint8 *)&v);
                            // store it in the block
                            b->veins.push_back(v);
                        }
                        b->collapseVeins(); // collapse *our* vein vector into vein matgloss data
                    }
                    ++blocks_read;
                }
            }
        }
    }
    printf("Blocks read into memory: %d\n", blocks_read);
    p->detach();
    return true;
}
// wrappers!
bool Extractor::loadMap(string FileName)
{
    if(df_map == NULL)
    {
        df_map = new DfMap(FileName);
    }
    else
    {
        df_map->load(FileName);
    }
    return df_map->isValid();
}

bool Extractor::writeMap(string FileName)
{
    if(df_map == NULL)
    {
        return false;
    }
    return df_map->write(FileName);
}

bool Extractor::isMapLoaded()
{
    if(df_map != NULL)
    {
        if(df_map->isValid())
        {
            return true;
        }
    }
    return false;
}
