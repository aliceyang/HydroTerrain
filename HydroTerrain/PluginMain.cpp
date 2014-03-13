#include <maya/MPxCommand.h>
#include <maya/MFnPlugin.h>
#include <maya/MIOStream.h>
#include <maya/MString.h>
#include <maya/MArgList.h>
#include <maya/MGlobal.h>
#include <maya/MSimple.h>
#include <maya/MDoubleArray.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MFnNurbsCurve.h>
#include <maya/MDGModifier.h>
#include <maya/MPlugArray.h>
#include <maya/MVector.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MStringArray.h>
#include <list>

#include "LSystemNode.h"

MStatus initializePlugin( MObject obj )
{
    MStatus   status = MStatus::kSuccess;
    MFnPlugin plugin( obj, "MyPlugin", "1.0", "Any");

	// Set name
	plugin.setName("LSystem");

	// Load UI from MEL file
	MGlobal::executeCommand("source \"" + plugin.loadPath() + "/ui.mel\"");
	status = plugin.registerUI("createLSystemUI", "deleteLSystemUI");

	// Register Node
	status = plugin.registerNode("LSystemNode", LSystemNode::id, LSystemNode::creator, LSystemNode::initialize);
	CHECK_MSTATUS_AND_RETURN_IT(status);

    return status;
}

MStatus uninitializePlugin( MObject obj )
{
    MStatus   status = MStatus::kSuccess;
    MFnPlugin plugin( obj );

	// Deregister Node
	status = plugin.deregisterNode(LSystemNode::id);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	return status;
}


