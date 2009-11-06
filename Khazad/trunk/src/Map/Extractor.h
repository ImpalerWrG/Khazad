#ifndef EXTRACTOR__HEADER
#define EXTRACTOR__HEADER

#include <Singleton.h>
#include <stdafx.h>

///FIXME: dfhack paths
#include "../../dfhack/library/DFTypes.h"
#include "../../dfhack/library/DFTileTypes.h"
#include "../../dfhack/library/DFHackAPI.h"
#include <string.h> // for memset


class Extractor
{
DECLARE_SINGLETON_CLASS(Extractor)

public:

	~Extractor();
	bool Init();

    bool Attach();
    void Detach();

    DFHackAPI* DFHack;

    bool isAttached()   { return Attached; }

protected:

    bool Initialized;
    bool Attached;
};

#define EXTRACTOR (Extractor::GetInstance())

#endif // EXTRACTOR__HEADER
