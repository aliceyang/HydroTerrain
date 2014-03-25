#include "RiverNode.h"

RiverNode::RiverNode()
{
	pi = 4;
}

RiverNode::RiverNode(int s)
{
	position = vec3(0,0,0);
	pi = s;
}

RiverNode::RiverNode(vec3 p, int s)
{
	position = p;
	pi = s;
}

vec3 RiverNode::getPosition() const
{
	return position;
}
