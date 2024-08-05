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
//----- DocData.cpp : Implementation file
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ADSKTetrahedronWithInscribedIcosahedronReactor.h"

extern std::unique_ptr <ADSKTetrahedronWithInscribedIcosahedronReactor> g_pTetrahedronWithInscribedIcosahedronReactor;
//-----------------------------------------------------------------------------
//----- The one and only document manager object. You can use the DocVars object to retrieve
//----- document specific data throughout your application
AcApDataManager<CDocData> DocVars ;

// TODO fix very long names
Acad::ErrorStatus attachToAllTetrahedronWithInscribedIcosahedron(bool attach) {
	AcDbDatabase* pWorkingDatabase{ acdbHostApplicationServices()->workingDatabase() };
	if (!pWorkingDatabase)
		return Acad::eNoDatabase;
	AcDbBlockTableRecordPointer pBlockTableRecord(pWorkingDatabase->currentSpaceId());
	AcDbBlockTableRecordIterator* pIt;
	auto es = pBlockTableRecord->newIterator(pIt);
	if (es != Acad::eOk) {
		return es;
	}
	std::unique_ptr<AcDbBlockTableRecordIterator> pIteratorScopedDeleter(pIt);
	for (; !pIt->done(); pIt->step()) {
		AcDbObjectId entityId{ AcDbObjectId::kNull };
		es = pIt->getEntityId(entityId);
		if (es != Acad::eOk)
			continue;
		AcDbObjectPointer<ADSKCustomPyramid> pEntity(entityId);
		if (pEntity.openStatus() != Acad::eOk)
			continue;



		if (attach) {
			acutPrintf(_T("INFO: addReactor\n"));
			pEntity->addReactor(g_pTetrahedronWithInscribedIcosahedronReactor.get());
		}
		else {
			acutPrintf(_T("INFO: removeReactor\n"));
			pEntity->removeReactor(g_pTetrahedronWithInscribedIcosahedronReactor.get());
		}

	}
	return Acad::eOk;
}
void detachAllTetrahedronWithInscribedIcosahedronReactors()
{
	// Get an AcApDocumentIterator
	std::unique_ptr<AcApDocumentIterator> pIt(acDocManager->newAcApDocumentIterator());
	if (pIt == nullptr)
		return;
	// Store a pointer to the current document
	AcApDocument* pCurDoc = acDocManager->curDocument();
	// Iterate through all documents, make each document the current document and call attachEmployeeReactorToAllEmployee(false)
	for (; !pIt->done(); pIt->step()) { // TODO improve
		AcApDocument* pDoc = pIt->document();
		// TODO check errors
		if (pDoc->lockMode() == AcAp::kNone && acDocManager->setCurDocument(pDoc, AcAp::kAutoWrite, Adesk::kFalse) != Acad::eOk) {
			continue;
		}
		acDocManager->setCurDocument(pDoc);
		attachToAllTetrahedronWithInscribedIcosahedron(false);
	}
	// After iterating, reset the current document to be the one that was current before iterating
	acDocManager->setCurDocument(pCurDoc, AcAp::kNone, Adesk::kFalse);
}

//-----------------------------------------------------------------------------
//----- Implementation of the document data class.
CDocData::CDocData () : m_bScaleCommand(false), m_bDoRepainting(false), m_pDatabaseReactor(nullptr) {
	attachToAllTetrahedronWithInscribedIcosahedron(true);
}

//-----------------------------------------------------------------------------
CDocData::CDocData (const CDocData &data) {
}

//-----------------------------------------------------------------------------
CDocData::~CDocData () {
}

