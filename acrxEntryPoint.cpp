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
#include "ADSKCustomPyramid.h"
#include "ADSKEditorReactor.h"
#include "ADSKIcosahedron.h"
#include "ADSKPyramidJig.h"
#include "ADSKPyramidReactor.h"
#include "ADSKTetrahedron.h"
#include "resource.h"
#include <array>
#include <cmath>
#include <numbers>
#include <ranges>
#include <vector>
//-----------------------------------------------------------------------------
#define szRDS _RXST("ADSK")

std::unique_ptr<ADSKEditorReactor> g_pEditorReactor;
std::unique_ptr<ADSKPyramidReactor> g_pPyramidReactor;

//-----------------------------------------------------------------------------
//----- ObjectARX EntryPoint
class CicosahedroninscribedinatetrahedronApp : public AcRxArxApp {

public:
  CicosahedroninscribedinatetrahedronApp() : AcRxArxApp() {}

  virtual AcRx::AppRetCode On_kInitAppMsg(void *pkt) {
    // TODO: Load dependencies here

    // You *must* call On_kInitAppMsg here
    AcRx::AppRetCode retCode = AcRxArxApp::On_kInitAppMsg(pkt);
    AcRxObject *pSvc;
    if (!(pSvc = acrxServiceDictionary->at(ASDKICOSAHEDRON_DBXSERVICE)) ||
        !(pSvc = acrxServiceDictionary->at(ASDKPOLYHEDRON_DBXSERVICE)) ||
        !(pSvc = acrxServiceDictionary->at(ASDKPOLYHEDRON_DBXSERVICE))) {
      // Try to load the module, if it is not yet present
      if (!acrxDynamicLinker->loadModule(_T("ADSKTetrahedron.dbx"), 0)) {
        acutPrintf(_T("\nERROR: Unable to load ADSKTetrahedron.dbx. Unloading ")
                   _T("this application..."));
        return (AcRx::kRetError);
      }
    }
    // TODO: Add your initialization code here
    try {
      g_pEditorReactor = std::move(std::make_unique<ADSKEditorReactor>(true));
      g_pPyramidReactor = std::move(std::make_unique<ADSKPyramidReactor>());
    } catch (std::bad_alloc &) {
    }

    return (retCode);
  }
  virtual AcRx::AppRetCode On_kLoadDwgMsg(void *pkt) {
    AcRx::AppRetCode retCode = AcRxArxApp::On_kLoadDwgMsg(pkt);
    // Create a new instance of  the database reactor for every new drawing
    try {
      DocVars.docData().m_pDatabaseReactor = new ADSKDatabaseReactor(
          acdbHostApplicationServices()->workingDatabase());
    } catch (std::bad_alloc &) {
      acutPrintf(
          _T("\nERROR: Memory allocation failure for ADSKDatabaseReactor"));
    }
    return (retCode);
  }

  virtual AcRx::AppRetCode On_kUnloadAppMsg(void *pkt) {
    // TODO: Add your code here

    // You *must* call On_kUnloadAppMsg here
    AcRx::AppRetCode retCode = AcRxArxApp::On_kUnloadAppMsg(pkt);

    // TODO: Unload dependencies here
    detachAllTetrahedronWithInscribedIcosahedronReactors();
    g_pEditorReactor.release();
    g_pPyramidReactor.release();

    return (retCode);
  }

  virtual void RegisterServerComponents() {}
  static void ADSKMyGroup_TETRAHEDRON(void) {
    try {
      auto *pDB = acdbHostApplicationServices()->workingDatabase();
      if (nullptr == pDB) {
        acutPrintf(_T("\nERROR: Cannot obtain workingDatabase"));
        return;
      }
      AcDbBlockTableRecordPointer pSpaceBlockTableRecord(pDB->currentSpaceId(),
                                                         AcDb::kForWrite);
      if (pSpaceBlockTableRecord.openStatus() != Acad::eOk) {
        acutPrintf(_T("\nERROR: Cannot open BlockTableRecord for write"));
        return;
      }
      AcDbObjectPointer<ADSKTetrahedron> pTetrahedron;
      pTetrahedron.create();
      AcDbObjectId idObj;
      if (pSpaceBlockTableRecord->appendAcDbEntity(idObj, pTetrahedron) !=
          Acad::eOk) {
        acutPrintf(_T("\nERROR: Cannot append ADSKTetrahedron to BlockTable"));
        return;
      }
    } catch (std::bad_alloc &) {
      acutPrintf(
          _T("\nERROR: Memory allocation failure for ADSKCustomPyramid"));
    } catch (...) {
      acutPrintf(_T("\nERROR: aborting command TETRAHEDRON"));
    }
  }
  static void ADSKMyGroup_ICOSAHEDRON(void) {
    try {
      auto *pDB = acdbHostApplicationServices()->workingDatabase();
      if (nullptr == pDB) {
        acutPrintf(_T("\nERROR: Cannot obtain workingDatabase"));
        return;
      }
      AcDbBlockTableRecordPointer pSpaceBlockTableRecord(pDB->currentSpaceId(),
                                                         AcDb::kForWrite);
      if (pSpaceBlockTableRecord.openStatus() != Acad::eOk) {
        acutPrintf(_T("\nERROR: Cannot open BlockTableRecord for write"));
        return;
      }
      AcDbObjectPointer<ADSKIcosahedron> pIcosahedron;
      pIcosahedron.create();
      AcDbObjectId idObj;
      auto es = pSpaceBlockTableRecord->appendAcDbEntity(idObj, pIcosahedron);
      if (es != Acad::eOk) {
        acutPrintf(
            _T("\nERROR: Cannot append ADSKIcosahedron to BlockTable: %s"),
            acadErrorStatusText(es));
        return;
      }
    } catch (std::bad_alloc &) {
      acutPrintf(
          _T("\nERROR: Memory allocation failure for ADSKCustomPyramid"));
    } catch (...) {
      acutPrintf(_T("\nERROR: aborting command ICOSAHEDRON"));
    }
  }
  static void ADSKMyGroup_CREATE(void) {
    try {
      auto *pDB = acdbHostApplicationServices()->workingDatabase();
      if (nullptr == pDB) {
        acutPrintf(_T("\nERROR: Cannot obtain workingDatabase"));
        return;
      }
      AcDbBlockTableRecordPointer pSpaceBlockTableRecord(pDB->currentSpaceId(),
                                                         AcDb::kForWrite);
      if (pSpaceBlockTableRecord.openStatus() != Acad::eOk) {
        acutPrintf(_T("\nERROR: Cannot open BlockTableRecord for write"));
        return;
      }
      AcDbObjectPointer<ADSKCustomPyramid> pEntity;
      pEntity.create();
      AcDbObjectId idObj;
      if (pSpaceBlockTableRecord->appendAcDbEntity(idObj, pEntity) !=
          Acad::eOk) {
        acutPrintf(_T("\nERROR: Cannot append ")
                   _T("ADSKTetrahedronWithInscribedIcosahedron to BlockTable"));
        return;
      }
      acutPrintf(_T("\nVolume is equal to %f"), pEntity->volumesDifference());
      acutPrintf(_T("\nCREATED ADSKTetrahedronWithInscribedIcosahedron!"));
    } catch (std::bad_alloc &) {
      acutPrintf(
          _T("\nERROR: Memory allocation failure for ADSKCustomPyramid"));
    } catch (...) {
      acutPrintf(_T("\nERROR: aborting command CREATE"));
    }
  }
  static void ADSKMyGroup_JIGCREATE(void) {
    auto ptCenter = AcGePoint3d::kOrigin;
    if (acedGetPoint(nullptr, _T("\nPick figure center point: "),
                     asDblArray(ptCenter)) != RTNORM)
      return;
    try {
      ADSKPyramidJig entityJig(ptCenter);
      entityJig.startJig(new ADSKCustomPyramid);
    } catch (std::bad_alloc &) {
      acutPrintf(
          _T("\nERROR: Memory allocation failure for ADSKCustomPyramid"));
    } catch (...) {
      acutPrintf(_T("\nERROR: aborting command JIGCREATE"));
    }
  }

#ifdef _DEBUG
  static void ADSKMyGroup_RUN_ALL_TESTS(void) {
    ADSKTetrahedron tetrahedron;
    if (!tetrahedron.runTests()) {
      acutPrintf(_T("\nERROR: ADSKTetrahedron tests failes"));
      return;
    }
    ADSKIcosahedron icosahedron;
    if (!icosahedron.runTests()) {
      acutPrintf(_T("\nERROR: ADSKIcosahedron tests failes"));
      return;
    }
    ADSKCustomPyramid pyramid;
    if (!pyramid.runTests()) {
      acutPrintf(_T("\nERROR: ADSKCustomPyramid tests failes"));
      return;
    }
    acutPrintf(_T("\nAll tests passed"));
  }
  static void ADSKMyGroup_TEST_CREATE(void) {
    auto *pDB = acdbHostApplicationServices()->workingDatabase();
    if (nullptr == pDB) {
      acutPrintf(_T("\nERROR: Cannot obtain workingDatabase"));
      return;
    }
    AcDbBlockTableRecordPointer pSpaceRecord(pDB->currentSpaceId(),
                                             AcDb::kForWrite);
    if (pSpaceRecord.openStatus() != Acad::eOk) {
      return;
    }
    AcDbObjectPointer<ADSKCustomPyramid> pEntity;
    pEntity.create();
    AcDbObjectId objId;
    if (pSpaceRecord->appendAcDbEntity(objId, pEntity) != Acad::eOk) {
      acutPrintf(_T("\nERROR: Cannot append ")
                 _T("ADSKTetrahedronWithInscribedIcosahedron to BlockTable"));
      return;
    }

    auto &aTetrahedronVertices = pEntity->tetrahedronVertices();
    for (int i = 0; i < aTetrahedronVertices.length(); ++i) {
      auto &vertex = aTetrahedronVertices[i];
      wchar_t buf[24];
      acutSPrintf(buf, _T("%d"), i);
      AcDbObjectPointer<AcDbText> pText;
      pText.create();
      pText->setPosition(vertex);
      pText->setTextString(buf);
      pText->setHeight(0.02);
      pSpaceRecord->appendAcDbEntity(objId, pText);
    }

    auto &aIcosahedronVertices = pEntity->icosahedronVertices();
    for (int i = 0; i < aIcosahedronVertices.length(); ++i) {
      auto &vertex = aIcosahedronVertices[i];
      wchar_t buf[24];
      acutSPrintf(buf, _T("%d"), i);
      AcDbObjectPointer<AcDbText> pText;
      pText.create();
      pText->setPosition(vertex);
      pText->setTextString(buf);
      pText->setHeight(0.02);
      pSpaceRecord->appendAcDbEntity(objId, pText);
    }
  }
#endif // !_DEBUG
};

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CicosahedroninscribedinatetrahedronApp)

#ifdef _DEBUG
ACED_ARXCOMMAND_ENTRY_AUTO(CicosahedroninscribedinatetrahedronApp, ADSKMyGroup,
                           _RUN_ALL_TESTS, RUN_ALL_TESTS, ACRX_CMD_TRANSPARENT,
                           NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CicosahedroninscribedinatetrahedronApp, ADSKMyGroup,
                           _TEST_CREATE, TEST_CREATE, ACRX_CMD_TRANSPARENT,
                           NULL)
#endif // !_DEBUG
ACED_ARXCOMMAND_ENTRY_AUTO(CicosahedroninscribedinatetrahedronApp, ADSKMyGroup,
                           _ICOSAHEDRON, ICOSAHEDRON, ACRX_CMD_TRANSPARENT,
                           NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CicosahedroninscribedinatetrahedronApp, ADSKMyGroup,
                           _TETRAHEDRON, TETRAHEDRON, ACRX_CMD_TRANSPARENT,
                           NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CicosahedroninscribedinatetrahedronApp, ADSKMyGroup,
                           _CREATE, CREATE, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CicosahedroninscribedinatetrahedronApp, ADSKMyGroup,
                           _JIGCREATE, JIGCREATE, ACRX_CMD_MODAL, NULL)
