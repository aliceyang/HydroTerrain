#define MNoVersionString
#define MNoPluginEntry

#include "RiverNetworkNode.h"

#include <maya/MFnPlugin.h>
#include <maya/MFnMesh.h>
#include <maya/MPoint.h>
#include <maya/MFloatPoint.h>
#include <maya/MFloatPointArray.h>

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


// The creator() method simply returns new instances of this node. The return type is a void* so
// Maya can create node instances internally in a general fashion without having to know the return type.
void* RiverNetworkNode::creator()
{
	return new RiverNetworkNode();
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
		MDataHandle inputData = data.inputValue( inputCurve, &returnStatus );

		if( returnStatus != MS::kSuccess )
			MGlobal::displayError( "Node RiverNetworkNode cannot get value\n" );
		else
		{
			// Read the input value from the handle.
			//
			float result = inputData.asFloat();

			// Get a handle to the output attribute.  This is similar to the
			// "inputValue" call above except that no dependency graph 
			// computation will be done as a result of this call.
			// 
			MDataHandle outputHandle = data.outputValue( RiverNetworkNode::outputPoints );
			// This just copies the input value through to the output.  
			// 
			outputHandle.set( result );
			// Mark the destination plug as being clean.  This will prevent the
			// dependency graph from repeating this calculation until an input 
			// of this node changes.
			// 
			data.setClean(plug);
		}
	} else {
		return MS::kUnknownParameter;
	}

	return MS::kSuccess;
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
	// This sample creates a single input float attribute and a single
	// output float attribute.
	//
	MFnNumericAttribute nAttr;
	MStatus				stat;

	inputCurve = nAttr.create( "input", "in", MFnNumericData::kFloat, 0.0 );
	// Attribute will be written to files when this type of node is stored
 	nAttr.setStorable(true);
	// Attribute is keyable and will show up in the channel box
 	nAttr.setKeyable(true);

	outputPoints = nAttr.create( "output", "out", MFnNumericData::kFloat, 0.0 );
	// Attribute is read-only because it is an output attribute
	nAttr.setWritable(false);
	// Attribute will not be written to files when this type of node is stored
	nAttr.setStorable(false);

	// Add the attributes we have created to the node
	//
	stat = addAttribute( inputCurve );
		if (!stat) { stat.perror("addAttribute"); return stat;}
	stat = addAttribute( outputPoints );
		if (!stat) { stat.perror("addAttribute"); return stat;}

	// Set up a dependency between the input and the output.  This will cause
	// the output to be marked dirty when the input changes.  The output will
	// then be recomputed the next time the value of the output is requested.
	//
	stat = attributeAffects( inputCurve, outputPoints );
		if (!stat) { stat.perror("attributeAffects"); return stat;}

	return MS::kSuccess;

}

