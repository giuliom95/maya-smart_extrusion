import pymel.core as pmc

def smartExtrude(nCircles = 3, divisions = -1):

    if divisions < 0:
        divisions = nCircles * 5
    
    face, cT = pmc.ls(sl=True)
    cS = cT.getShape()
    
    ct_s, ct_e = cS.getKnotDomain()
    
    cSp = cS.getPointAtParam
    cSt = cS.tangent
    
    extrN = pmc.polyExtrudeFacet(face, inc=cT, d=divisions)[0]
    extrN = pmc.ls(extrN)[0]
    
    sextrN = pmc.createNode('smartExtrude')
    pmc.connectAttr(cS.local, sextrN.ic)
    
    for i in range(nCircles):
        ct_normalized = i/(nCircles-1.)
        ct = ct_s + ct_normalized*(ct_e-ct_s)
        ccT = pmc.circle()[0]
    
        
        pmc.connectAttr(sextrN.crt[i].ct, ccT.t)
        pmc.connectAttr(sextrN.crt[i].cr, ccT.r)
        
        pmc.connectAttr(ccT.scale, sextrN.controls[i].controlScale)
        pmc.setAttr(sextrN.controls[i].controlPosition, ct_normalized)
        
        pmc.connectAttr(sextrN.taperCurve[i].position, extrN.taperCurve[i].taperCurve_Position)
        pmc.connectAttr(sextrN.taperCurve[i].value, extrN.taperCurve[i].taperCurve_FloatValue)
        pmc.setAttr(extrN.taperCurve[i].taperCurve_Interp, 2)