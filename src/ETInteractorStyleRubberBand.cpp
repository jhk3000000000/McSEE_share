#include "pch.h"
#include "ETInteractorStyle.h"
#include "ETInteractorStyleTrackballActor.h"
#include "ETInteractorStyleRubberBand.h"
#include "ETQVTKWidget.h"
#include "FunctionPanelRight.h"
#include "vtkKdTree.h"
#include "vtkUnsignedCharArray.h"

#include "Util.h"
#include <array>
#include <Eigen/Dense>

class Vector3 {
public:
	double x;
	double y;
	double z;

	// Constructor
	Vector3(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}

	// Copy constructor
	Vector3(const Vector3& other) : x(other.x), y(other.y), z(other.z) {}

	// Assignment operator
	Vector3& operator=(const Vector3& other) {
		x = other.x;
		y = other.y;
		z = other.z;
		return *this;
	}

	// Dot product
	double dot(const Vector3& other) const {
		return x * other.x + y * other.y + z * other.z;
	}
	
	// Cross product
	Vector3 cross(const Vector3& other) const {
		return Vector3(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
	}

	// Magnitude
	double magnitude() const {
		return sqrt(x * x + y * y + z * z);
	}

	// Normalize
	Vector3 normalize() const {
		double mag = magnitude();
		if (mag == 0.0) {
			return Vector3(0.0, 0.0, 0.0);
		}
		else {
			return Vector3(x / mag, y / mag, z / mag);
		}
	}
};

vtkStandardNewMacro(ETInteractorStyleRubberBand);

ETInteractorStyleRubberBand::ETInteractorStyleRubberBand() : vtkInteractorStyleRubberBandPick()
{
	m_bLbuttonDown = false;
	m_bMbuttonDown = false;
	m_bRbuttonDown = false;

	this->SelectedMapper = vtkSmartPointer<vtkDataSetMapper>::New();
	this->SelectedActor = vtkSmartPointer<vtkActor>::New();
	this->SelectedActor->SetMapper(SelectedMapper);
}

ETInteractorStyleRubberBand::~ETInteractorStyleRubberBand()
{
	
}

void ETInteractorStyleRubberBand::SetPolyData(vtkSmartPointer<vtkPolyData> polyData_original, vtkSmartPointer<vtkPolyData> AccumulatedPolyData_original)
{
	this->OrigianlPolyData = polyData_original;
	this->AccumulatedPolyData = AccumulatedPolyData_original; // Not-used
}

void ETInteractorStyleRubberBand::OnRightButtonDown()
{
	QMenu contextMenu;
	QAction *action1 = contextMenu.addAction("Camera View Control");
	action1->setCheckable(true);
	if(b_IsClothingMakingDraggingOn == false) action1->setChecked(true);
	QObject::connect(action1, &QAction::triggered, [&]() { this->Action1Function_Rubberband(); });

	QAction *action2 = contextMenu.addAction("Mouse Making");
	action2->setCheckable(true);
	if (b_IsClothingMakingDraggingOn == true) action2->setChecked(true);
	QObject::connect(action2, &QAction::triggered, [&]() { this->Action2Function_Rubberband(); });

	QAction *action3 = contextMenu.addAction("Select Through");
	action3->setCheckable(true);
	if (b_IsUserClothingSelectThrough == true) action3->setChecked(true);
	QObject::connect(action3, &QAction::triggered, [&]() { this->Action3Function_Rubberband(); });

	QAction *action4 = contextMenu.addAction("Done");
	action4->setCheckable(false);
	QObject::connect(action4, &QAction::triggered, [&]() { this->Action4Function_Rubberband(); });

	contextMenu.exec(QCursor::pos());

	m_bRbuttonDown = true;

	theApp.m_pVTKWidget->renderWindow()->Render(); // blackout prevention
	//if (this->CurrentMode == VTKISRBP_SELECT)
	//{
	//	m_bRbuttonDown = true;
	//	theApp.m_pVTKWidget->renderWindow()->Render(); // blackout prevention
	//	__super::OnRightButtonDown(); // 해당 키가 동작하게 함
	//}
	//else
	//{
	//	m_bRbuttonDown = true;
	//	theApp.m_pVTKWidget->renderWindow()->Render(); // blackout prevention
	//	__super::OnLeftButtonDown(); // 해당 키가 동작하게 함
	//}

}

void ETInteractorStyleRubberBand::OnRightButtonUp()
{
	__super::OnRightButtonUp();
	theApp.m_pVTKWidget->renderWindow()->Render(); // blackout prevention
	//if (this->CurrentMode == VTKISRBP_SELECT)
	//{
	//	m_bRbuttonDown = false;
	//	theApp.m_pVTKWidget->renderWindow()->Render(); // blackout prevention
	//	__super::OnRightButtonUp();
	//}
	//else
	//{
	//	m_bRbuttonDown = false;
	//	theApp.m_pVTKWidget->renderWindow()->Render(); // blackout prevention
	//	__super::OnLeftButtonUp();
	//}

}

//
void ETInteractorStyleRubberBand::OnMiddleButtonDown()
{
	m_bMbuttonDown = true;

	// Toggle between select and zoom mode
	if (this->CurrentMode == VTKISRBP_SELECT)
	{
		b_IsClothingMakingDraggingOn = false;
		this->CurrentMode = VTKISRBP_ORIENT;
	}
	else if (this->CurrentMode == VTKISRBP_ORIENT)
	{
		b_IsClothingMakingDraggingOn = true;
		this->CurrentMode = VTKISRBP_SELECT;
	}
	
	theApp.m_pVTKWidget->renderWindow()->Render(); // blackout prevention	
	__super::OnMiddleButtonDown();
}


void ETInteractorStyleRubberBand::OnMiddleButtonUp()
{
	m_bMbuttonDown = false;
	theApp.m_pVTKWidget->renderWindow()->Render(); // blackout prevention
	__super::OnMiddleButtonUp();
}

void ETInteractorStyleRubberBand::OnMouseWheelForward()
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
}


void ETInteractorStyleRubberBand::OnMouseWheelBackward()
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
}

void ETInteractorStyleRubberBand::OnLeftButtonDown()
{
	m_bLbuttonDown = true;
	theApp.m_pVTKWidget->renderWindow()->Render(); // blackout prevention
	__super::OnLeftButtonDown();
	//if (this->CurrentMode == VTKISRBP_SELECT)
	//{
	//	m_bLbuttonDown = true;
	//	theApp.m_pVTKWidget->renderWindow()->Render(); // blackout prevention
	//	__super::OnLeftButtonDown();
	//}
	//else
	//{
	//	m_bLbuttonDown = true;
	//	theApp.m_pVTKWidget->renderWindow()->Render(); // blackout prevention
	//	__super::OnRightButtonDown();
	//}

}

void ETInteractorStyleRubberBand::OnLeftButtonUp()

{
	m_bLbuttonDown = false;
	if (b_IsClothingMakingDraggingOn == true) this->CurrentMode = VTKISRBP_SELECT;
	if (b_IsClothingMakingDraggingOn == false) this->CurrentMode = VTKISRBP_ORIENT;

	if (this->CurrentMode == VTKISRBP_SELECT)
	{
		__super::OnLeftButtonUp(); // 먼저 실행해주어야 드래그-앤-드랍할때 바로 적용됨
		
		vtkSmartPointer<vtkRenderedAreaPicker> renderedPicker = vtkSmartPointer<vtkRenderedAreaPicker>::New();
		this->GetInteractor()->SetPicker(renderedPicker);
		int* startPos = this->StartPosition;
		int* endPos = this->EndPosition;

		vtkRenderer* renderer = this->GetInteractor()->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
		renderedPicker->AreaPick(startPos[0], startPos[1], endPos[0], endPos[1], renderer);
		vtkPlanes* frustum = renderedPicker->GetFrustum();		

		vtkSmartPointer<vtkExtractPolyDataGeometry> extractPolyDataGeometry = vtkSmartPointer<vtkExtractPolyDataGeometry>::New();
		extractPolyDataGeometry->SetInputData(this->OrigianlPolyData); // Rub으로 select 하는건 TransformedRotatedBasePolydata(OriginalPolydta는 의복이 누적되지 않은 이동/회전 적용된 baspolydata)로부터 만든 데이터로진행
		extractPolyDataGeometry->SetImplicitFunction(frustum);
		extractPolyDataGeometry->Update();		
		
		vtkSmartPointer<vtkPolyData> ExtractedPolydata = vtkSmartPointer<vtkPolyData>::New();
		ExtractedPolydata = extractPolyDataGeometry->GetOutput();
		vtkSmartPointer<vtkPolyDataNormals> NormalFilter = vtkSmartPointer<vtkPolyDataNormals>::New();
		NormalFilter->SetInputData(ExtractedPolydata);		
		NormalFilter->ComputePointNormalsOn();
		NormalFilter->ComputeCellNormalsOff();
		NormalFilter->Update();
		vtkPolyData* NormalPolydata = NormalFilter->GetOutput();
		vtkSmartPointer<vtkFloatArray> ExtractedNormals = vtkFloatArray::SafeDownCast(NormalPolydata->GetPointData()->GetNormals());

		vtkSmartPointer<vtkPoints> ExtractedPoints = vtkSmartPointer<vtkPoints>::New(); // 안경/선량계를 제외하고 이동/회전 적용된 polydata 상에서 select된 점들
		ExtractedPoints = ExtractedPolydata->GetPoints();		

		float skyblue_color[3] = { 62, 120, 219 }; // SkyBlue color
		float skin_color[3] = { 255, 192, 160 };
		vtkNew<vtkUnsignedCharArray> colors;
		colors->DeepCopy(theApp.PhantomPanelAccumulatedPolyData[theApp.pRt->m_Phantom_SelectedIndex]->GetPointData()->GetScalars());
		vtkSmartPointer<vtkKdTree> kdTree = vtkSmartPointer<vtkKdTree>::New();
		//kdTree->BuildLocatorFromPoints(this->AccumulatedPolyData); // Point scalar가 동일한 안경/선량계 누적팬텀을 transform한 팬텀
		kdTree->BuildLocatorFromPoints(this->OrigianlPolyData); // 어차피 kdTree에서 의복 제외한 origianl 팬텀의 좌표를 통해서만 의복을 만듦(pointID는 안씀)

		vtkRenderWindow* renderWindow = this->GetInteractor()->GetRenderWindow();
		int* size = renderWindow->GetSize();
		float* zBuffer = renderWindow->GetZbufferData(0, 0, size[0] - 1, size[1] - 1);

		const float depthTolerance = 0.005f;  // 해당 값이 너무 크면 실제로 보이는 점도 제외될 수 있음. -> Z값 비교가 아닌 실제 화면 픽셀 컬링 방식으로 개선 가능

		for (int i = 0; i < ExtractedPoints->GetNumberOfPoints(); ++i)
		{
			double point[3];
			ExtractedPoints->GetPoint(i, point);
			double normal[3];
			ExtractedNormals->GetTuple(i, normal);

			if (!b_IsUserClothingSelectThrough) // Select-through disabled
			{
				// Project point into SCREEN space
				this->GetInteractor()->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->SetWorldPoint(point[0], point[1], point[2], 1.0);
				this->GetInteractor()->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->WorldToDisplay();
				double displayCoords[3];
				this->GetInteractor()->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->GetDisplayPoint(displayCoords);

				int x = static_cast<int>(displayCoords[0]);
				int y = static_cast<int>(displayCoords[1]);

				// Check if point is inside the SCREEN bounds
				if (x >= 0 && x < size[0] && y >= 0 && y < size[1])
				{
					float zValue = zBuffer[x + y * size[0]];

					// Check if the point is actually visible (depth comparison)
					if (displayCoords[2] > zValue + depthTolerance) // Point is occluded
					{
						continue; // Skip occluded points
					}

					// Perform dot product check for point facing the camera
					Vector3 CameraDirection(theApp.m_pVTKWidget->GetSceneRenderer()->GetActiveCamera()->GetDirectionOfProjection()[0],
						theApp.m_pVTKWidget->GetSceneRenderer()->GetActiveCamera()->GetDirectionOfProjection()[1],
						theApp.m_pVTKWidget->GetSceneRenderer()->GetActiveCamera()->GetDirectionOfProjection()[2]);
					Vector3 NowNormal(normal[0], normal[1], normal[2]);

					if (CameraDirection.dot(NowNormal) > 0) // Skip points not facing the camera
					{
						continue;
					}
				}
				else
				{
					continue; // Skip points outside SCREEN bounds
				}
			}

			const double radius = 0.1; // cm 단위 기준, 해당 거리 내 좌표들은 모두 포함됨
			vtkSmartPointer<vtkIdList> closePointIds = vtkSmartPointer<vtkIdList>::New();
			kdTree->FindPointsWithinRadius(radius, point, closePointIds);

			for (vtkIdType k = 0; k < closePointIds->GetNumberOfIds(); ++k)
			{
				int nearbyPointID = closePointIds->GetId(k);

				// 부동소수점 오차를 다시 한 번 체크 (선택사항)
				double nearbyPoint[3];
				this->OrigianlPolyData->GetPoint(nearbyPointID, nearbyPoint);
				double distSquared = vtkMath::Distance2BetweenPoints(point, nearbyPoint);
				if (distSquared >= radius * radius) continue; 

				int ctrl = this->Interactor->GetControlKey();
				if (!ctrl) // select
				{
					if (theApp.RubberBandSelectedPointIDs.find(nearbyPointID) == theApp.RubberBandSelectedPointIDs.end())
					{
						theApp.RubberBandSelectedPointIDs.insert(nearbyPointID);
						colors->SetTuple(nearbyPointID, skyblue_color);
					}
				}
				else // unselect
				{
					auto iter = theApp.RubberBandSelectedPointIDs.find(nearbyPointID);
					if (iter != theApp.RubberBandSelectedPointIDs.end())
					{
						theApp.RubberBandSelectedPointIDs.erase(iter);
						colors->SetTuple(nearbyPointID, skin_color);
					}
				}
			}
			
		}
		theApp.PhantomPanelAccumulatedPolyData[theApp.pRt->m_Phantom_SelectedIndex]->GetPointData()->SetScalars(colors); // 실제 움직이는 팬텀은 누적 polydata
		theApp.m_pVTKWidget->renderWindow()->Render();

		__super::OnLeftButtonUp();
		//theApp.m_pVTKWidget->renderWindow()->Render(); // blackout prevention
		//__super::OnLeftButtonUp();
		//__super::OnLeftButtonDown();
	}
	else
	{
		__super::OnLeftButtonUp();
	}	
}


void ETInteractorStyleRubberBand::OnMouseMove()
{
	 //마우스 움직일때마다 현재 상태 파악("r" 버튼 눌렀는 지)
	if (b_IsClothingMakingDraggingOn == true) // dragging mode가 켜졌을때
	{
		this->CurrentMode = VTKISRBP_SELECT;
	}
	else
	{
		this->CurrentMode = VTKISRBP_ORIENT;
	}

	__super::OnMouseMove();
}

void ETInteractorStyleRubberBand::Action1Function_Rubberband()
{
	this->CurrentMode = VTKISRBP_ORIENT;
	b_IsClothingMakingDraggingOn = false;

}

void ETInteractorStyleRubberBand::Action2Function_Rubberband()
{
	this->CurrentMode = VTKISRBP_SELECT;
	b_IsClothingMakingDraggingOn = true;
}

void ETInteractorStyleRubberBand::Action3Function_Rubberband()
{		
	b_IsUserClothingSelectThrough = !b_IsUserClothingSelectThrough;
}

void ETInteractorStyleRubberBand::Action4Function_Rubberband()
{
	theApp.RubberBandUserClothingGenerate();
	theApp.pRt->AllButtonEnabled();
}

void ETInteractorStyleRubberBand::CallOnLeftButton() // 마우스 클릭 눌린 거 한 번 풀기
{
	this->OnLeftButtonDown();
	this->OnLeftButtonUp();
}