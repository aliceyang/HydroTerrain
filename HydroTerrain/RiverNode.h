#ifndef RIVERNODE_H
#define RIVERNODE_H

#include <string>
#include "vec.h"


//sample river types, can be changed later...
enum RIVER_TYPE_T {
	A,
	B,
	C
};

// Contain class that holds all necessary info about a River Node
class RiverNode
{
public:
	RiverNode();
	RiverNode(int s);
	RiverNode(vec3 p);
	RiverNode(vec3 p, int s);
	~RiverNode(){};

	vec3 getPosition() const;

	// Alice TODO: getters/setters
	int id;
	vec3 position;
	bool isCandidate;
	float e; // edge length (distance to parent)
	std::string name;
	int pi; // priority index (Horton-Strahler number)
	
	RIVER_TYPE_T river_type;
	// phi: flow
};


// Override stream operator
inline std::ostream& operator<<(std::ostream &strm, const RiverNode &r)
{
	return strm << "RiverNode:" << r.getPosition();
}

#endif