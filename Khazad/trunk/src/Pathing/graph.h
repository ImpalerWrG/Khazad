/* Copyright Benoit Hudson 2009 */
#ifndef GRAPH_HEADER
#define GRAPH_HEADER

#include <list>


#include <grid.h>
#include <map.h>

struct Vertex
{
    list<Vertex> Edges;
};

class Graph
{
public:

    Graph();
    ~Graph();

    Vertex addVertex();
    void addEdge(Vertex* SourceVertex, Vertex* DestinationVertex);

private:

    //map<Vertex> Vertices;
};

class ZoneGraph : Graph   // A Graph composed of zones built ontop of a Grid and reflectin high level zones
{

};

class GridGraph : Graph  // A Gaph drawn directly from a grid and storing lowest level edges
{
public:

    GridGraph(const gridInterface* SourceGrid);
    ~GridGraph();


    float edgeCost(const MapCoordinates &a, const MapCoordinates &b) const      { /*return Grid->edgeCost(a, b);*/ }

    bool contains(const MapCoordinates &TestCoords) const        { return Grid->contains(TestCoords); }

private:

    const gridInterface* Grid;
};

#endif // GRAPH_HEADER
