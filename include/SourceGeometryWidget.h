#pragma once

#include <QGroupBox>
#include <QFont>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QVBoxLayout>
#include "SourceModels.h" // 위에서 만든 헤더 포함


class FunctionPanelRight;       // 부모패널
class MultipleUIDialog;         // 다이얼로그
class MultipleButtonScorllArea; // 스크롤 영역

class SourceGeometryWidget : public QGroupBox
{
	Q_OBJECT


protected:
	explicit SourceGeometryWidget(QWidget* parent = Q_NULLPTR);
public:
	virtual ~SourceGeometryWidget();

    virtual void WriteSourceMacro(std::ofstream& ofp_source);
    virtual void WriteSourceInfo(std::ofstream& ofp_info);
    virtual void ReadSourceInfo(std::ifstream& ifp);
	FunctionPanelRight* panel = Q_NULLPTR;

public:
	virtual bool initialize();
};


//////////////////////////////////////////////////////////////////////
// Broad Beam (BB) Widget
//////////////////////////////////////////////////////////////////////
class BroadBeamWidget : public SourceGeometryWidget
{
    using ParentT = SourceGeometryWidget;
    Q_OBJECT

public:
    explicit BroadBeamWidget(QWidget* parent = Q_NULLPTR);
    virtual ~BroadBeamWidget();

    bool initialize() override;

    void WriteSourceMacro(std::ofstream& ofp_source) override;
    void WriteSourceInfo(std::ofstream& ofp_info) override;
    void ReadSourceInfo(std::ifstream& ifp) override;

    // [New] 모델 접근 함수 (이제 이 함수 하나로 모든 데이터에 접근 가능)
    BroadBeamModel& getModel() { return m_model; }
    void setModel(const BroadBeamModel& data); // 필요 시 구현

    // [Old] 기존 Getter들 (호환성을 위해 남겨둠, 나중에 m_model 값 리턴하도록 수정 권장)
    // int getBB_DirectionIndex();
    // double getBB_PolarAngle();
    // double getBB_AzimAngle();

private:
    // [New] 순수 데이터 저장소
    BroadBeamModel m_model;

    // [New] UI -> Model 데이터 동기화 함수
    void updateModelFromUI();
    void updateUI();

    // --- UI Components (View) ---
    // 포인터들은 그대로 둡니다.
    QPushButton* m_BroadBeamSourceVisualizationButton = Q_NULLPTR;
    QComboBox* m_comboBoxBeamdirection = Q_NULLPTR;
    QComboBox* m_comboBoxBroadBeamParticleType = Q_NULLPTR;
    QLineEdit* m_lineBroadBeamEnergy = Q_NULLPTR;

    QGroupBox* m_AziPolGroupBox = Q_NULLPTR;
    QLineEdit* m_lineEditAzimuthalAngle = Q_NULLPTR;
    QLineEdit* m_lineEditPolarAngle = Q_NULLPTR;

private:
    // 내부 UI 생성 로직 (그대로 유지)
    void CreateAziPol(int idx, QVBoxLayout*);
    void SetBroadBeamAziPol(int user);
    std::string GetBeamDirectionString(int index);

public slots:
    // UI 이벤트 슬롯 (그대로 유지)
    void slot_BroadBeamUpdate_ButtonClicked();
    void slot_BroadBeamUserDefined(int user);
    void slot_BroadBeamSourceVisualization_ButtonClicked();
};

//////////////////////////////////////////////////////////////////////
// External Point (EP) Widget
//////////////////////////////////////////////////////////////////////
class ExternalPointWidget : public SourceGeometryWidget
{
    using ParentT = SourceGeometryWidget;
    Q_OBJECT

public:
    explicit ExternalPointWidget(QWidget* parent = Q_NULLPTR);
    virtual ~ExternalPointWidget();

    bool initialize() override;
    
    void WriteSourceMacro(std::ofstream& ofp_source) override;
    void WriteSourceInfo(std::ofstream& ofp_info) override;
    void ReadSourceInfo(std::ifstream& ifp) override;

    // // [기존 함수] 좌표 업데이트용 (외부에서 호출되는 경우 유지)
    void updateEP_Points(double x, double y, double z);

    // [New] 모델 접근 및 동기화
    ExternalPointModel& getModel() { return m_model; }
    void setModel(const ExternalPointModel& data); // 필요 시 구현

private:
    // [New] 데이터 저장소
    ExternalPointModel m_model;

    // [New] UI -> Model 동기화 함수
    void updateModelFromUI();

    // ============================================================
    // [UI Components] 화면 구성 요소들은 그대로 둡니다.
    // ============================================================
    QGroupBox* m_sourceGeometryExternalPointGroupBox = Q_NULLPTR;
    
    // Sphere Size Buttons
    QPushButton* SourceEP_SphereSize_Minus_QPushButton = Q_NULLPTR;
    QPushButton* SourceEP_SphereSize_Plus_QPushButton = Q_NULLPTR;
    
    QPushButton* m_ExternalPointSourceVisualizationButton = Q_NULLPTR;
    
    // Position Inputs
    QLineEdit* PosX_sourceEP_QLineEdit = Q_NULLPTR;
    QLineEdit* PosY_sourceEP_QLineEdit = Q_NULLPTR;
    QLineEdit* PosZ_sourceEP_QLineEdit = Q_NULLPTR;
    
    QPushButton* pickExternal3DButton = Q_NULLPTR;
    QPushButton* sourceEP_UpdatePositionButton = Q_NULLPTR;
    
    // Mode Selection
    QRadioButton* RI_Select_sourceEP_QRadioButton = Q_NULLPTR;
    QRadioButton* ES_Select_sourceEP_QRadioButton = Q_NULLPTR;
    
    // Radionuclide UI
    QLabel* RI_Title_sourceEP_QLabel = Q_NULLPTR;
    QLineEdit* RI_Radionuclide_sourceEP_QLineEdit = Q_NULLPTR;
    QLabel* RI_Activity_sourceEP_QLabel = Q_NULLPTR;
    QLineEdit* RI_Activity_sourceEP_QLineEdit = Q_NULLPTR;
    QLabel* RI_List_sourceEP_QLabel = Q_NULLPTR;
    QPushButton* ExternalSingleRadionuclideAddButton = Q_NULLPTR;
    QListWidget* RI_List_sourceEP_QListWidget = Q_NULLPTR;

    // Energy Spectrum UI
    QLabel* ES_Title_sourceEP_QLabel = Q_NULLPTR;
    QPushButton* ExternalSingleEnergyspectrumFileLoadButton = Q_NULLPTR;
    QLabel* ES_FileName_sourceEP_QLabel = Q_NULLPTR;
    QTextEdit* sourceEP_EnergyspectrumFileNameText = Q_NULLPTR;
    QLabel* ES_Intensity_sourceEP_QLabel = Q_NULLPTR;
    QLineEdit* ES_Intensity_sourceEP_QLineEdit = Q_NULLPTR;
    QLabel* ES_List_sourceEP_QLabel = Q_NULLPTR;
    QPushButton* ExternalSingleEnergySpectrumAddButton = Q_NULLPTR;
    QListWidget* ES_List_sourceEP_QListWidget = Q_NULLPTR;


    // ============================================================
    // [주의] 아래 벡터들은 UI(버튼) 리스트이므로 Widget에 남깁니다!
    // ============================================================
    std::vector<QPushButton*> RI_ListDelete_sourceEP_QButton;
    std::vector<QPushButton*> RI_ListInfo_sourceEP_QButton;
    
    std::vector<QPushButton*> ES_ListDelete_sourceEP_QButton;
    std::vector<QPushButton*> ES_ListInfo_sourceEP_QButton;

    // [삭제됨] 아래 변수들은 Model로 이동했으므로 삭제합니다.
    // std::vector<std::map<int, QString>> ListInfo_sourceEP; 
    // int List_sourceEP_MakingIndex = 0; 

private slots:
    // --- 기능 함수들 ---
    void slot_SourceEP_SphereSize_Minus_ButtonClicked();
    void slot_SourceEP_SphereSize_Plus_ButtonClicked();
    void slot_ExternalPointSourceVisualization_ButtonClicked();
    void slot_sourceEP_Pick3D_ButtonClicked();
    
    // [참고] Update 버튼은 데이터 저장을 위해 this->updateModelFromUI()를 호출해야 함
    void slot_sourceEP_UpdatePosition_ButtonClicked(); 

    void slot_sourceEP_RadionuclideSelect_RadioButtonClicked();
    void slot_sourceEP_EnergySpectrumSelect_RadioButtonClicked();

    void slot_RIList_Add_sourceEP_ButtonClicked();
    void slot_RIList_Delete_sourceEP_ButtonClicked();
    void slot_RIList_Info_sourceEP_ButtonClicked();
    
    void slot_EnergyspectrumFileLoad_sourceEP_ButtonClicked();
    void slot_ESList_Add_sourceEP_ButtonClicked();
    void slot_ESList_Delete_sourceEP_ButtonClicked();
    void slot_ESList_Info_sourceEP_ButtonClicked();
};

//////////////////////////////////////////////////////////////////////
// Floor Disk Widget
//////////////////////////////////////////////////////////////////////
class FloorDiskWidget : public SourceGeometryWidget
{
    using ParentT = SourceGeometryWidget;
    Q_OBJECT

public:
    explicit FloorDiskWidget(QWidget* parent = Q_NULLPTR);
    virtual ~FloorDiskWidget();

    bool initialize() override;

    void WriteSourceMacro(std::ofstream& ofp_source) override;
    void WriteSourceInfo(std::ofstream& ofp_info) override;
    void ReadSourceInfo(std::ifstream& ifp) override;
    // [New] 모델 접근 함수
    FloorDiskModel& getModel() { return m_model; }
    void setModel(const FloorDiskModel& data); // 필요 시 구현

private:
    // [New] 데이터 저장소
    FloorDiskModel m_model;

    // [New] UI -> Model 동기화 함수 (Update 버튼 등에서 호출)
    void updateModelFromUI();


    // ============================================================
    // [UI Components] 화면 구성 요소 (그대로 유지)
    // ============================================================
    QGroupBox* m_sourceGeometryFloorDiskGroupBox = Q_NULLPTR;
    
    // Visualization
    QPushButton* m_FloorSourceVisualizationButton = Q_NULLPTR;

    // Geometry (Position & Radius)
    QLineEdit* PosX_SourceFD_QLineEdit = Q_NULLPTR;
    QLineEdit* PosY_SourceFD_QLineEdit = Q_NULLPTR;
    QLineEdit* PosZ_SourceFD_QLineEdit = Q_NULLPTR;
    QLineEdit* Radius_sourceFD_QLineEdit = Q_NULLPTR;
    
    // Update & Default Buttons
    QPushButton* m_FloorSetDefaultGeom = Q_NULLPTR;
    QPushButton* m_FloorUpdateButton = Q_NULLPTR;

    // Type Selection
    QRadioButton* RI_Select_sourceFD_QRadioButton = Q_NULLPTR;
    QRadioButton* ES_Select_sourceFD_QRadioButton = Q_NULLPTR;

    // Radionuclide UI
    QLabel* RI_Title_sourceFD_QLabel = Q_NULLPTR;
    QLineEdit* RI_Radionuclide_sourceFD_QLineEdit = Q_NULLPTR;
    QLabel* RI_Activity_sourceFD_QLabel = Q_NULLPTR;
    QLineEdit* RI_Activity_sourceFD_QLineEdit = Q_NULLPTR;
    QLabel* RI_List_sourceFD_QLabel = Q_NULLPTR;
    QPushButton* sourceFD_RadionuclideAddButton = Q_NULLPTR;
    QListWidget* RI_List_sourceFD_QListWidget = Q_NULLPTR;

    // Energy Spectrum UI
    QLabel* ES_Title_sourceFD_QLabel = Q_NULLPTR;
    QPushButton* sourceFD_EnergyspectrumFileLoadButton = Q_NULLPTR;
    QLabel* ES_FileName_sourceFD_QLabel = Q_NULLPTR;
    QTextEdit* sourceFD_EnergyspectrumFileNameText = Q_NULLPTR;
    QLabel* ES_Intensity_sourceFD_QLabel = Q_NULLPTR;
    QLineEdit* ES_Intensity_sourceFD_QLineEdit = Q_NULLPTR;
    QLabel* ES_List_sourceFD_QLabel = Q_NULLPTR;
    QPushButton* sourceFD_EnergySpectrumAddButton = Q_NULLPTR;
    QListWidget* ES_List_sourceFD_QListWidget = Q_NULLPTR;


    // ============================================================
    // [주의] 버튼 관리용 벡터는 UI 리스트이므로 Widget에 남깁니다!
    // ============================================================
    std::vector<QPushButton*> RI_ListDelete_sourceFD_QButton;
    std::vector<QPushButton*> RI_ListInfo_sourceFD_QButton;
    std::vector<QPushButton*> ES_ListDelete_sourceFD_QButton;
    std::vector<QPushButton*> ES_ListInfo_sourceFD_QButton;
    
    // [삭제됨] 아래 변수들은 Model(SourceModels.h)로 이동했습니다.
    // std::vector<std::map<int, QString>> ListInfo_sourceFD;
    // int List_sourceFD_MakingIndex = 0;

private slots:
    // --- Slot Functions (기존 유지) ---
    void slot_FloorSourceVisualization_ButtonClicked();
    void slot_FloorSetDefaultGeom_ButtonClicked();
    
    // [참고] 이 함수 내부에서 updateModelFromUI() 호출 필요
    void slot_FloorUpdate_ButtonClicked(); 
    
    void slot_sourceFD_RadionuclideSelect_RadioButtonClicked();
    void slot_sourceFD_EnergySpectrumSelect_RadioButtonClicked();

    // Radionuclide List Control
    void slot_RIList_Add_sourceFD_ButtonClicked();
    void slot_RIList_Delete_sourceFD_ButtonClicked();
    void slot_RIList_Info_sourceFD_ButtonClicked();

    // Energy Spectrum List Control
    void slot_sourceFD_EnergyspectrumFileLoad_ButtonClicked();
    void slot_ESList_Add_sourceFD_ButtonClicked();
    void slot_ESList_Delete_sourceFD_ButtonClicked();
    void slot_ESList_Info_sourceFD_ButtonClicked();
};

//////////////////////////////////////////////////////////////////////
// Object Volume (OV) Widget
//////////////////////////////////////////////////////////////////////

class ObjectVolumeWidget : public SourceGeometryWidget
{
    using ParentT = SourceGeometryWidget;
    Q_OBJECT

public:
    explicit ObjectVolumeWidget(QWidget* parent = Q_NULLPTR);
    virtual ~ObjectVolumeWidget();

    bool initialize() override;
    
    void WriteSourceMacro(std::ofstream& ofp_source) override;
    void WriteSourceInfo(std::ofstream& ofp_info) override;
    void ReadSourceInfo(std::ifstream& ifp) override;
    bool hasOV_Obejcts() const;
    // int getOV_SelectIndex();
    // void setOV_ObjectBound(int idx, double Xmin, double Xmax, double Ymin, double Ymax, double Zmin, double Zmax);
    // bool isOV_AddingSettingOKClicked();
    // void setOV_AddingSettingClosed();

    // [New] 모델 접근
    ObjectVolumeModel& getModel() { return m_model; }
    void setModel(const ObjectVolumeModel& data);

private:
    // [New] 데이터 저장소
    ObjectVolumeModel m_model;

    // [New] 동기화 함수
    void updateModelFromUI();

    // ============================================================
    // [UI Components] (그대로 유지)
    // ============================================================
    QGroupBox* m_sourceGeometryObjectVolumeGroupBox = Q_NULLPTR;
    QPushButton* m_ObjectVolumeSourceVisualizationButton = Q_NULLPTR;

    QHBoxLayout* layout_sourceOV_ObjectList = Q_NULLPTR;
    QScrollBar* sourceOV_ObjectScrollBar = Q_NULLPTR;
    QPushButton* sourceOV_ObjectAddButton = Q_NULLPTR;
    QPushButton* sourceOV_ObjectDeleteButton = Q_NULLPTR;

    QRadioButton* sourceOV_Radionuclide_radioButton = Q_NULLPTR;
    QRadioButton* sourceOV_Energyspectrum_radioButton = Q_NULLPTR;

    QLabel* RI_Title_sourceOV_QLabel = Q_NULLPTR;
    QLineEdit* RI_Radionuclide_sourceOV_QLineEdit = Q_NULLPTR;
    QLabel* RI_Activity_sourceOV_QLabel = Q_NULLPTR;
    QLineEdit* RI_Activity_sourceOV_QLineEdit = Q_NULLPTR;
    QLabel* RI_List_sourceOV_QLabel = Q_NULLPTR;
    QPushButton* sourceOV_RadionuclideAddButton = Q_NULLPTR;
    QListWidget* RI_List_sourceOV_QListWidget = Q_NULLPTR;

    QLabel* ES_Title_sourceOV_QLabel = Q_NULLPTR;
    QPushButton* sourceOV_EnergyspectrumFileLoadButton = Q_NULLPTR;
    QLabel* ES_FileName_sourceOV_QLabel = Q_NULLPTR;
    QTextEdit* sourceOV_EnergyspectrumFileNameText = Q_NULLPTR;
    QLabel* ES_Intensity_sourceOV_QLabel = Q_NULLPTR;
    QLineEdit* ES_Intensity_sourceOV_QLineEdit = Q_NULLPTR;
    QLabel* ES_List_sourceOV_QLabel = Q_NULLPTR;
    QPushButton* sourceOV_EnergyspectrumAddButton = Q_NULLPTR;
    QListWidget* ES_List_sourceOV_QListWidget = Q_NULLPTR;

    MultipleUIDialog* sourceOV_ObjectAddingDialog = Q_NULLPTR;
    QComboBox* m_comboBoxSetting_sourceOV_objectList = Q_NULLPTR;
    QPushButton* m_sourceOV_SettingOKButton = Q_NULLPTR;

    // ============================================================
    // [View Data] (UI 버튼 관리용 - Widget에 유지)
    // ============================================================
    std::map<int, QPushButton*> sourceOV_ObjectButton; // 버튼 포인터 맵

    std::vector<QPushButton*> RI_ListDelete_sourceOV_QButton;
    std::vector<QPushButton*> RI_ListInfo_sourceOV_QButton;
    std::vector<QPushButton*> ES_ListDelete_sourceOV_QButton;
    std::vector<QPushButton*> ES_ListInfo_sourceOV_QButton;

    // [삭제됨] 모델로 이동한 변수들 (기존 이름과 동일하므로 주석 처리 후 비교 가능)
    // std::vector<int> m_sourceOV_objectSequenceVector;
    // std::map<int, std::vector<double>> sourceOV_objectBound;
    // int Object_sourceOV_SelectedIndex = -1; 
    // std::map<int, std::vector<std::map<int, QString>>> ListInfo_sourceOV;
    // int List_sourceOV_MakingIndex = 0;
    // bool b_IsSourceLocationVisualized_sourceOV ...

private:
    void ExecuteSourceOVAddRoutines();
    void sourceOV_ObjectAddingSetting_Create();

private slots:
    void slot_ObjectVolumeSourceVisualization_ButtonClicked();
    void slot_sourceOV_ObjectAdd_ButtonClicked();
    void slot_sourceOV_ObjectDelete_ButtonClicked();
    void slot_sourceOV_ObjectSelect_ButtonClicked();
    void slot_sourceOV_RadionuclideSelect_RadioButtonClicked();
    void slot_sourceOV_EnergySpectrumSelect_RadioButtonClicked();
    void slot_sourceOV_ObjectSettingChangeCombo(int idx);
    void slot_sourceOV_SettingOK_ButtonClicked();
    void slot_sourceOV_RadionuclideAdd_ButtonClicked();
    void slot_sourceOV_RadionuclideDelete_ButtonClicked();
    void slot_sourceOV_RadionuclideInfo_ButtonClicked();
    void slot_sourceOV_EnergyspectrumFileLoad_ButtonClicked();
    void slot_ESList_Add_sourceOV_ButtonClicked();
    void slot_ESList_Delete_sourceOV_ButtonClicked();
    void slot_ESList_Info_sourceOV_ButtonClicked();
};

//////////////////////////////////////////////////////////////////////
// Phase Space (PS) Widget
//////////////////////////////////////////////////////////////////////
class PhaseSpaceWidget : public SourceGeometryWidget
{
    using ParentT = SourceGeometryWidget;
    Q_OBJECT

public:
    explicit PhaseSpaceWidget(QWidget* parent = Q_NULLPTR);
    virtual ~PhaseSpaceWidget();

    bool initialize() override;

    void WriteSourceMacro(std::ofstream& ofp_source) override;
    void WriteSourceInfo(std::ofstream& ofp_info) override;
    void ReadSourceInfo(std::ifstream& ifp) override;
    // [Moved] enum class PSMode는 SourceModels.h로 이동했습니다.

    // [New] 모델 접근 함수
    PhaseSpaceModel& getModel() { return m_model; }
    void setModel(const PhaseSpaceModel& data);

    // [Old] 호환성 유지용 (구현부에서 m_model.selectedMode를 리턴하게 수정)
    // PSMode getPS_mode() const;

private:
    // [New] 데이터 저장소
    PhaseSpaceModel m_model;

    // [New] UI -> Model 동기화 함수
    void updateModelFromUI();

    // ============================================================
    // [UI Components] (그대로 유지)
    // ============================================================
    QGroupBox* m_sourceGeometryPhaseSpaceGroupBox = Q_NULLPTR;
    
    // Visualization
    QPushButton* SourceVisualziation_sourePS_QButton = Q_NULLPTR;
    QCheckBox* DirectionCheck_sourcePS_QCheckBox = Q_NULLPTR;

    // Format Selection Buttons
    QPushButton* sourcePS_MCNP_button = Q_NULLPTR;
    QPushButton* sourcePS_PHITS_button = Q_NULLPTR;
    QPushButton* sourcePS_FLUKA_button = Q_NULLPTR;
    QPushButton* sourcePS_IAEA_button = Q_NULLPTR;
    QPushButton* sourcePS_USER_Button = Q_NULLPTR;

    // File Loading UI
    QPushButton* sourcePS_loadPSFButton = Q_NULLPTR;
    QTextEdit* sourcePS_PSFname = Q_NULLPTR;

private slots:
    // --- Slot Functions ---
    void slot_PhaseSpaceSourceVisualization_ButtonClicked();
    void slot_DirectionCheck_sourcePS_QCheckBox_ButtonClicked();

    // Format Selection
    void slot_SourcePS_MCNP_ButtonClicked();
    void slot_SourcePS_PHITS_ButtonClicked();
    void slot_SourcePS_FLUKA_ButtonClicked();
    void slot_SourcePS_IAEA_ButtonClicked();
    void slot_SourcePS_USER_ButtonClicked();

    // File Operation
    void slot_sourcePS_loadPSF_ButtonClicked();
};

//////////////////////////////////////////////////////////////////////
// Hot Particle (HP) Widget
//////////////////////////////////////////////////////////////////////
class HotParticleWidget : public SourceGeometryWidget
{
    using ParentT = SourceGeometryWidget;
    Q_OBJECT

public:
    explicit HotParticleWidget(QWidget* parent = Q_NULLPTR);
    virtual ~HotParticleWidget();

    bool initialize() override;

    void WriteSourceMacro(std::ofstream& ofp_source) override;
    void WriteSourceInfo(std::ofstream& ofp_info) override;
    void ReadSourceInfo(std::ifstream& ifp) override;
    
    // // [기존 함수] 외부에서 3D 포인팅 시 호출됨
    void updateHP_Vector(double x, double y, double z);
    void updateHP_Points(double x, double y, double z);

    // [New] 모델 접근
    HotParticleModel& getModel() { return m_model; }
    void setModel(const HotParticleModel& data);

private:
    // [New] 데이터 저장소
    HotParticleModel m_model;

    // [New] 동기화 함수
    void updateModelFromUI();

    // ============================================================
    // [UI Components] (그대로 유지)
    // ============================================================
    QGroupBox* m_sourceGeometryHotParticlePointGroupBox = Q_NULLPTR;

    QPushButton* SourceHP_SphereSize_Minus_QPushButton = Q_NULLPTR;
    QPushButton* SourceHP_SphereSize_Plus_QPushButton = Q_NULLPTR;
    QPushButton* m_HotParticleSourceVisualizationButton = Q_NULLPTR;

    QLineEdit* PosX_SourceHP_QLineEdit = Q_NULLPTR;
    QLineEdit* PosY_SourceHP_QLineEdit = Q_NULLPTR;
    QLineEdit* PosZ_SourceHP_QLineEdit = Q_NULLPTR;
    QLineEdit* SeparationDist_SourceHP_QLineEdit = Q_NULLPTR;
    
    QPushButton* Pick3D_HP_QButton = Q_NULLPTR;
    QPushButton* UpdatePosition_sourceHP_QButton = Q_NULLPTR;

    QRadioButton* RI_Select_sourceHP_QRadioButton = Q_NULLPTR;
    QRadioButton* ES_Select_sourceHP_QRadioButton = Q_NULLPTR;

    QLabel* RI_Title_sourceHP_QLabel = Q_NULLPTR;
    QLineEdit* RI_Radionuclide_sourceHP_QLineEdit = Q_NULLPTR;
    QLabel* RI_Activity_sourceHP_QLabel = Q_NULLPTR;
    QLineEdit* RI_Activity_sourceHP_QLineEdit = Q_NULLPTR;
    QLabel* RI_List_sourceHP_QLabel = Q_NULLPTR;
    QPushButton* RIList_Add_sourceHP_QButton = Q_NULLPTR;
    QListWidget* RI_List_sourceHP_QListWidget = Q_NULLPTR;

    QLabel* ES_Title_sourceHP_QLabel = Q_NULLPTR;
    QPushButton* sourceHP_EnergyspectrumFileLoadButton = Q_NULLPTR;
    QLabel* ES_FileName_sourceHP_QLabel = Q_NULLPTR;
    QTextEdit* sourceHP_EnergyspectrumFileNameText = Q_NULLPTR;
    QLabel* ES_Intensity_sourceHP_QLabel = Q_NULLPTR;
    QLineEdit* ES_Intensity_sourceHP_QLineEdit = Q_NULLPTR;
    QLabel* ES_List_sourceHP_QLabel = Q_NULLPTR;
    QPushButton* ESList_Add_sourceHP_QButton = Q_NULLPTR;
    QListWidget* ES_List_sourceHP_QListWidget = Q_NULLPTR;

    // ============================================================
    // [UI Lists] 버튼 관리 벡터는 Widget에 남김
    // ============================================================
    std::vector<QPushButton*> RIList_Delete_sourceHP_QButton;
    std::vector<QPushButton*> RIList_Info_sourceHP_QButton;
    std::vector<QPushButton*> ESList_Delete_sourceHP_QButton;
    std::vector<QPushButton*> ESList_Info_sourceHP_QButton;

    // [삭제됨] 모델로 이동한 변수들
    // ListInfo_sourceHP, RIList_sourceHP_MakingIndex, SelectedPointNormalVector_sourceHP

private slots:
    // --- Slot Functions ---
    void slot_SourceHP_SphereSize_Minus_ButtonClicked();
    void slot_SourceHP_SphereSize_Plus_ButtonClicked();
    void slot_HotParticleSourceVisualization_ButtonClicked();

    void slot_HotParticle_Pick3D_ButtonClicked();
    void slot_Update_Position_HP_ButtonClicked(); // Update 버튼

    void slot_RI_Select_sourceHP_RadioButtonClicked();
    void slot_ES_Select_sourceHP_RadioButtonClicked();

    void slot_RIList_Add_sourceHP_ButtonClicked();
    void slot_RIList_Delete_sourceHP_ButtonClicked();
    void slot_RIList_Info_sourceHP_ButtonClicked();

    void slot_EnergyspectrumFileLoad_sourceHP_ButtonClicked();
    void slot_ESList_Add_sourceHP_ButtonClicked();
    void slot_ESList_Delete_sourceHP_ButtonClicked();
    void slot_ESList_Info_sourceHP_ButtonClicked();
};

//////////////////////////////////////////////////////////////////////
// Cone Beam (CB) Widget
//////////////////////////////////////////////////////////////////////
class ConeBeamWidget : public SourceGeometryWidget
{
    using ParentT = SourceGeometryWidget;
    Q_OBJECT

public:
    explicit ConeBeamWidget(QWidget* parent = Q_NULLPTR);
    virtual ~ConeBeamWidget();

    bool initialize() override;
    
    void WriteSourceMacro(std::ofstream& ofp_source) override;
    void WriteSourceInfo(std::ofstream& ofp_info) override;
    void ReadSourceInfo(std::ifstream& ifp) override;
    // [기존 함수]
    void updateCB_Points(double x, double y, double z);
    
    // [수정] 구조체 정의는 SourceModels.h로 이동했으므로 리턴 타입만 유지
    ConeBeamParams getCB_Params();

    // [New] 모델 접근
    ConeBeamModel& getModel() { return m_model; }
    void setModel(const ConeBeamModel& data);

private:
    // [New] 데이터 저장소
    ConeBeamModel m_model;

    // [New] 동기화 함수
    void updateModelFromUI();

    // ============================================================
    // [UI Components] (그대로 유지)
    // ============================================================
    QGroupBox* m_sourceGeometryConeBeamGroupBox = Q_NULLPTR;

    // Visualization
    QPushButton* m_ConeBeamSourceVisualizationButton = Q_NULLPTR;
    QCheckBox* sourceCB_dirCheckBox = Q_NULLPTR;

    // Geometry: Position
    QLineEdit* m_lineEditConeBeamPointX = Q_NULLPTR;
    QLineEdit* m_lineEditConeBeamPointY = Q_NULLPTR;
    QLineEdit* m_lineEditConeBeamPointZ = Q_NULLPTR;

    // Geometry: Direction
    QLineEdit* m_lineEditConeBeamDirectionX = Q_NULLPTR;
    QLineEdit* m_lineEditConeBeamDirectionY = Q_NULLPTR;
    QLineEdit* m_lineEditConeBeamDirectionZ = Q_NULLPTR;

    // Geometry: Solid Angle
    QLineEdit* m_lineEditConeBeamDirectionSolidAngle = Q_NULLPTR;

    // Action Buttons
    QPushButton* pickConeBeam3DButton = Q_NULLPTR;
    QPushButton* sourceCB_UpdateButton = Q_NULLPTR;

    // Type Selection
    QRadioButton* sourceCB_Radionuclide_radioButton = Q_NULLPTR;
    QRadioButton* sourceCB_Energyspectrum_radioButton = Q_NULLPTR;

    // Radionuclide UI
    QLabel* labelConeBeamRadionuclideTitle = Q_NULLPTR;
    QLineEdit* m_lineEditConeBeamRadionuclide = Q_NULLPTR;
    QLabel* labelConeBeamRadionuclideActivity = Q_NULLPTR;
    QLineEdit* m_lineEditConeBeamActivity = Q_NULLPTR;
    QLabel* labelConeBeamRadionuclideList = Q_NULLPTR;
    QPushButton* ConeBeamRadionuclideAddButton = Q_NULLPTR;
    QListWidget* RI_List_sourceCB_QListWidget = Q_NULLPTR;

    // Energy Spectrum UI
    QLabel* labelConeBeamEnergyspectrumTitle = Q_NULLPTR;
    QPushButton* ConeBeamEnergyspectrumFileLoadButton = Q_NULLPTR;
    QLabel* labelConeBeamEnergyspectrumFileName = Q_NULLPTR;
    QTextEdit* sourceCB_EnergyspectrumFileNameText = Q_NULLPTR; 
    QLabel* labelConeBeamEnergyspectrumIntensity = Q_NULLPTR;
    QLineEdit* m_lineEditConeBeamIntensity = Q_NULLPTR;
    QLabel* labelConeBeamEnergyspectrumList = Q_NULLPTR;
    QPushButton* ConeBeamEnergyspectrumAddButton = Q_NULLPTR;
    QListWidget* ES_List_sourceCB_QListWidget = Q_NULLPTR;

    // ============================================================
    // [View Data] UI 버튼 관리 리스트는 Widget에 남김
    // ============================================================
    std::vector<QPushButton*> m_sourceCB_listDeleteButton;
    std::vector<QPushButton*> m_sourceCB_listInfoButton;
    
    // [New] ES 쪽 버튼 리스트가 기존 코드엔 없었지만, 필요하다면 여기에 추가
    // std::vector<QPushButton*> m_sourceCB_ES_listDeleteButton; ...

    // [삭제됨] 모델로 이동한 변수들
    // sourceCB_info, RIList_sourceCB_MakingIndex, EnergyspectrumListTotalCount_CB, b_IsSourceLocationVisualized_sourceCB

private slots:
    // --- Slot Functions ---
    void slot_ConeBeamSourceVisualization_ButtonClicked();
    
    void slot_ConeBeam_Pick3D_ButtonClicked();
    void slot_ConeBeamUpdate_ButtonClicked(); // Update 버튼
    void slot_CBreset_ButtonClicked();

    void slot_ConeBeamRadionuclide_ButtonClicked();
    void slot_ConeBeamEnergyspectrum_ButtonClicked();

    void slot_ConeBeamRadionuclideAdd_ButtonClicked();
    void slot_ConeBeamRadionuclideDelete_ButtonClicked(); 
    void slot_ConeBeamRadionuclideInfo_ButtonClicked();

    void slot_ConeBeamEnergyspectrumFileLoad_ButtonClicked();
    void slot_ConeBeamEnergyspectrumAdd_ButtonClicked();
    void slot_ConeBeamEnergyspectrumDelete_ButtonClicked(); 
    void slot_ConeBeamEnergyspectrumInfo_ButtonClicked();
};

//////////////////////////////////////////////////////////////////////
// Room Contamination (RC) Widget
//////////////////////////////////////////////////////////////////////
class RoomContaminationWidget : public SourceGeometryWidget
{
    using ParentT = SourceGeometryWidget;
    Q_OBJECT

public:
    explicit RoomContaminationWidget(QWidget* parent = Q_NULLPTR);
    virtual ~RoomContaminationWidget();

    bool initialize() override;

    void WriteSourceMacro(std::ofstream& ofp_source) override;
    void WriteSourceInfo(std::ofstream& ofp_info) override;
    void ReadSourceInfo(std::ifstream& ifp) override;
    // [New] 모델 접근
    RoomContaminationModel& getModel() { return m_model; }
    void setModel(const RoomContaminationModel& data);

private:
    // [New] 데이터 저장소
    RoomContaminationModel m_model;

    // [New] 동기화 함수
    void updateModelFromUI();

    // ============================================================
    // [UI Components] (그대로 유지)
    // ============================================================
    QGroupBox* m_sourceGeometryRoomContaminationGroupBox = Q_NULLPTR;
    QPushButton* m_RoomContaminationSourceVisualizationButton = Q_NULLPTR;

    // Geometry: Box Dimensions
    QLabel* label_sourceRC_BoxLength = Q_NULLPTR;
    QLineEdit* Box_HalfLengthX_sourceRC_QLineEdit = Q_NULLPTR;
    QLineEdit* Box_HalfLengthY_sourceRC_QLineEdit = Q_NULLPTR;
    QLineEdit* Box_HalfLengthZ_sourceRC_QLineEdit = Q_NULLPTR;

    // Geometry: Box Center
    QLabel* label_sourceRC_BoxCenter = Q_NULLPTR;
    QLineEdit* Box_CenterX_sourceRC_QLineEdit = Q_NULLPTR;
    QLineEdit* Box_CenterY_sourceRC_QLineEdit = Q_NULLPTR;
    QLineEdit* Box_CenterZ_sourceRC_QLineEdit = Q_NULLPTR;

    // Action Button
    QPushButton* UpdateRCposButton = Q_NULLPTR;

    // Type Selection
    QRadioButton* RI_Select_sourceRC_QRadioButton = Q_NULLPTR;
    QRadioButton* ES_Select_sourceRC_QRadioButton = Q_NULLPTR;

    // Radionuclide UI
    QLineEdit* RI_sourceRC_QLineEdit = Q_NULLPTR;       // Name
    QLineEdit* Activity_sourceRC_QLineEdit = Q_NULLPTR; // Activity
    QLabel* RadionuclideList_sourceRC_QLabel = Q_NULLPTR;
    QPushButton* RIList_Add_sourceRC_QButton = Q_NULLPTR;
    QListWidget* RI_List_sourceRC_QListWidget = Q_NULLPTR;

    // ============================================================
    // [View Data] UI 버튼 관리용 (Widget에 유지)
    // ============================================================
    std::vector<QPushButton*> RIList_Delete_sourceRC_QButton;
    std::vector<QPushButton*> RIList_Info_sourceRC_QButton;

    // [삭제됨] 모델로 이동한 변수들
    // RI_Info_sourceRC, RIList_sourceRC_MakingIndex

private slots:
    void slot_RoomContaminationSourceVisualization_ButtonClicked();
    void slot_UpdateRCpos_ButtonClicked();
    void slot_RI_Select_sourceRC_RadioButtonClicked();
    void slot_ES_Select_sourceRC_RadioButtonClicked();
    void slot_RIList_Add_sourceRC_ButtonClicked();
    void slot_RIList_Delete_sourceRC_ButtonClicked();
    void slot_RIList_Info_sourceRC_ButtonClicked();
};

//////////////////////////////////////////////////////////////////////
// Environmental Contamination (EC) Widget
//////////////////////////////////////////////////////////////////////
class EnvironmentalContaminationWidget : public SourceGeometryWidget
{
    using ParentT = SourceGeometryWidget;
    Q_OBJECT

public:
    explicit EnvironmentalContaminationWidget(QWidget* parent = Q_NULLPTR);
    virtual ~EnvironmentalContaminationWidget();

    bool initialize() override;

    void WriteSourceMacro(std::ofstream& ofp_source) override;
    void WriteSourceInfo(std::ofstream& ofp_info) override;
    void ReadSourceInfo(std::ifstream& ifp) override;
    // [New] 모델 접근
    EnvironmentalContaminationModel& getModel() { return m_model; }
    void setModel(const EnvironmentalContaminationModel& data);

private:
    // [New] 데이터 저장소
    EnvironmentalContaminationModel m_model;

    // [New] 동기화 함수
    void updateModelFromUI();

    // ============================================================
    // [UI Components] (그대로 유지)
    // ============================================================
    
    // Visualization
    QPushButton* m_EnvironmentalSourceVisualizationButton = Q_NULLPTR;

    // Type Selection
    QComboBox* m_comboBoxEnvironmentalSourceType = Q_NULLPTR;

    // Depth Input
    QLabel* labelEnvironmentalSoilDepth = Q_NULLPTR;
    QLineEdit* m_lineEditEnvironmentalSoilDepth = Q_NULLPTR;

    // Radionuclide Input
    QLabel* labelEnvironmentalRadionuclideTitle = Q_NULLPTR;
    QLineEdit* m_lineEditEnvironmentalRadionuclide = Q_NULLPTR;
    
    // Activity Input
    QLabel* labelEnvironmentalRadionuclideActivity = Q_NULLPTR;
    QLineEdit* m_lineEditEnvironmentalActivity = Q_NULLPTR;

    // Add Button & List
    QLabel* labelEnvironmentalRadionuclideList = Q_NULLPTR;
    QPushButton* EnvironmentalRadionuclideAddButton = Q_NULLPTR;
    QListWidget* listWidget_E_Radionuclide = Q_NULLPTR;
    
    // [참고] 만약 삭제 버튼 등을 관리하는 벡터가 필요하다면 여기에 추가
    // std::vector<QPushButton*> m_listDeleteButtons;

private slots:
    // --- Slot Functions ---
    void slot_EnvironmentalSourceVisualization_ButtonClicked();
    void slot_ChangeEnvironmentalSourceType(int idx);
    void slot_EnvironmentalRadionuclideAdd_ButtonClicked();
    void slot_Ereset_ButtonClicked();
};

//////////////////////////////////////////////////////////////////////
// Parallel Beam (PB) Widget
//////////////////////////////////////////////////////////////////////
class ParallelBeamWidget : public SourceGeometryWidget
{
    using ParentT = SourceGeometryWidget;
    Q_OBJECT

public:
    explicit ParallelBeamWidget(QWidget* parent = Q_NULLPTR);
    virtual ~ParallelBeamWidget();

    bool initialize() override;

    void WriteSourceMacro(std::ofstream& ofp_source) override;
    void WriteSourceInfo(std::ofstream& ofp_info) override;
    void ReadSourceInfo(std::ifstream& ifp) override;
    // [모델 접근]
    ParallelBeamModel& getModel() { return m_model; }
    void setModel(const ParallelBeamModel& data);

private:
    // [데이터 저장소]
    ParallelBeamModel m_model;

    // [동기화 함수]
    void updateModelFromUI();

    // ============================================================
    // [UI Components]
    // ============================================================
    QGroupBox* m_sourceGeometryParallelBeamGroupBox = Q_NULLPTR;

    // Visualization
    QPushButton* m_ParallelBeamSourceVisualizationButton = Q_NULLPTR;
    
    // [오타 수정] sourceCB_dirCheckBox -> sourcePB_dirCheckBox (ConeBeam 복붙 흔적 수정)
    QCheckBox* sourcePB_dirCheckBox = Q_NULLPTR; 

    // Geometry: Center Position
    QLineEdit* m_lineEditParallelBeamPointX = Q_NULLPTR;
    QLineEdit* m_lineEditParallelBeamPointY = Q_NULLPTR;
    QLineEdit* m_lineEditParallelBeamPointZ = Q_NULLPTR;

    // Geometry: Dimensions
    QLineEdit* m_lineEditParallelBeamRadius = Q_NULLPTR;

    // Geometry: Direction
    QLineEdit* m_lineEditParallelBeamDirectionTheta = Q_NULLPTR;
    QLineEdit* m_lineEditParallelBeamDirectionPhi = Q_NULLPTR;

    // Action Buttons
    QPushButton* pickParallelBeam3DButton = Q_NULLPTR;
    QPushButton* sourcePB_UpdateButton = Q_NULLPTR;

    // Type Selection
    QRadioButton* sourcePB_MonoEnergy_radioButton = Q_NULLPTR;
    QRadioButton* sourcePB_EnergySpectrum_radioButton = Q_NULLPTR;

    // Mono Energy UI
    QLabel* labelParallelBeamParticleTypeTitle = Q_NULLPTR;
    QComboBox* m_comboBoxParallelBeamParticleType = Q_NULLPTR;
    QLabel* labelParallelBeamEnergyTitle = Q_NULLPTR;
    QLineEdit* m_lineEditParallelBeamEnergy = Q_NULLPTR;
    QLabel* labelParallelBeamIntensityTitle = Q_NULLPTR;
    QLineEdit* m_lineEditParallelBeamIntensity_MonoEnergy = Q_NULLPTR;

    // Energy Spectrum UI
    QLabel* labelParallelBeamEnergyspectrumTitle = Q_NULLPTR;
    QPushButton* ParallelBeamEnergyspectrumFileLoadButton = Q_NULLPTR;
    QLabel* labelParallelBeamEnergyspectrumFileName = Q_NULLPTR;
    QTextEdit* sourcePB_EnergyspectrumFileNameText = Q_NULLPTR;
    QLabel* labelParallelBeamEnergyspectrumIntensity = Q_NULLPTR;
    QLineEdit* m_lineEditParallelBeamIntensity_EnergySpectrum = Q_NULLPTR;
    QLabel* labelParallelBeamEnergyspectrumList = Q_NULLPTR;
    QPushButton* ParallelBeamEnergyspectrumAddButton = Q_NULLPTR;
    QListWidget* ES_List_sourcePB_QListWidget = Q_NULLPTR;

    // ============================================================
    // [View Data] 버튼 관리 리스트는 유지
    // ============================================================
    std::vector<QPushButton*> m_sourcePB_ES_listDeleteButton;
    std::vector<QPushButton*> m_sourcePB_ES_listInfoButton;

    // [삭제됨] 모델로 이동
    // b_IsSourceLocationVisualized_sourcePB (모델의 isVisualized 사용 추천)

private slots:
    void slot_ParallelBeamSourceVisualization_ButtonClicked();
    void slot_ParallelBeam_Pick3D_ButtonClicked();
    void slot_ParallelBeamUpdate_ButtonClicked();
    void slot_PB_MonoEnergy_RadioButtonClicked();
    void slot_PB_EnergySpectrum_RadioButtonClicked();
    void slot_ParallelBeamEnergyspectrumFileLoad_ButtonClicked();
    void slot_ParallelBeamEnergyspectrumAdd_ButtonClicked();
    void slot_ParallelBeamEnergyspectrumDelete_ButtonClicked();
    void slot_ParallelBeamEnergyspectrumInfo_ButtonClicked();
};
//////////////////////////////////////////////////////////////////////
class SourceGeometryWidgetFactory
{
public:
	static SourceGeometryWidget* CreateSourceGeometry(const std::string& name);
};

