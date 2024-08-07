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
#include <array>
#include <numbers>
#include "ADSKIcosahedron.h"
#include "ADSKTetrahedron.h"
#include "ADSKCustomPyramid.h"
#include "ADSKEditorReactor.h"
//#include "ADSKDatabaseReactor.h"
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
	CicosahedroninscribedinatetrahedronApp () : AcRxArxApp () {}

	virtual AcRx::AppRetCode On_kInitAppMsg (void *pkt) {
		// TODO: Load dependencies here

		// You *must* call On_kInitAppMsg here
		AcRx::AppRetCode retCode =AcRxArxApp::On_kInitAppMsg (pkt) ;
        AcRxObject* pSvc;
        if (!(pSvc = acrxServiceDictionary->at(ASDKICOSAHEDRON_DBXSERVICE)) || !(pSvc = acrxServiceDictionary->at(ASDKPOLYHEDRON_DBXSERVICE)) || !(pSvc = acrxServiceDictionary->at(ASDKPOLYHEDRON_DBXSERVICE)))
        {
            // Try to load the module, if it is not yet present 
            if (!acrxDynamicLinker->loadModule(_T("ADSKTetrahedron.dbx"), 0))
            {
                acutPrintf(_T("Unable to load ADSKTetrahedron.dbx. Unloading this application...\n"));
                return (AcRx::kRetError);
            }
        }
		// TODO: Add your initialization code here
        g_pEditorReactor = std::move(std::make_unique<ADSKEditorReactor>(true));
        g_pPyramidReactor = std::move(std::make_unique<ADSKPyramidReactor>());

		return (retCode) ;
	}
    virtual AcRx::AppRetCode On_kLoadDwgMsg(void* pkt) {
        AcRx::AppRetCode retCode = AcRxArxApp::On_kLoadDwgMsg(pkt);
        // Create a new instance of  the database reactor for every new drawing
        DocVars.docData().m_pDatabaseReactor = new ADSKDatabaseReactor(acdbHostApplicationServices()->workingDatabase());
        return (retCode);
    }

	virtual AcRx::AppRetCode On_kUnloadAppMsg (void *pkt) {
		// TODO: Add your code here

		// You *must* call On_kUnloadAppMsg here
		AcRx::AppRetCode retCode =AcRxArxApp::On_kUnloadAppMsg (pkt) ;

		// TODO: Unload dependencies here
        detachAllTetrahedronWithInscribedIcosahedronReactors();
        g_pEditorReactor.release();
        g_pPyramidReactor.release();

		return (retCode) ;
	}

	virtual void RegisterServerComponents () {
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
            acutPrintf(_T("ERROR: Cannot append ADSKTetrahedron to BlockTable"));
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
            acutPrintf(_T("ERROR: Cannot append ADSKIcosahedron to BlockTable: %s"), acadErrorStatusText(es));
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
            acutPrintf(_T("ERROR: Cannot append ADSKTetrahedronWithInscribedIcosahedron to BlockTable"));
            return;
        }
        acutPrintf(_T("Volume is equal to %f"), pEntity->volumesDifference());
        acutPrintf(_T("CREATED ADSKTetrahedronWithInscribedIcosahedron!"));
        //pDB->setPdmode(66);
        //pDB->setPdsize(10.0);
        //for (auto& vertex : pEntity->vertices()) {
        //    auto pt = new AcDbPoint(vertex);


        //    pt->setColorIndex(1);
        //    pt->setLinetypeScale(2.0);
        //    pt->setLineWeight(AcDb::kLnWt120);
        //    AcDbObjectId idObj;
        //    pSpaceBlockTableRecord->appendAcDbEntity(idObj, pt);
        //}
    }
    static void ADSKMyGroup_JIGCREATE(void) {
        auto ptCenter = AcGePoint3d::kOrigin;
        if (acedGetPoint(nullptr, _T("\nEllipse center point: "), asDblArray(ptCenter)) != RTNORM)
            return;

        ADSKPyramidJig entityJig(ptCenter);
        entityJig.startJig(new ADSKCustomPyramid);

        //auto* pJig
        //    = new ADSKPyramidJig();
        //// Now start up the jig to interactively get the major
        //// and minor axes lengths.
        ////
        //pJig->doIt();
        //// Now delete the jig object, since it is no longer needed.
        ////
        //delete pJig;
    }
    //static void ADSKMyGroup_CREATE(void) // createIcosahedron
    //{
    //    

    //    AcDbBlockTableRecordPointer pSpaceRecord(acdbHostApplicationServices()->workingDatabase()->currentSpaceId(), AcDb::kForWrite);
    //    if (pSpaceRecord.openStatus() != Acad::eOk) {
    //        return;
    //    }
    //    double edgeLength = 5;
    //    auto phi = std::numbers::phi_v<double>;
    //    double t = edgeLength / 2.0;
    //    double s = edgeLength * phi / 2.0;
    //    
    //    AcGePoint3d vertices[12] = {
    //        AcGePoint3d(-t, 0, s), AcGePoint3d(t, 0, s), AcGePoint3d(-t, 0, -s), AcGePoint3d(t, 0, -s),
    //        AcGePoint3d(0, s, t), AcGePoint3d(0, s, -t), AcGePoint3d(0, -s, t), AcGePoint3d(0, -s, -t),
    //        AcGePoint3d(s, t, 0), AcGePoint3d(-s, t, 0), AcGePoint3d(s, -t, 0), AcGePoint3d(-s, -t, 0)
    //   //AcGePoint3d(0,  t,  s),  AcGePoint3d(0,  t, -s),  AcGePoint3d(0, -t,  s),  AcGePoint3d(0, -t, -s),
    //   //AcGePoint3d(t,  s,  0),  AcGePoint3d(t, -s,  0),  AcGePoint3d(-t,  s,  0),  AcGePoint3d(-t, -s,  0),
    //   //AcGePoint3d(s,  0,  t),  AcGePoint3d(s,  0, -t),  AcGePoint3d(-s,  0,  t),  AcGePoint3d(-s,  0, -t)
    //    };

    //    //int faces[20][3] = {
    //    //{0, 8, 4}, {0, 4, 6}, {0, 6, 2}, {0, 2, 10}, {0, 10, 8},
    //    //{1, 8, 4}, {1, 4, 5}, {1, 5, 7}, {1, 7, 3}, {1, 3, 9},
    //    //{1, 9, 8}, {2, 6, 10}, {2, 10, 11}, {2, 11, 3}, {2, 3, 7},
    //    //{3, 11, 9}, {4, 5, 7}, {4, 7, 6}, {5, 7, 11}, {5, 11, 9}
    //    //};

    //    //const double X = 0.525731112119133606;
    //    //const double Z = 0.850650808352039932;
    //    
    //    //AcGePoint3d vertices[12] = {
    //    //    AcGePoint3d(-X, 0, Z), AcGePoint3d(X, 0, Z), AcGePoint3d(-X, 0, -Z), AcGePoint3d(X, 0, -Z),
    //    //    AcGePoint3d(0, Z, X), AcGePoint3d(0, Z, -X), AcGePoint3d(0, -Z, X), AcGePoint3d(0, -Z, -X),
    //    //    AcGePoint3d(Z, X, 0), AcGePoint3d(-Z, X, 0), AcGePoint3d(Z, -X, 0), AcGePoint3d(-Z, -X, 0)
    //    //};

    //    
    //    int faces[20][3] = {
    //        {0, 4, 1}, {0, 9, 4}, {9, 5, 4}, {4, 5, 8}, {4, 8, 1},
    //        {8, 10, 1}, {8, 3, 10}, {5, 3, 8}, {5, 2, 3}, {2, 7, 3},
    //        {7, 10, 3}, {7, 6, 10}, {7, 11, 6}, {11, 0, 6}, {0, 1, 6},
    //        {6, 1, 10}, {9, 0, 11}, {9, 11, 2}, {9, 2, 5}, {7, 2, 11}
    //    };

    //    //AcDbObjectPointer<AcDbPolygonMesh> pPolygonMesh;
    //    //pPolygonMesh.create();

    //    for (int i = 0; i < 20; ++i)
    //    {
    //        // Создание треугольной грани
    //        AcDbFace* pFace = new AcDbFace(vertices[faces[i][0]], vertices[faces[i][1]], vertices[faces[i][2]]);
    //        AcDbObjectId solidId;
    //        if (pSpaceRecord->appendAcDbEntity(solidId, pFace) != Acad::eOk)
    //        {
    //            delete pFace;
    //            return;
    //        }
    //        pFace->close();
    //    }

    //    //double length = 10; // TODO replace for 1.0 and use scale
    //    //std::array < AcGePoint3d,4> vertices{
    //    //    AcGePoint3d(-length / 2.0,-std::sqrt(3.0)/6.0*length,0),
    //    //    AcGePoint3d(length / 2.0,-std::sqrt(3.0) / 6.0 * length,0),
    //    //    AcGePoint3d(0,std::sqrt(3.0)/3.0* length,0),
    //    //    AcGePoint3d(0,0,std::sqrt(33.0)/6.0* length)
    //    //};
    //    //std::array<std::array<int, 3>, 4> faces{
    //    //    std::array<int, 3>{0, 1, 2},
    //    //    std::array<int, 3>{0, 1, 3},
    //    //    std::array<int, 3>{0, 2, 3},
    //    //    std::array<int, 3>{1, 3, 2}
    //    //};

    //    
    //            
    //    //for (const auto& face : faces) {
    //    //    
    //    //    AcDbFace* pFace = new AcDbFace(vertices[face[0]], vertices[face[1]], vertices[face[2]], Adesk::kTrue);
    //    //    AcDbObjectId objId;
    //    //    pSpaceRecord->appendAcDbEntity(objId, pFace);
    //    //    pFace->close();
    //    //}
    //    

    //}
} ;

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CicosahedroninscribedinatetrahedronApp)

ACED_ARXCOMMAND_ENTRY_AUTO(CicosahedroninscribedinatetrahedronApp, ADSKMyGroup, _ICOSAHEDRON, ICOSAHEDRON, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CicosahedroninscribedinatetrahedronApp, ADSKMyGroup, _TETRAHEDRON, TETRAHEDRON, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CicosahedroninscribedinatetrahedronApp, ADSKMyGroup, _CREATE, CREATE, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CicosahedroninscribedinatetrahedronApp, ADSKMyGroup, _JIGCREATE, JIGCREATE, ACRX_CMD_MODAL, NULL)
