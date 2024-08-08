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
//----- ADSKIcosahedron.h : Declaration of the ADSKIcosahedron
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
#include "Tchar.h"
#include "AcGiFaceDataManager.h"
#include "ADSKTetrahedron.h"
#define ASDKICOSAHEDRON_DBXSERVICE _T("ASDKICOSAHEDRON_DBXSERVICE")
//-----------------------------------------------------------------------------
class ADSKCustomPyramid;
class DLLIMPEXP ADSKIcosahedron : public AcDbEntity {

public:
	ACRX_DECLARE_MEMBERS(ADSKIcosahedron);

protected:
	static Adesk::UInt32 kCurrentVersionNumber;

public:
	friend class ADSKCustomPyramid;
	ADSKIcosahedron();
	ADSKIcosahedron(AcGePoint3d aptCenter, double adEdgeLength);
	virtual ~ADSKIcosahedron();

	//----- AcDbObject protocols
	//- Dwg Filing protocol
	virtual Acad::ErrorStatus dwgOutFields(AcDbDwgFiler* pFiler) const;
	virtual Acad::ErrorStatus dwgInFields(AcDbDwgFiler* pFiler);

	//----- AcDbEntity protocols
	//- Graphics protocol
protected:
	virtual Adesk::Boolean subWorldDraw(AcGiWorldDraw* mode);
	//virtual Adesk::UInt32 subSetAttributes (AcGiDrawableTraits *traits) ;
	virtual Acad::ErrorStatus subTransformBy(const AcGeMatrix3d& xform);
	//- Osnap points protocol
public:
	//- Grip points protocol
	virtual Acad::ErrorStatus subGetGripPoints(
		AcDbGripDataPtrArray& grips, const double curViewUnitSize, const int gripSize,
		const AcGeVector3d& curViewDir, const int bitflags) const;
	virtual Acad::ErrorStatus subMoveGripPointsAt(const AcDbVoidPtrArray& gripAppData, const AcGeVector3d& offset, const int bitflags);

	// for Icosahedron
	/*!
		  \brief Вычисляет длину ребра икосаэдра по радиусу описанной сферы
		  \param[in] adCircumsphereRadius радиус описанной сферы икосаэдра
		  \return Длину ребра икосаэдра
	*/
	static double circumradius(double adEdgeLenght) noexcept;
	static double edgeLengthByCircumradius(double adCircumsphereRadius) noexcept;
	Acad::ErrorStatus setFaceColor(Adesk::Int32 aI, short anColor);
	Acad::ErrorStatus setCenter(const AcGePoint3d& aptCenter);
	const AcGePoint3d& center() const;
	double edgeLength() const;
	Acad::ErrorStatus setEdgeLength(double adEdgeLenght);
	/*!
		  \brief Вычисляет объём тетраэдра
		  \details https://en.wikipedia.org/wiki/Regular_icosahedron#Mensuration
		  \return Объём тетраэдра
	*/
	double volume() const noexcept;
	const AcGePoint3dArray& vertices() const;
	/*!
			\brief Вычисляет координаты вершин вписанного в тетраэдра икосаэдра по координатам тетраэдра
			\details Чтобы получить координаты вершин вписанного икосаэдра из тетраэдра, нужно разделить его ребра на золотое сечение. Полученные точки сметрично расположены от каждой вершины тетраэдра. В каждом треугольнике тетраэдра строятся 3 цевиана (от каждой вершины до точки разделившей сторону на золотое сечение). Пересечение цевианов дает нам грань икосаэдра. Source: https://veraviana.net/enclosing/inside-the-tetrahedron/.
			\throw std::invalid_argument в случае некоректного тетраэдра (неправильное количество или расположение вершин)
	*/
	void calculateVertices(ADSKTetrahedron& arTetrahedron);
private:
	static AcGePoint3dArray divideByGoldenRatio(const AcGePoint3dArray& arTetrahedronVertices, const std::vector<std::pair<int, int>>& arEdgesPointsIndexes);
	static AcGePoint3d divideByGoldenRatio(AcGePoint3d& aptA, AcGePoint3d& aptB);
	/*!
		  \details Обновляет длину ребер тетраэдра, вызывается в методе subTransformBy
	*/
	void updateEdgeLength() noexcept;
	/*!
		  \details Вычисляет координаты точек икосаэдра по длине ребра граней и координате центра фигуры
	*/
	void calculateVertices() noexcept;

	double m_dEdgeLength;
	AcGePoint3d m_ptCenter;
	AcGePoint3dArray m_aVertices;
	AcGiFaceDataManager m_faceDataManager;
};

#ifdef TETRAHEDRON_MODULE
ACDB_REGISTER_OBJECT_ENTRY_AUTO(ADSKIcosahedron)
#endif

//#if _DEBUG
// namespace {
//static_assert(adEdgeLenght > 0, "Edge lenght must be positive");
//static_assert(adCircumsphereRadius > 0, "Circumradius must be positive");
//static_assert(aI > 0 && aI < 20, "Face index must be in range [0, 20)");
// } // namespace
//#endif // !_DEBUG