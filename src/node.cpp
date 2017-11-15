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
	static  MObject		aControlsScale;
	static  MObject		aControlsPosition;

	static  MObject		aTaperCurve;
	static  MObject		aTaperCurveValue;
	static  MObject		aTaperCurvePosition;

	static	MTypeId		nodeId;
};

MObject	NodeSmartExtrude::aCurve;

MObject	NodeSmartExtrude::aControls;
MObject	NodeSmartExtrude::aControlsScale;
MObject	NodeSmartExtrude::aControlsPosition;

MObject	NodeSmartExtrude::aTaperCurve;
MObject	NodeSmartExtrude::aTaperCurveValue;
MObject	NodeSmartExtrude::aTaperCurvePosition;

MTypeId	NodeSmartExtrude::nodeId{0x000fd};

NodeSmartExtrude::NodeSmartExtrude() {}
NodeSmartExtrude::~NodeSmartExtrude() {}

MStatus NodeSmartExtrude::compute(const MPlug& plug, MDataBlock& data) {
	
	MStatus stat;
 
	if(plug == aTaperCurveValue || plug == aTaperCurvePosition) {	
		
		auto controlsHandle = data.inputArrayValue(aControls);

		auto taperCurveHandle = data.outputArrayValue(aTaperCurve);

		do {

			// Fetch output handles
			auto taperCurveCVHandle = taperCurveHandle.outputValue();
			auto taperCurveValueHandle = taperCurveCVHandle.child(aTaperCurveValue);
			auto taperCurvePositionHandle = taperCurveCVHandle.child(aTaperCurvePosition);

			auto controlHandle = controlsHandle.inputValue();
			auto controlScale = controlHandle.child(aControlsScale).asFloat3();
			auto controlPosition = controlHandle.child(aControlsPosition).asFloat();

			double outValue = controlScale[0];
			outValue = outValue < controlScale[1] ? controlScale[1] : outValue;
			outValue = outValue < controlScale[2] ? controlScale[2] : outValue;

			taperCurveValueHandle.setFloat(outValue);
			taperCurvePositionHandle.setFloat(controlPosition);
			
			controlsHandle.next();
		} while(taperCurveHandle.next());

		data.setClean(plug);
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

	/* "Controls" attribute */ {
		aControlsScale = nAttr.createPoint("controlScale", "cs");
		nAttr.setReadable(false);
		addAttribute(aControlsScale);

		aControlsPosition = nAttr.create("controlPosition", "cp", MFnNumericData::kFloat, 1.0);
		nAttr.setReadable(false);
		addAttribute(aControlsPosition);

		aControls = cAttr.create("controls", "cc");
		cAttr.addChild(aControlsScale);
		cAttr.addChild(aControlsPosition);
		cAttr.setArray(true);
		cAttr.setReadable(false);
		addAttribute(aControls);
	}

	/* "Taper Curve" attribute */ {
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
	}

	attributeAffects(aControls, aTaperCurve);
	
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