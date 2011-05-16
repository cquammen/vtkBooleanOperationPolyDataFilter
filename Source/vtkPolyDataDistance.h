/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkPolyDataDistance.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkPolyDataDistance
// .SECTION Description
//
// Computes the signed distance from one vtkPolyData to another. The
// signed distance to the second input is computed at every point in
// the first input using vtkImplicitPolyData. Optionally, the signed
// distance to the first input at every point in the second input can
// be computed. This may be enabled by calling
// ComputeSecondDistanceOn().
//
// If the signed distance is not desired, the unsigned distance can be
// computed by calling SignedDistanceOff(). The signed distance field
// may be negated by calling NegateDistanceOn();
//
// Written by Chris Weigle and Cory Quammen, The University of North
// Carolina at Chapel Hill.

#ifndef __vtkPolyDataDistance_h
#define __vtkPolyDataDistance_h

#include "vtkPolyDataAlgorithm.h"

class vtkPolyDataDistance : public vtkPolyDataAlgorithm {
public:
  static vtkPolyDataDistance *New();
  vtkTypeMacro(vtkPolyDataDistance, vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Enable/disable computation of the signed distance between
  // the first poly data and the second poly data. Defaults to on.
  vtkSetMacro(SignedDistance,int);
  vtkGetMacro(SignedDistance,int);
  vtkBooleanMacro(SignedDistance,int);

  // Description:
  // Enable/disable negation of the distance values. Defaults to
  // off. Has no effect if SignedDistance is off.
  vtkSetMacro(NegateDistance,int);
  vtkGetMacro(NegateDistance,int);
  vtkBooleanMacro(NegateDistance,int);

  // Description:
  // Enable/disable computation of a second output poly data with the
  // distance from the first poly data at each point. Defaults to on.
  vtkSetMacro(ComputeSecondDistance,int);
  vtkGetMacro(ComputeSecondDistance,int);
  vtkBooleanMacro(ComputeSecondDistance,int);

  // Description:
  // Get the second output, which is a copy of the second input with an
  // additional distance scalar field.
  vtkPolyData* GetSecondDistanceOutput();

protected:
  vtkPolyDataDistance();
  ~vtkPolyDataDistance();

  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*);
  int FillInputPortInformation(int, vtkInformation*);

  void GetPolyDataDistance(vtkPolyData*, vtkPolyData*);

  int SignedDistance;
  int NegateDistance;
  int ComputeSecondDistance;

private:
  vtkPolyDataDistance(const vtkPolyDataDistance&); // no implementation
  void operator=(const vtkPolyDataDistance&);      // no implementation
};

#endif
