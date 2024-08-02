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
//----- ADSKIcosahedron.cpp : Implementation of ADSKIcosahedron
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ADSKIcosahedron.h"
#include <cmath>
#include <numbers>
#include <memory>
#include <utility>
//-----------------------------------------------------------------------------
Adesk::UInt32 ADSKIcosahedron::kCurrentVersionNumber =1 ;

//-----------------------------------------------------------------------------
ACRX_DXF_DEFINE_MEMBERS (
	ADSKIcosahedron, AcDbEntity,
	AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent, 
	AcDbProxyEntity::kNoOperation, ADSKICOSAHEDRON,
ADSKPOLYHEDRONSAPP
|Product Desc:     A description for your object
|Company:          Your company name
|WEB Address:      Your company WEB site address
)

//-----------------------------------------------------------------------------
ADSKIcosahedron::ADSKIcosahedron () : AcDbEntity (), m_dEdgeLength(1.0) {
	calculateVertices();
	m_faceDataManager.setColors(std::move(std::make_unique<short[]>(20)));
}

//ADSKIcosahedron::ADSKIcosahedron(const ADSKIcosahedron& other)
//{
//	m_dEdgeLength = other.m_dEdgeLength;
//	m_aVertices = other.m_aVertices;
//}
//
//ADSKIcosahedron& ADSKIcosahedron::operator=(const ADSKIcosahedron& other)
//{
//	if (this == std::addressof(other))
//		return *this;
//	m_dEdgeLength = other.m_dEdgeLength;
//	m_aVertices = other.m_aVertices;
//	return *this;
//}

ADSKIcosahedron::ADSKIcosahedron(double adEdgeLength) : AcDbEntity(), m_dEdgeLength(adEdgeLength) {
	calculateVertices();
	m_faceDataManager.setColors(std::move(std::make_unique<short[]>(20)));
}

ADSKIcosahedron::~ADSKIcosahedron () {
}

//-----------------------------------------------------------------------------
//----- AcDbObject protocols
//- Dwg Filing protocol
Acad::ErrorStatus ADSKIcosahedron::dwgOutFields (AcDbDwgFiler *pFiler) const {
	assertReadEnabled () ;
	//----- Save parent class information first.
	Acad::ErrorStatus es =AcDbEntity::dwgOutFields (pFiler) ;
	if ( es != Acad::eOk )
		return (es) ;
	//----- Object version number needs to be saved first
	if ( (es =pFiler->writeUInt32 (ADSKIcosahedron::kCurrentVersionNumber)) != Acad::eOk )
		return (es) ;
	//----- Output params
	pFiler->writeItem(m_dEdgeLength);

	return (pFiler->filerStatus ()) ;
}

Acad::ErrorStatus ADSKIcosahedron::dwgInFields (AcDbDwgFiler *pFiler) {
	assertWriteEnabled () ;
	//----- Read parent class information first.
	Acad::ErrorStatus es =AcDbEntity::dwgInFields (pFiler) ;
	if ( es != Acad::eOk )
		return (es) ;
	//----- Object version number needs to be read first
	Adesk::UInt32 version =0 ;
	if ( (es =pFiler->readUInt32 (&version)) != Acad::eOk )
		return (es) ;
	if ( version > ADSKIcosahedron::kCurrentVersionNumber )
		return (Acad::eMakeMeProxy) ;
	//- Uncomment the 2 following lines if your current object implementation cannot
	//- support previous version of that object.
	//if ( version < ADSKIcosahedron::kCurrentVersionNumber )
	//	return (Acad::eMakeMeProxy) ;
	//----- Read params
	pFiler->readItem(&m_dEdgeLength);

	return (pFiler->filerStatus ()) ;
}

//- Dxf Filing protocol
Acad::ErrorStatus ADSKIcosahedron::dxfOutFields (AcDbDxfFiler *pFiler) const {
	assertReadEnabled () ;
	//----- Save parent class information first.
	Acad::ErrorStatus es =AcDbEntity::dxfOutFields (pFiler) ;
	if ( es != Acad::eOk )
		return (es) ;
	es =pFiler->writeItem (AcDb::kDxfSubclass, _RXST("ADSKIcosahedron")) ;
	if ( es != Acad::eOk )
		return (es) ;
	//----- Object version number needs to be saved first
	if ( (es =pFiler->writeUInt32 (kDxfInt32, ADSKIcosahedron::kCurrentVersionNumber)) != Acad::eOk )
		return (es) ;
	//----- Output params
	pFiler->writeItem(AcDb::kDxfReal + 1, m_dEdgeLength);

	return (pFiler->filerStatus ()) ;
}

Acad::ErrorStatus ADSKIcosahedron::dxfInFields (AcDbDxfFiler *pFiler) {
	assertWriteEnabled () ;
	//----- Read parent class information first.
	Acad::ErrorStatus es =AcDbEntity::dxfInFields (pFiler) ;
	if ( es != Acad::eOk || !pFiler->atSubclassData (_RXST("ADSKIcosahedron")) )
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
	if ( version > ADSKIcosahedron::kCurrentVersionNumber )
		return (Acad::eMakeMeProxy) ;
	//- Uncomment the 2 following lines if your current object implementation cannot
	//- support previous version of that object.
	//if ( version < ADSKIcosahedron::kCurrentVersionNumber )
	//	return (Acad::eMakeMeProxy) ;
	//----- Read params in non order dependant manner

	while ( es == Acad::eOk && (es =pFiler->readResBuf (&rb)) == Acad::eOk ) {
		switch ( rb.restype ) {
			case  AcDb::kDxfReal + 1:
				m_dEdgeLength = rb.resval.rreal;
			break;

			default:
				//----- An unrecognized group. Push it back so that the subclass can read it again.
				pFiler->pushBackItem () ;
				es =Acad::eEndOfFile ;
				break ;
		}
	}

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
Adesk::Boolean ADSKIcosahedron::subWorldDraw (AcGiWorldDraw *mode) {
	assertReadEnabled () ;

	Adesk::UInt32 faceListSize = 4*20;

	static Adesk::Int32 faceList[] = { 
		3, 0, 4, 1, 
		3, 0, 9, 4,
		3, 9, 5, 4,
		3, 4, 5, 8,
		3, 4, 8, 1,
		3, 8, 10, 1, 
		3, 8, 3, 10, 
		3, 5, 3, 8, 
		3, 5, 2, 3, 
		3, 2, 7, 3,
		3, 7, 10, 3, 
		3, 7, 6, 10, 
		3, 7, 11, 6, 
		3, 11, 0, 6, 
		3, 0, 1, 6,
		3, 6, 1, 10, 
		3, 9, 0, 11, 
		3, 9, 11, 2, 
		3, 9, 2, 5, 
		3, 7, 2, 11 
	};
	mode->subEntityTraits().setColor(1);
	mode->geometry().shell(m_aVertices.length(), m_aVertices.asArrayPtr(), faceListSize, faceList, nullptr, m_faceDataManager.faceData());
	//return (AcDbEntity::subWorldDraw (mode)) ;
	return Adesk::kTrue;
}


Adesk::UInt32 ADSKIcosahedron::subSetAttributes (AcGiDrawableTraits *traits) {
	assertReadEnabled () ;
	return (AcDbEntity::subSetAttributes (traits)) ;
}

Acad::ErrorStatus ADSKIcosahedron::subTransformBy(const AcGeMatrix3d& xform)
{
	assertWriteEnabled();
	// TODO if call scale need ubdate m_dEdgeLength
	for (auto& vertex : m_aVertices) {
		vertex.transformBy(xform);
	}
	return Acad::eOk;
}

	//- Osnap points protocol
Acad::ErrorStatus ADSKIcosahedron::subGetOsnapPoints (
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

Acad::ErrorStatus ADSKIcosahedron::subGetOsnapPoints (
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
Acad::ErrorStatus ADSKIcosahedron::subGetGripPoints (
	AcGePoint3dArray &gripPoints, AcDbIntArray &osnapModes, AcDbIntArray &geomIds
) const {
	assertReadEnabled () ;
	//----- This method is never called unless you return eNotImplemented 
	//----- from the new getGripPoints() method below (which is the default implementation)

	return (AcDbEntity::subGetGripPoints (gripPoints, osnapModes, geomIds)) ;
}

Acad::ErrorStatus ADSKIcosahedron::subMoveGripPointsAt (const AcDbIntArray &indices, const AcGeVector3d &offset) {
	assertWriteEnabled () ;
	//----- This method is never called unless you return eNotImplemented 
	//----- from the new moveGripPointsAt() method below (which is the default implementation)

	return (AcDbEntity::subMoveGripPointsAt (indices, offset)) ;
}

Acad::ErrorStatus ADSKIcosahedron::subGetGripPoints (
	AcDbGripDataPtrArray &grips, const double curViewUnitSize, const int gripSize, 
	const AcGeVector3d &curViewDir, const int bitflags
) const {
	assertReadEnabled () ;
	return Acad::eOk;
	//----- If you return eNotImplemented here, that will force AutoCAD to call
	//----- the older getGripPoints() implementation. The call below may return
	//----- eNotImplemented depending of your base class.
	//return (AcDbEntity::subGetGripPoints (grips, curViewUnitSize, gripSize, curViewDir, bitflags)) ;
}

Acad::ErrorStatus ADSKIcosahedron::subMoveGripPointsAt (
	const AcDbVoidPtrArray &gripAppData, const AcGeVector3d &offset,
	const int bitflags
) {
	assertWriteEnabled () ;
	for (auto& vertex : m_aVertices) {
		vertex += offset;
	}
	//----- If you return eNotImplemented here, that will force AutoCAD to call
	//----- the older getGripPoints() implementation. The call below may return
	//----- eNotImplemented depending of your base class.
	//return (AcDbEntity::subMoveGripPointsAt (gripAppData, offset, bitflags)) ;
	return Acad::eOk;
}

double ADSKIcosahedron::volume() const noexcept
{
	return 5.0*std::sqrt(3.0)*std::pow(m_dEdgeLength,2.0);
}

void ADSKIcosahedron::calculateVertices() noexcept
{
	auto phi = std::numbers::phi_v<double>;
	double t = m_dEdgeLength / 2.0;
	double s = m_dEdgeLength * phi / 2.0;
	if (m_aVertices.length() > 0)
		m_aVertices.removeAll();
	m_aVertices.appendList(AcGePoint3d(-t, 0, s), AcGePoint3d(t, 0, s), AcGePoint3d(-t, 0, -s), AcGePoint3d(t, 0, -s),
		AcGePoint3d(0, s, t), AcGePoint3d(0, s, -t), AcGePoint3d(0, -s, t), AcGePoint3d(0, -s, -t),
		AcGePoint3d(s, t, 0), AcGePoint3d(-s, t, 0), AcGePoint3d(s, -t, 0), AcGePoint3d(-s, -t, 0));
	
}

double ADSKIcosahedron::edgeLengthByCircumsphereRadius(double adCircumsphereRadius) noexcept
{
	return 2.0 * adCircumsphereRadius / std::sqrt(std::numbers::phi_v<double> +1.0);
}

Acad::ErrorStatus ADSKIcosahedron::setFaceColor(Adesk::Int32 aI, short anColor)
{
	assertWriteEnabled();
	//static_assert(aI > 0 && aI < 20);
	m_faceDataManager.colors()[aI] = anColor;
	return Acad::eOk;
}

Acad::ErrorStatus ADSKIcosahedron::edgeLength(double& ardEdgeLenght) const {
	assertReadEnabled();
	ardEdgeLenght = m_dEdgeLength;
	return Acad::eOk;
}
Acad::ErrorStatus ADSKIcosahedron::setEdgeLength(const double adEdgeLenght) {
	assertWriteEnabled();
	m_dEdgeLength = adEdgeLenght;
	//calculateVertices();
	return Acad::eOk;
}
