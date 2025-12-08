#include "pch.h"
#include "ETInteractorStyle.h"
#include "ETInteractorStyleTrackballActor.h"
#include "ETQVTKWidget.h"
#include "FunctionPanelRight.h"
#include "vtkTextActor.h"
#include "vtkKdTree.h"


#include "Util.h"
#include <array>
#include <Eigen/Dense>

vtkStandardNewMacro(ETInteractorStyle);

ETInteractorStyle::ETInteractorStyle()
{
	if (m_pCellPicker == nullptr)
		m_pCellPicker = vtkSmartPointer<vtkCellPicker>::New();	

	if (m_pPointPicker == nullptr)
		m_pPointPicker = vtkSmartPointer<vtkPointPicker>::New();
		

	m_bLbuttonDown = false;
	m_bMbuttonDown = false;
	m_bRbuttonDown = false;	

}

ETInteractorStyle::~ETInteractorStyle()
{
}

void ETInteractorStyle::OnChar()
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

void ETInteractorStyle::OnLeftButtonDown()
{
	m_bLbuttonDown = true;

	int ctrl = this->Interactor->GetControlKey(); // ctrl 키를 눌러야 뭔가 생성되고 함	
	if ((theApp.st_Pick3D_sourceEP && ctrl) || (theApp.st_Pick3D_sourceCB && ctrl) || (theApp.st_Pick3D_sourceHP && ctrl))
	{
		if (!this->GetInteractor())
			return;

		int* mousePos = this->GetInteractor()->GetEventPosition();

		double pickedpos[3] = { 0, };
		double pickedpos_normal[3] = { 0, };

		// 마우스 피킹 포인트 SCREEN -> world
		vtkSmartPointer<vtkRenderer> renderer = theApp.m_pVTKWidget->GetSceneRenderer();
		m_pCellPicker->Pick(mousePos[0], mousePos[1], 0, renderer);
		m_pCellPicker->GetPickPosition(pickedpos);
		m_pCellPicker->GetPickNormal(pickedpos_normal);

		// 클릭 위치 임시 생성
		theApp.Selecting3DShpere(pickedpos);

		// 특정 패널에 값 전달 
		if (theApp.st_Pick3D_sourceEP) // EP 3D pick 중일때
		{
			theApp.pRt->PosX_sourceEP_QLineEdit->setText(theApp.getQStringNumberInSpecificDigit(pickedpos[0], 6));
			theApp.pRt->PosY_sourceEP_QLineEdit->setText(theApp.getQStringNumberInSpecificDigit(pickedpos[1], 6));
			theApp.pRt->PosZ_sourceEP_QLineEdit->setText(theApp.getQStringNumberInSpecificDigit(pickedpos[2], 6));
		}

		if (theApp.st_Pick3D_sourceCB) // CB 3D pick 중일때
		{
			theApp.pRt->m_lineEditConeBeamPointX->setText(theApp.getQStringNumberInSpecificDigit(pickedpos[0], 6));
			theApp.pRt->m_lineEditConeBeamPointY->setText(theApp.getQStringNumberInSpecificDigit(pickedpos[1], 6));
			theApp.pRt->m_lineEditConeBeamPointZ->setText(theApp.getQStringNumberInSpecificDigit(pickedpos[2], 6));
		}

		if (theApp.st_Pick3D_sourceHP) // HP 3D pick 중일때
		{
			theApp.Selecting3DShpere_Delete(); // 생성한 임시 점 지우기
			vtkSmartPointer<vtkActor> PickedActor = m_pCellPicker->GetActor();
			if (PickedActor) // 팬텀을 선택했을 때만
			{
				theApp.pRt->SelectedPointNormalVector_sourceHP[0] = pickedpos_normal[0];
				theApp.pRt->SelectedPointNormalVector_sourceHP[1] = pickedpos_normal[1];
				theApp.pRt->SelectedPointNormalVector_sourceHP[2] = pickedpos_normal[2];
				//double translated_x = pickedpos[0] + pickedpos_normal[0] * theApp.pRt->SeparationDist_SourceHP_QLineEdit->text().toFloat();
				//double translated_y = pickedpos[1] + pickedpos_normal[1] * theApp.pRt->SeparationDist_SourceHP_QLineEdit->text().toFloat();
				//double translated_z = pickedpos[2] + pickedpos_normal[2] * theApp.pRt->SeparationDist_SourceHP_QLineEdit->text().toFloat();
				theApp.pRt->PosX_SourceHP_QLineEdit->setText(theApp.getQStringNumberInSpecificDigit(pickedpos[0], 6));
				theApp.pRt->PosY_SourceHP_QLineEdit->setText(theApp.getQStringNumberInSpecificDigit(pickedpos[1], 6));
				theApp.pRt->PosZ_SourceHP_QLineEdit->setText(theApp.getQStringNumberInSpecificDigit(pickedpos[2], 6));
				double coords[3] = { pickedpos[0] , pickedpos[1] , pickedpos[2] };
				theApp.Selecting3DShpere(coords);
			}
		}

		theApp.m_pVTKWidget->renderWindow()->Render();
	}	   
		
	if (theApp.m_DosimeterPick3D && ctrl)
	{
		if (!this->GetInteractor())
			return;

		int* mousePos = this->GetInteractor()->GetEventPosition();

		double pickedpos[3] = { 0, };
		int	pickedID;

		// 마우스 피킹 포인트 SCREEN -> world
		vtkSmartPointer<vtkRenderer> renderer = theApp.m_pVTKWidget->GetSceneRenderer();
		m_pCellPicker->Pick(mousePos[0], mousePos[1], 0, renderer);
		m_pCellPicker->GetPickPosition(pickedpos); // 이 pickedpos는 실제 actor 상의 점, pos/rot 이동 후의 점의 좌표
		// 여기서 얻는 아이디는 누적 polydata의 ID임 -> 즉 안경이나 선량계가 있으면 문제가 생김

		int PhantomIdx = theApp.pRt->m_Phantom_SelectedIndex;
		double PosX = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][5];
		double PosY = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][6];
		double PosZ = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][7];
		double RotX = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][8];
		double RotY = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][9];
		double RotZ = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][10];
		const double PI = 3.141592 / 180; // 라디안 변환 
		double RadianX = RotX * PI;
		double RadianY = RotY * PI;
		double RadianZ = RotZ * PI;

		// picked 좌표(실제 위치)를 polydata_base에 일치하는 위치로 옮기기(즉, 이동 및 회전 진행)
		// Define the translation vectors
		Eigen::Vector3d translation(-PosX, -PosY, -PosZ); // 반대로 옮기기
		// Create the rotation matrices for each axis
		Eigen::Matrix3d rotation_x;
		rotation_x = Eigen::AngleAxisd(-RadianX, Eigen::Vector3d::UnitX());
		Eigen::Matrix3d rotation_y;
		rotation_y = Eigen::AngleAxisd(-RadianY, Eigen::Vector3d::UnitY());
		Eigen::Matrix3d rotation_z;
		rotation_z = Eigen::AngleAxisd(-RadianZ, Eigen::Vector3d::UnitZ());
		// Combine the rotation matrices
		Eigen::Matrix3d rotation = rotation_y * rotation_x * rotation_z; //Rotation Y -> X -> Z -> **Reverse of origianl rotation
		// Create the combined transformation matrix
		Eigen::Matrix4d transform = Eigen::Matrix4d::Identity();
		transform.block<3, 3>(0, 0) = rotation; // rotation
		Eigen::Vector4d point;
		point = Eigen::Vector4d(pickedpos[0], pickedpos[1], pickedpos[2], 1.0);
		point.head<3>() += translation.head<3>(); // translation
		point = transform * point; // rot 
		vtkSmartPointer<vtkKdTree> kdTree = vtkSmartPointer<vtkKdTree>::New();
		kdTree->BuildLocatorFromPoints(theApp.m_3DHumanData_MultiplePhantom[PhantomIdx].polydata_base); // picked 좌표에서 polydata_base의 존재하는 가장 가까운 점을 찾을거임
		double closestPointDist;
		int closestPointID = kdTree->FindClosestPoint(point[0], point[1], point[2], closestPointDist); // 이 ID가 polydata_base 존재하는 점 중 가장 가까운 점의 ID
		double closestPoint[3]; // 이 점이 polydata_base에 존재하는 점 중 picked된 점을 이동/회전을 역적용한뒤의 점과 가장 가까운 점을 찾음
		theApp.m_3DHumanData_MultiplePhantom[PhantomIdx].polydata_base->GetPoint(closestPointID, closestPoint);
		pickedID = closestPointID; // 이거는 base polydata의 ID로 접근
		////////////////////////////////////////////////
		// 피킹 유무 판단
		vtkSmartPointer<vtkActor> spPickedActor = m_pCellPicker->GetActor();
		if (spPickedActor)
		{
			int SelectedDosimeterIdx = theApp.pRt->m_Dosimeter_Selected_Index;
			theApp.RefreshDosimeter3DShpere(pickedpos, SelectedDosimeterIdx);			
			theApp.m_DosimeterInfo.Dosimeter_PointID = pickedID;
			double PickedPoint[3];
			vtkSmartPointer<vtkPoints> points = theApp.m_3DHumanData_MultiplePhantom[PhantomIdx].polydata_base->GetPoints();
			points->GetPoint(theApp.m_DosimeterInfo.Dosimeter_PointID, PickedPoint); //Polydata_base에서 ID를 통해 얻은 좌표 -> 실제 선량계 생성은 여기서 됨
			// x, y, z relative position line값 update(팬텀에 대한 상대적 위치임) -> 즉, 팬텀의 중심이 0,0,0일때(원점이동 및 스케일링은 적용된 polydata_base) 상대적인 좌표값
			// 그림을 위한 Pick한 좌표를 팬텀 중심에 대한 상대적 좌표로 바꾼 값 -> 이는 값 입력을 통한 location update와의 일관성을 유지하기 위해서임
			double TransforemdPickedPos[3];
			TransforemdPickedPos[0] = point[0];
			TransforemdPickedPos[1] = point[1];
			TransforemdPickedPos[2] = point[2];
			theApp.pRt->DosimeterPosX_QLineEdit->setText(QString::number(TransforemdPickedPos[0]));
			theApp.pRt->DosimeterPosY_QLineEdit->setText(QString::number(TransforemdPickedPos[1]));
			theApp.pRt->DosimeterPosZ_QLineEdit->setText(QString::number(TransforemdPickedPos[2]));
		}
	} 

	if (theApp.m_GlassesPick3D && ctrl)
	{
		if (!this->GetInteractor())
			return;

		int* mousePos = this->GetInteractor()->GetEventPosition();

		double pickedpos[3] = { 0, };
		double pickedpos_normal[3] = { 0, };
		int	pickedID;

		// 마우스 피킹 포인트 SCREEN -> world
		vtkSmartPointer<vtkRenderer> renderer = theApp.m_pVTKWidget->GetSceneRenderer();
		m_pCellPicker->Pick(mousePos[0], mousePos[1], 0, renderer);
		m_pCellPicker->GetPickPosition(pickedpos);
		m_pCellPicker->GetPickNormal(pickedpos_normal);

		// 안경/선량계 누적 폴리데이터에서 선택한 좌표(pickedpos)로부터 polydata_base의 좌표로의 변환과정
		int PhantomIdx = theApp.pRt->m_Phantom_SelectedIndex;

		double PosX = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][5];
		double PosY = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][6];
		double PosZ = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][7];

		double RotX = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][8];
		double RotY = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][9];
		double RotZ = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][10];
		const double PI = 3.141592 / 180; // 라디안 변환 
		double RadianX = RotX * PI;
		double RadianY = RotY * PI;
		double RadianZ = RotZ * PI;

		// picked 좌표(실제 위치)를 polydata_base에 일치하는 위치로 옮기기(즉, 이동 및 회전 진행)
		// Define the translation vectors
		Eigen::Vector3d translation(-PosX, -PosY, -PosZ); // 반대로 옮기기

		// Create the rotation matrices for each axis
		Eigen::Matrix3d rotation_x;
		rotation_x = Eigen::AngleAxisd(-RadianX, Eigen::Vector3d::UnitX());

		Eigen::Matrix3d rotation_y;
		rotation_y = Eigen::AngleAxisd(-RadianY, Eigen::Vector3d::UnitY());

		Eigen::Matrix3d rotation_z;
		rotation_z = Eigen::AngleAxisd(-RadianZ, Eigen::Vector3d::UnitZ());

		// Combine the rotation matrices
		Eigen::Matrix3d rotation = rotation_y * rotation_x * rotation_z; //Rotation Y -> X -> Z -> **Reverse of origianl rotation

		// Create the combined transformation matrix
		Eigen::Matrix4d transform = Eigen::Matrix4d::Identity();
		transform.block<3, 3>(0, 0) = rotation; // rotation

		Eigen::Vector4d pointMatrix;
		pointMatrix = Eigen::Vector4d(pickedpos[0], pickedpos[1], pickedpos[2], 1.0);
		pointMatrix.head<3>() += translation.head<3>(); // translation
		pointMatrix = transform * pointMatrix; // rot 

		vtkSmartPointer<vtkKdTree> kdTree = vtkSmartPointer<vtkKdTree>::New();
		kdTree->BuildLocatorFromPoints(theApp.m_3DHumanData_MultiplePhantom[PhantomIdx].polydata_base); // picked 좌표에서 polydata_base의 존재하는 가장 가까운 점을 찾을거임

		double closestPointDist;
		int closestPointID = kdTree->FindClosestPoint(pointMatrix[0], pointMatrix[1], pointMatrix[2], closestPointDist); // 이 ID가 polydata_base 존재하는 점 중 가장 가까운 점의 ID
		double closestPoint[3]; // 이 점이 polydata_base에 존재하는 점 중 picked된 점을 이동/회전을 역적용한뒤의 점과 가장 가까운 점을 찾음
		theApp.m_3DHumanData_MultiplePhantom[PhantomIdx].polydata_base->GetPoint(closestPointID, closestPoint);

		pickedID = closestPointID; // 이거는 base polydata의 ID로 접근

		// 피킹 유무 판단
		vtkSmartPointer<vtkActor> spPickedActor = m_pCellPicker->GetActor();
		if (spPickedActor)
		{
			if (theApp.flgNextGlasses == false) // 첫번째 right glasses 일때
			{
				// 다시 선택하여 이전에 선택했던 point 존재할 시 삭제
				theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(theApp.Glasses2_Point);
				theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(theApp.Glasses1_Point);

				// Create a Picked sphere
				vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
				sphereSource->SetCenter(0.0, 0.0, 0.0);
				sphereSource->SetRadius(0.3);

				// Make the surface smooth.
				sphereSource->SetPhiResolution(60);
				sphereSource->SetThetaResolution(30);

				vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
				mapper->SetInputConnection(sphereSource->GetOutputPort());

				vtkSmartPointer<vtkNamedColors> colors = vtkSmartPointer<vtkNamedColors>::New();

				// Picked point actor 생성
				theApp.Glasses1_Point = vtkSmartPointer<vtkActor>::New();
				theApp.Glasses1_Point->SetMapper(mapper);
				theApp.Glasses1_Point->GetProperty()->SetColor(colors->GetColor3d("Green").GetData());
				theApp.Glasses1_Point->SetPosition(pickedpos);
				theApp.Glasses1_Point->SetPickable(false);

				theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(theApp.Glasses1_Point);
				theApp.m_pVTKWidget->renderWindow()->Render();

				theApp.flgNextGlasses = true;

				// Importdata_obj_glasses에 넘길 정보 설정
				theApp.m_GlassesInfo.Glasses1_ID = pickedID; // Polydata_base에 해당하는 점 ID
			}

			else if (theApp.flgNextGlasses == true) // 두번째 left glasses 일때
			{

				// Create a Picked sphere
				vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
				sphereSource->SetCenter(0.0, 0.0, 0.0);
				sphereSource->SetRadius(0.3);

				// Make the surface smooth.
				sphereSource->SetPhiResolution(60);
				sphereSource->SetThetaResolution(30);

				vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
				mapper->SetInputConnection(sphereSource->GetOutputPort());

				vtkSmartPointer<vtkNamedColors> colors = vtkSmartPointer<vtkNamedColors>::New();

				// Picked point actor 생성
				theApp.Glasses2_Point = vtkSmartPointer<vtkActor>::New();
				theApp.Glasses2_Point->SetMapper(mapper);
				theApp.Glasses2_Point->GetProperty()->SetColor(colors->GetColor3d("Green").GetData());
				theApp.Glasses2_Point->SetPosition(pickedpos);
				theApp.Glasses2_Point->SetPickable(false);

				theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(theApp.Glasses2_Point);
				theApp.m_pVTKWidget->renderWindow()->Render();

				// Glasses point pick 끝난 뒤 프로세스
				theApp.flgNextGlasses = false;
				theApp.m_GlassesPick3D = false;

				// Importdata_obj_glasses에 넘길 정보 설정
				theApp.m_GlassesInfo.Glasses2_ID = pickedID;
			}
		}		
	}
		
	__super::OnLeftButtonDown();
}

void ETInteractorStyle::OnLeftButtonUp()
{
	m_bLbuttonDown = false;

	__super::OnLeftButtonUp();

	if (m_Is_Trackball)
	{
		theApp.m_pVTKWidget->interactor()->SetInteractorStyle(theApp.m_pVTKWidget->GetBaseInteractorStyleTrackballActor());
		m_Is_Trackball = false;
	}
}

void ETInteractorStyle::OnMiddleButtonDown()
{
	m_bMbuttonDown = true;
	

	__super::OnMiddleButtonDown();

	// Interactor를 바꿀때는 Interactor 함수가 다 끝나고 바꿔야 함
	// 휠버튼 클릭시 Mouse Actor로
	theApp.m_pVTKWidget->renderWindow()->GetInteractor()->SetInteractorStyle(theApp.m_pVTKWidget->GetBaseInteractorStyleTrackballActor());
}


void ETInteractorStyle::OnMiddleButtonUp()
{
	m_bMbuttonDown = false;
	
	__super::OnMiddleButtonUp();
}

void ETInteractorStyle::OnMouseWheelForward()
{
	
	if (this->CurrentRenderer == nullptr)
	{
		return;
	}

	vtkCamera* camera = this->CurrentRenderer->GetActiveCamera();
	if (camera)
	{
		//For orthographic projection
		double scale = camera->GetParallelScale();
		camera->SetParallelScale(scale * 0.85); // Zoom in

		//For 3D perspective view
		//camera->Dolly(1.15);  // Zoom in
		//this->CurrentRenderer->ResetCameraClippingRange();
		//this->GetInteractor()->Render();
	}
	theApp.m_pVTKWidget->renderWindow()->Render(); // blackout prevention

	if (m_Is_Trackball)
	{
		theApp.m_pVTKWidget->interactor()->SetInteractorStyle(theApp.m_pVTKWidget->GetBaseInteractorStyleTrackballActor());
		m_Is_Trackball = false;
	}
}


void ETInteractorStyle::OnMouseWheelBackward()
{
	
	if (this->CurrentRenderer == nullptr)
	{
		return;
	}

	vtkCamera* camera = this->CurrentRenderer->GetActiveCamera();
	if (camera)
	{
		//For orthographic projection
		double scale = camera->GetParallelScale();
		camera->SetParallelScale(scale / 0.85); // Zoom out

		//For 3D perspective view
		//camera->Dolly(0.85);  // Zoom out
		//this->CurrentRenderer->ResetCameraClippingRange();
		//this->GetInteractor()->Render();
	}
	theApp.m_pVTKWidget->renderWindow()->Render(); // blackout prevention

	if (m_Is_Trackball)
	{
		theApp.m_pVTKWidget->interactor()->SetInteractorStyle(theApp.m_pVTKWidget->GetBaseInteractorStyleTrackballActor());
		m_Is_Trackball = false;
	}
}

void ETInteractorStyle::OnRightButtonDown()
{
	if (theApp.PanelMode == E_PHANTOM || theApp.PanelMode == E_GEOMETRY || theApp.PanelMode == E_SOURCE && theApp.pRt->b_IsCalculationExecuted == false) // 팬텀 모드 이면서, skin 3d visualization은 꺼져 있을때만 가능
	{
		QMenu contextMenu;
		QAction *action1 = contextMenu.addAction("Camera Navigation");
		action1->setCheckable(true);
		action1->setChecked(true);
		QObject::connect(action1, &QAction::triggered, [&]() { this->Action1Function_camera(); });
		
		QAction *action2 = contextMenu.addAction("Manipulation");
		if (theApp.PanelMode == E_PHANTOM) action2->setText("Phantom Manipulation");
		if (theApp.PanelMode == E_GEOMETRY) action2->setText("Object Manipulation");
		QObject::connect(action2, &QAction::triggered, [&]() { this->Action2Function_camera(); });

		contextMenu.exec(QCursor::pos());

		m_bRbuttonDown = true;
	}
}
		

void ETInteractorStyle::OnRightButtonUp()
{
	m_bRbuttonDown = false;

	__super::OnRightButtonUp();
}

void ETInteractorStyle::OnMouseMove()
{
	QToolTip::hideText(); // tooltip 숨기고 시작

	int alt = this->Interactor->GetAltKey();
	int ctrl = this->Interactor->GetControlKey();
	int shift = this->Interactor->GetShiftKey();
	if (alt) // 현재 좌표
	{
		if (!this->GetInteractor())
			return;

		int* mousePos = this->GetInteractor()->GetEventPosition();

		double pickedpos[3] = { 0, };

		// 마우스 피킹 포인트 SCREEN -> world
		vtkSmartPointer<vtkRenderer> renderer = theApp.m_pVTKWidget->GetSceneRenderer();
		m_pCellPicker->Pick(mousePos[0], mousePos[1], 0, renderer);
		m_pCellPicker->GetPickPosition(pickedpos);
		
		// 현재 좌표 표시
		QWidget* renderWindowWidget = theApp.m_pVTKWidget; /* Get your QVTKOpenGLWidget or equivalent here */;
		QPoint globalPoint = renderWindowWidget->mapToGlobal(QPoint(mousePos[0], renderWindowWidget->height() - mousePos[1]));
		double heightFactor = theApp.pRt->Window_height / theApp.pRt->Default_window_height;
		globalPoint.setY(globalPoint.y() - renderWindowWidget->height() * 0.02 * heightFactor);
		QString tooltipValue = QString::number(pickedpos[0]) + ", " + QString::number(pickedpos[1]) + ", " + QString::number(pickedpos[2]);

		if (theApp.b_IsSkinDoseVisualizationOn)
		{
			vtkIdType cellId = m_pCellPicker->GetCellId();
			if (cellId != -1)
			{  // Check if we picked something
				vtkSmartPointer<vtkActor> pickedActor = m_pCellPicker->GetActor();
				int actorID = -1;
				for (const auto& entry : theApp.SkinPhantomActor)
				{
					if (entry.second == pickedActor)
					{
						actorID = entry.first;
						break;
					}
				}
				if (actorID == -1)
				{
					// Handle case where the actor was not found in the map, if needed
					return;
				}

				vtkSmartPointer<vtkCell> pickedCell = pickedActor->GetMapper()->GetInput()->GetCell(cellId);
				double dose = theApp.FacetDose[actorID][cellId];	
				double relative_error = theApp.FacetError[actorID][cellId];
				tooltipValue += "\nDose: " + QString::number(dose, 'e', 4) + " pGy" + theApp.pRt->DoseUnit_QString;
				// 상대 오차가 50%를 초과하는 경우 혹은 계산되지 않은 경우
				if (std::isnan(relative_error) || std::isinf(relative_error) || relative_error < 0)
				{
					// 통계적 신뢰도가 낮다는 경고 메시지를 표시합니다.
					tooltipValue += "\nError: statistically unreliable";
				}
				else // 상대 오차가 50% 이하인 경우
				{
					// 계산된 오차 값을 숫자로 표시합니다.
					double percentage_error = relative_error * 100.0;
					tooltipValue += "\nError: " + QString::number(percentage_error, 'f', 2) + " %";
				}
			}
		}
		QToolTip::showText(globalPoint, tooltipValue);
	}			
	__super::OnMouseMove();
}

void ETInteractorStyle::CallOnLeftButton() // 마우스 클릭 눌린 거 한 번 풀기
{
	this->OnLeftButtonDown();
	this->OnLeftButtonUp();
}

void ETInteractorStyle::Action1Function_camera()
{
	theApp.MouseControl_toCamera();
}

void ETInteractorStyle::Action2Function_camera()
{
	theApp.MouseControl_toActor();
}