/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkBooleanOperationPolyDataFilter.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkBooleanOperationPolyDataFilter
// .SECTION Description
//
// Computes the boundary of the union, intersection, or difference
// volume computed from the volumes defined by two input surfaces. The
// two surfaces do not need to be manifold, but if they are not,
// unexpected results may be obtained. The resulting surface is
// available in the first output of the filter. The second output
// contains a set of polylines that represent the intersection between
// the two input surfaces.
//
// Written by Chris Weigle and Cory Quammen, The University of North
// Carolina at Chapel Hill.

#ifndef __vtkBooleanOperationPolyDataFilter_h
#define __vtkBooleanOperationPolyDataFilter_h

#include "vtkPolyDataAlgorithm.h"

#include "vtkDataSetAttributes.h" // Needed for CopyCells() method

class vtkIdList;
class vtkDistancePolyDataFilter;
class vtkIntersectionPolyDataFilter;


class vtkBooleanOperationPolyDataFilter : public vtkPolyDataAlgorithm
{
public:
  // Description:
  // Construct object that computes the boolean surface.
  static vtkBooleanOperationPolyDataFilter *New();

  vtkTypeMacro(vtkBooleanOperationPolyDataFilter,
               vtkPolyDataAlgorithm);

  void PrintSelf(ostream& os, vtkIndent indent);

  enum OperationTypes
  {
    UNION=0,
    INTERSECTION,
    DIFFERENCE
  };

  // Description:
  // Set the boolean operation to perform. Defaults to union.
  vtkSetClampMacro( Operation, int, UNION, DIFFERENCE );
  vtkGetMacro( Operation, int );
  void SetOperationToUnion()
  { this->SetOperation( UNION ); }
  void SetOperationToIntersection()
  { this->SetOperation( INTERSECTION ); }
  void SetOperationToDifference()
  { this->SetOperation( DIFFERENCE ); }

  // Description:
  // Turn on/off cell reorientation of the intersection portion of the
  // surface when the operation is set to DIFFERENCE. Defaults to on.
  vtkSetMacro( ReorientDifferenceCells, int );
  vtkGetMacro( ReorientDifferenceCells, int );
  vtkBooleanMacro( ReorientDifferenceCells, int );

  // Description:
  // Set/get the tolerance used to determine when a point's absolute
  // distance is considered to be zero. Defaults to 1e-6.
  vtkSetMacro(Tolerance, double);
  vtkGetMacro(Tolerance, double);

protected:
  vtkBooleanOperationPolyDataFilter();
  ~vtkBooleanOperationPolyDataFilter();

  // Description:
  // Tolerance used to determine when a point's absolute
  // distance is considered to be zero.
  double Tolerance;

  // Description:
  // Labels triangles in mesh as part of the intersection or union surface.
  void SortPolyData(vtkPolyData* input, vtkIdList* intersectionList,
                    vtkIdList* unionList);

  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*);
  int FillInputPortInformation(int, vtkInformation*);

  // Description:
  // Which operation to perform. Can be UNION, INTERSECTION, or DIFFERENCE.
  int Operation;

  // Description:
  // Determines if cells from the intersection surface should be
  // reversed in the difference surface.
  int ReorientDifferenceCells;

private:
  vtkBooleanOperationPolyDataFilter(const vtkBooleanOperationPolyDataFilter&); // no implementation
  void operator=(const vtkBooleanOperationPolyDataFilter&); // no implementation

  // Description:
  // Copies cells with indices given by  from one vtkPolyData to
  // another. The point and cell field lists are used to determine
  // which fields should be copied.
  void CopyCells(vtkPolyData* in, vtkPolyData* out, int idx,
                 vtkDataSetAttributes::FieldList & pointFieldList,
                 vtkDataSetAttributes::FieldList & cellFieldList,
                 vtkIdList* cellIds, bool reverseCells);

  vtkIntersectionPolyDataFilter *PolyDataIntersection;

  vtkDistancePolyDataFilter *PolyDataDistance;

};

#endif
