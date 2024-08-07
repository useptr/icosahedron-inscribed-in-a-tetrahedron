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
//----- ADSKCustomPyramid.h : Declaration of the ADSKTetrahedronWithInscribedIcosahedron
//-----------------------------------------------------------------------------
#pragma once

#ifndef DLLIMPEXP

#ifdef TETRAHEDRON_MODULE
#define DLLIMPEXP __declspec(dllexport)
#else
//----- Note: we don't use __declspec(dllimport) here, because of the
//----- "local vtable" problem with msvc. If you use __declspec(dllimport),
//----- then, when a client dll does a new on the class, the object's
//----- vtable pointer points to a vtable allocated in that client
//----- dll. If the client dll then passes the object to another dll,
//----- and the client dll is then unloaded, the vtable becomes invalid
//----- and any virtual calls on the object will access invalid memory.
//-----
//----- By not using __declspec(dllimport), we guarantee that the
//----- vtable is allocated in the server dll during the ctor and the
//----- client dll does not overwrite the vtable pointer after calling
//----- the ctor. And, since we expect the server dll to remain in
//----- memory indefinitely, there is no problem with vtables unexpectedly
//----- going away.
#define DLLIMPEXP
#endif

#endif // !DLLIMPEXP

//-----------------------------------------------------------------------------
#include "dbmain.h"
#include "ADSKIcosahedron.h"
#include "ADSKTetrahedron.h"
//-----------------------------------------------------------------------------
/*!
	\brief �������� � ��������� ����������
*/
class DLLIMPEXP ADSKCustomPyramid : public AcDbEntity {

public:
	ACRX_DECLARE_MEMBERS(ADSKCustomPyramid);

protected:
	static Adesk::UInt32 kCurrentVersionNumber;

public:
	ADSKCustomPyramid();
	/*!
		  \details ����������� � ����� ����������� ��� ������
		  \param[in] aptCenter ���������� ������ ������
		  \param[in] adEdgeLength ����� ���� ������
	*/
	ADSKCustomPyramid(AcGePoint3d aptCenter, double adEdgeLength);
	virtual ~ADSKCustomPyramid();

	//----- AcDbObject protocols
	//- Dwg Filing protocol
	virtual Acad::ErrorStatus dwgOutFields(AcDbDwgFiler* pFiler) const;
	virtual Acad::ErrorStatus dwgInFields(AcDbDwgFiler* pFiler);

	//- Dxf Filing protocol
	virtual Acad::ErrorStatus dxfOutFields(AcDbDxfFiler* pFiler) const;
	virtual Acad::ErrorStatus dxfInFields(AcDbDxfFiler* pFiler);

	//----- AcDbEntity protocols
	//- Graphics protocol
protected:
	virtual Adesk::Boolean subWorldDraw(AcGiWorldDraw* mode);
	virtual Adesk::UInt32 subSetAttributes(AcGiDrawableTraits* traits);
	virtual Acad::ErrorStatus subTransformBy(const AcGeMatrix3d& xform);
	//- Osnap points protocol
public:
	virtual Acad::ErrorStatus subGetOsnapPoints(
		AcDb::OsnapMode osnapMode,
		Adesk::GsMarker gsSelectionMark,
		const AcGePoint3d& pickPoint,
		const AcGePoint3d& lastPoint,
		const AcGeMatrix3d& viewXform,
		AcGePoint3dArray& snapPoints,
		AcDbIntArray& geomIds) const;
	virtual Acad::ErrorStatus subGetOsnapPoints(
		AcDb::OsnapMode osnapMode,
		Adesk::GsMarker gsSelectionMark,
		const AcGePoint3d& pickPoint,
		const AcGePoint3d& lastPoint,
		const AcGeMatrix3d& viewXform,
		AcGePoint3dArray& snapPoints,
		AcDbIntArray& geomIds,
		const AcGeMatrix3d& insertionMat) const;
	//- Grip points protocol
	virtual Acad::ErrorStatus subGetGripPoints(
		AcDbGripDataPtrArray& grips, const double curViewUnitSize, const int gripSize,
		const AcGeVector3d& curViewDir, const int bitflags) const;
	virtual Acad::ErrorStatus subMoveGripPointsAt(const AcDbVoidPtrArray& gripAppData, const AcGeVector3d& offset, const int bitflags);
	//virtual Acad::ErrorStatus   subGetStretchPoints(
	//	AcGePoint3dArray& stretchPoints) const override;
	//virtual Acad::ErrorStatus   subMoveStretchPointsAt(const AcDbIntArray& indices,
	//	const AcGeVector3d& offset) override;

	//virtual Acad::ErrorStatus subGetGeomExtents( AcDbExtents& extents) const;
	//virtual Acad::ErrorStatus subGetTransformedCopy( const AcGeMatrix3d& xform, AcDbEntity*& ent) const;

	// methods for working with ADSKTetrahedronWithInscribedIcosahedron
	/*!
		  \brief ������ ���� ��������� ����� ���������� ��������� �� ���� �� ����������� ������ AutoCAD'� (1-6)
		  \return Acad::eOk � ������ ��������� ����������, ����� ������ ������ ���� Acad::ErrorStatus
	*/
	Acad::ErrorStatus setFaceOfIcosahedronToRandomColor();
	/*!
		  \brief ��������� ������� ������� ��������� � ���������� ���������
		  \return ������� ������� ��������� � ���������� ���������
	*/
	double volumesDifference() const noexcept;
	/*!
		  \details ������������� ����� ����� ���� ��� ���������, ������������� ����� ���� ���������� ��������� � ����� �������� ����������� ����� � ���������
		  \param[in] adEdgeLenght ����� ����� ���������
		  \return Acad::eOk � ������ ��������� ����������, ����� ������ ������ ���� Acad::ErrorStatus
	*/
	Acad::ErrorStatus setEdgeLength(const double adEdgeLength);
	/*!
		  \brief ������������� ����� ���������� ������ ������
		  \param[in] adEdgeLenght ���������� ������ ������
		  \return Acad::eOk � ������ ��������� ����������, ����� ������ ������ ���� Acad::ErrorStatus
	*/
	Acad::ErrorStatus setCenter(const AcGePoint3d& aptCenter);
	/*!
		  \brief ���������� ���������� ������ ������
		  \return ���������� ������ ������
	*/
	const AcGePoint3d& center() const;
private:
	/*!
		  \brief ��������� ���������� m_ptBottomFaceCenter
		  \return ���������� m_ptBottomFaceCenter
	*/
	AcGePoint3d bottomFaceCenterPoint() const noexcept;
	/*!
		  \brief ��������� ���������� ������ ���������� ���������
		  \return ���������� ������ ���������� ���������
	*/
	AcGePoint3d icosahedronCenter() const noexcept;
	/*!
		  \brief ��������� ����� ����� ���������� ��������� �� ����� ����� ���������
		  \param[in] adEdgeLenght ����� ����� ���������
		  \return ����� ����� ���������� ��������� ���������
	*/
	double icosahedronEdgeLength(double adTetrahedronEdgeLenght) const noexcept;
	
	ADSKTetrahedron m_Tetrahedron; ///< ��������
	ADSKIcosahedron m_Icosahedron; ///< ��������� ��������
	AcGePoint3d m_ptBottomFaceCenter; ///< ����� �� ������ ����� ��������� ��� ����������� ������
	//AcGeMatrix3d m_transform; ///< ������� �������������� (TRS matrix)
	//AcGeMatrix3d m_translation;
	//AcGeMatrix3d m_scaling;
	//AcGeMatrix3d m_rotation;
};

#ifdef TETRAHEDRON_MODULE
ACDB_REGISTER_OBJECT_ENTRY_AUTO(ADSKCustomPyramid)
#endif
