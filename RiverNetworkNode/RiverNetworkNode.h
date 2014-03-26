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

#include <maya/MPxNode.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MTypeId.h> 
 
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
};

#endif
