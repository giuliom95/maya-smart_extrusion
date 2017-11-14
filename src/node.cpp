#include <string.h>
#include <maya/MIOStream.h>
#include <math.h>

#include <maya/MPxNode.h> 

#include <maya/MFnTypedAttribute.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnNumericAttribute.h>
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
	static  MObject		aControls;
	static  MObject		aTaperCurve;

	static  MObject		aTaperCurveValue;
	static  MObject		aTaperCurvePosition;

	static	MTypeId		nodeId;
};

MObject	NodeSmartExtrude::aCurve;
MObject	NodeSmartExtrude::aControls;
MObject	NodeSmartExtrude::aTaperCurve;

MObject	NodeSmartExtrude::aTaperCurveValue;
MObject	NodeSmartExtrude::aTaperCurvePosition;

MTypeId	NodeSmartExtrude::nodeId( 0x000fd );

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
	MFnTypedAttribute 		tAttr;
	MFnCompoundAttribute	cAttr;
	MFnNumericAttribute		nAttr;
	MStatus					stat;

	aCurve = tAttr.create("inputCurve", "ic", MFnData::kNurbsCurve);
	addAttribute(aCurve);

	aControls = tAttr.create("controls", "cc", MFnData::kMatrix);
	tAttr.setArray(true);
	tAttr.setReadable(false);
	addAttribute(aControls);
	
	aTaperCurveValue = nAttr.create("value", "v", MFnNumericData::kFloat, 1.0);
	nAttr.setWritable(false);
	addAttribute(aTaperCurveValue);

	aTaperCurvePosition = nAttr.create("position", "p", MFnNumericData::kFloat, 0.0);
	nAttr.setWritable(false);
	addAttribute(aTaperCurvePosition);
	
	aTaperCurve = cAttr.create("taperCurve", "tc");
	cAttr.addChild(aTaperCurveValue);
	cAttr.addChild(aTaperCurvePosition);
	cAttr.setArray(true);
	cAttr.setWritable(false);
	addAttribute(aTaperCurve);
	
	
	return MS::kSuccess;
}








/////////////////////////////////
///// PLUGIN INITIALIZATION /////
/////////////////////////////////

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