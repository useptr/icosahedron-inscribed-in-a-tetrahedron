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
//----- ADSKTetrahedronWithInscribedIcosahedron.cpp : Implementation of
// ADSKTetrahedronWithInscribedIcosahedron
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "ADSKCustomPyramid.h"
#include "Tchar.h"
#include "utilities.h"
#include <algorithm>
#include <cmath>
#include <numbers>
#include <random>
#include <ranges>
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
ADSKCustomPyramid::ADSKCustomPyramid()
    : ADSKCustomPyramid(
          AcGePoint3d(0.0, 0.0, ADSKTetrahedron::height(1.0) / 2.0), 1.0) {}

ADSKCustomPyramid::ADSKCustomPyramid(AcGePoint3d aptCenter, double adEdgeLength)
    : AcDbEntity(), m_Tetrahedron(aptCenter, adEdgeLength),
      m_Icosahedron(icosahedronCenter(), icosahedronEdgeLength(adEdgeLength)),
      m_ptBottomFaceCenter(bottomFaceCenterPoint()) {
  try {
    m_Icosahedron.calculateVertices(m_Tetrahedron);
  } catch (const std::invalid_argument &) {
    acutPrintf(_T("\nERROR: ADSKCustomPyramid an invalid cube was passed"));
  }
}

AcGePoint3d ADSKCustomPyramid::bottomFaceCenterPoint() const noexcept {
  assertReadEnabled();
  auto ptCenter = m_Tetrahedron.center();
  return AcGePoint3d(ptCenter.x, ptCenter.y,
                     ptCenter.z - m_Tetrahedron.height() / 2.0);
}

AcGePoint3d ADSKCustomPyramid::icosahedronCenter() const noexcept {
  assertReadEnabled();
  auto ptCenter = m_Tetrahedron.center();
  return AcGePoint3d(ptCenter.x, ptCenter.y,
                     ptCenter.z - m_Tetrahedron.height() / 2.0 +
                         m_Tetrahedron.inradius());
}

ADSKCustomPyramid::~ADSKCustomPyramid() {}

//-----------------------------------------------------------------------------
//----- AcDbObject protocols
//- Dwg Filing protocol
Acad::ErrorStatus ADSKCustomPyramid::dwgOutFields(AcDbDwgFiler *pFiler) const {
  assertReadEnabled();
  //----- Save parent class information first.
  Acad::ErrorStatus es = AcDbEntity::dwgOutFields(pFiler);
  if (es != Acad::eOk)
    return (es);
  //----- Object version number needs to be saved first
  if ((es = pFiler->writeUInt32(ADSKCustomPyramid::kCurrentVersionNumber)) !=
      Acad::eOk)
    return (es);
  //----- Output params

  es = pFiler->writePoint3d(m_Tetrahedron.center());
  if (es != Acad::eOk)
    return es;
  es = pFiler->writeItem(m_Tetrahedron.edgeLength());
  if (es != Acad::eOk)
    return es;

  return (pFiler->filerStatus());
}

Acad::ErrorStatus ADSKCustomPyramid::dwgInFields(AcDbDwgFiler *pFiler) {
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
  //- Uncomment the 2 following lines if your current object implementation
  // cannot
  //- support previous version of that object.
  // if ( version <
  // ADSKTetrahedronWithInscribedIcosahedron::kCurrentVersionNumber ) 	return
  //(Acad::eMakeMeProxy) ;
  //----- Read params

  AcGePoint3d ptCenter;
  es = pFiler->readPoint3d(&ptCenter);
  if (es != Acad::eOk)
    return es;
  double edgelength;
  es = pFiler->readItem(&edgelength);
  if (es != Acad::eOk)
    return es;

  setEdgeLength(edgelength);
  setCenter(ptCenter);

  return (pFiler->filerStatus());
}

//- Dxf Filing protocol
Acad::ErrorStatus ADSKCustomPyramid::dxfOutFields(AcDbDxfFiler *pFiler) const {
  assertReadEnabled();
  //----- Save parent class information first.
  Acad::ErrorStatus es = AcDbEntity::dxfOutFields(pFiler);
  if (es != Acad::eOk)
    return (es);
  es = pFiler->writeItem(AcDb::kDxfSubclass, _RXST("ADSKCustomPyramid"));
  if (es != Acad::eOk)
    return (es);
  //----- Object version number needs to be saved first
  if ((es = pFiler->writeUInt32(
           kDxfInt32, ADSKCustomPyramid::kCurrentVersionNumber)) != Acad::eOk)
    return (es);
  //----- Output params
  auto pt = m_Tetrahedron.center();
  es = pFiler->writeItem(AcDb::kDxfReal, pt.x);
  if (es != Acad::eOk)
    return es;
  es = pFiler->writeItem(AcDb::kDxfReal + 1, pt.y);
  if (es != Acad::eOk)
    return es;
  es = pFiler->writeItem(AcDb::kDxfReal + 2, pt.z);
  if (es != Acad::eOk)
    return es;
  es = pFiler->writeItem(AcDb::kDxfReal + 3, m_Tetrahedron.edgeLength());
  if (es != Acad::eOk)
    return es;

  return (pFiler->filerStatus());
}

Acad::ErrorStatus ADSKCustomPyramid::dxfInFields(AcDbDxfFiler *pFiler) {
  assertWriteEnabled();
  //----- Read parent class information first.
  Acad::ErrorStatus es = AcDbEntity::dxfInFields(pFiler);
  if (es != Acad::eOk || !pFiler->atSubclassData(_RXST("ADSKCustomPyramid")))
    return (pFiler->filerStatus());
  //----- Object version number needs to be read first
  struct resbuf rb;
  pFiler->readItem(&rb);
  if (rb.restype != AcDb::kDxfInt32) {
    pFiler->pushBackItem();
    pFiler->setError(Acad::eInvalidDxfCode,
                     _RXST("\nError: expected group code %d (version #)"),
                     AcDb::kDxfInt32);
    return (pFiler->filerStatus());
  }
  Adesk::UInt32 version = (Adesk::UInt32)rb.resval.rlong;
  if (version > ADSKCustomPyramid::kCurrentVersionNumber)
    return (Acad::eMakeMeProxy);
  //- Uncomment the 2 following lines if your current object implementation
  // cannot
  //- support previous version of that object.
  // if ( version <
  // ADSKTetrahedronWithInscribedIcosahedron::kCurrentVersionNumber ) 	return
  //(Acad::eMakeMeProxy) ;
  //----- Read params in non order dependant manner
  AcGePoint3d ptCenter;
  double edgeLength;
  while (es == Acad::eOk && (es = pFiler->readResBuf(&rb)) == Acad::eOk) {
    switch (rb.restype) {
    case (AcDb::kDxfReal):
      ptCenter.x = rb.resval.rreal;
      break;
    case (AcDb::kDxfReal + 1):
      ptCenter.y = rb.resval.rreal;
      break;
    case (AcDb::kDxfReal + 2):
      ptCenter.z = rb.resval.rreal;
      break;
    case (AcDb::kDxfReal + 3):
      edgeLength = rb.resval.rreal;
      break;

    default:
      //----- An unrecognized group. Push it back so that the subclass can read
      // it again.
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

  setEdgeLength(edgeLength);
  setCenter(ptCenter);

  return (pFiler->filerStatus());
}

//-----------------------------------------------------------------------------
//----- AcDbEntity protocols
Adesk::Boolean ADSKCustomPyramid::subWorldDraw(AcGiWorldDraw *mode) {
  assertReadEnabled();
  try {
    auto status{Adesk::kTrue};
    AcCmTransparency transparency(0.5);
    mode->subEntityTraits().setTransparency(transparency);

    status = m_Tetrahedron.subWorldDraw(mode);
    if (status != Adesk::kTrue)
      return status;

    transparency.setAlphaPercent(1.0);
    mode->subEntityTraits().setTransparency(transparency);

    status = m_Icosahedron.subWorldDraw(mode);
    if (status != Adesk::kTrue)
      return status;
  } catch (...) {
    return Adesk::kFalse;
  }
  return Adesk::kTrue;
}

Adesk::UInt32 ADSKCustomPyramid::subSetAttributes(AcGiDrawableTraits *traits) {
  assertReadEnabled();
  return (AcDbEntity::subSetAttributes(traits));
}

Acad::ErrorStatus ADSKCustomPyramid::setFaceOfIcosahedronToRandomColor() {
  assertWriteEnabled();
  // TODO create global class which returns a number in the range or static
  // function
  static std::default_random_engine engine;
  static std::uniform_int_distribution<short> dist(0, 19); // [a,b]
  short nColor{getRandomColor()};
  Adesk::Int32 i{dist(engine)};
  return m_Icosahedron.setFaceColor(i, nColor);
}

double ADSKCustomPyramid::volumesDifference() const noexcept {
  assertReadEnabled();
  return m_Tetrahedron.volume() - m_Icosahedron.volume();
}
// Acad::ErrorStatus
// ADSKTetrahedronWithInscribedIcosahedron::subGetTransformedCopy(const
// AcGeMatrix3d& xform, AcDbEntity*& ent) const
//{
//	assertReadEnabled();
//	auto* pEntityCopy = new
// ADSKTetrahedronWithInscribedIcosahedron(m_Tetrahedron.center(),
// m_Tetrahedron.edgeLength()); 	pEntityCopy->subTransformBy(xform);
// ent = pEntityCopy; 	return Acad::eOk;
//}

Acad::ErrorStatus ADSKCustomPyramid::subTransformBy(const AcGeMatrix3d &xform) {
  assertWriteEnabled();
  auto es{Acad::eOk};
  es = m_Icosahedron.subTransformBy(xform);
  if (es != Acad::eOk)
    return es;
  es = m_Tetrahedron.subTransformBy(xform);
  if (es != Acad::eOk)
    return es;
  m_ptBottomFaceCenter.transformBy(xform);
  return Acad::eOk;
}

Acad::ErrorStatus ADSKCustomPyramid::subGetGripPoints(
    AcDbGripDataPtrArray &grips, const double curViewUnitSize,
    const int gripSize, const AcGeVector3d &curViewDir,
    const int bitflags) const {
  assertReadEnabled();
  try {
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
  } catch (std::bad_alloc &) {
    return Acad::eOutOfMemory;
  }
  return Acad::eOk;
}

Acad::ErrorStatus
ADSKCustomPyramid::subMoveGripPointsAt(const AcDbVoidPtrArray &gripAppData,
                                       const AcGeVector3d &offset,
                                       const int bitflags) {
  assertWriteEnabled();
  for (auto pGripAppData : gripAppData) {
    auto pGripData = static_cast<GripAppData *>(pGripAppData);
    if (pGripData == nullptr) {
      continue;
    }
    switch (pGripData->index()) {
    case 0: // 0-3 tetrahedron corner points
    case 1:
    case 2:
    case 3: {
      auto es = subTransformBy(
          AcGeMatrix3d::scaling(offset.length(), m_Tetrahedron.center()));
      if (es != Acad::eOk)
        return es;
      break;
    }
    case 4: // botoom face center point
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

double ADSKCustomPyramid::icosahedronEdgeLength(
    double adTetrahedronEdgeLength) const noexcept {
  // https://en.wikipedia.org/wiki/Tetrahedron
  // https://en.wikipedia.org/wiki/Regular_icosahedron#Mensuration
  double dTetrahedronInsphereRadius{
      ADSKTetrahedron::inradius(adTetrahedronEdgeLength)};
  return ADSKIcosahedron::edgeLengthByCircumradius(dTetrahedronInsphereRadius);
}

Acad::ErrorStatus ADSKCustomPyramid::setEdgeLength(const double adEdgeLength) {
  assertWriteEnabled();
  m_Tetrahedron.setEdgeLength(adEdgeLength);
  m_Icosahedron.calculateVertices(m_Tetrahedron);
  m_ptBottomFaceCenter = bottomFaceCenterPoint();
  return Acad::eOk;
}

Acad::ErrorStatus ADSKCustomPyramid::setCenter(const AcGePoint3d &aptCenter) {
  assertWriteEnabled();
  auto es = m_Tetrahedron.setCenter(aptCenter);
  if (Acad::eOk != es)
    return es;
  m_ptBottomFaceCenter = bottomFaceCenterPoint();
  m_Icosahedron.calculateVertices(m_Tetrahedron);
  return Acad::eOk;
}

const AcGePoint3d &ADSKCustomPyramid::center() const {
  assertReadEnabled();
  return m_Tetrahedron.center();
}

const AcGePoint3dArray &ADSKCustomPyramid::vertices() const {
  assertReadEnabled();
  return m_Icosahedron.vertices();
}

#ifndef _DEBUG
bool ADSKCustomPyramid::runTests() const {
  // check that the points that make up the faces of the icosahedron lie on the
  // planes of the tetrahedron
  auto createPlane = [](const AcGePoint3d &pt1, const AcGePoint3d &pt2,
                        const AcGePoint3d &pt3) -> AcGePlane {
    auto v1 = pt2 - pt1;
    auto v2 = pt3 - pt1;
    auto normal = v1.crossProduct(v2);
    return AcGePlane(pt1, normal);
  };
  auto aTetrahedronVertices = m_Tetrahedron.vertices();
  auto aIcosahedronVertices = m_Icosahedron.vertices();
  AcGePlane bottomPlane =
      createPlane(aTetrahedronVertices[0], aTetrahedronVertices[1],
                  aTetrahedronVertices[2]);
  if (!bottomPlane.isOn(aIcosahedronVertices[0]) ||
      !bottomPlane.isOn(aIcosahedronVertices[1]) ||
      !bottomPlane.isOn(aIcosahedronVertices[2]))
    return false;
  AcGePlane frontPlane =
      createPlane(aTetrahedronVertices[0], aTetrahedronVertices[1],
                  aTetrahedronVertices[3]);
  if (!frontPlane.isOn(aIcosahedronVertices[3]) ||
      !frontPlane.isOn(aIcosahedronVertices[4]) ||
      !frontPlane.isOn(aIcosahedronVertices[5]))
    return false;
  AcGePlane backLeftPlane =
      createPlane(aTetrahedronVertices[0], aTetrahedronVertices[2],
                  aTetrahedronVertices[3]);
  if (!backLeftPlane.isOn(aIcosahedronVertices[9]) ||
      !backLeftPlane.isOn(aIcosahedronVertices[10]) ||
      !backLeftPlane.isOn(aIcosahedronVertices[11]))
    return false;
  AcGePlane rightBackPlane =
      createPlane(aTetrahedronVertices[1], aTetrahedronVertices[2],
                  aTetrahedronVertices[3]);
  if (!rightBackPlane.isOn(aIcosahedronVertices[6]) ||
      !rightBackPlane.isOn(aIcosahedronVertices[7]) ||
      !rightBackPlane.isOn(aIcosahedronVertices[8]))
    return false;
  return true;
}
const AcGePoint3dArray &ADSKCustomPyramid::tetrahedronVertices() const {
  return m_Tetrahedron.vertices();
}
const AcGePoint3dArray &ADSKCustomPyramid::icosahedronVertices() const {
  return m_Icosahedron.vertices();
}
#endif // !_DEBUG