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
//----- ADSKEditorReactor.cpp : Implementation of ADSKEditorReactor
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ADSKEditorReactor.h"
#include "Tchar.h"
//-----------------------------------------------------------------------------
ADSKEditorReactor::ADSKEditorReactor(const bool autoInitAndRelease) : AcEditorReactor(), mbAutoInitAndRelease(autoInitAndRelease) {
	if (autoInitAndRelease) {
		if (acedEditor)
			acedEditor->addReactor(this);
		else
			mbAutoInitAndRelease = false;
	}
}

//-----------------------------------------------------------------------------
ADSKEditorReactor::~ADSKEditorReactor() {
	Detach();
}

//-----------------------------------------------------------------------------
void ADSKEditorReactor::Attach() {
	Detach();
	if (!mbAutoInitAndRelease) {
		if (acedEditor) {
			acedEditor->addReactor(this);
			mbAutoInitAndRelease = true;
		}
	}
}

void ADSKEditorReactor::Detach() {
	if (mbAutoInitAndRelease) {
		if (acedEditor) {
			acedEditor->removeReactor(this);
			mbAutoInitAndRelease = false;
		}
	}
}

AcEditor* ADSKEditorReactor::Subject() const {
	return (acedEditor);
}

bool ADSKEditorReactor::IsAttached() const {
	return (mbAutoInitAndRelease);
}
void ADSKEditorReactor::commandWillStart(const ACHAR* aszCmdStr)
{
	if (_tcscmp(aszCmdStr, _T("STRETCH")) && _tcscmp(aszCmdStr, _T("SCALE")) && _tcscmp(aszCmdStr, _T("GRIP_STRETCH"))) {
		return;
	}
	DocVars.docData().m_bScaleCommand = true;
	DocVars.docData().m_bDoRepainting = false;
	DocVars.docData().m_aChangedObjects.setLogicalLength(0);
}

void ADSKEditorReactor::commandEnded(const ACHAR* aszCmdStr)
{
	if (!DocVars.docData().m_bScaleCommand)
		return;
	DocVars.docData().m_bScaleCommand = false;
	DocVars.docData().m_bDoRepainting = true;

	for (auto& objId : DocVars.docData().m_aChangedObjects) {
		AcDbEntityPointer pEntity(objId, AcDb::kForWrite);
		if (pEntity.openStatus() != Acad::eOk)
			continue;

		auto* pTetrahedronWithInscribedIcosahedron = ADSKCustomPyramid::cast(pEntity);
		if (nullptr == pTetrahedronWithInscribedIcosahedron)
			continue;

		pTetrahedronWithInscribedIcosahedron->setFaceOfIcosahedronToRandomColor();
	}

}
