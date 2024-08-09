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
#include <cmath>
#include "utilities.h"
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

ADSKTetrahedron::~ADSKTetrahedron() {

}

//-----------------------------------------------------------------------------
//----- AcDbObject protocols
//- Dwg Filing protocol
Acad::ErrorStatus ADSKTetrahedron::dwgOutFields(AcDbDwgFiler * pFiler) const {
	assertReadEnabled();
	//----- Save parent class information first.
	Acad::ErrorStatus es = AcDbEntity::dwgOutFields(pFiler);
	if (es != Acad::eOk)
		return (es);
	//----- Object version number needs to be saved first
	if ((es = pFiler->writeUInt32(ADSKTetrahedron::kCurrentVersionNumber)) != Acad::eOk)
		return (es);
	//----- Output params
	es = pFiler->writePoint3d(m_ptCenter);
	if (es != Acad::eOk)
		return es;
	es = pFiler->writeItem(m_dEdgeLength);
	if (es != Acad::eOk)
		return es;

	return (pFiler->filerStatus());
}

Acad::ErrorStatus ADSKTetrahedron::dwgInFields(AcDbDwgFiler * pFiler) {
	assertWriteEnabled();
	//----- Read parent class information first.
	Acad::ErrorStatus es = AcDbEntity::dwgInFields(pFiler);
	if (es != Acad::eOk)
		return (es);
	//----- Object version number needs to be read first
	Adesk::UInt32 version = 0;
	if ((es = pFiler->readUInt32(&version)) != Acad::eOk)
		return (es);
	if (version > ADSKTetrahedron::kCurrentVersionNumber)
		return (Acad::eMakeMeProxy);
	//- Uncomment the 2 following lines if your current object implementation cannot
	//- support previous version of that object.
	//if ( version < ADSKTetrahedron::kCurrentVersionNumber )
	//	return (Acad::eMakeMeProxy) ;
	//----- Read params
	es = pFiler->readPoint3d(&m_ptCenter);
	if (es != Acad::eOk)
		return es;
	es = pFiler->readItem(&m_dEdgeLength);
	if (es != Acad::eOk)
		return es;
	calculateVertices();

	return (pFiler->filerStatus());
}

//-----------------------------------------------------------------------------
//----- AcDbEntity protocols
Adesk::Boolean ADSKTetrahedron::subWorldDraw(AcGiWorldDraw * mode) {
	assertReadEnabled();
	try {
		Adesk::UInt32 faceListSize{ 4 * 4 };
		// Массив индексов вершин для каждой грани тетраэдра, метод shell из AcGiGeometry принимает сначала количество вершин для грани, затем индексы самих вершин 
		static const Adesk::Int32 faceList[] = {
			3, 0, 1, 2,
			3, 0, 1, 3,
			3, 0, 2, 3,
			3, 1, 2, 3,
		};
		auto status = mode->geometry().shell(m_aVertices.length(), m_aVertices.asArrayPtr(), faceListSize, faceList);
		if (status != Adesk::kTrue)
			return status;
	}
	catch (...) {
		return Adesk::kFalse;
	}
	return Adesk::kTrue;
}

Acad::ErrorStatus ADSKTetrahedron::subTransformBy(const AcGeMatrix3d & xform)
{
	assertWriteEnabled();
	for (auto& vertex : m_aVertices) {
		vertex.transformBy(xform);
	}
	m_ptCenter.transformBy(xform);
	updateEdgeLength();
	return Acad::eOk;
}

Acad::ErrorStatus ADSKTetrahedron::subGetGripPoints(
	AcDbGripDataPtrArray & grips, const double curViewUnitSize, const int gripSize,
	const AcGeVector3d & curViewDir, const int bitflags
) const {
	assertReadEnabled();
	try {
		auto pGripData = new AcDbGripData();
		pGripData->setGripPoint(m_ptCenter);
		pGripData->setAppData(new GripAppData(0));
		grips.append(pGripData);
	}
	catch (std::bad_alloc&) {
		return Acad::eOutOfMemory;
	}
	return Acad::eOk;
}

Acad::ErrorStatus ADSKTetrahedron::subMoveGripPointsAt(
	const AcDbVoidPtrArray & gripAppData, const AcGeVector3d & offset,
	const int bitflags
) {
	assertWriteEnabled();
		for (auto pGripAppData : gripAppData) {
			auto pGripData = static_cast<GripAppData*>(pGripAppData);
			if (pGripData == nullptr) {
				continue;
			}
			switch (pGripData->index())
			{
			case 0: // indexes 0-3 tetrahedron corner points 
			{
				auto es = subTransformBy(AcGeMatrix3d::translation(offset));
				if (es != Acad::eOk)
					return es;
				break;
			}
			}
		}
	return Acad::eOk;
}

void ADSKTetrahedron::updateEdgeLength() noexcept
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
}

void ADSKTetrahedron::calculateVertices() noexcept
{
	assertWriteEnabled();
	double dHalfHeight = height() / 2.0;
	static double sqrt3 = std::sqrt(3.0);
	if (m_aVertices.length() > 0)
		m_aVertices.removeAll();
	m_aVertices.append(AcGePoint3d(m_ptCenter.x - m_dEdgeLength / 2.0, m_ptCenter.y - sqrt3 / 6.0 * m_dEdgeLength, m_ptCenter.z - dHalfHeight));
	m_aVertices.append(AcGePoint3d(m_ptCenter.x + m_dEdgeLength / 2.0, m_ptCenter.y - sqrt3 / 6.0 * m_dEdgeLength, m_ptCenter.z - dHalfHeight));
	m_aVertices.append(AcGePoint3d(m_ptCenter.x, m_ptCenter.y + sqrt3 / 3.0 * m_dEdgeLength, m_ptCenter.z - dHalfHeight));
	m_aVertices.append(AcGePoint3d(m_ptCenter.x, m_ptCenter.y, m_ptCenter.z + dHalfHeight));
}

double ADSKTetrahedron::inradius(double adEdgeLength) noexcept
{
	// https://en.wikipedia.org/wiki/Tetrahedron#Measurement
	return adEdgeLength / std::sqrt(24.0);
}

double ADSKTetrahedron::midradius(double adEdgeLength) noexcept
{
	// https://en.wikipedia.org/wiki/Tetrahedron#Measurement
	return adEdgeLength / std::sqrt(8);
}

double ADSKTetrahedron::inradius() const noexcept
{
	assertReadEnabled();
	return inradius(m_dEdgeLength);
}

double ADSKTetrahedron::volume() const noexcept
{
	// https://en.wikipedia.org/wiki/Tetrahedron#Measurement
	assertReadEnabled();
	return  std::pow(m_dEdgeLength, 3.0) / (6.0 * std::sqrt(2.0));
}

const AcGePoint3d& ADSKTetrahedron::center() const
{
	assertReadEnabled();
	return m_ptCenter;
}
Acad::ErrorStatus ADSKTetrahedron::setCenter(const AcGePoint3d & aptCenter)
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

