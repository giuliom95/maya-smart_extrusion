import maya.api.OpenMaya as OpenMaya


def maya_useNewAPI():
    pass


class Node(OpenMaya.MPxNode):
    nodeName = 'smartExtrude'
    nodeClass = 'general'
    nodeId = OpenMaya.MTypeId(0x000fd)

    aCurve = OpenMaya.MObject()

    aControls = OpenMaya.MObject()
    aControlsScale = OpenMaya.MObject()
    aControlsPosition = OpenMaya.MObject()

    aRototranslation = OpenMaya.MObject()
    aControlsTranslation = OpenMaya.MObject()
    aControlsRotation = OpenMaya.MObject()
    aControlsRotationX = OpenMaya.MObject()
    aControlsRotationY = OpenMaya.MObject()
    aControlsRotationZ = OpenMaya.MObject()

    aTaperCurve = OpenMaya.MObject()
    aTaperCurveValue = OpenMaya.MObject()
    aTaperCurvePosition = OpenMaya.MObject()

    def __init__(self):
        OpenMaya.MPxNode.__init__(self)

    @staticmethod
    def create():
        return Node()

    @staticmethod
    def initialize():
        tAttr = OpenMaya.MFnTypedAttribute()
        cAttr = OpenMaya.MFnCompoundAttribute()
        nAttr = OpenMaya.MFnNumericAttribute()
        uAttr = OpenMaya.MFnUnitAttribute()

        Node.aCurve = tAttr.create(
            'inputCurve', 'ic', OpenMaya.MFnNurbsCurveData.kNurbsCurve)
        Node.addAttribute(Node.aCurve)

        # 'Controls' attribute
        Node.aControlsScale = nAttr.createPoint('controlScale', 'cs')
        nAttr.readable = False
        Node.addAttribute(Node.aControlsScale)

        Node.aControlsPosition = nAttr.create(
            'controlPosition', 'cp', OpenMaya.MFnNumericData.kFloat, 1.0)
        nAttr.setMin(0.0)
        nAttr.setMax(1.0)
        nAttr.readable = False
        Node.addAttribute(Node.aControlsPosition)

        Node.aControls = cAttr.create('controls', 'cc')
        cAttr.addChild(Node.aControlsScale)
        cAttr.addChild(Node.aControlsPosition)
        cAttr.array = True
        cAttr.readable = False
        Node.addAttribute(Node.aControls)

        # 'Rototranslation' attribute
        Node.aControlsTranslation = nAttr.createPoint('controlTranslation', 'ct')
        nAttr.writable = False
        Node.addAttribute(Node.aControlsTranslation)

        Node.aControlsRotationX = uAttr.create(
            'controlRotationX', 'crx', OpenMaya.MFnUnitAttribute.kAngle)
        Node.aControlsRotationY = uAttr.create(
            'controlRotationY', 'cry', OpenMaya.MFnUnitAttribute.kAngle)
        Node.aControlsRotationZ = uAttr.create(
            'controlRotationZ', 'crz', OpenMaya.MFnUnitAttribute.kAngle)

        Node.aControlsRotation = nAttr.create(
            'controlRotation', 'cr',
            Node.aControlsRotationX,
            Node.aControlsRotationY,
            Node.aControlsRotationZ)
        nAttr.writable = False
        Node.addAttribute(Node.aControlsRotation)

        Node.aRototranslation = cAttr.create('controlsRototranslation', 'crt')
        cAttr.addChild(Node.aControlsTranslation)
        cAttr.addChild(Node.aControlsRotation)
        cAttr.array = True
        cAttr.writable = False
        Node.addAttribute(Node.aRototranslation)

        # 'Taper Curve' attribute
        Node.aTaperCurveValue = nAttr.create(
            'value', 'v', OpenMaya.MFnNumericData.kFloat, 1.0)
        nAttr.writable = False
        Node.addAttribute(Node.aTaperCurveValue)

        Node.aTaperCurvePosition = nAttr.create(
            'position', 'p', OpenMaya.MFnNumericData.kFloat, 0.0)
        nAttr.writable = False
        Node.addAttribute(Node.aTaperCurvePosition)

        Node.aTaperCurve = cAttr.create('taperCurve', 'tc')
        cAttr.addChild(Node.aTaperCurveValue)
        cAttr.addChild(Node.aTaperCurvePosition)
        cAttr.array = True
        cAttr.writable = False
        Node.addAttribute(Node.aTaperCurve)

        Node.attributeAffects(Node.aControls, Node.aTaperCurve)
        Node.attributeAffects(Node.aControls, Node.aRototranslation)
        Node.attributeAffects(Node.aCurve, Node.aRototranslation)

    def compute(self, plug, data):
        controlsHandle = data.inputArrayValue(Node.aControls)

        if plug == Node.aTaperCurveValue or plug == Node.aTaperCurvePosition:

            taperCurveHandle = data.outputArrayValue(Node.aTaperCurve)

            while True:
                taperCurveCCHandle = taperCurveHandle.outputValue()
                taperCurveValueHandle = taperCurveCCHandle.child(Node.aTaperCurveValue)
                taperCurvePositionHandle = taperCurveCCHandle.child(Node.aTaperCurvePosition)

                controlHandle = controlsHandle.inputValue()
                controlScale = controlHandle.child(Node.aControlsScale).asFloat3()
                controlPosition = controlHandle.child(Node.aControlsPosition).asFloat()

                outValue = max(controlScale)

                taperCurveValueHandle.setFloat(outValue)
                taperCurvePositionHandle.setFloat(controlPosition)

                controlsHandle.next()
                if not taperCurveHandle.next():
                    break

        elif plug == Node.aControlsTranslation or \
                plug == Node.aControlsRotationX or \
                plug == Node.aControlsRotationY or \
                plug == Node.aControlsRotationZ:

            rototranslationHandle = data.outputArrayValue(Node.aRototranslation)

            curve = data.inputValue(Node.aCurve).asNurbsCurve()
            curveFn = OpenMaya.MFnNurbsCurve(curve)
            curvet_s, curvet_e = curveFn.knotDomain

            while True:
                rototranslationCCHandle = rototranslationHandle.outputValue()
                translationHandle = rototranslationCCHandle.child(Node.aControlsTranslation)
                rotationHandle = rototranslationCCHandle.child(Node.aControlsRotation)
                rotationXHandle = rotationHandle.child(Node.aControlsRotationX)
                rotationYHandle = rotationHandle.child(Node.aControlsRotationY)
                rotationZHandle = rotationHandle.child(Node.aControlsRotationZ)

                controlHandle = controlsHandle.inputValue()
                controlPosition = controlHandle.child(Node.aControlsPosition).asFloat()

                t = controlPosition*(curvet_e - curvet_s) + curvet_s
                translation = curveFn.getPointAtParam(t)

                tang = curveFn.tangent(t).normal()
                rot = OpenMaya.MVector(0,0,1).rotateTo(tang).asEulerRotation()

                translationHandle.set3Float(translation[0], translation[1], translation[2])
                rotationXHandle.setMAngle(OpenMaya.MAngle(rot[0]))
                rotationYHandle.setMAngle(OpenMaya.MAngle(rot[1]))
                rotationZHandle.setMAngle(OpenMaya.MAngle(rot[2]))

                controlsHandle.next()
                if not rototranslationHandle.next():
                    break



##########################################################
# Plug-in initialization.
##########################################################
def initializePlugin(mobject):

    mplugin = OpenMaya.MFnPlugin(mobject)
    try:
        mplugin.registerNode(
            Node.nodeName,
            Node.nodeId,
            Node.create,
            Node.initialize,
            OpenMaya.MPxNode.kDependNode,
            Node.nodeClass)
    except:
        sys.stderr.write(
            'Failed to register node: ' + Node.nodeName)
        raise


def uninitializePlugin(mobject):
    ''' Uninitializes the plug-in '''
    mplugin = OpenMaya.MFnPlugin(mobject)
    try:
        mplugin.deregisterNode(Node.nodeId)
    except:
        sys.stderr.write(
            'Failed to deregister node: ' + Node.nodeName)
        raise
