
#include "zone.h"

adjacentNode::adjacentNode(zoneBorderNode *n, float c) : MapCoordinates(*n), node_(n), cost_(c) {}

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
void adjacentNode::setCache(FullPath *cpath)
    {
      cache_ = cpath->PathCourse;
      cost_ = cpath->Length;

      if (cache_.size() > 1)
      {
        assert(node_ != NULL);
        for (zoneBorderNode::iterator it = node_->begin(); it != node_->end(); it++)
        {
          adjacentNode *another = *it;
          if (*another == cache_.back())
          {
            // found the other end
            another->cost_ = cost_;
            another->cache_ = cache_;
            std::reverse(another->cache_.begin(), another->cache_.end());
          }
        }
      }
    }
