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

// Node types used in expansion rules
enum NODE_TYPE_T {
	CANDIDATE, // Candidate node (alpha)
	INSTANTIATED, // An instantiated node (beta)
	TERMINAL // A node that has been added to the graph and cannot be further extended (tao)
};

// Contain class that holds all necessary info about a River Node
class RiverNode
{
public:
	RiverNode();
	RiverNode(vec3 p);
	~RiverNode(){};

	vec3 getPosition() const;
	static bool compare_node_elevations(const RiverNode& x, const RiverNode& y);
	static bool compare_node_priority_indices(const RiverNode& x, const RiverNode& y);
	friend bool operator== ( RiverNode x, RiverNode y);

	static int getId() {return id++;}

public:
	vec3 position;
	bool isCandidate;
	float edgeLength; // distance to parent
	int pi; // priority index (Horton-Strahler number)
	int myId;
	
	RIVER_TYPE_T river_type;
	NODE_TYPE_T node_type;
	
private:
	static int id;
};


// Override stream operator
inline std::ostream& operator<<(std::ostream &strm, const RiverNode &r)
{
	return strm << "RiverNode id:" << r.myId << " position: " << r.getPosition();
}

#endif