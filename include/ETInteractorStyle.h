#pragma once

#include <QtWidgets>
#include <vtkInteractorStyleTrackballCamera.h>
#include "ETHuman3DApp.h"
#include "vtkPointPicker.h"
#include "vtkWorldPointPicker.h"

class ETInteractorStyle : public vtkInteractorStyleTrackballCamera, public QObject
{
public:
	static ETInteractorStyle *New();
	vtkTypeMacro(ETInteractorStyle, vtkInteractorStyle);

public:
	ETInteractorStyle();
	virtual ~ETInteractorStyle();


public:
	bool m_bLbuttonDown;
	bool m_bMbuttonDown;
	bool m_bRbuttonDown;
	bool m_Is_Trackball = false;

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
	void CallOnLeftButton(); // 마우스 클릭 눌린 거 한 번 풀기

	void Action1Function_camera();
	void Action2Function_camera();


protected:

private:
	vtkSmartPointer<vtkCellPicker> m_pCellPicker = nullptr;
	vtkSmartPointer<vtkPointPicker> m_pPointPicker = nullptr;
};