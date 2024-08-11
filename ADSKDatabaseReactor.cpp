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
//----- ADSKDatabaseReactor.cpp : Implementation of ADSKDatabaseReactor
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ADSKDatabaseReactor.h"
#include "ADSKPyramidReactor.h"
extern std::unique_ptr<ADSKPyramidReactor> g_pPyramidReactor;
//-----------------------------------------------------------------------------
ACRX_CONS_DEFINE_MEMBERS(ADSKDatabaseReactor, AcDbDatabaseReactor, 1)

//-----------------------------------------------------------------------------
ADSKDatabaseReactor::ADSKDatabaseReactor(AcDbDatabase *pDb)
    : AcDbDatabaseReactor(), mpDatabase(pDb) {
  if (pDb)
    pDb->addReactor(this);
}

//-----------------------------------------------------------------------------
ADSKDatabaseReactor::~ADSKDatabaseReactor() { Detach(); }

//-----------------------------------------------------------------------------
void ADSKDatabaseReactor::Attach(AcDbDatabase *pDb) {
  Detach();
  if (mpDatabase == NULL) {
    if ((mpDatabase = pDb) != NULL)
      pDb->addReactor(this);
  }
}

void ADSKDatabaseReactor::Detach() {
  if (mpDatabase) {
    mpDatabase->removeReactor(this);
    mpDatabase = NULL;
  }
}

AcDbDatabase *ADSKDatabaseReactor::Subject() const { return (mpDatabase); }

bool ADSKDatabaseReactor::IsAttached() const { return (mpDatabase != NULL); }

void ADSKDatabaseReactor::objectAppended(const AcDbDatabase *,
                                         const AcDbObject *apDbObj) {
  auto *pEntity = ADSKCustomPyramid::cast(apDbObj);
  if (nullptr == pEntity)
    return;
  apDbObj->addReactor(g_pPyramidReactor.get());
}
