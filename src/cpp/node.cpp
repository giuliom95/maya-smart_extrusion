#include <string.h>
#include <maya/MIOStream.h>
#include <math.h>

#include <maya/MPxNode.h> 

#include <maya/MFnTypedAttribute.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnPlugin.h>
#include <maya/MFnData.h>

#include <maya/MFnNurbsCurve.h>

#include <maya/MQuaternion.h>
#include <maya/MEulerRotation.h>
#include <maya/MVector.h>
#include <maya/MPoint.h>
#include <maya/MAngle.h>
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

	// Input of scale and position on the taper curve of the control circles
	static  MObject		aControls;
	static  MObject		aControlsScale;
	static  MObject		aControlsPosition;

	// Out rotation and translation of the control circles
	static	MObject		aRototranslation;
	static	MObject		aControlsTranslation;
	static	MObject		aControlsRotation;
	static	MObject		aControlsRotationX;
	static	MObject		aControlsRotationY;
	static	MObject		aControlsRotationZ;

	// Parameters of the taper curve
	static  MObject		aTaperCurve;
	static  MObject		aTaperCurveValue;
	static  MObject		aTaperCurvePosition;

	static	MTypeId		nodeId;
};

MObject	NodeSmartExtrude::aCurve;

MObject	NodeSmartExtrude::aControls;
MObject	NodeSmartExtrude::aControlsScale;
MObject	NodeSmartExtrude::aControlsPosition;

MObject	NodeSmartExtrude::aRototranslation;
MObject	NodeSmartExtrude::aControlsTranslation;
MObject	NodeSmartExtrude::aControlsRotation;
MObject	NodeSmartExtrude::aControlsRotationX;
MObject	NodeSmartExtrude::aControlsRotationY;
MObject	NodeSmartExtrude::aControlsRotationZ;

MObject	NodeSmartExtrude::aTaperCurve;
MObject	NodeSmartExtrude::aTaperCurveValue;
MObject	NodeSmartExtrude::aTaperCurvePosition;

MTypeId	NodeSmartExtrude::nodeId{0x000fd};

NodeSmartExtrude::NodeSmartExtrude() {}
NodeSmartExtrude::~NodeSmartExtrude() {}

MStatus NodeSmartExtrude::compute(const MPlug& plug, MDataBlock& data) {

	auto controlsHandle = data.inputArrayValue(aControls);

	if(plug == aTaperCurveValue || plug == aTaperCurvePosition) {	
		
		auto taperCurveHandle = data.outputArrayValue(aTaperCurve);

		do {
			// Fetch output handles
			auto taperCurveCCHandle = taperCurveHandle.outputValue();
			auto taperCurveValueHandle = taperCurveCCHandle.child(aTaperCurveValue);
			auto taperCurvePositionHandle = taperCurveCCHandle.child(aTaperCurvePosition);

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

	} else if(plug == aControlsTranslation || 
		plug == aControlsRotationX ||
		plug == aControlsRotationY || 
		plug == aControlsRotationZ) {

		auto rototranslationHandle = data.outputArrayValue(aRototranslation);

		auto curve = data.inputValue(aCurve).asNurbsCurve();
		MFnNurbsCurve curveFn(curve);
		double curvet_s, curvet_e;
		curveFn.getKnotDomain(curvet_s, curvet_e);

		do {
			// Fetch output handles
			auto rototranslationCCHandle = rototranslationHandle.outputValue();
			auto translationHandle = rototranslationCCHandle.child(aControlsTranslation);
			auto rotationHandle = rototranslationCCHandle.child(aControlsRotation);
			auto rotationXHandle = rotationHandle.child(aControlsRotationX);
			auto rotationYHandle = rotationHandle.child(aControlsRotationY);
			auto rotationZHandle = rotationHandle.child(aControlsRotationZ);

			auto controlHandle = controlsHandle.inputValue();
			auto controlPosition = controlHandle.child(aControlsPosition).asFloat();

			auto t = controlPosition*(curvet_e - curvet_s) + curvet_s;
			MPoint translation{};
			curveFn.getPointAtParam(t, translation);

			auto tang = curveFn.tangent(t).normal();
			auto rot = MVector(0,0,1).rotateTo(tang).asEulerRotation();

			translationHandle.set3Float(translation[0], translation[1], translation[2]);
			rotationXHandle.setMAngle(MAngle(rot[0]));
			rotationYHandle.setMAngle(MAngle(rot[1]));
			rotationZHandle.setMAngle(MAngle(rot[2]));

			controlsHandle.next();
		} while(rototranslationHandle.next());

	} else {
		return MS::kUnknownParameter;
	}

	data.setClean(plug);

	return MS::kSuccess;
}

void* NodeSmartExtrude::create() {
	return new NodeSmartExtrude();
}

MStatus NodeSmartExtrude::initialize() {
	MFnTypedAttribute 		tAttr;
	MFnCompoundAttribute	cAttr;
	MFnNumericAttribute		nAttr;
	MFnUnitAttribute		uAttr;
	MStatus					stat;

	aCurve = tAttr.create("inputCurve", "ic", MFnData::kNurbsCurve);
	addAttribute(aCurve);

	/* "Controls" attribute */ {
		aControlsScale = nAttr.createPoint("controlScale", "cs");
		nAttr.setReadable(false);
		addAttribute(aControlsScale);

		aControlsPosition = nAttr.create("controlPosition", "cp", MFnNumericData::kFloat, 1.0);
		nAttr.setReadable(false);
		nAttr.setMin(0.0);
		nAttr.setMax(1.0);
		addAttribute(aControlsPosition);

		aControls = cAttr.create("controls", "cc");
		cAttr.addChild(aControlsScale);
		cAttr.addChild(aControlsPosition);
		cAttr.setArray(true);
		cAttr.setReadable(false);
		addAttribute(aControls);
	}

	/* "Rototranslation" attribute */ {
		aControlsTranslation = nAttr.createPoint("controlTranslation", "ct");
		nAttr.setWritable(false);
		addAttribute(aControlsTranslation);

		aControlsRotationX = uAttr.create("controlRotationX", "crx", MFnUnitAttribute::kAngle);
		aControlsRotationY = uAttr.create("controlRotationY", "cry", MFnUnitAttribute::kAngle);
		aControlsRotationZ = uAttr.create("controlRotationZ", "crz", MFnUnitAttribute::kAngle);

		aControlsRotation = nAttr.create("controlRotation", "cr", aControlsRotationX, aControlsRotationY, aControlsRotationZ);
		nAttr.setWritable(false);
		addAttribute(aControlsRotation);

		aRototranslation = cAttr.create("controlsRototranslation", "crt");
		cAttr.addChild(aControlsTranslation);
		cAttr.addChild(aControlsRotation);
		cAttr.setArray(true);
		cAttr.setWritable(false);
		addAttribute(aRototranslation);
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
	attributeAffects(aControls, aRototranslation);
	attributeAffects(aCurve, aRototranslation);
	
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