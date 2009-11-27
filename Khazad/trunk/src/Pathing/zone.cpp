/*
#include "zone.h"

adjacentNode::adjacentNode(zoneBorderNode *n, cost_t c) : MapCoordinates(*n), node_(n), cost_(c) {}

void zoneBorderNode::invalidateCache(const zone *affected, const Heuristic *h)
{
    for (iterator it = at_.begin(); it != at_.end(); it++)
    {
        adjacentNode* an = *it;
        if (an->node_->owner_->equals(affected))
        {
            an->cache_.clear();               //invalidate the previous path
            an->cost_ = (*h)(*this,*an->node_);  //estimate the cost
        }
    }
}

*/
