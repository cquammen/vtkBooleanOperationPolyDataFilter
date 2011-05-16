#include <vtkActor.h>
#include <vtkPolyDataBooleanOperationFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>

#include <vtkImplicitPolyData.h>
#include <vtkPolyDataDistance.h>
#include <vtkPolyDataBooleanOperationFilter.h>

#include <vtkPolyDataIntersection.h>

#define vtkNewSmartPointer( classname, var ) \
  vtkSmartPointer<classname> var = vtkSmartPointer<classname>::New();


vtkActor* GetBooleanOperationActor( double x, int operation )
{
  double centerSeparation = 0.15;
  vtkNewSmartPointer(vtkSphereSource, sphere1);
  sphere1->SetCenter(-centerSeparation + x, 0.0, 0.0);

  vtkNewSmartPointer(vtkSphereSource, sphere2);
  sphere2->SetCenter(  centerSeparation + x, 0.0, 0.0);

  vtkNewSmartPointer(vtkPolyDataBooleanOperationFilter, boolFilter);
  boolFilter->SetOperation( operation );
  boolFilter->SetInputConnection( 0, sphere1->GetOutputPort() );
  boolFilter->SetInputConnection( 1, sphere2->GetOutputPort() );
  boolFilter->Update();

  vtkNewSmartPointer(vtkPolyDataMapper, mapper);
  mapper->SetInputConnection( boolFilter->GetOutputPort() );
  mapper->ScalarVisibilityOff();

  vtkActor *actor = vtkActor::New();
  actor->SetMapper( mapper );

  return actor;
}


int main(int argc, char* argv[])
{
  vtkNewSmartPointer(vtkRenderer, renderer);
  vtkNewSmartPointer(vtkRenderWindow, renWin);
  renWin->AddRenderer( renderer );

  vtkNewSmartPointer(vtkRenderWindowInteractor, renWinInteractor);
  renWinInteractor->SetRenderWindow( renWin );

  vtkActor *unionActor =
    GetBooleanOperationActor( -2.0, vtkPolyDataBooleanOperationFilter::UNION );
  renderer->AddActor( unionActor );
  unionActor->Delete();

  vtkActor *intersectionActor =
    GetBooleanOperationActor(  0.0, vtkPolyDataBooleanOperationFilter::INTERSECTION );
  renderer->AddActor( intersectionActor );
  intersectionActor->Delete();

  vtkActor *differenceActor =
    GetBooleanOperationActor(  2.0, vtkPolyDataBooleanOperationFilter::DIFFERENCE );
  renderer->AddActor( differenceActor );
  differenceActor->Delete();

  renWin->Render();
  renWinInteractor->Start();

  return EXIT_SUCCESS;
}
