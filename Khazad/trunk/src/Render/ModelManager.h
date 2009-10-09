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

#ifndef MODELMANAGER_H
#define MODELMANAGER_H

class Model;

union SlopeIndex
{
    struct
    {
        uint nw : 2;
        uint n : 2;
        uint ne : 2;
        uint e : 2;
        uint se : 2;
        uint s : 2;
        uint sw : 2;
        uint w : 2;
    } directions;
    uint16_t value;
};

class ModelManager
{
    public:
        ModelManager();
        ~ModelManager();
        Model * LoadOBJModel(string filename);
        vector < vertex > * getSlope(SlopeIndex surroundings);
        void clear();

    private:
        map < string, Model * > models;
        map < uint16_t, vector < vertex > * > slopes;
};

#endif // MODELMANAGER_H
