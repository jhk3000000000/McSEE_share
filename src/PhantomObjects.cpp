//PhantomObjects.cpp
#include "pch.h"
#include "PhantomObjects.h"
#include "ETHuman3DApp.h"       // theApp 변수 접근용
#include "FunctionPanelRight.h" // UI 접근용
#include "ETQVTKWidget.h"       // 3D 뷰어 접근용
#include "Util.h"               // Util 기능 사용
#include "ETInteractorStyleRubberBand.h"

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

// =========================================================
// 1. [Public] 외부 호출 함수 구현
// =========================================================

////////////////// Multiple Phantom //////////////////
void PhantomObjects::PhantomPolydataActor_Generate(QString strFileName, int SelectedIndex, BodySizeInfo BodySizeInfo_for_ThisPhantom)
{	
	// Save phantom file name from phantomDir
	theApp.PhantomFileTitle[SelectedIndex] = QString::fromStdString(GetPhantomFileTitle(strFileName.toStdString()));
	// Save phantom sclae factor
	theApp.PhantomPolyDataScaleFactor[SelectedIndex][0] = BodySizeInfo_for_ThisPhantom.xyScale;
	theApp.PhantomPolyDataScaleFactor[SelectedIndex][1] = BodySizeInfo_for_ThisPhantom.zScale;
	// Generate phantom polydata
	vtkSmartPointer<vtkPolyData> polydata_phantom;
	if (theApp.pRt->m_Phantom_MainInfo[SelectedIndex][theApp.pRt->E_PHANTOMMAININFO_TYPE] != theApp.pRt->E_PHANTOMTYPE_IMPORTED) // Imported가 아닐 때
	{
		vtkSmartPointer<vtkOBJReader> reader = vtkSmartPointer<vtkOBJReader>::New();
		reader->SetFileName(Util::Wcs_to_mbs(strFileName.toStdWString()).c_str());
		reader->Update();
		polydata_phantom = Util::CreatePolyData(reader);
	}
	if (theApp.pRt->m_Phantom_MainInfo[SelectedIndex][theApp.pRt->E_PHANTOMMAININFO_TYPE] == theApp.pRt->E_PHANTOMTYPE_IMPORTED) // Imported 일때
	{
		std::vector<std::string> extract_organlist;
		extract_organlist.push_back("12200_Skin_surface");
		extract_organlist.push_back("6700_Cornea_left");
		extract_organlist.push_back("6900_Cornea_right");

		// 캡처한 변수들을 사용하여 ExtractPhantomOBJ 함수의 인자를 준비합니다.
		std::string path = theApp.m_ImportedPhantomFilePath_NoExtention[SelectedIndex].toStdString();
		std::string name = theApp.PhantomFileTitle[SelectedIndex].toStdString();

		// 백그라운드에서 시간이 오래 걸리는 함수를 호출합니다. 
		std::string outPath = ExtractPhantomOBJ(path, name, extract_organlist);

		QString QoutPath = QString::fromStdString(outPath);
		vtkSmartPointer<vtkOBJReader> reader = vtkSmartPointer<vtkOBJReader>::New();
		reader->SetFileName(Util::Wcs_to_mbs(QoutPath.toStdWString()).c_str());
		reader->Update();
		polydata_phantom = Util::CreatePolyData(reader);

		if (QFile::exists(QoutPath)) QFile::remove(QoutPath);
	}

	float SkinColor[3] = { 255, 192, 160 }; // Skin color
	vtkNew<vtkUnsignedCharArray> PhantomColors;
	PhantomColors->SetNumberOfComponents(3);
	PhantomColors->SetNumberOfTuples(polydata_phantom->GetNumberOfPoints());
	for (int i = 0; i < polydata_phantom->GetNumberOfPoints(); ++i)
	{
		PhantomColors->SetTuple(i, SkinColor);
	}
	polydata_phantom->GetPointData()->SetScalars(PhantomColors);
	// Generate polydata_original, polydata_base (which is set to 0,0,0 center and scaled (for body-size-dependent phantom)
	theApp.PhantomPolyData_original[SelectedIndex] = polydata_phantom; // Set polydata_original (.obj 불러온 그대로의 팬텀파일)
	double *pCenter = polydata_phantom->GetCenter();
	theApp.PhantomOrigianlPolyDataCenter[SelectedIndex][0] = pCenter[0]; 
    theApp.PhantomOrigianlPolyDataCenter[SelectedIndex][1] = pCenter[1]; 
    theApp.PhantomOrigianlPolyDataCenter[SelectedIndex][2] = pCenter[2]; // Set phantomOriginal center position
	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
	transform->PostMultiply(); //this is the key line
	transform->Translate(-theApp.PhantomOrigianlPolyDataCenter[SelectedIndex][0], -theApp.PhantomOrigianlPolyDataCenter[SelectedIndex][1], -theApp.PhantomOrigianlPolyDataCenter[SelectedIndex][2]); // 원본 파일을 offset 진행하여 GUI 상에서 (0,0,0)에 맞도록 설정
	transform->Scale(theApp.PhantomPolyDataScaleFactor[SelectedIndex][0], theApp.PhantomPolyDataScaleFactor[SelectedIndex][0], theApp.PhantomPolyDataScaleFactor[SelectedIndex][1]); // Body-size-dependent phantom sacling
	vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	transformFilter->SetInputData(polydata_phantom);
	transformFilter->SetTransform(transform);
	transformFilter->Update();
	theApp.PhantomPanelPolyData_Collection[SelectedIndex][0][0] = transformFilter->GetOutput(); // Set theApp.PhantomPanelPolyData_Collection (phantom_base -> 원점이동/스케일링된 polydata)
	
	// Generate actor
	vtkSmartPointer<vtkActor> actor_phantom = Util::CreateActor(theApp.PhantomPanelPolyData_Collection[SelectedIndex][0][0]);
	auto property = Util::GetProperty();
	property->SetInterpolationToPhong();
	property->SetDiffuse(1.0);
	actor_phantom->SetProperty(property);	
	// Full/Dummy phantom Actor save
	theApp.FullPhantomActor[SelectedIndex] = actor_phantom; //Full phantom
	if(theApp.pRt->m_Phantom_MainInfo[SelectedIndex][theApp.pRt->E_PHANTOMMAININFO_TYPE] == theApp.pRt->E_PHANTOMTYPE_ADULTMRCP
	|| theApp.pRt->m_Phantom_MainInfo[SelectedIndex][theApp.pRt->E_PHANTOMMAININFO_TYPE] == theApp.pRt->E_PHANTOMTYPE_PEDIATRICMRCP
	|| theApp.pRt->m_Phantom_MainInfo[SelectedIndex][theApp.pRt->E_PHANTOMMAININFO_TYPE] == theApp.pRt->E_PHANTOMTYPE_TRANSFORMED) 
	{
		//Dummy phantom
		vtkSmartPointer<vtkOBJReader> reader_dummy = vtkSmartPointer<vtkOBJReader>::New();
		QString dummyQstr;
		dummyQstr = "./data/phantom/dummy/" + BodySizeInfo_for_ThisPhantom.HtWtName_dummy + ".obj";		
		reader_dummy->SetFileName(Util::Wcs_to_mbs(dummyQstr.toStdWString()).c_str());
		reader_dummy->Update();
		vtkSmartPointer<vtkPolyData> polydata_dummy = Util::CreatePolyData(reader_dummy);
		float SkinColor_dummy[3] = { 255, 192, 160 }; // Skin color
		vtkNew<vtkUnsignedCharArray> PhantomColors_dummy;
		PhantomColors_dummy->SetNumberOfComponents(3);
		PhantomColors_dummy->SetNumberOfTuples(polydata_dummy->GetNumberOfPoints());
		for (int i = 0; i < polydata_dummy->GetNumberOfPoints(); ++i)
		{
			PhantomColors_dummy->SetTuple(i, SkinColor_dummy);
		}
		polydata_dummy->GetPointData()->SetScalars(PhantomColors_dummy);
		double *pCenter_dummy = polydata_dummy->GetCenter();
		theApp.DummyPolyDataInfo[SelectedIndex][0] = pCenter_dummy[0];
		theApp.DummyPolyDataInfo[SelectedIndex][1] = pCenter_dummy[1];
		theApp.DummyPolyDataInfo[SelectedIndex][2] = pCenter_dummy[2];
		theApp.DummyPolyDataInfo[SelectedIndex][3] = BodySizeInfo_for_ThisPhantom.xyScale_dummy;
		theApp.DummyPolyDataInfo[SelectedIndex][4] = BodySizeInfo_for_ThisPhantom.xyScale_dummy;
		theApp.DummyPolyDataInfo[SelectedIndex][5] = BodySizeInfo_for_ThisPhantom.zScale_dummy;
		vtkSmartPointer<vtkTransform> transform_dummy = vtkSmartPointer<vtkTransform>::New();
		transform_dummy->PostMultiply(); //this is the key line
		transform_dummy->Translate(-pCenter_dummy[0], -pCenter_dummy[1], -pCenter_dummy[2]);
		transform_dummy->Scale(BodySizeInfo_for_ThisPhantom.xyScale_dummy, BodySizeInfo_for_ThisPhantom.xyScale_dummy, BodySizeInfo_for_ThisPhantom.zScale_dummy); // Body-size-dependent phantom sacling
		vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter_dummy = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
		transformFilter_dummy->SetInputData(polydata_dummy);
		transformFilter_dummy->SetTransform(transform_dummy);
		transformFilter_dummy->Update();
		vtkSmartPointer<vtkActor> actor_dummy = Util::CreateActor(transformFilter_dummy->GetOutput());
		auto property_dummy = Util::GetProperty();
		property_dummy->SetInterpolationToPhong();
		property_dummy->SetDiffuse(1.0);
		actor_dummy->SetProperty(property_dummy);
		theApp.DummyPhantomActor[SelectedIndex] = actor_dummy;
	}
	AppendPhantomPolyData_GeneratePhantomActor(SelectedIndex);
}

void PhantomObjects::AppendPhantomPolyData_GeneratePhantomActor(int PhantomIdx)
{
	// Appending with phantom	
	theApp.PhantomPanelAccumulatedPolyData[PhantomIdx] = theApp.PhantomPanelPolyData_Collection[PhantomIdx][0][0]; // phantom polydata_base
	for (auto itr_PolyData : theApp.PhantomPanelPolyData_Collection[PhantomIdx][1]) // Clothing
	{
		vtkSmartPointer<vtkAppendFilter> appendFilter = vtkSmartPointer<vtkAppendFilter>::New();
		appendFilter->AddInputData(theApp.PhantomPanelAccumulatedPolyData[PhantomIdx]);
		appendFilter->AddInputData(itr_PolyData.second);
		appendFilter->Update();

		vtkSmartPointer<vtkGeometryFilter> geomFilter = vtkSmartPointer<vtkGeometryFilter>::New(); // For making appendfilter to polydata 
		geomFilter->SetInputData(appendFilter->GetOutput());
		geomFilter->Update();

		theApp.PhantomPanelAccumulatedPolyData[PhantomIdx] = geomFilter->GetOutput();
	}
	for (auto itr_PolyData : theApp.PhantomPanelPolyData_Collection[PhantomIdx][2]) // Dosimeter
	{
		vtkSmartPointer<vtkAppendFilter> appendFilter = vtkSmartPointer<vtkAppendFilter>::New();
		appendFilter->AddInputData(theApp.PhantomPanelAccumulatedPolyData[PhantomIdx]);
		appendFilter->AddInputData(itr_PolyData.second);
		appendFilter->Update();

		vtkSmartPointer<vtkGeometryFilter> geomFilter = vtkSmartPointer<vtkGeometryFilter>::New(); // For making appendfilter to polydata 
		geomFilter->SetInputData(appendFilter->GetOutput());
		geomFilter->Update();

		theApp.PhantomPanelAccumulatedPolyData[PhantomIdx] = geomFilter->GetOutput();
	}
	for (auto itr_PolyData : theApp.PhantomPanelPolyData_Collection[PhantomIdx][3]) // Glasses
	{
		vtkSmartPointer<vtkAppendFilter> appendFilter = vtkSmartPointer<vtkAppendFilter>::New();
		appendFilter->AddInputData(theApp.PhantomPanelAccumulatedPolyData[PhantomIdx]);
		appendFilter->AddInputData(itr_PolyData.second);
		appendFilter->Update();

		vtkSmartPointer<vtkGeometryFilter> geomFilter = vtkSmartPointer<vtkGeometryFilter>::New(); // For making appendfilter to polydata 
		geomFilter->SetInputData(appendFilter->GetOutput());
		geomFilter->Update();

		theApp.PhantomPanelAccumulatedPolyData[PhantomIdx] = geomFilter->GetOutput();
	}

	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(theApp.PhantomPanelAccumulatedPolyData[PhantomIdx]);
	mapper->SetScalarVisibility(true);

	// Generate actor
	theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(theApp.PhantomPanelActor[PhantomIdx]);
	theApp.PhantomPanelActor[PhantomIdx] = nullptr;
	theApp.PhantomPanelActor[PhantomIdx] = Util::CreateActor(theApp.PhantomPanelAccumulatedPolyData[PhantomIdx]);
	theApp.PhantomPanelActor[PhantomIdx]->SetMapper(mapper);
	auto property = Util::GetProperty();
	property->SetInterpolationToPhong();
	property->SetDiffuse(1.0);
	theApp.PhantomPanelActor[PhantomIdx]->SetProperty(property);

	// Translate actor
	double height = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][4]; // Set phantomInfo
	double weight = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][5];
	double CenterX = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][6];
	double CenterY = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][7];
	double CenterZ = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][8];
	double VectorX = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][9];
	double VectorY = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][10];
	double VectorZ = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][11];
	theApp.PhantomPanelActor[PhantomIdx]->SetPosition(CenterX, CenterY, CenterZ);
	theApp.PhantomPanelActor[PhantomIdx]->SetOrientation(VectorX, VectorY, VectorZ);

	// Add Actor to View
	theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(theApp.PhantomPanelActor[PhantomIdx]);
	//theApp.m_pVTKWidget->renderWindow()->Render();
	theApp.m_pVTKWidget->renderWindow()->Render();
}
BodySizeInfo PhantomObjects::CalcBoydSizeScaleFactor(int phantomType, int phantomGender, int phantomAge, int phantomPosture, double phantomHeight, double phantomWeight)
{
	BodySizeInfo BodySizeInfo_for_ThisPhantom;

	// Set Deformed Phantom List
	//std::vector<double> AdultMaleLibrary_Height = { 160,160,160,160,160,160,165,165,165,165,165,165,165,165,165,165,165,165,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,175,175,
	//	175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,185,185,185,185,185,185,185,185,185,
	//	185,185,185,185,185,185,185,185,185,185,190,190,190,190,190,190,190,190,190,190,190,190,190,190,190 };
	//std::vector<double> AdultMaleLibrary_Weight = { 50,55,60,65,70,75,50,55,60,65,70,75,80,85,90,95,100,105,110,50,55,60,65,70,75,80,85,90,95,100,105,110,115,120,125,55,60,65,70,75,80,85,90,95,100,
	//	105,110,115,120,125,130,135,140,55,60,65,70,75,80,85,90,95,100,105,110,115,120,125,130,135,140,145,150,155,65,70,75,80,85,90,95,100,105,110,115,120,125,130,135,140,145,150,155,70,75,80,
	//	85,90,95,100,105,110,115,120,125,130,135,140};
	//std::vector<double> AdultFemaleLibrary_Height = { 150,150,150,150,150,150,150,150,150,150,150,150,155,155,155,155,155,155,155,155,155,155,155,155,155,155,155,155,160,160,160,160,160,160,160,160,
	//	160,160,160,160,160,160,160,160,160,160,165,165,165,165,165,165,165,165,165,165,165,165,165,165,165,165,165,165,165,165,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,
	//	170,170,170,170,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175 };
	//std::vector<double> AdultFemaleLibrary_Weight = { 40,45,50,55,60,65,70,75,80,85,90,95,40,45,50,55,60,65,70,75,80,85,90,95,100,105,110,115,45,50,55,60,65,70,75,80,85,90,95,100,105,110,115,120,125,
	//	130,45,50,55,60,65,70,75,80,85,90,95,100,105,110,115,120,125,130,135,140,50,55,60,65,70,75,80,85,90,95,100,105,110,115,120,125,130,135,140,145,150,55,60,65,70,75,80,85,90,95,100,105,110,115,
	//	120,125,130,135	};
	std::vector<double> AdultMaleAnchor_Height = { 160,185,160,190,170,165,175,170,170,170,165 };
	std::vector<double> AdultMaleAnchor_Weight = { 65,90,70,105,85,85,100,95,100,105,105 };
	std::vector<double> AdultFemaleAnchor_Height = { 155,175,160,150,175,175,175,165,175,165,170 };	
	std::vector<double> AdultFemaleAnchor_Weight = { 60,80,70,65,95,100,105,95,110,105,120 };

	std::vector<double> Pediatric_15M_Anchor_Height = { 170, 170, 170, 170, 170, 170, 170 };
	std::vector<double> Pediatric_15M_Anchor_Weight = { 63.777, 67.059, 70.698, 74.754, 79.304, 84.444, 90.296 };
	std::vector<double> Pediatric_10M_Anchor_Height = { 135, 135, 135, 135, 135, 135, 135 };
	std::vector<double> Pediatric_10M_Anchor_Weight = { 32.265, 33.708, 35.287, 37.020, 38.933, 41.053, 43.419 };
	std::vector<double> Pediatric_05M_Anchor_Height = { 105, 105, 105, 105, 105 };
	std::vector<double> Pediatric_05M_Anchor_Weight = { 17.942, 18.786, 19.713, 20.736, 21.871 }; 
	std::vector<double> Pediatric_01M_Anchor_Height = { 81, 81, 81, 81, 81 };
	std::vector<double> Pediatric_01M_Anchor_Weight = { 10.317, 10.650, 11.004, 11.383, 11.789 };
	std::vector<double> Pediatric_00M_Anchor_Height = { 48, 48, 50 };
	std::vector<double> Pediatric_00M_Anchor_Weight = { 2.7, 3.1, 3.7 };

	std::vector<double> Pediatric_15F_Anchor_Height = { 165, 165, 165, 165, 165, 165, 165 };
	std::vector<double> Pediatric_15F_Anchor_Weight = { 59.212, 62.439, 66.038, 70.078, 74.644, 79.847, 85.829 };
	std::vector<double> Pediatric_10F_Anchor_Height = { 134, 134, 134, 134, 134, 134, 134 };
	std::vector<double> Pediatric_10F_Anchor_Weight = { 31.800, 33.281, 34.908, 36.702, 38.690, 40.905, 43.600 };
	std::vector<double> Pediatric_05F_Anchor_Height = { 105, 105, 105, 105, 105 };
	std::vector<double> Pediatric_05F_Anchor_Weight = { 18.163, 18.944, 19.795, 20.726, 21.749 };
	std::vector<double> Pediatric_01F_Anchor_Height = { 79, 79, 79, 79, 79 };
	std::vector<double> Pediatric_01F_Anchor_Weight = { 9.814, 10.227, 10.676, 11.166, 11.703 };
	std::vector<double> Pediatric_00F_Anchor_Height = { 47, 49, 51 };
	std::vector<double> Pediatric_00F_Anchor_Weight = { 2.6, 3.2, 3.8 };

	// Get global variables
	double phantomBMI = phantomWeight / phantomHeight / phantomHeight * 10000;
	// Initialize variables
	double xy_scale = 1;
	double z_scale = 1;
	QString filename;	
	double new_weight = 1.;
	std::vector<double> HeightVector;
	std::vector<double> WeightVector;
	double reference_height;
	double reference_weight;

	if (phantomType == theApp.pRt->E_PHANTOMTYPE_ADULTMRCP || phantomType == theApp.pRt->E_PHANTOMTYPE_PEDIATRICMRCP) // MRCP
	{
		BodySizeInfo_for_ThisPhantom.zScale = 1;
		BodySizeInfo_for_ThisPhantom.xyScale = 1;
		BodySizeInfo_for_ThisPhantom.zScale_dummy = 1;
		BodySizeInfo_for_ThisPhantom.xyScale_dummy = 1;

		BodySizeInfo_for_ThisPhantom.HtWtName = ""; // For MRCP, not used
		// male_MRCP
		if (phantomGender == 0 && phantomAge == 0) BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_AM";
		if (phantomGender == 0 && phantomAge == 1) BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_15M";
		if (phantomGender == 0 && phantomAge == 2) BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_10M";
		if (phantomGender == 0 && phantomAge == 3) BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_05M";
		if (phantomGender == 0 && phantomAge == 4) BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_01M";
		if (phantomGender == 0 && phantomAge == 5) BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_00M";
		// female_MRCP
		if (phantomGender == 1 && phantomAge == 0) BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_AF";
		if (phantomGender == 1 && phantomAge == 1) BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_15F";
		if (phantomGender == 1 && phantomAge == 2) BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_10F";
		if (phantomGender == 1 && phantomAge == 3) BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_05F";
		if (phantomGender == 1 && phantomAge == 4) BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_01F";
		if (phantomGender == 1 && phantomAge == 5) BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_00F";

		return BodySizeInfo_for_ThisPhantom; //MRCP 종료
	}
	if (phantomType == theApp.pRt->E_PHANTOMTYPE_IMPORTED || phantomType == theApp.pRt->E_PHANTOMTYPE_PFMRCP) //Imported and pfMRCP -> no dummy phantom
	{
		BodySizeInfo_for_ThisPhantom.zScale = 1;
		BodySizeInfo_for_ThisPhantom.xyScale = 1;
		BodySizeInfo_for_ThisPhantom.zScale_dummy = 1;
		BodySizeInfo_for_ThisPhantom.xyScale_dummy = 1;

		BodySizeInfo_for_ThisPhantom.HtWtName = "";
		BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "";

		return BodySizeInfo_for_ThisPhantom; // Imported 종료
	}

	// Transformed phantom - standing (body-size only)
	if (phantomGender == 0 && phantomAge == 0 && phantomPosture ==0)
	{
		reference_height = 176.;
		reference_weight = 73.;		
		BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_AM"; // Dummy phantom Name
		HeightVector = AdultMaleAnchor_Height;
		WeightVector = AdultMaleAnchor_Weight;
	}
	if (phantomGender == 0 && phantomAge == 1 && phantomPosture == 0)
	{
		reference_height = 167.;
		reference_weight = 56.;
		BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_15M"; // Dummy phantom Name
		HeightVector = Pediatric_15M_Anchor_Height;
		WeightVector = Pediatric_15M_Anchor_Weight;
	}
	if (phantomGender == 0 && phantomAge == 2 && phantomPosture == 0)
	{
		reference_height = 138.;
		reference_weight = 32.;
		BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_10M"; // Dummy phantom Name
		HeightVector = Pediatric_10M_Anchor_Height;
		WeightVector = Pediatric_10M_Anchor_Weight;
	}
	if (phantomGender == 0 && phantomAge == 3 && phantomPosture == 0)
	{
		reference_height = 109.;
		reference_weight = 19.;
		BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_05M"; // Dummy phantom Name
		HeightVector = Pediatric_05M_Anchor_Height;
		WeightVector = Pediatric_05M_Anchor_Weight;
	}
	if (phantomGender == 0 && phantomAge == 4 && phantomPosture == 0)
	{
		reference_height = 76.;
		reference_weight = 10.;
		BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_01M"; // Dummy phantom Name
		HeightVector = Pediatric_01M_Anchor_Height;
		WeightVector = Pediatric_01M_Anchor_Weight;
	}
	if (phantomGender == 0 && phantomAge == 5 && phantomPosture == 0)
	{
		reference_height = 51.;
		reference_weight = 3.5;
		BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_00M"; // Dummy phantom Name
		HeightVector = Pediatric_00M_Anchor_Height;
		WeightVector = Pediatric_00M_Anchor_Weight;
	}
	if (phantomGender == 1 && phantomAge == 0 && phantomPosture == 0)
	{
		reference_height = 163.;
		reference_weight = 60.;
		BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_AF"; // Dummy phantom Name
		HeightVector = AdultFemaleAnchor_Height;
		WeightVector = AdultFemaleAnchor_Weight;
	}
	if (phantomGender == 1 && phantomAge == 1 && phantomPosture == 0)
	{
		reference_height = 161.;
		reference_weight = 53.;
		BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_15F"; // Dummy phantom Name
		HeightVector = Pediatric_15F_Anchor_Height;
		WeightVector = Pediatric_15F_Anchor_Weight;
	}
	if (phantomGender == 1 && phantomAge == 2 && phantomPosture == 0)
	{
		reference_height = 138.;
		reference_weight = 32.;
		BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_10F"; // Dummy phantom Name
		HeightVector = Pediatric_10F_Anchor_Height;
		WeightVector = Pediatric_10F_Anchor_Weight;
	}
	if (phantomGender == 1 && phantomAge == 3 && phantomPosture == 0)
	{
		reference_height = 109.;
		reference_weight = 19.;
		BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_05F"; // Dummy phantom Name
		HeightVector = Pediatric_05F_Anchor_Height;
		WeightVector = Pediatric_05F_Anchor_Weight;
	}
	if (phantomGender == 1 && phantomAge == 4 && phantomPosture == 0)
	{
		reference_height = 76.;
		reference_weight = 10.;
		BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_01F"; // Dummy phantom Name
		HeightVector = Pediatric_01F_Anchor_Height;
		WeightVector = Pediatric_01F_Anchor_Weight;
	}
	if (phantomGender == 1 && phantomAge == 5 && phantomPosture == 0)
	{
		reference_height = 51.;
		reference_weight = 3.5;
		BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_00F"; // Dummy phantom Name
		HeightVector = Pediatric_00F_Anchor_Height;
		WeightVector = Pediatric_00F_Anchor_Weight;
	}

	// Deformed phantom - other postures (body-size and posture)
	if (phantomGender == 0 && phantomAge == 0 && phantomPosture != 0)
	{
		reference_height = 176.;
		reference_weight = 73.;	
		HeightVector;
		WeightVector;
		if (phantomPosture == 1) BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_AM_bending"; // Dummy phantom Name
		if (phantomPosture == 2) BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_AM_kneeling"; // Dummy phantom Name
		if (phantomPosture == 3) BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_AM_sitting"; // Dummy phantom Name
		if (phantomPosture == 4) BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_AM_squatting"; // Dummy phantom Name
		if (phantomPosture == 5) BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_AM_walking"; // Dummy phantom Name
	}
	if (phantomGender == 1 && phantomAge == 0 && phantomPosture != 0)
	{
		reference_height = 163.;
		reference_weight = 60.;
		HeightVector;
		WeightVector;
		if (phantomPosture == 1) BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_AF_bending"; // Dummy phantom Name
		if (phantomPosture == 2) BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_AF_kneeling"; // Dummy phantom Name
		if (phantomPosture == 3) BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_AF_sitting"; // Dummy phantom Name
		if (phantomPosture == 4) BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_AF_squatting"; // Dummy phantom Name
		if (phantomPosture == 5) BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_AF_walking"; // Dummy phantom Name
	}

	if (phantomPosture != 0) // 임시: 자세변형 팬텀은 아직 체형변형 안되어서 무조건 표준팬텀으로부터 변경
	{
		if (phantomGender == 0)
		{
			if (phantomPosture == 1) BodySizeInfo_for_ThisPhantom.HtWtName = "MRCP_AM_bending"; // Dummy phantom Name
			if (phantomPosture == 2) BodySizeInfo_for_ThisPhantom.HtWtName = "MRCP_AM_kneeling"; // Dummy phantom Name
			if (phantomPosture == 3) BodySizeInfo_for_ThisPhantom.HtWtName = "MRCP_AM_sitting"; // Dummy phantom Name
			if (phantomPosture == 4) BodySizeInfo_for_ThisPhantom.HtWtName = "MRCP_AM_squatting"; // Dummy phantom Name
			if (phantomPosture == 5) BodySizeInfo_for_ThisPhantom.HtWtName = "MRCP_AM_walking"; // Dummy phantom Name
		}
		if (phantomGender == 1)
		{
			// Set Full phantom scale info
			if (phantomPosture == 1) BodySizeInfo_for_ThisPhantom.HtWtName = "MRCP_AF_bending"; // Dummy phantom Name
			if (phantomPosture == 2) BodySizeInfo_for_ThisPhantom.HtWtName = "MRCP_AF_kneeling"; // Dummy phantom Name
			if (phantomPosture == 3) BodySizeInfo_for_ThisPhantom.HtWtName = "MRCP_AF_sitting"; // Dummy phantom Name
			if (phantomPosture == 4) BodySizeInfo_for_ThisPhantom.HtWtName = "MRCP_AF_squatting"; // Dummy phantom Name
			if (phantomPosture == 5) BodySizeInfo_for_ThisPhantom.HtWtName = "MRCP_AF_walking"; // Dummy phantom Name
		}

		BodySizeInfo_for_ThisPhantom.zScale = 1;
		BodySizeInfo_for_ThisPhantom.xyScale = 1;
		BodySizeInfo_for_ThisPhantom.zScale_dummy = 1;
		BodySizeInfo_for_ThisPhantom.xyScale_dummy = 1;

		return BodySizeInfo_for_ThisPhantom; // Posture 임시 종료 -> 현재는 자세-체형 동시변형 불가
	}

	// Calculate height within X% and closest BMI phantom
	double min_height = phantomHeight * 0.0;
	double max_height = phantomHeight * 1000.0;
	std::vector<int> height_within_index;
	for (size_t i = 0; i < HeightVector.size(); ++i) {
		if (HeightVector[i] >= min_height && HeightVector[i] <= max_height) {
			height_within_index.push_back(i);
		}
	}
	std::map<int, double> BMI_within;
	for (auto itr_index : height_within_index)
	{
		BMI_within[itr_index] = (WeightVector[itr_index] / HeightVector[itr_index] / HeightVector[itr_index] * 10000);
	}
	int closest_ID = -1;
	double smallestDifference = DBL_MAX;
	for (const auto& itr : BMI_within)
	{		
		double difference = std::abs(itr.second - phantomBMI);
		if (difference < smallestDifference) 
		{
			smallestDifference = difference;
			closest_ID = itr.first;
		}
	}
	
	// Set Full phantom scale info
	BodySizeInfo_for_ThisPhantom.HtWtName = "H" + QString::number((int)HeightVector[closest_ID]) + "M" + QString::number((int)(WeightVector[closest_ID]*1000));
	BodySizeInfo_for_ThisPhantom.zScale = phantomHeight / HeightVector[closest_ID];
	BodySizeInfo_for_ThisPhantom.xyScale = sqrt(phantomWeight / (WeightVector[closest_ID] * BodySizeInfo_for_ThisPhantom.zScale));

	//Set Dummy phantom scale info except for pre-determined phantom name
	BodySizeInfo_for_ThisPhantom.zScale_dummy = phantomHeight / reference_height;
	BodySizeInfo_for_ThisPhantom.xyScale_dummy = sqrt(phantomWeight / (reference_weight * BodySizeInfo_for_ThisPhantom.zScale_dummy));

	return BodySizeInfo_for_ThisPhantom;
}
std::string PhantomObjects::GetPhantomFileTitle(std::string phantomDir)
{
	std::string delimiter = "/";
	size_t pos = phantomDir.rfind(delimiter);
	phantomDir = phantomDir.substr(pos + 1); // pTitle.XXX만 남기기
	pos = phantomDir.find(".");
	if (pos != std::string::npos)
	{
		phantomDir.erase(pos);
	}

	return phantomDir;
}
QString PhantomObjects::GetPhantomFile_AbsolutePath(int phantomType, int phantomGender, int phantomAge, int phantomPosture, int phantomFetalAge, int phantomIndex, BodySizeInfo BodySizeInfo_for_ThisPhantom)
{
	QString phantomPath;
	
	if (phantomType == theApp.pRt->E_PHANTOMTYPE_IMPORTED) 
	{
		phantomPath = theApp.m_ImportedPhantomFilePath_NoExtention[phantomIndex];

		// Set m_ImportedPhantom_ParentPhantomFileName

		// male_MRCP
		if (phantomGender == 0 && phantomAge == 0) theApp.m_ImportedPhantom_ParentPhantomFileName[phantomIndex] = "MRCP_AM";
		if (phantomGender == 0 && phantomAge == 1) theApp.m_ImportedPhantom_ParentPhantomFileName[phantomIndex] = "MRCP_15M";
		if (phantomGender == 0 && phantomAge == 2) theApp.m_ImportedPhantom_ParentPhantomFileName[phantomIndex] = "MRCP_10M";
		if (phantomGender == 0 && phantomAge == 3) theApp.m_ImportedPhantom_ParentPhantomFileName[phantomIndex] = "MRCP_05M";
		if (phantomGender == 0 && phantomAge == 4) theApp.m_ImportedPhantom_ParentPhantomFileName[phantomIndex] = "MRCP_01M";
		if (phantomGender == 0 && phantomAge == 5) theApp.m_ImportedPhantom_ParentPhantomFileName[phantomIndex] = "MRCP_00M";
		// female_MRCP
		if (phantomGender == 1 && phantomAge == 0) theApp.m_ImportedPhantom_ParentPhantomFileName[phantomIndex] = "MRCP_AF";
		if (phantomGender == 1 && phantomAge == 1) theApp.m_ImportedPhantom_ParentPhantomFileName[phantomIndex] = "MRCP_15F";
		if (phantomGender == 1 && phantomAge == 2) theApp.m_ImportedPhantom_ParentPhantomFileName[phantomIndex] = "MRCP_10F";
		if (phantomGender == 1 && phantomAge == 3) theApp.m_ImportedPhantom_ParentPhantomFileName[phantomIndex] = "MRCP_05F";
		if (phantomGender == 1 && phantomAge == 4) theApp.m_ImportedPhantom_ParentPhantomFileName[phantomIndex] = "MRCP_01F";
		if (phantomGender == 1 && phantomAge == 5) theApp.m_ImportedPhantom_ParentPhantomFileName[phantomIndex] = "MRCP_00F";						
	}
	
	// male_MRCP
	if (phantomType == theApp.pRt->E_PHANTOMTYPE_ADULTMRCP && phantomGender == 0 && phantomAge == 0) phantomPath = "./data/phantom/MRCP/MRCP_AM.obj";
	if (phantomType == theApp.pRt->E_PHANTOMTYPE_PEDIATRICMRCP && phantomGender == 0 && phantomAge == 1) phantomPath = "./data/phantom/MRCP/MRCP_15M.obj";
	if (phantomType == theApp.pRt->E_PHANTOMTYPE_PEDIATRICMRCP && phantomGender == 0 && phantomAge == 2) phantomPath = "./data/phantom/MRCP/MRCP_10M.obj";
	if (phantomType == theApp.pRt->E_PHANTOMTYPE_PEDIATRICMRCP && phantomGender == 0 && phantomAge == 3) phantomPath = "./data/phantom/MRCP/MRCP_05M.obj";
	if (phantomType == theApp.pRt->E_PHANTOMTYPE_PEDIATRICMRCP && phantomGender == 0 && phantomAge == 4) phantomPath = "./data/phantom/MRCP/MRCP_01M.obj";
	if (phantomType == theApp.pRt->E_PHANTOMTYPE_PEDIATRICMRCP && phantomGender == 0 && phantomAge == 5) phantomPath = "./data/phantom/MRCP/MRCP_00M.obj";
	// female_MRCP
	if (phantomType == theApp.pRt->E_PHANTOMTYPE_ADULTMRCP && phantomGender == 1 && phantomAge == 0) phantomPath = "./data/phantom/MRCP/MRCP_AF.obj";
	if (phantomType == theApp.pRt->E_PHANTOMTYPE_PEDIATRICMRCP && phantomGender == 1 && phantomAge == 1) phantomPath = "./data/phantom/MRCP/MRCP_15F.obj";
	if (phantomType == theApp.pRt->E_PHANTOMTYPE_PEDIATRICMRCP && phantomGender == 1 && phantomAge == 2) phantomPath = "./data/phantom/MRCP/MRCP_10F.obj";
	if (phantomType == theApp.pRt->E_PHANTOMTYPE_PEDIATRICMRCP && phantomGender == 1 && phantomAge == 3) phantomPath = "./data/phantom/MRCP/MRCP_05F.obj";
	if (phantomType == theApp.pRt->E_PHANTOMTYPE_PEDIATRICMRCP && phantomGender == 1 && phantomAge == 4) phantomPath = "./data/phantom/MRCP/MRCP_01F.obj";
	if (phantomType == theApp.pRt->E_PHANTOMTYPE_PEDIATRICMRCP && phantomGender == 1 && phantomAge == 5) phantomPath = "./data/phantom/MRCP/MRCP_00F.obj";
	// pf_MRCP
	if (phantomType == theApp.pRt->E_PHANTOMTYPE_PFMRCP && phantomFetalAge == theApp.pRt->E_PHANTOMFETALAGE_8w) phantomPath = "./data/phantom/MRCP/08wM.obj";
	if (phantomType == theApp.pRt->E_PHANTOMTYPE_PFMRCP && phantomFetalAge == theApp.pRt->E_PHANTOMFETALAGE_10w) phantomPath = "./data/phantom/MRCP/10wM.obj";
	if (phantomType == theApp.pRt->E_PHANTOMTYPE_PFMRCP && phantomFetalAge == theApp.pRt->E_PHANTOMFETALAGE_15w) phantomPath = "./data/phantom/MRCP/15wM.obj";
	if (phantomType == theApp.pRt->E_PHANTOMTYPE_PFMRCP && phantomFetalAge == theApp.pRt->E_PHANTOMFETALAGE_20w) phantomPath = "./data/phantom/MRCP/20wM.obj";
	if (phantomType == theApp.pRt->E_PHANTOMTYPE_PFMRCP && phantomFetalAge == theApp.pRt->E_PHANTOMFETALAGE_25w) phantomPath = "./data/phantom/MRCP/25wM.obj";
	if (phantomType == theApp.pRt->E_PHANTOMTYPE_PFMRCP && phantomFetalAge == theApp.pRt->E_PHANTOMFETALAGE_30w) phantomPath = "./data/phantom/MRCP/30wM.obj";
	if (phantomType == theApp.pRt->E_PHANTOMTYPE_PFMRCP && phantomFetalAge == theApp.pRt->E_PHANTOMFETALAGE_35w) phantomPath = "./data/phantom/MRCP/35wM.obj";
	if (phantomType == theApp.pRt->E_PHANTOMTYPE_PFMRCP && phantomFetalAge == theApp.pRt->E_PHANTOMFETALAGE_38w) phantomPath = "./data/phantom/MRCP/38wM.obj";	
	// male_deformed - standing (body-size only)
	if (phantomType == theApp.pRt->E_PHANTOMTYPE_TRANSFORMED && phantomPosture == 0 && phantomGender == 0 && phantomAge == 0) phantomPath = "./data/phantom/bodysize/AM_" + BodySizeInfo_for_ThisPhantom.HtWtName + ".obj";
	if (phantomType == theApp.pRt->E_PHANTOMTYPE_TRANSFORMED && phantomPosture == 0 && phantomGender == 0 && phantomAge == 1) phantomPath = "./data/phantom/bodysize/15M_" + BodySizeInfo_for_ThisPhantom.HtWtName + ".obj";
	if (phantomType == theApp.pRt->E_PHANTOMTYPE_TRANSFORMED && phantomPosture == 0 && phantomGender == 0 && phantomAge == 2) phantomPath = "./data/phantom/bodysize/10M_" + BodySizeInfo_for_ThisPhantom.HtWtName + ".obj";
	if (phantomType == theApp.pRt->E_PHANTOMTYPE_TRANSFORMED && phantomPosture == 0 && phantomGender == 0 && phantomAge == 3) phantomPath = "./data/phantom/bodysize/05M_" + BodySizeInfo_for_ThisPhantom.HtWtName + ".obj";
	if (phantomType == theApp.pRt->E_PHANTOMTYPE_TRANSFORMED && phantomPosture == 0 && phantomGender == 0 && phantomAge == 4) phantomPath = "./data/phantom/bodysize/01M_" + BodySizeInfo_for_ThisPhantom.HtWtName + ".obj";
	if (phantomType == theApp.pRt->E_PHANTOMTYPE_TRANSFORMED && phantomPosture == 0 && phantomGender == 0 && phantomAge == 5) phantomPath = "./data/phantom/bodysize/00M_" + BodySizeInfo_for_ThisPhantom.HtWtName + ".obj";
	// female_deformed - standing (body-size only)
	if (phantomType == theApp.pRt->E_PHANTOMTYPE_TRANSFORMED && phantomPosture == 0 && phantomGender == 1 && phantomAge == 0) phantomPath = "./data/phantom/bodysize/AF_" + BodySizeInfo_for_ThisPhantom.HtWtName + ".obj";
	if (phantomType == theApp.pRt->E_PHANTOMTYPE_TRANSFORMED && phantomPosture == 0 && phantomGender == 1 && phantomAge == 1) phantomPath = "./data/phantom/bodysize/15F_" + BodySizeInfo_for_ThisPhantom.HtWtName + ".obj";
	if (phantomType == theApp.pRt->E_PHANTOMTYPE_TRANSFORMED && phantomPosture == 0 && phantomGender == 1 && phantomAge == 2) phantomPath = "./data/phantom/bodysize/10F_" + BodySizeInfo_for_ThisPhantom.HtWtName + ".obj";
	if (phantomType == theApp.pRt->E_PHANTOMTYPE_TRANSFORMED && phantomPosture == 0 && phantomGender == 1 && phantomAge == 3) phantomPath = "./data/phantom/bodysize/05F_" + BodySizeInfo_for_ThisPhantom.HtWtName + ".obj";
	if (phantomType == theApp.pRt->E_PHANTOMTYPE_TRANSFORMED && phantomPosture == 0 && phantomGender == 1 && phantomAge == 4) phantomPath = "./data/phantom/bodysize/01F_" + BodySizeInfo_for_ThisPhantom.HtWtName + ".obj";
	if (phantomType == theApp.pRt->E_PHANTOMTYPE_TRANSFORMED && phantomPosture == 0 && phantomGender == 1 && phantomAge == 5) phantomPath = "./data/phantom/bodysize/00F_" + BodySizeInfo_for_ThisPhantom.HtWtName + ".obj";
		
	// male_deformed - other posture (posture/body-size) - 지금은 body-size X
	if (phantomType == theApp.pRt->E_PHANTOMTYPE_TRANSFORMED && phantomGender == 0 && phantomPosture == 1) phantomPath = "./data/phantom/posture/" + BodySizeInfo_for_ThisPhantom.HtWtName + ".obj";
	if (phantomType == theApp.pRt->E_PHANTOMTYPE_TRANSFORMED && phantomGender == 0 && phantomPosture == 2) phantomPath = "./data/phantom/posture/" + BodySizeInfo_for_ThisPhantom.HtWtName + ".obj";
	if (phantomType == theApp.pRt->E_PHANTOMTYPE_TRANSFORMED && phantomGender == 0 && phantomPosture == 3) phantomPath = "./data/phantom/posture/" + BodySizeInfo_for_ThisPhantom.HtWtName + ".obj";
	if (phantomType == theApp.pRt->E_PHANTOMTYPE_TRANSFORMED && phantomGender == 0 && phantomPosture == 4) phantomPath = "./data/phantom/posture/" + BodySizeInfo_for_ThisPhantom.HtWtName + ".obj";
	if (phantomType == theApp.pRt->E_PHANTOMTYPE_TRANSFORMED && phantomGender == 0 && phantomPosture == 5) phantomPath = "./data/phantom/posture/" + BodySizeInfo_for_ThisPhantom.HtWtName + ".obj";
	// female_deformed - other posture (posture/body-size) - 지금은 body-size X
	if (phantomType == theApp.pRt->E_PHANTOMTYPE_TRANSFORMED && phantomGender == 1 && phantomPosture == 1) phantomPath = "./data/phantom/posture/" + BodySizeInfo_for_ThisPhantom.HtWtName + ".obj";
	if (phantomType == theApp.pRt->E_PHANTOMTYPE_TRANSFORMED && phantomGender == 1 && phantomPosture == 2) phantomPath = "./data/phantom/posture/" + BodySizeInfo_for_ThisPhantom.HtWtName + ".obj";
	if (phantomType == theApp.pRt->E_PHANTOMTYPE_TRANSFORMED && phantomGender == 1 && phantomPosture == 3) phantomPath = "./data/phantom/posture/" + BodySizeInfo_for_ThisPhantom.HtWtName + ".obj";
	if (phantomType == theApp.pRt->E_PHANTOMTYPE_TRANSFORMED && phantomGender == 1 && phantomPosture == 4) phantomPath = "./data/phantom/posture/" + BodySizeInfo_for_ThisPhantom.HtWtName + ".obj";
	if (phantomType == theApp.pRt->E_PHANTOMTYPE_TRANSFORMED && phantomGender == 1 && phantomPosture == 5) phantomPath = "./data/phantom/posture/" + BodySizeInfo_for_ThisPhantom.HtWtName + ".obj";

	return phantomPath;
}
std::string PhantomObjects::ExtractPhantomOBJ(std::string PATH, std::string Name, std::vector<std::string> list)
{
	std::string input = PATH + ".obj";
	std::string output = PATH + "_o.obj"; // 바깥에서 참조함

	QFile* inFile = new QFile(QString::fromStdString(input));
	QIODeviceIStream in(inFile);
		
	std::string temp_str;
	int temp_int;
	double temp_double;
	char temp_char[256];
	char* remain = NULL;

	std::map<int, std::vector<std::tuple<double, double, double>>> vertex_vec;
	std::map<int, std::vector<std::tuple<int, int, int>>> face_vec;
	int num = 0;

	int OrganID;
	std::vector<double> OrganID_vec;
	std::vector<std::string> OrganName_vec;

	std::vector<std::pair<std::string, std::pair<double, std::vector<std::tuple<double, double, double>>>>> vertex;
	std::vector<std::pair<std::string, std::pair<double, std::vector<std::tuple<int, int, int>>>>> face;

	while (!in.eof())
	{
		in >> temp_str;
		if (temp_str.size() == 1 && temp_str == "v")
		{
			double vertex[3] = { 0,0,0 };
			for (int i = 0; i < 3; i++)
			{
				in >> temp_double;
				vertex[i] = temp_double;
			}
			vertex_vec[num].push_back(std::make_tuple(vertex[0], vertex[1], vertex[2]));
		}

		if (temp_str == "g")
		{
			in >> temp_char;
			char* newe = strtok_s(temp_char, "_", &remain);
			OrganID = atoi(newe);
			OrganID_vec.push_back(OrganID);

		}
		if (temp_str == "usemtl")
		{
			in >> temp_str;
			//cout << temp_str << endl;
			OrganName_vec.push_back(temp_str);
		}

		if (temp_str.size() == 1 && temp_str == "f")
		{
			int facet[3] = { 0,0,0 };
			for (int i = 0; i < 3; i++)
			{
				in >> temp_int;
				facet[i] = temp_int; 
			}
			face_vec[num].push_back(std::make_tuple(facet[0], facet[1], facet[2]));
		}

		num = OrganName_vec.size();
	}
	inFile->close();


	for (int i = 0; i < OrganName_vec.size(); i++)
	{
		bool skip = true;
		for (auto itr : list)
		{
			if (OrganName_vec[i] == itr) skip = false;
		}
		if (!skip)
		{
			vertex.push_back(make_pair(OrganName_vec[i], make_pair(OrganID_vec[i], vertex_vec[i])));
			face.push_back(make_pair(OrganName_vec[i], make_pair(OrganID_vec[i], face_vec[i + 1])));
		}
	}


	std::vector<int> x;
	std::vector<int> y;
	std::vector<int> z;
	std::vector<int> min_face;

	int min_find[3];

	for (int i = 0; i < face.size(); i++) {
		for (int j = 0; j < face[i].second.second.size(); j++) {
			x.push_back(get<0>(face[i].second.second[j]));
			y.push_back(get<1>(face[i].second.second[j]));
			z.push_back(get<2>(face[i].second.second[j]));
		}

		min_find[0] = *min_element(x.begin(), x.end());
		min_find[1] = *min_element(y.begin(), y.end());
		min_find[2] = *min_element(z.begin(), z.end());

		int min = min_find[0];
		for (int j = 0; j < 3; j++) {
			if (min_find[j] < min) min = min_find[j];
		}

		min_face.push_back(min);

		x.clear();
		y.clear();
		z.clear();
	}

	std::pair<std::string, std::pair<double, std::vector<std::tuple<double, double, double>>>> vertex_new;
	std::pair<std::string, std::pair<double, std::vector<std::tuple<int, int, int>>>> face_new;

	for (int i = 0; i < face.size(); i++) {
		for (int j = 0; j < face[i].second.second.size(); j++) {
			get<0>(face[i].second.second[j]) = get<0>(face[i].second.second[j]) - min_face[i] + 1;
			get<1>(face[i].second.second[j]) = get<1>(face[i].second.second[j]) - min_face[i] + 1;
			get<2>(face[i].second.second[j]) = get<2>(face[i].second.second[j]) - min_face[i] + 1;
		}
	}

	for (int i = 0; i < vertex.size() - 1; i++) {
		for (int j = i + 1; j < vertex.size(); j++) {
			if (vertex[j].second.first <= vertex[i].second.first) {
				vertex_new = vertex[j];
				vertex[j] = vertex[i];
				vertex[i] = vertex_new;
			}
		}
		for (int j = i + 1; j < face.size(); j++) {
			if (face[j].second.first <= face[i].second.first) {
				face_new = face[j];
				face[j] = face[i];
				face[i] = face_new;
			}
		}
	}

	std::vector<int> size_vertex;
	int vertex_s = 0;

	for (int i = 0; i < face.size(); i++) {
		size_vertex.push_back(vertex_s);
		vertex_s += vertex[i].second.second.size();
	}
	for (int i = 0; i < face.size(); i++) {
		for (int j = 0; j < face[i].second.second.size(); j++) {
			get<0>(face[i].second.second[j]) = get<0>(face[i].second.second[j]) + size_vertex[i];
			get<1>(face[i].second.second[j]) = get<1>(face[i].second.second[j]) + size_vertex[i];
			get<2>(face[i].second.second[j]) = get<2>(face[i].second.second[j]) + size_vertex[i];
		}
	}
	
	QFile* outFile = new QFile(QString::fromStdString(output));
	QIODeviceOStream result(outFile);

	result << "#Obj File : Exported From RapidForm (INUS Technology Co. Ltd.)" << endl << endl;
	result << "mtllib " + Name + ".mtl" << endl << endl;

	for (int i = 0; i < vertex.size(); i++) {
		for (int j = 0; j < vertex[i].second.second.size(); j++) {
			result << "v " << get<0>(vertex[i].second.second[j]) << " " << get<1>(vertex[i].second.second[j]) << " " << get<2>(vertex[i].second.second[j]) << endl;
		}

		result << endl;
		result << "g " << vertex[i].first << endl << "usemtl " << vertex[i].first << endl << "s" << endl << endl;

		if (vertex[i].first == OrganName_vec[vertex.size() - 1]) {
			for (int j = 0; j < face[i].second.second.size() - 1; j++) {
				result << "f " << get<0>(face[i].second.second[j]) << " " << get<1>(face[i].second.second[j]) << " " << get<2>(face[i].second.second[j]) << endl;
			}
		}
		else {
			for (int j = 0; j < face[i].second.second.size(); j++) {
				result << "f " << get<0>(face[i].second.second[j]) << " " << get<1>(face[i].second.second[j]) << " " << get<2>(face[i].second.second[j]) << endl;
			}
		}

		result << endl;

	}
	outFile->close();

	return output;
}

void PhantomObjects::UpdatePhantom_InfoStatus_InActorMouseControl(int phantomIndex) // 마우스로 움직인 팬텀 현재 위치 및 회전으로 패널값(line) 업데이트
{		
	double* pCenter = theApp.PhantomPanelActor[phantomIndex]->GetPosition();
	double TransformPos[3];
	TransformPos[0] = pCenter[0];
	TransformPos[1] = pCenter[1];
	TransformPos[2] = pCenter[2];
	QString StringTranslatedPosX = theApp.getQStringNumberInSpecificDigit(TransformPos[0], 6);
	QString StringTranslatedPosY = theApp.getQStringNumberInSpecificDigit(TransformPos[1], 6);
	QString StringTranslatedPosZ = theApp.getQStringNumberInSpecificDigit(TransformPos[2], 6);

	double* pRotation = theApp.PhantomPanelActor[phantomIndex]->GetOrientation();
	double TransformRot[3];
	TransformRot[0] = pRotation[0];
	TransformRot[1] = pRotation[1];
	TransformRot[2] = pRotation[2];
	QString StringRotX = theApp.getQStringNumberInSpecificDigit(TransformRot[0], 6);
	QString StringRotY = theApp.getQStringNumberInSpecificDigit(TransformRot[1], 6);
	QString StringRotZ = theApp.getQStringNumberInSpecificDigit(TransformRot[2], 6);

	theApp.pRt->PhantomPosX_QLineEdit->setText(StringTranslatedPosX);
	theApp.pRt->PhantomPosY_QLineEdit->setText(StringTranslatedPosY);
	theApp.pRt->PhantomPosZ_QLineEdit->setText(StringTranslatedPosZ);

	theApp.pRt->PhantomRotX_QLineEdit->setText(StringRotX);
	theApp.pRt->PhantomRotY_QLineEdit->setText(StringRotY);
	theApp.pRt->PhantomRotZ_QLineEdit->setText(StringRotZ);
}
void PhantomObjects::UpdatePhantom_ActorHighlighted(int phantomIndex) // 현재 선택된 팬텀버튼에 해당하는 팬텀만 pickable 하도록
{
	for (auto itr_phanntom : theApp.pRt->m_Phantom_SequenceVector)
	{
		if (itr_phanntom == phantomIndex)
		{
			theApp.PhantomPanelActor[itr_phanntom]->GetProperty()->SetDiffuse(1.0);
		}
		else
		{
			theApp.PhantomPanelActor[itr_phanntom]->GetProperty()->SetDiffuse(0.6);
		}
	}
	//theApp.m_pVTKWidget->renderWindow()->Render();
	theApp.m_pVTKWidget->renderWindow()->Render();
}

////////////////// Phantom Clothing ////////////////// 
void PhantomObjects::PhantomClothingGenerate(QString strFileName, bool IsPreDefinedClothing) // called by (1) slot_ClothingAddingOK_ButtonClicked, (2) RubberBandUserClothingGenerate, (3) LoadReconsturctionFile_previous
{
	int phantomIdx = theApp.pRt->m_Phantom_SelectedIndex;
	int clothingIdx = theApp.pRt->m_Clothing_MakingIndex; 	

	if (strFileName == "") return; //Imported 팬텀이어서 없으면 빠져나가기	
	
	vtkSmartPointer<vtkOBJReader> reader = vtkSmartPointer<vtkOBJReader>::New();
	reader->SetFileName(Util::Wcs_to_mbs(strFileName.toStdWString()).c_str());
	reader->Update();
	vtkSmartPointer<vtkPolyData> clothingPolydata_forTetra = vtkSmartPointer<vtkPolyData>::New();
	clothingPolydata_forTetra = Util::CreatePolyData(reader);
	vtkSmartPointer<vtkPolyData> clothingPolydata = vtkSmartPointer<vtkPolyData>::New();
	clothingPolydata = Util::CreatePolyData(reader);
	
	// 사면체화 시 이용할 polydata 저장(pCenter 0,0,0 안 맞추고 체형 스케일링만 된 상태 -> 추후 Geant4에서 의복의 스케일링은 진행되지 않도록 해야함)
	vtkSmartPointer<vtkTransform> transform_forTetra = vtkSmartPointer<vtkTransform>::New();
	transform_forTetra->PostMultiply(); //this is the key line	
	if (IsPreDefinedClothing == true) transform_forTetra->Scale(theApp.PhantomPolyDataScaleFactor[phantomIdx][0], theApp.PhantomPolyDataScaleFactor[phantomIdx][0], theApp.PhantomPolyDataScaleFactor[phantomIdx][1]); // 체형 스케일링
	vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter_forTetra = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	transformFilter_forTetra->SetInputData(clothingPolydata_forTetra);
	transformFilter_forTetra->SetTransform(transform_forTetra);
	transformFilter_forTetra->Update();
	theApp.OriginalClothingPolydata_forTetra[phantomIdx][clothingIdx] = transformFilter_forTetra->GetOutput(); // 이후 dosimeter 겹침 체크하기 위해 polydata로 저장
	vtkSmartPointer<vtkOBJWriter> writer_ClothingOBJ = vtkSmartPointer<vtkOBJWriter>::New();
	std::string ClothingOBJStr = "./data/wearable/Clothing_forTetra/" + std::to_string(phantomIdx) + "_" + std::to_string(clothingIdx) + ".obj"; // 사면체화에 사용할 OBJ 파일 생성
	writer_ClothingOBJ->SetFileName(ClothingOBJStr.c_str());
	writer_ClothingOBJ->SetInputData(theApp.OriginalClothingPolydata_forTetra[phantomIdx][clothingIdx]);
	writer_ClothingOBJ->Update();

	// 가시화용 의복 polydata pCenter 0,0,0 맞추고 체형스케일링
	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
	transform->PostMultiply(); //this is the key line
	transform->Translate(-theApp.PhantomOrigianlPolyDataCenter[phantomIdx][0], -theApp.PhantomOrigianlPolyDataCenter[phantomIdx][1], -theApp.PhantomOrigianlPolyDataCenter[phantomIdx][2]);
	if (IsPreDefinedClothing == true) transform->Scale(theApp.PhantomPolyDataScaleFactor[phantomIdx][0], theApp.PhantomPolyDataScaleFactor[phantomIdx][0], theApp.PhantomPolyDataScaleFactor[phantomIdx][1]);
	vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	transformFilter->SetInputData(clothingPolydata);
	transformFilter->SetTransform(transform);
	transformFilter->Update();
	clothingPolydata = transformFilter->GetOutput();
	vtkSmartPointer<vtkPolyDataNormals> normalsFilter = vtkSmartPointer<vtkPolyDataNormals>::New();
	normalsFilter->SetInputData(clothingPolydata);
	normalsFilter->ComputePointNormalsOn();
	normalsFilter->ComputeCellNormalsOff();
	normalsFilter->Update();
	vtkDataArray* normalData = normalsFilter->GetOutput()->GetPointData()->GetNormals();
	double offsetDistance;
	offsetDistance = 0.0005; // 가시화를 위해 offset 5 um from phantom skin (dosimeter is 10 um offset)
	for (vtkIdType i = 0; i < clothingPolydata->GetNumberOfPoints(); i++)
	{
		double p[3];
		double n[3];
		clothingPolydata->GetPoint(i, p);
		normalData->GetTuple(i, n);
		p[0] = p[0] + n[0] * offsetDistance;
		p[1] = p[1] + n[1] * offsetDistance;
		p[2] = p[2] + n[2] * offsetDistance;
		clothingPolydata->GetPoints()->SetPoint(i, p);
	}
	clothingPolydata->Modified();

	float SelectedColor[3];
	float ClothesColor[3] = { 240, 228, 140 }; // Wearable color
	SelectedColor[0] = ClothesColor[0]; SelectedColor[1] = ClothesColor[1]; SelectedColor[2] = ClothesColor[2];
	vtkNew<vtkUnsignedCharArray> WearableColors;
	WearableColors->SetNumberOfComponents(3);
	WearableColors->SetNumberOfTuples(clothingPolydata->GetNumberOfPoints());
	for (int i = 0; i < clothingPolydata->GetNumberOfPoints(); ++i)
	{
		WearableColors->SetTuple(i, SelectedColor);
	}
	clothingPolydata->GetPointData()->SetScalars(WearableColors);
	theApp.PhantomPanelPolyData_Collection[phantomIdx][1][clothingIdx] = clothingPolydata;

	// Appending with phantom
	AppendPhantomPolyData_GeneratePhantomActor(phantomIdx);

	// Save clothing information
	theApp.pRt->SaveClothingInformation_InClothingGenerate();

	// Set clothing panel
	theApp.pRt->SetClothingPanelInfo(clothingIdx, 0); // Generate 중에는 0번 layer index를 생성
	theApp.pRt->InitializeClothingPanel_InClothingAdd();
}
void PhantomObjects::RubberBandInitialization() // called by (1) slot_ClothingAddingOK_ButtonClicked
{
	// Generate areaPickerand set to interactor
	vtkSmartPointer<vtkAreaPicker> areaPicker = vtkSmartPointer<vtkAreaPicker>::New();
	theApp.m_pVTKWidget->renderWindow()->GetInteractor()->SetPicker(areaPicker);

	const double PI = vtkMath::Pi() / 180; // deg to rad 변환용 PI
	int PhantomIdx = theApp.pRt->m_Phantom_SelectedIndex;

	// Origianl polydata 생성 
	vtkSmartPointer<vtkPolyData> PhantomPolydataOriginal;
	PhantomPolydataOriginal = theApp.PhantomPolyData_original[PhantomIdx];

	// Move original polydata to center (0,0,0)
	vtkSmartPointer<vtkTransform> transform_p = vtkSmartPointer<vtkTransform>::New();
	transform_p->PostMultiply(); //this is the key line
	transform_p->Translate(-theApp.PhantomOrigianlPolyDataCenter[PhantomIdx][0], -theApp.PhantomOrigianlPolyDataCenter[PhantomIdx][1], -theApp.PhantomOrigianlPolyDataCenter[PhantomIdx][2]);
	vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter_p = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	transformFilter_p->SetInputData(PhantomPolydataOriginal);
	transformFilter_p->SetTransform(transform_p);
	transformFilter_p->Update();
	PhantomPolydataOriginal = transformFilter_p->GetOutput(); 

	// 원점이동된 polydata를 위치/회전/스케일링 적용하여 polydata 생성
	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
	transform->PostMultiply(); //this is the key line
	double xy_scale = theApp.PhantomPolyDataScaleFactor[PhantomIdx][0];
	double z_scale = theApp.PhantomPolyDataScaleFactor[PhantomIdx][1];

	double PosX = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][6];
	double PosY = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][7];
	double PosZ = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][8];
	double RotX = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][9];
	double RotY = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][10];
	double RotZ = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][11];

	Eigen::Matrix4f X;
	Eigen::Matrix4f Y;
	Eigen::Matrix4f Z;
	Eigen::Matrix4f Scale;

	double RadianX = RotX * PI;
	double RadianY = RotY * PI;
	double RadianZ = RotZ * PI;

	X << 1, 0, 0, 0,
		0, cos(RadianX), -sin(RadianX), 0,
		0, sin(RadianX), cos(RadianX), 0,
		0, 0, 0, 1;

	Y << cos(RadianY), 0, sin(RadianY), 0,
		0, 1, 0, 0,
		-sin(RadianY), 0, cos(RadianY), 0,
		0, 0, 0, 1;

	Z << cos(RadianZ), -sin(RadianZ), 0, 0,
		sin(RadianZ), cos(RadianZ), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1;

	Scale << xy_scale, 0, 0, 0,
		0, xy_scale, 0, 0,
		0, 0, z_scale, 0,
		0, 0, 0, 1;

	Eigen::Matrix4f Result = Z * X * Y * Scale; // Rotate Z->X->Y and scaling

	double elementsResult[16];
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			elementsResult[i * 4 + j] = Result(i, j);
		}
	}
	transform->SetMatrix(elementsResult);	
	transform->Translate(PosX, PosY, PosZ);

	vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	transformFilter->SetInputData(PhantomPolydataOriginal);
	transformFilter->SetTransform(transform);
	transformFilter->Update();
	vtkSmartPointer<vtkPolyData> TransformedRotatedBasePolydata = vtkSmartPointer<vtkPolyData>::New();
	TransformedRotatedBasePolydata = transformFilter->GetOutput();

	//누적(Accumulated) 팬텀 기반으로 Transformed 팬텀 제작-> kdTree point 추출용
	vtkSmartPointer<vtkTransform> transform_AC = vtkSmartPointer<vtkTransform>::New();
	transform_AC->PostMultiply(); //this is the key line
	double xy_scale_AC = 1; // 체형변형 팬텀 또한 이미 scale 되어 있으므로 스케일링은 형식상 존재
	double z_scale_AC = 1;

	double PosX_AC = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][6];
	double PosY_AC = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][7];
	double PosZ_AC = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][8];
	double RotX_AC = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][9];
	double RotY_AC = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][10];
	double RotZ_AC = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][11];

	Eigen::Matrix4f X_AC;
	Eigen::Matrix4f Y_AC;
	Eigen::Matrix4f Z_AC;
	Eigen::Matrix4f Scale_AC;

	double RadianX_AC = RotX_AC * PI;
	double RadianY_AC = RotY_AC * PI;
	double RadianZ_AC = RotZ_AC * PI;

	X_AC << 1, 0, 0, 0,
		0, cos(RadianX_AC), -sin(RadianX_AC), 0,
		0, sin(RadianX_AC), cos(RadianX_AC), 0,
		0, 0, 0, 1;

	Y_AC << cos(RadianY_AC), 0, sin(RadianY_AC), 0,
		0, 1, 0, 0,
		-sin(RadianY_AC), 0, cos(RadianY_AC), 0,
		0, 0, 0, 1;

	Z_AC << cos(RadianZ_AC), -sin(RadianZ_AC), 0, 0,
		sin(RadianZ_AC), cos(RadianZ_AC), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1;

	Scale_AC << xy_scale_AC, 0, 0, 0,
		0, xy_scale_AC, 0, 0,
		0, 0, z_scale_AC, 0,
		0, 0, 0, 1;

	Eigen::Matrix4f Result_AC = Z_AC * X_AC * Y_AC * Scale_AC; // Rotate Z->X->Y and scaling

	double elementsResult_AC[16];
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			elementsResult_AC[i * 4 + j] = Result_AC(i, j);
		}
	}
	transform_AC->SetMatrix(elementsResult_AC);
	transform_AC->Translate(PosX_AC, PosY_AC, PosZ_AC);

	vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter_AC = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	transformFilter_AC->SetInputData(theApp.PhantomPanelAccumulatedPolyData[PhantomIdx]); // 그냥 polydata(의복, 선량계, 안경까지 모두 누적된 polydata)
	transformFilter_AC->SetTransform(transform_AC);
	transformFilter_AC->Update();
	vtkSmartPointer<vtkPolyData> TransformedAccumulatedPolydata = vtkSmartPointer<vtkPolyData>::New();
	TransformedAccumulatedPolydata = transformFilter_AC->GetOutput();

	theApp.m_pVTKWidget->GetBaseInteractorStyleRubberBand()->SetPolyData(TransformedRotatedBasePolydata, TransformedAccumulatedPolydata);
	theApp.m_pVTKWidget->renderWindow()->GetInteractor()->SetInteractorStyle(theApp.m_pVTKWidget->GetBaseInteractorStyleRubberBand()); // ETInteractorStyleRubberBand를 실행함
	theApp.m_pVTKWidget->GetBaseInteractorStyleRubberBand()->CallOnLeftButton(); // 최초 wheel button 움직이기 위해 한 번 풀어주기
}
void PhantomObjects::RubberBandUserClothingGenerate() // called by (1) ETInteractorStyleRubberBand::Action4Function_Rubberband
{
	if (theApp.RubberBandSelectedPointIDs.size() == 0) // 선택한 게 하나도 없을 때
	{
		theApp.SetMessageBox("No region is selected");

		theApp.RubberBandSelectedPointIDs.clear();
		theApp.MouseControl_toCamera();
		theApp.m_pVTKWidget->renderWindow()->GetInteractor()->SetInteractorStyle(theApp.m_pVTKWidget->GetBaseInteractorStyle()); // 다 종료 후 cameraview모드로 돌아가기
		return;
	}	
	int phantomIdx = theApp.pRt->m_Phantom_SelectedIndex;
	int clothingIdx = theApp.pRt->m_Clothing_MakingIndex;

	// 의복까지 누적된 팬텀에 해당하는 pointID에 일치하는 point와 일치하는 cell data 얻어서 polydata 제작
	vtkSmartPointer<vtkPoints> originalPoints = theApp.PhantomPanelAccumulatedPolyData[phantomIdx]->GetPoints(); // 누적 팬텀에서 points 데이터 획득

	vtkSmartPointer<vtkCellArray> SelectedCells = vtkSmartPointer<vtkCellArray>::New();// vtkCellArray = {1,2,3,  2,5,8,  3,8,9, ...} -> 1,2,3 pointIds로 하나의 cell(tri)를 만듦
	
	float SkinColor[3] = { 255, 192, 160 };
	vtkNew<vtkUnsignedCharArray> Colors; // RubberBand 포함된 point 다시 skincolor로
	Colors->DeepCopy(theApp.PhantomPanelAccumulatedPolyData[phantomIdx]->GetPointData()->GetScalars());

	for (vtkIdType i = 0; i < originalPoints->GetNumberOfPoints(); i++)
	{
		auto iter = theApp.RubberBandSelectedPointIDs.find(i);
		if (iter != theApp.RubberBandSelectedPointIDs.end()) // RubberBand 포함된 pointID이면
		{
			Colors->SetTuple(i, SkinColor); // Selection된 부분 다시 skin color로
			vtkSmartPointer<vtkIdList> cellIds = vtkSmartPointer<vtkIdList>::New();
			theApp.PhantomPanelAccumulatedPolyData[phantomIdx]->GetPointCells(i, cellIds); // cellIds에 dosim_idx번째 point가 포함된 cell id 배열을 반환 -> 즉 dosim_idx가 2라면 {1,2 ...}을 반환 (1번, 2번 cell에는 ID:2인 point가 포함됨)
			for (vtkIdType j = 0; j < cellIds->GetNumberOfIds(); j++) // 해당 cell id 개수만큼 반복
			{
				vtkSmartPointer<vtkIdList> cellPointIds = vtkSmartPointer<vtkIdList>::New();
				theApp.PhantomPanelAccumulatedPolyData[phantomIdx]->GetCellPoints(cellIds->GetId(j), cellPointIds); // 해당 cell id에 해당하는 pointIds를 반환 -> 즉 cell id=2 라면 {2,5,8} 반환
				SelectedCells->InsertNextCell(cellPointIds); // point ID 2,5,8을 하나의 cell이 되도록 1차원 배열로 계속 insert
			}
		}
	}
	theApp.PhantomPanelAccumulatedPolyData[phantomIdx]->GetPointData()->SetScalars(Colors); // Selection된 부분 다시 skin color로 초기화
	vtkSmartPointer<vtkPolyData> SelectedPolyData = vtkSmartPointer<vtkPolyData>::New();
	SelectedPolyData->SetPoints(originalPoints); //Original point(누적 polydata의 node)
	SelectedPolyData->SetPolys(SelectedCells); // 누적 polydata에서 selected된 cell

	//polydata를 0.001 cm offset 진행 후 OBJ로 뽑기(base layer, 이후 thickness에 맞춰서 offset 후 추가 layer들 정의 되고 이를 기반으로 사면체화진행)
	vtkNew<vtkPolyDataConnectivityFilter> connectivityFilter;
	connectivityFilter->SetInputData(SelectedPolyData);
	connectivityFilter->SetExtractionModeToAllRegions(); // 이걸 해야 VTK 가시화창에서 의복색상이 깔끔하게 나옴
	connectivityFilter->Update();

	vtkSmartPointer<vtkPolyData> TempSelectedPolydata = vtkSmartPointer<vtkPolyData>::New();
	TempSelectedPolydata = connectivityFilter->GetOutput();

	//offset
	vtkSmartPointer<vtkPolyDataNormals> normalsFilter = vtkSmartPointer<vtkPolyDataNormals>::New();
	normalsFilter->SetInputData(TempSelectedPolydata);
	normalsFilter->ComputePointNormalsOn();
	normalsFilter->ComputeCellNormalsOff();
	normalsFilter->Update();
	vtkDataArray* normalData = normalsFilter->GetOutput()->GetPointData()->GetNormals();
	for (vtkIdType i = 0; i < TempSelectedPolydata->GetNumberOfPoints(); i++)
	{
		double p[3];
		double n[3];
		TempSelectedPolydata->GetPoint(i, p);
		normalData->GetTuple(i, n);
		p[0] = p[0] + n[0] * 0.001; // 10 um(0.001 cm) margin
		p[1] = p[1] + n[1] * 0.001;
		p[2] = p[2] + n[2] * 0.001;
		TempSelectedPolydata->GetPoints()->SetPoint(i, p);
	}
	TempSelectedPolydata->Modified();

	// pCenter를 0,0,0 맞추지 않은 것으로 되돌리기(pre-defined 의복의 원본 obj파일과 같이 맞춰주기 위함)
	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
	transform->PostMultiply(); //this is the key line
	transform->Translate(theApp.PhantomOrigianlPolyDataCenter[phantomIdx][0], theApp.PhantomOrigianlPolyDataCenter[phantomIdx][1], theApp.PhantomOrigianlPolyDataCenter[phantomIdx][2]); // Center 0,0,0 맞추기
	vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	transformFilter->SetInputData(TempSelectedPolydata);
	transformFilter->SetTransform(transform);
	transformFilter->Update();
	TempSelectedPolydata = transformFilter->GetOutput();

	// 사용자제작 의복 obj 파일 경로
	std::string UserDefinedClothingFileName = "./data/wearable/Clothing_UserDefined/" + std::to_string(phantomIdx) + "_" + std::to_string(clothingIdx) + ".obj";

	//Cleaning polydata and Generate OBJ file
	theApp.CleaningPolyData(TempSelectedPolydata, UserDefinedClothingFileName);

	PhantomClothingGenerate(QString::fromStdString(UserDefinedClothingFileName), false);

	theApp.RubberBandSelectedPointIDs.clear();
	theApp.MouseControl_toCamera();
	theApp.m_pVTKWidget->renderWindow()->GetInteractor()->SetInteractorStyle(theApp.m_pVTKWidget->GetBaseInteractorStyle()); // 다 종료 후 cameraview모드로 돌아가기
	theApp.m_pVTKWidget->GetBaseInteractorStyleRubberBand()->b_IsClothingMakingDraggingOn = false;
}

////////////////// Phantom Eyewear ////////////////// 
void PhantomObjects::PhantomFlatGlassesGenerate() //
{
	int PhantomIdx = theApp.pRt->m_Phantom_SelectedIndex;
	theApp.pRt->m_Clothing_MainInfo[PhantomIdx][100][0][0] = theApp.pRt->EyewearDistance_QLineEdit->text().toDouble(); // distance
	theApp.pRt->m_Clothing_MainInfo[PhantomIdx][100][0][1] = theApp.pRt->EyewearRadius_QLineEdit->text().toDouble(); // radius
	theApp.pRt->m_Clothing_MainInfo[PhantomIdx][100][0][2] = theApp.pRt->EyewearThickness_QLineEdit->text().toDouble(); // radius
	theApp.pRt->m_Clothing_MainInfo[PhantomIdx][100][0][3] = theApp.pRt->EyewearDensity_QLineEdit->text().toDouble(); // density
	// composition(1: Glasses, 2: Leaded glasses)
	theApp.pRt->m_Clothing_MainInfo[PhantomIdx][100][0][4] = theApp.pRt->EyewearComposition_QComboBox->currentIndex();

	double centerX, centerY, centerZ, nx, ny, nz, pointX, pointY, pointZ;
	int pointID;	

	vtkSmartPointer<vtkPolyData> PhantomPolydataBase; // 원점이동 및 스케일링만 (이동 및 회전은 X)
	PhantomPolydataBase = theApp.m_3DHumanData_MultiplePhantom[PhantomIdx].polydata_base;

	for (int i = 1; i <= 2; i++)
	{
		vtkSmartPointer<vtkCylinderSource> cylinderSource = vtkSmartPointer<vtkCylinderSource>::New();
		if (i == 1)
		{
		
			pointID = theApp.m_GlassesInfo.Glasses1_ID;

			// create a vtkPolyDataNormals object and set input
			vtkSmartPointer<vtkPolyDataNormals> normals = vtkSmartPointer<vtkPolyDataNormals>::New();
			normals->SetInputData(PhantomPolydataBase);

			// compute point normals and update
			normals->ComputePointNormalsOn();
			normals->ComputeCellNormalsOff();
			normals->Update();

			// get the normal data array
			vtkDataArray* normalData = PhantomPolydataBase->GetPointData()->GetNormals();

			// get the normal vector at a specific point index			
			double normal[3];
			double originalNormal[3];
			normalData->GetTuple(pointID, originalNormal);
			double averagedNormal[3] = { 0,0,0 };
			
			vtkSmartPointer<vtkPoints> points = PhantomPolydataBase->GetPoints(); // Polydata_base의 좌표들
			double Pickedpoint[3];
			points->GetPoint(pointID, Pickedpoint); // Polydata_base에서 pickeID를 이용해서 구한 선택한 좌표

			for (vtkIdType i = 0; i < points->GetNumberOfPoints(); i++) 
			{
				double point[3];
				points->GetPoint(i, point);
				double distance = vtkMath::Distance2BetweenPoints(point, Pickedpoint);//Polydata_base에서 선택한 좌표와 polydata_base 모든 좌표들간의 거리를 구함.
				
				double angleThreshold = 60;
				if (distance < 0.6)
				{
					normalData->GetTuple(i, normal);					

					const double cosineThreshold = cos(vtkMath::RadiansFromDegrees(angleThreshold));

					vtkMath::Normalize(originalNormal); // polydata_base에서 선택한 좌표의 normal값
					vtkMath::Normalize(normal); // polydata_base의 모든 좌표들의 normal값

					const double dotProduct = vtkMath::Dot(originalNormal, normal);
					if(dotProduct>=cosineThreshold)
					{
						averagedNormal[0] += normal[0];
						averagedNormal[1] += normal[1];
						averagedNormal[2] += normal[2];
					}
				}
			}
			vtkMath::Normalize(averagedNormal);
			nx = averagedNormal[0];
			ny = averagedNormal[1];
			nz = averagedNormal[2];
			//QString str = "x: " + QString::number(nx) + "y: " + QString::number(ny) + "z: " + QString::number(nz);
			//theApp.SetMessageBox(str);

			centerX = Pickedpoint[0]; // InteractorStyle에서 지정한 선택한 좌표
			centerY = Pickedpoint[1];
			centerZ = Pickedpoint[2];


		}
		if (i == 2)
		{
			pointID = theApp.m_GlassesInfo.Glasses2_ID;

			// create a vtkPolyDataNormals object and set input
			vtkSmartPointer<vtkPolyDataNormals> normals = vtkSmartPointer<vtkPolyDataNormals>::New();
			normals->SetInputData(PhantomPolydataBase);

			// compute point normals and update
			normals->ComputePointNormalsOn();
			normals->ComputeCellNormalsOff();
			normals->Update();

			// get the normal data array
			vtkDataArray* normalData = PhantomPolydataBase->GetPointData()->GetNormals();

			// get the normal vector at a specific point index			
			double normal[3];
			double originalNormal[3];
			normalData->GetTuple(pointID, originalNormal);
			double averagedNormal[3] = { 0,0,0 };

			vtkSmartPointer<vtkPoints> points = PhantomPolydataBase->GetPoints();
			double Pickedpoint[3];
			points->GetPoint(pointID, Pickedpoint);
			
			for (vtkIdType i = 0; i < points->GetNumberOfPoints(); i++)
			{
				double point[3];
				points->GetPoint(i, point);
				double distance = vtkMath::Distance2BetweenPoints(point, Pickedpoint);
				
				double angleThreshold = 60; // Normal이 급격하게 다른 건 제외하는 threhsold(각)
				if (distance < 0.6) // 0.6 cm 내에 위치한 점들의 평균 노말선택
				{
					normalData->GetTuple(i, normal);

					const double cosineThreshold = cos(vtkMath::RadiansFromDegrees(angleThreshold));

					vtkMath::Normalize(originalNormal);
					vtkMath::Normalize(normal);

					const double dotProduct = vtkMath::Dot(originalNormal, normal);
					if (dotProduct >= cosineThreshold)
					{
						averagedNormal[0] += normal[0];
						averagedNormal[1] += normal[1];
						averagedNormal[2] += normal[2];			
					}
				}
			}
			vtkMath::Normalize(averagedNormal);
			nx = averagedNormal[0];
			ny = averagedNormal[1];
			nz = averagedNormal[2];

			centerX = Pickedpoint[0];
			centerY = Pickedpoint[1];
			centerZ = Pickedpoint[2];
		}	

		cylinderSource->SetRadius(theApp.pRt->EyewearRadius_QLineEdit->text().toDouble());
		cylinderSource->SetHeight(theApp.pRt->EyewearThickness_QLineEdit->text().toDouble());
		cylinderSource->SetCenter(0, (cylinderSource->GetHeight() / 2) + theApp.pRt->EyewearDistance_QLineEdit->text().toDouble(), 0); // Height(두께)의 중점에서 시작 + 눈으로부터 이격거리 만큼 이동
		cylinderSource->SetResolution(15);
				
		vtkSmartPointer<vtkMath> math = vtkSmartPointer<vtkMath>::New();
		double normalVector[3] = { nx, ny, nz };
		math->Normalize(normalVector);
		double originalVector[3] = { 0, 1, 0 }; // 실린더 기본 방향은 y-axis에 along한 방향

		double rotationAxis[3], rotationAngle;
		vtkMath::Cross(originalVector, normalVector, rotationAxis);
		rotationAngle = vtkMath::DegreesFromRadians(vtkMath::AngleBetweenVectors(originalVector, normalVector));

		// Create the transform -> 이렇게 생성되는 Input glasses.obj들은 polydata_base에 맞음(원점이동 되어 있음)
		vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
		transform->PostMultiply();
		transform->RotateWXYZ(rotationAngle, rotationAxis);
		transform->Translate(centerX, centerY, centerZ);

		// obj 파일이 원점이동 전 origianl polydata(즉 .obj파일)과 맞아야 하는데, 현재는 원점이동 및 스케일링 이루어진 polydata_base에서 이루어졌으므로 역원점이동 시켜주어야 함
		// Geant4에 사용되는 tet(node)파일도 origianl .obj파일과 좌표 맞아야 함.
		//transform->Translate(centerX + theApp.m_3DHumanData_MultiplePhantom[phantomIdx].pCenterOriginal[0], centerY + theApp.m_3DHumanData_MultiplePhantom[phantomIdx].pCenterOriginal[1], centerZ + theApp.m_3DHumanData_MultiplePhantom[phantomIdx].pCenterOriginal[2]);

		vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
		transformFilter->SetInputConnection(cylinderSource->GetOutputPort());
		transformFilter->SetTransform(transform);
		transformFilter->Update();

		vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
		mapper->SetInputConnection(transformFilter->GetOutputPort());

		if (i == 1)
		{
			//vtkSmartPointer<vtkActor> actor1 = vtkSmartPointer<vtkActor>::New();
			//actor1->SetMapper(mapper);

			//theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(actor1);
			//theApp.m_pVTKWidget->renderWindow()->Render();

			std::string OutputFilename = "./data/wearable/Glasses/glassesL_" + std::to_string(PhantomIdx) + ".obj";
			
			vtkSmartPointer<vtkOBJWriter> objWriter = vtkSmartPointer<vtkOBJWriter>::New();
			objWriter->SetFileName(OutputFilename.c_str());

			objWriter->SetInputConnection(transformFilter->GetOutputPort());
			objWriter->Write();
		}
		if (i == 2)
		{
			//vtkSmartPointer<vtkActor> actor2 = vtkSmartPointer<vtkActor>::New();
			//actor2->SetMapper(mapper);

			//theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(actor2);
			//theApp.m_pVTKWidget->renderWindow()->Render();

			std::string OutputFilename = "./data/wearable/Glasses/glassesR_" + std::to_string(PhantomIdx)+ ".obj";

			vtkSmartPointer<vtkOBJWriter> objWriter = vtkSmartPointer<vtkOBJWriter>::New();
			objWriter->SetFileName(OutputFilename.c_str());

			objWriter->SetInputConnection(transformFilter->GetOutputPort());
			objWriter->Write();
		}
	}
	
	// append two glasses
	std::string GlassesLFileName = "./data/wearable/Glasses/glassesL_" + std::to_string(PhantomIdx) + ".obj";
	vtkSmartPointer<vtkOBJReader> reader1 = vtkSmartPointer<vtkOBJReader>::New();
	reader1->SetFileName(GlassesLFileName.c_str());
	reader1->Update();

	std::string GlassesRFileName = "./data/wearable/Glasses/glassesR_" + std::to_string(PhantomIdx) + ".obj";
	vtkSmartPointer<vtkOBJReader> reader2 = vtkSmartPointer<vtkOBJReader>::New();
	reader2->SetFileName(GlassesRFileName.c_str());
	reader2->Update();

	// Create a vtkAppendPolyData object to combine the two vtkPolyData objects
	vtkSmartPointer<vtkAppendPolyData> appendFilterGlasses = vtkSmartPointer<vtkAppendPolyData>::New();
	appendFilterGlasses->AddInputData(reader1->GetOutput());
	appendFilterGlasses->AddInputData(reader2->GetOutput());
	appendFilterGlasses->Update();

	// Write the combined vtkPolyData object to a file
	vtkSmartPointer<vtkOBJWriter> writer = vtkSmartPointer<vtkOBJWriter>::New();
	std::string glassName = "./data/wearable/Glasses/glasses_" + std::to_string(PhantomIdx) + ".obj"; // 이 파일이 통합된 polydata_base에 맞는 obj 파일로 append에 사용됨
	writer->SetFileName(glassName.c_str());
	writer->SetInputData(appendFilterGlasses->GetOutput());
	writer->Update();	
	
	// 통합된 glass_obj 다시 불러오기
	vtkSmartPointer<vtkOBJReader> reader_obj = vtkSmartPointer<vtkOBJReader>::New();
	QString strPath = QString::fromStdString(glassName);
	reader_obj->SetFileName(Util::Wcs_to_mbs(strPath.toStdWString()).c_str());
	reader_obj->Update();
	vtkSmartPointer<vtkPolyData> WearablePolydata = vtkSmartPointer<vtkPolyData>::New();
	WearablePolydata = Util::CreatePolyData(reader_obj); // polydata_original에 맞음

	//// VTK 상에서 사용할 glasses는 다시 원점이동 시켜주어야 함(polydata_base에 맞아야 하므로)
	//vtkSmartPointer<vtkTransform> transform_pCenter = vtkSmartPointer<vtkTransform>::New();
	//transform_pCenter->PostMultiply();
	//transform_pCenter->Translate(-theApp.m_3DHumanData_MultiplePhantom[phantomIdx].pCenterOriginal[0],-theApp.m_3DHumanData_MultiplePhantom[phantomIdx].pCenterOriginal[1], -theApp.m_3DHumanData_MultiplePhantom[phantomIdx].pCenterOriginal[2]);

	//vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter_pCenter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	//transformFilter_pCenter->SetInputData(clothingPolydata);
	//transformFilter_pCenter->SetTransform(transform_pCenter);
	//transformFilter_pCenter->Update();
	//clothingPolydata = transformFilter_pCenter->GetOutput();

	///////////////////////// Color scalar 설정 //////////////////////
	double radius = theApp.pRt->EyewearRadius_QLineEdit->text().toDouble(); // 기본 radius = 1 
	double thickness = theApp.pRt->EyewearThickness_QLineEdit->text().toDouble(); // 기본 thickness(height) = 1

	float LeadColor[3] = { 240, 255, 255 }; // Wearable color

	vtkNew<vtkUnsignedCharArray> WearableColors;
	WearableColors->SetNumberOfComponents(3);
	WearableColors->SetNumberOfTuples(WearablePolydata->GetNumberOfPoints());
	for (int i = 0; i < WearablePolydata->GetNumberOfPoints(); ++i)
	{
		WearableColors->SetTuple(i, LeadColor);
	}
	WearablePolydata->GetPointData()->SetScalars(WearableColors);

	//////////////////*** append 순서는 항상 누적되는 .polydata를 먼저!!!////////////////////////////
	vtkSmartPointer<vtkAppendFilter> appendFilter = vtkSmartPointer<vtkAppendFilter>::New();
	appendFilter->AddInputData(theApp.m_3DHumanData_MultiplePhantom[PhantomIdx].polydata); // 원점이동 및 스케일링, 의복 누적 된 polydata과 append
	appendFilter->AddInputData(WearablePolydata);
	appendFilter->Update();
		
	vtkSmartPointer<vtkGeometryFilter> geomFilter = vtkSmartPointer<vtkGeometryFilter>::New();
	geomFilter->SetInputData(appendFilter->GetOutput());
	geomFilter->Update();

	theApp.m_3DHumanData_MultiplePhantom[PhantomIdx].polydata = geomFilter->GetOutput();

	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(theApp.m_3DHumanData_MultiplePhantom[PhantomIdx].polydata);
	mapper->SetScalarVisibility(true);

	theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(theApp.m_3DHumanData_MultiplePhantom[PhantomIdx].actor);
	theApp.m_3DHumanData_MultiplePhantom[PhantomIdx].actor = nullptr;
	theApp.m_3DHumanData_MultiplePhantom[PhantomIdx].actor = Util::CreateActor(theApp.m_3DHumanData_MultiplePhantom[PhantomIdx].polydata);
	theApp.m_3DHumanData_MultiplePhantom[PhantomIdx].actor->SetMapper(mapper);

	double height = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][3]; // Set phantomInfo
	double weight = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][4];
	double CenterX = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][5];
	double CenterY = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][6];
	double CenterZ = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][7];
	double VectorX = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][8];
	double VectorY = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][9];
	double VectorZ = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][10];

	theApp.m_3DHumanData_MultiplePhantom[PhantomIdx].actor->SetPosition(CenterX, CenterY, CenterZ);
	theApp.m_3DHumanData_MultiplePhantom[PhantomIdx].actor->SetOrientation(VectorX, VectorY, VectorZ);
			
	//   
	//선택된 팬텀 하이라이트 
	auto property = Util::GetProperty();
	property->SetInterpolationToPhong();
	property->SetDiffuse(1.0);
	theApp.m_3DHumanData_MultiplePhantom[PhantomIdx].actor->SetProperty(property);

	theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(theApp.m_3DHumanData_MultiplePhantom[PhantomIdx].actor);
	theApp.m_pVTKWidget->renderWindow()->Render();

	// For remeshing cylinder
	//// Generate the normals for the polydata
	//vtkSmartPointer<vtkPolyDataNormals> normals = vtkSmartPointer<vtkPolyDataNormals>::New();
	//normals->SetInputData(transformFilter->GetOutput());
	//normals->ComputePointNormalsOn();
	//normals->Update();
	//// Generate a new set of polygons for the cylinder
	//vtkSmartPointer<vtkTriangleFilter> triangleFilter = vtkSmartPointer<vtkTriangleFilter>::New();
	//triangleFilter->SetInputConnection(normals->GetOutputPort());
	//triangleFilter->Update();

}
void PhantomObjects::PhantomWraparoundGlassesGenerate() //
{
	int PhantomIdx = theApp.pRt->m_Phantom_SelectedIndex;
	theApp.pRt->m_Clothing_MainInfo[PhantomIdx][100][0][0] = theApp.pRt->EyewearDistance_QLineEdit->text().toDouble(); // distance
	theApp.pRt->m_Clothing_MainInfo[PhantomIdx][100][0][1] = theApp.pRt->EyewearRadius_QLineEdit->text().toDouble(); // radius
	theApp.pRt->m_Clothing_MainInfo[PhantomIdx][100][0][2] = theApp.pRt->EyewearThickness_QLineEdit->text().toDouble(); // radius
	theApp.pRt->m_Clothing_MainInfo[PhantomIdx][100][0][3] = theApp.pRt->EyewearDensity_QLineEdit->text().toDouble(); // density
	// composition(1: Glasses, 2: Leaded glasses)
	theApp.pRt->m_Clothing_MainInfo[PhantomIdx][100][0][4] = theApp.pRt->EyewearComposition_QComboBox->currentIndex();

	std::map<int, double> pickedPointCenterX, pickedPointCenterY, pickedPointCenterZ, pickedPointNx, pickedPointNy, pickedPointNz, pointX, pointY, pointZ;
	int pointID;

	vtkSmartPointer<vtkPolyData> PhantomPolydataBase; // 원점이동 및 스케일링만 (이동 및 회전은 X)
	PhantomPolydataBase = theApp.m_3DHumanData_MultiplePhantom[PhantomIdx].polydata_base;

	for (int index = 0; index < 2; index++)
	{
		if (index == 0) pointID = theApp.m_GlassesInfo.Glasses1_ID;
		if (index == 1) pointID = theApp.m_GlassesInfo.Glasses2_ID;

		// create a vtkPolyDataNormals object and set input
		vtkSmartPointer<vtkPolyDataNormals> normals = vtkSmartPointer<vtkPolyDataNormals>::New();
		normals->SetInputData(PhantomPolydataBase);

		// compute point normals and update
		normals->ComputePointNormalsOn();
		normals->ComputeCellNormalsOff();
		normals->Update();

		// get the normal data array
		vtkDataArray* normalData = PhantomPolydataBase->GetPointData()->GetNormals();

		// get the normal vector at a specific point index			
		double normal[3];
		double originalNormal[3];
		normalData->GetTuple(pointID, originalNormal);
		double averagedNormal[3] = { 0,0,0 };

		vtkSmartPointer<vtkPoints> points = PhantomPolydataBase->GetPoints(); // Polydata_base의 좌표들
		double Pickedpoint[3];
		points->GetPoint(pointID, Pickedpoint); // Polydata_base에서 pickeID를 이용해서 구한 선택한 좌표

		for (vtkIdType i = 0; i < points->GetNumberOfPoints(); i++)
		{
			double point[3];
			points->GetPoint(i, point);
			double distance = vtkMath::Distance2BetweenPoints(point, Pickedpoint);//Polydata_base에서 선택한 좌표와 polydata_base 모든 좌표들간의 거리를 구함.

			double angleThreshold = 60;
			if (distance < 0.6)
			{
				normalData->GetTuple(i, normal);

				const double cosineThreshold = cos(vtkMath::RadiansFromDegrees(angleThreshold));

				vtkMath::Normalize(originalNormal); // polydata_base에서 선택한 좌표의 normal값
				vtkMath::Normalize(normal); // polydata_base의 모든 좌표들의 normal값

				const double dotProduct = vtkMath::Dot(originalNormal, normal);
				if (dotProduct >= cosineThreshold)
				{
					averagedNormal[0] += normal[0];
					averagedNormal[1] += normal[1];
					averagedNormal[2] += normal[2];
				}
			}
		}
		vtkMath::Normalize(averagedNormal);
		pickedPointNx[index] = averagedNormal[0];
		pickedPointNy[index] = averagedNormal[1];
		pickedPointNz[index] = averagedNormal[2];

		pickedPointCenterX[index] = Pickedpoint[0]; // InteractorStyle에서 지정한 선택한 좌표
		pickedPointCenterY[index] = Pickedpoint[1];
		pickedPointCenterZ[index] = Pickedpoint[2];
	}
	vtkSmartPointer<vtkCubeSource> cubeSource_center = vtkSmartPointer<vtkCubeSource>::New();
	cubeSource_center->SetXLength(3.0 * abs(pickedPointCenterX[0] - pickedPointCenterX[1]));
	cubeSource_center->SetYLength(theApp.pRt->EyewearThickness_QLineEdit->text().toDouble());
	cubeSource_center->SetZLength(2.0 * theApp.pRt->EyewearRadius_QLineEdit->text().toDouble());

	double normalVector[3] = { (pickedPointNx[0] + pickedPointNx[1]) * 0.5, (pickedPointNy[0] + pickedPointNy[1]) * 0.5, (pickedPointNz[0] + pickedPointNz[1]) * 0.5 };
	vtkMath::Normalize(normalVector);
	double originalVector[3] = { 0, -1, 0 }; // 실린더 기본 방향은 y-axis에 along한 방향

	double rotationAxis[3], rotationAngle;
	vtkMath::Cross(originalVector, normalVector, rotationAxis);
	rotationAngle = vtkMath::DegreesFromRadians(vtkMath::AngleBetweenVectors(originalVector, normalVector));

	// Create the transform -> 이렇게 생성되는 Input glasses.obj들은 polydata_base에 맞음(원점이동 되어 있음)
	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
	transform->PostMultiply();
	transform->RotateWXYZ(rotationAngle, rotationAxis);
	transform->Translate((pickedPointCenterX[0] + pickedPointCenterX[1]) * 0.5, ((pickedPointCenterY[0] + pickedPointCenterY[1]) * 0.5) - cubeSource_center->GetYLength()*0.5 - theApp.pRt->EyewearDistance_QLineEdit->text().toDouble(), (pickedPointCenterZ[0] + pickedPointCenterZ[1]) * 0.5);

	vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	transformFilter->SetInputConnection(cubeSource_center->GetOutputPort());
	transformFilter->SetTransform(transform);
	transformFilter->Update();

	std::string OutputFilename_center = "./data/wearable/Glasses/GoggleCenter_" + std::to_string(PhantomIdx) + ".obj";
	vtkSmartPointer<vtkOBJWriter> objWriter_center = vtkSmartPointer<vtkOBJWriter>::New();
	objWriter_center->SetFileName(OutputFilename_center.c_str());
	objWriter_center->SetInputConnection(transformFilter->GetOutputPort());
	objWriter_center->Write();

	vtkSmartPointer<vtkCubeSource> cubeSource_side1 = vtkSmartPointer<vtkCubeSource>::New();
	cubeSource_side1->SetXLength(theApp.pRt->EyewearThickness_QLineEdit->text().toDouble());
	cubeSource_side1->SetYLength(abs(pickedPointCenterX[0] - pickedPointCenterX[1]));
	cubeSource_side1->SetZLength(2.0 * theApp.pRt->EyewearRadius_QLineEdit->text().toDouble());
	
	vtkSmartPointer<vtkTransform> transform_side1 = vtkSmartPointer<vtkTransform>::New();
	transform_side1->PostMultiply();
	transform_side1->RotateWXYZ(rotationAngle, rotationAxis);
	transform_side1->Translate((pickedPointCenterX[0] + pickedPointCenterX[1]) * 0.5 + 1.5 * abs(pickedPointCenterX[0] - pickedPointCenterX[1]) - 0.5 * cubeSource_side1->GetXLength(), ((pickedPointCenterY[0] + pickedPointCenterY[1]) * 0.5) - theApp.pRt->EyewearDistance_QLineEdit->text().toDouble() + 0.5 * cubeSource_side1->GetYLength(), (pickedPointCenterZ[0] + pickedPointCenterZ[1]) * 0.5);

	vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter_side1 = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	transformFilter_side1->SetInputConnection(cubeSource_side1->GetOutputPort());
	transformFilter_side1->SetTransform(transform_side1);
	transformFilter_side1->Update();

	std::string OutputFilename_side1 = "./data/wearable/Glasses/GoggleSide1_" + std::to_string(PhantomIdx) + ".obj";
	vtkSmartPointer<vtkOBJWriter> objWriter_side1 = vtkSmartPointer<vtkOBJWriter>::New();
	objWriter_side1->SetFileName(OutputFilename_side1.c_str());
	objWriter_side1->SetInputConnection(transformFilter_side1->GetOutputPort());
	objWriter_side1->Write();

	vtkSmartPointer<vtkCubeSource> cubeSource_side2 = vtkSmartPointer<vtkCubeSource>::New();
	cubeSource_side2->SetXLength(theApp.pRt->EyewearThickness_QLineEdit->text().toDouble());
	cubeSource_side2->SetYLength(abs(pickedPointCenterX[0] - pickedPointCenterX[1]));
	cubeSource_side2->SetZLength(2.0 * theApp.pRt->EyewearRadius_QLineEdit->text().toDouble());

	vtkSmartPointer<vtkTransform> transform_side2 = vtkSmartPointer<vtkTransform>::New();
	transform_side2->PostMultiply();
	transform_side2->RotateWXYZ(rotationAngle, rotationAxis);
	transform_side2->Translate((pickedPointCenterX[0] + pickedPointCenterX[1]) * 0.5 - 1.5 * abs(pickedPointCenterX[0] - pickedPointCenterX[1]) + 0.5 * cubeSource_side2->GetXLength(), ((pickedPointCenterY[0] + pickedPointCenterY[1]) * 0.5) - theApp.pRt->EyewearDistance_QLineEdit->text().toDouble() + 0.5 * cubeSource_side2->GetYLength(), (pickedPointCenterZ[0] + pickedPointCenterZ[1]) * 0.5);

	vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter_side2 = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	transformFilter_side2->SetInputConnection(cubeSource_side2->GetOutputPort());
	transformFilter_side2->SetTransform(transform_side2);
	transformFilter_side2->Update();

	std::string OutputFilename_side2 = "./data/wearable/Glasses/GoggleSide2_" + std::to_string(PhantomIdx) + ".obj";
	vtkSmartPointer<vtkOBJWriter> objWriter_side2 = vtkSmartPointer<vtkOBJWriter>::New();
	objWriter_side2->SetFileName(OutputFilename_side2.c_str());
	objWriter_side2->SetInputConnection(transformFilter_side2->GetOutputPort());
	objWriter_side2->Write();

	auto property = Util::GetProperty();
	property->SetInterpolationToPhong();
	property->SetColor(0.5,0.5,0.5);
	property->SetOpacity(0.5);
	
	vtkSmartPointer<vtkPolyDataMapper> mapper_center = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper_center->SetInputConnection(transformFilter->GetOutputPort());
	vtkSmartPointer<vtkActor> actor_center = vtkSmartPointer<vtkActor>::New();
	actor_center->SetMapper(mapper_center);
	actor_center->SetProperty(property);

	vtkSmartPointer<vtkPolyDataMapper> mapper_side1 = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper_side1->SetInputConnection(transformFilter_side1->GetOutputPort());
	vtkSmartPointer<vtkActor> actor_side1 = vtkSmartPointer<vtkActor>::New();
	actor_side1->SetMapper(mapper_side1);
	actor_side1->SetProperty(property);	

	vtkSmartPointer<vtkPolyDataMapper> mapper_side2 = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper_side2->SetInputConnection(transformFilter_side2->GetOutputPort());
	vtkSmartPointer<vtkActor> actor_side2 = vtkSmartPointer<vtkActor>::New();
	actor_side2->SetMapper(mapper_side2);
	actor_side2->SetProperty(property);

	theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(actor_center);
	theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(actor_side1);
	theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(actor_side2);
	theApp.m_pVTKWidget->renderWindow()->Render();
}

////////////////// Phantom Dosimeter ////////////////// 
void PhantomObjects::DosimeterGenerate(int no) // 가시화용(10 um offset, pCenter 0,0,0 맞춤) polydata, 의복비교용(면적 30% margin, pCenter 안 맞춤) polydata, 사면체화용(1 um + separation distance offset, pCenter 안 맞춤) obj 파일 생성
{
	int PhantomIdx = theApp.pRt->m_Phantom_SelectedIndex;
	int DosimeterIdx = no; 
	// dosimeter info 업데이트
	theApp.pRt->m_Clothing_MainInfo[PhantomIdx][999][0][0] = theApp.pRt->m_DosimeterSequenceVector[PhantomIdx].size();
	theApp.pRt->m_Clothing_MainInfo[PhantomIdx][1000][DosimeterIdx][3] = theApp.pRt->DosimeterPosX_QLineEdit->text().toDouble(); // X
	theApp.pRt->m_Clothing_MainInfo[PhantomIdx][1000][DosimeterIdx][4] = theApp.pRt->DosimeterPosY_QLineEdit->text().toDouble(); // Y
	theApp.pRt->m_Clothing_MainInfo[PhantomIdx][1000][DosimeterIdx][5] = theApp.pRt->DosimeterPosZ_QLineEdit->text().toDouble(); // Z
	theApp.pRt->m_Clothing_MainInfo[PhantomIdx][1000][DosimeterIdx][1] = theApp.pRt->DosimeterRadius_QLineEdit->text().toDouble(); // Radius
	theApp.pRt->m_Clothing_MainInfo[PhantomIdx][1000][DosimeterIdx][2] = theApp.pRt->DosimeterSeparationDistance_QLineEdit->text().toDouble(); // Distance
		
	vtkSmartPointer<vtkPolyData> PhantomPolydataBase = theApp.m_3DHumanData_MultiplePhantom[PhantomIdx].polydata_base; // 이 데이터는 BASE POLYDATA임
	vtkSmartPointer<vtkPoints> points = PhantomPolydataBase->GetPoints();
	double PickedPoint[3];
	points->GetPoint(theApp.m_DosimeterInfo.Dosimeter_PointID, PickedPoint); // 3D Pick 및 location update 통해 받아오는 좌표 ID
		
	// 가시화용 사용자가 지정한 영역만으로 polydata 생성
	vtkSmartPointer<vtkCellArray> nearbyCells = vtkSmartPointer<vtkCellArray>::New();// vtkCellArray = {1,2,3,  2,5,8,  3,8,9, ...} -> 1,2,3 pointIds로 하나의 cell(tri)를 만듦
	vtkSmartPointer<vtkPoints> PhantomPolydataBasePoints = PhantomPolydataBase->GetPoints();
	for (vtkIdType i = 0; i < PhantomPolydataBasePoints->GetNumberOfPoints(); i++)
	{
		// Check if the point is within radius of the specified point
		double dist = vtkMath::Distance2BetweenPoints(PhantomPolydataBasePoints->GetPoint(i), PickedPoint); // 거리 제곱을 반환
		double radius = theApp.pRt->DosimeterRadius_QLineEdit->text().toDouble();
		if (dist <= radius * radius) // radius = 2 cm -> dist = 2^2 = 4
		{			
			vtkSmartPointer<vtkIdList> cellIds = vtkSmartPointer<vtkIdList>::New();
			PhantomPolydataBase->GetPointCells(i, cellIds); //  dosim_idx번째 point가 포함된 cell id 배열을 반환 -> 즉 dosim_idx가 2라면 {1,2 ...}을 반환 (1번, 2번 cell에는 ID:2인 point가 포함됨)
			for (vtkIdType j = 0; j < cellIds->GetNumberOfIds(); j++) // 해당 cell id 개수만큼 반복
			{
				vtkSmartPointer<vtkIdList> cellPointIds = vtkSmartPointer<vtkIdList>::New();
				PhantomPolydataBase->GetCellPoints(cellIds->GetId(j), cellPointIds); // 해당 cell id에 해당하는 pointIds를 반환 -> 즉 cell id=2 라면 {2,5,8} 반환
				nearbyCells->InsertNextCell(cellPointIds); // point ID 2,5,8을 하나의 cell이 되도록 1차원 배열로 계속 insert
			}
		}
	}		
	vtkSmartPointer<vtkPolyData> nearbyPolyData = vtkSmartPointer<vtkPolyData>::New(); // 임시 데이터
	nearbyPolyData->SetPoints(PhantomPolydataBasePoints);
	nearbyPolyData->SetPolys(nearbyCells);
	vtkNew<vtkPolyDataConnectivityFilter> connectivityFilter_Dosimeter;
	connectivityFilter_Dosimeter->SetInputData(nearbyPolyData);
	connectivityFilter_Dosimeter->SetExtractionModeToLargestRegion(); // clean 및 island 제거
	connectivityFilter_Dosimeter->Update();
	vtkSmartPointer<vtkPolyData> PolydataOriginalDosimeter = vtkSmartPointer<vtkPolyData>::New();
	PolydataOriginalDosimeter = connectivityFilter_Dosimeter->GetOutput(); // 사용자가 지정한 선량계 polydata (마진 offset 하기 전)
			
	// 0.001 cm offset -> 가시화만 진행
	vtkSmartPointer<vtkPolyDataNormals> normalsFilter = vtkSmartPointer<vtkPolyDataNormals>::New();
	normalsFilter->SetInputData(PolydataOriginalDosimeter);
	normalsFilter->ComputePointNormalsOn();
	normalsFilter->ComputeCellNormalsOff();
	normalsFilter->Update();
	vtkDataArray* normalData = normalsFilter->GetOutput()->GetPointData()->GetNormals();
	double offsetDistance;
	offsetDistance = 0.001; // Just for visulaziation for offset 10 um from phantom skin
	for (vtkIdType i = 0; i < PolydataOriginalDosimeter->GetNumberOfPoints(); i++)
	{
		double p[3];
		double n[3];
		PolydataOriginalDosimeter->GetPoint(i, p);
		normalData->GetTuple(i, n);
		p[0] = p[0] + n[0] * offsetDistance;
		p[1] = p[1] + n[1] * offsetDistance;
		p[2] = p[2] + n[2] * offsetDistance;
		PolydataOriginalDosimeter->GetPoints()->SetPoint(i, p);
	}
	PolydataOriginalDosimeter->Modified();

	vtkSmartPointer<vtkOBJWriter> writer_OriginalDosimeter = vtkSmartPointer<vtkOBJWriter>::New();
	std::string OrigianlDosimeterStr = "./data/wearable/Dosimeter/" + std::to_string(PhantomIdx) + "_" + std::to_string(DosimeterIdx) + "_tempForVis.obj";
	writer_OriginalDosimeter->SetFileName(OrigianlDosimeterStr.c_str());
	writer_OriginalDosimeter->SetInputData(PolydataOriginalDosimeter);
	writer_OriginalDosimeter->Update();
	
	// Importing polydata for matching with phantom and visualizing by color	
	vtkSmartPointer<vtkOBJReader> reader_dosimeter = vtkSmartPointer<vtkOBJReader>::New(); // TempDosimeterPolydata의 texture coordinate는 scalar 할당 안 되어 있는 원래 좌표도 갖고 있으므로 OBJ를 다시 불러옴
	QString dosimeterQstr = QString::fromStdString("./data/wearable/Dosimeter/" + std::to_string(PhantomIdx) + "_" + std::to_string(DosimeterIdx) + "_tempForVis.obj"); // 생성된 main region OBJ 불러오기
	reader_dosimeter->SetFileName(Util::Wcs_to_mbs(dosimeterQstr.toStdWString()).c_str());
	reader_dosimeter->Update();
	vtkSmartPointer<vtkPolyData> FinalDosimeterPolydata = vtkSmartPointer<vtkPolyData>::New(); // 해당 선량계는 polydata_base와 좌표 평행이동 위치가 맞음 (따라서 추후 사면체화 후, node 정렬할때 pCenter 원점이동 거침)
	FinalDosimeterPolydata = Util::CreatePolyData(reader_dosimeter);

	float GoldColor[3] = { 50, 50, 50 }; // Wearable color
	vtkNew<vtkUnsignedCharArray> DosimeterColors;
	DosimeterColors->SetNumberOfComponents(3);
	DosimeterColors->SetNumberOfTuples(FinalDosimeterPolydata->GetNumberOfPoints());
	for (int i = 0; i < FinalDosimeterPolydata->GetNumberOfPoints(); ++i)
	{
		DosimeterColors->SetTuple(i, GoldColor);
	}
	FinalDosimeterPolydata->GetPointData()->SetScalars(DosimeterColors);

	// Append dosimeter and phantom(accumulated with wearable) 
	vtkSmartPointer<vtkAppendFilter> appendFilter = vtkSmartPointer<vtkAppendFilter>::New();
	appendFilter->AddInputData(theApp.m_3DHumanData_MultiplePhantom[PhantomIdx].polydata);
	appendFilter->AddInputData(FinalDosimeterPolydata);
	appendFilter->Update();

	vtkSmartPointer<vtkGeometryFilter> geomFilter = vtkSmartPointer<vtkGeometryFilter>::New();
	geomFilter->SetInputData(appendFilter->GetOutput());
	geomFilter->Update();
	theApp.m_3DHumanData_MultiplePhantom[PhantomIdx].polydata = geomFilter->GetOutput();

	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(theApp.m_3DHumanData_MultiplePhantom[PhantomIdx].polydata);
	mapper->SetScalarVisibility(true);

	theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(theApp.m_3DHumanData_MultiplePhantom[PhantomIdx].actor);
	theApp.m_3DHumanData_MultiplePhantom[PhantomIdx].actor = nullptr;
	theApp.m_3DHumanData_MultiplePhantom[PhantomIdx].actor = Util::CreateActor(theApp.m_3DHumanData_MultiplePhantom[PhantomIdx].polydata);
	theApp.m_3DHumanData_MultiplePhantom[PhantomIdx].actor->SetMapper(mapper);

	double height = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][3]; // Set phantomInfo
	double weight = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][4];
	double CenterX = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][5];
	double CenterY = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][6];
	double CenterZ = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][7];
	double VectorX = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][8];
	double VectorY = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][9];
	double VectorZ = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][10];

	theApp.m_3DHumanData_MultiplePhantom[PhantomIdx].actor->SetPosition(CenterX, CenterY, CenterZ);
	theApp.m_3DHumanData_MultiplePhantom[PhantomIdx].actor->SetOrientation(VectorX, VectorY, VectorZ);

	auto property = Util::GetProperty();
	property->SetInterpolationToPhong();
	property->SetDiffuse(1.0);
	theApp.m_3DHumanData_MultiplePhantom[PhantomIdx].actor->SetProperty(property);

	theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(theApp.m_3DHumanData_MultiplePhantom[PhantomIdx].actor);
	theApp.m_pVTKWidget->renderWindow()->Render();

	// 겹치는 의복을 만들기 위한 margin을 둔 OriginalDosimeterPolydata_forTetra 생성
	vtkSmartPointer<vtkCellArray> nearbyCells_margin = vtkSmartPointer<vtkCellArray>::New();// vtkCellArray = {1,2,3,  2,5,8,  3,8,9, ...} -> 1,2,3 pointIds로 하나의 cell(tri)를 만듦
	for (vtkIdType i = 0; i < PhantomPolydataBasePoints->GetNumberOfPoints(); i++)
	{
		// Check if the point is within radius of the specified point
		double dist = vtkMath::Distance2BetweenPoints(PhantomPolydataBasePoints->GetPoint(i), PickedPoint); // 거리 제곱을 반환
		double radius = theApp.pRt->DosimeterRadius_QLineEdit->text().toDouble();
		if (dist <= radius * 1.3 * radius * 1.3) // 30% margin
		{
			vtkSmartPointer<vtkIdList> cellIds = vtkSmartPointer<vtkIdList>::New();
			PhantomPolydataBase->GetPointCells(i, cellIds); //  dosim_idx번째 point가 포함된 cell id 배열을 반환 -> 즉 dosim_idx가 2라면 {1,2 ...}을 반환 (1번, 2번 cell에는 ID:2인 point가 포함됨)
			for (vtkIdType j = 0; j < cellIds->GetNumberOfIds(); j++) // 해당 cell id 개수만큼 반복
			{
				vtkSmartPointer<vtkIdList> cellPointIds = vtkSmartPointer<vtkIdList>::New();
				PhantomPolydataBase->GetCellPoints(cellIds->GetId(j), cellPointIds); // 해당 cell id에 해당하는 pointIds를 반환 -> 즉 cell id=2 라면 {2,5,8} 반환
				nearbyCells_margin->InsertNextCell(cellPointIds); // point ID 2,5,8을 하나의 cell이 되도록 1차원 배열로 계속 insert
			}
		}
	}
	vtkSmartPointer<vtkPolyData> nearbyPolyData_margin = vtkSmartPointer<vtkPolyData>::New(); // 임시 데이터
	nearbyPolyData_margin->SetPoints(PhantomPolydataBasePoints);
	nearbyPolyData_margin->SetPolys(nearbyCells_margin);
	vtkNew<vtkPolyDataConnectivityFilter> connectivityFilter_Dosimeter_margin;
	connectivityFilter_Dosimeter_margin->SetInputData(nearbyPolyData_margin);
	connectivityFilter_Dosimeter_margin->SetExtractionModeToLargestRegion(); // clean 및 island 제거
	connectivityFilter_Dosimeter_margin->Update();
	vtkSmartPointer<vtkPolyData> PolydataDosimeter_margin = vtkSmartPointer<vtkPolyData>::New();
	PolydataDosimeter_margin = connectivityFilter_Dosimeter_margin->GetOutput();
	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
	double pCenterX = theApp.m_3DHumanData_MultiplePhantom[PhantomIdx].pCenterOriginal[0];
	double pCenterY = theApp.m_3DHumanData_MultiplePhantom[PhantomIdx].pCenterOriginal[1];
	double pCenterZ = theApp.m_3DHumanData_MultiplePhantom[PhantomIdx].pCenterOriginal[2];
	transform->Translate(pCenterX, pCenterY, pCenterZ);  // pCenter 안 맞춘걸로 돌리기
	vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	transformFilter->SetInputData(PolydataDosimeter_margin);
	transformFilter->SetTransform(transform);
	transformFilter->Update();
	theApp.OriginalDosimeterPolydata_forTetra[PhantomIdx][DosimeterIdx] = transformFilter->GetOutput(); // 이 데이터는 pCenter 0,0,0으로 안 맞춘(즉, 팬텀 original 파일) 좌표와 일치함 -> 여기에 offset해서 가시화용 + 의복과 겹침 확인용
	
	// 실제 사면체화에 사용될 offset된 팬텀 데이터로부터 제작하는 선량계 사면체
	vtkSmartPointer<vtkPolyData> PhantomPolydata_offset;
	// polydata_base로부터 pCenter 0,0,0 안 맞춘 상태로 돌리기
	vtkSmartPointer<vtkTransform> transform_p = vtkSmartPointer<vtkTransform>::New();
	transform_p->PostMultiply(); //this is the key line
	transform_p->Translate(theApp.m_3DHumanData_MultiplePhantom[PhantomIdx].pCenterOriginal[0], theApp.m_3DHumanData_MultiplePhantom[PhantomIdx].pCenterOriginal[1], theApp.m_3DHumanData_MultiplePhantom[PhantomIdx].pCenterOriginal[2]);
	vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter_p = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	transformFilter_p->SetInputData(theApp.m_3DHumanData_MultiplePhantom[PhantomIdx].polydata_base);
	transformFilter_p->SetTransform(transform_p);
	transformFilter_p->Update();
	PhantomPolydata_offset = transformFilter_p->GetOutput();
	//선량계 거리만큼 offset offset
	vtkSmartPointer<vtkPolyDataNormals> normalsFilter_offset = vtkSmartPointer<vtkPolyDataNormals>::New();
	normalsFilter_offset->SetInputData(PhantomPolydata_offset);
	normalsFilter_offset->ComputePointNormalsOn();
	normalsFilter_offset->ComputeCellNormalsOff();
	normalsFilter_offset->Update();
	vtkDataArray* normalData_offset = normalsFilter_offset->GetOutput()->GetPointData()->GetNormals();
	double offsetDistance_offset = 0.0001 + theApp.pRt->m_Clothing_MainInfo[PhantomIdx][1000][DosimeterIdx][2]; // 1um 마진 + separation distance
	for (vtkIdType i = 0; i < PhantomPolydata_offset->GetNumberOfPoints(); i++)
	{
		double p[3];
		double n[3];
		PhantomPolydata_offset->GetPoint(i, p);
		normalData_offset->GetTuple(i, n);
		p[0] = p[0] + n[0] * offsetDistance_offset;
		p[1] = p[1] + n[1] * offsetDistance_offset;
		p[2] = p[2] + n[2] * offsetDistance_offset;
		PhantomPolydata_offset->GetPoints()->SetPoint(i, p);
	}
	PhantomPolydata_offset->Modified();

	vtkSmartPointer<vtkPoints> points_offset = PhantomPolydata_offset->GetPoints();
	double PickedPoint_offset[3];
	points_offset->GetPoint(theApp.m_DosimeterInfo.Dosimeter_PointID, PickedPoint_offset); // 3D Pick 및 location update 통해 받아오는 좌표 ID
	vtkSmartPointer<vtkCellArray> nearbyCells_offset = vtkSmartPointer<vtkCellArray>::New();// vtkCellArray = {1,2,3,  2,5,8,  3,8,9, ...} -> 1,2,3 pointIds로 하나의 cell(tri)를 만듦

	// 사용자가 지정한 영역의 polydata 생성
	vtkSmartPointer<vtkPoints> PhantomPolydata_offsetPoints = PhantomPolydata_offset->GetPoints();
	for (vtkIdType i = 0; i < PhantomPolydata_offsetPoints->GetNumberOfPoints(); i++)
	{
		// Check if the point is within radius of the specified point
		double dist = vtkMath::Distance2BetweenPoints(PhantomPolydata_offsetPoints->GetPoint(i), PickedPoint_offset); // 거리 제곱을 반환
		double radius = theApp.pRt->DosimeterRadius_QLineEdit->text().toDouble();
		if (dist <= radius * radius) // radius = 2 cm -> dist = 2^2 = 4
		{
			vtkSmartPointer<vtkIdList> cellIds = vtkSmartPointer<vtkIdList>::New();
			PhantomPolydata_offset->GetPointCells(i, cellIds); //  dosim_idx번째 point가 포함된 cell id 배열을 반환 -> 즉 dosim_idx가 2라면 {1,2 ...}을 반환 (1번, 2번 cell에는 ID:2인 point가 포함됨)
			for (vtkIdType j = 0; j < cellIds->GetNumberOfIds(); j++) // 해당 cell id 개수만큼 반복
			{
				vtkSmartPointer<vtkIdList> cellPointIds = vtkSmartPointer<vtkIdList>::New();
				PhantomPolydata_offset->GetCellPoints(cellIds->GetId(j), cellPointIds); // 해당 cell id에 해당하는 pointIds를 반환 -> 즉 cell id=2 라면 {2,5,8} 반환
				nearbyCells_offset->InsertNextCell(cellPointIds); // point ID 2,5,8을 하나의 cell이 되도록 1차원 배열로 계속 insert
			}
		}
	}
	vtkSmartPointer<vtkPolyData> nearbyPolyData_offset = vtkSmartPointer<vtkPolyData>::New(); // 임시 데이터
	nearbyPolyData_offset->SetPoints(PhantomPolydata_offsetPoints);
	nearbyPolyData_offset->SetPolys(nearbyCells_offset);
	vtkNew<vtkPolyDataConnectivityFilter> connectivityFilter_Dosimeter_offset;
	connectivityFilter_Dosimeter_offset->SetInputData(nearbyPolyData_offset);
	connectivityFilter_Dosimeter_offset->SetExtractionModeToLargestRegion(); // clean 및 island 제거
	connectivityFilter_Dosimeter_offset->Update();
	vtkSmartPointer<vtkPolyData> PolydataDosimeter_offset = vtkSmartPointer<vtkPolyData>::New();
	PolydataDosimeter_offset = connectivityFilter_Dosimeter_offset->GetOutput(); // 사용자가 지정한 선량계 polydata (마진 offset 하기 전)

	vtkSmartPointer<vtkOBJWriter> writer_OffsetDosimeter = vtkSmartPointer<vtkOBJWriter>::New();
	std::string OffsetDosimeterStr = "./data/wearable/Dosimeter/" + std::to_string(PhantomIdx) + "_" + std::to_string(DosimeterIdx) + ".obj";
	writer_OffsetDosimeter->SetFileName(OffsetDosimeterStr.c_str());
	writer_OffsetDosimeter->SetInputData(PolydataDosimeter_offset);
	writer_OffsetDosimeter->Update();
}
void PhantomObjects::RefreshDosimeter3DShpere(double PickedPos[3], int no)
{
	if (theApp.m_3DHumanData.dosimeterActorSphereClicked)
		theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(theApp.m_3DHumanData.dosimeterActorSphereClicked);
	// Create a sphere
	vtkSmartPointer<vtkSphereSource> sphereSource =	vtkSmartPointer<vtkSphereSource>::New();
	sphereSource->SetCenter(0.0, 0.0, 0.0);
	sphereSource->SetRadius(0.5);
	// Make the surface smooth.
	sphereSource->SetPhiResolution(60);
	sphereSource->SetThetaResolution(30);
	double centerX = PickedPos[0];
	double centerY = PickedPos[1];
	double centerZ = PickedPos[2];
	// actor 생성
	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputConnection(sphereSource->GetOutputPort());
	theApp.m_3DHumanData.dosimeterActorSphereClicked = vtkSmartPointer<vtkActor>::New();
	theApp.m_3DHumanData.dosimeterActorSphereClicked->SetMapper(mapper);
	vtkSmartPointer<vtkNamedColors> colors = vtkSmartPointer<vtkNamedColors>::New();
	theApp.m_3DHumanData.dosimeterActorSphereClicked->GetProperty()->SetColor(colors->GetColor3d("Green").GetData());
	theApp.m_3DHumanData.dosimeterActorSphereClicked->GetProperty()->SetOpacity(0.4);
	theApp.m_3DHumanData.dosimeterActorSphereClicked->SetPosition(centerX, centerY, centerZ);	
	theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(theApp.m_3DHumanData.dosimeterActorSphereClicked);
	theApp.m_pVTKWidget->renderWindow()->Render();
	theApp.m_DosimeterInfo.Dosimeter_point[0] = centerX;
	theApp.m_DosimeterInfo.Dosimeter_point[1] = centerY;
	theApp.m_DosimeterInfo.Dosimeter_point[2] = centerZ;
}
void PhantomObjects::RemoveDosimeter3DShpere() {
	theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(theApp.m_3DHumanData.dosimeterActorSphereClicked);
}
//////////////////////////////////////////////////////