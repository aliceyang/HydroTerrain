#define MNoVersionString
#define MNoPluginEntry

#include "vec.h"
#include "tree_util.hpp"

#include "RiverNetworkNode.h"

#include <maya/MFnPlugin.h>
#include <maya/MFnMesh.h>
#include <maya/MPoint.h>
#include <maya/MFloatPoint.h>
#include <maya/MFloatPointArray.h>
#include <maya/MPointArray.h>

#include <maya/MFnUnitAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnNumericAttribute.h>

#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>

#include <maya/MObject.h>
#include <maya/MFnMeshData.h>
#include <maya/MFnNurbsCurve.h>
#include <maya/MFnNurbsCurveData.h>
#include <maya/MIOStream.h>
#include <maya/MGlobal.h>
#include <maya/MFnArrayAttrsData.h>

// NOTE: THESE NEED TO BE IN INCREASING ORDER
#define EXPANSION_PA_PROB 0.1 // Probability of Asymmetric Horton-Strahler junction
#define EXPANSION_PC_PROB 0.2 // Probability of River Growth
#define EXPANSION_PS_PROB 0.7 // Probability of Symmetric Horton-Strahler junction


#define McheckErr(stat,msg)			\
	if ( MS::kSuccess != stat ) {	\
	cerr << msg;				\
	return MS::kFailure;		\
	}

#define MAKE_INPUT(attr) \
	CHECK_MSTATUS(attr.setKeyable(true)); \
	CHECK_MSTATUS(attr.setStorable(true)); \
	CHECK_MSTATUS(attr.setReadable(true)); \
	CHECK_MSTATUS(attr.setWritable(true));

#define MAKE_OUTPUT(attr) \
	CHECK_MSTATUS(attr.setKeyable(false)); \
	CHECK_MSTATUS(attr.setStorable(false)); \
	CHECK_MSTATUS(attr.setReadable(true)); \
	CHECK_MSTATUS(attr.setWritable(false));

#define MAKE_ADDR(attr) \
	CHECK_MSTATUS(attr.setKeyable(false)); \
	CHECK_MSTATUS(attr.setStorable(false)); \
	CHECK_MSTATUS(attr.setReadable(true)); \
	CHECK_MSTATUS(attr.setWritable(false)); \
	CHECK_MSTATUS(attr.setHidden(true));


MStatus returnStatus;

// This initializes the node’s identifier to a unique tag 
MTypeId     RiverNetworkNode::id( 0x1 );

//The attributes of the node are initialized to NULL values.  
MObject     RiverNetworkNode::inputCurve;        
MObject     RiverNetworkNode::outputPoints;       


void* RiverNetworkNode::creator()
//
// The creator() method simply returns new instances of this node. The return type is a void* so
// Maya can create node instances internally in a general fashion without having to know the return type.
//
{
	return new RiverNetworkNode();
}



MStatus RiverNetworkNode::initialize()
//
//	Description:
//		This method is called to create and initialize all of the attributes
//      and attribute dependencies for this node type.  This is only called 
//		once when the node type is registered with Maya.
//
//	Return Values:
//		MS::kSuccess
//		MS::kFailure
//		
{
	MFnTypedAttribute tAttr; // for nurbsCurve, outputPoints
	MStatus returnStatus;

	// Create attributes
	inputCurve = tAttr.create( "inputCurve", "in", MFnNurbsCurveData::kNurbsCurve, &returnStatus);
	McheckErr(returnStatus, "ERROR creating RiverNetworkNode inputCurve attribute\n");
	MAKE_INPUT(tAttr);

	outputPoints = tAttr.create( "outputPoints", "op", MFnArrayAttrsData::kDynArrayAttrs, &returnStatus);
	McheckErr(returnStatus, "ERROR creating RiverNetworkNode outputPoints attribute\n");
	MAKE_OUTPUT(tAttr);

	// Add attributes
	returnStatus = addAttribute( inputCurve );
	McheckErr(returnStatus, "ERROR adding inputCurve attribute\n");

	returnStatus = addAttribute( outputPoints );
	McheckErr(returnStatus, "ERROR adding outputPoints attribute\n");

	// Set up a dependency between the input and the output.  This will cause
	// the output to be marked dirty when the input changes.  The output will
	// then be recomputed the next time the value of the output is requested.
	// 
	returnStatus = attributeAffects(inputCurve, outputPoints);
	McheckErr(returnStatus, "ERROR in attributeAffects\n");

	return MS::kSuccess;
}

MStatus RiverNetworkNode::compute( const MPlug& plug, MDataBlock& data )
	//
	//	Description:
	//		This method computes the value of the given output plug based
	//		on the values of the input attributes.
	//
	//	Arguments:
	//		plug - the plug to compute
	//		data - object that provides access to the attributes for this node
	//
{
	MStatus returnStatus;

	// Check which output attribute we have been asked to compute.  If this 
	// node doesn't know how to compute it, we must return 
	// MS::kUnknownParameter.
	// 
	if( plug == outputPoints )
	{
		// Get a handle to the input attribute that we will need for the
		// computation.  If the value is being supplied via a connection 
		// in the dependency graph, then this call will cause all upstream  
		// connections to be evaluated so that the correct value is supplied.
		// 
		MDataHandle inputCurveData = data.inputValue( inputCurve, &returnStatus );
		McheckErr(returnStatus, "ERROR getting inputCurve data handle\n");

		MObject curve = inputCurveData.asNurbsCurve();
		MFnNurbsCurve curveFn (curve, &returnStatus); // Can also set as MItCurveCV
		McheckErr(returnStatus, "ERROR creating curve function set\n");

		// Sanity check
		//double curveLength = curveFn.length();
		//cout << "DEBUG: CURVE LENGTH IS: " << curveLength << endl;

		// Grab the CV points from the input curve
		MPointArray cvs;
		returnStatus = curveFn.getCVs(cvs, MSpace::kWorld);
		McheckErr(returnStatus, "ERROR  in getting CVs\n");

		
		// **************************
		//  RIVER NETWORK GENERATION
		// **************************

		// Create the initial set of candidate nodes and add them as separate heads in a tree
		// Algorithm creates coverage of input domain by a set of trees denoted as G
		std::vector<RiverNode>candidateNodes;
		tree<RiverNode>G;
		tree<RiverNode>::iterator top;
		top = G.begin();

		for (int i = 0; i < cvs.length(); i++)
		{
			MPoint pos = cvs[i];
			RiverNode r(vec3(pos.x, pos.y, pos.z));
			r.node_type = CANDIDATE;
			candidateNodes.push_back(r);
			G.insert(top, r);
		}

		// ALICE TODO: While there are still candidate nodes, select one and expand. Expansion 
		// cease when it is not possible anymore (i.e. run out of candidate nodes)

		// 1. NODE SELECTION: Choose a node Nx to expand from the list of candidate nodes X
		RiverNode candidateNode;
		selectCandidateNode(candidateNodes, candidateNode);

			
		// 2. NODE EXPANSION + CREATION: Expand the candidate node Nx and perform geometric 
		// tests to verify that the new nodes {N} are comptible with the previously created 
		// ones, then update the list of candidate nodes X: X <- (X \ {Nx}) U {N}
		expandCandidateNode(candidateNode, G, candidateNodes);








		// SANITY CHECK
		kptree::print_tree_tabbed(G, std::cout);


		// Get a handle to the output attribute.  This is similar to the
		// "inputValue" call above except that no dependency graph 
		// computation will be done as a result of this call.
		// 
		MDataHandle outputHandle = data.outputValue( outputPoints );

		MFnArrayAttrsData outputPointsAAD;
		MObject outputPointsObj = outputPointsAAD.create(&returnStatus);
		McheckErr(returnStatus, "ERROR creating output points object\n");

		// Create vectors for position and id
		MVectorArray positionArray = outputPointsAAD.vectorArray("position");
		MDoubleArray idArray = outputPointsAAD.doubleArray("id");

		// Traverse through all nodes in G tree and output their positions
		int headCount = G.number_of_siblings(G.begin());
		int headNum = 0;
		int id = 0;
		// Iterate through the head nodes and iterate through each head node's tree/children
		for (tree<RiverNode>::sibling_iterator iRoot = G.begin(); iRoot != G.end(); ++iRoot, ++headNum)
		{
			tree<RiverNode>::iterator it = iRoot;
			tree<RiverNode>::iterator end = G.end(iRoot);
			while (it != end)
			{
				RiverNode currNode = *it;
				MPoint currNodePos(currNode.position[0],currNode.position[1],currNode.position[2]);
				positionArray.append(MVector(currNodePos));
				idArray.append(id); // TODO ALICE Replace with node.myid?
				id++;
				++it;
			}
		}

		outputHandle.setMObject(outputPointsObj);

		// Mark the destination plug as being clean.  This will prevent the
		// dependency graph from repeating this calculation until an input 
		// of this node changes.
		// 
		data.setClean(plug);

	} else {
		return MS::kUnknownParameter;
	}

	return MS::kSuccess;
}




















double RiverNetworkNode::lookUpSlopeValue(const RiverNode &node)
	// Takes the (x,z) coordinates of a node and looks up the corresponding river slope value on
	// the user-provided river slope map. The value is calculated by taking the corners of the image
	// map and fitting it to the bounding box of the CV curve.
{
	vec3 pos = node.position;
	// ALICE TODO
	return rand() % 3;
}

void RiverNetworkNode::selectCandidateNode(std::vector<RiverNode> &candidateNodes, RiverNode &candidateNode)
	// 1. NODE SELECTION: Choose a node Nx to expand from the list of candidate nodes X
{
	// 1.1 Find the elevation z of the lowest located candidate
	std::vector<RiverNode>::iterator lowestElevationCandidate = std::min_element(candidateNodes.begin(), candidateNodes.end(), RiverNode::compare_node_elevations);
	double z = lowestElevationCandidate->position[1];
	double zeta = 0;

	// 1.2 Consider a subset of candidate nodes who's elevation is within the range [z, z + zeta]
	std::vector<RiverNode> candidateNodesSubset;
	for (int i = 0; i < candidateNodes.size(); i++)
	{
		RiverNode currNode = candidateNodes[i];
		if (currNode.position[1] <= (z+zeta))
		{
			candidateNodesSubset.push_back(currNode);
		}
	}

	// 1.3 Choose from the subset the node Nx with the highest priority. 
	std::vector<RiverNode>::iterator highestPriorityCandidate = std::max_element(candidateNodesSubset.begin(), candidateNodesSubset.end(), RiverNode::compare_node_priority_indices);
	candidateNode = *highestPriorityCandidate;
	cout << "candidateNode: " << candidateNode.position << endl;
}

void RiverNetworkNode::expandCandidateNode(RiverNode &candidateNode, tree<RiverNode> &G, std::vector<RiverNode> &candidateNodes)
	// 2. NODE EXPANSION: Expand the candidate node Nx and perform geometric tests to verify
	// that the new nodes {N} are comptible with the previously created ones
{
	if (candidateNode.pi == 1)
	{
		// Filling (ALICE TODO)
		// For now lets just stop expansion and remove the candidate node from the vector
		// http://stackoverflow.com/questions/39912/how-do-i-remove-an-item-from-a-stl-vector-with-a-certain-value
		candidateNodes.erase(std::remove(candidateNodes.begin(), candidateNodes.end(), candidateNode), candidateNodes.end());
	}
	else if (candidateNode.pi > 1)
	{
		EXPANSION_TYPE_T expansionType = chooseExpansionType();

		// Asymmetric Horton-Strahler junction
		if (expansionType == EXPANSION_PA)
		{
			// expand PA
		}
		// River growth (no branching)
		if (expansionType == EXPANSION_PC)
		{
			// expand PC
		}
		// Symmetric Horton-Strahler junction ALICE TODO
		if (expansionType == EXPANSION_PS)
		{
			RiverNode t;
			t.node_type = TERMINAL;
			t.pi = candidateNode.pi;
			vec3 expansionDirection = vec3(0,0,0) - candidateNode.position;
			double distToCenter = Distance(vec3(0,0,0), candidateNode.position);
			double multiplier = 0.3;//t.edgeLength / distToCenter;
			t.position = candidateNode.position + multiplier * expansionDirection + vec3 (rand()%3, rand()%3, rand()%3);
			
			// Append terminal node as child of candidate node
			tree<RiverNode>::iterator candidateNodeLoc = find (G.begin(), G.end(), candidateNode); // DEBUG THIS
			tree<RiverNode>::iterator tNodeLoc;
			if (candidateNodeLoc != G.end())
			{
				tNodeLoc = G.append_child(candidateNodeLoc, t);
			}

			// Skipping compatibility check. Normally B nodes should be geographically compatible before being added

			RiverNode b1;
			b1.node_type = INSTANTIATED;
			b1.pi = candidateNode.pi - 1;
			b1.position = t.position + multiplier * expansionDirection + vec3 (rand()%3, rand()%3, rand()%3) + vec3 (0, lookUpSlopeValue(t), 0);
			// Append this node as child of terminal node
			G.append_child(tNodeLoc, b1); // if (b1.isCompatible)
			
			RiverNode b2;
			b2.node_type = INSTANTIATED;
			b2.pi = candidateNode.pi - 1;
			b2.position = t.position + multiplier * expansionDirection + vec3 (rand()%3, rand()%3, rand()%3) + vec3 (0, lookUpSlopeValue(t), 0);
			// Append this node as child of terminal node
			G.append_child(tNodeLoc, b2); // if (b2.isCompatible)
			
			// Update list of candidate nodes
			candidateNodes.erase(std::remove(candidateNodes.begin(), candidateNodes.end(), candidateNode), candidateNodes.end());
			candidateNodes.push_back(b2);
			candidateNodes.push_back(b1);
		}
	}
}

EXPANSION_TYPE_T RiverNetworkNode::chooseExpansionType()
{
	// Probabilistically determines which type of river expansion to use
	// http://www.vcskicks.com/random-element.php

	float p = (rand() / static_cast<float>(RAND_MAX));
	double cumulativePA = EXPANSION_PA_PROB;
	double cumulativePC = EXPANSION_PA_PROB + EXPANSION_PC_PROB;
	double cumulativePS = EXPANSION_PA_PROB + EXPANSION_PC_PROB + EXPANSION_PS_PROB;

	EXPANSION_TYPE_T expansionType = EXPANSION_PA;

	if ( p <= cumulativePA)
		expansionType = EXPANSION_PA;
	else if (p <= cumulativePC)
		expansionType = EXPANSION_PC;
	else if (p <= cumulativePS)
		expansionType = EXPANSION_PS;

	//return expansionType;
	return EXPANSION_PS; // TESTING
}