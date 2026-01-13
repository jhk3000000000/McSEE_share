#pragma once
#include <vector>
#include <map>
#include <QString>

struct BroadBeamModel 
{
    // --- Direction Selection ---
    // 콤보박스(m_comboBoxBeamdirection)의 선택된 인덱스 (ISO, ROT 등)
    int BroadBeamDirectionIndex = 0; 

    // --- User Defined Angles (만약 User Defined 모드일 때 사용) ---
    double AzimuthalAngle = 0.0;
    double PolarAngle = 0.0;

    // --- Particle & Energy ---
    QString BroadBeamParticleType; // 예: "Photon"
    double BroadBeamEnergy = 0.0;

    // --- Visualization State (선택 사항) ---
    // 시각화 버튼이 켜져 있는지 여부
    bool isVisualized = false; 
};

struct ExternalPointModel 
{
    // =================================================================
    // 1. [핵심] 리스트 데이터 (기존 Widget에서 가져옴)
    // =================================================================
    // ListInfo_sourceEP[pointID][0~5] 
    // 0: 사용여부, 1: posX, 2: posY, 3: posZ, 4: RI, 5: activity(Bq)
    std::vector<std::map<int, QString>> ListInfo_sourceEP; 
    
    // 리스트 인덱스 카운터
    int List_sourceEP_MakingIndex = 0;


    // =================================================================
    // 2. [입력값] UI에 입력된 값을 임시 저장할 변수들
    // =================================================================

    // --- Geometry Input (현재 입력창에 적힌 좌표) ---
    double PosX = 0.0;
    double PosY = 0.0;
    double PosZ = 0.0;

    // --- Mode Selection (Radionuclide vs Energy Spectrum) ---
    bool isRadionuclideMode = false; 
    bool isEnergySpectrumMode = false;

    // --- Radionuclide Input ---
    QString currentRadionuclideName; // 입력창의 핵종 이름
    double currentActivity = 0.0;    // 입력창의 방사능 값

    // --- Energy Spectrum Input ---
    QString currentSpectrumFile;     // 선택된 파일 경로
    double currentSpectrumIntensity = 0.0; 
    
    // (참고: Energy Spectrum용 데이터 벡터가 기존 코드에는 안 보였는데,
    //  만약 존재한다면 여기에 std::vector<...> ES_ListInfo; 같은 게 추가되어야 합니다.)
};

struct FloorDiskModel 
{
    // =================================================================
    // 1. [핵심] 리스트 데이터 (기존 Widget에서 가져옴 - 변수명 유지)
    // =================================================================
    // ListInfo_sourceFD[pointID][0~2] 
    // 0: 사용여부, 1: RI, 2: activity(Bq/cm3) (주석 내용 참고하여 map 사용)
    std::vector<std::map<int, QString>> ListInfo_sourceFD; 
    
    // 리스트 인덱스 카운터
    int List_sourceFD_MakingIndex = 0;


    // =================================================================
    // 2. [입력값] UI 값을 임시 저장할 변수들 (새로 생성)
    // =================================================================

    // --- Geometry Input ---
    double PosX = 0.0;
    double PosY = 0.0;
    double PosZ = 0.0;
    double Radius = 0.0;

    // --- Mode Selection ---
    bool isRadionuclideMode = false; 
    bool isEnergySpectrumMode = false;

    // --- Radionuclide Input ---
    QString currentRadionuclideName; 
    double currentActivity = 0.0;

    // --- Energy Spectrum Input ---
    QString currentSpectrumFile;     
    double currentSpectrumIntensity = 0.0; 
};
// [SourceModels.h 에 추가 - 수정본]

struct ObjectVolumeModel 
{
    // =================================================================
    // 1. [핵심] 오브젝트 관리 데이터 (변수명 원복)
    // =================================================================
    
    // 추가된 오브젝트 ID 목록
    std::vector<int> m_sourceOV_objectSequenceVector; 

    // 오브젝트 바운딩 박스 정보
    std::map<int, std::vector<double>> sourceOV_objectBound; 

    // 현재 선택된 오브젝트 ID
    int Object_sourceOV_SelectedIndex = -1; 


    // =================================================================
    // 2. [핵심] 오브젝트별 소스 리스트 데이터
    // =================================================================
    
    // Key: Object ID, Value: 해당 오브젝트에 속한 소스 리스트
    std::map<int, std::vector<std::map<int, QString>>> ListInfo_sourceOV; 
    
    // 리스트 인덱스 카운터
    int List_sourceOV_MakingIndex = 0;


    // =================================================================
    // 3. [상태 플래그] (기존 변수명 그대로 유지)
    // =================================================================
    bool b_IsSourceLocationVisualized_sourceOV = false;
    bool b_IsSourceOV_AddingSettingClosed = false;
    bool b_IsSourceOV_AddingSettingOKClicked = false;
    bool b_sourceOV_InObjectSelect = false;


    // =================================================================
    // 4. [입력값] UI 임시 저장 값 (이건 새로 만드는 거라 이름 정하기 나름이지만, 통일성 있게)
    // =================================================================
    
    // 라디오 버튼 상태
    bool isRadionuclideMode = false; 
    bool isEnergySpectrumMode = false;

    // RI 입력값
    QString currentRadionuclideName;
    double currentActivity = 0.0;

    // ES 입력값
    QString currentSpectrumFile;
    double currentSpectrumIntensity = 0.0;
};

enum class PSMode {
    MCNP,
    PHITS,
    FLUKA,
    IAEA,
    USER,
    None      // 아무것도 선택 안 된 경우
};

struct PhaseSpaceModel 
{
    // --- Mode Selection ---
    PSMode selectedMode = PSMode::None; // 현재 선택된 포맷

    // --- Visualization & Options ---
    bool isVisualized = false;        // 시각화 On/Off
    bool isDirectionChecked = false;  // Direction CheckBox On/Off

    // --- File Information ---
    // 파일 경로는 QTextEdit에 있으므로 QString으로 저장
    QString phaseSpaceFileName; 
};

struct HotParticleModel 
{
    // =================================================================
    // 1. [핵심] 리스트 데이터 및 상태 변수 (기존 변수명 유지)
    // =================================================================
    
    // 소스 리스트
    std::vector<std::map<int, QString>> ListInfo_sourceHP; 
    
    // 리스트 인덱스 카운터 (변수명 오타 주의: RIList... 입니다)
    int RIList_sourceHP_MakingIndex = 0; 

    // 3D Pick 로직용 노말 벡터 (배열 -> 벡터로 변경)
    // 초기값 {0,0,0} 설정
    std::vector<double> SelectedPointNormalVector_sourceHP = { 0.0, 0.0, 0.0 };


    // =================================================================
    // 2. [입력값] UI 임시 저장 값
    // =================================================================

    // --- Geometry ---
    double PosX = 0.0;
    double PosY = 0.0;
    double PosZ = 0.0;
    double SeparationDist = 0.0; // 이격 거리

    // --- Mode Selection ---
    bool isRadionuclideMode = false; 
    bool isEnergySpectrumMode = false;

    // --- Radionuclide Input ---
    QString currentRadionuclideName;
    double currentActivity = 0.0;

    // --- Energy Spectrum Input ---
    QString currentSpectrumFile;
    double currentSpectrumIntensity = 0.0;
    
    // --- Visualization ---
    bool isVisualized = false;
};


// 1. 공용 구조체 정의 (위젯 내부 -> 공용 헤더로 이동)
struct ConeBeamParams {
    double maxAngle;
    double direction[3]; // a, b, c
    double point[3];     // cx, cy, cz
    int numPoints;       // 계산된 포인트 수
};

// 2. 모델 정의
struct ConeBeamModel 
{
    // =================================================================
    // 1. [핵심] 지오메트리 데이터
    // =================================================================
    double PointX = 0.0, PointY = 0.0, PointZ = 0.0;
    double DirectionX = 0.0, DirectionY = 0.0, DirectionZ = 1.0;
    double SolidAngle = 0.0;


    // =================================================================
    // 2. [핵심] 리스트 및 관리 데이터 (기존 변수명 유지)
    // =================================================================
    
    // 소스 리스트 (변수명 sourceCB_info 유지)
    std::vector<std::map<int, QString>> sourceCB_info; 
    
    // 리스트 카운터
    int RIList_sourceCB_MakingIndex = 0; 
    int EnergyspectrumListTotalCount_CB = 0;

    // 시각화 플래그
    bool b_IsSourceLocationVisualized_sourceCB = false;


    // =================================================================
    // 3. [입력값] UI 임시 저장 값
    // =================================================================
    
    // Direction CheckBox (방향 표시 여부)
    bool isDirectionChecked = false;

    // Mode
    bool isRadionuclideMode = false; 
    bool isEnergySpectrumMode = false;

    // RI Input
    QString currentRadionuclideName;
    double currentActivity = 0.0;

    // ES Input
    QString currentSpectrumFile;
    double currentSpectrumIntensity = 0.0;
};

struct RoomContaminationModel 
{
    // =================================================================
    // 1. [핵심] 지오메트리 데이터 (Box Dimensions & Center)
    // =================================================================
    double Box_HalfLengthX = 0.0;
    double Box_HalfLengthY = 0.0;
    double Box_HalfLengthZ = 0.0;

    double Box_CenterX = 0.0;
    double Box_CenterY = 0.0;
    double Box_CenterZ = 0.0;


    // =================================================================
    // 2. [핵심] 리스트 데이터 (기존 변수명 유지)
    // =================================================================
    
    // 소스 리스트: [pointID] -> [0:사용여부, 1:RI, 2:Activity]
    std::vector<std::map<int, QString>> RI_Info_sourceRC; 

    // 리스트 인덱스 카운터
    int RIList_sourceRC_MakingIndex = 0;


    // =================================================================
    // 3. [입력값] UI 임시 저장 값
    // =================================================================

    // --- Mode Selection ---
    bool isRadionuclideMode = false; 
    bool isEnergySpectrumMode = false;

    // --- Radionuclide Input ---
    QString currentRadionuclideName;
    double currentActivity = 0.0;
    
    // --- Visualization ---
    bool isVisualized = false;
};


struct EnvironmentalContaminationModel 
{
    // =================================================================
    // 1. [핵심] 설정 데이터
    // =================================================================
    
    // 소스 유형 (ComboBox Index)
    // 0: Air, 1: Soil, 2: Water 등
    int selectedTypeIndex = 0; 

    // 토양 깊이 (Soil Depth) - Soil 모드일 때만 유효하지만 저장은 해둠
    double soilDepth = 0.0;


    // =================================================================
    // 2. [핵심] 리스트 데이터
    // =================================================================
    
    // 소스 리스트 (기존 위젯들의 패턴에 맞춰 추가)
    // Key: ID, Value: 정보 맵
    std::vector<std::map<int, QString>> sourceEC_ListInfo; 
    
    // 리스트 인덱스 카운터
    int List_sourceEC_MakingIndex = 0;


    // =================================================================
    // 3. [입력값] UI 임시 저장 값
    // =================================================================

    // Radionuclide Input
    QString currentRadionuclideName;
    double currentActivity = 0.0;
    
    // Visualization
    bool isVisualized = false;
};
// [새로 만든 데이터 보관함]
struct ParallelBeamModel 
{
    // =================================================================
    // 1. [핵심] 지오메트리 데이터
    // =================================================================
    double PointX = 0.0;
    double PointY = 0.0;
    double PointZ = 0.0;
    
    double Radius = 0.0; // Parallel Beam 반지름

    double DirectionTheta = 0.0;
    double DirectionPhi = 0.0;


    // =================================================================
    // 2. [핵심] 물리 모드 및 설정
    // =================================================================
    
    // 모드 선택 (true: Mono, false: Spectrum)
    bool isMonoEnergyMode = true; 

    // --- Mono Energy Mode ---
    int particleTypeIndex = 0; // Particle Type ComboBox Index
    QString ParallelBeamParticleType;
    double monoEnergy = 0.0;
    double monoIntensity = 0.0;

    // --- Energy Spectrum Mode ---
    QString spectrumFileName;
    double spectrumIntensity = 0.0;

    // --- Visualization & Option ---
    bool b_IsSourceLocationVisualized_sourcePB = false;       // 시각화 여부 (Model로 이동 추천)
    bool isDirectionChecked = false; // 방향 화살표 표시 여부


    // =================================================================
    // 3. [핵심] 리스트 데이터 (스펙트럼 파일 목록)
    // =================================================================
    
    // Key: ID, Value: 정보 맵
    std::vector<std::map<int, QString>> sourcePB_ES_info; 
    
    // 리스트 인덱스 카운터
    int ESList_sourcePB_MakingIndex = 0; 
};