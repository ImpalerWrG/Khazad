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
bool SimpleAPI::InitMap()
{
    block = new uint32_t[x_block_count*y_block_count*z_block_count];
    for (uint32_t i = 0; i < x_block_count*y_block_count*z_block_count; i++ )
    {
        block[i] = NULL;
    }
}

void SimpleAPI::getSize(uint32_t& x, uint32_t& y, uint32_t& z)
{
    x = x_block_count;
    y = y_block_count;
    z = z_block_count;
}

/// TODO: turn next line into an XML entry
RawType matglossRawMapping[] = {Mat_Wood, Mat_Stone, Mat_Metal, Mat_Plant};

/// Matgloss
bool SimpleAPI::ReadMatgloss(uint32_t &count, vector< vector <t_matgloss> > &Matgloss)
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
                // store it in the block

                /// FIXME: we have to add vectors to the vector
                Matgloss[matglossRawMapping[counter]].push_back(mat);
            }
        }
        return true;
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
