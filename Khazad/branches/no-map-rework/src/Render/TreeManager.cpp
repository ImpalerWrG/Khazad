/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) <year>  <name of author>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <stdafx.h>
#include "Renderer.h"
#include "TreeManager.h"
#include "ModelManager.h"
#include "Model.h"
#include <DataManager.h>
#include "../../dfhack/library/DFTypes.h"

// needs wood for trees
//FIXME: khazad needs both a GAME and RENDER material library.
TreeManager::TreeManager(vector<t_matgloss> & wood)
{
    // build matgloss ID->tree map
    for(uint32_t i = 0; i < DATA->getNumTree(); ++i)
    {
        TreeData * td =  DATA->getTreeData(i);
        string mat1 = td->getMatgloss();
        for(uint32_t j = 0; j < wood.size();j++)
        {
            string mat2 = wood[j].id;
            if( mat1 == mat2)
            {
                treemap[j]=td;
            }
        }
    }
}
TreeData *TreeManager::getTreeDesc( int32_t matgloss_idx )
{
    if (treemap.count(matgloss_idx))
        return treemap[matgloss_idx];
    return NULL;
}
TreeManager::~TreeManager()
{
}
