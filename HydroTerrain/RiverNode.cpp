#include "RiverNode.h"

#define DEFAULT_PRIORITY_INDEX 4
#define DEFAULT_EDGE_LENGTH 5

RiverNode::RiverNode()
{
	pi = DEFAULT_PRIORITY_INDEX;
	position = vec3(0,0,0);
	e = DEFAULT_EDGE_LENGTH;
}

RiverNode::RiverNode(int s)
{
	position = vec3(0,0,0);
	pi = s;
	e = DEFAULT_EDGE_LENGTH;
}

RiverNode::RiverNode(vec3 p)
{
	position = p;
	pi = DEFAULT_PRIORITY_INDEX;
	e = DEFAULT_EDGE_LENGTH;
}

RiverNode::RiverNode(vec3 p, int s)
{
	position = p;
	pi = s;
	e = DEFAULT_EDGE_LENGTH;
}

vec3 RiverNode::getPosition() const
{
	return position;
}
