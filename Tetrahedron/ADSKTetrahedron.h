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
//----- ADSKTetrahedron.h : Declaration of the ADSKTetrahedron
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
#include "Tchar.h"
#include "dbmain.h"

#define ASDKTETRAHEDRON_DBXSERVICE _T("ASDKTETRAHEDRON_DBXSERVICE")
//-----------------------------------------------------------------------------
class ADSKCustomPyramid;
class DLLIMPEXP ADSKTetrahedron : public AcDbEntity {

public:
  ACRX_DECLARE_MEMBERS(ADSKTetrahedron);

protected:
  static Adesk::UInt32 kCurrentVersionNumber;

public:
  friend class ADSKCustomPyramid;
  ADSKTetrahedron();
  ADSKTetrahedron(AcGePoint3d aptCenter, double adEdgeLength);
  virtual ~ADSKTetrahedron();

  //----- AcDbObject protocols
  //- Dwg Filing protocol
  virtual Acad::ErrorStatus dwgOutFields(AcDbDwgFiler *pFiler) const;
  virtual Acad::ErrorStatus dwgInFields(AcDbDwgFiler *pFiler);

  //----- AcDbEntity protocols
  //- Graphics protocol
protected:
  virtual Adesk::Boolean subWorldDraw(AcGiWorldDraw *mode);
  // virtual Adesk::UInt32 subSetAttributes (AcGiDrawableTraits *traits) ;
  virtual Acad::ErrorStatus subTransformBy(const AcGeMatrix3d &xform);
  //- Osnap points protocol
public:
  //- Grip points protocol
  virtual Acad::ErrorStatus subGetGripPoints(AcDbGripDataPtrArray &grips,
                                             const double curViewUnitSize,
                                             const int gripSize,
                                             const AcGeVector3d &curViewDir,
                                             const int bitflags) const;
  virtual Acad::ErrorStatus
  subMoveGripPointsAt(const AcDbVoidPtrArray &gripAppData,
                      const AcGeVector3d &offset, const int bitflags);
  // for Tetrahedron
  /*!
            \brief Вычисляет радиус вписанной сферы тетраэдра по длине ребра
     тетраэдра 
            \param[in] adEdgeLenght длина ребра тетраэдра 
            \return Радиус вписанной сферы тетраэдра
  */
  [[nodiscard]] static double inradius(double adEdgeLength) noexcept;
  [[nodiscard]] static double midradius(double adEdgeLength) noexcept;
  [[nodiscard]] double inradius() const noexcept;
  /*!
            \brief Вычисляет объём тетраэдра
            \details https://en.wikipedia.org/wiki/Tetrahedron#Measurement
            \return Объём тетраэдра
  */
  [[nodiscard]] double volume() const noexcept;
  /*!
            \brief Вычисляет высоту тетраэдра по длине ребра тетраэдра
            \details https://en.wikipedia.org/wiki/Tetrahedron#Measurement
            \return Высоту тетраэдра
  */
  [[nodiscard]] static double height(double adEdgeLenght) noexcept;
  [[nodiscard]] double height() const noexcept;
  Acad::ErrorStatus setCenter(const AcGePoint3d &aptCenter);
  [[nodiscard]] const AcGePoint3d &center() const;
  [[nodiscard]] double edgeLength() const;
  Acad::ErrorStatus setEdgeLength(double adEdgeLenght);
  [[nodiscard]] const AcGePoint3dArray &vertices() const;
#ifndef _DEBUG
  bool runTests() const;
#endif // !_DEBUG
private:
  /*!
            \details Обновляет длину ребер тетраэдра, вызывается в методе
     subTransformBy
  */
  void updateEdgeLength() noexcept;
  /*!
            \details Вычисляет координаты точек тетраэдра по длине ребра граней
     и координате центра фигуры
  */
  void calculateVertices() noexcept;
  double m_dEdgeLength;
  AcGePoint3d m_ptCenter;
  AcGePoint3dArray m_aVertices; ///< Координаты вершин тетраэдра
                                // static std::array<>aFaces Vertices Indexes
};

#ifdef TETRAHEDRON_MODULE
ACDB_REGISTER_OBJECT_ENTRY_AUTO(ADSKTetrahedron)
#endif
