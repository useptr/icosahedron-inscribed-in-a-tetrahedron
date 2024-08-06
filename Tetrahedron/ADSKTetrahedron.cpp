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
//----- ADSKTetrahedron.cpp : Implementation of ADSKTetrahedron
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ADSKTetrahedron.h"
#include "Tchar.h"
#include <memory>
#include <ranges>
//-----------------------------------------------------------------------------
Adesk::UInt32 ADSKTetrahedron::kCurrentVersionNumber = 1;

//-----------------------------------------------------------------------------
ACRX_DXF_DEFINE_MEMBERS(
	ADSKTetrahedron, AcDbEntity,
	AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent,
	AcDbProxyEntity::kNoOperation, ADSKTETRAHEDRON,
	ADSKTETRAHEDRONAPP
	| Product Desc : A description for your object
	| Company : Your company name
	| WEB Address : Your company WEB site address
)

//-----------------------------------------------------------------------------
ADSKTetrahedron::ADSKTetrahedron() : ADSKTetrahedron(AcGePoint3d::kOrigin, 1.0) {
}
ADSKTetrahedron::ADSKTetrahedron(AcGePoint3d aptCenter, double adEdgeLength) : AcDbEntity(), m_dEdgeLength(adEdgeLength), m_ptCenter(aptCenter) {
	calculateVertices();
}
//ADSKTetrahedron::ADSKTetrahedron(const ADSKTetrahedron& other)
//{
//	m_dEdgeLength = other.m_dEdgeLength;
//	m_aVertices = other.m_aVertices;
//}
//
//ADSKTetrahedron& ADSKTetrahedron::operator=(const ADSKTetrahedron& other)
//{
//	if (this == std::addressof(other))
//		return *this;
//	m_dEdgeLength = other.m_dEdgeLength;
//	m_aVertices = other.m_aVertices;
//
//	return *this;
//}


ADSKTetrahedron::~ADSKTetrahedron() {

}

//-----------------------------------------------------------------------------
//----- AcDbObject protocols
//- Dwg Filing protocol
//Acad::ErrorStatus ADSKTetrahedron::dwgOutFields(AcDbDwgFiler * pFiler) const {
//	assertReadEnabled();
//	//----- Save parent class information first.
//	Acad::ErrorStatus es = AcDbEntity::dwgOutFields(pFiler);
//	if (es != Acad::eOk)
//		return (es);
//	//----- Object version number needs to be saved first
//	if ((es = pFiler->writeUInt32(ADSKTetrahedron::kCurrentVersionNumber)) != Acad::eOk)
//		return (es);
//	//----- Output params
//	//pFiler->writeItem(m_dEdgeLength);
//	for (auto& vertex : m_aVertices) {
//		pFiler->writeItem(vertex.x);
//		pFiler->writeItem(vertex.y);
//		pFiler->writeItem(vertex.z);
//	}
//
//	return (pFiler->filerStatus());
//}
//
//Acad::ErrorStatus ADSKTetrahedron::dwgInFields(AcDbDwgFiler * pFiler) {
//	assertWriteEnabled();
//	//----- Read parent class information first.
//	Acad::ErrorStatus es = AcDbEntity::dwgInFields(pFiler);
//	if (es != Acad::eOk)
//		return (es);
//	//----- Object version number needs to be read first
//	Adesk::UInt32 version = 0;
//	if ((es = pFiler->readUInt32(&version)) != Acad::eOk)
//		return (es);
//	if (version > ADSKTetrahedron::kCurrentVersionNumber)
//		return (Acad::eMakeMeProxy);
//	//- Uncomment the 2 following lines if your current object implementation cannot
//	//- support previous version of that object.
//	//if ( version < ADSKTetrahedron::kCurrentVersionNumber )
//	//	return (Acad::eMakeMeProxy) ;
//	//----- Read params
//	//pFiler->readItem(&m_dEdgeLength);
//	for (int i : std::views::iota(0, 4)) {
//		AcGePoint3d pt;
//		pFiler->readItem(&pt.x);
//		pFiler->readItem(&pt.y);
//		pFiler->readItem(&pt.z);
//		m_aVertices.at(i) = pt;
//	}
//	m_dEdgeLength = m_aVertices[0].distanceTo(m_aVertices[1]);
//
//	return (pFiler->filerStatus());
//}
//
////- Dxf Filing protocol
//Acad::ErrorStatus ADSKTetrahedron::dxfOutFields(AcDbDxfFiler * pFiler) const {
//	assertReadEnabled();
//	//----- Save parent class information first.
//	Acad::ErrorStatus es = AcDbEntity::dxfOutFields(pFiler);
//	if (es != Acad::eOk)
//		return (es);
//	es = pFiler->writeItem(AcDb::kDxfSubclass, _RXST("ADSKTetrahedron"));
//	if (es != Acad::eOk)
//		return (es);
//	//----- Object version number needs to be saved first
//	if ((es = pFiler->writeUInt32(kDxfInt32, ADSKTetrahedron::kCurrentVersionNumber)) != Acad::eOk)
//		return (es);
//	//----- Output params
//	pFiler->writeItem(AcDb::kDxfReal + 1, m_dEdgeLength);
//
//	return (pFiler->filerStatus());
//}
//
//Acad::ErrorStatus ADSKTetrahedron::dxfInFields(AcDbDxfFiler * pFiler) {
//	assertWriteEnabled();
//	//----- Read parent class information first.
//	Acad::ErrorStatus es = AcDbEntity::dxfInFields(pFiler);
//	if (es != Acad::eOk || !pFiler->atSubclassData(_RXST("ADSKTetrahedron")))
//		return (pFiler->filerStatus());
//	//----- Object version number needs to be read first
//	struct resbuf rb;
//	pFiler->readItem(&rb);
//	if (rb.restype != AcDb::kDxfInt32) {
//		pFiler->pushBackItem();
//		pFiler->setError(Acad::eInvalidDxfCode, _RXST("\nError: expected group code %d (version #)"), AcDb::kDxfInt32);
//		return (pFiler->filerStatus());
//	}
//	Adesk::UInt32 version = (Adesk::UInt32)rb.resval.rlong;
//	if (version > ADSKTetrahedron::kCurrentVersionNumber)
//		return (Acad::eMakeMeProxy);
//	//- Uncomment the 2 following lines if your current object implementation cannot
//	//- support previous version of that object.
//	//if ( version < ADSKTetrahedron::kCurrentVersionNumber )
//	//	return (Acad::eMakeMeProxy) ;
//	//----- Read params in non order dependant manner
//
//	while ( es == Acad::eOk && (es =pFiler->readResBuf (&rb)) == Acad::eOk ) {
//		switch ( rb.restype ) {
//		case  AcDb::kDxfReal + 1:
//			m_dEdgeLength = rb.resval.rreal;
//			break;
//
//			default:
//				//----- An unrecognized group. Push it back so that the subclass can read it again.
//				pFiler->pushBackItem () ;
//				es =Acad::eEndOfFile ;
//				break ;
//		}
//	}
//
//	//----- At this point the es variable must contain eEndOfFile
//	//----- - either from readResBuf() or from pushback. If not,
//	//----- it indicates that an error happened and we should
//	//----- return immediately.
//	if (es != Acad::eEndOfFile)
//		return (Acad::eInvalidResBuf);
//
//	return (pFiler->filerStatus());
//}

//-----------------------------------------------------------------------------
//----- AcDbEntity protocols
Adesk::Boolean ADSKTetrahedron::subWorldDraw(AcGiWorldDraw * mode) {
	assertReadEnabled();

	Adesk::UInt32 faceListSize = 4 * 4;
	static Adesk::Int32 faceList[] = {
		3, 0, 1, 2,
		3, 0, 1, 3,
		3, 0, 2, 3,
		3, 1, 2, 3,
	};
	
	mode->geometry().shell(m_aVertices.length(), m_aVertices.asArrayPtr(), faceListSize, faceList);
	return Adesk::kTrue;
}


//Adesk::UInt32 ADSKTetrahedron::subSetAttributes(AcGiDrawableTraits * traits) {
//	assertReadEnabled();
//	return (AcDbEntity::subSetAttributes(traits));
//}

Acad::ErrorStatus ADSKTetrahedron::subTransformBy(const AcGeMatrix3d& xform)
{
	assertWriteEnabled();
	// TODO if call scale need ubdate m_dEdgeLength
	for (auto& vertex : m_aVertices) {
		vertex.transformBy(xform);
	}
	m_ptCenter.transformBy(xform);
	updateEdgeLength();
	return Acad::eOk;
}

//- Osnap points protocol

//Acad::ErrorStatus ADSKTetrahedron::subGetOsnapPoints(
//	AcDb::OsnapMode osnapMode,
//	Adesk::GsMarker gsSelectionMark,
//	const AcGePoint3d & pickPoint,
//	const AcGePoint3d & lastPoint,
//	const AcGeMatrix3d & viewXform,
//	AcGePoint3dArray & snapPoints,
//	AcDbIntArray & geomIds) const
//{
//	assertReadEnabled();
//	return (AcDbEntity::subGetOsnapPoints(osnapMode, gsSelectionMark, pickPoint, lastPoint, viewXform, snapPoints, geomIds));
//}
//
//Acad::ErrorStatus ADSKTetrahedron::subGetOsnapPoints(
//	AcDb::OsnapMode osnapMode,
//	Adesk::GsMarker gsSelectionMark,
//	const AcGePoint3d & pickPoint,
//	const AcGePoint3d & lastPoint,
//	const AcGeMatrix3d & viewXform,
//	AcGePoint3dArray & snapPoints,
//	AcDbIntArray & geomIds,
//	const AcGeMatrix3d & insertionMat) const
//{
//	assertReadEnabled();
//	return (AcDbEntity::subGetOsnapPoints(osnapMode, gsSelectionMark, pickPoint, lastPoint, viewXform, snapPoints, geomIds, insertionMat));
//}

//- Grip points protocol

//Acad::ErrorStatus ADSKTetrahedron::subGetGripPoints(
//	AcDbGripDataPtrArray & grips, const double curViewUnitSize, const int gripSize,
//	const AcGeVector3d & curViewDir, const int bitflags
//) const {
//	assertReadEnabled();
//	return Acad::eOk;
//	//----- If you return eNotImplemented here, that will force AutoCAD to call
//	//----- the older getGripPoints() implementation. The call below may return
//	//----- eNotImplemented depending of your base class.
//	//return (AcDbEntity::subGetGripPoints(grips, curViewUnitSize, gripSize, curViewDir, bitflags));
//}
//
//Acad::ErrorStatus ADSKTetrahedron::subMoveGripPointsAt(
//	const AcDbVoidPtrArray & gripAppData, const AcGeVector3d & offset,
//	const int bitflags
//) {
//	assertWriteEnabled();
//	assertWriteEnabled();
//	for (auto& vertex : m_aVertices) {
//		vertex += offset;
//	}
//	return Acad::eOk;
//	//----- If you return eNotImplemented here, that will force AutoCAD to call
//	//----- the older getGripPoints() implementation. The call below may return
//	//----- eNotImplemented depending of your base class.
//	//return (AcDbEntity::subMoveGripPointsAt(gripAppData, offset, bitflags));
//}
#include <cmath>
void ADSKTetrahedron::updateEdgeLength()
{
	assertWriteEnabled();
	m_dEdgeLength = m_aVertices[0].distanceTo(m_aVertices[1]);
}
double ADSKTetrahedron::height(double adEdgeLenght) noexcept
{
	return std::sqrt(6.0) / 3.0 * adEdgeLenght;
}

double ADSKTetrahedron::height() const noexcept
{
	assertReadEnabled();
	return height(m_dEdgeLength);
	//return std::sqrt(6.0) / 3.0 * m_dEdgeLength;
}

void ADSKTetrahedron::calculateVertices() noexcept
{
	assertWriteEnabled();
	double dHalfHeight = height() / 2.0;
	//static double sqrt3 = std::sqrt(3.0); // TODO       
	if (m_aVertices.length() > 0) 
		m_aVertices.removeAll();
	
	m_aVertices.append(AcGePoint3d(m_ptCenter.x -m_dEdgeLength / 2.0, m_ptCenter.y -std::sqrt(3.0) / 6.0 * m_dEdgeLength, m_ptCenter.z - dHalfHeight));
	m_aVertices.append(AcGePoint3d(m_ptCenter.x + m_dEdgeLength / 2.0, m_ptCenter.y -std::sqrt(3.0) / 6.0 * m_dEdgeLength, m_ptCenter.z - dHalfHeight));
	m_aVertices.append(AcGePoint3d(m_ptCenter.x, m_ptCenter.y + std::sqrt(3.0) / 3.0 * m_dEdgeLength, m_ptCenter.z - dHalfHeight));
	m_aVertices.append(AcGePoint3d(m_ptCenter.x, m_ptCenter.y, m_ptCenter.z + dHalfHeight));

	/*m_aVertices.append(AcGePoint3d(-m_dEdgeLength / 2.0, -std::sqrt(3.0) / 6.0 * m_dEdgeLength, 0));
	m_aVertices.append(AcGePoint3d(m_dEdgeLength / 2.0, -std::sqrt(3.0) / 6.0 * m_dEdgeLength, 0));
	m_aVertices.append(AcGePoint3d(0, std::sqrt(3.0) / 3.0 * m_dEdgeLength, 0));
	m_aVertices.append(AcGePoint3d(0, 0, std::sqrt(33.0) / 6.0 * m_dEdgeLength)); */   


}

double ADSKTetrahedron::inradius(double adEdgeLength) noexcept
{
	// https://en.wikipedia.org/wiki/Tetrahedron#Measurement
	return adEdgeLength / std::sqrt(24.0);
}

double ADSKTetrahedron::inradius() const noexcept
{
	return inradius(m_dEdgeLength);
}

double ADSKTetrahedron::volume() const noexcept
{
	// https://en.wikipedia.org/wiki/Tetrahedron#Measurement
	assertReadEnabled();
	return  std::pow(m_dEdgeLength, 3.0)/(6.0*std::sqrt(2.0));
}

const AcGePoint3d& ADSKTetrahedron::center() const
{
	assertReadEnabled();
	return m_ptCenter;
}
Acad::ErrorStatus ADSKTetrahedron::setCenter(const AcGePoint3d& aptCenter)
{
	assertWriteEnabled();
	m_ptCenter = aptCenter;
	calculateVertices();
	return Acad::eOk;
}

double ADSKTetrahedron::edgeLength() const {
	assertReadEnabled();
	return m_dEdgeLength;
}
Acad::ErrorStatus ADSKTetrahedron::setEdgeLength(double adEdgeLenght) {
	assertWriteEnabled();
	m_dEdgeLength = adEdgeLenght;
	calculateVertices();
	return Acad::eOk;
}

const AcGePoint3dArray& ADSKTetrahedron::vertices() const
{
	assertReadEnabled();
	return m_aVertices;
}

Acad::ErrorStatus ADSKTetrahedron::setVertexAt(int aI, AcGePoint3d& arPt)
{
	assertWriteEnabled();
	m_aVertices.at(aI) = arPt;
	return Acad::eOk;
}

AcGePoint3d ADSKTetrahedron::pointAt(Adesk::Int32 ai) const
{
	assertReadEnabled();
	return m_aVertices.at(ai);
}
