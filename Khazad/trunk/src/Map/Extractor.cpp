#include <stdafx.h>

#include <Extractor.h>
#include <Map.h>
#include <Singleton.h>
#include <TextureManager.h>
#include <DataManager.h>

///FIXME: dfhack paths
#include "../../dfhack/library/DFTypes.h"
#include "../../dfhack/library/DFTileTypes.h"
#include "../../dfhack/library/DFHackAPI.h"
#include <string.h> // for memset


DECLARE_SINGLETON(Extractor)

Extractor::Extractor()
{
    DFHack = NULL;
    Initialized = false;
}

Extractor::~Extractor()
{

}

bool Extractor::Init()
{
    Path path_to_xml("Assets/XML/Memory.xml");
    DFHack = new DFHack::API(path_to_xml);

    if (DFHack != NULL)
    {
        Initialized = true;
        return true;
    }
    return false;
}

bool Extractor::Attach()
{
    if(!DFHack->Attach())
    {
        Attached = false;
        return false;
    }
    if(!DFHack->InitMap())
    {
        Attached = false;
        return false;
    }

    Attached = true;
    return Attached;
}

void Extractor::Detach()
{
    DFHack->Detach();
    Attached = false;
}

