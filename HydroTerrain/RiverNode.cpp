#include "RiverNode.h"

#define DEFAULT_PRIORITY_INDEX 3
#define DEFAULT_EDGE_LENGTH 5

int RiverNode::id = 0;

RiverNode::RiverNode() : myId(getId())
{
	pi = DEFAULT_PRIORITY_INDEX;
	position = vec3(0,0,0);
	edgeLength = DEFAULT_EDGE_LENGTH;
}

RiverNode::RiverNode(vec3 p) : myId(getId())
{
	position = p;
	pi = DEFAULT_PRIORITY_INDEX;
	edgeLength = DEFAULT_EDGE_LENGTH;
}

vec3 RiverNode::getPosition() const
{
	return position;
}

bool RiverNode::compare_node_elevations(const RiverNode& x, const RiverNode& y)
{
	return x.position[1] < y.position[1];
}

bool RiverNode::compare_node_priority_indices(const RiverNode& x, const RiverNode& y)
{
	return x.pi < y.pi;
}

bool operator == (RiverNode x, RiverNode y)
{
	return x.myId == y.myId;
}