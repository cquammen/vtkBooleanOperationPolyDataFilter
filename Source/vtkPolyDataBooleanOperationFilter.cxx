/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkPolyDataBooleanOperationFilter.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkPolyDataBooleanOperationFilter.h"

#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkDataArray.h"
#include "vtkDataSetAttributes.h"
#include "vtkDoubleArray.h"
#include "vtkFloatArray.h"
#include "vtkGenericCell.h"
#include "vtkIdList.h"
#include "vtkImplicitPolyData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkIntArray.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkPolyDataDistance.h"
#include "vtkPolyDataIntersection.h"
#include "vtkStreamingDemandDrivenPipeline.h"


vtkCxxRevisionMacro(vtkPolyDataBooleanOperationFilter, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkPolyDataBooleanOperationFilter);

//-----------------------------------------------------------------------------
vtkPolyDataBooleanOperationFilter::vtkPolyDataBooleanOperationFilter() : 
  vtkPolyDataAlgorithm()
{
  this->Tolerance = 1e-6;
  this->Operation = UNION;
  this->ReorientDifferenceCells = 1;

  this->SetNumberOfInputPorts(2);
  this->SetNumberOfOutputPorts(2);

  this->PolyDataIntersection = vtkPolyDataIntersection::New();
  this->PolyDataDistance = vtkPolyDataDistance::New();
}

//-----------------------------------------------------------------------------
vtkPolyDataBooleanOperationFilter::~vtkPolyDataBooleanOperationFilter()
{
  this->PolyDataIntersection->Delete();
  this->PolyDataDistance->Delete();
}

//-----------------------------------------------------------------------------
void vtkPolyDataBooleanOperationFilter::SortPolyData(vtkPolyData* input,
                                       vtkIdList* interList,
                                       vtkIdList* unionList)
{
  int numCells = input->GetNumberOfCells();

  double* dist = static_cast<double*>(input->GetPointData()->GetArray("Distance")->WriteVoidPointer(0, 0));

  vtkIdType npts;
  vtkIdType* pts;
  for (int cid = 0; cid < numCells; cid++)
    {
    input->GetCellPoints(cid, npts, pts);

    int outCount = 0, inCount = 0, onCount = 0;
    for (int i = 0; i < npts; i++)
      {
      outCount += (dist[pts[i]] >  this->Tolerance ? 1 : 0);
      inCount  += (dist[pts[i]] < -this->Tolerance ? 1 : 0);
      onCount  += (fabs(dist[pts[i]]) <= this->Tolerance ? 1 : 0);
      }

    // Sort between union and intersection.
    if (onCount == npts) // Completely on inter surface
      {
      interList->InsertNextId(cid);
      }
    else if (outCount + onCount == npts)
      {
      unionList->InsertNextId(cid);
      }
    else if (inCount + onCount == npts)
      {
      interList->InsertNextId(cid);
      }
    else
      {
      // Some points have positive distance and some have negative
      // distance. Do a simple vote based on which side has the most
      // points.  This shouldn't ever happen when the inputs are
      // processed by the vtkPolyDataIntersection class.
      if (outCount > inCount)
        {
        unionList->InsertNextId(cid);
        }
      else
        {
        interList->InsertNextId(cid);
        }
      }
    }
}


//-----------------------------------------------------------------------------
int vtkPolyDataBooleanOperationFilter::RequestData(vtkInformation*        vtkNotUsed(request),
                                     vtkInformationVector** inputVector,
                                     vtkInformationVector*  outputVector)
{
  vtkInformation* inInfo0 = inputVector[0]->GetInformationObject(0);
  vtkInformation* inInfo1 = inputVector[1]->GetInformationObject(0);
  vtkInformation* outInfo0 = outputVector->GetInformationObject(0);
  vtkInformation* outInfo1 = outputVector->GetInformationObject(1);

  if (!inInfo0 || !inInfo1 || !outInfo0 || !outInfo1)
    {
    return 0;
    }

  vtkPolyData* input0 = 
    vtkPolyData::SafeDownCast(inInfo0->Get(vtkDataObject::DATA_OBJECT()));
  vtkPolyData* input1 = 
    vtkPolyData::SafeDownCast(inInfo1->Get(vtkDataObject::DATA_OBJECT()));

  vtkPolyData* outputSurface = 
    vtkPolyData::SafeDownCast(outInfo0->Get(vtkDataObject::DATA_OBJECT()));
  vtkPolyData* outputIntersection = 
    vtkPolyData::SafeDownCast(outInfo1->Get(vtkDataObject::DATA_OBJECT()));

  if (!input0 || !input1 || !outputSurface || !outputIntersection)
    {
    return 0;
    }

  // Get intersected versions
  this->PolyDataIntersection->SetInput(0, input0);
  this->PolyDataIntersection->SetInput(1, input1);
  this->PolyDataIntersection->SplitFirstOutputOn();
  this->PolyDataIntersection->SplitSecondOutputOn();
  this->PolyDataIntersection->Update();

  outputIntersection->CopyStructure(this->PolyDataIntersection->GetOutput());
  outputIntersection->GetPointData()->PassData(this->PolyDataIntersection->GetOutput()->GetPointData());
  outputIntersection->GetCellData()->PassData(this->PolyDataIntersection->GetOutput()->GetCellData());

  // Compute distances
  this->PolyDataDistance->SetInput(0, this->PolyDataIntersection->GetOutput(1));
  this->PolyDataDistance->SetInput(1, this->PolyDataIntersection->GetOutput(2));
  this->PolyDataDistance->ComputeSecondDistanceOn();
  this->PolyDataDistance->Update();

  vtkPolyData* pd0 = this->PolyDataDistance->GetOutput();
  vtkPolyData* pd1 = this->PolyDataDistance->GetSecondDistanceOutput();

  pd0->BuildCells();
  pd0->BuildLinks();
  pd1->BuildCells();
  pd1->BuildLinks();

  // Set up field lists of both points and cells that are shared by
  // the input data sets.
  vtkDataSetAttributes::FieldList pointFields(2);
  pointFields.InitializeFieldList( pd0->GetPointData() );
  pointFields.IntersectFieldList(  pd1->GetPointData() );

  vtkDataSetAttributes::FieldList cellFields(2);
  cellFields.InitializeFieldList( pd0->GetCellData() );
  cellFields.IntersectFieldList(  pd1->GetCellData() );

  // Sort union/intersection.
  vtkIdList* interList = vtkIdList::New();
  vtkIdList* unionList = vtkIdList::New();

  this->SortPolyData(pd0, interList, unionList);

  outputSurface->Allocate(pd0);
  outputSurface->GetPointData()->CopyAllocate(pointFields);
  outputSurface->GetCellData()->CopyAllocate(cellFields);

  if ( this->Operation == UNION || this->Operation == DIFFERENCE )
    {
    this->CopyCells(pd0, outputSurface, 0, pointFields, cellFields, unionList,
                    false);
    }
  else if ( this->Operation == INTERSECTION )
    {
    this->CopyCells(pd0, outputSurface, 0, pointFields, cellFields, interList,
                    false);
    }

  // Label sources for each point and cell.
  vtkIntArray* pointSourceLabel = vtkIntArray::New();
  pointSourceLabel->SetNumberOfComponents(1);
  pointSourceLabel->SetName("PointSource");
  pointSourceLabel->SetNumberOfTuples(outputSurface->GetNumberOfPoints());
  for (vtkIdType i = 0; i < outputSurface->GetNumberOfPoints(); i++)
    {
    pointSourceLabel->InsertValue(i, 0);
    }

  vtkIntArray *cellSourceLabel = vtkIntArray::New();
  cellSourceLabel->SetNumberOfComponents(1);
  cellSourceLabel->SetName("CellSource");
  cellSourceLabel->SetNumberOfValues(outputSurface->GetNumberOfCells());
  for (vtkIdType i = 0; i < outputSurface->GetNumberOfCells(); i++)
    {
    cellSourceLabel->InsertValue(i, 0);
    }

  interList->Reset();
  unionList->Reset();

  this->SortPolyData(pd1, interList, unionList);

  if ( this->Operation == UNION )
    {
    this->CopyCells(pd1, outputSurface, 1, pointFields, cellFields, unionList,
      false);
    }
  else if ( this->Operation == INTERSECTION || this->Operation == DIFFERENCE )
    {
    this->CopyCells(pd1, outputSurface, 1, pointFields, cellFields, interList,
                    (this->ReorientDifferenceCells == 1 && 
                     this->Operation == DIFFERENCE));
    }

  vtkIdType i;
  i = pointSourceLabel->GetNumberOfTuples();
  pointSourceLabel->Resize(outputSurface->GetNumberOfPoints());
  for ( ; i < outputSurface->GetNumberOfPoints(); i++)
    {
    pointSourceLabel->InsertValue(i, 1);
    }

  i = cellSourceLabel->GetNumberOfTuples();
  cellSourceLabel->Resize(outputSurface->GetNumberOfCells());
  for ( ; i < outputSurface->GetNumberOfCells(); i++)
    {
    cellSourceLabel->InsertValue(i, 1);
    }

  outputSurface->GetPointData()->AddArray(pointSourceLabel);
  pointSourceLabel->Delete();
  outputSurface->GetCellData()->AddArray(cellSourceLabel);
  cellSourceLabel->Delete();

  unionList->Delete();
  interList->Delete();

  outputSurface->Squeeze();
  outputSurface->GetPointData()->Squeeze();
  outputSurface->GetCellData()->Squeeze();

  return 1;
}

//-----------------------------------------------------------------------------
void vtkPolyDataBooleanOperationFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//-----------------------------------------------------------------------------
int vtkPolyDataBooleanOperationFilter::FillInputPortInformation(int port, vtkInformation *info)
{
  if (!this->Superclass::FillInputPortInformation(port, info)) return 0;
  if (port == 0)
    {
    info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");
    }
  else if (port == 1)
    {
    info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");
    info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(), 0);
    }
  return 1;
}

//-----------------------------------------------------------------------------
void vtkPolyDataBooleanOperationFilter
::CopyCells(vtkPolyData* in, vtkPolyData* out, int idx,
            vtkDataSetAttributes::FieldList & pointFieldList,
            vtkDataSetAttributes::FieldList & cellFieldList,
            vtkIdList* cellIds, bool reverseCells)
{
  // Largely copied from vtkPolyData::CopyCells, but modified to
  // use the special form of CopyData that uses a field list to
  // determine which data values to copy over.

  vtkPointData* outPD = out->GetPointData();
  vtkCellData*  outCD = out->GetCellData();

  vtkFloatArray *outNormals = NULL;
  if ( reverseCells )
    {
    outNormals = vtkFloatArray::SafeDownCast( outPD->GetArray("Normals") );
    }

  vtkIdType numPts = in->GetNumberOfPoints();

  if ( out->GetPoints() == NULL)
    {
    out->SetPoints( vtkPoints::New() );
    }

  vtkPoints *newPoints = out->GetPoints();

  vtkIdList *pointMap = vtkIdList::New();
  pointMap->SetNumberOfIds( numPts );
  for ( vtkIdType i = 0; i < numPts; i++ )
    {
    pointMap->SetId(i, -1);
    }

  // Filter the cells
  vtkGenericCell *cell = vtkGenericCell::New();
  vtkIdList *newCellPts = vtkIdList::New();
  for ( vtkIdType cellId = 0; cellId < cellIds->GetNumberOfIds(); cellId++ )
    {
    in->GetCell( cellIds->GetId( cellId ), cell );
    vtkIdList *cellPts = cell->GetPointIds();
    vtkIdType numCellPts = cell->GetNumberOfPoints();

    for ( vtkIdType i = 0; i < numCellPts; i++ )
      {
      vtkIdType ptId = cellPts->GetId( i );
      vtkIdType newId = pointMap->GetId( ptId );
      if ( newId < 0 )
        {
        double x[3];
        in->GetPoint( ptId, x );
        newId = newPoints->InsertNextPoint( x );
        pointMap->SetId( ptId, newId );
        outPD->CopyData( pointFieldList, in->GetPointData(), idx, ptId, newId );

        if ( reverseCells && outNormals )
          {
          float normal[3];
          outNormals->GetTupleValue( newId, normal );
          normal[0] *= -1.0;
          normal[1] *= -1.0;
          normal[2] *= -1.0;
          outNormals->SetTupleValue( newId, normal );
          }

        }
      newCellPts->InsertId( i, newId );
      }
    if ( reverseCells )
      {
      for (vtkIdType i = 0; i < newCellPts->GetNumberOfIds() / 2; i++)
        {
        vtkIdType i1 = i;
        vtkIdType i2 = newCellPts->GetNumberOfIds()-i-1;

        vtkIdType id = newCellPts->GetId( i1 );
        newCellPts->SetId( i1, newCellPts->GetId( i2 ) );
        newCellPts->SetId( i2, id );
        }
      }

    vtkIdType newCellId = out->InsertNextCell( cell->GetCellType(), newCellPts );
    outCD->CopyData( cellFieldList, in->GetCellData(), idx,
                     cellIds->GetId( cellId ), newCellId );

    newCellPts->Reset();
    } // for all cells

  newCellPts->Delete();
  pointMap->Delete();
  cell->Delete();
}