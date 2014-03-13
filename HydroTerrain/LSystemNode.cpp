#define MNoVersionString
#define MNoPluginEntry

#include "LSystemNode.h"
#include "cylinder.h"
#include "LSystem.h"

#include <maya/MFnPlugin.h>
#include <maya/MTime.h>
#include <maya/MFnMesh.h>
#include <maya/MPoint.h>
#include <maya/MFloatPoint.h>
#include <maya/MFloatPointArray.h>
#include <maya/MIntArray.h>
#include <maya/MDoubleArray.h>

#include <maya/MFnUnitAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnNumericAttribute.h>

#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MFnMeshData.h>
#include <maya/MIOStream.h>

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
MTypeId LSystemNode::id (0x0); 

//The attributes of the node are initialized to NULL values. 
MObject LSystemNode::time;
MObject LSystemNode::angle;
MObject LSystemNode::stepSize;
MObject LSystemNode::grammarFile;
MObject LSystemNode::outputMesh;

// The creator() method simply returns new instances of this node. The return type is a void* so
// Maya can create node instances internally in a general fashion without having to know the return type.
void* LSystemNode::creator()
{
	return new LSystemNode;
}

// The initialize method is called only once when the node is first registered with Maya. In this method 
// you define the attributes of the node, what data comes in and goes out of the node that other nodes may want to connect to. 
MStatus LSystemNode::initialize()
{
	MFnNumericAttribute numAttr;
	MFnTypedAttribute typedAttr;
	MFnUnitAttribute unitAttr;

	MStatus returnStatus;

	// Create attributes
	LSystemNode::time = unitAttr.create( "time", "tm", MFnUnitAttribute::kTime, 0.0, &returnStatus );
	McheckErr(returnStatus, "ERROR creating LSystemNode time attribute\n");
	MAKE_INPUT(unitAttr);

	LSystemNode::angle = numAttr.create( "angle", "a", MFnNumericData::kDouble, 0.0, &returnStatus );
	McheckErr(returnStatus, "ERROR creating LSystemNode angle attribute\n");
	MAKE_INPUT(numAttr);

	LSystemNode::stepSize = numAttr.create( "stepSize", "ss", MFnNumericData::kDouble, 0.0, &returnStatus );
	McheckErr(returnStatus, "ERROR creating LSystemNode stepSize attribute\n");
	MAKE_INPUT(numAttr);

	LSystemNode::grammarFile = typedAttr.create( "grammarFile", "g", MFnData::kString, &returnStatus );
	McheckErr(returnStatus, "ERROR creating LSystemNode grammarFile attribute\n");
	MAKE_INPUT(typedAttr);

	LSystemNode::outputMesh = typedAttr.create( "outputMesh", "out", MFnData::kMesh, &returnStatus );
	McheckErr(returnStatus, "ERROR creating LSystemNode outputMesh attribute\n");
	MAKE_OUTPUT(typedAttr);

	// Add attributes
	returnStatus = addAttribute(LSystemNode::time);
	McheckErr(returnStatus, "ERROR adding time attribute\n");

	returnStatus = addAttribute(LSystemNode::angle);
	McheckErr(returnStatus, "ERROR adding angle attribute\n");

	returnStatus = addAttribute(LSystemNode::stepSize);
	McheckErr(returnStatus, "ERROR adding stepSize attribute\n");

	returnStatus = addAttribute(LSystemNode::grammarFile);
	McheckErr(returnStatus, "ERROR adding grammarFile attribute\n");

	returnStatus = addAttribute(LSystemNode::outputMesh);
	McheckErr(returnStatus, "ERROR adding outputMesh attribute\n");

	// Connect inputs to outputs via attributeAffects
	returnStatus = attributeAffects(LSystemNode::time, LSystemNode::outputMesh);
	McheckErr(returnStatus, "ERROR in attributeAffects\n");
	
	returnStatus = attributeAffects(LSystemNode::angle, LSystemNode::outputMesh);
	McheckErr(returnStatus, "ERROR in attributeAffects\n");

	returnStatus = attributeAffects(LSystemNode::stepSize, LSystemNode::outputMesh);
	McheckErr(returnStatus, "ERROR in attributeAffects\n");

	returnStatus = attributeAffects(LSystemNode::grammarFile, LSystemNode::outputMesh);
	McheckErr(returnStatus, "ERROR in attributeAffects\n");

	return MS::kSuccess;
}

// Called whenever an attribute, such as time, changes, and recomputes the output mesh
MStatus LSystemNode::compute(const MPlug& plug, MDataBlock& data)
{
	MStatus returnStatus;

	if (plug == outputMesh)
	{
		// Get time
		MDataHandle timeData = data.inputValue(time, &returnStatus);
		McheckErr(returnStatus, "ERROR getting time data handle\n");
		MTime timeValue = timeData.asTime();

		// Get angle
		MDataHandle angleData = data.inputValue(angle, &returnStatus);
		McheckErr(returnStatus, "ERROR getting angle data handle\n");
		double angleValue = angleData.asDouble();

		// Get step size
		MDataHandle stepSizeData = data.inputValue(stepSize, &returnStatus);
		McheckErr(returnStatus, "ERROR getting angle data handle\n");
		double stepSizeValue = stepSizeData.asDouble();

		// Get grammar file
		MDataHandle grammarFileData = data.inputValue(grammarFile, &returnStatus);
		McheckErr(returnStatus, "ERROR getting grammar file data handle\n");
		MString grammarValue = grammarFileData.asString();

		// Get output object
		MDataHandle outputHandle = data.outputValue(outputMesh, &returnStatus);
		McheckErr(returnStatus, "ERROR getting polygon data handle\n");

		MFnMeshData dataCreator;
		MObject newOutputData = dataCreator.create(&returnStatus);
		McheckErr(returnStatus, "ERROR creating outputData");

		createMesh(timeValue, angleValue, stepSizeValue, grammarValue, newOutputData, returnStatus);
		McheckErr(returnStatus, "ERROR creating new mesh");

		outputHandle.set(newOutputData);
		data.setClean(plug);
	}
	else
	{
		return MS::kUnknownParameter;
	}

	return MS::kSuccess;
}

MObject LSystemNode::createMesh(const MTime& time, const double& angle, const double& stepSize, const MString& grammar, MObject& outData, MStatus& stat)
{
	int frame = (int)time.as(MTime::kFilm);
	MFnMesh meshFS;

	MObject pluginObj = MFnPlugin::findPlugin("LSystem");
	MFnPlugin plugin(pluginObj);
	MString filePath = plugin.loadPath();
	MString grammarPath = filePath + "/" + grammar;

	MIntArray faceCounts;
	MIntArray faceConnects;
	MPointArray points;

	cout << "TODO: GENERATE OUTPUT MESH" << endl;

	// Create the output mesh
	MObject newMesh = meshFS.create(points.length(), faceCounts.length(),
									points, faceCounts, faceConnects,
									outData, &stat);

	return newMesh;
}
