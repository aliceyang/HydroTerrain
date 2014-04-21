//	CIS 660  | HydroTerrain Authoring Tool
//
//	Author: Alice Yang
//
//	River Network Node: This is a custom Maya node that is responsible for taking in a 
//	user-created CV curve as the river contour curve, and outputs an array of locations 
//	for the newly expanded set of river nodes
//

#ifndef _RiverNetworkNodeNode
#define _RiverNetworkNodeNode

#include <string>
#include <maya/MPxNode.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MTypeId.h> 
#include <maya/MBoundingBox.h>
#include <vector>
#include "RiverNode.h"
#include "tree.hpp"
#include "vec.h"

// Different expansion types for a River node
enum EXPANSION_TYPE_T
{
	EXPANSION_PA,
	EXPANSION_PS,
	EXPANSION_PC
};

class RiverNetworkNode : public MPxNode
{
public:
	RiverNetworkNode() {};
	virtual	~RiverNetworkNode() {}; 

	virtual MStatus	compute( const MPlug& plug, MDataBlock& data );
	static  void*		creator();
	static  MStatus		initialize();

public:
	static	MTypeId		id;
	static  MObject		inputCurve;		// User-created CV curve as river contour
	static  MObject		outputPoints;	// Outputs array of locations for newly expanded nodes
	static	MObject		riverSlopeFile;

	static vec3 bboxMin;
	static vec3 bboxMax;
	static std::string riverSlopeFilePath;

protected:
	void selectCandidateNode(std::vector<RiverNode> &candidateNodes, RiverNode &candidateNode);
	void expandCandidateNode(RiverNode &candidateNode, tree<RiverNode> &G, std::vector<RiverNode> &candidateNodes);
	double getSlopeValue(const RiverNode &node);
	vec3 getSlopeVector(const RiverNode &node);
	vec3 getGradientVector(const RiverNode &node);
	vec3 getXZJitter();
	EXPANSION_TYPE_T chooseExpansionType();
	bool isNodeCompatible(const RiverNode &node, const tree<RiverNode> &G);
};

#endif
