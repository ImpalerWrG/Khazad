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

#ifndef MODELMANAGER_HEADER
#define MODELMANAGER_HEADER


#include <stdafx.h>

#include <Singleton.h>


class Model;

union SlopeIndex
{
    struct
    {
        unsigned int nw : 2;
        unsigned int n : 2;
        unsigned int ne : 2;
        unsigned int e : 2;
        unsigned int se : 2;
        unsigned int s : 2;
        unsigned int sw : 2;
        unsigned int w : 2;
    } directions;
    uint16_t value;
};

class ModelManager
{
    DECLARE_SINGLETON_CLASS(ModelManager)

public:

    ~ModelManager();

    bool Init();

    Model* LoadModel(string filename);

    vector <vertex>* getSlope(SlopeIndex surroundings);

    Model* getModel(Sint16 ModelID)             { return ModelVector[ModelID]; }

private:

    vector <Model*> ModelVector;

    map< Uint16, vector <vertex>* > slopes;
};

#define MODEL (ModelManager::GetInstance())

#endif // MODELMANAGER_HEADER
