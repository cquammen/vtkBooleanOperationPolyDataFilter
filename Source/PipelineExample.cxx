#include <vtkActor.h>
#include <vtkAppendPolyData.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkPolyDataBooleanOperationFilter.h>
#include <vtkPolyDataDistance.h>
#include <vtkPolyDataIntersection.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkReverseSense.h>
#include <vtkSphereSource.h>
#include <vtkThreshold.h>


vtkActor* GetBooleanOperationActor( double x, int operation )
{
  double centerSeparation = 0.15;
  vtkSphereSource *sphere1 = vtkSphereSource::New();
  sphere1->SetCenter(-centerSeparation + x, 0.0, 0.0);

  vtkSphereSource *sphere2 = vtkSphereSource::New();
  sphere2->SetCenter(  centerSeparation + x, 0.0, 0.0);

  vtkPolyDataIntersection *intersection = vtkPolyDataIntersection::New();
  intersection->SetInputConnection( 0, sphere1->GetOutputPort() );
  intersection->SetInputConnection( 1, sphere2->GetOutputPort() );

  vtkPolyDataDistance *distance = vtkPolyDataDistance::New();
  distance->SetInputConnection( 0, intersection->GetOutputPort( 1 ) );
  distance->SetInputConnection( 1, intersection->GetOutputPort( 2 ) );

  vtkThreshold *thresh1 = vtkThreshold::New();
  thresh1->SetInputConnection( distance->GetOutputPort( 0 ) );

  vtkThreshold *thresh2 = vtkThreshold::New();
  thresh2->SetInputConnection( distance->GetOutputPort( 1 ) );

  if ( operation == vtkPolyDataBooleanOperationFilter::UNION )
    {
    thresh1->ThresholdByUpper( 0.0 );
    thresh2->ThresholdByUpper( 0.0 );
    }
  else if ( operation == vtkPolyDataBooleanOperationFilter::INTERSECTION )
    {
    thresh1->ThresholdByLower( 0.0 );
    thresh2->ThresholdByLower( 0.0 );
    }
  else // Difference
    {
    thresh1->ThresholdByUpper( 0.0 );
    thresh2->ThresholdByLower( 0.0 );
    }

  vtkDataSetSurfaceFilter *surface1 = vtkDataSetSurfaceFilter::New();
  surface1->SetInputConnection( thresh1->GetOutputPort() );
  thresh1->Delete();

  vtkDataSetSurfaceFilter *surface2 = vtkDataSetSurfaceFilter::New();
  surface2->SetInputConnection( thresh2->GetOutputPort() );
  thresh2->Delete();

  vtkReverseSense *reverseSense = vtkReverseSense::New();
  reverseSense->SetInputConnection( surface2->GetOutputPort() );
  if ( operation == 2 ) // difference
    {
    reverseSense->ReverseCellsOn();
    reverseSense->ReverseNormalsOn();
    }

  vtkAppendPolyData *appender = vtkAppendPolyData::New();
  appender->SetInputConnection( surface1->GetOutputPort() );
  if ( operation == 2)
    {
    appender->AddInputConnection( reverseSense->GetOutputPort() );
    }
  else
    {
    appender->AddInputConnection( surface2->GetOutputPort() );
    }
  surface2->Delete();

  vtkPolyDataMapper *mapper = vtkPolyDataMapper::New();
  mapper->SetInputConnection( appender->GetOutputPort() );
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
