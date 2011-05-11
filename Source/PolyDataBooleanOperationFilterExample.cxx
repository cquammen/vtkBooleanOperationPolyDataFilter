#include <vtkActor.h>
#include <vtkImplicitPolyData.h>
#include <vtkPolyDataBooleanOperationFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSphereSource.h>

int main(int argc, char* argv[])
{
  vtkSphereSource *sphere1 = vtkSphereSource::New();
  sphere1->SetCenter(0.0, -0.25, 0.0);

  vtkSphereSource *sphere2 = vtkSphereSource::New();
  sphere2->SetCenter(0.0, 0.25, 0.0);

  vtkPolyDataBooleanOperationFilter *boolFilter =
    vtkPolyDataBooleanOperationFilter::New();
  boolFilter->SetOperationToDifference();
  boolFilter->SetInputConnection( 0, sphere1->GetOutputPort() );
  boolFilter->SetInputConnection( 1, sphere1->GetOutputPort() );

  vtkPolyDataMapper *mapper = vtkPolyDataMapper::New();
  mapper->SetInputConnection( boolFilter->GetOutputPort() );

  vtkActor *actor = vtkActor::New();
  actor->SetMapper( mapper );

  vtkRenderer *renderer = vtkRenderer::New();

  vtkRenderWindow *renWin = vtkRenderWindow::New();
  renWin->AddRenderer( renderer );

  vtkRenderWindowInteractor * renWinInteractor = vtkRenderWindowInteractor::New();
  renWinInteractor->SetRenderWindow( renWin );

  renderer->AddActor( actor );

  renWin->Render();
  renWinInteractor->Start();

  return EXIT_SUCCESS;
}
