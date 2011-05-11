/*=========================================================================

  Written by: Chris Weigle, University of North Carolina at Chapel Hill
  Email Contact: weigle@cs.unc.edu

  IN NO EVENT SHALL THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL BE
  LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR
  CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING OUT OF THE USE
  OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF NORTH
  CAROLINA HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.

  THE UNIVERSITY OF NORTH CAROLINA SPECIFICALLY DISCLAIM ANY WARRANTIES,
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
  AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE PROVIDED HEREUNDER IS
  ON AN "AS IS" BASIS, AND THE UNIVERSITY OF NORTH CAROLINA HAS NO
  OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR
  MODIFICATIONS.

=========================================================================*/
#ifndef __vtkPolyDataDistance_h
#define __vtkPolyDataDistance_h

#include <vtkPolyDataAlgorithm.h>

class vtkPolyDataDistance : public vtkPolyDataAlgorithm {
public:
  static vtkPolyDataDistance *New();
  vtkTypeRevisionMacro(vtkPolyDataDistance, vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Enable/disable computation of the signed distance between
  // the first poly data and the second poly data.
  vtkSetMacro(SignedDistance,int);
  vtkGetMacro(SignedDistance,int);
  vtkBooleanMacro(SignedDistance,int);

  // Description:
  // Enable/disable negation of the distance values.
  vtkSetMacro(NegateDistance,int);
  vtkGetMacro(NegateDistance,int);
  vtkBooleanMacro(NegateDistance,int);

  // Description:
  // Enable/disable computation of a second output poly data with the
  // distance from the first poly data at each point.
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
