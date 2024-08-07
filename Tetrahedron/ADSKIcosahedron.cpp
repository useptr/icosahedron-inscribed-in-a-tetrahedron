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
#include <ranges>
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
ADSKIcosahedron::ADSKIcosahedron() : ADSKIcosahedron(AcGePoint3d::kOrigin, 1.0) {
}

ADSKIcosahedron::ADSKIcosahedron(AcGePoint3d aptCenter, double adEdgeLength) : AcDbEntity(), m_dEdgeLength(adEdgeLength), m_ptCenter(aptCenter) {
	calculateVertices();
	m_faceDataManager.setColors(std::move(std::make_unique<short[]>(20)));
}

ADSKIcosahedron::~ADSKIcosahedron () {
}

//-----------------------------------------------------------------------------
//----- AcDbObject protocols
//- Dwg Filing protocol
//Acad::ErrorStatus ADSKIcosahedron::dwgOutFields (AcDbDwgFiler *pFiler) const {
//	assertReadEnabled () ;
//	//----- Save parent class information first.
//	Acad::ErrorStatus es =AcDbEntity::dwgOutFields (pFiler) ;
//	if ( es != Acad::eOk )
//		return (es) ;
//	//----- Object version number needs to be saved first
//	if ( (es =pFiler->writeUInt32 (ADSKIcosahedron::kCurrentVersionNumber)) != Acad::eOk )
//		return (es) ;
//	//----- Output params
//	//pFiler->writeItem(m_dEdgeLength);
//	for (auto& vertex : m_aVertices) {
//		pFiler->writeItem(vertex.x);
//		pFiler->writeItem(vertex.y);
//		pFiler->writeItem(vertex.z);
//	}
//
//	return (pFiler->filerStatus ()) ;
//}
//
//Acad::ErrorStatus ADSKIcosahedron::dwgInFields (AcDbDwgFiler *pFiler) {
//	assertWriteEnabled () ;
//	//----- Read parent class information first.
//	Acad::ErrorStatus es =AcDbEntity::dwgInFields (pFiler) ;
//	if ( es != Acad::eOk )
//		return (es) ;
//	//----- Object version number needs to be read first
//	Adesk::UInt32 version =0 ;
//	if ( (es =pFiler->readUInt32 (&version)) != Acad::eOk )
//		return (es) ;
//	if ( version > ADSKIcosahedron::kCurrentVersionNumber )
//		return (Acad::eMakeMeProxy) ;
//	//- Uncomment the 2 following lines if your current object implementation cannot
//	//- support previous version of that object.
//	//if ( version < ADSKIcosahedron::kCurrentVersionNumber )
//	//	return (Acad::eMakeMeProxy) ;
//	//----- Read params
//	//pFiler->readItem(&m_dEdgeLength);
//	for (int i : std::views::iota(0, 12)) {
//		AcGePoint3d pt;
//		pFiler->readItem(&pt.x);
//		pFiler->readItem(&pt.y);
//		pFiler->readItem(&pt.z);
//		m_aVertices.at(i) = pt;
//	}
//
//	return (pFiler->filerStatus ()) ;
//}
//

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


//Adesk::UInt32 ADSKIcosahedron::subSetAttributes (AcGiDrawableTraits *traits) {
//	assertReadEnabled () ;
//	return (AcDbEntity::subSetAttributes (traits)) ;
//}

Acad::ErrorStatus ADSKIcosahedron::subTransformBy(const AcGeMatrix3d& xform)
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

double ADSKIcosahedron::volume() const noexcept
{
	// https://en.wikipedia.org/wiki/Regular_icosahedron#Mensuration
	return 5.0* std::pow(std::numbers::phi_v<double>, 2.0) / 6.0*std::pow(m_dEdgeLength,3.0);
}

const AcGePoint3dArray& ADSKIcosahedron::vertices() const
{
	assertReadEnabled();
	return m_aVertices;
}

Acad::ErrorStatus ADSKIcosahedron::setVertexAt(int aI, AcGePoint3d& arPt)
{
	assertWriteEnabled();
	m_aVertices.at(aI) = arPt;
	return Acad::eOk;
}

void ADSKIcosahedron::updateEdgeLength()
{
	assertWriteEnabled();
	m_dEdgeLength = m_aVertices[0].distanceTo(m_aVertices[1]);
}

void ADSKIcosahedron::calculateVertices() noexcept
{
	assertWriteEnabled();
	double t = m_dEdgeLength / 2.0;
	double s = m_dEdgeLength * std::numbers::phi_v<double> / 2.0;
	if (m_aVertices.length() > 0)
		m_aVertices.removeAll();
	m_aVertices.appendList(AcGePoint3d(m_ptCenter.x -t, m_ptCenter.y, m_ptCenter.z+ s), AcGePoint3d(m_ptCenter.x + t, m_ptCenter.y, m_ptCenter.z+ s), AcGePoint3d(m_ptCenter.x -t, m_ptCenter.y, m_ptCenter.z -s), AcGePoint3d(m_ptCenter.x+ t, m_ptCenter.y, m_ptCenter.z -s),
		AcGePoint3d(m_ptCenter.x, m_ptCenter.y+ s, m_ptCenter.z+ t), AcGePoint3d(m_ptCenter.x, m_ptCenter.y+ s, m_ptCenter.z -t), AcGePoint3d(m_ptCenter.x, m_ptCenter.y -s, m_ptCenter.z+ t), AcGePoint3d(m_ptCenter.x, m_ptCenter.y -s, m_ptCenter.z -t),
		AcGePoint3d(m_ptCenter.x+ s, m_ptCenter.y+ t, m_ptCenter.z), AcGePoint3d(m_ptCenter.x -s, m_ptCenter.y+ t, m_ptCenter.z), AcGePoint3d(m_ptCenter.x+ s, m_ptCenter.y -t, m_ptCenter.z), AcGePoint3d(m_ptCenter.x -s, m_ptCenter.y -t, m_ptCenter.z));
	
}

double ADSKIcosahedron::circumradius(double adEdgeLenght) noexcept
{
	// https://en.wikipedia.org/wiki/Regular_icosahedron#Mensuration
	return std::sqrt(std::pow(std::numbers::phi_v<double>, 2.0)+1.0)/2.0*adEdgeLenght;
}

double ADSKIcosahedron::edgeLengthByCircumradius(double adCircumsphereRadius) noexcept
{
	// https://en.wikipedia.org/wiki/Regular_icosahedron#Mensuration
	return 2.0 * adCircumsphereRadius / std::sqrt(std::pow(std::numbers::phi_v<double>, 2.0) +1.0);
}

Acad::ErrorStatus ADSKIcosahedron::setFaceColor(Adesk::Int32 aI, short anColor)
{
	assertWriteEnabled();
	//static_assert(aI > 0 && aI < 20);
	m_faceDataManager.colors()[aI] = anColor;
	return Acad::eOk;
}

Acad::ErrorStatus ADSKIcosahedron::setCenter(const AcGePoint3d& aptCenter)
{
	assertWriteEnabled();
	m_ptCenter = aptCenter;
	calculateVertices();
	return Acad::eOk;
}

const AcGePoint3d& ADSKIcosahedron::center() const
{
	assertReadEnabled();
	return m_ptCenter;
}

double ADSKIcosahedron::edgeLength() const {
	assertReadEnabled();
	return m_dEdgeLength;
}
Acad::ErrorStatus ADSKIcosahedron::setEdgeLength(double adEdgeLenght) {
	assertWriteEnabled();
	m_dEdgeLength = adEdgeLenght;
	calculateVertices();
	return Acad::eOk;
}
