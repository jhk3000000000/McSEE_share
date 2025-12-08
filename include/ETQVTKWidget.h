#pragma once

#include <QVTKOpenGLNativeWidget.h>

#include <vtkCamera.h>
#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkAssembly.h>
#include <vtkAssemblyPath.h>
#include <vtkAxesActor.h>
#include <vtkAxisActor.h>
#include <vtkCommand.h>
#include <vtkConeSource.h>
#include <vtkFollower.h>
#include <vtkInteractorStyle.h>
#include <vtkInteractorStyleTrackballActor.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProp3DCollection.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTextActor.h>
#include <vtkVectorText.h>
#include <vtkDoubleArray.h>
#include <vtkInteractorStyleRubberBandPick.h>
#include <qpushbutton.h>

class ETInteractorStyle;
class ETInteractorStyleTrackballActor;
class ETInteractorStyleRubberBand;
class ETQVTKWidget : public QVTKOpenGLNativeWidget
{
public:
	explicit ETQVTKWidget();
	virtual ~ETQVTKWidget();

public:
	void initialize();
	void SetBackGroundColor();
	void SetSceneRenderer(vtkSmartPointer<vtkRenderer> renderer);
	vtkSmartPointer<vtkRenderer> GetSceneRenderer();
	vtkSmartPointer<vtkRenderWindowInteractor> GetInteractorCamera() { return m_pInteractorCamera; };
	vtkSmartPointer<vtkRenderWindowInteractor> GetInteractorActor() { return m_pInteractorActor; };

	vtkSmartPointer<ETInteractorStyle> GetBaseInteractorStyle() { return m_pBaseInteractorStyle; };
	vtkSmartPointer<ETInteractorStyleTrackballActor> GetBaseInteractorStyleTrackballActor() { return m_pBaseInteractorStyleTrackballActor; };
	vtkSmartPointer<ETInteractorStyleRubberBand> GetBaseInteractorStyleRubberBand() { return m_pBaseInteractorStyleRubberBand; };
	void StippledLine(vtkActor* actor, int LineStipplePattern = 0xFFFF, int LineStippleRepeat = 1);
	void ForceNonPickableForFixedObjects(); // SetInteractorStyle로 Interactor 바꾸다보면 pickableoff가 풀리므로 이를 다시 pickableoff로 해줌.
	void SetVisibilityOn_AxesObjects();
	void SetVisibilityOff_AxesObjects();

	//vtkSmartPointer<vtkAxesActor> GetMovableAxesWidget() { return m_movableaxesWidget; };
	vtkNew<vtkAssembly> movableAxes;

	QList<QPushButton*> Buttons_ETQVTKWidget; // button 총 집합

	QPushButton *settingButton = nullptr;
	QPushButton *runButton = nullptr;
	QPushButton *stopButton = nullptr;
	QPushButton *resetButton = nullptr;
	

private:
	vtkSmartPointer<vtkRenderWindowInteractor> m_pInteractorCamera = nullptr;
	vtkSmartPointer<vtkRenderWindowInteractor> m_pInteractorActor = nullptr;
	vtkSmartPointer<vtkRenderWindowInteractor> m_pInteractorRubberBand = nullptr;
	vtkSmartPointer<vtkRenderer> m_pSceneRenderer = nullptr;
	vtkSmartPointer<ETInteractorStyle> m_pBaseInteractorStyle = nullptr;
	vtkSmartPointer<ETInteractorStyleTrackballActor> m_pBaseInteractorStyleTrackballActor = nullptr;
	vtkSmartPointer<ETInteractorStyleRubberBand> m_pBaseInteractorStyleRubberBand = nullptr;
	
//	vtkOrientationMarkerWidget* m_axesWidget = nullptr;
	vtkNew<vtkPropCollection> collection;
	vtkNew<vtkAxesActor> m_movableaxesWidget;
	vtkNew<vtkFollower> xLabel;
	vtkNew<vtkFollower> yLabel;
	vtkNew<vtkFollower> zLabel;
	vtkNew<vtkActor> LineXActor;
	vtkNew<vtkActor> LineYActor;
	vtkNew<vtkActor> LineZActor;

	void runButtonClicked();
	void stopButtonClicked();
	void resetButtonClicked();
	void settingButtonClicked();
	
};