#ifndef SIMPLEAPI_H_INCLUDED
#define SIMPLEAPI_H_INCLUDED
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
struct Matgloss
{
       char id[32];
       uint8_t fore; // Annoyingly the offset for this differes between types
       uint8_t back;
       uint8_t bright;
};

vector <Matgloss> StoneMatgloss;
vector <Matgloss> WoodMatgloss;
vector <Matgloss> PlantMatgloss;
vector <Matgloss> MetalMatgloss;

#endif // SIMPLEAPI_H_INCLUDED
