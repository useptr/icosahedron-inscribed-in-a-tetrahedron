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
//----- ADSKTetrahedronWithInscribedIcosahedronReactor.cpp : Implementation of
//ADSKTetrahedronWithInscribedIcosahedronReactor
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ADSKPyramidReactor.h"

//-----------------------------------------------------------------------------
ACRX_CONS_DEFINE_MEMBERS(ADSKPyramidReactor, AcDbObjectReactor, 1)

//-----------------------------------------------------------------------------
ADSKPyramidReactor::ADSKPyramidReactor() : AcDbObjectReactor() {}

//-----------------------------------------------------------------------------
ADSKPyramidReactor::~ADSKPyramidReactor() {}
void ADSKPyramidReactor::openedForModify(const AcDbObject *pDbObj) {
  if (DocVars.docData().m_bDoRepainting)
    return;
  if (!DocVars.docData().m_bScaleCommand)
    return;

  auto *pEntity = ADSKCustomPyramid::cast(pDbObj);
  if (nullptr == pEntity)
    return;
  // pDbObj->objectId()
  DocVars.docData().m_aChangedObjects.append(pDbObj->objectId());
}
