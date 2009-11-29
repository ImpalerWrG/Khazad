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

#include <Renderer.h>
#include <ModelManager.h>
#include <DataManager.h>
#include <Model.h>


DECLARE_SINGLETON(ModelManager)

ModelManager::ModelManager()
{
    //ctor, whatever
}

ModelManager::~ModelManager()
{
    for( Uint32 i = 0; i < ModelVector.size(); i++)
    {
        delete ModelVector[i];
    }
}

bool ModelManager::Init()
{
    for(Uint32 i = 0; i < DATA->getNumModels(); ++i)
    {
        Model* NewModel = LoadModel(DATA->getModelData(i)->getPath());

        ModelVector.push_back(NewModel);
    }
}

Model* ModelManager::LoadModel(string filename)
{
    // skip creation if we already have a model

    char buffer[256];
    vector <vertex3f> verts;
    vector <normal3f> normals;
    vector <texcoord2f> texcoords;

    map <string, pair <RenderObject*, vector <vertex> *> > submodels;
    string currentgroup = "main";

    vector <vertex> *finished_verts = new vector<vertex>;

    int v1,v2,v3,v4,v5,n1,n2,n3,n4,n5,t1,t2,t3,t4,t5;
    FILE * modelfile = fopen (filename.c_str(), "r");
    if (! modelfile)
    {
        return NULL;
    }
    while (fgets(buffer, 255, modelfile) != NULL)
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

    Model *NewModel = new Model(submodels);
    submodels.clear();

    return NewModel;
}

/// TODO: separate this from khazad, use for mesh generation
vector < vertex > * ModelManager::getSlope(SlopeIndex surroundings)
{
    if(slopes.count(surroundings.value))
    {
        return slopes[surroundings.value];
    }
    vector <vertex> * ret = new vector <vertex>;
    /**
    * heightmap. order is from nort-west to north-west, clockwise. hm[9] is the center
    * 0=8--1---2
    *  |   |   |
    *  7---9---3
    *  |   |   |
    *  6---5---4
    */
    uint8_t hm[10] = {0,0,0,0,0,0,0,0,0,1};
    float hmf[10];// same in float

    uint8_t covered[4] = {0,0,0,0}; // view of a side blocked by geometry?

    // coordinates of the directions... fixed for spillover at the end of the loop
    static const float dc[9][2] =
    {
        {-0.5,-0.5},
        {0   ,-0.5},
        {0.5 ,-0.5},
        {0.5 ,0},
        {0.5 ,0.5},
        {0   ,0.5},
        {-0.5,0.5},
        {-0.5,0},
        {-0.5,-0.5}
    };
    // same for texture coords
    static const float tc[9][2] =
    {
        {0,1},
        {0.5   ,1},
        {1 ,1},
        {1 ,0.5},
        {1 ,0},
        {0.5   ,0},
        {0,0},
        {0,0.5},
        {0,1}
    };
    static const float norms[8][3] =
    {
        {0,-1,0},
        {0,0,0},
        {1,0,0},
        {0,0,0},
        {0,1,0},
        {0,0,0},
        {-1,0,0},
        {0,0,0}
    };
    uint8_t strong, weak, numsolids = 0;

    // copy surroundings
    bool solid = 0;

    solid = surroundings.directions.nw == 2;
    hm[0] = solid << 1;
    numsolids += solid;

    solid = surroundings.directions.n == 2;
    hm[1] = solid << 1;
    numsolids += solid;

    solid = surroundings.directions.ne == 2;
    hm[2] = solid << 1;
    numsolids += solid;

    solid = surroundings.directions.e == 2;
    hm[3] = solid << 1;
    numsolids += solid;

    solid = surroundings.directions.se == 2;
    hm[4] = solid << 1;
    numsolids += solid;

    solid = surroundings.directions.s == 2;
    hm[5] = solid << 1;
    numsolids += solid;

    solid = surroundings.directions.sw == 2;
    hm[6] = solid << 1;
    numsolids += solid;

    solid = surroundings.directions.w == 2;
    hm[7] = solid << 1;
    numsolids += solid;

    // test for covered and uncovered sides
    covered[0] = hm[1] || surroundings.directions.n == 1;
    covered[1] = hm[3] || surroundings.directions.e == 1;
    covered[2] = hm[5] || surroundings.directions.s == 1;
    covered[3] = hm[7] || surroundings.directions.w == 1;

    // determine center
    strong = (hm[7] && hm[1] && !hm[3] && !hm[4] && !hm[5])
    + (hm[1] && hm[3] && !hm[5] && !hm[6] && !hm[7])
    + (hm[3] && hm[5] && !hm[7] && !hm[0] && !hm[1])
    + (hm[5] && hm[7] && !hm[1] && !hm[2] && !hm[3]);
    if(numsolids == 1)
    {
        if (hm[0] || hm[2] || hm[4] || hm[6] )
        {
            hm[9] = 0;
        }
    }
    else if(strong == 1) hm[9] = 2;
    else hm[9] = 1;

    // fix corners
    hm[0] = hm[7] | hm[0] | hm[1];
    hm[2] = hm[1] | hm[2] | hm[3];
    hm[4] = hm[3] | hm[4] | hm[5];
    hm[6] = hm[5] | hm[6] | hm[7];

    // fix sides so that slopes aren't jaggy.
    hm[1] = (hm[1] >> 1) + (hm[0] || hm[2]);
    hm[3] = (hm[3] >> 1) + (hm[2] || hm[4]);
    hm[5] = (hm[5] >> 1) + (hm[4] || hm[6]);
    hm[7] = (hm[7] >> 1) + (hm[6] || hm[0]);

    hm[8] = hm[0]; // copy first point so we can safely use algorithms that go to N+1

    // convert int heights to floats
    for(int i = 0; i < 10; ++i)
    {
        hmf[i] = ((float)hm[i]) / 2 - 0.5;
    }

    // draw triangles
    for(Direction i = COMPASS_DIRECTIONS_START; i <= NUM_COMPASS_DIRECTIONS; ++i)
    {
        /**
        *  P--P+1--*
        *  | \ | / |
        *  *---C---*
        *  | / | \ |
        *  *---*---*
        */
        s3f center = {0, 0,hmf[9]};
        s3f pt1 = {dc[i+1][0],dc[i+1][1],hmf[i+1]};
        s3f pt2 = {dc[i][0], dc[i][1],hmf[i]};
        s3f norm = CalculateNormal(center, pt2, pt1 );
        s2f tx0 = {0.5,0.5};
        s2f tx1 = {tc[i+1][0],tc[i+1][1]};
        s2f tx2 = {tc[i][0],tc[i][1]};

        vertex v0 = vertex(center, tx0, norm);
        vertex v1 = vertex(pt1, tx1, norm);
        vertex v2 = vertex(pt2, tx2, norm);

        ret->push_back(v0);
        ret->push_back(v1);
        ret->push_back(v2);
    }
    //patch holes
    for(int i = 0; i < 8; i += 2)
    {
        // already covered by wall or nearby slope, don't draw side
        if (covered[i/2])
        {
            continue;
        }

        // zero center can be ignored
        if (hm[i+1] == 0)
        {
            continue;
        }

        // determine how many triangles are needed an in what configuration
        if(hm[i] == 0)// one tri, hm[i+2] is high
        {
            // second upper
            ret->push_back(vertex(dc[i+2][0], dc[i+2][1], hmf[i+2], 0.0,1.0, norms[i][0],norms[i][1],norms[i][2]));
            // second lower
            ret->push_back(vertex(dc[i+2][0],  dc[i+2][1], -0.5, 0.0,0.0, norms[i][0],norms[i][1],norms[i][2]));
            // first lower
            ret->push_back(vertex(dc[i][0], dc[i][1], hmf[i], 1.0,0.0,norms[i][0],norms[i][1],norms[i][2]));
        }
        else if(hm[i+2] == 0)// one tri, hm[i] is high
        {
            // first lower
            ret->push_back(vertex(dc[i][0],  dc[i][1], -0.5,1.0,0.0,   norms[i][0],norms[i][1],norms[i][2] ));
            // first upper
            ret->push_back(vertex(dc[i][0],  dc[i][1], hmf[i],   1.0,1.0,   norms[i][0],norms[i][1],norms[i][2] ));
            // second
            ret->push_back(vertex(dc[i+2][0], dc[i+2][1], hmf[i+2],   0.0,0.0,   norms[i][0],norms[i][1],norms[i][2] ));
        }
        else // two tris, both corners high
        {
            // second upper
            ret->push_back(vertex(dc[i+2][0],  dc[i+2][1], 0.5,    0.0,1.0,   norms[i][0],norms[i][1],norms[i][2] ));
            // second lower
            ret->push_back(vertex(dc[i+2][0],  dc[i+2][1], -0.5,    0.0,0.0,   norms[i][0],norms[i][1],norms[i][2] ));
            // first lower
            ret->push_back(vertex(dc[i][0], dc[i][1], -0.5,    1.0,0.0,   norms[i][0],norms[i][1],norms[i][2] ));

            // first lower
            ret->push_back(vertex(dc[i][0], dc[i][1], -0.5,    1.0,0.0,   norms[i][0],norms[i][1],norms[i][2] ));
            // first upper
            ret->push_back(vertex(dc[i][0],  dc[i][1], 0.5,    1.0,1.0,   norms[i][0],norms[i][1],norms[i][2] ));
            // center
            ret->push_back(vertex(dc[i+1][0], dc[i+1][1], 0.0,    0.5,0.5,   norms[i][0],norms[i][1],norms[i][2] ));
        }
    }
    slopes[surroundings.value] = ret;
    return ret;
}
