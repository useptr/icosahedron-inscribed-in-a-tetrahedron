// (C) Copyright 2002-2012 by Autodesk, Inc. 
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
//----- acrxEntryPoint.cpp
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "resource.h"
#include <cmath>
#include <ranges>
#include <numbers>
#include <vector>
#include <array>
#include <numbers>
#include "ADSKIcosahedron.h"
#include "ADSKTetrahedron.h"
#include "ADSKCustomPyramid.h"
#include "ADSKEditorReactor.h"
#include "ADSKPyramidReactor.h"
#include "ADSKPyramidJig.h"
//-----------------------------------------------------------------------------
#define szRDS _RXST("ADSK")

std::unique_ptr<ADSKEditorReactor> g_pEditorReactor;
std::unique_ptr <ADSKPyramidReactor> g_pPyramidReactor;

//-----------------------------------------------------------------------------
//----- ObjectARX EntryPoint
class CicosahedroninscribedinatetrahedronApp : public AcRxArxApp {

public:
	CicosahedroninscribedinatetrahedronApp() : AcRxArxApp() {}

	virtual AcRx::AppRetCode On_kInitAppMsg(void* pkt) {
		// TODO: Load dependencies here

		// You *must* call On_kInitAppMsg here
		AcRx::AppRetCode retCode = AcRxArxApp::On_kInitAppMsg(pkt);
		AcRxObject* pSvc;
		if (!(pSvc = acrxServiceDictionary->at(ASDKICOSAHEDRON_DBXSERVICE)) || !(pSvc = acrxServiceDictionary->at(ASDKPOLYHEDRON_DBXSERVICE)) || !(pSvc = acrxServiceDictionary->at(ASDKPOLYHEDRON_DBXSERVICE)))
		{
			// Try to load the module, if it is not yet present 
			if (!acrxDynamicLinker->loadModule(_T("ADSKTetrahedron.dbx"), 0))
			{
				acutPrintf(_T("\nERROR: Unable to load ADSKTetrahedron.dbx. Unloading this application..."));
				return (AcRx::kRetError);
			}
		}
		// TODO: Add your initialization code here
		try {
			g_pEditorReactor = std::move(std::make_unique<ADSKEditorReactor>(true));
			g_pPyramidReactor = std::move(std::make_unique<ADSKPyramidReactor>());
		}
		catch (std::bad_alloc&) {

		}

		return (retCode);
	}
	virtual AcRx::AppRetCode On_kLoadDwgMsg(void* pkt) {
		AcRx::AppRetCode retCode = AcRxArxApp::On_kLoadDwgMsg(pkt);
		// Create a new instance of  the database reactor for every new drawing
		try {
			DocVars.docData().m_pDatabaseReactor = new ADSKDatabaseReactor(acdbHostApplicationServices()->workingDatabase());
		}
		catch (std::bad_alloc&) {
			acutPrintf(_T("\nERROR: Memory allocation failure for ADSKDatabaseReactor"));
		}
		return (retCode);
	}

	virtual AcRx::AppRetCode On_kUnloadAppMsg(void* pkt) {
		// TODO: Add your code here

		// You *must* call On_kUnloadAppMsg here
		AcRx::AppRetCode retCode = AcRxArxApp::On_kUnloadAppMsg(pkt);

		// TODO: Unload dependencies here
		detachAllTetrahedronWithInscribedIcosahedronReactors();
		g_pEditorReactor.release();
		g_pPyramidReactor.release();

		return (retCode);
	}

	virtual void RegisterServerComponents() {
	}
	static void ADSKMyGroup_TETRAHEDRON(void) {
		auto* pDB = acdbHostApplicationServices()->workingDatabase();
		if (nullptr == pDB) {
			acutPrintf(_T("\nERROR: Cannot obtain workingDatabase"));
			return;
		}
		AcDbBlockTableRecordPointer pSpaceBlockTableRecord(pDB->currentSpaceId(), AcDb::kForWrite);
		if (pSpaceBlockTableRecord.openStatus() != Acad::eOk) {
			acutPrintf(_T("\nERROR: Cannot open BlockTableRecord for write"));
			return;
		}
		AcDbObjectPointer<ADSKTetrahedron> pTetrahedron;
		pTetrahedron.create();
		AcDbObjectId idObj;
		if (pSpaceBlockTableRecord->appendAcDbEntity(idObj, pTetrahedron) != Acad::eOk) {
			acutPrintf(_T("\nERROR: Cannot append ADSKTetrahedron to BlockTable"));
			return;
		}
	}
	static void ADSKMyGroup_ICOSAHEDRON(void) {
		auto* pDB = acdbHostApplicationServices()->workingDatabase();
		if (nullptr == pDB) {
			acutPrintf(_T("\nERROR: Cannot obtain workingDatabase"));
			return;
		}
		AcDbBlockTableRecordPointer pSpaceBlockTableRecord(pDB->currentSpaceId(), AcDb::kForWrite);
		if (pSpaceBlockTableRecord.openStatus() != Acad::eOk) {
			acutPrintf(_T("\nERROR: Cannot open BlockTableRecord for write"));
			return;
		}
		AcDbObjectPointer<ADSKIcosahedron> pIcosahedron;
		pIcosahedron.create();
		AcDbObjectId idObj;
		auto es = pSpaceBlockTableRecord->appendAcDbEntity(idObj, pIcosahedron);
		if (es != Acad::eOk) {
			acutPrintf(_T("\nERROR: Cannot append ADSKIcosahedron to BlockTable: %s"), acadErrorStatusText(es));
			return;
		}
	}
	static void ADSKMyGroup_CREATE(void) {
		auto* pDB = acdbHostApplicationServices()->workingDatabase();
		if (nullptr == pDB) {
			acutPrintf(_T("\nERROR: Cannot obtain workingDatabase"));
			return;
		}
		AcDbBlockTableRecordPointer pSpaceBlockTableRecord(pDB->currentSpaceId(), AcDb::kForWrite);
		if (pSpaceBlockTableRecord.openStatus() != Acad::eOk) {
			acutPrintf(_T("\nERROR: Cannot open BlockTableRecord for write"));
			return;
		}
		AcDbObjectPointer<ADSKCustomPyramid> pEntity;
		pEntity.create();
		AcDbObjectId idObj;
		if (pSpaceBlockTableRecord->appendAcDbEntity(idObj, pEntity) != Acad::eOk) {
			acutPrintf(_T("\nERROR: Cannot append ADSKTetrahedronWithInscribedIcosahedron to BlockTable"));
			return;
		}
		acutPrintf(_T("\nVolume is equal to %f"), pEntity->volumesDifference());
		acutPrintf(_T("\nCREATED ADSKTetrahedronWithInscribedIcosahedron!"));
	}
	static void ADSKMyGroup_JIGCREATE(void) {
		auto ptCenter = AcGePoint3d::kOrigin;
		if (acedGetPoint(nullptr, _T("\nPick figure center point: "), asDblArray(ptCenter)) != RTNORM)
			return;
		try {
			ADSKPyramidJig entityJig(ptCenter);		
			entityJig.startJig(new ADSKCustomPyramid);
		}
		catch (std::bad_alloc&) {
			acutPrintf(_T("\nERROR: Memory allocation failure for ADSKCustomPyramid. ADSKPyramidJig "));
		}
		catch (...) {
			acutPrintf(_T("\nERROR: aborting command JIGCREATE"));
		}
	}
#ifdef _DEBUG
	static AcGePoint3d divideInGoldenRatio(AcGePoint3d& aptA, AcGePoint3d& aptB) {
		return aptA + (aptB - aptA) / std::numbers::phi_v<double>;
	}
	static AcGePoint3dArray EdgesPointsDividedInGoldenRatio(const AcGePoint3dArray& TetrahedronVertices, const std::vector<std::pair<int, int>>& edgesPointsIndexes) {
		AcGePoint3dArray verticesDivideInGoldenRatio;
		for (auto& [iA, iB] : edgesPointsIndexes)
		{
			auto A = TetrahedronVertices[iA];
			auto B = TetrahedronVertices[iB];
			auto pt = divideInGoldenRatio(A, B);

			verticesDivideInGoldenRatio.append(pt);
		}
		return verticesDivideInGoldenRatio;
	}
	static void ADSKMyGroup_TEST_CREATE(void) // createIcosahedron
	{

		auto* pDB = acdbHostApplicationServices()->workingDatabase();
		if (nullptr == pDB) {
			acutPrintf(_T("\nERROR: Cannot obtain workingDatabase"));
			return;
		}
		AcDbBlockTableRecordPointer pSpaceRecord(pDB->currentSpaceId(), AcDb::kForWrite);
		if (pSpaceRecord.openStatus() != Acad::eOk) {
			return;
		}
		// double edgeLength = 5;
		// auto phi = std::numbers::phi_v<double>;
		// double t = edgeLength / 2.0;
		// double s = edgeLength * phi / 2.0;
		// 
		// AcGePoint3d vertices[12] = {
		//     AcGePoint3d(-t, 0, s), AcGePoint3d(t, 0, s), AcGePoint3d(-t, 0, -s), AcGePoint3d(t, 0, -s),
		//     AcGePoint3d(0, s, t), AcGePoint3d(0, s, -t), AcGePoint3d(0, -s, t), AcGePoint3d(0, -s, -t),
		//     AcGePoint3d(s, t, 0), AcGePoint3d(-s, t, 0), AcGePoint3d(s, -t, 0), AcGePoint3d(-s, -t, 0)
		////AcGePoint3d(0,  t,  s),  AcGePoint3d(0,  t, -s),  AcGePoint3d(0, -t,  s),  AcGePoint3d(0, -t, -s),
		////AcGePoint3d(t,  s,  0),  AcGePoint3d(t, -s,  0),  AcGePoint3d(-t,  s,  0),  AcGePoint3d(-t, -s,  0),
		////AcGePoint3d(s,  0,  t),  AcGePoint3d(s,  0, -t),  AcGePoint3d(-s,  0,  t),  AcGePoint3d(-s,  0, -t)
		// };
		// 
		// int faces[20][3] = {
		//     {0, 4, 1}, {0, 9, 4}, {9, 5, 4}, {4, 5, 8}, {4, 8, 1},
		//     {8, 10, 1}, {8, 3, 10}, {5, 3, 8}, {5, 2, 3}, {2, 7, 3},
		//     {7, 10, 3}, {7, 6, 10}, {7, 11, 6}, {11, 0, 6}, {0, 1, 6},
		//     {6, 1, 10}, {9, 0, 11}, {9, 11, 2}, {9, 2, 5}, {7, 2, 11}
		// };

		double length = 10;
		AcGePoint3dArray vertices;
		auto m_ptCenter = AcGePoint3d::kOrigin;
		double m_dEdgeLength = 1.0;
		double height = std::sqrt(6.0) / 3.0 * m_dEdgeLength;
		double dHalfHeight = height / 2.0;

		vertices.append(AcGePoint3d(m_ptCenter.x - m_dEdgeLength / 2.0, m_ptCenter.y - std::sqrt(3.0) / 6.0 * m_dEdgeLength, m_ptCenter.z - dHalfHeight));
		vertices.append(AcGePoint3d(m_ptCenter.x + m_dEdgeLength / 2.0, m_ptCenter.y - std::sqrt(3.0) / 6.0 * m_dEdgeLength, m_ptCenter.z - dHalfHeight));
		vertices.append(AcGePoint3d(m_ptCenter.x, m_ptCenter.y + std::sqrt(3.0) / 3.0 * m_dEdgeLength, m_ptCenter.z - dHalfHeight));
		vertices.append(AcGePoint3d(m_ptCenter.x, m_ptCenter.y, m_ptCenter.z + dHalfHeight));

		std::array<std::array<int, 3>, 4> faces{
			std::array<int, 3>{0, 1, 2},
			std::array<int, 3>{0, 1, 3},
			std::array<int, 3>{0, 2, 3},
			std::array<int, 3>{1, 3, 2}
		};

		int cnt = 0;
		for (auto& vertex : vertices) {
			AcDbObjectPointer<AcDbPoint> pPt;
			pPt.create();
			pPt->setColorIndex(cnt);
			pPt->setPosition(vertex);
			AcDbObjectId objId;
			pSpaceRecord->appendAcDbEntity(objId, pPt);
			++cnt;
		}

		for (const auto& face : faces) {
			AcDbFace* pFace = new AcDbFace(vertices[face[0]], vertices[face[1]], vertices[face[2]], Adesk::kTrue);
			AcDbObjectId objId;
			pSpaceRecord->appendAcDbEntity(objId, pFace);
			pFace->close();
		}

		AcGePoint3dArray IcosahedronVertices;
		// bottom
		{
			const std::vector<std::pair<int, int>> edges = {
				{1, 0}, {0, 2}, {2, 1}
			};
			AcGePoint3dArray bottomVerticesDivideInGoldenRatio = EdgesPointsDividedInGoldenRatio(vertices, edges);

			AcArray<AcGeLineSeg3d> aCevians;
			aCevians.appendList(AcGeLineSeg3d(vertices[0], bottomVerticesDivideInGoldenRatio[2]), AcGeLineSeg3d(vertices[1], bottomVerticesDivideInGoldenRatio[1]), AcGeLineSeg3d(vertices[2], bottomVerticesDivideInGoldenRatio[0]));
			// добавляет все точки пересечения цевианов
			for (int curI = 0; curI < 3; ++curI) {
				int nextI = (curI + 1) % 3;
				AcGePoint3d resultPoint;
				aCevians[curI].intersectWith(aCevians[nextI], resultPoint);
				IcosahedronVertices.append(resultPoint);

			}
		}
		// front
		{
			const std::vector<std::pair<int, int>> edges = {
					{0, 1}, {1, 3}, {3, 0}
			};
			AcGePoint3dArray bottomVerticesDivideInGoldenRatio = EdgesPointsDividedInGoldenRatio(vertices, edges);
			AcArray<AcGeLineSeg3d> aCevians;
			aCevians.appendList(AcGeLineSeg3d(vertices[0], bottomVerticesDivideInGoldenRatio[1]), AcGeLineSeg3d(vertices[1], bottomVerticesDivideInGoldenRatio[2]), AcGeLineSeg3d(vertices[3], bottomVerticesDivideInGoldenRatio[0]));

			for (int curI = 0; curI < 3; ++curI) {
				int nextI = (curI + 1) % 3;
				AcGePoint3d resultPoint;
				aCevians[curI].intersectWith(aCevians[nextI], resultPoint);
				IcosahedronVertices.append(resultPoint);

			}
		}

		// right-back
		{
			const std::vector<std::pair<int, int>> edges = {
			{1, 2}, {2, 3}, {3, 1}
			};
			AcGePoint3dArray bottomVerticesDivideInGoldenRatio = EdgesPointsDividedInGoldenRatio(vertices, edges);
			AcArray<AcGeLineSeg3d> aCevians;
			aCevians.appendList(AcGeLineSeg3d(vertices[1], bottomVerticesDivideInGoldenRatio[1]), AcGeLineSeg3d(vertices[2], bottomVerticesDivideInGoldenRatio[2]), AcGeLineSeg3d(vertices[3], bottomVerticesDivideInGoldenRatio[0]));

			for (int curI = 0; curI < 3; ++curI) {
				int nextI = (curI + 1) % 3;
				AcGePoint3d resultPoint;
				aCevians[curI].intersectWith(aCevians[nextI], resultPoint);
				IcosahedronVertices.append(resultPoint);

			}
		}
		// back-left
		{
			const std::vector<std::pair<int, int>> edges = {
			{0, 3}, {3, 2}, {2, 0}
			};
			AcGePoint3dArray bottomVerticesDivideInGoldenRatio = EdgesPointsDividedInGoldenRatio(vertices, edges);
			AcArray<AcGeLineSeg3d> aCevians;
			aCevians.appendList(AcGeLineSeg3d(vertices[0], bottomVerticesDivideInGoldenRatio[1]), AcGeLineSeg3d(vertices[2], bottomVerticesDivideInGoldenRatio[0]), AcGeLineSeg3d(vertices[3], bottomVerticesDivideInGoldenRatio[2]));

			for (int curI = 0; curI < 3; ++curI) {
				int nextI = (curI + 1) % 3;
				AcGePoint3d resultPoint;
				aCevians[curI].intersectWith(aCevians[nextI], resultPoint);
				IcosahedronVertices.append(resultPoint);

			}
		}
		auto edgeLength = vertices[0].distanceTo(vertices[1]);
		acutPrintf(_T("TL1 %f\n"), edgeLength);
		edgeLength = vertices[0].distanceTo(vertices[3]);
		acutPrintf(_T("TL2 %f\n"), edgeLength);
		edgeLength = IcosahedronVertices[0].distanceTo(IcosahedronVertices[1]);
		acutPrintf(_T("LEHGTH1 %f\n"), edgeLength);
		edgeLength = IcosahedronVertices[3].distanceTo(IcosahedronVertices[4]);
		acutPrintf(_T("LEHGTH2 %f\n"), edgeLength);
		acutPrintf(_T("%f\n"), IcosahedronVertices[0].distanceTo(IcosahedronVertices[4]));
		acutPrintf(_T("%f %f\n"), IcosahedronVertices[2].distanceTo(IcosahedronVertices[3]), IcosahedronVertices[2].distanceTo(IcosahedronVertices[4]));
		acutPrintf(_T("%f\n"), IcosahedronVertices[0].distanceTo(IcosahedronVertices[7]));
		Adesk::Int32 faceList[][3]{
			{0, 1, 2}, // icosahedron face on bottom tetrahedron face
			{0, 1, 7},
			{0, 2, 4},
			{0, 4, 6},
			{0, 6, 7},
			{1, 2, 11},
			{1, 7, 9},
			{1, 9, 11},
			{2, 3, 4},
			{2, 3, 11},
			{3, 4, 5}, // icosahedron face on front tetrahedron face
			{3, 5, 10},
			{3, 10, 11},
			{4, 5, 6},
			{5, 6, 8},
			{5, 8, 10},
			{6, 7, 8}, // icosahedron face on right-back tetrahedron face 
			{7, 8, 9},
			{8, 9, 10},
			{9, 10, 11} // icosahedron face on back-left tetrahedron face 
		};
		for (const auto& face : faceList) {

			AcDbFace* pFace = new AcDbFace(IcosahedronVertices[face[0]], IcosahedronVertices[face[1]], IcosahedronVertices[face[2]], Adesk::kTrue);
			AcDbObjectId objId;
			pFace->setColorIndex(1);
			pSpaceRecord->appendAcDbEntity(objId, pFace);
			pFace->close();
		}
		int c = 0;
		int index = 0;

		for (auto& vertex : IcosahedronVertices) {
			AcDbObjectId objId;

			wchar_t buf[24];
			acutSPrintf(buf, _T("%d"), index);
			auto* pText = new AcDbText(vertex, buf, AcDbObjectId::kNull, 0.02);
			pSpaceRecord->appendAcDbEntity(objId, pText);
			pText->close();

			AcDbObjectPointer<AcDbPoint> pPt;
			pPt.create();
			pPt->setColorIndex(c);
			pPt->setPosition(vertex);

			pSpaceRecord->appendAcDbEntity(objId, pPt);
			++c;
			++index;
		}
	}
#endif // !_DEBUG
};

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CicosahedroninscribedinatetrahedronApp)

#ifdef _DEBUG
ACED_ARXCOMMAND_ENTRY_AUTO(CicosahedroninscribedinatetrahedronApp, ADSKMyGroup, _TEST_CREATE, TEST_CREATE, ACRX_CMD_TRANSPARENT, NULL)
#endif // !_DEBUG
ACED_ARXCOMMAND_ENTRY_AUTO(CicosahedroninscribedinatetrahedronApp, ADSKMyGroup, _ICOSAHEDRON, ICOSAHEDRON, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CicosahedroninscribedinatetrahedronApp, ADSKMyGroup, _TETRAHEDRON, TETRAHEDRON, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CicosahedroninscribedinatetrahedronApp, ADSKMyGroup, _CREATE, CREATE, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CicosahedroninscribedinatetrahedronApp, ADSKMyGroup, _JIGCREATE, JIGCREATE, ACRX_CMD_MODAL, NULL)
