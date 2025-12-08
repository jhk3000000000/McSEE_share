#pragma once

#include <QtWidgets>
#include <vtkInteractorStyleTrackballActor.h>
#include <vtkNamedColors.h>
#include <vtkProperty.h>
#include <vtkAxis.h>
#include <vtkAxes.h>
#include "ETHuman3DApp.h"

class ETInteractorStyleTrackballActor : public vtkInteractorStyleTrackballActor, public QObject
{
public:
	static ETInteractorStyleTrackballActor *New();
	vtkTypeMacro(ETInteractorStyleTrackballActor, vtkInteractorStyle);

public:
	ETInteractorStyleTrackballActor();
	virtual ~ETInteractorStyleTrackballActor();


public:
	bool m_bLbuttonDown;
	bool m_bMbuttonDown;
	bool m_bRbuttonDown;	

	bool b_IsPhantomActorMoving = false;
	bool b_IsObjectActorMoving = false;
	bool b_IsObjectGroupActor = false;

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
	virtual void OnChar() override;

	QAction* action1 = nullptr;
	QAction* action2 = nullptr;
	QAction* action3 = nullptr;

	void Action1Function_mouse();
	void Action2Function_mouse();
	void Action3Function_mouse();


protected:

private:

	void HandleGeometryPick(vtkActor* pickedActor);
	void HandlePhantomPick(vtkActor* pickedActor);
	vtkSmartPointer<vtkActor> LastPickedActor;	
	vtkSmartPointer<vtkAssembly> LastPickedAssembly;
	vtkSmartPointer<vtkProperty> LastPickedProperty;
	vtkSmartPointer<vtkPropPicker> m_pPropPicker = nullptr;
	
};