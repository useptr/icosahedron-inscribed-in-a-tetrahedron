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
//-----------------------------------------------------------------------------
#define szRDS _RXST("ADSK")

//-----------------------------------------------------------------------------
//----- ObjectARX EntryPoint
class CicosahedroninscribedinatetrahedronApp : public AcRxArxApp {

public:
	CicosahedroninscribedinatetrahedronApp () : AcRxArxApp () {}

	virtual AcRx::AppRetCode On_kInitAppMsg (void *pkt) {
		// TODO: Load dependencies here

		// You *must* call On_kInitAppMsg here
		AcRx::AppRetCode retCode =AcRxArxApp::On_kInitAppMsg (pkt) ;
		
		// TODO: Add your initialization code here

		return (retCode) ;
	}

	virtual AcRx::AppRetCode On_kUnloadAppMsg (void *pkt) {
		// TODO: Add your code here

		// You *must* call On_kUnloadAppMsg here
		AcRx::AppRetCode retCode =AcRxArxApp::On_kUnloadAppMsg (pkt) ;

		// TODO: Unload dependencies here

		return (retCode) ;
	}

	virtual void RegisterServerComponents () {
	}
	
    static void ADSKMyGroup_CREATE(void) // createIcosahedron
    {
        

        AcDbBlockTableRecordPointer pSpaceRecord(acdbHostApplicationServices()->workingDatabase()->currentSpaceId(), AcDb::kForWrite);
        if (pSpaceRecord.openStatus() != Acad::eOk) {
            return;
        }

        auto phi = std::numbers::phi_v<double>;

        const double X = 0.525731112119133606;
        const double Z = 0.850650808352039932;
        
        AcGePoint3d vertices[12] = {
            AcGePoint3d(),
        }
        //AcGePoint3d vertices[12] = {
        //    AcGePoint3d(-X, 0, Z), AcGePoint3d(X, 0, Z), AcGePoint3d(-X, 0, -Z), AcGePoint3d(X, 0, -Z),
        //    AcGePoint3d(0, Z, X), AcGePoint3d(0, Z, -X), AcGePoint3d(0, -Z, X), AcGePoint3d(0, -Z, -X),
        //    AcGePoint3d(Z, X, 0), AcGePoint3d(-Z, X, 0), AcGePoint3d(Z, -X, 0), AcGePoint3d(-Z, -X, 0)
        //};

        
        int faces[20][3] = {
            {0, 4, 1}, {0, 9, 4}, {9, 5, 4}, {4, 5, 8}, {4, 8, 1},
            {8, 10, 1}, {8, 3, 10}, {5, 3, 8}, {5, 2, 3}, {2, 7, 3},
            {7, 10, 3}, {7, 6, 10}, {7, 11, 6}, {11, 0, 6}, {0, 1, 6},
            {6, 1, 10}, {9, 0, 11}, {9, 11, 2}, {9, 2, 5}, {7, 2, 11}
        };

        for (int i = 0; i < 20; ++i)
        {
            // Создание треугольной грани
            AcDbFace* pFace = new AcDbFace(vertices[faces[i][0]], vertices[faces[i][1]], vertices[faces[i][2]]);
            AcDbObjectId solidId;
            if (pSpaceRecord->appendAcDbEntity(solidId, pFace) != Acad::eOk)
            {
                delete pFace;
                return;
            }
            pFace->close();
        }

        //double length = 10; // TODO replace for 1.0 and use scale
        //std::array < AcGePoint3d,4> vertices{
        //    AcGePoint3d(-length / 2.0,-std::sqrt(3.0)/6.0*length,0),
        //    AcGePoint3d(length / 2.0,-std::sqrt(3.0) / 6.0 * length,0),
        //    AcGePoint3d(0,std::sqrt(3.0)/3.0* length,0),
        //    AcGePoint3d(0,0,std::sqrt(33.0)/6.0* length)
        //};
        //std::array<std::array<int, 3>, 4> faces{
        //    std::array<int, 3>{0, 1, 2},
        //    std::array<int, 3>{0, 1, 3},
        //    std::array<int, 3>{0, 2, 3},
        //    std::array<int, 3>{1, 3, 2}
        //};

        
                
        //for (const auto& face : faces) {
        //    
        //    AcDbFace* pFace = new AcDbFace(vertices[face[0]], vertices[face[1]], vertices[face[2]], Adesk::kTrue);
        //    AcDbObjectId objId;
        //    pSpaceRecord->appendAcDbEntity(objId, pFace);
        //    pFace->close();
        //}
        

    }
} ;

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CicosahedroninscribedinatetrahedronApp)

ACED_ARXCOMMAND_ENTRY_AUTO(CicosahedroninscribedinatetrahedronApp, ADSKMyGroup, _CREATE, CREATE, ACRX_CMD_TRANSPARENT, NULL)

