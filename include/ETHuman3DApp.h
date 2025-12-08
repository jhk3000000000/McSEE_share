#ifndef MAINAPPLICATION_H
#define MAINAPPLICATION_H

#include <Eigen/Dense>
#include <fstream>
#include <vector>
#include <set>
#include <unordered_map>
#include <string>
#include <iostream>
#include <vtkCommand.h>
#include <vtkIntersectionPolyDataFilter.h>
#include <vtkBooleanOperationPolyDataFilter.h>
#include <vtkTriangleFilter.h>
#include <vtkCleanPolyData.h>
#include <vtkTransformFilter.h>
#include <vtkScalarBarActor.h>
#include <vtkKdTree.h>
#include <vtkKdTreePointLocator.h>
#include <vtkCellLocator.h>
#include <windows.h>
#include <psapi.h>
#include <QFile>

#ifdef _WIN32	
	#include <windows.h>
#elif __linux__
	#include <sys/sysinfo.h>
#endif


class ETQVTKWidget;
class FunctionPanelTop;
class FunctionPanelRight;
class ProcessInformationPanel;
class RealTimeResultPanel;
class QHoverSensitiveButton;

class QIODeviceStreamBuf : public std::streambuf {
public:
	QIODeviceStreamBuf(QIODevice* device) : m_device(device) {
		// For input (get area)
		setg(nullptr, nullptr, nullptr);
		// For output (put area)
		setp(m_buffer, m_buffer + sizeof(m_buffer) - 1);
	}

protected:
	// Input function 
	int_type underflow() override;

	// Output function 
	int_type overflow(int_type c = traits_type::eof()) override;

	// Synchronize the buffer with the device
	int sync() override;

private:
	QIODevice* m_device;
	char m_buffer[4096];
};

// Input stream
class QIODeviceIStream : public std::istream {
public:
	QIODeviceIStream(QIODevice* device);
	~QIODeviceIStream();

private:
	QIODeviceStreamBuf m_buf;
};

// Output stream
class QIODeviceOStream : public std::ostream {
public:
	QIODeviceOStream(QIODevice* device);
	~QIODeviceOStream();

private:
	QIODeviceStreamBuf m_buf;
};

// Input/Output stream
class QIODeviceIOStream : public std::iostream {
public:
	QIODeviceIOStream(QIODevice* device);
	~QIODeviceIOStream();

private:
	QIODeviceStreamBuf m_buf;
};

enum ePanelMode : int
{
	E_PHANTOM = 0,
	E_GEOMETRY = 1,
	E_SOURCE = 2,
	E_OUTPUT = 3
};

// 개별 선량 데이터를 저장할 구조체
struct SkinDoseData {
	qint32 phantomId;
	double x;
	double y;
	double z;
	double dE;
};

struct s3DHumanData
{
	QString strFilePath;
	QString PhantomName;
	//Original Polydata(polydata_original)는 Reader로 읽은 직후의 polydata로 따로 변수는 없음->필요할때 읽기
	vtkSmartPointer<vtkPolyData> polydata_base; // This is Base Polydata (pCenter 0,0,0 맞추는 원점이동과 체형변형 스케일링이 진행된 base polydata)
	vtkSmartPointer<vtkPolyData> polydata; // This is Accumulated Polydata (base polydata에 의복을 누적해가면서 색상이 변하는 polydata)	
	double pCenterOriginal[3]; 
	/* polydata 원본(original)의 중심좌표(실제 obj는 0,0,0이 center가 아니지만 vtk 상에서는 0, 0, 0으로 맞추기 위함)
	translation (위치)과 roatation (회전)은 polydata에서 마지막에 한 번에 이동 및 회전시킴 */

	vtkSmartPointer<vtkActor> actor;

	vtkSmartPointer<vtkActor> dosimeterActorSphereClicked; // just for point actor
	vtkSmartPointer<vtkActor> CBactorSphereClicked; // just for point actor
	std::vector<vtkSmartPointer<vtkActor>> actorSphereMultiple; // Used for multiple external point source (Point kernal) and phsp file, which are imported from text file.
	vtkSmartPointer<vtkActor> actorSkinMaxPoint; // just for point actor


	s3DHumanData()
	{
		strFilePath = "";
		PhantomName = "";
		pCenterOriginal[0] = 0;
		pCenterOriginal[1] = 0;
		pCenterOriginal[2] = 0;

		if (polydata_base)
			polydata = nullptr;
		if (polydata)
			polydata = nullptr;
		if (actor)
			actor = nullptr;
		if (CBactorSphereClicked)
			CBactorSphereClicked = nullptr;
		if (!actorSphereMultiple.empty())
			actorSphereMultiple.clear();
		if (actorSkinMaxPoint)
			actorSkinMaxPoint = nullptr;

	}

	void reset()
	{
		strFilePath = "";
		PhantomName = "";
		pCenterOriginal[0] = 0;
		pCenterOriginal[1] = 0;
		pCenterOriginal[2] = 0;

		if (polydata_base)
			polydata = nullptr;
		if (polydata)
			polydata = nullptr;
		if (actor)
			actor = nullptr;
		if (CBactorSphereClicked)
			CBactorSphereClicked = nullptr;
		if (!actorSphereMultiple.empty())
			actorSphereMultiple.clear();
		if (actorSkinMaxPoint)
			actorSkinMaxPoint = nullptr;
	}

	void resetActor()
	{	
		if (actor)
			actor = nullptr;	
	}
};
struct BodySizeInfo {
	QString HtWtName;
	QString HtWtName_dummy;

	double xyScale =1;
	double zScale =1;

	double xyScale_dummy = 1;
	double zScale_dummy = 1;

};
struct GlassesInfo {
	int    Glasses1_ID;
	int    Glasses2_ID;
};
struct DosimeterInfo {
	int    Dosimeter_PointID;
	double Dosimeter_point[3];
};
struct Point_SolidAngle {
	double theta, phi, x, y, z;
};

class ETHuman3DApp : public QObject
{
public:
// [1] 안내 데스크: 유일한 인스턴스를 반환하는 함수
    static ETHuman3DApp& Instance() {
        // C++11 이상에서는 이 정적 지역 변수의 초기화가 스레드 안전(Thread-Safe)합니다.
        // 즉, 처음 호출될 때 딱 한 번만 안전하게 생성됩니다.
        static ETHuman3DApp instance; 
        return instance;
    }

    // [2] 복제 방지: 실수로 앱을 복사하거나 대입하는 것을 원천 봉쇄
    ETHuman3DApp(const ETHuman3DApp&) = delete;
    void operator=(const ETHuman3DApp&) = delete;

private: 
    // [3] 생성자 숨기기: 외부에서 'new ETHuman3DApp'을 못하게 막습니다.
    ETHuman3DApp(); 
    ~ETHuman3DApp();

public:
	void SetMessageBox(QString text);
	void SetMessageBox_RadionuclideWarning(QString text);
	void SetMessageBox_NoButton(QString text);
	int	 SetMessageBox_YesNo(QString text);

public:
	QProgressBar* LoadingBar = nullptr;
	ETQVTKWidget* m_pVTKWidget = nullptr;

	FunctionPanelTop* pTop = nullptr;
	FunctionPanelRight* pRt = nullptr;
	ProcessInformationPanel* pInf = nullptr;
	RealTimeResultPanel* pRslt = nullptr;

public:
	s3DHumanData  m_3DHumanData;
	GlassesInfo	  m_GlassesInfo;
	DosimeterInfo m_DosimeterInfo;
	
	ePanelMode	PanelMode;

	vtkSmartPointer<vtkActor> testActor1;
	vtkSmartPointer<vtkActor> testActor2;
	vtkSmartPointer<vtkActor> testActor3;

	bool b_IsDontShowRadionuclideWarningChecked = false;
	
	// Phantom panel
	std::map<int, std::map<int, std::map<int, vtkSmartPointer<vtkPolyData>>>> PhantomPanelPolyData_Collection; // 팬텀, 의복류가 각각 따로 존재하는 폴리데이터 모음 PhantomPanelPolydata[PhantomIdx][Type][Idx] = polydata / Type 0: Phantom, Type 1: Clothing, Type 2: Dosimeters, Type 3: Glasses
	std::map<int, vtkSmartPointer<vtkPolyData>> PhantomPanelAccumulatedPolyData; // PhantomPanelAccumulatedPolyData[PhantomIdx] = polydata *PhantomPanelAccumulatedPolyData는 PhantomPanelPolyData_Collection의 polydata가 모두 어펜드된 현재 actor를 만드는 polydata이며, rubberband 시에 필요함
	std::map<int, vtkSmartPointer<vtkActor>> FullPhantomActor;
	std::map<int, vtkSmartPointer<vtkActor>> DummyPhantomActor;
	std::map<int, vtkSmartPointer<vtkActor>> PhantomPanelActor; // 팬텀패널에 존재하는 actor
	std::map<int, vtkSmartPointer<vtkPolyData>> PhantomPolyData_original; // Obj 그대로 불러온 polydata
	std::map<int, std::map<int, double>> PhantomOrigianlPolyDataCenter; // origianl .obj 팬텀 데이터의 중점, PhantomOrigianlPolyDataCenter[phantomIndex][0~2]: x, y, z
	std::map<int, std::map<int, double>> PhantomPolyDataScaleFactor; // PhantomPolyDataScaleFactor[phantomIndex][0~1]: xy scale, z scale
	std::map<int, std::map<int, double>> DummyPolyDataInfo; //0: PolyCenterX, 1: PolyCenterX, 2: PolyCenterX, 3: ScaleX, 4: ScaleX, 5: ScaleX
	std::map<int, QString> PhantomFileTitle; // Used Phantom file name -> 확장자를 제거한 팬텀파일이름
	std::map<int, QString> m_ImportedPhantomFilePath; // 뒤의 확장자 까지 있는 파일경로
	std::map<int, QString> m_ImportedPhantomFilePath_NoExtention; // 뒤의 확장자만 없는 파일 경로
	std::map<int, QString> m_ImportedPhantom_ParentPhantomFileName; // 기반 팬텀의 확장자 없는 파일명
	std::set<int> RubberBandSelectedPointIDs;
	vtkSmartPointer<vtkPolyData> WearableForImportedPolydata = vtkSmartPointer<vtkPolyData>::New();
	std::map<int, std::map<int, vtkSmartPointer<vtkPolyData>>> OriginalClothingPolydata_forTetra; // pCenter 0,0,0 안 맞추고 scaling만 된 상태
	std::map<int, std::map<int, vtkSmartPointer<vtkPolyData>>> OriginalDosimeterPolydata_forTetra;
	std::map<int, std::map<int, std::vector<std::tuple<int, int, double>>>> SortedOverlappedDosimeterInfo; // SortedOverlappedDosimeterInfo[PhantomID][ClothingID] = {겹치는선량계ID, 겹치는의복시작레이어ID, 피부로부터 겹치는 의복레이어의 시작면 까지의 거리}
	bool		flgNextGlasses = false;
	vtkSmartPointer<vtkActor> Glasses1_Point;
	vtkSmartPointer<vtkActor> Glasses2_Point;
	// Not used
	std::vector<s3DHumanData> m_3DHumanData_MultiplePhantom;	
	std::map<int, std::map<int, vtkNew<vtkActor>>> BooleanActor; // intersection check하여 boolean된 actor map
	std::map<int, std::map<int, bool>> PhantomIntersectionInfo;
	QString PhantomIntersectionInfoString;

	// Geometry panel		
	std::map<int, vtkSmartPointer<vtkActor>> ObjectPanelActors;
	std::map<int, vtkSmartPointer<vtkPolyData>> ObjectPanelPolydata_Collection; // ObjectPanelPolydata_Collection[ObjectIdx] = polydata (이 폴리데이터는 polydata_base로 scale만 되어 있는것, pos/rot은 actor에서 설정)	
	std::map<int, vtkSmartPointer<vtkActor>> H_AK_Actors;

	// Source panel
	int m_Point_Source_Scale = 0;
	double const_PointSource_ScaleFactorPerClick = 1.07;
	std::vector<vtkSmartPointer<vtkActor>> SourcePanelActor_Position;
	std::vector<vtkSmartPointer<vtkActor>> SourcePanelActor_Direction; // CB,PS 이식하기!
	vtkSmartPointer<vtkActor> NowSelecting3DSphereActor; // 클릭중표시 임시 벡터 (하나만 있으면 됨)
	// 이식 필요
	vtkSmartPointer<vtkActor> actor_circleB;
	vtkSmartPointer<vtkActor> actor_sphereB;
	vtkSmartPointer<vtkActor> actor_single_cylinderB;
	std::vector<vtkSmartPointer<vtkAssembly>> actors_arrowB;
	std::vector<vtkSmartPointer<vtkActor>> actors_planeB;
	std::vector<vtkSmartPointer<vtkActor>> actors_cylinderB;
	std::vector<vtkSmartPointer<vtkActor>> actors_coneB;

	vtkSmartPointer<vtkActor> actor_RC;
	std::vector<vtkSmartPointer<vtkActor>> actors_directionCB;
	
	std::vector<std::tuple<double, std::string, std::string>> RAD;
	std::vector<std::tuple<std::string, std::string, std::string, std::string, std::string>> Point_kernel;
	std::vector<std::pair<std::string, std::string>> MultipleRI;
	std::vector<std::tuple<std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string>> PSF_data;
	std::vector<std::tuple<double, double, double>> PSF_location_vector;
	std::vector<std::tuple<double, double, double>> PSF_direction_vector;
	
	// Not used
	std::vector<vtkSmartPointer<vtkActor>> SourceDirectionActor;
	std::vector<vtkSmartPointer<vtkActor>> multipleHPsphereActor;
	std::map<int, bool> multipleHPsphereActorGenerated;

	// Pick3D
	bool        st_Pick3D_sourceEP = false;
	bool        st_Pick3D_sourceHP = false;
	bool        st_Pick3D_sourceCB = false;
	bool        m_AirKermaPick3Dflag = false;	
	bool		m_ConeBeamPick3D = false;
	bool		m_HotParticlePick3D = false;	
	bool		m_DosimeterPick3D = false;
	bool		m_GlassesPick3D = false;
	
	// Calculation
	double DoseConversionFactor_inUImodule;
	
	// Output panel
	std::map<int, std::map<int, std::map<int, QString>>> OutputDose_data; // OutputDose_data[phantomID][OrganIdx][0 or 1] = organname or dose
	
	// 하나로 통합 필요
	std::map<int, std::map<int, std::map<int, QString>>> multiple_outputDoseRateEP; // multiple_outputDoseRateEP[phantomID][OrganIdx][0 or 1] = organname or dose
	std::map<int, std::map<int, std::map<int, QString>>> multiple_outputPersonalDosimeterEP; // multiple_outputPersonalDosimeterEP[phantomID][DosimeterID][0 or 1] = dosimetername or dose
	std::map<int, std::map<int, std::map<int, QString>>> multiple_outputDoseRateFD; // multiple_outputDoseRateEP[phantomID][OrganIdx][0 or 1] = organname or dose
	std::map<int, std::map<int, std::map<int, QString>>> multiple_outputPersonalDosimeterFD; // multiple_outputPersonalDosimeterEP[phantomID][DosimeterID][0 or 1] = dosimetername or dose
	std::map<int, std::map<int, std::map<int, QString>>> multiple_outputFluenceB; // multiple_outputDoseRateEP[phantomID][OrganIdx][0 or 1] = organname or dose
	std::map<int, std::map<int, std::map<int, QString>>> multiple_outputPersonalDosimeterB; // multiple_outputPersonalDosimeterEP[phantomID][DosimeterID][0 or 1] = dosimetername or dose
	std::map<int, std::map<int, std::map<int, QString>>> multiple_outputDosePS; // multiple_outputDoseRateEP[phantomID][OrganIdx][0 or 1] = organname or dose
	std::map<int, std::map<int, std::map<int, QString>>> multiple_outputPersonalDosimeterPS; // multiple_outputPersonalDosimeterEP[phantomID][DosimeterID][0 or 1] = dosimetername or dose
	std::map<int, std::map<int, std::map<int, QString>>> multiple_outputDoseRateHP; // multiple_outputDoseRateEP[phantomID][OrganIdx][0 or 1] = organname or dose
	std::map<int, std::map<int, std::map<int, QString>>> multiple_outputPersonalDosimeterHP; // multiple_outputPersonalDosimeterEP[phantomID][DosimeterID][0 or 1] = dosimetername or dose	
	std::map<int, std::map<int, std::map<int, QString>>> multiple_outputDoseRateOV; // multiple_outputDoseRateEP[phantomID][OrganIdx][0 or 1] = organname or dose
	std::map<int, std::map<int, std::map<int, QString>>> multiple_outputPersonalDosimeterOV; // multiple_outputPersonalDosimeterEP[phantomID][DosimeterID][0 or 1] = dosimetername or dose
	std::map<int, std::map<int, std::map<int, QString>>> multiple_outputDoseRateRC; // multiple_outputDoseRateEP[phantomID][OrganIdx][0 or 1] = organname or dose
	std::map<int, std::map<int, std::map<int, QString>>> multiple_outputPersonalDosimeterRC; // multiple_outputPersonalDosimeterEP[phantomID][DosimeterID][0 or 1] = dosimetername or dose	
	
	// Skin layer generation
	std::map<int, double> SkinDenstiy;
	std::map<int, double> SkinAverageDepth; //SkinAverageDepth[phantomID] = average depth from outer skin to inner skin (RST) (cm)
	std::map<int, vtkSmartPointer<vtkPolyData>> SkinLayer_PolyData; //SkinLayer_PolyData[phantomID] = polydata_skin_outer
	std::map<int, vtkSmartPointer<vtkCellLocator>> SkinCellLocator; // 최근접점 판단을 위한 cell 판단
	std::map<int, vtkSmartPointer<vtkCellLocator>> m_SkinVolumeLocator;
	std::map<int, std::vector<std::array<double, 9>>> FacetInfo; //FacetInfo[phantomID][FacetID] = 무게중심점의 [centerx(cm), centery(cm), centerz(cm), nx, ny, nz, Distance to inner skin surface(RST)(cm), area of facet(cm2), volume of wedge(50-100) um, cm3)]
	std::map<int, std::vector<std::vector<int>>> VertexToFacetMap; // VertexToFacetMap[phantomID][vertexID] = {facetID1, facetID2, ...}
	std::map<int, std::map<int, std::set<int>>> TinyFacetAdjacentFacetMap; // TinyFacetAdjacentFacetMap[phantomID][facetID] = {facetID1, facetID2, ...}
	std::map<int, std::map<int, double>> TinyFacetAdjacentFacetVolumeSum; // TinyFacetAdjacentFacetVolumeSum[phantomID][facetID] = areaSum

	// For skin dose visulaization 
	bool		b_IsSkinDoseVisualizationOn = false;
	std::map<int, vtkSmartPointer<vtkActor>> SkinPhantomActor;
	vtkSmartPointer<vtkScalarBarActor> SkinDoseVisualization_ScalarBar;
	vtkSmartPointer<vtkTextActor> SkinDoseVisualization_Label0percent, SkinDoseVisualization_Label20percent, SkinDoseVisualization_Label40percent, SkinDoseVisualization_Label60percent, SkinDoseVisualization_Label80percent, SkinDoseVisualization_Label100percent;

	// Skin dose visulaization calculation (누적 데이터)
	std::map<int, std::map<int, std::pair<double, double>>> Facet_DE_DE2; //FacetDE[phantomID][FacetID] = 누적 dE
	std::map<int, std::map<int, double>> FacetDose; //FacetDose[phantomID][FacteID] = 누적 Dose(Gy, Joule/kg)	
	std::map<int, std::map<int, double>> FacetError; //FacetDose[phantomID][FacteID] = 누적 Dose(Gy, Joule/kg)	
	std::map<int, std::map<int, double>> VertexDose; //VertexDose[phantomID][vertexID] = 누적 Dose(Gy, Joule/kg)	
	std::map<int, int> MaximumFacetDoseID; // MaximumFacetDoseID[phantomID] = FacetID;

	// 1, 10 cm2 최대선량 누적데이터
	std::map<int, std::pair<double, double>> FacetDE_DE2_1cm2;
	std::map<int, std::pair<double, double>> FacetDE_DE2_10cm2;


	// HP skin dose depth calculation	
	int HP_SkinDepth_Interval = 10; // (um)
	std::map<int, std::map<int, std::vector<int>>> HP_Facet_DEpointID; // HP_Facet_DEpointID[phantomID][FacetID] = {pointID, pointID, ...} -> 해당 facet에 dE를 준 DEpoint의 ID들 모음
	std::map<int, std::map<int, double>> HP_FacetDE;
	std::map<int, std::map<int, std::map<double, double>>> HP_FacetDE_Depth; // HP_FacetDE_Depth[phantomID][FacetID][depth(cm)] = 누적 dE, *1/10cm2 국부 facet의 선량 -> 각각 최대 선량 지점(facetID, 해당좌표) 알기 위함
	std::map<int, std::map<int, double>> HP_FacetDE_50_100um; // HP_FacetDE_50_100ump[phantomID][FacetID] = 50-100 um depth인 dE만 더한 것
	// 1, 10cm2 최대선량지점/선량값 -> 그래프제작용
	std::map<int, std::vector<double>> HP_1cm2_MaximumLocation_GraphInfo;
	std::map<int, double> HP_1cm2_MaximumDose_GraphInfo;
	std::map<int, double> HP_1cm2_RelativeError_GraphInfo;
	std::map<int, std::vector<double>> HP_10cm2_MaximumLocation_GraphInfo;
	std::map<int, double> HP_10cm2_MaximumDose_GraphInfo;
	std::map<int, double> HP_10cm2_RelativeError_GraphInfo;
	// 1, 10cm2 깊이분포 -> 그래프제작용
	std::map<int, std::vector<std::pair<double, double>>> HP_1cm2_Depth_GraphInfo; // HP_1cm2_Depth_GraphInfo[phantomID] = {depth 간격의 중간, dose}, {depth 간격의 중간, dose}, {depth 간격의 중간, dose}...
	std::map<int, std::vector<std::pair<double, double>>> HP_10cm2_Depth_GraphInfo; // HP_1cm2_Depth_GraphInfo[phantomID] = {depth 간격의 중간, dose}, {depth 간격의 중간, dose}, {depth 간격의 중간, dose}...

public:
	void	RestartProgram();	
	bool    LoadPSF_data(QString path);
	void    PSF_MCNP_ssw_parse_file(const std::string& filename);
	int		PSF_MCNP_ssw_loadrecord(std::ifstream& file, std::vector<char>& buf, uint32_t reclen, uint64_t& lbuf);	
	bool	PSF_MCNP_ssw_readbytes(std::ifstream& file, char* buf, size_t bytes);
	int32_t PSF_MCNP_conv_mcnp6_ssw2pdg(int32_t c);
	std::string PSF_PHITS_replaceDwithE(std::string input);

	bool    SettingContentsChangeCheck(); // CalculationSettingDialog에서 setting 값이 바뀌었는 지 여부를 반환하는 함수
	void    WearableTetrahedralization(); // Wearable tetrahedralization + Save macro data + .node tranformation
	void    OffsetFromObj(QString inputPath, QString outputPath, double offset); // obj 파일 받아서 offset 진행
	void    combineTetrahedronFiles(const std::string& nodeFileL, const std::string& nodeFileR, const std::string& eleFileL, const std::string& eleFileR, const std::string& eleFileOutput, const std::string& nodeFileOutput); // Non-used, for test
	void	GenerateClothingLayerOverlappedWithDosimeter(int phantomIdx, int WearableIdx, int dosimeterIdx, std::map<int, std::map<int, std::vector<std::tuple<int, int, double>>>>& sorted_overlapped_dosimeter_info); // 선량계와 겹치는 의복 + 의복 side(최소 4 mm)를 offset(단, 의복 side는 절반만 offset)
	void	OffsetPointsByNormals(vtkPolyData* polyData, std::vector<vtkIdType>& pointIds, double offset);
	void    GenerateSideDosimeter(QString inputOrigianlDosimeterPath, int phantomIdx, double offset, QString OutputPath); // 선량계 side 부분 obj파일 생성하는 함수
	void    CleaningPolyData(vtkSmartPointer<vtkPolyData> polyData, std::string path);
	void    SkinLayerGeneration(); // 피부선량분포 계산을 위한 obj 파일 및 데이터들 초기화	
	void    ReadSkinDoseData(std::stringstream &ss);
	
	void    VisualizeVolumeRatio(int a);

	void	ReadSkinDoseData_1_10cm2(const std::vector<SkinDoseData>& totalDoseData,
		qint64 event_count,
		double unit_conversion_factor);
	void    ReadSkinDoseData_HP(std::stringstream &ss);	
	vtkSmartPointer<vtkPolyData> ExportGeodesicROIForPhantom(int phantomID,
		double targetAreaCm2,
		double refineFactor,
		vtkSmartPointer<vtkPolyData>* roiBeforeSubdivision = nullptr);
	int     HP_SkinDepth_to_Index(int phantomID, double input_depth);
	void    SaveData();
	void	Make_mcsee_File_extdata_previous(QString filepath);
	void	MakeFile_PhantomCollection();
	void    MakeFile_ListCollection();	
	void	MakeFile_CalcCollection();
	void	MakeFile_SourceMacro();
	void	Generate_MaterialFile();
	void	Generate_PSFdata(); // Phase space file source 사용 시, 선원항 파일 생성
	std::pair<std::vector<std::map<int, std::string>>, double> Read_RI_File(std::string RIpath);
	void    dbsendFilesClear();
	void	ResultLoad_OrganDose_OutputPanel(std::stringstream &ss);
	void	LoadOutputData_AirKerma(std::stringstream &ss);
	SIZE_T GetProcessMemoryUsage();
	std::string formatToThreeDecimalPlaces(const std::string& numStr);
	FILETIME getLastWriteTime(const std::string& path);

	void WriteSaveFile(std::string path);
	void WriteExtdataFile(std::string path);
	void Read_mcsee_File_previous(QString filepath);

	void	PhantomPolydataActor_Generate(QString strFileName, int SelectedIndex, BodySizeInfo BodySizeInfo_for_ThisPhantom);
	std::string    GetPhantomFileTitle(std::string phantomDir); // 뒤의 확장자와 앞의 경로명이 제거된 순수 팬텀 파일명
	
	QString GetPhantomFile_AbsolutePath(int phantomType, int phantomGender, int phantomAge, int phantomPosture, int phantomFetalAge, int phantomIndex, BodySizeInfo BodySizeInfo_for_ThisPhantom);
	void	PhantomClothingGenerate(QString strFileName, bool IsPreDefinedClothing); 
	/* 최초 의복 입력 시, 의복 제작 함수 -> 가시화용(5 um offset, pCenter 0,0,0 맞춤) polydata와 사면체화용(offset X, pCenter 안 맞춤) obj 파일 생성
	 PhantomClothingGenerate 함수는 Pre-defined 의복 뿐 아니라, User-defined 의복 입력 시에도 실행되어야함.
	 이때, User-defined 의복 입력 시에는 ETInteractorStyleRubberBand 실행하여 제작완료 함수 입력 후에도 실행되어야 하기 때문에, 해당 함수에 의복정보를 저장 및 의복패널 초기화 함수가 포함되어 있음.
	 */
	std::string	ExtractPhantomOBJ(std::string PATH, std::string Name, std::vector<std::string> list);

	void    MultipleUICloseTrigger();
	std::string		ExtractInnerString(std::string& input);
	void	RubberBandInitialization(); // 사용자 제작 의복을 제작할 때, 도구 초기화 과정
	void    RubberBandUserClothingGenerate();
	void    PhantomFlatGlassesGenerate();
	void	PhantomWraparoundGlassesGenerate();
	void	TranslatePhantomTetFile(int reset_phantomID); // resetID-> makingindex와 잔트상의 팬텀ID정렬 필요...
	void	TranslateClothingTetFile(int reset_phantomID); // resetID-> makingindex와 잔트상의 팬텀ID정렬 필요...
	void    GenerateGlassesTetFile(int phantomIdx);
	void    TranslateGlassesTetFile(int phantomIdx);
	void	DosimeterGenerate(int no);	
	void	ObjectGenerate_Box(int no);
	void	ObjectGenerate_Sphere(int no);
	vtkSmartPointer<vtkPolyData> CreateTubeMesh(double R_in, double R_out, double halfHeight, double startDeg, double deltaDeg, int resolution);
	void	ObjectGenerate_Cylinder(int no);	
	//void	ObjectGenerate_Group(int no);
	void    AddAzimuthalAngularPoints(vtkSmartPointer<vtkPoints> points, double radius, double startAngle, double endAngle, double FixedAngle, int numPoints);
	void    AddPartialCirclePoints(vtkSmartPointer<vtkPoints> points, double radius, double startAngle, double endAngle, int numPoints);	
	void	UpdateObject_ActorHighlighted(int objectIndex);	
	void	UpdateObject_InfoStatus_InActorMouseControl(int objectIndex);
	

	BodySizeInfo  CalcBoydSizeScaleFactor(int phantomType, int phantomGender, int phantomAge, int phantomPosture, double phantomHeight, double phantomWeight);

	void    SetAssemblyOpacity(vtkAssembly* assembly, double value);
	std::vector<Point_SolidAngle> GetPointsWithinSolidAngle_sourceCB(double maxAngle, double a, double b, double c, int numPoints);
	void    GenerateSourceActor_sourceBB();
	void	GenerateSourceActor_sourceEP(double* ptCenter);
	void	GenerateSourceActor_sourceHP(double* ptCenter);
	void	GenerateSourceActor_sourceCB(double* ptCenter);
	void	GenerateSourceActor_sourceFD(double* center_radius);
	void	GenerateSourceDirectionActor_sourceCB();	
	void	GenerateSourceActor_sourcePB(double* center_radius, double* theta_phi);
	void    Selecting3DShpere(double* ptCenter);
	void    Selecting3DShpere_Delete();
	
	
	void    GenerateSourceActor_sourcePS(std::vector<std::tuple<double, double, double>> ptMultiple, std::vector<std::tuple<double, double, double>> dirMultiple);
	
	bool    arePointsEqual(double point1[3], double point2[3], double tolerance);
	bool    areCellsEquivalent(vtkSmartPointer<vtkPolyData> polyData1, vtkIdType cellId1, vtkSmartPointer<vtkPolyData> polyData2, vtkIdType cellId2, double tolerance);
	
	void    RefreshDosimeter3DShpere(double PickedPos[3], int no);
	void	RemoveDosimeter3DShpere();

	void    UpdatePhantom_ActorHighlighted(int i);	
	void    MouseControl_toActor();
	void    MouseControl_toCamera();
	void    UpdatePhantom_InfoStatus_InActorMouseControl(int phantomIndex);

	void	AppendPhantomPolyData_GeneratePhantomActor(int PhantomIdx);

	QHoverSensitiveButton* m_hoverButtonIntersectionInfo = nullptr;

	float	CalcDistance2D(QPointF v1, QPointF v2);
	int     removeElement(std::vector<int>& vec, int toRemove);
	int    transposeElement(std::vector<int>& vec, int elementToMove, QString LEFT_Right);

	QString    getQStringNumberInSpecificDigit(double number, int digit);

	std::string SetComputerID();

	void	test_obj();
	void	test_poly();
	bool	b_IsButtonDragging = false;

	//Memory Usage
	double TotalSystemMemoryGB = 8;
	double GetTotalSystemMemoryGB();
	bool isLowSpecMode();//legacy
	bool OldComputer(); //legacy

	bool isLowMemoryState() const;
	void setLowMemoryState(bool LowMemoryState_);
	bool LowMemoryState = true;
};


// [4] 핵심 꿀팁: 기존 코드를 수정하지 않기 위한 매크로
// 이제 코드 어디서든 'theApp'이라고 쓰면 자동으로 'ETHuman3DApp::Instance()'로 바꿔줍니다.
#define theApp ETHuman3DApp::Instance()

// extern ETHuman3DApp theApp; // <--- 이 줄은 삭제합니다.

#endif // MAINAPPLICATION_H
