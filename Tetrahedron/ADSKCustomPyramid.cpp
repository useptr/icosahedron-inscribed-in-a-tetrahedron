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
#include "ADSKCustomPyramid.h"
#include "Tchar.h"
#include "utilities.h" 
#include <cmath>
#include <numbers>
#include <ranges>
#include <algorithm>
#include <random>
//-----------------------------------------------------------------------------
Adesk::UInt32 ADSKCustomPyramid::kCurrentVersionNumber = 1;

//-----------------------------------------------------------------------------
ACRX_DXF_DEFINE_MEMBERS(
	ADSKCustomPyramid, AcDbEntity,
	AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent,
	AcDbProxyEntity::kNoOperation, ADSKTETRAHEDRONWITHINSCRIBEDICOSAHEDRON,
	ADSKTETRAHEDRONAPP
	| Product Desc : A description for your object
	| Company : Your company name
	| WEB Address : Your company WEB site address
)

//-----------------------------------------------------------------------------
ADSKCustomPyramid::ADSKCustomPyramid() : ADSKCustomPyramid(AcGePoint3d(0.0, 0.0, ADSKTetrahedron::height(1.0)/2.0), 1.0) {
}

ADSKCustomPyramid::ADSKCustomPyramid(AcGePoint3d aptCenter, double adEdgeLength) : AcDbEntity(), m_ptBottomFaceCenter(AcGePoint3d(aptCenter.x, aptCenter.y, aptCenter.z-(ADSKTetrahedron::height(adEdgeLength) / 2.0))), m_Tetrahedron(aptCenter, adEdgeLength), m_Icosahedron(AcGePoint3d(aptCenter.x, aptCenter.y, m_ptBottomFaceCenter.z+ADSKTetrahedron::inradius(adEdgeLength)), icosahedronEdgeLength(adEdgeLength)) {
	//m_Icosahedron.subTransformBy(AcGeMatrix3d::translation(AcGeVector3d(0.0, 0.0, ADSKTetrahedron::insphereRadius(adEdgeLength))));
	//m_Icosahedron.subTransformBy(AcGeMatrix3d::rotation(45.0 * std::numbers::pi_v<double> / 180.0, AcGeVector3d::kZAxis, m_Icosahedron.center()));
}

//ADSKTetrahedronWithInscribedIcosahedron::ADSKTetrahedronWithInscribedIcosahedron(const ADSKTetrahedronWithInscribedIcosahedron& other)
//{
//	m_Tetrahedron = other.m_Tetrahedron;
//	m_Icosahedron = other.m_Icosahedron;
//}

ADSKCustomPyramid::~ADSKCustomPyramid() {
}

//-----------------------------------------------------------------------------
//----- AcDbObject protocols
//- Dwg Filing protocol
Acad::ErrorStatus ADSKCustomPyramid::dwgOutFields(AcDbDwgFiler * pFiler) const {
	assertReadEnabled();
	//----- Save parent class information first.
	Acad::ErrorStatus es = AcDbEntity::dwgOutFields(pFiler);
	if (es != Acad::eOk)
		return (es);
	//----- Object version number needs to be saved first
	if ((es = pFiler->writeUInt32(ADSKCustomPyramid::kCurrentVersionNumber)) != Acad::eOk)
		return (es);
	//----- Output params
	
	// TODO maybe store only Icosahedron center and edge length and calculate Tetrahedron center and edge length
	auto pt = m_Tetrahedron.center();
	pFiler->writeItem(pt.x);
	pFiler->writeItem(pt.y);
	pFiler->writeItem(pt.z);
	pFiler->writeItem(m_Tetrahedron.edgeLength());

	pt = m_Icosahedron.center();
	pFiler->writeItem(pt.x);
	pFiler->writeItem(pt.y);
	pFiler->writeItem(pt.z);
	pFiler->writeItem(m_Icosahedron.edgeLength());

	//for (auto& vertex : m_Icosahedron.vertices()) {
	//	pFiler->writeItem(vertex.x);
	//	pFiler->writeItem(vertex.y);
	//	pFiler->writeItem(vertex.z);
	//}
	//for (auto& vertex : m_Tetrahedron.vertices()) {
	//	pFiler->writeItem(vertex.x);
	//	pFiler->writeItem(vertex.y);
	//	pFiler->writeItem(vertex.z);
	//}

	return (pFiler->filerStatus());
}

Acad::ErrorStatus ADSKCustomPyramid::dwgInFields(AcDbDwgFiler * pFiler) {
	assertWriteEnabled();
	//----- Read parent class information first.
	Acad::ErrorStatus es = AcDbEntity::dwgInFields(pFiler);
	if (es != Acad::eOk)
		return (es);
	//----- Object version number needs to be read first
	Adesk::UInt32 version = 0;
	if ((es = pFiler->readUInt32(&version)) != Acad::eOk)
		return (es);
	if (version > ADSKCustomPyramid::kCurrentVersionNumber)
		return (Acad::eMakeMeProxy);
	//- Uncomment the 2 following lines if your current object implementation cannot
	//- support previous version of that object.
	//if ( version < ADSKTetrahedronWithInscribedIcosahedron::kCurrentVersionNumber )
	//	return (Acad::eMakeMeProxy) ;
	//----- Read params

	// TODO maybe store only Icosahedron center and edge length and calculate Tetrahedron center and edge length
	AcGePoint3d pt;
	pFiler->readItem(&pt.x);
	pFiler->readItem(&pt.y);
	pFiler->readItem(&pt.z);
	m_Tetrahedron.setCenter(pt);
	double edgelength;
	m_ptBottomFaceCenter = AcGePoint3d(pt.x, pt.y, pt.z - m_Tetrahedron.height() / 2.0);
	pFiler->readItem(&edgelength);
	m_Tetrahedron.setEdgeLength(edgelength);

	pFiler->readItem(&pt.x);
	pFiler->readItem(&pt.y);
	pFiler->readItem(&pt.z);
	m_Icosahedron.setCenter(pt);
	pFiler->readItem(&edgelength);
	m_Icosahedron.setEdgeLength(edgelength);

	//for (int i : std::views::iota(0, 12)) {
	//	AcGePoint3d pt;
	//	pFiler->readItem(&pt.x);
	//	pFiler->readItem(&pt.y);
	//	pFiler->readItem(&pt.z);
	//	m_Icosahedron.setVertexAt(i, pt);
	//}
	//for (int i : std::views::iota(0, 4)) {
	//	AcGePoint3d pt;
	//	pFiler->readItem(&pt.x);
	//	pFiler->readItem(&pt.y);
	//	pFiler->readItem(&pt.z);
	//	m_Tetrahedron.setVertexAt(i, pt);
	//}

	return (pFiler->filerStatus());
}

//- Dxf Filing protocol
Acad::ErrorStatus ADSKCustomPyramid::dxfOutFields(AcDbDxfFiler * pFiler) const {
	assertReadEnabled();
	//----- Save parent class information first.
	Acad::ErrorStatus es = AcDbEntity::dxfOutFields(pFiler);
	if (es != Acad::eOk)
		return (es);
	es = pFiler->writeItem(AcDb::kDxfSubclass, _RXST("ADSKTetrahedronWithInscribedIcosahedron"));
	if (es != Acad::eOk)
		return (es);
	//----- Object version number needs to be saved first
	if ((es = pFiler->writeUInt32(kDxfInt32, ADSKCustomPyramid::kCurrentVersionNumber)) != Acad::eOk)
		return (es);
	//----- Output params
	int cnt{ 0 };
	for (int r : std::views::iota(0, 4)) {
		for (int c : std::views::iota(0, 4)) {
			pFiler->writeItem(AcDb::kDxfReal + cnt, m_transform(r, c));
			++cnt;
		}
	}

	return (pFiler->filerStatus());
}

Acad::ErrorStatus ADSKCustomPyramid::dxfInFields(AcDbDxfFiler * pFiler) {
	assertWriteEnabled();
	//----- Read parent class information first.
	Acad::ErrorStatus es = AcDbEntity::dxfInFields(pFiler);
	if (es != Acad::eOk || !pFiler->atSubclassData(_RXST("ADSKTetrahedronWithInscribedIcosahedron")))
		return (pFiler->filerStatus());
	//----- Object version number needs to be read first
	struct resbuf rb;
	pFiler->readItem(&rb);
	if (rb.restype != AcDb::kDxfInt32) {
		pFiler->pushBackItem();
		pFiler->setError(Acad::eInvalidDxfCode, _RXST("\nError: expected group code %d (version #)"), AcDb::kDxfInt32);
		return (pFiler->filerStatus());
	}
	Adesk::UInt32 version = (Adesk::UInt32)rb.resval.rlong;
	if (version > ADSKCustomPyramid::kCurrentVersionNumber)
		return (Acad::eMakeMeProxy);
	//- Uncomment the 2 following lines if your current object implementation cannot
	//- support previous version of that object.
	//if ( version < ADSKTetrahedronWithInscribedIcosahedron::kCurrentVersionNumber )
	//	return (Acad::eMakeMeProxy) ;
	//----- Read params in non order dependant manner

	while (es == Acad::eOk && (es = pFiler->readResBuf(&rb)) == Acad::eOk) {
		switch (rb.restype) {
		case (AcDb::kDxfReal):
			m_transform(0, 0) = rb.resval.rreal;
			break;
		case (AcDb::kDxfReal + 1):
			m_transform(0, 1) = rb.resval.rreal;
			break;
		case (AcDb::kDxfReal + 2):
			m_transform(0, 2) = rb.resval.rreal;
			break;
		case (AcDb::kDxfReal + 3):
			m_transform(0, 3) = rb.resval.rreal;
			break;

		case (AcDb::kDxfReal + 4):
			m_transform(1, 0) = rb.resval.rreal;
			break;
		case (AcDb::kDxfReal + 5):
			m_transform(1, 1) = rb.resval.rreal;
			break;
		case (AcDb::kDxfReal + 6):
			m_transform(1, 2) = rb.resval.rreal;
			break;
		case (AcDb::kDxfReal + 7):
			m_transform(1, 3) = rb.resval.rreal;
			break;

		case (AcDb::kDxfReal + 8):
			m_transform(2, 0) = rb.resval.rreal;
			break;
		case (AcDb::kDxfReal + 9):
			m_transform(2, 1) = rb.resval.rreal;
			break;
		case (AcDb::kDxfReal + 10):
			m_transform(2, 2) = rb.resval.rreal;
			break;
		case (AcDb::kDxfReal + 11):
			m_transform(2, 3) = rb.resval.rreal;
			break;

		case (AcDb::kDxfReal + 12):
			m_transform(3, 0) = rb.resval.rreal;
			break;
		case (AcDb::kDxfReal + 13):
			m_transform(3, 1) = rb.resval.rreal;
			break;
		case (AcDb::kDxfReal + 14):
			m_transform(3, 2) = rb.resval.rreal;
			break;
		case (AcDb::kDxfReal + 15):
			m_transform(3, 3) = rb.resval.rreal;
			break;

		default:
			//----- An unrecognized group. Push it back so that the subclass can read it again.
			pFiler->pushBackItem();
			es = Acad::eEndOfFile;
			break;
		}
	}

	//----- At this point the es variable must contain eEndOfFile
	//----- - either from readResBuf() or from pushback. If not,
	//----- it indicates that an error happened and we should
	//----- return immediately.
	if (es != Acad::eEndOfFile)
		return (Acad::eInvalidResBuf);

	applyTransformMatrix(m_transform);

	return (pFiler->filerStatus());
}

//-----------------------------------------------------------------------------
//----- AcDbEntity protocols
Adesk::Boolean ADSKCustomPyramid::subWorldDraw(AcGiWorldDraw * mode) {
	assertReadEnabled();
	AcCmTransparency transparency(0.5);
	mode->subEntityTraits().setTransparency(transparency);
	m_Tetrahedron.subWorldDraw(mode);

	transparency.setAlphaPercent(1.0);
	mode->subEntityTraits().setTransparency(transparency);
	m_Icosahedron.subWorldDraw(mode);
	return Adesk::kTrue;
}


Adesk::UInt32 ADSKCustomPyramid::subSetAttributes(AcGiDrawableTraits * traits) {
	assertReadEnabled();
	//acutPrintf(_T("::subSetAttributes CALLED\n"));

	return (AcDbEntity::subSetAttributes(traits));
}

Acad::ErrorStatus ADSKCustomPyramid::setFaceOfIcosahedronToRandomColor()
{
	assertWriteEnabled();
	//acutPrintf(_T("setFaceOfIcosahedronToRandomColor CALLED\n"));
	// TODO create global class which returns a number in the range or static function
	static std::default_random_engine engine;
	static std::uniform_int_distribution<short> dist(0, 19); // [a,b]
	short nColor{ getRandomColor() };
	Adesk::Int32 i{ dist(engine) };
	//acutPrintf(_T("%d %d\n"), (int)i, (int)nColor);
	return m_Icosahedron.setFaceColor(i, nColor);
}

double ADSKCustomPyramid::volumesDifference() const noexcept
{
	return m_Tetrahedron.volume() - m_Icosahedron.volume();
}

Acad::ErrorStatus ADSKCustomPyramid::applyTransformMatrix(const AcGeMatrix3d & xform)
{
	assertWriteEnabled();
	m_Icosahedron.subTransformBy(xform);
	m_Tetrahedron.subTransformBy(xform);
	m_ptBottomFaceCenter.transformBy(xform);
	return Acad::eOk;
}
//Acad::ErrorStatus ADSKTetrahedronWithInscribedIcosahedron::subGetTransformedCopy(const AcGeMatrix3d& xform, AcDbEntity*& ent) const
//{
//	assertReadEnabled();
//	auto pEntityCopy = new ADSKTetrahedronWithInscribedIcosahedron();
//	pEntityCopy->m_transform = m_transform;
//	pEntityCopy->m_transform *= xform;
//	pEntityCopy->m_ptMoveGripPoint = m_ptMoveGripPoint;
//	pEntityCopy->m_ptMoveGripPoint.transformBy( xform);
//	// TODO implement subGetTransformedCopy in Icosahedron and Tetrahedron
//	for (int i : std::views::iota(0, m_Icosahedron.m_aVertices.length())) {
//		pEntityCopy->m_Icosahedron.m_aVertices.at(i) = m_Icosahedron.m_aVertices.at(i);
//		pEntityCopy->m_Icosahedron.m_aVertices.at(i).transformBy(xform);
//	}
//		
//	for (int i : std::views::iota(0, m_Tetrahedron.m_aVertices.length())) {
//		pEntityCopy->m_Tetrahedron.m_aVertices.at(i) = m_Tetrahedron.m_aVertices.at(i);
//		pEntityCopy->m_Icosahedron.m_aVertices.at(i).transformBy(xform);
//	}
//		
//	ent = pEntityCopy;
//	return Acad::eOk;
//}

Acad::ErrorStatus ADSKCustomPyramid::subTransformBy(const AcGeMatrix3d & xform)
{
	assertWriteEnabled();
	/*acutPrintf(_T("xform:\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n\n"), xform(0, 0), xform(0, 1), xform(0, 2), xform(0, 3),
		xform(1, 0), xform(1, 1), xform(1, 2), xform(1, 3),
		xform(2, 0), xform(2, 1), xform(2, 2), xform(2, 3),
		xform(3, 0), xform(3, 1), xform(3, 2), xform(3, 3));*/

	m_Icosahedron.subTransformBy(xform);
	m_Tetrahedron.subTransformBy(xform);
	m_ptBottomFaceCenter.transformBy(xform);

	return Acad::eOk;
}

//- Osnap points protocol
Acad::ErrorStatus ADSKCustomPyramid::subGetOsnapPoints(
	AcDb::OsnapMode osnapMode,
	Adesk::GsMarker gsSelectionMark,
	const AcGePoint3d & pickPoint,
	const AcGePoint3d & lastPoint,
	const AcGeMatrix3d & viewXform,
	AcGePoint3dArray & snapPoints,
	AcDbIntArray & geomIds) const
{
	assertReadEnabled();
	return (AcDbEntity::subGetOsnapPoints(osnapMode, gsSelectionMark, pickPoint, lastPoint, viewXform, snapPoints, geomIds));
}

Acad::ErrorStatus ADSKCustomPyramid::subGetOsnapPoints(
	AcDb::OsnapMode osnapMode,
	Adesk::GsMarker gsSelectionMark,
	const AcGePoint3d & pickPoint,
	const AcGePoint3d & lastPoint,
	const AcGeMatrix3d & viewXform,
	AcGePoint3dArray & snapPoints,
	AcDbIntArray & geomIds,
	const AcGeMatrix3d & insertionMat) const
{
	assertReadEnabled();
	return (AcDbEntity::subGetOsnapPoints(osnapMode, gsSelectionMark, pickPoint, lastPoint, viewXform, snapPoints, geomIds, insertionMat));
}

//- Grip points protocol
//Acad::ErrorStatus ADSKTetrahedronWithInscribedIcosahedron::subGetStretchPoints(AcGePoint3dArray& stretchPoints) const
//{
//	assertReadEnabled();
//	acutPrintf(_T("subGetStretchPoints CALLED!\n"));
//	stretchPoints.appendList(m_Tetrahedron.pointAt(0), m_Tetrahedron.pointAt(1), m_Tetrahedron.pointAt(2));
//	return Acad::eOk;
//	//return Acad::eNotImplemented;
//}
//
//Acad::ErrorStatus ADSKTetrahedronWithInscribedIcosahedron::subMoveStretchPointsAt(const AcDbIntArray& indices, const AcGeVector3d& offset)
//{
//	assertWriteEnabled();
//	for (int i : std::views::iota(0, indices.length())) {
//		// max from windows.h
//		auto dMaxOffset = max(max(offset.x, offset.y), offset.z);
//		subTransformBy(AcGeMatrix3d::scaling(dMaxOffset));
//	}
//	return Acad::eOk;
//	//return Acad::eNotImplemented;
//}

class GripAppData final
{
private:
	int m_ID; ///< идентификатор для Grip
public:
	GripAppData(int aId) : m_ID(aId) {}
	int index() const noexcept { return m_ID; }
};

Acad::ErrorStatus ADSKCustomPyramid::subGetGripPoints(
	AcDbGripDataPtrArray & grips, const double curViewUnitSize, const int gripSize,
	const AcGeVector3d & curViewDir, const int bitflags
) const {
	assertReadEnabled();
	for (auto i : std::views::iota(0, 4)) {
		auto pGripData = new AcDbGripData();
		pGripData->setGripPoint((m_Tetrahedron.vertices()[i]));
		pGripData->setAppData(new GripAppData(i));
		grips.append(pGripData);
	}
	auto pGripData = new AcDbGripData();
	pGripData->setGripPoint(m_ptBottomFaceCenter);
	pGripData->setAppData(new GripAppData(4));
	grips.append(pGripData);
	return Acad::eOk;
	//----- If you return eNotImplemented here, that will force AutoCAD to call
	//----- the older getGripPoints() implementation. The call below may return
	//----- eNotImplemented depending of your base class.
	//return (AcDbEntity::subGetGripPoints (grips, curViewUnitSize, gripSize, curViewDir, bitflags)) ;
	//return Acad::eNotImplemented;
}

Acad::ErrorStatus ADSKCustomPyramid::subMoveGripPointsAt(
	const AcDbVoidPtrArray & gripAppData, const AcGeVector3d & offset,
	const int bitflags
) {
	assertWriteEnabled();
	for (auto pGripAppData : gripAppData) {
		auto pGripData = static_cast<GripAppData*>(pGripAppData);
		if (pGripData == nullptr) {
			continue;
		}
		auto idx = pGripData->index();
		if (4 == idx) { // botoom face center point
			subTransformBy(AcGeMatrix3d::translation(offset));
		}
		else { // tetrahedron corner points
			/*auto pt = m_Tetrahedron.vertices()[idx];
			auto vec = m_Tetrahedron.center() - pt;
			double originalLength = vec.length();
			double newLength = (vec + offset).length();
			double scaleFactor = newLength / originalLength;
			subTransformBy(AcGeMatrix3d::scaling(scaleFactor, m_Tetrahedron.center()));
			subTransformBy(offset);*/
			subTransformBy(AcGeMatrix3d::scaling(offset.length(), m_Tetrahedron.center()));
		}
	}
	return Acad::eOk;
	//----- If you return eNotImplemented here, that will force AutoCAD to call
	//----- the older getGripPoints() implementation. The call below may return
	//----- eNotImplemented depending of your base class.
	//return (AcDbEntity::subMoveGripPointsAt (gripAppData, offset, bitflags)) ;
	//return Acad::eNotImplemented;
}


double ADSKCustomPyramid::icosahedronEdgeLength(double adTetrahedronEdgeLength) const noexcept
{
	// https://en.wikipedia.org/wiki/Tetrahedron
	// https://en.wikipedia.org/wiki/Regular_icosahedron#Mensuration
	double dTetrahedronInsphereRadius{ ADSKTetrahedron::inradius(adTetrahedronEdgeLength) };
	return ADSKIcosahedron::edgeLengthByCircumradius(dTetrahedronInsphereRadius);
}

Acad::ErrorStatus ADSKCustomPyramid::setEdgeLength(const double adEdgeLength) {
	assertWriteEnabled();
	m_Tetrahedron.setEdgeLength(adEdgeLength);
	m_Icosahedron.setEdgeLength(icosahedronEdgeLength(adEdgeLength));
	return Acad::eOk;
}

Acad::ErrorStatus ADSKCustomPyramid::setCenter(const AcGePoint3d& aptCenter)
{
	auto es = m_Tetrahedron.setCenter(aptCenter);
	m_ptBottomFaceCenter = AcGePoint3d(aptCenter.x, aptCenter.y, aptCenter.z - (m_Tetrahedron.height() / 2.0));
	if (Acad::eOk != es)
		return es;
	return m_Icosahedron.setCenter(AcGePoint3d(aptCenter.x, aptCenter.y, m_ptBottomFaceCenter.z + m_Tetrahedron.inradius()));
}

const AcGePoint3d& ADSKCustomPyramid::center() const
{
	return m_Tetrahedron.center();
}
