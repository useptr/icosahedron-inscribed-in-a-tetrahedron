﻿// (C) Copyright 2005-2007 by Autodesk, Inc.
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

//-----------------------------------------------------------------------------
//- ADSKPyramidJig.h
#pragma once
#include "ADSKCustomPyramid.h"
#include "dbjig.h"
//-----------------------------------------------------------------------------
/*!
        \brief Позволяет создать экземпляр класса ADSKCustomPyramid с заданным
   центром координат и длиной граней
*/
class ADSKPyramidJig : public AcEdJig {
private:
  //- Member variables
  AcGePoint3d mptCenter;
  double mdEdgeLenght;
  //- current input level, increment for each input
  int mCurrentInputLevel;
  //- Dynamic dimension info
  AcDbDimDataPtrArray mDimData;

public:
  //- Array of input points, each level corresponds to the mCurrentInputLevel
  AcGePoint3dArray mInputPoints;
  //- Entity being jigged
  ADSKCustomPyramid *mpEntity;

public:
  ADSKPyramidJig(AcGePoint3d &);
  ~ADSKPyramidJig();
  //- Command invoke the jig, call passing a new'd instance of the object to jig
  AcEdJig::DragStatus startJig(ADSKCustomPyramid *pEntityToJig);

protected:
  //- AcEdJig overrides
  //- input sampler
  virtual DragStatus sampler();
  //- jigged entity update
  virtual Adesk::Boolean update();
  //- jigged entity pointer return
  virtual AcDbEntity *entity() const;
  //- dynamic dimension data setup
  // virtual AcDbDimDataPtrArray *dimData (const double dimScale) ;
  ////- dynamic dimension data update
  // virtual Acad::ErrorStatus setDimValue (const AcDbDimData *pDimData, const
  // double dimValue) ;

  ////- Standard helper functions
  ////- dynamic dimdata update function
  // virtual Adesk::Boolean updateDimData () ;

  ////- Std input to get a point with no rubber band
  // AcEdJig::DragStatus GetStartPoint () ;
  ////- Std input to get a point with rubber band from point
  // AcEdJig::DragStatus GetNextPoint () ;
};
