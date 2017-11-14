#include <string.h>
#include <maya/MIOStream.h>
#include <math.h>

#include <maya/MPxNode.h> 

#include <maya/MFnTypedAttribute.h>
#include <maya/MFnPlugin.h>
#include <maya/MFnData.h>

#include <maya/MString.h> 
#include <maya/MTypeId.h> 
#include <maya/MPlug.h>
#include <maya/MVector.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
 
class NodeSmartExtrude : public MPxNode {
public:
						NodeSmartExtrude();
	virtual				~NodeSmartExtrude(); 

	virtual MStatus		compute( const MPlug& plug, MDataBlock& data );

	static  void*		create();
	static  MStatus		initialize();

public:
	static  MObject		aCurve;
	static  MObject		aTaperCurve;
	static	MTypeId		nodeId;
};

MTypeId	NodeSmartExtrude::nodeId( 0x000fd );
MObject	NodeSmartExtrude::aCurve;
MObject	NodeSmartExtrude::aTaperCurve;

NodeSmartExtrude::NodeSmartExtrude() {}
NodeSmartExtrude::~NodeSmartExtrude() {}

MStatus NodeSmartExtrude::compute( const MPlug& plug, MDataBlock& data ) {
	
	MStatus stat;
 
	if(plug == NodeSmartExtrude::aTaperCurve) {	
		
	} else {
		return MS::kUnknownParameter;
	}

	return MS::kSuccess;
}

void* NodeSmartExtrude::create() {
	return new NodeSmartExtrude();
}

MStatus NodeSmartExtrude::initialize() {
	MFnTypedAttribute 	tAttr;
	MStatus				stat;

	NodeSmartExtrude::aCurve = tAttr.create("inputCurve", "ic", MFnData::kNurbsCurve);
	
	stat = addAttribute(NodeSmartExtrude::aCurve);
		if (!stat) { stat.perror("addAttribute"); return stat;}

	return MS::kSuccess;
}

MStatus initializePlugin(MObject obj) {
	MStatus   status;
	MFnPlugin plugin(obj, "Giulio Martella", "0.1", "Any");

	status = plugin.registerNode(
		"smartExtrude", 
		NodeSmartExtrude::nodeId, 
		NodeSmartExtrude::create,
		NodeSmartExtrude::initialize);

	if(!status) {
		status.perror("registerNode");
		return status;
	}

	return status;
}

MStatus uninitializePlugin(MObject obj)
{
	MStatus   status;
	MFnPlugin plugin( obj );

	status = plugin.deregisterNode( NodeSmartExtrude::nodeId );
	if (!status) {
		status.perror("deregisterNode");
		return status;
	}

	return status;
}