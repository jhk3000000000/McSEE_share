#pragma once

#include <QtWidgets>
#include <vtkInteractorStyleTrackballActor.h>
#include <vtkNamedColors.h>
#include <vtkProperty.h>
#include <vtkAxis.h>
#include <vtkAxes.h>
#include <vtkActor.h>
#include <vtkAreaPicker.h>
#include <vtkRenderedAreaPicker.h>
#include <vtkDataSetMapper.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkExtractPolyDataGeometry.h>
#include <vtkIdFilter.h>
#include <vtkIdTypeArray.h>
#include <vtkInteractorStyleRubberBandPick.h>
#include <vtkObjectFactory.h>
#include <vtkPlanes.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkUnstructuredGrid.h>
#include <vtkVersion.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkHardwareSelector.h>
#include <vtkSelection.h>
#include <vtkSelectionNode.h>
#include <vtkSelectVisiblePoints.h>

#include <set>
#include "ETHuman3DApp.h"

#define VTKISRBP_ORIENT 0
#define VTKISRBP_SELECT 1

class ETInteractorStyleRubberBand : public vtkInteractorStyleRubberBandPick, public QObject
{
public:
	static ETInteractorStyleRubberBand *New();
	vtkTypeMacro(ETInteractorStyleRubberBand, vtkInteractorStyleRubberBandPick);

public:
	ETInteractorStyleRubberBand();
	virtual ~ETInteractorStyleRubberBand();


public:
	bool m_bLbuttonDown;
	bool m_bMbuttonDown;
	bool m_bRbuttonDown;

	bool b_IsUserClothingSelectThrough = true; // User-defiend clothing 제작 시 select through 켜졌을때 true
	bool b_IsClothingMakingDraggingOn = false; // User-defined clothing 제작 시 dragging mode 켜졌을때 true


public:
	virtual void OnLeftButtonDown() override;
	virtual void OnLeftButtonUp() override;
	virtual void OnMiddleButtonDown() override;
	virtual void OnMiddleButtonUp() override;
	virtual void OnRightButtonDown() override;
	virtual void OnRightButtonUp() override;
	virtual void OnMouseMove() override;
	virtual void OnMouseWheelForward() override;
	virtual void OnMouseWheelBackward() override;
	//virtual void OnChar() override;
		
	void Action1Function_Rubberband();
	void Action2Function_Rubberband();
	void Action3Function_Rubberband();
	void Action4Function_Rubberband();

	void SetPolyData(vtkSmartPointer<vtkPolyData> polyData_original, vtkSmartPointer<vtkPolyData> AccumulatedPolyData_original);
	void CallOnLeftButton();


protected:

private:
	vtkSmartPointer<vtkPolyData> OrigianlPolyData;
	vtkSmartPointer<vtkPolyData> AccumulatedPolyData; // Not used
	vtkSmartPointer<vtkActor> SelectedActor;
	vtkSmartPointer<vtkDataSetMapper> SelectedMapper;
};