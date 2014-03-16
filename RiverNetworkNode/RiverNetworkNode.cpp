#define MNoVersionString
#define MNoPluginEntry

#include <vector>

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

		// testing
		double curveLength = curveFn.length();
		cout << "DEBUG: CURVE LENGTH IS: " << curveLength << endl;

		// Grab the CV points from the input curve
		MPointArray cvs;
		returnStatus = curveFn.getCVs(cvs, MSpace::kWorld);
		McheckErr(returnStatus, "ERROR  in getting CVs\n");

		std::vector<MPoint>riverNodes;

		for (int i = 0; i < cvs.length(); i++)
		{
			MPoint point = cvs[i];
			riverNodes.push_back(cvs[i]);
			cout << "Adding CV Point: " << point[0] << " " << point[1] << " " << point[2] << "\n" << endl;
		}


		// ************************
		// TODO: EXPAND RIVER NODES
		// ************************
		


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

		// Loop through vector of expanded set of river nodes and output their positions
		for (int i = 0; i < riverNodes.size(); i++)
		{
			MPoint currNode = riverNodes[i];
			positionArray.append(MVector(currNode));
			idArray.append(i);
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