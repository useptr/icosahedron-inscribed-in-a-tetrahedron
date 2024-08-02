// (C) Copyright 2002-2007 by Autodesk, Inc. 
//
// Permission to use, copy, modify, and distribute this software in
// object code form for any purpose and without fee is hereby granted, 
// provided that the above copyright notice appears in all copies and 
// that both that copyright notice and the limited warranty and
// restricted rights notice below appear in all supporting 
// documentation.
//
// AUTODESK PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS. 
// AUTODESK SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF
// MERCHANTABILITY OR FITNESS FOR A PARTICULAR USE.  AUTODESK, INC. 
// DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
// UNINTERRUPTED OR ERROR FREE.
//
// Use, duplication, or disclosure by the U.S. Government is subject to 
// restrictions set forth in FAR 52.227-19 (Commercial Computer
// Software - Restricted Rights) and DFAR 252.227-7013(c)(1)(ii)
// (Rights in Technical Data and Computer Software), as applicable.
//

//-----------------------------------------------------------------------------
//----- ADSKTetrahedronWithInscribedIcosahedron.cpp : Implementation of ADSKTetrahedronWithInscribedIcosahedron
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ADSKTetrahedronWithInscribedIcosahedron.h"
#include "Tchar.h"
#include "utilities.h" 
#include <cmath>
#include <numbers>
#include <ranges>
#include <algorithm>
#include <random>
//-----------------------------------------------------------------------------
Adesk::UInt32 ADSKTetrahedronWithInscribedIcosahedron::kCurrentVersionNumber =1 ;

//-----------------------------------------------------------------------------
ACRX_DXF_DEFINE_MEMBERS (
	ADSKTetrahedronWithInscribedIcosahedron, AcDbEntity,
	AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent, 
	AcDbProxyEntity::kNoOperation, ADSKTETRAHEDRONWITHINSCRIBEDICOSAHEDRON,
ADSKTETRAHEDRONAPP
|Product Desc:     A description for your object
|Company:          Your company name
|WEB Address:      Your company WEB site address
)

//-----------------------------------------------------------------------------
ADSKTetrahedronWithInscribedIcosahedron::ADSKTetrahedronWithInscribedIcosahedron () : AcDbEntity (), m_ptMoveGripPoint(AcGePoint3d::kOrigin), m_Tetrahedron(), m_Icosahedron(getInscribedIcosahedronEdgeLength(1.0/* m_Tetrahedron.edgeLenght()*/)), m_bNeedTranfomTransformMatrix(true) {

}

//ADSKTetrahedronWithInscribedIcosahedron::ADSKTetrahedronWithInscribedIcosahedron(const ADSKTetrahedronWithInscribedIcosahedron& other)
//{
//	m_Tetrahedron = other.m_Tetrahedron;
//	m_Icosahedron = other.m_Icosahedron;
//}

ADSKTetrahedronWithInscribedIcosahedron::~ADSKTetrahedronWithInscribedIcosahedron () {
}

//-----------------------------------------------------------------------------
//----- AcDbObject protocols
//- Dwg Filing protocol
Acad::ErrorStatus ADSKTetrahedronWithInscribedIcosahedron::dwgOutFields (AcDbDwgFiler *pFiler) const {
	assertReadEnabled () ;
	//----- Save parent class information first.
	Acad::ErrorStatus es =AcDbEntity::dwgOutFields (pFiler) ;
	if ( es != Acad::eOk )
		return (es) ;
	//----- Object version number needs to be saved first
	if ( (es =pFiler->writeUInt32 (ADSKTetrahedronWithInscribedIcosahedron::kCurrentVersionNumber)) != Acad::eOk )
		return (es) ;
	//----- Output params
	auto transformMatrix = m_translation * m_scaling;
	//m_transform = m_translation * m_scaling; // m_rotation *
	for (int r : std::views::iota(0, 4)) {
		for (int c : std::views::iota(0, 4)) {
			pFiler->writeItem(transformMatrix(r,c));
		}
	}
	
	
	/*double dTetrahedronEdgeLenght{ 0 };
	m_Tetrahedron.edgeLength(dTetrahedronEdgeLenght);
	pFiler->writeItem(dTetrahedronEdgeLenght);*/

	return (pFiler->filerStatus ()) ;
}

Acad::ErrorStatus ADSKTetrahedronWithInscribedIcosahedron::dwgInFields (AcDbDwgFiler *pFiler) {
	assertWriteEnabled () ;
	//----- Read parent class information first.
	Acad::ErrorStatus es =AcDbEntity::dwgInFields (pFiler) ;
	if ( es != Acad::eOk )
		return (es) ;
	//----- Object version number needs to be read first
	Adesk::UInt32 version =0 ;
	if ( (es =pFiler->readUInt32 (&version)) != Acad::eOk )
		return (es) ;
	if ( version > ADSKTetrahedronWithInscribedIcosahedron::kCurrentVersionNumber )
		return (Acad::eMakeMeProxy) ;
	//- Uncomment the 2 following lines if your current object implementation cannot
	//- support previous version of that object.
	//if ( version < ADSKTetrahedronWithInscribedIcosahedron::kCurrentVersionNumber )
	//	return (Acad::eMakeMeProxy) ;
	//----- Read params
	AcGeMatrix3d transformMatrix;
	for (int r : std::views::iota(0, 4)) {
		for (int c : std::views::iota(0, 4)) {
			pFiler->readItem(&transformMatrix(r, c));
		}
	}
	m_bNeedTranfomTransformMatrix = false;
	subTransformBy(transformMatrix);
	//myTransformBy(m_transform);
	

	return (pFiler->filerStatus ()) ;
}

//- Dxf Filing protocol
Acad::ErrorStatus ADSKTetrahedronWithInscribedIcosahedron::dxfOutFields (AcDbDxfFiler *pFiler) const {
	assertReadEnabled () ;
	//----- Save parent class information first.
	Acad::ErrorStatus es =AcDbEntity::dxfOutFields (pFiler) ;
	if ( es != Acad::eOk )
		return (es) ;
	es =pFiler->writeItem (AcDb::kDxfSubclass, _RXST("ADSKTetrahedronWithInscribedIcosahedron")) ;
	if ( es != Acad::eOk )
		return (es) ;
	//----- Object version number needs to be saved first
	if ( (es =pFiler->writeUInt32 (kDxfInt32, ADSKTetrahedronWithInscribedIcosahedron::kCurrentVersionNumber)) != Acad::eOk )
		return (es) ;
	//----- Output params
	//.....

	return (pFiler->filerStatus ()) ;
}

Acad::ErrorStatus ADSKTetrahedronWithInscribedIcosahedron::dxfInFields (AcDbDxfFiler *pFiler) {
	assertWriteEnabled () ;
	//----- Read parent class information first.
	Acad::ErrorStatus es =AcDbEntity::dxfInFields (pFiler) ;
	if ( es != Acad::eOk || !pFiler->atSubclassData (_RXST("ADSKTetrahedronWithInscribedIcosahedron")) )
		return (pFiler->filerStatus ()) ;
	//----- Object version number needs to be read first
	struct resbuf rb ;
	pFiler->readItem (&rb) ;
	if ( rb.restype != AcDb::kDxfInt32 ) {
		pFiler->pushBackItem () ;
		pFiler->setError (Acad::eInvalidDxfCode, _RXST("\nError: expected group code %d (version #)"), AcDb::kDxfInt32) ;
		return (pFiler->filerStatus ()) ;
	}
	Adesk::UInt32 version =(Adesk::UInt32)rb.resval.rlong ;
	if ( version > ADSKTetrahedronWithInscribedIcosahedron::kCurrentVersionNumber )
		return (Acad::eMakeMeProxy) ;
	//- Uncomment the 2 following lines if your current object implementation cannot
	//- support previous version of that object.
	//if ( version < ADSKTetrahedronWithInscribedIcosahedron::kCurrentVersionNumber )
	//	return (Acad::eMakeMeProxy) ;
	//----- Read params in non order dependant manner
	
	//while ( es == Acad::eOk && (es =pFiler->readResBuf (&rb)) == Acad::eOk ) {
	//	switch ( rb.restype ) {

	//		default:
	//			//----- An unrecognized group. Push it back so that the subclass can read it again.
	//			pFiler->pushBackItem () ;
	//			es =Acad::eEndOfFile ;
	//			break ;
	//	}
	//}
	
	//----- At this point the es variable must contain eEndOfFile
	//----- - either from readResBuf() or from pushback. If not,
	//----- it indicates that an error happened and we should
	//----- return immediately.
	if ( es != Acad::eEndOfFile )
		return (Acad::eInvalidResBuf) ;

	return (pFiler->filerStatus ()) ;
}

//-----------------------------------------------------------------------------
//----- AcDbEntity protocols
Adesk::Boolean ADSKTetrahedronWithInscribedIcosahedron::subWorldDraw (AcGiWorldDraw *mode) {
	assertReadEnabled () ;
	//acutPrintf(_T("ADSKTetrahedronWithInscribedIcosahedron::subWorldDraw\n"));
	AcCmTransparency transparency(0.5);
	mode->subEntityTraits().setTransparency(transparency);
	m_Tetrahedron.subWorldDraw(mode);

	transparency.setAlphaPercent(1.0);
	mode->subEntityTraits().setTransparency(transparency);
	m_Icosahedron.subWorldDraw(mode);
	//return (AcDbEntity::subWorldDraw (mode)) ;
	return Adesk::kTrue;
}


Adesk::UInt32 ADSKTetrahedronWithInscribedIcosahedron::subSetAttributes (AcGiDrawableTraits *traits) {
	assertReadEnabled () ;
	//acutPrintf(_T("::subSetAttributes CALLED\n"));
	
	return (AcDbEntity::subSetAttributes (traits)) ;
}

Acad::ErrorStatus ADSKTetrahedronWithInscribedIcosahedron::setFaceOfIcosahedronToRandomColor()
{
	assertWriteEnabled();
	acutPrintf(_T("setFaceOfIcosahedronToRandomColor CALLED\n"));
	// TODO create global class which returns a number in the range or static function
	static std::default_random_engine engine;
	static std::uniform_int_distribution<short> dist(0, 19); // [a,b]
	short nColor{ getRandomColor() };
	Adesk::Int32 i{ dist(engine) };
	acutPrintf(_T("%d %d\n"), (int)i, (int)nColor);
	return m_Icosahedron.setFaceColor(i, nColor);
}

double ADSKTetrahedronWithInscribedIcosahedron::volumesDifference() const noexcept
{
	return m_Icosahedron.volume() - m_Tetrahedron.volume();
}

Acad::ErrorStatus ADSKTetrahedronWithInscribedIcosahedron::myTransformBy(const AcGeMatrix3d& xform)
{
	assertWriteEnabled();
	m_Icosahedron.subTransformBy(xform);
	m_Tetrahedron.subTransformBy(xform);
	m_ptMoveGripPoint.transformBy(xform);
	return Acad::eOk;
}

Acad::ErrorStatus ADSKTetrahedronWithInscribedIcosahedron::subTransformBy(const AcGeMatrix3d& xform)
{
	assertWriteEnabled();
	m_translation *= AcGeMatrix3d::translation(xform.translation());
	m_scaling *= AcGeMatrix3d::scaling(xform.scale());
	//m_rotation *= 

	m_Icosahedron.subTransformBy(xform);
	m_Tetrahedron.subTransformBy(xform);
	m_ptMoveGripPoint.transformBy(xform);
	if (m_bNeedTranfomTransformMatrix) {
		m_transform *= xform;
		m_bNeedTranfomTransformMatrix = true;
	}
		
	//return Acad::eOk;
	return (AcDbEntity::subTransformBy(xform));
}

	//- Osnap points protocol
Acad::ErrorStatus ADSKTetrahedronWithInscribedIcosahedron::subGetOsnapPoints (
	AcDb::OsnapMode osnapMode,
	Adesk::GsMarker gsSelectionMark,
	const AcGePoint3d &pickPoint,
	const AcGePoint3d &lastPoint,
	const AcGeMatrix3d &viewXform,
	AcGePoint3dArray &snapPoints,
	AcDbIntArray &geomIds) const
{
	assertReadEnabled () ;
	return (AcDbEntity::subGetOsnapPoints (osnapMode, gsSelectionMark, pickPoint, lastPoint, viewXform, snapPoints, geomIds)) ;
}

Acad::ErrorStatus ADSKTetrahedronWithInscribedIcosahedron::subGetOsnapPoints (
	AcDb::OsnapMode osnapMode,
	Adesk::GsMarker gsSelectionMark,
	const AcGePoint3d &pickPoint,
	const AcGePoint3d &lastPoint,
	const AcGeMatrix3d &viewXform,
	AcGePoint3dArray &snapPoints,
	AcDbIntArray &geomIds,
	const AcGeMatrix3d &insertionMat) const
{
	assertReadEnabled () ;
	return (AcDbEntity::subGetOsnapPoints (osnapMode, gsSelectionMark, pickPoint, lastPoint, viewXform, snapPoints, geomIds, insertionMat)) ;
}

//- Grip points protocol
Acad::ErrorStatus ADSKTetrahedronWithInscribedIcosahedron::subGetStretchPoints(AcGePoint3dArray& stretchPoints) const
{
	assertReadEnabled();
	acutPrintf(_T("subGetStretchPoints CALLED!"));
	stretchPoints.appendList(m_Tetrahedron.pointAt(0), m_Tetrahedron.pointAt(1), m_Tetrahedron.pointAt(2));
	return Acad::eOk;
	//return Acad::eNotImplemented;
}

Acad::ErrorStatus ADSKTetrahedronWithInscribedIcosahedron::subMoveStretchPointsAt(const AcDbIntArray& indices, const AcGeVector3d& offset)
{
	assertWriteEnabled();
	for (int i : std::views::iota(0, indices.length())) {
		// max from windows.h
		auto dMaxOffset = max(max(offset.x, offset.y), offset.z);
		subTransformBy(AcGeMatrix3d::scaling(dMaxOffset));
	}
	return Acad::eOk;
	//return Acad::eNotImplemented;
}

Acad::ErrorStatus ADSKTetrahedronWithInscribedIcosahedron::subGetGripPoints (
	AcGePoint3dArray &gripPoints, AcDbIntArray &osnapModes, AcDbIntArray &geomIds
) const {
	assertReadEnabled () ;
	//----- This method is never called unless you return eNotImplemented 
	//----- from the new getGripPoints() method below (which is the default implementation)
	gripPoints.append(m_ptMoveGripPoint);
	//gripPoints.appendList(m_Tetrahedron.pointAt(0), m_Tetrahedron.pointAt(1), m_Tetrahedron.pointAt(2));
	return Acad::eOk;
	//return (AcDbEntity::subGetGripPoints (gripPoints, osnapModes, geomIds)) ;
}



Acad::ErrorStatus ADSKTetrahedronWithInscribedIcosahedron::subMoveGripPointsAt (const AcDbIntArray &indices, const AcGeVector3d &offset) {
	if (indices.length() == 0 || offset.isZeroLength())
		return Acad::eOk;
	assertWriteEnabled () ;

	//----- This method is never called unless you return eNotImplemented 
	//----- from the new moveGripPointsAt() method below (which is the default implementation)
	
	for (auto i : indices) {
		if (0 == i)
			subTransformBy(AcGeMatrix3d::translation(offset));
		/*else {
		}*/
	}
	//switch (m_pCurrentGripMode) {

	//}
	return Acad::eOk;

	//return (AcDbEntity::subMoveGripPointsAt (indices, offset)) ;
}

Acad::ErrorStatus ADSKTetrahedronWithInscribedIcosahedron::subGetGripPoints(
	AcDbGripDataPtrArray& grips, const double curViewUnitSize, const int gripSize,
	const AcGeVector3d& curViewDir, const int bitflags
) const {
	assertReadEnabled();
	auto pGripData = new AcDbGripData();
	//pGripData->setAppData();
	pGripData->setGripPoint(m_ptMoveGripPoint);
	grips.append(pGripData);
	return Acad::eOk;
	//----- If you return eNotImplemented here, that will force AutoCAD to call
	//----- the older getGripPoints() implementation. The call below may return
	//----- eNotImplemented depending of your base class.
	//return (AcDbEntity::subGetGripPoints (grips, curViewUnitSize, gripSize, curViewDir, bitflags)) ;
	//return Acad::eNotImplemented;
}

Acad::ErrorStatus ADSKTetrahedronWithInscribedIcosahedron::subMoveGripPointsAt (
	const AcDbVoidPtrArray &gripAppData, const AcGeVector3d &offset,
	const int bitflags
) {
	assertWriteEnabled () ;
	m_translation *= AcGeMatrix3d::translation(offset);

	m_ptMoveGripPoint += offset;
	m_transform *= AcGeMatrix3d::translation(offset);
	m_Icosahedron.subMoveGripPointsAt(gripAppData, offset, bitflags);
	m_Tetrahedron.subMoveGripPointsAt(gripAppData, offset, bitflags);
	return Acad::eOk;
	//----- If you return eNotImplemented here, that will force AutoCAD to call
	//----- the older getGripPoints() implementation. The call below may return
	//----- eNotImplemented depending of your base class.
	//return (AcDbEntity::subMoveGripPointsAt (gripAppData, offset, bitflags)) ;
	//return Acad::eNotImplemented;
}



double ADSKTetrahedronWithInscribedIcosahedron::getInscribedIcosahedronEdgeLength(double adTetrahedronEdgeLenght) const noexcept
{
	// https://en.wikipedia.org/wiki/Tetrahedron
	// https://en.wikipedia.org/wiki/Regular_icosahedron#Mensuration
	double dTetrahedronInsphereRadius{ ADSKTetrahedron::insphereRadiusByEdgeLength(adTetrahedronEdgeLenght) };
	return ADSKIcosahedron::edgeLengthByCircumsphereRadius(dTetrahedronInsphereRadius);
}

