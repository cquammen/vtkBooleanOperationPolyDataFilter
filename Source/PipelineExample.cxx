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
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkThreshold.h>


vtkActor* GetBooleanOperationActor( double x, int operation )
{
  double centerSeparation = 0.15;
  vtkSmartPointer<vtkSphereSource> sphere1 =
    vtkSmartPointer<vtkSphereSource>::New();
  sphere1->SetCenter(-centerSeparation + x, 0.0, 0.0);

  vtkSmartPointer<vtkSphereSource> sphere2 =
    vtkSmartPointer<vtkSphereSource>::New();
  sphere2->SetCenter(  centerSeparation + x, 0.0, 0.0);

  vtkSmartPointer<vtkPolyDataIntersection> intersection =
    vtkSmartPointer<vtkPolyDataIntersection>::New();
  intersection->SetInputConnection( 0, sphere1->GetOutputPort() );
  intersection->SetInputConnection( 1, sphere2->GetOutputPort() );

  vtkSmartPointer<vtkPolyDataDistance> distance =
    vtkSmartPointer<vtkPolyDataDistance>::New();
  distance->SetInputConnection( 0, intersection->GetOutputPort( 1 ) );
  distance->SetInputConnection( 1, intersection->GetOutputPort( 2 ) );

  vtkSmartPointer<vtkThreshold> thresh1 =
    vtkSmartPointer<vtkThreshold>::New();
  thresh1->AllScalarsOn();
  thresh1->SetInputArrayToProcess
    ( 0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, "Distance" );
  thresh1->SetInputConnection( distance->GetOutputPort( 0 ) );

  vtkSmartPointer<vtkThreshold> thresh2 =
    vtkSmartPointer<vtkThreshold>::New();
  thresh2->AllScalarsOn();
  thresh2->SetInputArrayToProcess
    ( 0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, "Distance" );
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

  vtkSmartPointer<vtkDataSetSurfaceFilter> surface1 =
    vtkSmartPointer<vtkDataSetSurfaceFilter>::New();
  surface1->SetInputConnection( thresh1->GetOutputPort() );

  vtkSmartPointer<vtkDataSetSurfaceFilter> surface2 =
    vtkSmartPointer<vtkDataSetSurfaceFilter>::New();
  surface2->SetInputConnection( thresh2->GetOutputPort() );

  vtkSmartPointer<vtkReverseSense> reverseSense =
    vtkSmartPointer<vtkReverseSense>::New();
  reverseSense->SetInputConnection( surface2->GetOutputPort() );
  if ( operation == 2 ) // difference
    {
    reverseSense->ReverseCellsOn();
    reverseSense->ReverseNormalsOn();
    }

  vtkSmartPointer<vtkAppendPolyData> appender =
    vtkSmartPointer<vtkAppendPolyData>::New();
  appender->SetInputConnection( surface1->GetOutputPort() );
  if ( operation == 2)
    {
    appender->AddInputConnection( reverseSense->GetOutputPort() );
    }
  else
    {
    appender->AddInputConnection( surface2->GetOutputPort() );
    }

  vtkSmartPointer<vtkPolyDataMapper> mapper =
    vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputConnection( appender->GetOutputPort() );
  mapper->ScalarVisibilityOff();

  vtkActor *actor = vtkActor::New();
  actor->SetMapper( mapper );

  return actor;
}


int main(int argc, char* argv[])
{
  vtkSmartPointer<vtkRenderer> renderer =
    vtkSmartPointer<vtkRenderer>::New();

  vtkSmartPointer<vtkRenderWindow> renWin =
    vtkSmartPointer<vtkRenderWindow>::New();
  renWin->AddRenderer( renderer );

  vtkSmartPointer<vtkRenderWindowInteractor> renWinInteractor =
    vtkSmartPointer<vtkRenderWindowInteractor>::New();
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
