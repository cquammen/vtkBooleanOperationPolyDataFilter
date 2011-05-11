/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkPolyDataBooleanOperationFilter.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkPolyDataBooleanOperationFilter
// .SECTION Description
//
// Computes the boundary of the union, intersection, or difference
// volume computed from the volumes defined by two input surfaces. The
// two surfaces do not need to be manifold. The resulting surface is
// available in the first output of the filter. The second output
// contains a set of polylines that represent the intersection between
// the two input surfaces.

#ifndef __vtkPolyDataBooleanOperationFilter_h
#define __vtkPolyDataBooleanOperationFilter_h

#include <vtkPolyDataAlgorithm.h>

#include <vtkDataSetAttributes.h> // Needed for CopyCells() method

class vtkIdList;
class vtkPolyDataDistance;
class vtkPolyDataIntersection;


class vtkPolyDataBooleanOperationFilter : public vtkPolyDataAlgorithm
{
public:
  // Description:
  // Construct object that computes the boolean surface.
  static vtkPolyDataBooleanOperationFilter *New();

  vtkTypeRevisionMacro(vtkPolyDataBooleanOperationFilter,
                       vtkPolyDataAlgorithm);

  void PrintSelf(ostream& os, vtkIndent indent);

  enum OperationTypes
  {
    UNION=0,
    INTERSECTION,
    DIFFERENCE
  };

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
  // surface when the operation is set to DIFFERENCE.
  vtkSetMacro( ReorientDifferenceCells, int );
  vtkGetMacro( ReorientDifferenceCells, int );
  vtkBooleanMacro( ReorientDifferenceCells, int );

  // Description:
  // Set/get the tolerance used to determine when a point's absolute
  // distance is considered to be zero.
  vtkSetMacro(Tolerance, double);
  vtkGetMacro(Tolerance, double);

protected:
  vtkPolyDataBooleanOperationFilter();
  ~vtkPolyDataBooleanOperationFilter();

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
  vtkPolyDataBooleanOperationFilter(const vtkPolyDataBooleanOperationFilter&); // no implementation
  void operator=(const vtkPolyDataBooleanOperationFilter&); // no implementation

  // Description:
  // Copies cells with indices given by  from one vtkPolyData to
  // another. The point and cell field lists are used to determine
  // which fields should be copied.
  void CopyCells(vtkPolyData* in, vtkPolyData* out, int idx,
                 vtkDataSetAttributes::FieldList & pointFieldList,
                 vtkDataSetAttributes::FieldList & cellFieldList,
                 vtkIdList* cellIds, bool reverseCells);

  vtkPolyDataIntersection *PolyDataIntersection;

  vtkPolyDataDistance *PolyDataDistance;

};

#endif