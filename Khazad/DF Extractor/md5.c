#include <string>
using namespace std;
#include "md5.h"
#include <mhash.h>
#include <stdio.h>
#include <stdlib.h>


//
string MD5Sum (const char * filename)
{
        uint32_t i;
        MHASH td;
        unsigned char buffer;
        unsigned char *hash;
        unsigned char final[33];

        td = mhash_init(MHASH_MD5);

        if (td == MHASH_FAILED) return string("");
        FILE *hashme = fopen(filename, "rb");
        while (fread(&buffer, 1, 1, hashme) == 1) {
                mhash(td, &buffer, 1);
        }

        hash = (unsigned char *)mhash_end(td);

        for (i = 0; i < mhash_get_block_size(MHASH_MD5); i++)
        {
                sprintf((char *)(final + i*2), "%.2x", hash[i]);
        }
        final[32] = 0;
        fclose(hashme);
        return string((const char *)final);
}
