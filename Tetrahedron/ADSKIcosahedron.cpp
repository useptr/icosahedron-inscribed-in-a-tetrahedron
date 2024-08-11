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
#include "stdafx.h"
#include "ADSKIcosahedron.h"
#include <cmath>
#include <memory>
#include <numbers>
#include <stdexcept>
#include "utilities.h"
#include <ranges>
#include <utility>
//-----------------------------------------------------------------------------
Adesk::UInt32 ADSKIcosahedron::kCurrentVersionNumber = 1;

//-----------------------------------------------------------------------------
ACRX_DXF_DEFINE_MEMBERS(
	ADSKIcosahedron, AcDbEntity,
	AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent,
	AcDbProxyEntity::kNoOperation, ADSKICOSAHEDRON,
	ADSKPOLYHEDRONSAPP
	| Product Desc : A description for your object
	| Company : Your company name
	| WEB Address : Your company WEB site address
)

//-----------------------------------------------------------------------------
ADSKIcosahedron::ADSKIcosahedron()
    : ADSKIcosahedron(AcGePoint3d::kOrigin, 1.0) {}

ADSKIcosahedron::ADSKIcosahedron(AcGePoint3d aptCenter, double adEdgeLength)
    : AcDbEntity(), m_dEdgeLength(adEdgeLength), m_ptCenter(aptCenter) {
  calculateVertices();
  try {
    m_faceDataManager.setColors(std::move(std::vector<short>(20, 0)));
  } catch (std::bad_alloc &) {
  }
}

ADSKIcosahedron::~ADSKIcosahedron() {}

//-----------------------------------------------------------------------------
//----- AcDbObject protocols
//- Dwg Filing protocol
Acad::ErrorStatus ADSKIcosahedron::dwgOutFields(AcDbDwgFiler *pFiler) const {
  assertReadEnabled();
  //----- Save parent class information first.
  Acad::ErrorStatus es = AcDbEntity::dwgOutFields(pFiler);
  if (es != Acad::eOk)
    return (es);
  //----- Object version number needs to be saved first
  if ((es = pFiler->writeUInt32(ADSKIcosahedron::kCurrentVersionNumber)) !=
      Acad::eOk)
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

Acad::ErrorStatus ADSKIcosahedron::dwgInFields(AcDbDwgFiler *pFiler) {
  assertWriteEnabled();
  //----- Read parent class information first.
  Acad::ErrorStatus es = AcDbEntity::dwgInFields(pFiler);
  if (es != Acad::eOk)
    return (es);
  //----- Object version number needs to be read first
  Adesk::UInt32 version = 0;
  if ((es = pFiler->readUInt32(&version)) != Acad::eOk)
    return (es);
  if (version > ADSKIcosahedron::kCurrentVersionNumber)
    return (Acad::eMakeMeProxy);
  //- Uncomment the 2 following lines if your current object implementation
  // cannot
  //- support previous version of that object.
  // if ( version < ADSKIcosahedron::kCurrentVersionNumber )
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
Adesk::Boolean ADSKIcosahedron::subWorldDraw(AcGiWorldDraw *mode) {
  assertReadEnabled();
  try {
    Adesk::UInt32 faceListSize = 4 * 20;
    // Массив индексов вершин для каждой грани икосаэдра, метод shell из
    // AcGiGeometry принимает сначала количество вершин для грани, затем индексы
    // самих вершин
    static const Adesk::Int32 faceList[]{
        3, 0, 1, 2, // icosahedron face on bottom tetrahedron face
        3, 0, 1, 7, 3, 0, 2, 4, 3, 0, 4, 6, 3, 0, 6, 7, 3, 1, 2, 11, 3, 1, 7, 9,
        3, 1, 9, 11, 3, 2, 3, 4, 3, 2, 3, 11, 3, 3, 4, 5, // icosahedron
                                                          // face on
                                                          // front
                                                          // tetrahedron
                                                          // face
        3, 3, 5, 10, 3, 3, 10, 11, 3, 4, 5, 6, 3, 5, 6, 8, 3, 5, 8, 10, 3, 6, 7,
        8, // icosahedron face on right-back tetrahedron facc
        3, 7, 8, 9, 3, 8, 9, 10, 3, 9, 10, 11 // icosahedron face on back-left
                                              // tetrahedron facc
    };
    mode->subEntityTraits().setColor(1);
    auto status = mode->geometry().shell(
        m_aVertices.length(), m_aVertices.asArrayPtr(), faceListSize, faceList,
        nullptr, m_faceDataManager.faceData());
    if (status != Adesk::kTrue)
      return status;
  } catch (...) {
    return Adesk::kFalse;
  }
  return Adesk::kTrue;
}

Acad::ErrorStatus ADSKIcosahedron::subTransformBy(const AcGeMatrix3d &xform) {
  assertWriteEnabled();
  for (auto &vertex : m_aVertices) {
    vertex.transformBy(xform);
  }
  m_ptCenter.transformBy(xform);
  updateEdgeLength();
  return Acad::eOk;
}

Acad::ErrorStatus ADSKIcosahedron::subGetGripPoints(
    AcDbGripDataPtrArray &grips, const double curViewUnitSize,
    const int gripSize, const AcGeVector3d &curViewDir,
    const int bitflags) const {
  assertReadEnabled();
  try {
    auto pGripData = new AcDbGripData();
    pGripData->setGripPoint(m_ptCenter);
    pGripData->setAppData(new GripAppData(0));
    grips.append(pGripData);
  } catch (std::bad_alloc &) {
    return Acad::eOutOfMemory;
  }
  return Acad::eOk;
}

Acad::ErrorStatus
ADSKIcosahedron::subMoveGripPointsAt(const AcDbVoidPtrArray &gripAppData,
                                     const AcGeVector3d &offset,
                                     const int bitflags) {
  assertWriteEnabled();
  for (auto pGripAppData : gripAppData) {
    auto pGripData = static_cast<GripAppData *>(pGripAppData);
    if (pGripData == nullptr) {
      continue;
    }
    switch (pGripData->index()) {
    case 0: // index of center point
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

double ADSKIcosahedron::volume() const noexcept {
  // https://en.wikipedia.org/wiki/Regular_icosahedron#Mensuration
  assertReadEnabled();
  return 5.0 * std::pow(std::numbers::phi_v<double>, 2.0) / 6.0 *
         std::pow(m_dEdgeLength, 3.0);
}

const AcGePoint3dArray &ADSKIcosahedron::vertices() const {
  assertReadEnabled();
  return m_aVertices;
}

void ADSKIcosahedron::updateEdgeLength() noexcept {
  assertWriteEnabled();
  m_dEdgeLength = m_aVertices[0].distanceTo(m_aVertices[1]);
}

void ADSKIcosahedron::calculateVertices() noexcept {
  assertWriteEnabled();
  double t = m_dEdgeLength / 2.0;
  double s = m_dEdgeLength * std::numbers::phi_v<double> / 2.0;
  if (m_aVertices.length() > 0)
    m_aVertices.removeAll();
  m_aVertices.append(
      AcGePoint3d(m_ptCenter.x - t, m_ptCenter.y, m_ptCenter.z + s)); // 0
  m_aVertices.append(
      AcGePoint3d(m_ptCenter.x + t, m_ptCenter.y, m_ptCenter.z + s)); // 1
  m_aVertices.append(
      AcGePoint3d(m_ptCenter.x, m_ptCenter.y + s, m_ptCenter.z + t)); // 2
  m_aVertices.append(
      AcGePoint3d(m_ptCenter.x, m_ptCenter.y + s, m_ptCenter.z - t)); // 5
  m_aVertices.append(
      AcGePoint3d(m_ptCenter.x - s, m_ptCenter.y + t, m_ptCenter.z)); // 4
  m_aVertices.append(
      AcGePoint3d(m_ptCenter.x - t, m_ptCenter.y, m_ptCenter.z - s)); // 5
  m_aVertices.append(
      AcGePoint3d(m_ptCenter.x - s, m_ptCenter.y - t, m_ptCenter.z)); // 6
  m_aVertices.append(
      AcGePoint3d(m_ptCenter.x, m_ptCenter.y - s, m_ptCenter.z + t)); // 7
  m_aVertices.append(
      AcGePoint3d(m_ptCenter.x, m_ptCenter.y - s, m_ptCenter.z - t)); // 8
  m_aVertices.append(
      AcGePoint3d(m_ptCenter.x + s, m_ptCenter.y - t, m_ptCenter.z)); // 9
  m_aVertices.append(
      AcGePoint3d(m_ptCenter.x + t, m_ptCenter.y, m_ptCenter.z - s)); // 10
  m_aVertices.append(
      AcGePoint3d(m_ptCenter.x + s, m_ptCenter.y + t, m_ptCenter.z)); // 11
}

void ADSKIcosahedron::calculateVertices(ADSKTetrahedron &arTetrahedron) {
  assertWriteEnabled();
  // TODO сделать вычисления более очевидными
  auto aTetrahedronVertices = arTetrahedron.vertices();
  if (aTetrahedronVertices.length() != 4)
    throw std::invalid_argument("Invalid Tetrahedron");
  if (m_aVertices.length() > 0)
    m_aVertices.setLogicalLength(0);

  auto addIntersections =
      [&](AcArray<AcGeLineSeg3d>
              &aCevians) { // добавляет в массив вершин икосаэдра точки
                           // пересечения цевианов
        for (int curI = 0; curI < 3;
             ++curI) { // находит точки пересечения 3 цевианов
          int nextI =
              (curI + 1) %
              3; // перебираем по порядку все цевианы (0 и 1), (1 и 2), (2 и 0)
          AcGePoint3d resultPoint;
          aCevians[curI].intersectWith(
              aCevians[nextI], resultPoint); // находим точку пересечения цевиа
          m_aVertices.append(
              resultPoint); // точки пересечения цевианов это вершины икосаэдра
        }
      };

  { // bottom tetrahedron face
    const std::vector<std::pair<int, int>> aEdges{
        {1, 0},
        {0, 2},
        {2, 1}}; // Индексы точек рёбер описывающие одну из граней икосаэдра
    AcGePoint3dArray aDividedVertices = divideByGoldenRatio(
        aTetrahedronVertices,
        aEdges); // Делим ребра на золотое сечение. Получаем точки сметрично
                 // расположеные от каждой вершины тетраэдра
    AcArray<AcGeLineSeg3d> aCevians;
    aCevians.appendList(
        AcGeLineSeg3d(aTetrahedronVertices[0], aDividedVertices[2]),
        AcGeLineSeg3d(aTetrahedronVertices[1], aDividedVertices[1]),
        AcGeLineSeg3d(aTetrahedronVertices[2],
                      aDividedVertices[0])); // получаем цевианы, соединяя
                                             // вершины тетраэдра с точками
                                             // полученными в aDividedVertices
    addIntersections(aCevians);
  }
  { // front tetrahedron face
    const std::vector<std::pair<int, int>> aEdges{{0, 1}, {1, 3}, {3, 0}};
    AcGePoint3dArray aDividedVertices =
        divideByGoldenRatio(aTetrahedronVertices, aEdges);
    AcArray<AcGeLineSeg3d> aCevians;
    aCevians.appendList(
        AcGeLineSeg3d(aTetrahedronVertices[0], aDividedVertices[1]),
        AcGeLineSeg3d(aTetrahedronVertices[1], aDividedVertices[2]),
        AcGeLineSeg3d(aTetrahedronVertices[3], aDividedVertices[0]));
    addIntersections(aCevians);
  }
  { // right-back tetrahedron face
    const std::vector<std::pair<int, int>> aEdges{{1, 2}, {2, 3}, {3, 1}};
    AcGePoint3dArray aDividedVertices =
        divideByGoldenRatio(aTetrahedronVertices, aEdges);
    AcArray<AcGeLineSeg3d> aCevians;
    aCevians.appendList(
        AcGeLineSeg3d(aTetrahedronVertices[1], aDividedVertices[1]),
        AcGeLineSeg3d(aTetrahedronVertices[2], aDividedVertices[2]),
        AcGeLineSeg3d(aTetrahedronVertices[3], aDividedVertices[0]));
    addIntersections(aCevians);
  }
  { // back-left tetrahedron face
    const std::vector<std::pair<int, int>> aEdges{{0, 3}, {3, 2}, {2, 0}};
    AcGePoint3dArray aDividedVertices =
        divideByGoldenRatio(aTetrahedronVertices, aEdges);
    AcArray<AcGeLineSeg3d> aCevians;
    aCevians.appendList(
        AcGeLineSeg3d(aTetrahedronVertices[0], aDividedVertices[1]),
        AcGeLineSeg3d(aTetrahedronVertices[2], aDividedVertices[0]),
        AcGeLineSeg3d(aTetrahedronVertices[3], aDividedVertices[2]));
    addIntersections(aCevians);
  }

  if (m_aVertices.length() != 12)
    throw std::invalid_argument("Invalid Tetrahedron");
  updateEdgeLength();
  // TODO update center
}

AcGePoint3dArray ADSKIcosahedron::divideByGoldenRatio(
    const AcGePoint3dArray &arTetrahedronVertices,
    const std::vector<std::pair<int, int>> &arEdgesPointsIndexes) {
  AcGePoint3dArray verticesDivideInGoldenRatio;
  for (auto &[iA, iB] : arEdgesPointsIndexes) {
    auto A = arTetrahedronVertices[iA];
    auto B = arTetrahedronVertices[iB];
    verticesDivideInGoldenRatio.append(divideByGoldenRatio(A, B));
  }
  return verticesDivideInGoldenRatio;
}

AcGePoint3d ADSKIcosahedron::divideByGoldenRatio(AcGePoint3d &aptA,
                                                 AcGePoint3d &aptB) {
  return aptA + (aptB - aptA) / std::numbers::phi_v<double>;
}

double ADSKIcosahedron::circumradius(double adEdgeLenght) noexcept {
  // https://en.wikipedia.org/wiki/Regular_icosahedron#Mensuration
  return std::sqrt(std::pow(std::numbers::phi_v<double>, 2.0) + 1.0) / 2.0 *
         adEdgeLenght;
}

double ADSKIcosahedron::edgeLengthByCircumradius(
    double adCircumsphereRadius) noexcept {
  // https://en.wikipedia.org/wiki/Regular_icosahedron#Mensuration
  return adCircumsphereRadius * 2.0 /
         std::sqrt(std::pow(std::numbers::phi_v<double>, 2.0) + 1.0);
}

Acad::ErrorStatus ADSKIcosahedron::setFaceColor(Adesk::Int32 aI,
                                                short anColor) {
  assertWriteEnabled();
  try {
    m_faceDataManager.setColor(aI, anColor);
  } catch (const std::out_of_range &) {
    return Acad::eInvalidIndex;
  }
  return Acad::eOk;
}

Acad::ErrorStatus ADSKIcosahedron::setCenter(const AcGePoint3d &aptCenter) {
  assertWriteEnabled();
  m_ptCenter = aptCenter;
  calculateVertices();
  return Acad::eOk;
}

const AcGePoint3d &ADSKIcosahedron::center() const {
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
#ifndef _DEBUG
bool ADSKIcosahedron::runTests() const {
  double dTolerance = AcGeTol().equalPoint();
  // check distance between points
  auto d0 = m_aVertices[0].distanceTo(m_aVertices[1]);
  if (!(std::abs(m_dEdgeLength - m_aVertices[0].distanceTo(m_aVertices[1])) <
        dTolerance))
    return false;
  auto d = m_aVertices[0].distanceTo(m_aVertices[2]);
  auto r = std::abs(m_dEdgeLength - m_aVertices[0].distanceTo(m_aVertices[2]));
  if (!(std::abs(m_dEdgeLength - m_aVertices[0].distanceTo(m_aVertices[2])) <
        dTolerance))
    return false;
  if (!(std::abs(m_dEdgeLength - m_aVertices[0].distanceTo(m_aVertices[4])) <
        dTolerance))
    return false;
  if (!(std::abs(m_dEdgeLength - m_aVertices[0].distanceTo(m_aVertices[6])) <
        dTolerance))
    return false;
  if (!(std::abs(m_dEdgeLength - m_aVertices[0].distanceTo(m_aVertices[7])) <
        dTolerance))
    return false;

  if (!(std::abs(m_dEdgeLength - m_aVertices[5].distanceTo(m_aVertices[3])) <
        dTolerance))
    return false;
  if (!(std::abs(m_dEdgeLength - m_aVertices[5].distanceTo(m_aVertices[4])) <
        dTolerance))
    return false;
  if (!(std::abs(m_dEdgeLength - m_aVertices[5].distanceTo(m_aVertices[6])) <
        dTolerance))
    return false;
  if (!(std::abs(m_dEdgeLength - m_aVertices[5].distanceTo(m_aVertices[8])) <
        dTolerance))
    return false;
  if (!(std::abs(m_dEdgeLength - m_aVertices[5].distanceTo(m_aVertices[10])) <
        dTolerance))
    return false;
  return true;
}
#endif // !_DEBUG