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

#ifndef MODEL_H
#define MODEL_H

struct RenderObject;
class Model
{
    friend class ModelManager;
    public:
        inline void Render()
        {
            RENDERER->CallRenderObject(submodels[0]);
        };
        inline void Render( string submodel )
        {
            RENDERER->CallRenderObject(submodels[getSubmodelIndex(submodel)]);
        };
        inline void Render( uint32_t submodel )
        {
            RENDERER->CallRenderObject(submodels[submodel]);
        };

        inline int32_t getSubmodelIndex (string submodel)
        {
            if(submodel_names.count(submodel))
                return submodel_names[submodel];
            return -1;
        }
    private:
        Model(map <string, RenderObject *> & _submodels)
        {
            uint32_t i = 0;
            for(map <string, RenderObject *>::iterator it = _submodels.begin(); it != _submodels.end(); ++it)
            {
                submodel_names[it->first] = i;
                i++;
                submodels.push_back(it->second);
            }
        }
        map <string, uint32_t> submodel_names;
        vector <RenderObject *> submodels;
};

#endif // MODEL_H
