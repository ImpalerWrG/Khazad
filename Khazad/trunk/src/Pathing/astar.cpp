#include <vector>
#include <algorithm>

#include <PathManager.h>

#include "astar.h"
#include "Path.h"
#include "zone.h"
#include "grid.h"
#include "Pool.h"


AStar::AStar(const gridInterface *TargetSearchGraph)
{
    SearchGraph = TargetSearchGraph;
    NodePool = PATH->getCentralNodePool()->ProvidePool();
    FinalPath = NULL;
}

AStar::~AStar()
{
    NodePool->Release();
}

void AStar::setEndPoints(const MapCoordinates &StartCoords, const MapCoordinates &GoalCoords)
{
    StartCoordinates = StartCoords;
    GoalCoordinates = GoalCoords;
    GraphReads = ExpandedNodes = 0;
    FinalPath = NULL;
    FringeExausted = false;

    FringeNodes.clear();
    VisitedCoordinates.clear();

    NodePool->Wipe();

    AStarNode* StartNode = NodePool->ProvideObject();
    StartNode->Set(StartCoordinates, NULL, 0, MainHeuristic->Estimate(StartCoords, GoalCoords), TieBreakerHeuristic->Estimate(StartCoords, GoalCoords));

    FringeNodes.push_back(StartNode);
    std::make_heap(FringeNodes.begin(), FringeNodes.end(), NodeGreaterThan());

    //VisitedCoordinates.insert(StartCoordinates);
};

bool AStar::SearchPath(int NodesToExpand)
{
    if (FringeExausted)
    {
        return false; // No more searching can be done
    }

    if (FinalPath == NULL)
    {
        bool GoalFound;
        if (NodesToExpand > 0)  // Search for a limited time
        {
            int RemainingNodes = NodesToExpand;
            while (!FringeNodes.empty())
            {
                if (RemainingNodes-- > 0 )
                {
                    if (ExpandNode())
                    {
                        return true; // Path found
                    }
                }
                return false; // Path not yet found
            }
            FringeExausted = true; // Path could not be found
            return false;
        }
        else // Search untill Path is found or Fringe is exhausted
        {
            while (!FringeNodes.empty())
            {
                if (ExpandNode())
                {
                    return true;
                }
            }
            FringeExausted = true;
            return false;
        }
    }
    return true;  // Final Path already found don't do any more searching
}

MapPath* AStar::FindPath(int NodesToExpand)
{
    if (FringeExausted)
    {
        return NULL; // Fringe Exhastion, don't return a useless path
    }

    bool FinalPathFound = SearchPath(NodesToExpand);

    if (FinalPath == NULL)
    {
        MapPath* CurrentPath = GenerateBestPath();

        if (FinalPathFound)
        {
            FinalPath = CurrentPath;
            NodePool->Wipe();  // Nodes can be released now that a final path has been found
        }

        return CurrentPath;
    }
    return FinalPath;
}

inline bool AStar::ExpandNode()
{
    // Get the min-weight element off the fringe.
    std::pop_heap(FringeNodes.begin(), FringeNodes.end(), NodeGreaterThan());
    CurrentNode = FringeNodes.back();
    FringeNodes.pop_back();

    MapCoordinates TestCoordinates = CurrentNode->LocationCoordinates;

    if (VisitedCoordinates.find(TestCoordinates) != VisitedCoordinates.end())
    {
        return false;
    }

    if (TestCoordinates == GoalCoordinates)
    {
        return true; //GenerateBestPath();
    }

    // mark as VisitedCoordinates if not already VisitedCoordinates
    VisitedCoordinates.insert(TestCoordinates);

    MapCoordinates NeiboringCoordinates;
    DirectionFlags TestDirections = SearchGraph->getDirectionFlags(TestCoordinates);

    // Check all Neibors
    for (Direction DirectionType = ANGULAR_DIRECTIONS_START; DirectionType < NUM_ANGULAR_DIRECTIONS; ++DirectionType)
    {
        if (TestDirections & (1 << (int) DirectionType))  // Connectivity is valid for this direction
        {
            NeiboringCoordinates = TestCoordinates;
            NeiboringCoordinates.TranslateMapCoordinates(DirectionType);

            // If Coordinate is not already on the VisitedCoordinates list
            if (VisitedCoordinates.find(NeiboringCoordinates) == VisitedCoordinates.end())
            {
                float EdgeCost = SearchGraph->getEdgeCost(TestCoordinates, DirectionType);
                GraphReads++;

                AStarNode* NewNode = NodePool->ProvideObject();
                NewNode->Set(NeiboringCoordinates, CurrentNode, CurrentNode->PathLengthFromStart + EdgeCost, MainHeuristic->Estimate(NeiboringCoordinates, GoalCoordinates), TieBreakerHeuristic->Estimate(NeiboringCoordinates, GoalCoordinates));

                // Add the new Node to the Fringe
                FringeNodes.push_back(NewNode);
                std::push_heap(FringeNodes.begin(), FringeNodes.end(), NodeGreaterThan());
            }
        }
    }

    return false; // Goal was not found
}

MapPath* AStar::GenerateBestPath()
{
    ExpandedNodes = VisitedCoordinates.size();

    int PathLength = CurrentNode->PathLengthFromStart;
    std::vector<MapCoordinates> Course;

    while(CurrentNode != NULL)
    {
        Course.push_back(CurrentNode->LocationCoordinates);
        CurrentNode = CurrentNode->Parent;
    }
    Course.push_back(StartCoordinates);

    reverse(Course.begin(), Course.end());
    return new FullPath(PathLength, Course);
}

/*
class scopedAddBorderNode
{
public:
    scopedAddBorderNode(zone *z, const MapCoordinates &p, const Heuristic *h) : z_(z), zoneModified(false)
    {
        zbn_ = z->get(p);
        if (zbn_ == NULL)
        {
//          fprintf(stderr,"adding bordernode: (%2d,%2d,%2d)\n",p[0],p[1],p[2]);
            zbn_ = z->addBorderNode(p,h);
            zoneModified = true;
        }
//        else
//          fprintf(stderr,"Using bordernode: (%2d,%2d,%2d)\n",p[0],p[1],p[2]);
    }

    ~scopedAddBorderNode()
    {
        if (zoneModified)
            z_->removeBorderNode(zbn_);
#ifdef ZONE_DEBUG
        z_->checkValid();
#endif
    }

private:
    zone* z_;
    zoneBorderNode* zbn_;
    bool zoneModified;
};

//typedef boost::shared_ptr<AStarZoneNode> AStarZoneNodePtr;

/*
FullPath* HierarchicalAStar::doFindPath (const MapCoordinates &StartCoordinates, const MapCoordinates &GoalCoordinates)
{
    zone* zoneS = zm_->findContainingZone(StartCoordinates);
    zone* zoneT = zm_->findContainingZone(GoalCoordinates);

    if ((zoneS == NULL) || (zoneT == NULL))
        return new FullPath();

    //add source and dest to zone
    scopedAddBorderNode sbs(zoneS, StartCoordinates, MainHeuristic), sbt(zoneT, GoalCoordinates, MainHeuristic);

    zoneBorderNode* bs = zoneS->get(StartCoordinates);
    //zoneBorderNode* bt = zoneT->get(GoalCoordinates);

    NodeGreaterThan egt(GoalCoordinates, MainHeuristic);
    std::vector<AStarZoneNodePtr> fringe;
    boost::unordered_set<MapCoordinates,MapCoordinates::hash> VisitedCoordinates;

    std::make_heap(fringe.begin(), fringe.end(),egt);

    adjacentNode ans = adjacentNode(bs,0);
    ans.cache_.push_back(StartCoordinates);
    ans.cache_.push_back(StartCoordinates);

    fringe.push_back(AStarZoneNodePtr(new AStarZoneNode(&ans,0)));
    std::push_heap(fringe.begin(),fringe.end(),egt);

    while (!fringe.empty())
    {
        // Get the min-weight element off the fringe.
        std::pop_heap(fringe.begin(),fringe.end(),egt);
        AStarZoneNodePtr e = fringe.back();
        fringe.pop_back();

        if ((VisitedCoordinates.find(*e->node_) != VisitedCoordinates.end()) || (!SearchGraph->contains(*e->node_)))
            continue;

        if (!e->cached())
        {
            //cachemiss++;
            //no cached path. (current cost is only an estimate)

            FullPath *cpath;
            //find the path, cache it
            ZonedGridGraph zgg(SearchGraph, zm_->findContainingZone(e->getCoordinates()));
            if (zm_->down() == NULL)
            {
                AStar lastar(&zgg,MainHeuristic,TieBreakerHeuristic);
                cpath = lastar.doFindPath(e->path_.back(),e->getCoordinates());
                //count += lastar.getCount();
            }
            else
            {
                HierarchicalAStar dastar(&zgg,zm_->down(),MainHeuristic,TieBreakerHeuristic);
                cpath = dastar.doFindPath(e->path_.back(),e->getCoordinates());
                //count += dastar.getCount();
                //cachemiss += dastar.cachemiss;
            }

            e->node_->setCache(cpath);
            //e->node_->cache_ = cpath->PathCourse; //update the cache!
            //e->node_->cost_ = cpath->Length;

            /*MapCoordinates p = e->getPoint();
            MapCoordinates q = e->path_.back();
            printf("Examine: (%2d,%2d,%2d)->(%2d,%2d,%2d) - %d:%d\n",q[0],q[1],q[2],p[0],p[1],p[2],e->path_.size()+cpath.second.size()-1,e->cost_+cpath.first);
            std::vector<MapCoordinates>::iterator it = e->path_.begin();
            it++;
            for (; it != e->path_.end(); it++)
              printf("(%2d,%2d,%2d)->",(*it)[0],(*it)[1],(*it)[2]);
            for (it = cpath.second.begin(); it != cpath.second.end(); it++)
              printf("(%2d,%2d,%2d)->",(*it)[0],(*it)[1],(*it)[2]);
            printf(": %d\n",e->cost_+cpath.first);


            //if disconnected, ignore it
            if (cpath->Length >= 0)
            {
                //now let's try again with the real cost
                fringe.push_back(e);
                std::push_heap(fringe.begin(),fringe.end(),egt);
            }
            delete cpath;
            continue;
        }

        //count++;

        // if it's the destination, congratuations, we win a prize!
        if (e->getCoordinates() == GoalCoordinates)
        {
            e->path_.insert(e->path_.end(), ++e->node_->cache_.begin(),e->node_->cache_.end());
            return new FullPath(e->value(GoalCoordinates,*MainHeuristic),e->path_);
        }

        // mark it VisitedCoordinates if not already VisitedCoordinates
        VisitedCoordinates.insert(e->getCoordinates());
        assert(*e->node_ == *e->node_->node_);

        e->node_->node_->checkValid();
        // relax neighbours
        for (zoneBorderNode::iterator nit = e->node_->node_->begin(); nit != e->node_->node_->end(); ++nit)
        {
            adjacentNode *neigh = *nit;

            //printf("%g: (%2d,%2d,%2d)->(%2d,%2d,%2d) ~ %g=? ",e->node_->cost_,(*e->node_)[0],(*e->node_)[1],(*e->node_)[2],(*neigh)[0],(*neigh)[1],(*neigh)[2],neigh->cost_);

            // try to find ey in the VisitedCoordinates set
            if (VisitedCoordinates.find(*neigh) != VisitedCoordinates.end())
            {
                //printf("No -- VisitedCoordinates\n");
                continue;
            }

            if (neigh->cost_ < 0)
            {
                //printf("No -- not an edge\n");
                continue; //Not valid edge
            }

            assert(neigh != e->node_);

            AStarZoneNode* eneigh (new AStarZoneNode(neigh, *e, (*TieBreakerHeuristic)(*neigh, GoalCoordinates)));

#if 0
            typedef std::vector<AStarZoneNode*>::iterator eiterator;
            // try to find neigh in the fringe
            eiterator it = std::find(fringe.begin(), fringe.end(), eneigh);
            if (it != fringe.end())
            {
                // it's here, we just haven't gotten there yet; decrease_key if applicable
                //if both cached or shortest one cached, choose shortest
                if ((eneigh->value(t,*MainHeuristic) < (*it)->value(t,*MainHeuristic)) && (eneigh->cached()))
                {
                    //printf("Zdecreasing node cost (%2d,%2d,%2d)\n",(*neigh)[0],(*neigh)[1],(*neigh)[2]);
                    fringe.erase(it);
                    fringe.push_back(eneigh);
                    std::make_heap(fringe.begin(),fringe.end(),egt);
                }
                else
                {
                    //else we have to keep both =(
                    //printf("Not yet\n");
                    fringe.push_back(eneigh);
                    std::push_heap(fringe.begin(), fringe.end(), egt);
                }
                continue;
            }
#endif

            //printf("Ok\n");
            // ey was found neither in the fringe nor in VisitedCoordinates; add it to the fringe

            fringe.push_back(eneigh);
            std::push_heap(fringe.begin(),fringe.end(),egt);
        } // end loop over neighbours
    } // end loop over fringe
    return new FullPath();
}
*/
