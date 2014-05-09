//	CIS 660  | HydroTerrain Authoring Tool
//
//	Author: Alice Yang
//
//	River Network Node: This node is responsible for taking in a user-created CV curve as the 
//	river contour curve, and outputs an array of locations for the newly expanded set of 
//	river nodes
//

#include "RiverNetworkNode.h"
#include <maya/MGlobal.h>
#include <maya/MFnPlugin.h>

MStatus initializePlugin( MObject obj )
//
//	Description:
//		this method is called when the plug-in is loaded into Maya.  It 
//		registers all of the services that this plug-in provides with 
//		Maya.
//
//	Arguments:
//		obj - a handle to the plug-in object (use MFnPlugin to access it)
//
{ 
	MStatus   status;
	MFnPlugin plugin( obj, "", "2012", "Any");

	// Set name
	plugin.setName("RiverNetworkNode");

	// Load UI from MEL file
	std::cout << "source \"" + plugin.loadPath() + "/ui.mel\"" << std::endl;
	MGlobal::executeCommand("source \"" + plugin.loadPath() + "/ui.mel\"");
	status = plugin.registerUI("createHydroTerrainUI", "deleteHydroTerrainUI");

	// Register Node
	status = plugin.registerNode( "RiverNetworkNode", RiverNetworkNode::id, RiverNetworkNode::creator,
								  RiverNetworkNode::initialize );
	if (!status) {
		status.perror("registerNode");
		return status;
	}

	return status;
}

MStatus uninitializePlugin( MObject obj)
//
//	Description:
//		this method is called when the plug-in is unloaded from Maya. It 
//		deregisters all of the services that it was providing.
//
//	Arguments:
//		obj - a handle to the plug-in object (use MFnPlugin to access it)
//
{
	MStatus   status;
	MFnPlugin plugin( obj );

	status = plugin.deregisterNode( RiverNetworkNode::id );
	if (!status) {
		status.perror("deregisterNode");
		return status;
	}

	return status;
}
