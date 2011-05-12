/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkPolyDataDistance.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkPolyDataDistance.h"

#include "vtkCellData.h"
#include "vtkDoubleArray.h"
#include "vtkFieldData.h"
#include "vtkImplicitPolyData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkPolygon.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkTriangle.h"

vtkCxxRevisionMacro(vtkPolyDataDistance, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkPolyDataDistance);

//-----------------------------------------------------------------------------
vtkPolyDataDistance::vtkPolyDataDistance() : vtkPolyDataAlgorithm()
{
  this->SignedDistance = 1;
  this->NegateDistance = 0;
  this->ComputeSecondDistance = 1;

  this->SetNumberOfInputPorts(2);
  this->SetNumberOfOutputPorts(2);

  vtkPolyData* output1 = vtkPolyData::New();
  this->GetExecutive()->SetOutputData(1, output1);
  output1->Delete();
}

//-----------------------------------------------------------------------------
vtkPolyDataDistance::~vtkPolyDataDistance()
{
}


//-----------------------------------------------------------------------------
int vtkPolyDataDistance::RequestData(vtkInformation*        vtkNotUsed(request),
                                     vtkInformationVector** inputVector,
                                     vtkInformationVector*  outputVector)
{
  vtkInformation* inInfo0 = inputVector[0]->GetInformationObject(0);
  vtkInformation* inInfo1 = inputVector[1]->GetInformationObject(0);
  vtkInformation* outInfo = outputVector->GetInformationObject(0);

  if (!inInfo0 || !inInfo1 || !outInfo) return 0;

  vtkPolyData *input0 = vtkPolyData::SafeDownCast(inInfo0->Get(vtkDataObject::DATA_OBJECT()));
  vtkPolyData *input1 = vtkPolyData::SafeDownCast(inInfo1->Get(vtkDataObject::DATA_OBJECT()));
  vtkPolyData* output0 = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkPolyData* output1 = this->GetSecondDistanceOutput();

  if (!input0 || !input1 || !output0 || !output1) return 0;

  output0->CopyStructure(input0);
  output0->GetPointData()->PassData(input0->GetPointData());
  output0->GetCellData()->PassData(input0->GetCellData());
  this->GetPolyDataDistance(output0, input1);

  if (this->ComputeSecondDistance)
    {
    output1->CopyStructure(input1);
    output1->GetPointData()->PassData(input1->GetPointData());
    output1->GetCellData()->PassData(input1->GetCellData());
    this->GetPolyDataDistance(output1, input0);
    }

  return 1;
}

//-----------------------------------------------------------------------------
void vtkPolyDataDistance::GetPolyDataDistance(vtkPolyData* mesh, vtkPolyData* src)
{
  vtkDebugMacro(<<"Start vtkPolyDataDistance::GetPolyDataDistance");

  if (mesh->GetNumberOfPolys() == 0 || mesh->GetNumberOfPoints() == 0)
    {
    vtkErrorMacro(<<"No points/cells to operate on");
    return;
    }

  if (src->GetNumberOfPolys() == 0 || src->GetNumberOfPoints() == 0)
    {
    vtkErrorMacro(<<"No points/cells to difference from");
    return;
    }

  int numPts = mesh->GetNumberOfPoints();

  vtkDoubleArray* da = vtkDoubleArray::New();
  da->SetName("Distance");
  da->SetNumberOfComponents(1);
  da->SetNumberOfTuples(numPts);

  double* dist = da->GetPointer(0);

  vtkImplicitPolyData* imp = vtkImplicitPolyData::New();
  imp->SetInput(src);

  for (int i = 0; i < numPts; i++)
    {
    double pt[3];
    mesh->GetPoint(i, pt);
    double val = imp->EvaluateFunction(pt);
    dist[i] = SignedDistance ? (NegateDistance ? -val : val) : fabs(val);
    }

  mesh->GetPointData()->AddArray(da);
  mesh->GetPointData()->SetActiveScalars("Distance");

  imp->Delete();
  da->Delete();

  vtkDebugMacro(<<"End vtkPolyDataDistance::GetPolyDataDistance");
}

//-----------------------------------------------------------------------------
vtkPolyData* vtkPolyDataDistance::GetSecondDistanceOutput()
{
  if (!this->ComputeSecondDistance) return 0;
  return vtkPolyData::SafeDownCast(this->GetExecutive()->GetOutputData(1));
}

//-----------------------------------------------------------------------------
int vtkPolyDataDistance::FillInputPortInformation(int port, vtkInformation *info)
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
void vtkPolyDataDistance::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "SignedDistance: " << this->SignedDistance << "\n";
  os << indent << "NegateDistance: " << this->NegateDistance << "\n";
  os << indent << "ComputeSecondDistance: " << this->ComputeSecondDistance << "\n";
}
