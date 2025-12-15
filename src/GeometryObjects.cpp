// GeometryObjects.cpp
#include "pch.h" 
#include "GeometryObjects.h"
#include "FunctionPanelRight.h"
#include "ETQVTKWidget.h"
// [추가] VTK 관련 헤더 파일들 (이것들이 없으면 에러가 납니다!)
#include <vtkSmartPointer.h>
#include <vtkCubeSource.h>      // Box 만들 때 필요
#include <vtkSphereSource.h>    // Sphere 만들 때 필요
#include <vtkCylinderSource.h>  // Cylinder 만들 때 필요
#include <vtkAppendFilter.h>    // PolyData 합칠 때 필요
#include <vtkGeometryFilter.h>  // GeometryFilter 필요
#include <vtkPolyDataMapper.h>  // Mapper 필요
#include <vtkActor.h>           // Actor 필요
#include <vtkProperty.h>        // SetColor, SetOpacity 등 속성 설정 시 필요

// --- [새로 추가해야 할 헤더들 (Sphere 생성용)] ---
#include <vtkDelaunay2D.h>            // <--- 현재 에러의 원인!
#include <vtkTransform.h>             // 위치 이동/회전용
#include <vtkTransformPolyDataFilter.h> // 데이터 변환용
#include <vtkPoints.h>                // 점 데이터용
#include <vtkMath.h>                  // 수학 연산용 (M_PI 등)
#include <cmath>                      // 표준 수학 라이브러리

#ifndef M_PI
	#define M_PI 3.14159265358979323846   // M_PI가 정의되지 않았을 경우를 대비
#endif

#include <vtkUnstructuredGrid.h>
#include "Util.h"

static void AddAzimuthalAngularPoints(vtkSmartPointer<vtkPoints> points, double radius, double startAngle, double endAngle, double FixedAngle, int numPoints) {
	double deltaAngle = (endAngle - startAngle) / (numPoints - 1);
	double PolarAngle = vtkMath::RadiansFromDegrees(FixedAngle);
	for (int i = 0; i < numPoints; ++i) {
		double AzimuthalAngle = vtkMath::RadiansFromDegrees(startAngle + i * deltaAngle);
		double x = radius * sin(PolarAngle) * cos(AzimuthalAngle);
		double y = radius * sin(PolarAngle) * sin(AzimuthalAngle);
		double z = radius * cos(PolarAngle);
		points->InsertNextPoint(x, y, z);
	}
}

static void AddPartialCirclePoints(vtkSmartPointer<vtkPoints> points, double radius, double startAngle, double endAngle, int numPoints) {
	double deltaAngle = (endAngle - startAngle) / (numPoints - 1);
	for (int i = 0; i < numPoints; ++i) {
		double angle = startAngle + i * deltaAngle;
		double radians = vtkMath::RadiansFromDegrees(angle);
		double x = radius * cos(radians);
		double y = radius * sin(radians);
		points->InsertNextPoint(x, y, 0.0);
	}
}

// Box 구현
void BoxObject::Generate(int objectID) {
	bool isUpdate = false;
	if (theApp.ObjectPanelActors[objectID] != nullptr)
	{
		isUpdate = true;
		theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(theApp.ObjectPanelActors[objectID]);
		theApp.ObjectPanelActors[objectID] = nullptr; // ObjectActor 삭제
		theApp.m_pVTKWidget->renderWindow()->Render();
		theApp.ObjectPanelPolydata_Collection[objectID] = nullptr; // ObjectPolydata 삭제
	}

	// Get global variables 
	double XCenter = theApp.pRt->m_Object_MainInfo[objectID][1];
	double YCenter = theApp.pRt->m_Object_MainInfo[objectID][2];
	double ZCenter = theApp.pRt->m_Object_MainInfo[objectID][3];
	double XRotate = theApp.pRt->m_Object_MainInfo[objectID][4];
	double YRotate = theApp.pRt->m_Object_MainInfo[objectID][5];
	double ZRotate = theApp.pRt->m_Object_MainInfo[objectID][6];
	double HalfLenngthX = theApp.pRt->m_Object_MainInfo[objectID][7];
	double HalfLenngthY = theApp.pRt->m_Object_MainInfo[objectID][8];
	double HalfLenngthZ = theApp.pRt->m_Object_MainInfo[objectID][9];
	double ThicknessX = theApp.pRt->m_Object_MainInfo[objectID][10];
	double ThicknessY = theApp.pRt->m_Object_MainInfo[objectID][11];
	double ThicknessZ = theApp.pRt->m_Object_MainInfo[objectID][12];

	std::vector<vtkSmartPointer<vtkPolyData>> GeneratedPolyDataVector;
	// Create the outer cube
	vtkSmartPointer<vtkCubeSource> outerCube = vtkSmartPointer<vtkCubeSource>::New();
	outerCube->SetXLength(2 * HalfLenngthX);
	outerCube->SetYLength(2 * HalfLenngthY);
	outerCube->SetZLength(2 * HalfLenngthZ);
	outerCube->SetCenter(0, 0, 0);
	outerCube->Update();
	GeneratedPolyDataVector.push_back(outerCube->GetOutput());

	if (ThicknessX != 0 && ThicknessY != 0 && ThicknessZ != 0) // Inner cube
	{
		// Create the inner cube
		vtkSmartPointer<vtkCubeSource> innerCube = vtkSmartPointer<vtkCubeSource>::New();
		innerCube->SetXLength(2 * HalfLenngthX - 2 * ThicknessX);
		innerCube->SetYLength(2 * HalfLenngthY - 2 * ThicknessY);
		innerCube->SetZLength(2 * HalfLenngthZ - 2 * ThicknessZ);
		innerCube->SetCenter(0, 0, 0);
		innerCube->Update();
		GeneratedPolyDataVector.push_back(innerCube->GetOutput());
	}

	// Appending two parts
	vtkSmartPointer<vtkPolyData> AppendedPolyData = vtkSmartPointer<vtkPolyData>::New();
	AppendedPolyData = GeneratedPolyDataVector[0];
	for (auto itr_PolyData : GeneratedPolyDataVector)
	{
		vtkSmartPointer<vtkAppendFilter> appendFilter = vtkSmartPointer<vtkAppendFilter>::New();
		appendFilter->AddInputData(AppendedPolyData);
		appendFilter->AddInputData(itr_PolyData);
		appendFilter->Update();

		vtkSmartPointer<vtkGeometryFilter> geomFilter = vtkSmartPointer<vtkGeometryFilter>::New(); // For making appendfilter to polydata 
		geomFilter->SetInputData(appendFilter->GetOutput());
		geomFilter->Update();

		AppendedPolyData = geomFilter->GetOutput();
	}
	theApp.ObjectPanelPolydata_Collection[objectID] = AppendedPolyData;

	// Visualize
	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(theApp.ObjectPanelPolydata_Collection[objectID]);
	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);
	theApp.ObjectPanelActors[objectID] = actor;

	theApp.ObjectPanelActors[objectID]->SetProperty(Util::GetProperty());
	theApp.ObjectPanelActors[objectID]->GetProperty()->SetColor(0.5, 0.5, 0.5);

	if(isUpdate) theApp.ObjectPanelActors[objectID]->GetProperty()->SetOpacity(1 - (double(theApp.pRt->m_Object_Transparency[objectID]) / 100));
	else theApp.ObjectPanelActors[objectID]->GetProperty()->SetOpacity(0.5);
	theApp.ObjectPanelActors[objectID]->SetPosition(XCenter, YCenter, ZCenter);
	theApp.ObjectPanelActors[objectID]->SetOrientation(XRotate, YRotate, ZRotate);

	theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(theApp.ObjectPanelActors[objectID]);
	theApp.m_pVTKWidget->renderWindow()->Render();
}

void BoxObject::UpdateUI(int objectID) 
{
    // 안전장치: Actor가 없으면 중단
    if (theApp.ObjectPanelActors.find(objectID) == theApp.ObjectPanelActors.end() || 
        theApp.ObjectPanelActors[objectID] == nullptr) {
        return;
    }

    vtkActor* actor = theApp.ObjectPanelActors[objectID];

    // 1. 값 가져오기
    double* pCenter = actor->GetPosition();
    double* pRotation = actor->GetOrientation();

    // 2. 값 변환 (기존 앱의 포맷팅 함수 사용)
    QString strPosX = theApp.getQStringNumberInSpecificDigit(pCenter[0], 6);
    QString strPosY = theApp.getQStringNumberInSpecificDigit(pCenter[1], 6);
    QString strPosZ = theApp.getQStringNumberInSpecificDigit(pCenter[2], 6);

    QString strRotX = theApp.getQStringNumberInSpecificDigit(pRotation[0], 6);
    QString strRotY = theApp.getQStringNumberInSpecificDigit(pRotation[1], 6);
    QString strRotZ = theApp.getQStringNumberInSpecificDigit(pRotation[2], 6);

    // 3. Box 전용 UI 업데이트
    if (theApp.pRt) {
        theApp.pRt->ObjectBox_CenterX_QLineEdit->setText(strPosX);
        theApp.pRt->ObjectBox_CenterY_QLineEdit->setText(strPosY);
        theApp.pRt->ObjectBox_CenterZ_QLineEdit->setText(strPosZ);

        theApp.pRt->ObjectBox_RotateX_QLineEdit->setText(strRotX);
        theApp.pRt->ObjectBox_RotateY_QLineEdit->setText(strRotY);
        theApp.pRt->ObjectBox_RotateZ_QLineEdit->setText(strRotZ);
    }
}


// Sphere 구현
void SphereObject::Generate(int objectID) {
	bool isUpdate = false;
	if (theApp.ObjectPanelActors[objectID] != nullptr)
	{
		isUpdate = true;
		theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(theApp.ObjectPanelActors[objectID]);
		theApp.ObjectPanelActors[objectID] = nullptr; // ObjectActor 삭제
		theApp.m_pVTKWidget->renderWindow()->Render();
		theApp.ObjectPanelPolydata_Collection[objectID]= nullptr; // ObjectPolydata 삭제
	}

	// Get global variables 
	double XCenter = theApp.pRt->m_Object_MainInfo[objectID][1];
	double YCenter = theApp.pRt->m_Object_MainInfo[objectID][2];
	double ZCenter = theApp.pRt->m_Object_MainInfo[objectID][3];
	double XRotate = theApp.pRt->m_Object_MainInfo[objectID][4];
	double YRotate = theApp.pRt->m_Object_MainInfo[objectID][5];
	double ZRotate = theApp.pRt->m_Object_MainInfo[objectID][6];
	double InnerRadius = theApp.pRt->m_Object_MainInfo[objectID][14];
	double OuterRadius = theApp.pRt->m_Object_MainInfo[objectID][15];
	double AzimuthalStartingAngle = theApp.pRt->m_Object_MainInfo[objectID][16];
	double AzimuthalDeltaAngle = theApp.pRt->m_Object_MainInfo[objectID][17];	
	double AzimuthalEndAngle = AzimuthalStartingAngle + AzimuthalDeltaAngle;
	double PolarStartingAngle = theApp.pRt->m_Object_MainInfo[objectID][18];
	double PolarDeltaAngle = theApp.pRt->m_Object_MainInfo[objectID][19];		
	double PolarEndAngle = PolarStartingAngle + PolarDeltaAngle;

	// Case 1: No Angle / No inner radius
	if (InnerRadius == 0 && AzimuthalDeltaAngle == 360 && PolarDeltaAngle == 180)
	{
		vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
		sphereSource->SetCenter(0, 0, 0);
		sphereSource->SetRadius(1.0);
		sphereSource->SetThetaResolution(50);
		sphereSource->SetPhiResolution(50);
		sphereSource->Update();
		vtkSmartPointer<vtkTransform> transform1 = vtkSmartPointer<vtkTransform>::New();
		transform1->PostMultiply(); //this is the key line
		transform1->Scale(OuterRadius, OuterRadius, OuterRadius); // radius = 1 에서 시작
		vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter1 = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
		transformFilter1->SetInputConnection(sphereSource->GetOutputPort());
		transformFilter1->SetTransform(transform1);
		transformFilter1->Update();
		theApp.ObjectPanelPolydata_Collection[objectID] = transformFilter1->GetOutput();
	}	
	// Case 2: No Angle / Yes inner radius 
	if (InnerRadius != 0 && AzimuthalDeltaAngle == 360 && PolarDeltaAngle == 180)
	{
		std::vector<vtkSmartPointer<vtkPolyData>> GeneratedPolyDataVector;	
		vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
		sphereSource->SetCenter(0, 0, 0);
		sphereSource->SetRadius(1.0);
		sphereSource->SetThetaResolution(50);
		sphereSource->SetPhiResolution(50);
		sphereSource->Update();

		// Create the outer sphere
		vtkSmartPointer<vtkTransform> transform1 = vtkSmartPointer<vtkTransform>::New();
		transform1->PostMultiply(); //this is the key line
		transform1->Scale(OuterRadius, OuterRadius, OuterRadius); // radius = 1 에서 시작
		vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter1 = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
		transformFilter1->SetInputConnection(sphereSource->GetOutputPort());
		transformFilter1->SetTransform(transform1);
		transformFilter1->Update();
		GeneratedPolyDataVector.push_back(transformFilter1->GetOutput());

		// Create the inner sphere
		vtkSmartPointer<vtkTransform> transform2 = vtkSmartPointer<vtkTransform>::New();
		transform2->PostMultiply(); //this is the key line
		transform2->Scale(InnerRadius, InnerRadius, InnerRadius); // radius = 1 에서 시작
		vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter2 = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
		transformFilter2->SetInputConnection(sphereSource->GetOutputPort());
		transformFilter2->SetTransform(transform2);
		transformFilter2->Update();
		GeneratedPolyDataVector.push_back(transformFilter2->GetOutput());
				
		// Appending two parts
		vtkSmartPointer<vtkPolyData> AppendedPolyData = vtkSmartPointer<vtkPolyData>::New();
		AppendedPolyData = GeneratedPolyDataVector[0];
		for (auto itr_PolyData : GeneratedPolyDataVector)
		{
			vtkSmartPointer<vtkAppendFilter> appendFilter = vtkSmartPointer<vtkAppendFilter>::New();
			appendFilter->AddInputData(AppendedPolyData);
			appendFilter->AddInputData(itr_PolyData);
			appendFilter->Update();

			vtkSmartPointer<vtkGeometryFilter> geomFilter = vtkSmartPointer<vtkGeometryFilter>::New(); // For making appendfilter to polydata 
			geomFilter->SetInputData(appendFilter->GetOutput());
			geomFilter->Update();

			AppendedPolyData = geomFilter->GetOutput();
		}
		theApp.ObjectPanelPolydata_Collection[objectID] = AppendedPolyData;
	}
	// Case 3: Yes Angle / No inner radius && Case 4: Yes Angle / Yes inner radius
	if (AzimuthalDeltaAngle != 360 || PolarDeltaAngle != 180)
	{
		std::vector<vtkSmartPointer<vtkPolyData>> GeneratedPolyDataVector;
		//Generate partial sphere surface
		double StandardSphereRadius = 1.0;
		double startAzimuthalAngle = AzimuthalStartingAngle;
		double deltaAzimuthalAngle = AzimuthalDeltaAngle;
		double AzimuthalIntervalCount = std::ceil(deltaAzimuthalAngle);
		double startPolarAngle = PolarStartingAngle;
		double deltaPolarAngle = PolarDeltaAngle;
		double PolarIntervalCount = std::ceil(deltaPolarAngle);

		int AzimuthalSeparationCount = 0;
		double endAzimuthalAngle = startAzimuthalAngle + deltaAzimuthalAngle;
		if (startAzimuthalAngle <= 180 && endAzimuthalAngle <= 180) AzimuthalSeparationCount = 1;
		if (startAzimuthalAngle <= 180 && endAzimuthalAngle > 180) AzimuthalSeparationCount = 2;
		if (startAzimuthalAngle > 180 && endAzimuthalAngle > 180) AzimuthalSeparationCount = 1;

		int PolarSeparationCount = 0;
		double endPolarAngle = startPolarAngle + deltaPolarAngle;
		if (startPolarAngle <= 90 && endPolarAngle <= 90) PolarSeparationCount = 1;
		if (startPolarAngle <= 90 && endPolarAngle > 90) PolarSeparationCount = 2;
		if (startPolarAngle > 90 && endPolarAngle > 90) PolarSeparationCount = 1;

		std::vector<vtkSmartPointer<vtkPolyData>> GeneratedSurfacePolyData;
		for (int PolarSeparation = 0; PolarSeparation < PolarSeparationCount; PolarSeparation++)
		{
			double SeparationStartPolarAngle = startPolarAngle + PolarSeparation * (90 - startPolarAngle);
			double SeparationEndPolarAngle;
			if (endPolarAngle <= 90) SeparationEndPolarAngle = endPolarAngle;
			if (endPolarAngle > 90 && PolarSeparationCount == 1) SeparationEndPolarAngle = endPolarAngle;
			if (endPolarAngle > 90 && PolarSeparationCount == 2) SeparationEndPolarAngle = 90 + PolarSeparation * (endPolarAngle - 90);
			double SeparationDeltaPolarAngle = SeparationEndPolarAngle - SeparationStartPolarAngle;
			double SeparationPolarIntervalCount = std::ceil(SeparationDeltaPolarAngle);
			for (int AzimuthalSeparation = 0; AzimuthalSeparation < AzimuthalSeparationCount; AzimuthalSeparation++)
			{
				vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
				double SeparationStartAzimuthalAngle = startAzimuthalAngle + AzimuthalSeparation * (180 - startAzimuthalAngle);
				double SeparationEndAzimuthalAngle;
				if (endAzimuthalAngle <= 180) SeparationEndAzimuthalAngle = endAzimuthalAngle;
				if (endAzimuthalAngle > 180 && AzimuthalSeparationCount == 1) SeparationEndAzimuthalAngle = endAzimuthalAngle;
				if (endAzimuthalAngle > 180 && AzimuthalSeparationCount == 2) SeparationEndAzimuthalAngle = 180 + AzimuthalSeparation * (endAzimuthalAngle - 180);
				double SeparationDeltaAzimuthalAngle = SeparationEndAzimuthalAngle - SeparationStartAzimuthalAngle;
				double SeparationAzimuthalIntervalCount = std::ceil(SeparationDeltaAzimuthalAngle);
				for (int AzimuthalInterval = 0; AzimuthalInterval < SeparationAzimuthalIntervalCount + 1; AzimuthalInterval++)
				{
					for (int PolarInterval = 0; PolarInterval < SeparationPolarIntervalCount + 1; PolarInterval++)
					{
						double PolarAngle = vtkMath::RadiansFromDegrees(SeparationStartPolarAngle + SeparationDeltaPolarAngle / SeparationPolarIntervalCount * PolarInterval);
						double AzimuthalAngle = vtkMath::RadiansFromDegrees(SeparationStartAzimuthalAngle + SeparationDeltaAzimuthalAngle / SeparationAzimuthalIntervalCount * AzimuthalInterval);
						// Convert spherical coordinates to Cartesian
						double x = StandardSphereRadius * sin(PolarAngle) * cos(AzimuthalAngle);
						double y = StandardSphereRadius * sin(PolarAngle) * sin(AzimuthalAngle);
						double z = StandardSphereRadius * cos(PolarAngle);
						// Add point to the points array
						points->InsertNextPoint(x, y, z);
					}
				}
				vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
				polyData->SetPoints(points);

				vtkSmartPointer<vtkDelaunay2D> delaunay = vtkSmartPointer<vtkDelaunay2D>::New();
				delaunay->SetInputData(polyData);
				double alphaFactor = 3; // Adjust this
				delaunay->SetAlpha(2 * M_PI*StandardSphereRadius / 360 * alphaFactor); // Need for optimized(2*pi*r/360 is partial delimeter of circle(or surface of sphere) per 1 degree.
				delaunay->Update();

				GeneratedSurfacePolyData.push_back(delaunay->GetOutput());
			}
		}
		vtkSmartPointer<vtkPolyData> AppendedSurfacePolyData = GeneratedSurfacePolyData[0];
		for (int count = 0; count < AzimuthalSeparationCount * PolarSeparationCount - 1; count++)
		{
			vtkSmartPointer<vtkAppendFilter> appendFilter = vtkSmartPointer<vtkAppendFilter>::New();
			appendFilter->AddInputData(AppendedSurfacePolyData);
			appendFilter->AddInputData(GeneratedSurfacePolyData[count + 1]);
			appendFilter->Update();

			vtkSmartPointer<vtkGeometryFilter> geomFilter = vtkSmartPointer<vtkGeometryFilter>::New(); // For making appendfilter to polydata 
			geomFilter->SetInputData(appendFilter->GetOutput());
			geomFilter->Update();

			AppendedSurfacePolyData = geomFilter->GetOutput();
		}

		// Outer sphere surface
		vtkSmartPointer<vtkPolyData> outerSpherePolyData = vtkSmartPointer<vtkPolyData>::New();
		outerSpherePolyData->DeepCopy(AppendedSurfacePolyData);
		vtkSmartPointer<vtkTransform> transform1 = vtkSmartPointer<vtkTransform>::New();
		transform1->PostMultiply(); //this is the key line
		transform1->Scale(OuterRadius, OuterRadius, OuterRadius);
		vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter1 = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
		transformFilter1->SetInputData(outerSpherePolyData);
		transformFilter1->SetTransform(transform1);
		transformFilter1->Update();
		GeneratedPolyDataVector.push_back(transformFilter1->GetOutput());
		// Inner sphere surface
		if (InnerRadius != 0) // Just for inner radius exist
		{
			vtkSmartPointer<vtkPolyData> InnerSpherePolyData = vtkSmartPointer<vtkPolyData>::New();
			InnerSpherePolyData->DeepCopy(AppendedSurfacePolyData);
			vtkSmartPointer<vtkTransform> transform2 = vtkSmartPointer<vtkTransform>::New();
			transform2->PostMultiply(); //this is the key line
			transform2->Scale(InnerRadius, InnerRadius, InnerRadius);
			vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter2 = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
			transformFilter2->SetInputData(InnerSpherePolyData);
			transformFilter2->SetTransform(transform2);
			transformFilter2->Update();
			GeneratedPolyDataVector.push_back(transformFilter2->GetOutput());
		}
		
		//Generate mesh of inner surface of partial sphere	
		std::vector<vtkSmartPointer<vtkPolyData>> GeneratedCuttedPolyData;
		double normarlizedOuterRadius;
		double normarlizedInnerRadius;
		if (InnerRadius == 0)
		{
			normarlizedOuterRadius = 1.0;
			normarlizedInnerRadius = 0.0;
		}
		else
		{
			normarlizedOuterRadius = OuterRadius / InnerRadius;
			normarlizedInnerRadius = 1.0;
		}

		int radialIntervalCount = std::ceil(normarlizedOuterRadius - normarlizedInnerRadius); // 대략 1cm 정도로 radial interval을 갖도록
		int radialPoints = 15; // 1cm 두께마다 radial 방향으로 몇개 찍을지
		bool startPolarUsed = false;
		bool endPolarUsed = false;
		bool startAzimuthalUsed = false;
		bool endAzimuthalUsed = false;
		if (startPolarAngle == 0) // StartPolar 사용 X
		{
			if (endPolarAngle == 180) // EndPolar 사용 X
			{
				if (deltaAzimuthalAngle == 360) // Azimuthal 사용 X
				{
					theApp.SetMessageBox("This is whole sphere! This shouldn't be happened");
				}
				if (deltaAzimuthalAngle != 360) // Azimuthal 사용 O (Partial azimuthal 존재)
				{
					startPolarUsed = false; endPolarUsed = false; startAzimuthalUsed = true; endAzimuthalUsed = true;
				}
			}
			if (endPolarAngle != 180) // EndPolar 사용 O
			{
				if (deltaAzimuthalAngle == 360) // Azimuthal 사용 X
				{
					startPolarUsed = false; endPolarUsed = true; startAzimuthalUsed = false; endAzimuthalUsed = false;
				}
				if (deltaAzimuthalAngle != 360) // Azimuthal 사용 O (Partial azimuthal 존재)
				{
					startPolarUsed = false; endPolarUsed = true; startAzimuthalUsed = true; endAzimuthalUsed = true;
				}
			}
		}
		if (startPolarAngle != 0) // StartPolar 사용 O
		{
			if (endPolarAngle == 180) // EndPolar 사용 X
			{
				if (deltaAzimuthalAngle == 360) // Azimuthal 사용 X
				{
					startPolarUsed = true; endPolarUsed = false; startAzimuthalUsed = false; endAzimuthalUsed = false;
				}
				if (deltaAzimuthalAngle != 360) // Azimuthal 사용 O (Partial azimuthal 존재)
				{
					startPolarUsed = true; endPolarUsed = false; startAzimuthalUsed = true; endAzimuthalUsed = true;
				}
			}
			if (endPolarAngle != 180) // EndPolar 사용 O
			{
				if (deltaAzimuthalAngle == 360) // Azimuthal 사용 X
				{
					startPolarUsed = true; endPolarUsed = true; startAzimuthalUsed = false; endAzimuthalUsed = false;
				}
				if (deltaAzimuthalAngle != 360) // Azimuthal 사용 O (Partial azimuthal 존재)
				{
					startPolarUsed = true; endPolarUsed = true; startAzimuthalUsed = true; endAzimuthalUsed = true;
				}
			}
		}
		if (startAzimuthalUsed)
		{
			std::vector<vtkSmartPointer<vtkPolyData>> GeneratedPartialPolyData;
			double startAngle = startPolarAngle; // Start Polar Angle
			double endAngle = endPolarAngle; // End Polar Angle
			int numPoints = std::ceil((endAngle - startAngle) / 2); // 대략 2도당 1개씩 찍히도록
			if (numPoints < 10) numPoints = 10; // 최소 10개 보장
			for (int radialInterval = 0; radialInterval < radialIntervalCount; radialInterval++)
			{
				vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
				double radialIntervalOuterRadius = normarlizedInnerRadius + (normarlizedOuterRadius - normarlizedInnerRadius) / (radialIntervalCount) * (radialInterval + 1);
				double radialIntervalInnerRadius = normarlizedInnerRadius + (normarlizedOuterRadius - normarlizedInnerRadius) / (radialIntervalCount)* radialInterval;
				for (int i = 0; i < radialPoints; i++) // 대략 0.1 cm 씩 (radial방향으로 10개 찍힘)
				{
					AddPartialCirclePoints(points, radialIntervalInnerRadius + (radialIntervalOuterRadius - radialIntervalInnerRadius) / (radialPoints - 1)*i, startAngle, endAngle, numPoints);
				}
				// Create a vtkPolyData object and set the points
				vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
				polyData->SetPoints(points);

				vtkSmartPointer<vtkDelaunay2D> delaunay = vtkSmartPointer<vtkDelaunay2D>::New();
				delaunay->SetInputData(polyData);
				double alphaFactor = 3; // Adjust this
				delaunay->SetAlpha(2 * M_PI*radialIntervalOuterRadius / 360 * alphaFactor); // Need for optimized (~0.6 for minimum radius)
				delaunay->Update();

				vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
				transform->PostMultiply(); //this is the key line
				if (InnerRadius == 0) transform->Scale(OuterRadius, OuterRadius, 1.0);
				if (InnerRadius != 0) transform->Scale(InnerRadius, InnerRadius, 1.0);
				// Rotate to fit the sphere position
				transform->RotateY(-90); // 
				transform->RotateZ(-90 + startAzimuthalAngle);
				vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
				transformFilter->SetInputData(delaunay->GetOutput());
				transformFilter->SetTransform(transform);
				transformFilter->Update();

				GeneratedPartialPolyData.push_back(transformFilter->GetOutput());
			}
			vtkSmartPointer<vtkPolyData> AppendedPartialPolyData = GeneratedPartialPolyData[0];
			for (int radialInterval = 0; radialInterval < radialIntervalCount - 1; radialInterval++)
			{
				vtkSmartPointer<vtkAppendFilter> appendFilter = vtkSmartPointer<vtkAppendFilter>::New();
				appendFilter->AddInputData(AppendedPartialPolyData);
				appendFilter->AddInputData(GeneratedPartialPolyData[radialInterval + 1]);
				appendFilter->Update();

				vtkSmartPointer<vtkGeometryFilter> geomFilter = vtkSmartPointer<vtkGeometryFilter>::New(); // For making appendfilter to polydata 
				geomFilter->SetInputData(appendFilter->GetOutput());
				geomFilter->Update();

				AppendedPartialPolyData = geomFilter->GetOutput();
			}
			GeneratedCuttedPolyData.push_back(AppendedPartialPolyData);
		}
		if (endAzimuthalUsed)
		{
			std::vector<vtkSmartPointer<vtkPolyData>> GeneratedPartialPolyData;
			double startAngle = startPolarAngle; // Start Polar Angle
			double endAngle = endPolarAngle; // End Polar Angle
			int numPoints = std::ceil((endAngle - startAngle) / 2); // 대략 2도당 1개씩 찍히도록
			if (numPoints < 10) numPoints = 10; // 최소 10개 보장
			for (int radialInterval = 0; radialInterval < radialIntervalCount; radialInterval++)
			{
				vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
				double radialIntervalOuterRadius = normarlizedInnerRadius + (normarlizedOuterRadius - normarlizedInnerRadius) / (radialIntervalCount) * (radialInterval + 1);
				double radialIntervalInnerRadius = normarlizedInnerRadius + (normarlizedOuterRadius - normarlizedInnerRadius) / (radialIntervalCount)* radialInterval;
				for (int i = 0; i < radialPoints; i++) // 대략 0.1 cm 씩 (radial방향으로 10개 찍힘)
				{
					AddPartialCirclePoints(points, radialIntervalInnerRadius + (radialIntervalOuterRadius - radialIntervalInnerRadius) / (radialPoints - 1)*i, startAngle, endAngle, numPoints);
				}
				// Create a vtkPolyData object and set the points
				vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
				polyData->SetPoints(points);

				vtkSmartPointer<vtkDelaunay2D> delaunay = vtkSmartPointer<vtkDelaunay2D>::New();
				delaunay->SetInputData(polyData);
				double alphaFactor = 3; // Adjust this
				delaunay->SetAlpha(2 * M_PI*radialIntervalOuterRadius / 360 * alphaFactor); // Need for optimized (~0.6 for minimum radius)
				delaunay->Update();

				vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
				transform->PostMultiply(); //this is the key line
				if (InnerRadius == 0) transform->Scale(OuterRadius, OuterRadius, 1.0);
				if (InnerRadius != 0) transform->Scale(InnerRadius, InnerRadius, 1.0);
				// Rotate to fit the sphere position
				transform->RotateY(-90);
				transform->RotateZ(-90 + endAzimuthalAngle);
				vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
				transformFilter->SetInputData(delaunay->GetOutput());
				transformFilter->SetTransform(transform);
				transformFilter->Update();

				GeneratedPartialPolyData.push_back(transformFilter->GetOutput());
			}
			vtkSmartPointer<vtkPolyData> AppendedPartialPolyData = GeneratedPartialPolyData[0];
			for (int radialInterval = 0; radialInterval < radialIntervalCount - 1; radialInterval++)
			{
				vtkSmartPointer<vtkAppendFilter> appendFilter = vtkSmartPointer<vtkAppendFilter>::New();
				appendFilter->AddInputData(AppendedPartialPolyData);
				appendFilter->AddInputData(GeneratedPartialPolyData[radialInterval + 1]);
				appendFilter->Update();

				vtkSmartPointer<vtkGeometryFilter> geomFilter = vtkSmartPointer<vtkGeometryFilter>::New(); // For making appendfilter to polydata 
				geomFilter->SetInputData(appendFilter->GetOutput());
				geomFilter->Update();

				AppendedPartialPolyData = geomFilter->GetOutput();
			}
			GeneratedCuttedPolyData.push_back(AppendedPartialPolyData);
		}
		if (startPolarUsed)
		{
			std::vector<vtkSmartPointer<vtkPolyData>> GeneratedPartialPolyData;
			for (int radialInterval = 0; radialInterval < radialIntervalCount; radialInterval++) // Radial part (1 cm 두께)
			{
				vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
				double radialIntervalOuterRadius = normarlizedInnerRadius + (normarlizedOuterRadius - normarlizedInnerRadius) / (radialIntervalCount) * (radialInterval + 1);
				double radialIntervalInnerRadius = normarlizedInnerRadius + (normarlizedOuterRadius - normarlizedInnerRadius) / (radialIntervalCount)* radialInterval;
				double startAngle = startAzimuthalAngle;
				double endAngle = endAzimuthalAngle;
				int angularPoints = std::ceil((endAngle - startAngle) / 2); // 대략 1도당 1개씩 찍히도록
				if (angularPoints < 10) angularPoints = 10; // 최소 10개 보장
				for (int i = 0; i < radialPoints; i++) // radial 방향으로 증가하면서 찍기
				{
					AddAzimuthalAngularPoints(points, radialIntervalInnerRadius + (radialIntervalOuterRadius - radialIntervalInnerRadius) / (radialPoints - 1)*i, startAngle, endAngle, startPolarAngle, angularPoints); // 해당 radius 에서 angular 방향으로 증가하면서 찍기
				}
				// Create a vtkPolyData object and set the points
				vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
				polyData->SetPoints(points);

				vtkSmartPointer<vtkDelaunay2D> delaunay = vtkSmartPointer<vtkDelaunay2D>::New();
				delaunay->SetInputData(polyData);
				double alphaFactor = 3; // Adjust this
				delaunay->SetAlpha(2 * M_PI*radialIntervalOuterRadius / 360 * alphaFactor);
				delaunay->Update();

				vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
				transform->PostMultiply(); //this is the key line
				if (InnerRadius == 0) transform->Scale(OuterRadius, OuterRadius, OuterRadius);
				if (InnerRadius != 0) transform->Scale(InnerRadius, InnerRadius, InnerRadius);
				vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
				transformFilter->SetInputData(delaunay->GetOutput());
				transformFilter->SetTransform(transform);
				transformFilter->Update();

				GeneratedPartialPolyData.push_back(transformFilter->GetOutput());
			}
			vtkSmartPointer<vtkPolyData> AppendedPartialPolyData = vtkSmartPointer<vtkPolyData>::New();
			AppendedPartialPolyData = GeneratedPartialPolyData[0];
			for (int radialInterval = 0; radialInterval < radialIntervalCount - 1; radialInterval++)
			{
				vtkSmartPointer<vtkAppendFilter> appendFilter = vtkSmartPointer<vtkAppendFilter>::New();
				appendFilter->AddInputData(AppendedPartialPolyData);
				appendFilter->AddInputData(GeneratedPartialPolyData[radialInterval + 1]);
				appendFilter->Update();

				vtkSmartPointer<vtkGeometryFilter> geomFilter = vtkSmartPointer<vtkGeometryFilter>::New(); // For making appendfilter to polydata 
				geomFilter->SetInputData(appendFilter->GetOutput());
				geomFilter->Update();

				AppendedPartialPolyData = geomFilter->GetOutput();
			}
			GeneratedCuttedPolyData.push_back(AppendedPartialPolyData);
		}
		if (endPolarUsed)
		{
			std::vector<vtkSmartPointer<vtkPolyData>> GeneratedPartialPolyData;
			for (int radialInterval = 0; radialInterval < radialIntervalCount; radialInterval++) // Radial part (1 cm 두께)
			{
				vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
				double radialIntervalOuterRadius = normarlizedInnerRadius + (normarlizedOuterRadius - normarlizedInnerRadius) / (radialIntervalCount) * (radialInterval + 1);
				double radialIntervalInnerRadius = normarlizedInnerRadius + (normarlizedOuterRadius - normarlizedInnerRadius) / (radialIntervalCount)* radialInterval;
				double startAngle = startAzimuthalAngle;
				double endAngle = endAzimuthalAngle;
				int angularPoints = std::ceil((endAngle - startAngle) / 2); // 대략 1도당 1개씩 찍히도록
				if (angularPoints < 10) angularPoints = 10; // 최소 10개 보장
				for (int i = 0; i < radialPoints; i++) // radial 방향으로 증가하면서 찍기
				{
					AddAzimuthalAngularPoints(points, radialIntervalInnerRadius + (radialIntervalOuterRadius - radialIntervalInnerRadius) / (radialPoints - 1)*i, startAngle, endAngle, endPolarAngle, angularPoints); // 해당 radius 에서 angular 방향으로 증가하면서 찍기
				}
				// Create a vtkPolyData object and set the points
				vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
				polyData->SetPoints(points);

				vtkSmartPointer<vtkDelaunay2D> delaunay = vtkSmartPointer<vtkDelaunay2D>::New();
				delaunay->SetInputData(polyData);
				double alphaFactor = 3; // Adjust this
				delaunay->SetAlpha(2 * M_PI*radialIntervalOuterRadius / 360 * alphaFactor);
				delaunay->Update();

				vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
				transform->PostMultiply(); //this is the key line
				if (InnerRadius == 0) transform->Scale(OuterRadius, OuterRadius, OuterRadius);
				if (InnerRadius != 0) transform->Scale(InnerRadius, InnerRadius, InnerRadius);
				vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
				transformFilter->SetInputData(delaunay->GetOutput());
				transformFilter->SetTransform(transform);
				transformFilter->Update();

				GeneratedPartialPolyData.push_back(transformFilter->GetOutput());
			}
			vtkSmartPointer<vtkPolyData> AppendedPartialPolyData = vtkSmartPointer<vtkPolyData>::New();
			AppendedPartialPolyData = GeneratedPartialPolyData[0];
			for (int radialInterval = 0; radialInterval < radialIntervalCount - 1; radialInterval++)
			{
				vtkSmartPointer<vtkAppendFilter> appendFilter = vtkSmartPointer<vtkAppendFilter>::New();
				appendFilter->AddInputData(AppendedPartialPolyData);
				appendFilter->AddInputData(GeneratedPartialPolyData[radialInterval + 1]);
				appendFilter->Update();

				vtkSmartPointer<vtkGeometryFilter> geomFilter = vtkSmartPointer<vtkGeometryFilter>::New(); // For making appendfilter to polydata 
				geomFilter->SetInputData(appendFilter->GetOutput());
				geomFilter->Update();

				AppendedPartialPolyData = geomFilter->GetOutput();
			}
			GeneratedCuttedPolyData.push_back(AppendedPartialPolyData);
		}
		vtkSmartPointer<vtkPolyData> AppendedCuttedPolyData = vtkSmartPointer<vtkPolyData>::New();
		AppendedCuttedPolyData = GeneratedCuttedPolyData[0];
		for (auto itr_CuttedPolyData : GeneratedCuttedPolyData)
		{
			vtkSmartPointer<vtkAppendFilter> appendFilter = vtkSmartPointer<vtkAppendFilter>::New();
			appendFilter->AddInputData(AppendedCuttedPolyData);
			appendFilter->AddInputData(itr_CuttedPolyData);
			appendFilter->Update();

			vtkSmartPointer<vtkGeometryFilter> geomFilter = vtkSmartPointer<vtkGeometryFilter>::New(); // For making appendfilter to polydata 
			geomFilter->SetInputData(appendFilter->GetOutput());
			geomFilter->Update();

			AppendedCuttedPolyData = geomFilter->GetOutput();
		}
		GeneratedPolyDataVector.push_back(AppendedCuttedPolyData);

		// Appending two parts
		vtkSmartPointer<vtkPolyData> AppendedPolyData = vtkSmartPointer<vtkPolyData>::New();
		AppendedPolyData = GeneratedPolyDataVector[0];
		for (auto itr_PolyData : GeneratedPolyDataVector)
		{
			vtkSmartPointer<vtkAppendFilter> appendFilter = vtkSmartPointer<vtkAppendFilter>::New();
			appendFilter->AddInputData(AppendedPolyData);
			appendFilter->AddInputData(itr_PolyData);
			appendFilter->Update();

			vtkSmartPointer<vtkGeometryFilter> geomFilter = vtkSmartPointer<vtkGeometryFilter>::New(); // For making appendfilter to polydata 
			geomFilter->SetInputData(appendFilter->GetOutput());
			geomFilter->Update();

			AppendedPolyData = geomFilter->GetOutput();
		}
		theApp.ObjectPanelPolydata_Collection[objectID] = AppendedPolyData;
	}
	// Visualize
	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(theApp.ObjectPanelPolydata_Collection[objectID]);
	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);
	theApp.ObjectPanelActors[objectID] = actor;

	theApp.ObjectPanelActors[objectID]->SetProperty(Util::GetProperty());
	theApp.ObjectPanelActors[objectID]->GetProperty()->SetColor(0.5, 0.5, 0.5);	
	if (isUpdate) theApp.ObjectPanelActors[objectID]->GetProperty()->SetOpacity(1 - (double(theApp.pRt->m_Object_Transparency[objectID]) / 100));
	else theApp.ObjectPanelActors[objectID]->GetProperty()->SetOpacity(0.5);
	theApp.ObjectPanelActors[objectID]->SetPosition(XCenter, YCenter, ZCenter);
	theApp.ObjectPanelActors[objectID]->SetOrientation(XRotate, YRotate, ZRotate);
	
	theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(theApp.ObjectPanelActors[objectID]);
	theApp.m_pVTKWidget->renderWindow()->Render();
}

void SphereObject::UpdateUI(int objectID) 
{
    if (theApp.ObjectPanelActors.find(objectID) == theApp.ObjectPanelActors.end() || 
        theApp.ObjectPanelActors[objectID] == nullptr) {
        return;
    }

    vtkActor* actor = theApp.ObjectPanelActors[objectID];

    double* pCenter = actor->GetPosition();
    double* pRotation = actor->GetOrientation();

    QString strPosX = theApp.getQStringNumberInSpecificDigit(pCenter[0], 6);
    QString strPosY = theApp.getQStringNumberInSpecificDigit(pCenter[1], 6);
    QString strPosZ = theApp.getQStringNumberInSpecificDigit(pCenter[2], 6);

    QString strRotX = theApp.getQStringNumberInSpecificDigit(pRotation[0], 6);
    QString strRotY = theApp.getQStringNumberInSpecificDigit(pRotation[1], 6);
    QString strRotZ = theApp.getQStringNumberInSpecificDigit(pRotation[2], 6);

    // Sphere 전용 UI 업데이트
    if (theApp.pRt) {
        theApp.pRt->ObjectSphere_CenterX_QLineEdit->setText(strPosX);
        theApp.pRt->ObjectSphere_CenterY_QLineEdit->setText(strPosY);
        theApp.pRt->ObjectSphere_CenterZ_QLineEdit->setText(strPosZ);

        theApp.pRt->ObjectSphere_RotateX_QLineEdit->setText(strRotX);
        theApp.pRt->ObjectSphere_RotateY_QLineEdit->setText(strRotY);
        theApp.pRt->ObjectSphere_RotateZ_QLineEdit->setText(strRotZ);
    }
}


// Cylinder 구현
void CylinderObject::Generate(int objectID) {
 	// 1) Update 모드 체크 및 기존 Actor/PolyData 제거
	bool isUpdate = false;
	if (theApp.ObjectPanelActors[objectID] != nullptr) {
		isUpdate = true;
		theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(theApp.ObjectPanelActors[objectID]);
		theApp.ObjectPanelActors[objectID] = nullptr;
		theApp.m_pVTKWidget->renderWindow()->Render();
		theApp.ObjectPanelPolydata_Collection[objectID] = nullptr;
	}

	// 2) 파라미터 로드
	auto& info = theApp.pRt->m_Object_MainInfo[objectID];
	const double XCenter = info[1];
	const double YCenter = info[2];
	const double ZCenter = info[3];
	const double XRotate = info[4];
	const double YRotate = info[5];
	const double ZRotate = info[6];
	const double InnerRadius = info[20];
	const double OuterRadius = info[21];
	const double HalfHeightZ = info[22];
	const double StartingPhi = info[23];
	const double DeltaPhi = info[24];

	// 3) PolyData 생성: 솔리드 vs 튜브/웨지 분기
	vtkSmartPointer<vtkPolyData> pd;
	if (InnerRadius == 0.0 && DeltaPhi == 360.0) {
		// ——— 완전한 솔리드 실린더 ———
		auto cyl = vtkSmartPointer<vtkCylinderSource>::New();
		cyl->SetRadius(OuterRadius);
		cyl->SetHeight(2.0 * HalfHeightZ);
		cyl->SetResolution(256);
		cyl->Update();

		// Geant4 변환
		auto tfG4 = vtkSmartPointer<vtkTransform>::New();
		tfG4->PostMultiply();
		tfG4->RotateZ(180);
		tfG4->RotateX(90);
		auto tflt = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
		tflt->SetInputConnection(cyl->GetOutputPort());
		tflt->SetTransform(tfG4);
		tflt->Update();

		pd = tflt->GetOutput();
	}
	else {
		// ——— 튜브(InnerRadius>0) 혹은 웨지(DeltaPhi<360) ———
		pd = CreateTubeMesh(
			InnerRadius,
			OuterRadius,
			HalfHeightZ,
			StartingPhi,
			DeltaPhi,
			/*resolution=*/128
		);
	}

	// 4) 저장
	theApp.ObjectPanelPolydata_Collection[objectID] = pd;

	// 5) Mapper/Actor 생성 및 속성 설정
	auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(pd);
	mapper->ScalarVisibilityOff();

	auto actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);
	actor->SetProperty(Util::GetProperty());
	actor->GetProperty()->SetColor(0.5, 0.5, 0.5);
	actor->GetProperty()->SetOpacity(
		isUpdate
		? 1.0 - (theApp.pRt->m_Object_Transparency[objectID] / 100.0)
		: 0.5
	);
	actor->SetPosition(XCenter, YCenter, ZCenter);
	actor->SetOrientation(XRotate, YRotate, ZRotate);

	theApp.ObjectPanelActors[objectID] = actor;
	theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(actor);
	theApp.m_pVTKWidget->renderWindow()->Render();
}

void CylinderObject::UpdateUI(int objectID) 
{
    if (theApp.ObjectPanelActors.find(objectID) == theApp.ObjectPanelActors.end() || 
        theApp.ObjectPanelActors[objectID] == nullptr) {
        return;
    }

    vtkActor* actor = theApp.ObjectPanelActors[objectID];

    double* pCenter = actor->GetPosition();
    double* pRotation = actor->GetOrientation();

    QString strPosX = theApp.getQStringNumberInSpecificDigit(pCenter[0], 6);
    QString strPosY = theApp.getQStringNumberInSpecificDigit(pCenter[1], 6);
    QString strPosZ = theApp.getQStringNumberInSpecificDigit(pCenter[2], 6);

    QString strRotX = theApp.getQStringNumberInSpecificDigit(pRotation[0], 6);
    QString strRotY = theApp.getQStringNumberInSpecificDigit(pRotation[1], 6);
    QString strRotZ = theApp.getQStringNumberInSpecificDigit(pRotation[2], 6);

    // Cylinder 전용 UI 업데이트
    if (theApp.pRt) {
        theApp.pRt->ObjectCylinder_CenterX_QLineEdit->setText(strPosX);
        theApp.pRt->ObjectCylinder_CenterY_QLineEdit->setText(strPosY);
        theApp.pRt->ObjectCylinder_CenterZ_QLineEdit->setText(strPosZ);

        theApp.pRt->ObjectCylinder_RotateX_QLineEdit->setText(strRotX);
        theApp.pRt->ObjectCylinder_RotateY_QLineEdit->setText(strRotY);
        theApp.pRt->ObjectCylinder_RotateZ_QLineEdit->setText(strRotZ);
    }
}
// Cylinder 도우미 함수 구현
vtkSmartPointer<vtkPolyData> CylinderObject::CreateTubeMesh(double R_in, double R_out, double halfH, double startDeg, double deltaDeg, int resolution){
	const double startRad = vtkMath::RadiansFromDegrees(startDeg);
	const double dPhi = vtkMath::RadiansFromDegrees(deltaDeg) / resolution;

	// 1) Points & Polygons 준비
	auto pts = vtkSmartPointer<vtkPoints>::New();
	auto polys = vtkSmartPointer<vtkCellArray>::New();

	// 2) 버텍스 삽입: outerTop, outerBottom, innerBottom, innerTop 순서로 (4 per slice)
	for (int i = 0; i <= resolution; ++i) {
		double phi = startRad + i * dPhi;
		double co = cos(phi), si = sin(phi);

		// Outer top/bottom
		pts->InsertNextPoint(R_out*co, R_out*si, +halfH);
		pts->InsertNextPoint(R_out*co, R_out*si, -halfH);
		// Inner bottom/top (역순으로 넣어야 법선 바깥)
		pts->InsertNextPoint(R_in *co, R_in *si, -halfH);
		pts->InsertNextPoint(R_in *co, R_in *si, +halfH);
	}

	// 3) Faces 생성

	// (i절점: base=i*4 + offset)
	for (int i = 0; i < resolution; ++i) {
		vtkIdType base = i * 4;
		vtkIdType next = (i + 1) * 4;

		// 3-1) Outer cylindrical wall
		{
			vtkIdType id[4] = {
				base + 0,    // outerTop i
				next + 0,    // outerTop i+1
				next + 1,    // outerBottom i+1
				base + 1     // outerBottom i
			};
			polys->InsertNextCell(4, id);
		}

		// 3-2) Inner cylindrical wall (if R_in>0)
		if (R_in > 0.0) {
			vtkIdType id[4] = {
				base + 2,    // innerBottom i
				next + 2,    // innerBottom i+1
				next + 3,    // innerTop i+1
				base + 3     // innerTop i
			};
			polys->InsertNextCell(4, id);
		}

		// 3-3) Top annulus
		{
			vtkIdType id[4] = {
				base + 0,    // outerTop i
				base + 3,    // innerTop i
				next + 3,    // innerTop i+1
				next + 0     // outerTop i+1
			};
			polys->InsertNextCell(4, id);
		}
		// 3-4) Bottom annulus
		{
			vtkIdType id[4] = {
				base + 1,    // outerBottom i
				next + 1,    // outerBottom i+1
				next + 2,    // innerBottom i+1
				base + 2     // innerBottom i
			};
			polys->InsertNextCell(4, id);
		}
	}

	// 4) Wedge(각도<360)라면 앞·뒤 면(plane)도 채워 줌
	if (deltaDeg < 360.0) {
		// 시작면
		{
			vtkIdType id[4] = {
				/*outerTop0*/    0,
				/*outerBot0*/    1,
				/*innerBot0*/    2,
				/*innerTop0*/    3
			};
			polys->InsertNextCell(4, id);
		}
		// 종료면
		{
			vtkIdType b = resolution * 4;
			vtkIdType id[4] = {
				/*innerTopEnd*/  b + 3,
				/*innerBotEnd*/  b + 2,
				/*outerBotEnd*/  b + 1,
				/*outerTopEnd*/  b + 0
			};
			polys->InsertNextCell(4, id);
		}
	}

	// 5) PolyData 완성
	auto poly = vtkSmartPointer<vtkPolyData>::New();
	poly->SetPoints(pts);
	poly->SetPolys(polys);

	// 6) Geant4 좌표계 변환
	auto transform = vtkSmartPointer<vtkTransform>::New();
	transform->PostMultiply();
	transform->RotateZ(180);
	transform->RotateX(90);
	auto tfilt = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	tfilt->SetInputData(poly);
	tfilt->SetTransform(transform);
	tfilt->Update();

	return tfilt->GetOutput();
}