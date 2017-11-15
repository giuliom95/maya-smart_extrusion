import pymel.core as pmc

nCircles = 3
precision = 4

cT = pmc.ls(sl=True)[0]
cS = cT.getShape()

ct_s, ct_e = cS.getKnotDomain()

cSp = cS.getPointAtParam
cSt = cS.tangent

plT = pmc.polyPlane(sx=1, sy=1, ax=cSt(ct_s))[0]
plT.setTranslation(cSp(ct_s))
plS = plT.getShape()

extrN = pmc.polyExtrudeFacet(plS.f[0], inc=cT, d=nCircles*precision)[0]
extrN = pmc.ls(extrN)[0]

sextrN = pmc.createNode('smartExtrude')

for i in range(nCircles):
    ct_normalized = i/(nCircles-1.)
    ct = ct_s + ct_normalized*(ct_e-ct_s)
    ccT = pmc.circle()[0]
    tang = cSt(ct).normal()
    ccT.rotateBy(pmc.dt.Vector([0,0,1]).rotateTo(tang))
    ccT.setTranslation(cSp(ct))
    
    pmc.connectAttr(ccT.scale, sextrN.controls[i].controlScale)
    pmc.setAttr(sextrN.controls[i].controlPosition, ct_normalized)
    
    pmc.connectAttr(sextrN.taperCurve[i].position, extrN.taperCurve[i].taperCurve_Position)
    pmc.connectAttr(sextrN.taperCurve[i].value, extrN.taperCurve[i].taperCurve_FloatValue)
    pmc.setAttr(extrN.taperCurve[i].taperCurve_Interp, 2)