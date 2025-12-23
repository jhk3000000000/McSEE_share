//SourceObjects.cpp
#include "pch.h"
#include "PhantomObjects.h"
#include "SourceObjects.h"       // theApp 변수 접근용
#include "FunctionPanelRight.h" // UI 접근용
#include "ETQVTKWidget.h"       // 3D 뷰어 접근용
#include "Util.h"               // Util 기능 사용
#include "ETInteractorStyleRubberBand.h"
#include "ETHuman3DApp.h"       // theApp 변수 접근용

// [C++ 표준 라이브러리] (ExtractPhantomOBJ 등 파싱 로직용)
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>

// [VTK 필수 헤더] (팬텀/의복 생성 및 조작용)
#include <vtkSmartPointer.h>
#include <vtkNew.h>

// 1. 파일 입출력
#include <vtkOBJReader.h>           // .obj 파일 읽기 (핵심)
#include <vtkSTLReader.h>           // 혹시 모를 stl 파일용
#include <vtkPLYWriter.h>           // 데이터 저장용

// 2. 데이터 가공 (위치 이동, 회전)
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>

// 3. 데이터 조립 (몸+옷 합치기)
#include <vtkAppendFilter.h>
#include <vtkCleanPolyData.h>       // 중복 점 제거
#include <vtkPolyDataNormals.h>     // 법선 벡터 계산 (매끈하게 보이게)

// 4. 가시화 (화면에 띄우기)
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>            // 투명도, 색상 설정
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>

// 5. 데이터 구조 (점, 셀 등)
#include <vtkPoints.h>
#include <vtkCellArray.h>
#include <vtkFloatArray.h>
#include <vtkDataArray.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>

// 6. 기타 (러버밴드, 선량계 등 특수 기능용)
#include <vtkExtractPolyDataGeometry.h> // 영역 추출
#include <vtkSphereSource.h>            // 선량계(구) 생성
#include <vtkDelaunay2D.h>              // (혹시 사용된다면)
#include <vtkUnstructuredGrid.h>
#include <vtkOBJWriter.h>
#include <vtkAreaPicker.h>  
#include <vtkPolyDataConnectivityFilter.h>
#include <vtkCubeSource.h>
#include <vtkWedge.h>
#include <vtkTriangle.h>
#include <vtkTriangleFilter.h>
#include <vtkDiskSource.h>

void SourceObjects::GenerateSourceActor_sourceBB() // Broad Beam 선원 actor 가시화 -> 전용코드로 따로 관리
{
	// Reset process
	for (auto itr : theApp.theApp.actors_cylinderB)
	{
		if (itr != nullptr)
		{
			theApp.theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(itr);
			itr = nullptr;
		}
	}
	theApp.actors_cylinderB.clear();
	for (auto itr : theApp.actors_coneB)
	{
		if (itr != nullptr)
		{
			theApp.theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(itr);
			itr = nullptr;
		}
	}
	theApp.actors_coneB.clear();
	for (auto itr : theApp.actors_arrowB)
	{
		if (itr != nullptr)
		{
			theApp.theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(itr);
			itr = nullptr;
		}
	}
	theApp.actors_arrowB.clear();

	theApp.theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(theApp.actor_circleB);
	theApp.actor_circleB = nullptr;
	theApp.theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(theApp.actor_sphereB);
	theApp.actor_sphereB = nullptr;
	theApp.theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(theApp.actor_single_cylinderB);
	theApp.actor_single_cylinderB = nullptr;

	// set varialbes
	double DistanceFromPhantomBox = 100; // 1 m

	double PhantomBoxXmin = DBL_MAX;
	double PhantomBoxXmax = -DBL_MAX;
	double PhantomBoxYmin = DBL_MAX;
	double PhantomBoxYmax = -DBL_MAX;
	double PhantomBoxZmin = DBL_MAX;
	double PhantomBoxZmax = -DBL_MAX;
	for (auto phantomID : theApp.theApp.pRt->m_Phantom_SequenceVector)
	{
		double* bounds = theApp.PhantomPanelActor[phantomID]->GetBounds();
		if (bounds[0] < PhantomBoxXmin) PhantomBoxXmin = bounds[0];
		if (bounds[1] > PhantomBoxXmax) PhantomBoxXmax = bounds[1];
		if (bounds[2] < PhantomBoxYmin) PhantomBoxYmin = bounds[2];
		if (bounds[3] > PhantomBoxYmax) PhantomBoxYmax = bounds[3];
		if (bounds[4] < PhantomBoxZmin) PhantomBoxZmin = bounds[4];
		if (bounds[5] > PhantomBoxZmax) PhantomBoxZmax = bounds[5];
	}

	double xLength = PhantomBoxXmax - PhantomBoxXmin;
	double yLength = PhantomBoxYmax - PhantomBoxYmin;
	double zLength = PhantomBoxZmax - PhantomBoxZmin;

	double centerX = (PhantomBoxXmax + PhantomBoxXmin) / 2;
	double centerY = (PhantomBoxYmax + PhantomBoxYmin) / 2;
	double centerZ = (PhantomBoxZmax + PhantomBoxZmin) / 2;
	
	// Actor property
	auto property = Util::GetProperty();
	double color[3] = { 1.0, 0., 0. };
	property->SetColor(color);
	property->SetInterpolationToPhong();
	//property->SetSpecular(1.0);
	//property->SetSpecularPower(0.01);

	// Actor parameter
	double CylinderHeight = 20;
	double CylinderRadius = 1;
	double ConeHeight = 10;
	double ConeRadius = 2;
	int iterationNo = 1;
	if (theApp.theApp.pRt->m_comboBoxBeamdirection->currentIndex() == 4) iterationNo = 8;
	if (theApp.theApp.pRt->m_comboBoxBeamdirection->currentIndex() == 5) iterationNo = 14;
	for (int actorNo = 0; actorNo < iterationNo; actorNo++)
	{
		// Create a cylinder (shaft of the arrow)
		vtkSmartPointer<vtkCylinderSource> cylinderSource = vtkSmartPointer<vtkCylinderSource>::New();
		cylinderSource->SetHeight(CylinderHeight);
		cylinderSource->SetRadius(CylinderRadius);
		cylinderSource->SetResolution(60);
		vtkSmartPointer<vtkPolyDataMapper> cylinderMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
		cylinderMapper->SetInputConnection(cylinderSource->GetOutputPort());
		theApp.actors_cylinderB.push_back(vtkSmartPointer<vtkActor>::New());
		theApp.actors_cylinderB.back()->SetMapper(cylinderMapper);
		theApp.actors_cylinderB.back()->SetProperty(property);
		theApp.theApp.theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(theApp.actors_cylinderB.back());
		// Create a cone (arrowhead)
		vtkSmartPointer<vtkConeSource> coneSource = vtkSmartPointer<vtkConeSource>::New();
		coneSource->SetHeight(ConeHeight);
		coneSource->SetRadius(ConeRadius);
		coneSource->SetResolution(60);
		vtkSmartPointer<vtkPolyDataMapper> coneMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
		coneMapper->SetInputConnection(coneSource->GetOutputPort());
		theApp.actors_coneB.push_back(vtkSmartPointer<vtkActor>::New());
		theApp.actors_coneB.back()->SetMapper(coneMapper);
		theApp.actors_coneB.back()->SetProperty(property);
		theApp.theApp.theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(theApp.actors_coneB.back());
	}
	if (theApp.theApp.pRt->m_comboBoxBeamdirection->currentIndex() == 0) // AP
	{
		// Create a circle
		double radius_AP;
		if (xLength >= zLength) radius_AP = xLength / 2 * 1.05;
		if (zLength >= xLength) radius_AP = zLength / 2 * 1.05;
		vtkSmartPointer<vtkDiskSource> diskSource = vtkSmartPointer<vtkDiskSource>::New();
		diskSource->SetInnerRadius(0); // Set the inner radius
		diskSource->SetOuterRadius(radius_AP); // Set the outer radius 
		diskSource->SetRadialResolution(1); // Minimal radial divisions, since it's a circle
		diskSource->SetCircumferentialResolution(100); // Adjust the resolution as needed
		vtkSmartPointer<vtkPolyDataMapper> mapper_disk = vtkSmartPointer<vtkPolyDataMapper>::New();
		mapper_disk->SetInputConnection(diskSource->GetOutputPort());
		theApp.actor_circleB = vtkSmartPointer<vtkActor>::New();
		theApp.actor_circleB->SetMapper(mapper_disk);
		theApp.actor_circleB->SetProperty(property);
		theApp.actor_circleB->PickableOff();
		theApp.theApp.theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(theApp.actor_circleB);
		// Set position	
		theApp.actor_circleB->SetPosition(centerX, centerY - DistanceFromPhantomBox, centerZ);
		theApp.actor_circleB->SetOrientation(90, 0, 0);
		theApp.actors_cylinderB[0]->SetPosition(centerX, centerY - DistanceFromPhantomBox + CylinderHeight / 2, centerZ);
		theApp.actors_cylinderB[0]->SetOrientation(0, 0, 0);
		theApp.actors_coneB[0]->SetOrientation(0, 0, 90);
		theApp.actors_coneB[0]->SetPosition(centerX, centerY - DistanceFromPhantomBox + ConeHeight / 2 + CylinderHeight, centerZ);
	}
	if (theApp.theApp.pRt->m_comboBoxBeamdirection->currentIndex() == 1) //PA
	{
		double radius_PA;
		if (xLength >= zLength) radius_PA = xLength / 2 * 1.05;
		if (zLength >= xLength) radius_PA = zLength / 2 * 1.05;
		vtkSmartPointer<vtkDiskSource> diskSource = vtkSmartPointer<vtkDiskSource>::New();
		diskSource->SetInnerRadius(0); // Set the inner radius
		diskSource->SetOuterRadius(radius_PA); // Set the outer radius 
		diskSource->SetRadialResolution(1); // Minimal radial divisions, since it's a circle
		diskSource->SetCircumferentialResolution(100); // Adjust the resolution as needed
		vtkSmartPointer<vtkPolyDataMapper> mapper_disk = vtkSmartPointer<vtkPolyDataMapper>::New();
		mapper_disk->SetInputConnection(diskSource->GetOutputPort());
		theApp.actor_circleB = vtkSmartPointer<vtkActor>::New();
		theApp.actor_circleB->SetMapper(mapper_disk);
		theApp.actor_circleB->SetProperty(property);
		theApp.actor_circleB->PickableOff();
		theApp.theApp.theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(theApp.actor_circleB);
		// Set position	
		theApp.actor_circleB->SetPosition(centerX, centerY + DistanceFromPhantomBox, centerZ);
		theApp.actor_circleB->SetOrientation(90, 0, 0);
		theApp.actors_cylinderB[0]->SetPosition(centerX, centerY + DistanceFromPhantomBox - CylinderHeight / 2, centerZ);
		theApp.actors_cylinderB[0]->SetOrientation(0, 0, 0);
		theApp.actors_coneB[0]->SetOrientation(0, 0, 270);
		theApp.actors_coneB[0]->SetPosition(centerX, centerY + DistanceFromPhantomBox - ConeHeight / 2 - CylinderHeight, centerZ); 
	}
	if (theApp.theApp.pRt->m_comboBoxBeamdirection->currentIndex() == 2) // LLAT
	{
		double radius_LLAT;
		if (xLength >= zLength) radius_LLAT = xLength / 2 * 1.05;
		if (zLength >= xLength) radius_LLAT = zLength / 2 * 1.05;
		vtkSmartPointer<vtkDiskSource> diskSource = vtkSmartPointer<vtkDiskSource>::New();
		diskSource->SetInnerRadius(0); // Set the inner radius
		diskSource->SetOuterRadius(radius_LLAT); // Set the outer radius 
		diskSource->SetRadialResolution(1); // Minimal radial divisions, since it's a circle
		diskSource->SetCircumferentialResolution(100); // Adjust the resolution as needed
		vtkSmartPointer<vtkPolyDataMapper> mapper_disk = vtkSmartPointer<vtkPolyDataMapper>::New();
		mapper_disk->SetInputConnection(diskSource->GetOutputPort());
		theApp.actor_circleB = vtkSmartPointer<vtkActor>::New();
		theApp.actor_circleB->SetMapper(mapper_disk);
		theApp.actor_circleB->SetProperty(property);
		theApp.actor_circleB->PickableOff();
		theApp.theApp.theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(theApp.actor_circleB);
		// Set position	
		theApp.actor_circleB->SetPosition(centerX - DistanceFromPhantomBox, centerY, centerZ);
		theApp.actor_circleB->SetOrientation(90, 0, 90);
		theApp.actors_cylinderB[0]->SetPosition(centerX - DistanceFromPhantomBox + CylinderHeight / 2, centerY, centerZ);
		theApp.actors_cylinderB[0]->SetOrientation(0, 0, 90);
		theApp.actors_coneB[0]->SetOrientation(0, 0, 0);
		theApp.actors_coneB[0]->SetPosition(centerX - DistanceFromPhantomBox + ConeHeight / 2 + CylinderHeight, centerY, centerZ);
	}
	if (theApp.theApp.pRt->m_comboBoxBeamdirection->currentIndex() == 3) // RLAT
	{
		double radius_AP;
		if (xLength >= zLength) radius_AP = xLength / 2 * 1.05;
		if (zLength >= xLength) radius_AP = zLength / 2 * 1.05;
		vtkSmartPointer<vtkDiskSource> diskSource = vtkSmartPointer<vtkDiskSource>::New();
		diskSource->SetInnerRadius(0); // Set the inner radius
		diskSource->SetOuterRadius(radius_AP); // Set the outer radius 
		diskSource->SetRadialResolution(1); // Minimal radial divisions, since it's a circle
		diskSource->SetCircumferentialResolution(100); // Adjust the resolution as needed
		vtkSmartPointer<vtkPolyDataMapper> mapper_disk = vtkSmartPointer<vtkPolyDataMapper>::New();
		mapper_disk->SetInputConnection(diskSource->GetOutputPort());
		theApp.actor_circleB = vtkSmartPointer<vtkActor>::New();
		theApp.actor_circleB->SetMapper(mapper_disk);
		theApp.actor_circleB->SetProperty(property);
		theApp.actor_circleB->PickableOff();
		theApp.theApp.theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(theApp.actor_circleB);
		// Set position	
		theApp.actor_circleB->SetPosition(centerX + DistanceFromPhantomBox, centerY, centerZ);
		theApp.actor_circleB->SetOrientation(90, 0, 90);
		theApp.actors_cylinderB[0]->SetPosition(centerX + DistanceFromPhantomBox - CylinderHeight / 2, centerY, centerZ);
		theApp.actors_cylinderB[0]->SetOrientation(0, 0, 90);
		theApp.actors_coneB[0]->SetOrientation(0, 0, 180);
		theApp.actors_coneB[0]->SetPosition(centerX + DistanceFromPhantomBox - ConeHeight / 2 - CylinderHeight, centerY, centerZ);
	}
	if (theApp.theApp.pRt->m_comboBoxBeamdirection->currentIndex() == 4) // ROT
	{
		// Create a circle
		double radius_ROT = sqrt((yLength / 2 * yLength / 2) + (zLength / 2 * zLength / 2)) * 1.05; // 5% margin
		vtkSmartPointer<vtkDiskSource> diskSource = vtkSmartPointer<vtkDiskSource>::New();
		diskSource->SetInnerRadius(0); // Set the inner radius
		diskSource->SetOuterRadius(radius_ROT); // Set the outer radius 
		diskSource->SetRadialResolution(1); // Minimal radial divisions, since it's a circle
		diskSource->SetCircumferentialResolution(100); // Adjust the resolution as needed
		vtkSmartPointer<vtkPolyDataMapper> mapper_disk = vtkSmartPointer<vtkPolyDataMapper>::New();
		mapper_disk->SetInputConnection(diskSource->GetOutputPort());
		theApp.actor_circleB = vtkSmartPointer<vtkActor>::New();
		theApp.actor_circleB->SetMapper(mapper_disk);
		theApp.actor_circleB->SetProperty(property);
		theApp.actor_circleB->PickableOff();
		theApp.theApp.theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(theApp.actor_circleB);

		// Create a cylinder
		double distance_ROT = xLength / 2 + 50; // 원점(팬텀박스중심)으로부터 원판이 떨어진 거리 + 50 cm (도합 100 cm)
		vtkSmartPointer<vtkCylinderSource> cylinderSource = vtkSmartPointer<vtkCylinderSource>::New();
		cylinderSource->SetRadius(distance_ROT); // Set the radius
		cylinderSource->SetHeight(radius_ROT * 2);
		cylinderSource->SetResolution(30); // Set the resolution 		
		vtkSmartPointer<vtkPolyDataMapper> mapper_cylinder = vtkSmartPointer<vtkPolyDataMapper>::New();
		mapper_cylinder->SetInputConnection(cylinderSource->GetOutputPort());
		theApp.actor_single_cylinderB = vtkSmartPointer<vtkActor>::New();
		theApp.actor_single_cylinderB->SetMapper(mapper_cylinder);
		auto property_cylinder = Util::GetProperty();
		double color[3] = { 1.0, 0., 0. };
		property_cylinder->SetColor(color);
		property_cylinder->SetInterpolationToPhong();
		property_cylinder->SetOpacity(0.05);
		theApp.actor_single_cylinderB->SetProperty(property_cylinder);
		theApp.actor_single_cylinderB->PickableOff();
		theApp.theApp.theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(theApp.actor_single_cylinderB);
		 
		// Create curved arrow
		double radiusX_ROT = distance_ROT / 3; // Larger radius for x-axis
		double radiusY_ROT = distance_ROT / 2; // Smaller radius for y-axis
		int numSegments = 2000;
		double tubeRadius = distance_ROT / 50;
		double coneHeight_ROT = distance_ROT / 12;
		double coneRaidus_ROT = distance_ROT / 25;
		for (int i = 0; i < 1; i++)
		{
			vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
			vtkSmartPointer<vtkCellArray> lines = vtkSmartPointer<vtkCellArray>::New();
			double angleStep = (60.0 / numSegments) * (M_PI / 180.0); // angle을 조정하여 어디까지 타원의 어디까지 생성할 건지 조정 가능
			for (int i = 0; i <= numSegments; ++i) {
				double angle = angleStep * i;
				double x = radiusX_ROT * cos(angle);
				double y = radiusY_ROT * sin(angle);
				points->InsertNextPoint(x, y, 0.0);

				if (i > 0) {
					vtkSmartPointer<vtkLine> line = vtkSmartPointer<vtkLine>::New();
					line->GetPointIds()->SetId(0, i - 1);
					line->GetPointIds()->SetId(1, i);
					lines->InsertNextCell(line);
				}
			}
			vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
			polyData->SetPoints(points);
			polyData->SetLines(lines);
			vtkSmartPointer<vtkTubeFilter> tubeFilter = vtkSmartPointer<vtkTubeFilter>::New();
			tubeFilter->SetInputData(polyData);
			tubeFilter->SetRadius(tubeRadius); // Set the radius of the tube
			tubeFilter->SetNumberOfSides(50); // Set the number of sides for the tube's cross-section
			tubeFilter->CappingOn(); // Cap the ends of the tube
			vtkSmartPointer<vtkPolyDataMapper> mapper_tube = vtkSmartPointer<vtkPolyDataMapper>::New();
			mapper_tube->SetInputConnection(tubeFilter->GetOutputPort());
			vtkSmartPointer<vtkActor> tubeActor = vtkSmartPointer<vtkActor>::New();
			tubeActor->SetMapper(mapper_tube);
			tubeActor->SetProperty(property);

			vtkSmartPointer<vtkConeSource> coneSource = vtkSmartPointer<vtkConeSource>::New();
			coneSource->SetHeight(coneHeight_ROT); // Set the height of the cone (adjust as needed)
			coneSource->SetRadius(coneRaidus_ROT); // Set the radius of the cone (adjust as needed)
			coneSource->SetResolution(20); // Set the resolution of the cone
			coneSource->SetCenter(radiusX_ROT, -coneHeight_ROT / 2, 0); // Position at the end of the curve (+X)
			coneSource->SetDirection(0, -1, 0);
			vtkSmartPointer<vtkPolyDataMapper> coneMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
			coneMapper->SetInputConnection(coneSource->GetOutputPort());
			vtkSmartPointer<vtkActor> coneActor = vtkSmartPointer<vtkActor>::New();
			coneActor->SetMapper(coneMapper);
			coneActor->SetProperty(property);
			theApp.actors_arrowB.push_back(vtkSmartPointer<vtkAssembly>::New());
			theApp.actors_arrowB[i]->AddPart(tubeActor);
			theApp.actors_arrowB[i]->AddPart(coneActor);
			theApp.actors_arrowB[i]->PickableOff();
			theApp.theApp.theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(theApp.actors_arrowB[i]);
		}

		// Set position
		theApp.actors_arrowB[0]->SetPosition(centerX + distance_ROT / 3, centerY - distance_ROT + radiusY_ROT / 2, centerZ);
		theApp.actors_arrowB[0]->SetOrientation(0, 0, -85);
		//theApp.actors_arrowB[1]->SetPosition(centerX, centerY, centerZ);
		//theApp.actors_arrowB[1]->SetOrientation(0, 0, 0);
		theApp.actor_circleB->SetPosition(centerX + distance_ROT, centerY, centerZ);
		theApp.actor_circleB->SetOrientation(90, 90, 0);
		theApp.actor_single_cylinderB->SetPosition(centerX, centerY, centerZ);
		theApp.actor_single_cylinderB->SetOrientation(90, 0, 0);
	}
	if (theApp.theApp.pRt->m_comboBoxBeamdirection->currentIndex() == 5) // ISO
	{
		// Create a circle
		double XYarea = xLength * yLength;
		double XZarea = xLength * zLength;
		double YZarea = yLength * zLength;
		double radius_ISO;				
		if (XYarea >= YZarea && XYarea >= YZarea) {
			radius_ISO = sqrt((xLength / 2 * xLength / 2) + (yLength / 2 * yLength / 2)) * 1.05;
		}
		else if (YZarea >= XYarea && YZarea >= XZarea) {
			radius_ISO = sqrt((yLength / 2 * yLength / 2) + (zLength / 2 * zLength / 2)) * 1.05;
		}
		else {
			radius_ISO = sqrt((xLength / 2 * xLength / 2) + (zLength / 2 * zLength / 2)) * 1.05;
		}
		vtkSmartPointer<vtkDiskSource> diskSource = vtkSmartPointer<vtkDiskSource>::New();
		diskSource->SetInnerRadius(0); // Set the inner radius
		diskSource->SetOuterRadius(radius_ISO); // Set the outer radius 
		diskSource->SetRadialResolution(1); // Minimal radial divisions, since it's a circle
		diskSource->SetCircumferentialResolution(100); // Adjust the resolution as needed
		vtkSmartPointer<vtkPolyDataMapper> mapper_disk = vtkSmartPointer<vtkPolyDataMapper>::New();
		mapper_disk->SetInputConnection(diskSource->GetOutputPort());
		theApp.actor_circleB = vtkSmartPointer<vtkActor>::New();
		theApp.actor_circleB->SetMapper(mapper_disk);
		theApp.actor_circleB->SetProperty(property);
		theApp.actor_circleB->PickableOff();
		theApp.theApp.theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(theApp.actor_circleB);

		// Create a sphere
		double distance_ISO = sqrt((xLength / 2 * xLength / 2) + (yLength / 2 * yLength / 2) + (zLength / 2 * zLength / 2)) * 1.05 + 50; // 원점(팬텀박스중심)으로부터 원판이 떨어진 거리 + 50 cm (도합 100 cm)
		vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
		sphereSource->SetRadius(distance_ISO); // Set the radius
		sphereSource->SetThetaResolution(30); // Set the resolution (number of subdivisions) in theta
		sphereSource->SetPhiResolution(30); // Set the resolution in phi
		vtkSmartPointer<vtkPolyDataMapper> mapper_sphere = vtkSmartPointer<vtkPolyDataMapper>::New();
		mapper_sphere->SetInputConnection(sphereSource->GetOutputPort());
		theApp.actor_sphereB = vtkSmartPointer<vtkActor>::New();
		theApp.actor_sphereB->SetMapper(mapper_sphere);
		auto property_sphere = Util::GetProperty();
		double color[3] = { 1.0, 0., 0. };
		property_sphere->SetColor(color);
		property_sphere->SetInterpolationToPhong();
		property_sphere->SetOpacity(0.05);
		theApp.actor_sphereB->SetProperty(property_sphere);
		theApp.actor_sphereB->PickableOff();
		theApp.theApp.theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(theApp.actor_sphereB);

		// Create curved arrow
		double radiusX_ISO = distance_ISO / 1.5; // Larger radius for x-axis
		double radiusY_ISO = distance_ISO / 5; // Smaller radius for y-axis
		int numSegments = 2000;
		double tubeRadius = distance_ISO / 50;
		double coneHeight_ISO = distance_ISO / 12;
		double coneRaidus_ISO = distance_ISO / 25;
		for (int i = 0; i < 2; i++)
		{
			vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
			vtkSmartPointer<vtkCellArray> lines = vtkSmartPointer<vtkCellArray>::New();
			double angleStep = (45.0 / numSegments) * (M_PI / 180.0); // angle을 조정하여 어디까지 타원의 어디까지 생성할 건지 조정 가능
			for (int i = 0; i <= numSegments; ++i) {
				double angle = angleStep * i;
				double x = radiusX_ISO * cos(angle);
				double y = radiusY_ISO * sin(angle);
				points->InsertNextPoint(x, y, 0.0);

				if (i > 0) {
					vtkSmartPointer<vtkLine> line = vtkSmartPointer<vtkLine>::New();
					line->GetPointIds()->SetId(0, i - 1);
					line->GetPointIds()->SetId(1, i);
					lines->InsertNextCell(line);
				}
			}
			vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
			polyData->SetPoints(points);
			polyData->SetLines(lines);
			vtkSmartPointer<vtkTubeFilter> tubeFilter = vtkSmartPointer<vtkTubeFilter>::New();
			tubeFilter->SetInputData(polyData);
			tubeFilter->SetRadius(tubeRadius); // Set the radius of the tube
			tubeFilter->SetNumberOfSides(50); // Set the number of sides for the tube's cross-section
			tubeFilter->CappingOn(); // Cap the ends of the tube
			vtkSmartPointer<vtkPolyDataMapper> mapper_tube = vtkSmartPointer<vtkPolyDataMapper>::New();
			mapper_tube->SetInputConnection(tubeFilter->GetOutputPort());
			vtkSmartPointer<vtkActor> tubeActor = vtkSmartPointer<vtkActor>::New();
			tubeActor->SetMapper(mapper_tube);
			tubeActor->SetProperty(property);

			vtkSmartPointer<vtkConeSource> coneSource = vtkSmartPointer<vtkConeSource>::New();
			coneSource->SetHeight(coneHeight_ISO); // Set the height of the cone (adjust as needed)
			coneSource->SetRadius(coneRaidus_ISO); // Set the radius of the cone (adjust as needed)
			coneSource->SetResolution(20); // Set the resolution of the cone
			coneSource->SetCenter(radiusX_ISO, -coneHeight_ISO / 2, 0); // Position at the end of the curve (+X)
			coneSource->SetDirection(0, -1, 0);
			vtkSmartPointer<vtkPolyDataMapper> coneMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
			coneMapper->SetInputConnection(coneSource->GetOutputPort());
			vtkSmartPointer<vtkActor> coneActor = vtkSmartPointer<vtkActor>::New();
			coneActor->SetMapper(coneMapper);
			coneActor->SetProperty(property);
			theApp.actors_arrowB.push_back(vtkSmartPointer<vtkAssembly>::New());
			theApp.actors_arrowB[i]->AddPart(tubeActor);
			theApp.actors_arrowB[i]->AddPart(coneActor);
			theApp.actors_arrowB[i]->PickableOff();
			theApp.theApp.theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(theApp.actors_arrowB[i]);
		}		

		// Set position
		theApp.actors_arrowB[0]->SetPosition(centerX + radius_ISO - radiusX_ISO / 2, centerY, centerZ + distance_ISO - radiusY_ISO);
		theApp.actors_arrowB[0]->SetOrientation(85, 90, -90);
		theApp.actors_arrowB[1]->SetPosition(centerX, centerY - radius_ISO + radiusX_ISO / 2, centerZ + distance_ISO - radiusY_ISO);
		theApp.actors_arrowB[1]->SetOrientation(90, 85,-180);
		theApp.actor_circleB->SetPosition(centerX, centerY, centerZ + distance_ISO);
		theApp.actor_sphereB->SetPosition(centerX, centerY, centerZ);
	}
	if (theApp.theApp.pRt->m_comboBoxBeamdirection->currentIndex() == 6) //User-defined
	{
		double radius_USER;
		double XYarea = xLength * yLength;
		double XZarea = xLength * zLength;
		double YZarea = yLength * zLength;
		if (XYarea >= YZarea && XYarea >= YZarea) {
			radius_USER = sqrt((xLength / 2 * xLength / 2) + (yLength / 2 * yLength / 2)) * 1.05;
		}
		else if (YZarea >= XYarea && YZarea >= XZarea) {
			radius_USER = sqrt((yLength / 2 * yLength / 2) + (zLength / 2 * zLength / 2)) * 1.05;
		}
		else {
			radius_USER = sqrt((xLength / 2 * xLength / 2) + (zLength / 2 * zLength / 2)) * 1.05;
		}
		DistanceFromPhantomBox = sqrt((xLength / 2 * xLength / 2) + (yLength / 2 * yLength / 2) + (zLength / 2 * zLength / 2)) * 1.05; // 
		vtkSmartPointer<vtkDiskSource> diskSource = vtkSmartPointer<vtkDiskSource>::New();
		diskSource->SetInnerRadius(0); // Set the inner radius
		diskSource->SetOuterRadius(radius_USER); // Set the outer radius 
		diskSource->SetRadialResolution(1); // Minimal radial divisions, since it's a circle
		diskSource->SetCircumferentialResolution(100); // Adjust the resolution as needed
		vtkSmartPointer<vtkPolyDataMapper> mapper_disk = vtkSmartPointer<vtkPolyDataMapper>::New();
		mapper_disk->SetInputConnection(diskSource->GetOutputPort());
		vtkSmartPointer<vtkActor> actor_circle = vtkSmartPointer<vtkActor>::New();
		actor_circle->SetMapper(mapper_disk);
		actor_circle->SetProperty(property);
		actor_circle->PickableOff();
		theApp.theApp.theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(actor_circle);
		// Set position	
		actor_circle->SetPosition(centerX, centerY, centerZ - DistanceFromPhantomBox);
		actor_circle->SetOrientation(0, 0, 0);
		theApp.actors_cylinderB[0]->SetPosition(centerX, centerY, centerZ - DistanceFromPhantomBox + CylinderHeight / 2);
		theApp.actors_cylinderB[0]->SetOrientation(90, 0, 0);		
		theApp.actors_coneB[0]->SetPosition(centerX, centerY, centerZ - DistanceFromPhantomBox + ConeHeight / 2 + CylinderHeight);
		theApp.actors_coneB[0]->SetOrientation(0, -90, 180);
		theApp.actors_arrowB.push_back(vtkSmartPointer<vtkAssembly>::New());
		theApp.actors_arrowB[0]->AddPart(actor_circle);
		theApp.theApp.theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(actor_circle);
		theApp.actors_arrowB[0]->AddPart(theApp.actors_cylinderB[0]);
		theApp.theApp.theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(theApp.actors_cylinderB[0]);
		theApp.actors_arrowB[0]->AddPart(theApp.actors_coneB[0]);
		theApp.theApp.theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(theApp.actors_coneB[0]);
		theApp.actors_arrowB[0]->PickableOff();

		theApp.theApp.theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(theApp.actors_arrowB[0]);
		theApp.actors_arrowB[0]->SetOrientation(0, theApp.theApp.pRt->m_lineEditPolarAngle->text().toDouble(), theApp.theApp.pRt->m_lineEditAzimuthalAngle->text().toDouble());
	}

	theApp.theApp.theApp.m_pVTKWidget->renderWindow()->Render();
}
void SourceObjects::GenerateSourceActor_sourcePS(double* ptCenter) // Point Source, originally EP
{
	// Create a sphere
	vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
	sphereSource->SetCenter(0.0, 0.0, 0.0);
	sphereSource->SetRadius(0.5);

	// Make the surface smooth.
	sphereSource->SetPhiResolution(60);
	sphereSource->SetThetaResolution(30);

	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputConnection(sphereSource->GetOutputPort());

	vtkSmartPointer<vtkNamedColors> colors = vtkSmartPointer<vtkNamedColors>::New();

	// actor 생성	
	theApp.theApp.SourcePanelActor_Position.push_back(vtkSmartPointer<vtkActor>::New());
	theApp.theApp.SourcePanelActor_Position.back()->SetMapper(mapper);
	theApp.theApp.SourcePanelActor_Position.back()->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());
	theApp.theApp.SourcePanelActor_Position.back()->SetPosition(ptCenter);
	theApp.theApp.SourcePanelActor_Position.back()->SetPickable(false);
	theApp.theApp.SourcePanelActor_Position.back()->SetScale(pow(theApp.const_PointSource_ScaleFactorPerClick, theApp.m_Point_Source_Scale));

	theApp.theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(theApp.theApp.SourcePanelActor_Position.back());

	// 임시 actor 삭제
	Selecting3DShpere_Delete();

	theApp.theApp.m_pVTKWidget->renderWindow()->Render();
}
void SourceObjects::GenerateSourceActor_sourceFD(double* center_radius)// Floor Disk
{	
	double center_x = center_radius[0];
	double center_y = center_radius[1];
	double center_z = center_radius[2];
	double center[3] = { center_x , center_y , center_z };
	double radius = center_radius[3];

	// Create a filled circle (disk with inner radius 0)
	vtkSmartPointer<vtkDiskSource> diskSource = vtkSmartPointer<vtkDiskSource>::New();
	diskSource->SetInnerRadius(0.0); // 내부 반지름을 0으로 설정하여 채워진 원(circle) 생성
	diskSource->SetOuterRadius(radius); // 외부 반지름 설정 (circle의 반지름)

	// 원의 세그먼트 수 설정 (선택 사항)
	diskSource->SetRadialResolution(30);
	diskSource->SetCircumferentialResolution(60);

	// Apply a transform to orient the disk perpendicular to the Z-axis
	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
	transform->RotateX(90); // X축을 기준으로 90도 회전하여 Z축에 수직 배치

	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputConnection(diskSource->GetOutputPort());

	vtkSmartPointer<vtkNamedColors> colors = vtkSmartPointer<vtkNamedColors>::New();

	// actor 생성
	theApp.SourcePanelActor_Position.push_back(vtkSmartPointer<vtkActor>::New());
	if (theApp.theApp.SourcePanelActor_Position.size() != 1)// 벡터 크기가 1이 아니어서 기존에 있었을때
	{
		theApp.theApp.theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(theApp.theApp.SourcePanelActor_Position[0]);
		theApp.theApp.SourcePanelActor_Position = { theApp.theApp.SourcePanelActor_Position.back() }; // 마지막 원소 하나로만 있게 변경
	}
	theApp.SourcePanelActor_Position.back()->SetMapper(mapper);
	double color[3] = { 0.66, 0.33, 0.0 };
	theApp.SourcePanelActor_Position.back()->GetProperty()->SetColor(color); // 원의 색상 설정
	theApp.SourcePanelActor_Position.back()->SetPosition(center); // 원 위치 설정
	theApp.SourcePanelActor_Position.back()->SetPickable(false);

	theApp.theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(theApp.SourcePanelActor_Position.back());

	theApp.theApp.m_pVTKWidget->renderWindow()->Render();
}
void SourceObjects::GenerateSourceActor_sourceVS()
{
    // 1. 빨간색으로 변경 (시각적 표시)
    // "이 도형은 이제 선원입니다"라고 표시하는 것
    theApp.ObjectPanelActors[theApp.pRt->Object_sourceOV_SelectedIndex]->GetProperty()->SetColor(1.0, 0., 0.);

    // 2. 경계 박스(Bounds) 계산 및 저장
    // 시뮬레이션을 위해 이 도형이 차지하는 공간(x,y,z 범위)을 잽니다.
    double* bounds = theApp.ObjectPanelActors[theApp.pRt->Object_sourceOV_SelectedIndex]->GetBounds();
    
    // theApp에 있는 데이터 저장소에 기록 (이건 나중에 Manager로 옮길 예정)
    theApp.pRt->sourceOV_objectBound[theApp.pRt->Object_sourceOV_SelectedIndex][0] = bounds[0];
    theApp.pRt->sourceOV_objectBound[theApp.pRt->Object_sourceOV_SelectedIndex][1] = bounds[1];
    theApp.pRt->sourceOV_objectBound[theApp.pRt->Object_sourceOV_SelectedIndex][2] = bounds[2];
    theApp.pRt->sourceOV_objectBound[theApp.pRt->Object_sourceOV_SelectedIndex][3] = bounds[3];
    theApp.pRt->sourceOV_objectBound[theApp.pRt->Object_sourceOV_SelectedIndex][4] = bounds[4];
    theApp.pRt->sourceOV_objectBound[theApp.pRt->Object_sourceOV_SelectedIndex][5] = bounds[5];

    // 3. 화면 갱신
    theApp.theApp.theApp.m_pVTKWidget->renderWindow()->Render();
}
void SourceObjects::GenerateSourceActor_sourcePSF(std::vector<std::tuple<double, double, double>> ptMultiple, std::vector<std::tuple<double, double, double>> dirMultiple) // Phase-space File, originally PS
{
	int actor_count = 500;
	int count;
	int DividingFactor;

	if (ptMultiple.size() <= actor_count)
	{
		count = ptMultiple.size();
		DividingFactor = 1;
	}
	else
	{
		count = actor_count;
		int DividingFactor = ptMultiple.size() / actor_count;
	}

	for (int i = 0; i < count; i++)
	{
		// Create a sphere
		vtkSmartPointer<vtkSphereSource> sphereSource =
			vtkSmartPointer<vtkSphereSource>::New();
		sphereSource->SetCenter(0.0, 0.0, 0.0);
		sphereSource->SetRadius(0.8);

		// Make the surface smooth.
		sphereSource->SetPhiResolution(60);
		sphereSource->SetThetaResolution(30);

		vtkSmartPointer<vtkPolyDataMapper> mapper =
			vtkSmartPointer<vtkPolyDataMapper>::New();
		mapper->SetInputConnection(sphereSource->GetOutputPort());

		vtkSmartPointer<vtkNamedColors> colors =
			vtkSmartPointer<vtkNamedColors>::New();

		double ptCenter[3];
		ptCenter[0] = std::get<0>(ptMultiple[i*DividingFactor]);
		ptCenter[1] = std::get<1>(ptMultiple[i*DividingFactor]);
		ptCenter[2] = std::get<2>(ptMultiple[i*DividingFactor]);

		// actor 생성
		theApp.SourcePanelActor_Position.push_back(vtkSmartPointer<vtkActor>::New());
		theApp.SourcePanelActor_Position[i]->SetMapper(mapper);
		theApp.SourcePanelActor_Position[i]->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());
		theApp.SourcePanelActor_Position[i]->SetPosition(ptCenter);
		theApp.SourcePanelActor_Position[i]->SetPickable(false);

		theApp.theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(theApp.SourcePanelActor_Position[i]);

		//dir actor
		vtkSmartPointer<vtkLineSource> lineSource =
			vtkSmartPointer<vtkLineSource>::New();

		lineSource->SetPoint1(0, 0, 0);  // origin
		lineSource->SetPoint2(std::get<0>(dirMultiple[i*DividingFactor]), std::get<1>(dirMultiple[i*DividingFactor]), std::get<2>(dirMultiple[i*DividingFactor]));
		vtkSmartPointer<vtkPolyDataMapper> mapper2 =
			vtkSmartPointer<vtkPolyDataMapper>::New();
		mapper2->SetInputConnection(lineSource->GetOutputPort());

		// actor 생성
		theApp.SourcePanelActor_Direction.push_back(vtkSmartPointer<vtkActor>::New());
		theApp.SourcePanelActor_Direction.back()->SetMapper(mapper2);
		theApp.SourcePanelActor_Direction.back()->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());
		theApp.SourcePanelActor_Direction.back()->SetScale(100); // 100 cm length
		theApp.SourcePanelActor_Direction.back()->SetPosition(ptCenter);
		theApp.SourcePanelActor_Direction.back()->SetPickable(false);

		theApp.theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(theApp.SourcePanelActor_Direction.back());
		theApp.theApp.m_pVTKWidget->renderWindow()->Render();
	}
}
void SourceObjects::GenerateSourceActor_sourceHP(double* ptCenter) // Hot Particle
{
	// Create a sphere
	vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
	sphereSource->SetCenter(0.0, 0.0, 0.0);
	sphereSource->SetRadius(0.5);

	// Make the surface smooth.
	sphereSource->SetPhiResolution(60);
	sphereSource->SetThetaResolution(30);

	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputConnection(sphereSource->GetOutputPort());

	vtkSmartPointer<vtkNamedColors> colors = vtkSmartPointer<vtkNamedColors>::New();

	// actor 생성	
	theApp.SourcePanelActor_Position.push_back(vtkSmartPointer<vtkActor>::New());
	theApp.SourcePanelActor_Position.back()->SetMapper(mapper);
	theApp.SourcePanelActor_Position.back()->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());
	theApp.SourcePanelActor_Position.back()->SetPosition(ptCenter);
	theApp.SourcePanelActor_Position.back()->SetPickable(false);
	theApp.SourcePanelActor_Position.back()->SetScale(pow(theApp.const_PointSource_ScaleFactorPerClick, theApp.m_Point_Source_Scale));

	theApp.theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(theApp.SourcePanelActor_Position.back());

	// 임시 actor 삭제
	Selecting3DShpere_Delete();

	theApp.theApp.m_pVTKWidget->renderWindow()->Render();
}
void SourceObjects::GenerateSourceActor_sourceCB(double* ptCenter) // Cone beam
{
	// Create a sphere
	vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
	sphereSource->SetCenter(0.0, 0.0, 0.0);
	sphereSource->SetRadius(0.5);

	// Make the surface smooth.
	sphereSource->SetPhiResolution(60);
	sphereSource->SetThetaResolution(30);

	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputConnection(sphereSource->GetOutputPort());

	vtkSmartPointer<vtkNamedColors> colors = vtkSmartPointer<vtkNamedColors>::New();

	// actor 생성	
	if(theApp.SourcePanelActor_Position.size()==0) theApp.SourcePanelActor_Position.push_back(vtkSmartPointer<vtkActor>::New());
	else
	{
		theApp.theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(theApp.SourcePanelActor_Position[0]); // 반투명 임시 actor 삭세
		theApp.SourcePanelActor_Position[0] = nullptr;
	}
	theApp.SourcePanelActor_Position[0]->SetMapper(mapper);
	theApp.SourcePanelActor_Position[0]->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());
	theApp.SourcePanelActor_Position[0]->SetPosition(ptCenter);
	theApp.SourcePanelActor_Position[0]->SetPickable(false);

	theApp.theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(theApp.SourcePanelActor_Position[0]);
	theApp.theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(theApp.NowSelecting3DSphereActor); // 반투명 임시 actor 삭세
	theApp.NowSelecting3DSphereActor = nullptr;

	theApp.theApp.m_pVTKWidget->renderWindow()->Render();
}
void SourceObjects::GenerateSourceDirectionActor_sourceCB()
{
	// 방향
	double maxAngle = theApp.theApp.pRt->m_lineEditConeBeamDirectionSolidAngle->text().toDouble();  // 예: 30도
	double a = theApp.theApp.pRt->m_lineEditConeBeamDirectionX->text().toDouble();
	double b = theApp.theApp.pRt->m_lineEditConeBeamDirectionY->text().toDouble();
	double c = theApp.theApp.pRt->m_lineEditConeBeamDirectionZ->text().toDouble();
	double cx = theApp.theApp.pRt->m_lineEditConeBeamPointX->text().toDouble();
	double cy = theApp.theApp.pRt->m_lineEditConeBeamPointY->text().toDouble();
	double cz = theApp.theApp.pRt->m_lineEditConeBeamPointZ->text().toDouble();
	int numPoints = 5 * theApp.theApp.pRt->m_lineEditConeBeamDirectionSolidAngle->text().toDouble();

	if(maxAngle <= 0 || (a == 0 && b ==0 && c ==0)) return;

	std::vector<Point_SolidAngle> points = GetPointsWithinSolidAngle_sourceCB(maxAngle, a, b, c, numPoints);

	// Delete remaining actors
	for (auto itr_actors : theApp.SourceDirectionActor)
	{
		theApp.theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(itr_actors);
	}
	theApp.SourceDirectionActor.clear();

	theApp.theApp.m_pVTKWidget->renderWindow()->Render();

	for (const Point_SolidAngle& point : points) {
		// Create a sphere
		vtkSmartPointer<vtkLineSource> lineSource =
			vtkSmartPointer<vtkLineSource>::New();

		lineSource->SetPoint1(0, 0, 0);  // origin
		lineSource->SetPoint2(point.x, point.y, point.z);

		vtkSmartPointer<vtkPolyDataMapper> mapper =
			vtkSmartPointer<vtkPolyDataMapper>::New();
		mapper->SetInputConnection(lineSource->GetOutputPort());

		vtkSmartPointer<vtkNamedColors> colors =
			vtkSmartPointer<vtkNamedColors>::New();

		// actor 생성
		theApp.SourceDirectionActor.push_back(vtkSmartPointer<vtkActor>::New());
		theApp.SourceDirectionActor.back()->SetMapper(mapper);
		theApp.SourceDirectionActor.back()->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());
		theApp.SourceDirectionActor.back()->GetProperty()->SetOpacity(1.0);
		theApp.SourceDirectionActor.back()->SetScale(100);

		double ptCenter[3];
		ptCenter[0] = cx;
		ptCenter[1] = cy;
		ptCenter[2] = cz;

		theApp.SourceDirectionActor.back()->SetPosition(ptCenter);
		theApp.SourceDirectionActor.back()->SetPickable(false);

		theApp.theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(theApp.SourceDirectionActor.back());
	}

	theApp.theApp.m_pVTKWidget->renderWindow()->Render();
}
void SourceObjects::GenerateSourceActor_sourceRC() 
{
	// Parameter 불러오기
	double XHalfLength = theApp.theApp.pRt->Box_HalfLengthX_sourceRC_QLineEdit->text().toDouble();
	double YHalfLength = theApp.theApp.pRt->Box_HalfLengthY_sourceRC_QLineEdit->text().toDouble();
	double ZHalfLength = theApp.theApp.pRt->Box_HalfLengthZ_sourceRC_QLineEdit->text().toDouble();
	double XCenter = theApp.theApp.pRt->Box_CenterX_sourceRC_QLineEdit->text().toDouble();
	double YCenter = theApp.theApp.pRt->Box_CenterY_sourceRC_QLineEdit->text().toDouble();
	double ZCenter = theApp.theApp.pRt->Box_CenterZ_sourceRC_QLineEdit->text().toDouble();

	// Parameter 불러오기 ENDS
	auto property1 = Util::GetProperty();
	double color1[3] = { 1., 0., 0. };
	property1->SetColor(color1);
	property1->SetInterpolationToPhong();
	property1->SetOpacity(0.2);

	// Create the outer cube
	vtkSmartPointer<vtkCubeSource> outerCube = vtkSmartPointer<vtkCubeSource>::New();
	outerCube->SetXLength(2 * XHalfLength);
	outerCube->SetYLength(2 * YHalfLength);
	outerCube->SetZLength(2 * ZHalfLength);
	outerCube->SetCenter(0, 0, 0);
	outerCube->Update();

	vtkSmartPointer<vtkPolyDataMapper> mapper1 = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper1->SetInputConnection(outerCube->GetOutputPort());
		
	theApp.theApp.SourcePanelActor_Position.push_back(vtkSmartPointer<vtkActor>::New());
	if (theApp.theApp.SourcePanelActor_Position.size() != 1)// 벡터 크기가 1이 아니어서 기존에 있었을때
	{
		theApp.theApp.theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(theApp.theApp.SourcePanelActor_Position[0]);
		theApp.theApp.SourcePanelActor_Position = { theApp.theApp.SourcePanelActor_Position.back() };
	}
	theApp.theApp.SourcePanelActor_Position[0]->SetMapper(mapper1);

	theApp.theApp.SourcePanelActor_Position[0]->SetProperty(property1);
	theApp.theApp.SourcePanelActor_Position[0]->PickableOff();
	theApp.theApp.SourcePanelActor_Position[0]->SetPosition(XCenter, YCenter, ZCenter);
	
	theApp.theApp.theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(theApp.theApp.SourcePanelActor_Position[0]);
	theApp.theApp.theApp.m_pVTKWidget->renderWindow()->Render();
}
//void SourceObjects::GenerateSourceActor_sourceEC() {} // Under-development
void SourceObjects::GenerateSourceActor_sourcePB(double* center_radius, double* theta_phi) // Parallel Beam
{
	// Reset process
	for (auto itr : theApp.theApp.SourcePanelActor_Position)
	{
		if (itr != nullptr)
		{
			theApp.theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(itr);
			itr = nullptr;
		}
	}

	double center_x = center_radius[0];
	double center_y = center_radius[1];
	double center_z = center_radius[2];
	double center[3] = { center_x , center_y , center_z };
	double radius = center_radius[3];
	// theta_phi[0] = theta (polar angle in degrees)
	// theta_phi[1] = phi (azimuthal angle in degrees)
	double theta = theta_phi[0];  // Keep in degrees for SetOrientation
	double phi = theta_phi[1];    // Keep in degrees for SetOrientation

	double theta_rad = theta * vtkMath::Pi() / 180.0;
	double phi_rad = phi * vtkMath::Pi() / 180.0;
	double dir_x = sin(theta_rad) * cos(phi_rad);
	double dir_y = sin(theta_rad) * sin(phi_rad);
	double dir_z = cos(theta_rad);

	// ===== DISK (원판) =====

	// Create a filled circle (disk with inner radius 0)
	vtkSmartPointer<vtkDiskSource> diskSource = vtkSmartPointer<vtkDiskSource>::New();
	diskSource->SetInnerRadius(0.0); // 내부 반지름을 0으로 설정하여 채워진 원(circle) 생성
	diskSource->SetOuterRadius(radius); // 외부 반지름 설정 (circle의 반지름)
	// 원의 세그먼트 수 설정 (선택 사항)
	diskSource->SetRadialResolution(30);
	diskSource->SetCircumferentialResolution(60);
	
	// ===== Arrow (화살표) =====

	// Arrow parameters
	double ArrowLength = radius * 0.5;  // 화살표 전체 길이 (원 반경의 0.5배)
	if (ArrowLength < 5) ArrowLength = 5;
	double CylinderHeight = ArrowLength * 0.3;  // 실린더 높이 (화살표 길이의 0.3)
	double CylinderRadius = radius * 0.02;  // 실린더 반경 (원 반경의 0.02)
	if (CylinderRadius < 0.2) CylinderRadius = 0.2;
	double ConeHeight = ArrowLength * 0.1;  // 콘 높이 (화살표 길이의 0.1)
	double ConeRadius = radius * 0.03;  // 콘 반경 (원 반경의 0.03)
	if (ConeRadius < 0.3) ConeRadius = 0.3;

	// Create a cylinder (shaft of the arrow)
	vtkSmartPointer<vtkCylinderSource> cylinderSource = vtkSmartPointer<vtkCylinderSource>::New();
	cylinderSource->SetHeight(CylinderHeight);
	cylinderSource->SetRadius(CylinderRadius);
	cylinderSource->SetResolution(30);
		
	vtkSmartPointer<vtkTransform> transform_cylinder = vtkSmartPointer<vtkTransform>::New();
	transform_cylinder->PostMultiply(); //this is the key line
	transform_cylinder->RotateX(90);
	transform_cylinder->Translate(0, 0, CylinderHeight / 2.0); // (0,0,1) +z방향이 기본 형태	
	vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter_cylinder = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	transformFilter_cylinder->SetInputConnection(cylinderSource->GetOutputPort());
	transformFilter_cylinder->SetTransform(transform_cylinder);
	transformFilter_cylinder->Update();
	
	// Create a cone (arrowhead)
	vtkSmartPointer<vtkConeSource> coneSource = vtkSmartPointer<vtkConeSource>::New();
	coneSource->SetHeight(ConeHeight);
	coneSource->SetRadius(ConeRadius);
	coneSource->SetResolution(30);

	vtkSmartPointer<vtkTransform> transform_cone = vtkSmartPointer<vtkTransform>::New();
	transform_cone->PostMultiply(); //this is the key line
	transform_cone->RotateY(-90);
	transform_cone->Translate(0, 0, CylinderHeight + ConeHeight / 2.0); // (0,0,1) +z방향이 기본 형태	
	vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter_cone = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	transformFilter_cone->SetInputConnection(coneSource->GetOutputPort());
	transformFilter_cone->SetTransform(transform_cone);
	transformFilter_cone->Update();

	auto appendFilter = vtkSmartPointer<vtkAppendPolyData>::New();
	appendFilter->AddInputConnection(diskSource->GetOutputPort());
	appendFilter->AddInputData(transformFilter_cylinder->GetOutput());
	appendFilter->AddInputData(transformFilter_cone->GetOutput());
	if (radius == 0) // pencil beam이면 점으로 추가
	{
		// Create a sphere
		vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
		sphereSource->SetCenter(0.0, 0.0, 0.0);
		sphereSource->SetRadius(0.5);

		// Make the surface smooth.
		sphereSource->SetPhiResolution(60);
		sphereSource->SetThetaResolution(30);

		appendFilter->AddInputConnection(sphereSource->GetOutputPort());
	}
	appendFilter->Update();

	auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputConnection(appendFilter->GetOutputPort());

	theApp.SourcePanelActor_Position.push_back(vtkSmartPointer<vtkActor>::New());
	theApp.SourcePanelActor_Position.back()->SetMapper(mapper);
	vtkSmartPointer<vtkNamedColors> colors = vtkSmartPointer<vtkNamedColors>::New();
	theApp.SourcePanelActor_Position.back()->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());
	theApp.SourcePanelActor_Position.back()->SetPickable(false);

	// Add cylinder actor to renderer
	theApp.theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(theApp.SourcePanelActor_Position.back());

	theApp.SourcePanelActor_Position.back()->SetPosition(center_x, center_y, center_z);

	double orientationAngles[3] = { 0, 0, 0 };
	orientationAngles[0] = 0;      // X축 회전 불필요
	orientationAngles[1] = theta;  // Y축 회전으로 theta 적용
	orientationAngles[2] = phi;    // Z축 회전으로 phi 적용

	theApp.SourcePanelActor_Position.back()->SetOrientation(orientationAngles);

	theApp.theApp.m_pVTKWidget->renderWindow()->Render();
}
void SourceObjects::Selecting3DShpere(double* ptCenter) // 마우스 클릭한 지점을 임시로 표시하는 기능
{
	if (theApp.theApp.NowSelecting3DSphereActor != nullptr)
	{
		theApp.theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(theApp.NowSelecting3DSphereActor);
		theApp.NowSelecting3DSphereActor = nullptr; // Actor 삭제
		//theApp.m_pVTKWidget->renderWindow()->Render();
		theApp.m_pVTKWidget->renderWindow()->Render();
	}

	// Create a sphere
	vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
	sphereSource->SetCenter(0.0, 0.0, 0.0);
	sphereSource->SetRadius(0.5);

	// Make the surface smooth.
	sphereSource->SetPhiResolution(60);
	sphereSource->SetThetaResolution(30);

	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputConnection(sphereSource->GetOutputPort());

	vtkSmartPointer<vtkNamedColors> colors = vtkSmartPointer<vtkNamedColors>::New();

	// actor 생성	 
	theApp.NowSelecting3DSphereActor = vtkSmartPointer<vtkActor>::New();
	theApp.NowSelecting3DSphereActor->SetMapper(mapper);
	theApp.NowSelecting3DSphereActor->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());
	theApp.NowSelecting3DSphereActor->SetPosition(ptCenter);
	theApp.NowSelecting3DSphereActor->SetPickable(false);
	theApp.NowSelecting3DSphereActor->GetProperty()->SetOpacity(0.2);	

	theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(theApp.NowSelecting3DSphereActor);
	//theApp.m_pVTKWidget->renderWindow()->Render();
	theApp.m_pVTKWidget->renderWindow()->Render();
}
void SourceObjects::Selecting3DShpere_Delete()
{
	if (theApp.NowSelecting3DSphereActor != nullptr)
	{
		theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(theApp.NowSelecting3DSphereActor);
		theApp.NowSelecting3DSphereActor = nullptr; // Actor 삭제
		//theApp.m_pVTKWidget->renderWindow()->Render();
		theApp.m_pVTKWidget->renderWindow()->Render();
	}
}
bool SourceObjects::LoadPSF_data(QString path) // 위의 file header 주석은 사용자가 직접 지워주어야 함
{
	theApp.theApp.PSF_data.clear(); // 입자 정보 순서대로 나열한 data
	theApp.theApp.PSF_location_vector.clear(); // 위치만 포함된 vector (가시화용)
	theApp.theApp.PSF_direction_vector.clear(); // 방향만 포함된 vector (가시화용)

	QFont font;
	font.setFamily("Arial");    // Font family
	font.setPointSize(theApp.theApp.pRt->FontSizeScaling(10));      // Font size
	font.setWeight(QFont::Bold);   // Font weight

	std::string stdPath = path.toStdString();
	QString qPath = QString::fromStdString(stdPath);

	// Progress Dialog 초기화
	QProgressDialog progressDialog;
	progressDialog.setWindowFlag(Qt::WindowStaysOnTopHint);
	progressDialog.setWindowModality(Qt::WindowModal);
	progressDialog.setLabelText("Initializing...");
	progressDialog.setCancelButton(nullptr);
	progressDialog.setMinimumDuration(0);
	progressDialog.setRange(0, 0); // Indeterminate
	progressDialog.setFont(font);
	progressDialog.show();
	QCoreApplication::processEvents();

	// ====== [IAEA 처리 시작] ======
	if (theApp.theApp.pRt->sourcePS_IAEA_button->isChecked())
	{
		// 경로에서 .IAEAphsp 확장자 제거
		std::string IAEA_filename = stdPath;
		std::string IAEA_extension = ".IAEAphsp";
		size_t extension_pos = IAEA_filename.find(IAEA_extension);
		if (extension_pos != std::string::npos)
			IAEA_filename.erase(extension_pos);

		// .IAEAheader 파일 존재 확인
		QString headerFilePath = QString::fromStdString(IAEA_filename) + ".IAEAheader";
		QFileInfo headerFileInfo(headerFilePath);
		if (!headerFileInfo.exists()) {
			theApp.SetMessageBox("Cannot find IAEAheader file");
			return false;
		}

		QString basePath = QString::fromStdString(IAEA_filename);
		QString exePath = ".\\Ext_lib\\IAEA2ascii.exe";

		progressDialog.setLabelText("Loading " + basePath + ".IAEAphsp\nThis process will take a few minutes.");
		QCoreApplication::processEvents();

		QString output_IAEA;
		// QProcess로 실행 -> 외부.exe 실행 폴링방식으로 메인스레드에서 while문 내에서 0.1초마다 잠깐 porcessEvents()를 호출하여 로딩창 업데이트
		{
			QProcess process;
			process.setProgram(exePath);
			process.setArguments(QStringList() << basePath);
			process.setProcessChannelMode(QProcess::MergedChannels); // stdout + stderr 같이 읽기
			process.start();

			if (!process.waitForStarted()) {
				theApp.SetMessageBox("Cannot start IAEA2ascii.exe");
				return false;
			}

			while (!process.waitForFinished(100)) {
				// 0.1초마다 UI 갱신 (중간에 응답 없음 방지)
				QCoreApplication::processEvents();
			}

			output_IAEA = process.readAllStandardOutput();
		}
		
		if (output_IAEA.contains("Normal Program Termination")) {
		}
		else {
			theApp.SetMessageBox("IAEA2ascii conversion failed.");
			return false;
		}
		QStringList output_lines = output_IAEA.split('\n');
		QString history_keyword = " Total number of histories:";
		int totalHistories = -1;

		for (const QString& line : output_lines)
		{
			if (line.contains(history_keyword))
			{
				// 키워드 이후의 문자열 잘라냄
				QString numberPart = line.section(history_keyword, 1).trimmed();

				// 숫자만 남기기 (혹시 공백 등 있을 경우 대비)
				bool ok = false;
				totalHistories = numberPart.toInt(&ok);
				if (ok) break;
			}
		}
		if (totalHistories == -1)
		{
			qDebug() << "Keyword not found or number parse error";
			return false;
		}

		stdPath = IAEA_filename + ".txt"; // ASCII 파일로 경로 변경
	}
	// ====== [일반 PSF 파일 처리] ======
	std::ifstream ifp_phsp_size(stdPath, std::ios::binary);
	ifp_phsp_size.seekg(0, std::ios::end);
	std::streamsize size = ifp_phsp_size.tellg();
	double fileSize = static_cast<double>(size);
	ifp_phsp_size.close();

	double size_scale_factor = 100.0 / static_cast<double>(fileSize);

	// ====== [데이터 읽기 시작] ======

	std::ifstream ifp(stdPath);
	std::string line;
	std::string particle, x, y, z, u, v, w, E, SWF;
	double pCode;
	std::string pCode_str_PHITS;
	theApp.pRt->PhaseSpaceFileRow = 0;

	progressDialog.setLabelText("Reading phase space file...");
	progressDialog.setRange(0, 100);
	QCoreApplication::processEvents();

	int lastPercent = -1;
	while (std::getline(ifp, line)) {
		// Trim any leading whitespace from the line for safety
		line.erase(0, line.find_first_not_of(" \t"));

		// Skip lines starting with '#' -> In FLUKA or 주석
		if (!line.empty() && line[0] == '#') {
			continue;
		}

		std::istringstream iss(line); // Create a stream from the line for parsing
		if (theApp.pRt->sourcePS_MCNP_button->isChecked()) // MCNP6
		{
			iss >> pCode >> x >> y >> z >> u >> v >> w >> E >> SWF; // Converted ASCII-formatted MCNP WSSA file
			if (pCode == 22) particle = "gamma";
			else if (pCode == 11) particle = "e-";
			else if (pCode == -11) particle = "e+";
			else if (pCode == 2112) particle = "neutron";
			else if (pCode == 2212) particle = "proton";
			else if (pCode == 2000004) particle = "alpha";
			else
			{
				theApp.SetMessageBox("Invalid particle code!!");
				theApp.PSF_data.clear();
				theApp.PSF_location_vector.clear();
				theApp.PSF_direction_vector.clear();
				ifp.close();
				return false;
			}
		}
		if (theApp.pRt->sourcePS_PHITS_button->isChecked()) // PHITS
		{
			iss >> pCode_str_PHITS >> x >> y >> z >> u >> v >> w >> E >> SWF; // PHITS dmp file format (pCode, cm, cm, cm, u, v, w, MeV, SWF), manual 98p.
			pCode = std::stod(PSF_PHITS_replaceDwithE(pCode_str_PHITS)); // PHITS는 거듭제곱으로 E대신 D를 씀
			x = PSF_PHITS_replaceDwithE(x);
			y = PSF_PHITS_replaceDwithE(y);
			z = PSF_PHITS_replaceDwithE(z);
			u = PSF_PHITS_replaceDwithE(u);
			v = PSF_PHITS_replaceDwithE(v);
			w = PSF_PHITS_replaceDwithE(w);
			E = PSF_PHITS_replaceDwithE(E);
			SWF = PSF_PHITS_replaceDwithE(SWF);
			if (pCode == 22) particle = "gamma";
			else if (pCode == 11) particle = "e-";
			else if (pCode == -11) particle = "e+";
			else if (pCode == 2112) particle = "neutron";
			else if (pCode == 2212) particle = "proton";
			else if (pCode == 2000004) particle = "alpha";
			else
			{
				theApp.SetMessageBox("Invalid particle code!!");
				theApp.PSF_data.clear();
				theApp.PSF_location_vector.clear();
				theApp.PSF_direction_vector.clear();
				ifp.close();
				return false;
			}
		}
		if (theApp.pRt->sourcePS_FLUKA_button->isChecked()) // FLUKA
		{
			iss >> pCode >> E >> x >> y >> z >> u >> v >> w >> SWF; // FLUKA phsp file 
			//iss >> pCode >> E >> z >> x >> y >> w >> u >> v >> SWF; // Coords converted Otto FULKA phsp file 
			if (pCode == 22 || pCode == 7) particle = "gamma";
			else if (pCode == 11 || pCode == 3) particle = "e-";
			else if (pCode == -11 || pCode == 4) particle = "e+";
			else if (pCode == 2112 || pCode == 8) particle = "neutron";
			else if (pCode == 2212 || pCode == 1) particle = "proton";
			else if (pCode == 2000004 || pCode == -6) particle = "alpha";
			else
			{
				theApp.SetMessageBox("Invalid particle code!!");
				theApp.PSF_data.clear();
				theApp.PSF_location_vector.clear();
				theApp.PSF_direction_vector.clear();
				ifp.close();
				return false;
			}
			E = std::to_string(std::stod(E) * 1000.); // GeV to MeV
		}
		if (theApp.pRt->sourcePS_IAEA_button->isChecked()) // IAEAphsp
		{
			double charge, history;
			iss >> pCode >> charge >> x >> y >> z >> u >> v >> w >> E >> SWF >> history; 
			if (pCode == 1) particle = "gamma";
			else if (pCode == 2) particle = "e-";
			else if (pCode == 3) particle = "e+";
			else if (pCode == 4) particle = "neutron";
			else if (pCode == 5) particle = "proton";
			else
			{
				theApp.SetMessageBox("Invalid particle code!!");
				theApp.PSF_data.clear();
				theApp.PSF_location_vector.clear();
				theApp.PSF_direction_vector.clear();
				ifp.close();
				return false;
			}
		}
		if (theApp.pRt->sourcePS_USER_Button->isChecked())
		{
			iss >> particle >> x >> y >> z >> u >> v >> w >> E >> SWF; // USER sequence (particle_ASCII, cm, cm, cm, u, v, w, MeV, SWF)
		}

		//
		std::streampos currentPos = ifp.tellg();
		if (currentPos != -1)
		{
			int percent = static_cast<int>(currentPos * size_scale_factor);

			if (percent != lastPercent && percent % 1 == 0)
			{
				lastPercent = percent;
				progressDialog.setValue(percent);
				progressDialog.setLabelText(QString("Reading phase space file... %1%").arg(percent));
				QCoreApplication::processEvents();
			}
		}
		theApp.PSF_data.push_back(std::make_tuple(particle, x, y, z, u, v, w, E, SWF));
		theApp.PSF_location_vector.push_back(std::make_tuple(std::stof(x), std::stof(y), std::stof(z)));
		theApp.PSF_direction_vector.push_back(std::make_tuple(std::stof(u), std::stof(v), std::stof(w)));
		theApp.pRt->PhaseSpaceFileRow++;

	}
	progressDialog.close();
	GenerateSourceActor_sourcePSF(theApp.PSF_location_vector, theApp.PSF_direction_vector);

	ifp.close();
	if (theApp.pRt->sourcePS_IAEA_button->isChecked())
	{
		remove(stdPath.c_str());
	}
	return true;
}

std::vector<Point_SolidAngle> SourceObjects::GetPointsWithinSolidAngle_sourceCB(double maxAngle, double a, double b, double c, int numPoints) {
	// a, b, c 벡터를 정규화
	Vector3 InputDirectionalVector(a, b, c);
	InputDirectionalVector = InputDirectionalVector.normalize();
	std::vector<Point_SolidAngle> result;

	while (result.size() < numPoints) {
		double theta = M_PI * ((double)rand() / RAND_MAX);
		double phi = 2.0 * M_PI * ((double)rand() / RAND_MAX);

		// 구 좌표를 직교 좌표로 변환
		double X = sin(theta) * cos(phi);
		double Y = sin(theta) * sin(phi);
		double Z = cos(theta);
		Vector3 randomDirectionVector(X, Y, Z);
		randomDirectionVector = randomDirectionVector.normalize();

		// (a, b, c)와의 각도 계산
		double cosMaxAngle = cos(maxAngle*M_PI / 180.0);
		double cosInputAngle = InputDirectionalVector.dot(randomDirectionVector);

		// 각도가 maxAngle 이내면 결과에 추가
		if (cosInputAngle >= cosMaxAngle) {
			result.push_back({ theta, phi, X, Y, Z });
		}
	}
	return result;
}

void SourceObjects::PSF_MCNP_ssw_parse_file(const std::string& filename) {
	std::ifstream file(filename, std::ios::binary);
	if (!file.is_open()) {
		theApp.SetMessageBox("Unable to open WSSA file!");
	}
	std::ofstream out("./data/WSSA_converted_ASCII_file");    //  output file 출력

	uint32_t reclen = 4;  // 기본적으로 4바이트로 설정
	uint64_t lbuf;
	std::vector<char> buf(1024);  // 초기 버퍼 크기 설정

	while (file) {
		if (!PSF_MCNP_ssw_loadrecord(file, buf, reclen, lbuf)) {
			break;  // 파일 끝이나 오류 발생 시 종료
		}

		// np1, nrss, nrcd, njsw 값을 추출
		if (lbuf >= 88) {  // double은 8바이트이므로 최소 32바이트가 필요
			// 데이터를 double 타입으로 해석
			double np1 = *reinterpret_cast<double*>(buf.data());
			double np2 = *reinterpret_cast<double*>(buf.data() + 8);
			int64_t nx = np2;
			if (nx < 0) nx = nx * (-1);
			nx /= 4;
			double np3 = *reinterpret_cast<double*>(buf.data() + 16);
			double np4 = *reinterpret_cast<double*>(buf.data() + 24);
			double np5 = *reinterpret_cast<double*>(buf.data() + 32);
			double np6 = *reinterpret_cast<double*>(buf.data() + 40);
			double np7 = *reinterpret_cast<double*>(buf.data() + 48);
			double np8 = *reinterpret_cast<double*>(buf.data() + 56);
			double np9 = *reinterpret_cast<double*>(buf.data() + 64);   //  dirx
			double dirx = np9;
			double np10 = *reinterpret_cast<double*>(buf.data() + 72);  //  diry
			double diry = np10;
			double np11 = *reinterpret_cast<double*>(buf.data() + 80);


			double dirz = sqrt(fmax(0.0, 1.0 - dirx * dirx - diry * diry));

			if (np2 < 0)
			{
				dirz = dirz * (-1);
			}

			int pdgcode = PSF_MCNP_conv_mcnp6_ssw2pdg(nx);

			std::cout << pdgcode << " " << np6 << " " << np7 << " " << np8 << " " << np9 << " " << np10 << " " << dirz << " " << np4 << " " << np3 << std::endl;

			if (pdgcode != 0) out << pdgcode << " " << np6 << " " << np7 << " " << np8 << " " << np9 << " " << np10 << " " << dirz << " " << np4 << " " << np3 << std::endl;

		}
	}
	out.close();

	file.close();
}
int SourceObjects::PSF_MCNP_ssw_loadrecord(std::ifstream& file, std::vector<char>& buf, uint32_t reclen, uint64_t& lbuf)
{
	if (reclen == 4) {
		uint32_t rl;
		if (!PSF_MCNP_ssw_readbytes(file, reinterpret_cast<char*>(&rl), 4)) {
			return 0;
		}
		lbuf = rl;
	}
	else {
		uint64_t rl;
		if (!PSF_MCNP_ssw_readbytes(file, reinterpret_cast<char*>(&rl), 8)) {
			return 0;
		}
		lbuf = rl;
	}

	if (lbuf > buf.size()) {
		// 필요한 경우 버퍼 크기를 늘림
		buf.resize(static_cast<size_t>(lbuf)); // 여기서 uint64_t -> size_t 캐스팅
	}

	if (!PSF_MCNP_ssw_readbytes(file, buf.data(), static_cast<size_t>(lbuf))) {
		return 0;
	}

	// 레코드 끝에서 동일한 크기인지 확인
	if (reclen == 4) {
		uint32_t rl;
		return PSF_MCNP_ssw_readbytes(file, reinterpret_cast<char*>(&rl), 4) && lbuf == rl;
	}
	else {
		uint64_t rl;
		return PSF_MCNP_ssw_readbytes(file, reinterpret_cast<char*>(&rl), 8) && lbuf == rl;
	}
}
bool SourceObjects::PSF_MCNP_ssw_readbytes(std::ifstream& file, char* buf, size_t bytes) {
	if (!file.read(buf, bytes)) {
		return false;
	}
	return true;
}
int32_t SourceObjects::PSF_MCNP_conv_mcnp6_ssw2pdg(int32_t c) {
	std::vector<int32_t> conv_mcnp6_to_pdg_0to36 = { 0, 2112, 22, 11, 13, -2112, 12, 14, -11, 2212, 3122,
												3222, 3112, 3322, 3312, 3334, -13, -12, -14, -2212, 211,
												111, 321, 310, 130, -3122, -3222, -3112, -3322, -3312, -3334,
												1000010020, 1000010030, 1000020030, 1000020040, -211, -321 };
	if (c < 0)
		return 0;  // 유효하지 않은 값일 경우 0 반환

	int antibit = c % 2;  // 반입자 여부 결정 (1이면 반입자)
	c /= 2;               // 나머지 데이터 추출

	int ptype = c % 64;    // 입자 유형 추출 (0~63 범위)
	c /= 64;

	if (ptype <= 36) {
		// 입자 유형이 36 이하일 때 PDG 코드를 변환
		int32_t p = conv_mcnp6_to_pdg_0to36[ptype];

		// 반입자일 경우 음수로 변환, 단 감마선(p == 22) 제외
		return (antibit && p != 22) ? -p : p;
	}

	return 0;  // ptype이 36보다 크면 0 반환 (유효하지 않은 경우)
}
std::string SourceObjects::PSF_PHITS_replaceDwithE(std::string input)
{
	std::string result = input;
	size_t pos = result.find('D');  // Find the position of 'D'
	if (pos != std::string::npos) { // Check if 'D' was found
		result[pos] = 'E';          // Replace 'D' with 'E'
	}
	return result;
}
