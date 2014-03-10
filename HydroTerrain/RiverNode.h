#ifndef RIVERNODE_H
#define RIVERNODE_H

#include <vector>
#include "BaseNode.h"

//sample river types, can be changed later...
enum RIVER_TYPE_T {
	A,
	B,
	C
};

class RiverNode: public BaseNode
{
public:
	RiverNode();
	~RiverNode();

	void setParent(BaseNode parentNode);

private:
	bool isCandidate;
	RIVER_TYPE_T river_type;
	BaseNode parent;
//	vector<RiverNode> children;


};

#endif