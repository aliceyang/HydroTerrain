#ifndef LSystemNode_H_
#define LSystemNode_H_

#include <maya/MPxNode.h>

class LSystemNode : public MPxNode
{
public:
	LSystemNode() {};
	virtual ~LSystemNode() {};
	virtual MStatus compute(const MPlug& plug, MDataBlock& data);
	static  void*	creator();
	static  MStatus initialize();
	
public:
	static MTypeId id;
	static MObject time;
	static MObject angle;
	static MObject stepSize;
	static MObject grammarFile;
	static MObject outputMesh;

protected:
	MObject createMesh(const MTime& time, const double& angle, const double& stepSize, const MString& grammar, MObject& outData, MStatus& stat);
};


#endif