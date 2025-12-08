#include "pch.h"
#include "ETInteractorStyle.h"
#include "ETInteractorStyleTrackballActor.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "ETQVTKWidget.h"
#include "FunctionPanelRight.h"

#include "Util.h"
#include <array>
#include <Eigen/Dense>

vtkStandardNewMacro(ETInteractorStyleTrackballActor);

ETInteractorStyleTrackballActor::ETInteractorStyleTrackballActor()
{
	   
	m_bLbuttonDown = false;
	m_bMbuttonDown = false;
	m_bRbuttonDown = false;
	LastPickedActor = nullptr;
	LastPickedProperty = vtkSmartPointer<vtkProperty>::New();
	if (m_pPropPicker == nullptr)
		m_pPropPicker = vtkSmartPointer<vtkPropPicker>::New();	

}

ETInteractorStyleTrackballActor::~ETInteractorStyleTrackballActor()
{
	LastPickedActor->Delete();
}

void ETInteractorStyleTrackballActor::OnChar() // w, p 키 disable
{
	auto pressedKey = this->GetInteractor()->GetKeySym();
	if (pressedKey != "w")
	{
		vtkInteractorStyle::OnKeyPress();
	}
	if (pressedKey != "p")
	{
		vtkInteractorStyle::OnKeyPress();
	}
}


void ETInteractorStyleTrackballActor::OnLeftButtonDown()
{
	m_bLbuttonDown = true;
	int* clickPos = this->GetInteractor()->GetEventPosition();

	// Pick from this location.
	vtkSmartPointer<vtkRenderer> renderer = theApp.m_pVTKWidget->GetSceneRenderer();
	m_pPropPicker->Pick(clickPos[0], clickPos[1], 0, renderer);
	this->LastPickedActor = m_pPropPicker->GetActor();

	// 1. 빈 공간을 클릭했는지 확인 (카메라 네비게이션)
	if (this->LastPickedActor == nullptr)
	{
		theApp.m_pVTKWidget->interactor()->SetInteractorStyle(theApp.m_pVTKWidget->GetBaseInteractorStyle());
		theApp.m_pVTKWidget->GetBaseInteractorStyle()->m_Is_Trackball = true;
		theApp.m_pVTKWidget->GetBaseInteractorStyle()->OnLeftButtonDown();
		return;
	}

	// 2. 패널 모드에 따라 적절한 핸들러를 호출
	switch (theApp.PanelMode)
	{
	case E_GEOMETRY:
		HandleGeometryPick(this->LastPickedActor);
		break;

	case E_PHANTOM:
		HandlePhantomPick(this->LastPickedActor);
		break;

	default:
		// 다른 모드나 기본 처리가 필요하다면 여기에 작성
		break;
	}

	theApp.m_pVTKWidget->renderWindow()->Render();
	__super::OnLeftButtonDown(); // 해당 키가 동작하게 함
}

void ETInteractorStyleTrackballActor::HandleGeometryPick(vtkActor* pickedActor)
{
	for (auto const& pair : theApp.ObjectPanelActors)
	{
		// pair.first가 key, pair.second가 value(actor)에 해당합니다.
		int key = pair.first;
		vtkSmartPointer<vtkActor> actor = pair.second;

		if (pickedActor == actor)
		{
			b_IsObjectActorMoving = true;
			theApp.pRt->m_Object_SelectedIndex = key; // Object의 Key(ID) 저장

			// Status창 Mode 설정
			theApp.pRt->UpdateObject_Mode(key);

			// 버튼 Clicked 및 하이라이트 설정
			theApp.pRt->UpdateObject_ListButton(key);
			theApp.UpdateObject_ActorHighlighted(key);

			pickedActor->GetProperty()->SetOpacity(pickedActor->GetProperty()->GetOpacity() * 0.5);
			return; // 찾았으면 루프 종료
		}
	}
}

void ETInteractorStyleTrackballActor::HandlePhantomPick(vtkActor* pickedActor)
{
	for (auto const& pair : theApp.PhantomPanelActor)
	{
		// pair.first가 key, pair.second가 value(actor)에 해당합니다.
		int key = pair.first;
		vtkSmartPointer<vtkActor> actor = pair.second;
		if (pickedActor == actor)
		{
			b_IsPhantomActorMoving = true;
			theApp.pRt->m_Phantom_SelectedIndex = key; // Key값 (ID)을 저장

			// 버튼 Clicked 및 하이라이트 설정
			theApp.pRt->UpdatePhantom_ListButton(key);
			theApp.UpdatePhantom_ActorHighlighted(key);

			pickedActor->GetProperty()->SetOpacity(pickedActor->GetProperty()->GetOpacity() * 0.5);
			return; // 찾았으면 루프 종료
		}
	}
}

void ETInteractorStyleTrackballActor::OnLeftButtonUp()
{
	m_bLbuttonDown = false;

	if (b_IsObjectActorMoving)
	{
		theApp.pRt->SaveObject_InfoData_InMouseControl(theApp.pRt->m_Object_SelectedIndex); // Actor 기반의 데이터를 InfoStatus에 업데이트
		theApp.pRt->UpdateObject_InfoStatus(theApp.pRt->m_Object_SelectedIndex); // InfoStatus의 데이터를 InfoData 컨테이너에 업데이트

		theApp.ObjectPanelActors[theApp.pRt->m_Object_SelectedIndex]->GetProperty()->SetOpacity(theApp.ObjectPanelActors[theApp.pRt->m_Object_SelectedIndex]->GetProperty()->GetOpacity() * 2.0);
		b_IsObjectActorMoving = false;
	}
	else if (b_IsPhantomActorMoving) 
	{		
		theApp.pRt->SavePhantom_InfoData_InMouseControl(theApp.pRt->m_Phantom_SelectedIndex); // Actor 기반의 데이터를 InfoStatus에 업데이트
		theApp.pRt->UpdatePhantom_InfoStatus(theApp.pRt->m_Phantom_SelectedIndex); // InfoStatus의 데이터를 InfoData 컨테이너에 업데이트

		theApp.PhantomPanelActor[theApp.pRt->m_Phantom_SelectedIndex]->GetProperty()->SetOpacity(theApp.PhantomPanelActor[theApp.pRt->m_Phantom_SelectedIndex]->GetProperty()->GetOpacity() * 2.0);
		b_IsPhantomActorMoving = false;
	}

	theApp.m_pVTKWidget->renderWindow()->Render(); // 렌더링은 마지막에 한 번만 호출
	__super::OnLeftButtonUp();
}


void ETInteractorStyleTrackballActor::OnMiddleButtonDown()
{
	m_bMbuttonDown = true;
	theApp.m_pVTKWidget->renderWindow()->Render(); // blackout prevention
	__super::OnMiddleButtonDown();

	// 휠버튼 클릭 시 camera로	
	theApp.m_pVTKWidget->renderWindow()->GetInteractor()->SetInteractorStyle(theApp.m_pVTKWidget->GetBaseInteractorStyle());
}

void ETInteractorStyleTrackballActor::OnMiddleButtonUp()
{
	m_bMbuttonDown = false;
	theApp.m_pVTKWidget->renderWindow()->Render(); // blackout prevention
	__super::OnMiddleButtonUp();
}

void ETInteractorStyleTrackballActor::OnMouseWheelForward()
{	
	theApp.m_pVTKWidget->interactor()->SetInteractorStyle(theApp.m_pVTKWidget->GetBaseInteractorStyle());
	theApp.m_pVTKWidget->GetBaseInteractorStyle()->m_Is_Trackball = true;
	theApp.m_pVTKWidget->GetBaseInteractorStyle()->OnMouseWheelForward();
}


void ETInteractorStyleTrackballActor::OnMouseWheelBackward()
{
	theApp.m_pVTKWidget->interactor()->SetInteractorStyle(theApp.m_pVTKWidget->GetBaseInteractorStyle());
	theApp.m_pVTKWidget->GetBaseInteractorStyle()->m_Is_Trackball = true;
	theApp.m_pVTKWidget->GetBaseInteractorStyle()->OnMouseWheelBackward();
}

void ETInteractorStyleTrackballActor::OnRightButtonDown()
{	
	QMenu contextMenu;
	action1 = contextMenu.addAction("Camera View Control");
	action1->setCheckable(true);
	QObject::connect(action1, &QAction::triggered, [&]() { this->Action1Function_mouse(); });		

	QAction *action2 = contextMenu.addAction("Manipulation");
	action2->setCheckable(true);
	action2->setChecked(true);
	if (theApp.PanelMode == E_PHANTOM) action2->setText("Phantom Manipulation");
	if (theApp.PanelMode == E_GEOMETRY) action2->setText("Object Manipulation");
	QObject::connect(action2, &QAction::triggered, [&]() { this->Action2Function_mouse(); });

	//action3 = contextMenu.addAction("Undo");
	//action3->setCheckable(false);
	//QObject::connect(action3, &QAction::triggered, [&]() { this->Action3Function_mouse(); });

	contextMenu.exec(QCursor::pos());

	theApp.m_pVTKWidget->renderWindow()->Render(); // blackout prevention

}

void ETInteractorStyleTrackballActor::OnRightButtonUp()
{
	theApp.m_pVTKWidget->renderWindow()->Render(); // blackout prevention
	//우클릭 disable
	m_bRbuttonDown = false;

	__super::OnRightButtonUp();
}

void ETInteractorStyleTrackballActor::OnMouseMove()
{	
	__super::OnMouseMove();
}

void ETInteractorStyleTrackballActor::Action1Function_mouse()
{
	theApp.MouseControl_toCamera();
}

void ETInteractorStyleTrackballActor::Action2Function_mouse()
{
	theApp.MouseControl_toActor();
}

void ETInteractorStyleTrackballActor::Action3Function_mouse()
{
	
}


