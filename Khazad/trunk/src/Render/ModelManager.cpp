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
#include "ModelManager.h"
#include "Model.h"


ModelManager::ModelManager()
{
    //ctor, whatever
}
ModelManager::~ModelManager()
{
    clear();
}

void ModelManager::clear()
{
    for( map<string, Model * >::iterator it = models.begin(); it != models.end(); ++it)
    {
        delete it->second;
    }
    models.clear();
}

Model * ModelManager::LoadOBJModel(string filename)
{
    // skip creation if we already have a model
    if(models.count(filename))
        return models[filename];

    char buffer[256];
    vector < vertex3f > verts;
    vector < normal3f > normals;
    vector < texcoord2f > texcoords;
    map <string, pair <RenderObject *, vector <vertex> *> > submodels;
    string currentgroup = "main";

    vector <vertex> *finished_verts = new vector<vertex>;

    int v1,v2,v3,v4,v5,n1,n2,n3,n4,n5,t1,t2,t3,t4,t5;
    FILE * modelfile = fopen (filename.c_str(), "r");
    if (! modelfile)
        return NULL;
    while (fgets(buffer,255,modelfile) != NULL)
    {
        char test = buffer[0];
        char test2 = buffer[1];
        if( test == 'v' )
        {
            // three normals
            if(test2 == 'n')
            {
                normal3f tmp;
                sscanf(buffer, "vn %f %f %f", &tmp.x, &tmp.y, &tmp.z);
                normals.push_back(tmp);
            }
            // two tex coords
            else if( test2 == 't')
            {
                texcoord2f tmp;
                sscanf(buffer, "vt %f %f", &tmp.u, &tmp.v);
                texcoords.push_back(tmp);
            }
            // plain old vertex
            else
            {
                vertex3f tmp;
                sscanf(buffer, "v %f %f %f", &tmp.x, &tmp.z, &tmp.y);
                verts.push_back(tmp);
            }
        }
        // face, hack, hack, h.. hard coded only two formats of many
        else if( test == 'f' )
        {
            if(sscanf(buffer , "f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d",
                &v1, &t1, &n1, &v2, &t2, &n2, &v3, &t3, &n3, &v4, &t4, &n4, &v5, &t5, &n5) == 15)
            {
                // a pentagon, subdivide
                // tri 1
                finished_verts->push_back(vertex(verts[v1-1],texcoords[t1-1],normals[n1-1]));
                finished_verts->push_back(vertex(verts[v2-1],texcoords[t2-1],normals[n2-1]));
                finished_verts->push_back(vertex(verts[v3-1],texcoords[t3-1],normals[n3-1]));

                // tri 2
                finished_verts->push_back(vertex(verts[v1-1],texcoords[t1-1],normals[n1-1]));
                finished_verts->push_back(vertex(verts[v3-1],texcoords[t3-1],normals[n3-1]));
                finished_verts->push_back(vertex(verts[v4-1],texcoords[t4-1],normals[n4-1]));

                // tri 3
                finished_verts->push_back(vertex(verts[v1-1],texcoords[t1-1],normals[n1-1]));
                finished_verts->push_back(vertex(verts[v4-1],texcoords[t4-1],normals[n4-1]));
                finished_verts->push_back(vertex(verts[v5-1],texcoords[t5-1],normals[n5-1]));
            }

            if(sscanf(buffer , "f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d",
               &v1, &t1, &n1, &v2, &t2, &n2, &v3, &t3, &n3, &v4, &t4, &n4) == 12)
            {
                // a quad, subdivide
                // tri 1
                finished_verts->push_back(vertex(verts[v1-1],texcoords[t1-1],normals[n1-1]));
                finished_verts->push_back(vertex(verts[v2-1],texcoords[t2-1],normals[n2-1]));
                finished_verts->push_back(vertex(verts[v3-1],texcoords[t3-1],normals[n3-1]));

                // tri 2
                finished_verts->push_back(vertex(verts[v1-1],texcoords[t1-1],normals[n1-1]));
                finished_verts->push_back(vertex(verts[v3-1],texcoords[t3-1],normals[n3-1]));
                finished_verts->push_back(vertex(verts[v4-1],texcoords[t4-1],normals[n4-1]));
            }
            else if(sscanf(buffer , "f %d/%d/%d %d/%d/%d %d/%d/%d",
                &v1, &t1, &n1, &v2, &t2, &n2, &v3, &t3, &n3) == 9)
            {
                // a triangle
                finished_verts->push_back(vertex(verts[v1-1],texcoords[t1-1],normals[n1-1]));
                finished_verts->push_back(vertex(verts[v2-1],texcoords[t2-1],normals[n2-1]));
                finished_verts->push_back(vertex(verts[v3-1],texcoords[t3-1],normals[n3-1]));
            }
            else
            {
                //unsupported format, fail here
                cerr << "Unsupported model format : " << filename << endl;
                fclose(modelfile);
                return NULL;
            }
        }
        // next group.
        else if( test == 'g' )
        {
            char buf2[64];
            // process current group into named render object
            if(finished_verts->size())
            {
                pair <RenderObject *, vector <vertex> *> RO_VERT;
                RO_VERT.first = NULL;
                RO_VERT.second = finished_verts;
                submodels[currentgroup] = RO_VERT;
                finished_verts = new vector <vertex>;
            }
            // start with new group
            sscanf(buffer,"g %s", buf2);
            currentgroup = buf2;
        }
    }
    fclose(modelfile);
    // got em all

    // finalize, process current group into named render object
    if(finished_verts->size())
    {
        pair <RenderObject *, vector <vertex> *> RO_VERT;
        RO_VERT.first = NULL;
        RO_VERT.second = finished_verts;
        submodels[currentgroup] = RO_VERT;
        finished_verts = new vector <vertex>;
    }
    else
    {
        delete finished_verts;
    }
    Model *ret = new Model(submodels);
    models[filename] = ret;
    submodels.clear();
    return ret;
}
