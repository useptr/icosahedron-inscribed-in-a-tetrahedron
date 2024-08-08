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
#define ASDKPOLYHEDRON_DBXSERVICE _T("ASDKPOLYHEDRON_DBXSERVICE")
/*!
	\brief Тетраэдр с вписанным икосаэдром
*/
class DLLIMPEXP ADSKCustomPyramid : public AcDbEntity {

public:
	ACRX_DECLARE_MEMBERS(ADSKCustomPyramid);

protected:
	static Adesk::UInt32 kCurrentVersionNumber;

public:
	ADSKCustomPyramid();
	/*!
		  \details Конструктор с двумя параметрами для фигуры
		  \param[in] aptCenter координаты центра фигурв
		  \param[in] adEdgeLength Длина рёбер фигуры
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
public:
	//- Grip points protocol
	virtual Acad::ErrorStatus subGetGripPoints(
		AcDbGripDataPtrArray& grips, const double curViewUnitSize, const int gripSize,
		const AcGeVector3d& curViewDir, const int bitflags) const;
	virtual Acad::ErrorStatus subMoveGripPointsAt(const AcDbVoidPtrArray& gripAppData, const AcGeVector3d& offset, const int bitflags);
	//virtual Acad::ErrorStatus subGetGeomExtents( AcDbExtents& extents) const;
	//virtual Acad::ErrorStatus subGetTransformedCopy( const AcGeMatrix3d& xform, AcDbEntity*& ent) const;

	// ADSKTetrahedronWithInscribedIcosahedron methods 
	/*!
		  \brief Меняет цвет случайной грани вписанного икосаэдра на один из стандартных цветов AutoCAD'а (1-6)
		  \return Acad::eOk в случае успешного выполнения, иначе другую ошибку типа Acad::ErrorStatus
	*/
	Acad::ErrorStatus setFaceOfIcosahedronToRandomColor();
	/*!
		  \brief Вычисляет разницу объемов икосаэдра и вписанного тетраэдра
		  \return Разницу объемов икосаэдра и вписанного тетраэдра
	*/
	double volumesDifference() const noexcept;
	/*!
		  \details Устанавливает новую длину рёбер для тетраэдра, пересчитывает длину рёбер вписанного икосаэдра и затем вызывает анологичный метод у икосаэдра
		  \param[in] adEdgeLenght длина ребра тетраэдра
		  \return Acad::eOk в случае успешного выполнения, иначе другую ошибку типа Acad::ErrorStatus
	*/
	Acad::ErrorStatus setEdgeLength(const double adEdgeLength);
	/*!
		  \brief Устанавливает новые координаты центра фигуры
		  \param[in] adEdgeLenght координаты центра фигуры
		  \return Acad::eOk в случае успешного выполнения, иначе другую ошибку типа Acad::ErrorStatus
	*/
	Acad::ErrorStatus setCenter(const AcGePoint3d& aptCenter);
	/*!
		  \brief Возвращает координаты центра фигуры
		  \return Координаты центра фигуры
	*/
	const AcGePoint3d& center() const;
	const AcGePoint3dArray& vertices() const;
private:
	void update();
	/*!
		  \brief Вычисляет координаты m_ptBottomFaceCenter
		  \return координаты m_ptBottomFaceCenter
	*/
	AcGePoint3d bottomFaceCenterPoint() const noexcept;
	/*!
		  \brief Вычисляет координаты центра вписанного икосаэдра
		  \return координаты центра вписанного икосаэдра
	*/
	//static AcGePoint3d icosahedronCenter(AcGePoint3d aptTetrahedronCenter) noexcept;
	AcGePoint3d icosahedronCenter() const noexcept;
	/*!
		  \brief Вычисляет длину ребра вписанного икосаэдра по длине ребра тетраэдра
		  \param[in] adEdgeLenght длина ребра тетраэдра
		  \return Длину ребра вписанного икосаэдра тетраэдра
	*/
	double icosahedronEdgeLength(double adTetrahedronEdgeLenght) const noexcept;

	ADSKTetrahedron m_Tetrahedron; ///< Тетраэдр
	ADSKIcosahedron m_Icosahedron; ///< Вписанный икосаэдр
	AcGePoint3d m_ptBottomFaceCenter; ///< Точка на нижней гране тетраэдра для перемещения фигуры
};

#ifdef TETRAHEDRON_MODULE
ACDB_REGISTER_OBJECT_ENTRY_AUTO(ADSKCustomPyramid)
#endif
