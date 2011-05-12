#include <vtkActor.h>
#include <vtkPolyDataBooleanOperationFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSphereSource.h>

vtkActor* GetBooleanOperationActor( double x, int operation )
{
  double centerSeparation = 0.15;
  vtkSphereSource *sphere1 = vtkSphereSource::New();
  sphere1->SetCenter(-centerSeparation + x, 0.0, 0.0);

  vtkSphereSource *sphere2 = vtkSphereSource::New();
  sphere2->SetCenter(  centerSeparation + x, 0.0, 0.0);

  vtkPolyDataBooleanOperationFilter *boolFilter =
    vtkPolyDataBooleanOperationFilter::New();
  boolFilter->SetOperation( operation );
  boolFilter->SetInputConnection( 0, sphere1->GetOutputPort() );
  sphere1->Delete();
  boolFilter->SetInputConnection( 1, sphere2->GetOutputPort() );
  sphere2->Delete();

  vtkPolyDataMapper *mapper = vtkPolyDataMapper::New();
  mapper->SetInputConnection( boolFilter->GetOutputPort() );
  mapper->ScalarVisibilityOff();

  vtkActor *actor = vtkActor::New();
  actor->SetMapper( mapper );
  mapper->Delete();

  return actor;
}


int main(int argc, char* argv[])
{
  vtkRenderer *renderer = vtkRenderer::New();

  vtkRenderWindow *renWin = vtkRenderWindow::New();
  renWin->AddRenderer( renderer );

  vtkRenderWindowInteractor * renWinInteractor = vtkRenderWindowInteractor::New();
  renWinInteractor->SetRenderWindow( renWin );

  vtkActor *unionActor = 
    GetBooleanOperationActor( -2.0, vtkPolyDataBooleanOperationFilter::UNION );
  vtkActor *intersectionActor =
    GetBooleanOperationActor(  0.0, vtkPolyDataBooleanOperationFilter::INTERSECTION );
  vtkActor *differenceActor =
    GetBooleanOperationActor(  2.0, vtkPolyDataBooleanOperationFilter::DIFFERENCE );

  renderer->AddActor( unionActor );
  unionActor->Delete();
  renderer->AddActor( intersectionActor );
  intersectionActor->Delete();
  renderer->AddActor( differenceActor );
  differenceActor->Delete();

  renWin->Render();
  renWinInteractor->Start();

  renWinInteractor->Delete();
  renWin->Delete();

  return EXIT_SUCCESS;
}
