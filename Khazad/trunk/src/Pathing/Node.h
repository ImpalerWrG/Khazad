#ifndef Node_HEADER
#define Node_HEADER

#include <vector>
#include <memory.h>

class Heuristic;

#include <boost/shared_ptr.hpp>


struct Node
{
    virtual ~Node() {};
    virtual MapCoordinates& getCoordinates() = 0;

    float TotalCost;
    float TieBreakerValue;

    virtual bool costLessThan(const Node &TargetNode) const
    {
        if (TotalCost < TargetNode.TotalCost)
        {
            return true;
        }
        else if (TotalCost == TargetNode.TotalCost)
        {
            return TieBreakerValue < TargetNode.TieBreakerValue;
        }
        else
        {
            return false;
        }
    }
};

struct AStarNode : Node
{
    MapCoordinates LocationCoordinates;

    float PathLengthFromStart;
    float MinimumCostToGoal;

    AStarNode* Parent;

    AStarNode()  {}

    void Set(MapCoordinates TargetCoordinates, AStarNode* ParentNode, float DistanceFromStart, float MinimumCost, float TieBreaker)
    {
        Parent = ParentNode;
        LocationCoordinates = TargetCoordinates;
        PathLengthFromStart = DistanceFromStart;
        MinimumCostToGoal = MinimumCost;
        TieBreakerValue = TieBreaker;
        TotalCost = PathLengthFromStart + MinimumCostToGoal;
    }

    bool operator == (const AStarNode& other) const
    {
        return LocationCoordinates == other.LocationCoordinates;
    }

    virtual MapCoordinates& getCoordinates()
    {
        return LocationCoordinates;
    }
};

struct NodeGreaterThan
{
    bool operator()(const boost::shared_ptr<Node> a, const boost::shared_ptr<Node> b) const
    {
        return !a->costLessThan(*b);
    }

    bool operator()(const Node* a, const Node* b) const
    {
        return !a->costLessThan(*b);
    }

    bool operator()(const Node &a, const Node &b) const
    {
        return !a.costLessThan(b);
    }
};

#endif
