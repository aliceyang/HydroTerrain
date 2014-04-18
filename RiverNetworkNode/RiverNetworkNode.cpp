#define MNoVersionString
#define MNoPluginEntry

#include "tree_util.hpp"
#include "CImg.h"

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

using namespace cimg_library;

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
MObject		RiverNetworkNode::riverSlopeFile;

std::string RiverNetworkNode::riverSlopeFilePath;
vec3 RiverNetworkNode::bboxMax;
vec3 RiverNetworkNode::bboxMin;

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

	riverSlopeFile = tAttr.create("riverSlopeFile", "rs", MFnData::kString, &returnStatus);
	McheckErr(returnStatus, "ERROR creating RiverNetworkNode riverSlopeFile attribute\n");
	MAKE_INPUT(tAttr);

	outputPoints = tAttr.create( "outputPoints", "op", MFnArrayAttrsData::kDynArrayAttrs, &returnStatus);
	McheckErr(returnStatus, "ERROR creating RiverNetworkNode outputPoints attribute\n");
	MAKE_OUTPUT(tAttr);

	// Add attributes
	returnStatus = addAttribute( inputCurve );
	McheckErr(returnStatus, "ERROR adding inputCurve attribute\n");

	returnStatus = addAttribute(riverSlopeFile);
	McheckErr(returnStatus, "ERROR adding riverSlopeFile attribute\n");

	returnStatus = addAttribute( outputPoints );
	McheckErr(returnStatus, "ERROR adding outputPoints attribute\n");

	// Set up a dependency between the input and the output.  This will cause
	// the output to be marked dirty when the input changes.  The output will
	// then be recomputed the next time the value of the output is requested.
	// 
	returnStatus = attributeAffects(inputCurve, outputPoints);
	McheckErr(returnStatus, "ERROR in attributeAffects\n");

	returnStatus = attributeAffects(riverSlopeFile, outputPoints);
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
		McheckErr(returnStatus, "ERROR in getting curve CVs\n");

		// Manually calculate the bounding box because Maya sucks
		double minX, minY, minZ, maxX, maxY, maxZ;
		MPoint firstPt = cvs[0];
		minX = maxX = firstPt.x;
		minY = maxY = firstPt.y;
		minZ = maxZ = firstPt.z;

		for(int i = 1; i < cvs.length(); i++)
		{
			MPoint pos = cvs[i];
			if (pos.x < minX)
				minX = pos.x;
			if (pos.x > maxX)
				maxX = pos.x;
			if (pos.y < minY)
				minY = pos.y;
			if (pos.y > maxY)
				maxY = pos.y;
			if (pos.z < minZ)
				minZ = pos.z;
			if (pos.z > maxZ)
				maxZ = pos.z;
		}

		bboxMin = vec3(minX, minY, minZ);
		bboxMax = vec3(maxX, maxY, maxZ);

		// Verified correct
		//std::cout << "bbMin: " << bboxMin << std::endl;
		//std::cout << "bbMax " << bboxMax << std::endl;

		// Grab the river slope image file for the curve (hard-coded for now)
		MDataHandle riverSlopeFileData = data.inputValue(riverSlopeFile, &returnStatus);
		McheckErr(returnStatus, "ERROR getting riverSlopeFile data handle\n");
		MString riverSlopeFileValue = riverSlopeFileData.asString();
		riverSlopeFilePath = riverSlopeFileValue.asChar();
		riverSlopeFilePath = "../HydroTerrain/img/grey.bmp"; // ALICE TODO remove this later


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

		// TESTING
		//for (int i = 0; i < 10; i++)
		//{
		//	selectCandidateNode(candidateNodes, candidateNode);
		//	expandCandidateNode(candidateNode, G, candidateNodes);
		//}
		
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
				idArray.append(currNode.myId);
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




















double RiverNetworkNode::getSlopeValue(const RiverNode &node)
	// Takes the (x,z) coordinates of a node and looks up the corresponding river slope value on
	// the user-provided river slope map. The value is calculated by taking the corners of the image
	// map and fitting it to the bounding box of the CV curve. River slope value defines the magnitude
	// of variation from the given node.
{
	vec3 pos = node.position;
	double ratioX, ratioZ;
	int imgX, imgZ;
	int imgWidth, imgHeight;

	ratioX = (pos[0]-bboxMin[0])/(bboxMax[0]-bboxMin[0]);
	ratioZ = (pos[2]-bboxMin[2])/(bboxMax[2]-bboxMin[2]);
	
	CImg<double> src("../HydroTerrain/img/grey.bmp"); // TODO should source from riverSlopeFilePath
	imgWidth = src.width() - 1; // convert to 0-indexed
	imgHeight = src.height() - 1;

	imgX = int(ratioX * imgWidth);
	imgZ = int(ratioZ * imgHeight);

	float r = src(imgX, imgZ, 0, 0); // last value is R from RGB, r ranges from 0 to 255

	std::cout << "imgWidth: " << imgWidth << " imgHeight: " << imgHeight << std::endl;
	std::cout << "imgX: " << imgX << " imgZ: " << imgZ << std::endl;
	std::cout << "slope value: " << r/100.0 << "\n" << std::endl;

	return r/100.0; // normalize to a reasonable height
}



vec3 RiverNetworkNode::getSlopeVector(const RiverNode &node)
	// Returns a vec3 with the slope value as the y value, and 0 for x and z
{
	return vec3 (0, getSlopeValue(node), 0);
}



vec3 RiverNetworkNode::getGradientVector(const RiverNode &node)
	// Takes the (x,z) coordinates of a node and calculates the local (x,z) gradient vector on
	// the user-provided river slope map. The value is calculated by the surrounding 3x3 kernel
{
	vec3 pos = node.position;

	double ratioX, ratioZ;
	int imgX, imgZ;
	int imgWidth, imgHeight;

	ratioX = (pos[0]-bboxMin[0])/(bboxMax[0]-bboxMin[0]);
	ratioZ = (pos[2]-bboxMin[2])/(bboxMax[2]-bboxMin[2]);

	CImg<double> src("../HydroTerrain/img/grey.bmp"); // should source from riverSlopeFilePath
	imgWidth = src.width() - 1; // convert to 0-indexed
	imgHeight = src.height() - 1;

	imgX = int(ratioX * imgWidth);
	imgZ = int(ratioZ * imgHeight);

	double gradX, gradZ;

	// Calculate gradient in the X direction, with border correction
	if (imgX == 0)
	{
		imgX = 1;
	}
	if (imgX == imgWidth)
	{
		imgX = imgWidth - 1;
	}

	// Calculate gradient in the Z direction, with border correction
	if (imgZ == 0)
	{
		imgZ = 1;
	}
	if (imgZ == imgHeight)
	{
		imgZ = imgHeight - 1;
	}

	std::cout << "corrected imgX, imgZ: " << imgX << ", " << imgZ << std::endl;

	double xNext = src(imgX+1, imgZ, 0,0);
	double xPrev = src(imgX-1, imgZ, 0,0);
	double zNext = src(imgX, imgZ+1, 0, 0);
	double zPrev = src(imgX, imgZ-1, 0, 0);

	std::cout << "xNext, xPrev: " << xNext << ", " << xPrev << std::endl;
	std::cout << "zNext, zPrev: " << zNext << ", " << zPrev << std::endl;

	gradX = xNext - xPrev;
	gradZ = zNext - zPrev;

	std::cout << "(gradX, gradZ):" << gradX << " " << gradZ << "\n" << std::endl;

	return vec3 (gradX, 0, gradZ);
}



vec3 RiverNetworkNode::getXZJitter()
	// Returns a vec3 with small random values in X and Z fields
{
	//return vec3 (rand()%3, rand()%3, rand()%3);
	return vec3 (0,0,0);
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
	cout << "candidateNode: " << candidateNode.position << "\n" << endl;
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
			// Terminal Node
			RiverNode t;
			t.node_type = TERMINAL;
			t.pi = candidateNode.pi;
			vec3 expansionDirection_t = getGradientVector(candidateNode) + getSlopeVector(candidateNode) + getXZJitter();
			t.position = candidateNode.position + expansionDirection_t;

			// Append terminal node as child of candidate node
			tree<RiverNode>::iterator candidateNodeLoc = find (G.begin(), G.end(), candidateNode);
			tree<RiverNode>::iterator tNodeLoc;
			tNodeLoc = G.append_child(candidateNodeLoc, t);


			// Skipping compatibility check. Normally B nodes should be geographically compatible before being added ALICE TODO

			// Symmetric River Nodes
			RiverNode b1;
			b1.node_type = INSTANTIATED;
			b1.pi = t.pi - 1;
			vec3 expansionDirection_b1 = getGradientVector(t) + getSlopeVector(t) + getXZJitter();
			b1.position = t.position + expansionDirection_b1;
			// Append this node as child of terminal node
			G.append_child(tNodeLoc, b1); // if (b1.isCompatible)

			RiverNode b2;
			b2.node_type = INSTANTIATED;
			b2.pi = t.pi - 1;
			vec3 expansionDirection_b2 = getGradientVector(t) + getSlopeVector(t) + getXZJitter();
			b2.position = t.position + expansionDirection_b2;
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