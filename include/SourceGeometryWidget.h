#pragma once

#include <QGroupBox>
#include <QFont>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QVBoxLayout>


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


private:
	QPushButton* m_BroadBeamSourceVisualizationButton = Q_NULLPTR;
	QComboBox* m_comboBoxBeamdirection = Q_NULLPTR;
	QComboBox* m_comboBoxBroadBeamParticleType = Q_NULLPTR;
	QLineEdit* m_lineBroadBeamEnergy = Q_NULLPTR;

	QGroupBox* m_AziPolGroupBox = Q_NULLPTR;
	QLineEdit* m_lineEditAzimuthalAngle = Q_NULLPTR;
	QLineEdit* m_lineEditPolarAngle = Q_NULLPTR;

private:
	void CreateAziPol(int idx, QVBoxLayout*);
	void SetBroadBeamAziPol(int user);

public slots:
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

private:
    // --- UI Components (기존 코드에서 추출) ---
    QGroupBox* m_sourceGeometryExternalPointGroupBox = Q_NULLPTR;
    QPushButton* SourceEP_SphereSize_Minus_QPushButton = Q_NULLPTR;
    QPushButton* SourceEP_SphereSize_Plus_QPushButton = Q_NULLPTR;
    QPushButton* m_ExternalPointSourceVisualizationButton = Q_NULLPTR;
    
    QLineEdit* PosX_sourceEP_QLineEdit = Q_NULLPTR;
    QLineEdit* PosY_sourceEP_QLineEdit = Q_NULLPTR;
    QLineEdit* PosZ_sourceEP_QLineEdit = Q_NULLPTR;
    
    QPushButton* pickExternal3DButton = Q_NULLPTR;
    QPushButton* sourceEP_UpdatePositionButton = Q_NULLPTR;
    
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

    // --- Data & Logic Variables (패널에서 가져옴) ---
    // 리스트 관리를 위한 벡터들입니다.
    std::vector<std::map<int, QString>> ListInfo_sourceEP; // ListInfo_sourceEP[pointID][0~5] = 0: 사용여부(True or ""), 1: posX, 2: posY, 3: posZ, 4: RI, 5: activity(Bq)		
    std::vector<QPushButton*> RI_ListDelete_sourceEP_QButton;
    std::vector<QPushButton*> RI_ListInfo_sourceEP_QButton;
    std::vector<QPushButton*> ES_ListDelete_sourceEP_QButton;
    std::vector<QPushButton*> ES_ListInfo_sourceEP_QButton;
    int List_sourceEP_MakingIndex = 0; // radionulcide에서 현재 입력한 핵종 개수	

private slots:
    // --- 기능 함수들 (기존 Panel의 Slot 함수들) ---
    void slot_SourceEP_SphereSize_Minus_ButtonClicked();
    void slot_SourceEP_SphereSize_Plus_ButtonClicked();
    void slot_ExternalPointSourceVisualization_ButtonClicked();
    void slot_sourceEP_Pick3D_ButtonClicked();
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

private:
    // --- UI Components (Main GroupBox) ---
    QGroupBox* m_sourceGeometryFloorDiskGroupBox = Q_NULLPTR;
    
    // --- Visualization ---
    QPushButton* m_FloorSourceVisualizationButton = Q_NULLPTR;

    // --- Geometry (Position & Radius) ---
    QLineEdit* PosX_SourceFD_QLineEdit = Q_NULLPTR;
    QLineEdit* PosY_SourceFD_QLineEdit = Q_NULLPTR;
    QLineEdit* PosZ_SourceFD_QLineEdit = Q_NULLPTR;
    QLineEdit* Radius_sourceFD_QLineEdit = Q_NULLPTR;
    
    // --- Update & Default Buttons ---
    QPushButton* m_FloorSetDefaultGeom = Q_NULLPTR;
    QPushButton* m_FloorUpdateButton = Q_NULLPTR;

    // --- Type Selection (Radionuclide vs Energy Spectrum) ---
    QRadioButton* RI_Select_sourceFD_QRadioButton = Q_NULLPTR;
    QRadioButton* ES_Select_sourceFD_QRadioButton = Q_NULLPTR;

    // --- Radionuclide UI ---
    QLabel* RI_Title_sourceFD_QLabel = Q_NULLPTR;
    QLineEdit* RI_Radionuclide_sourceFD_QLineEdit = Q_NULLPTR;
    QLabel* RI_Activity_sourceFD_QLabel = Q_NULLPTR;
    QLineEdit* RI_Activity_sourceFD_QLineEdit = Q_NULLPTR;
    QLabel* RI_List_sourceFD_QLabel = Q_NULLPTR;
    QPushButton* sourceFD_RadionuclideAddButton = Q_NULLPTR;
    QListWidget* RI_List_sourceFD_QListWidget = Q_NULLPTR;

    // --- Energy Spectrum UI ---
    QLabel* ES_Title_sourceFD_QLabel = Q_NULLPTR;
    QPushButton* sourceFD_EnergyspectrumFileLoadButton = Q_NULLPTR;
    QLabel* ES_FileName_sourceFD_QLabel = Q_NULLPTR;
    QTextEdit* sourceFD_EnergyspectrumFileNameText = Q_NULLPTR;
    QLabel* ES_Intensity_sourceFD_QLabel = Q_NULLPTR;
    QLineEdit* ES_Intensity_sourceFD_QLineEdit = Q_NULLPTR;
    QLabel* ES_List_sourceFD_QLabel = Q_NULLPTR;
    QPushButton* sourceFD_EnergySpectrumAddButton = Q_NULLPTR;
    QListWidget* ES_List_sourceFD_QListWidget = Q_NULLPTR;

    // --- Data Management (리스트 관리용 변수들) ---
    // FunctionPanelRight에 있던 ListInfo 및 관리용 벡터들을 멤버로 가져옵니다.
    std::vector<std::map<int, QString>> ListInfo_sourceFD; // RI_Info_sourceRC[pointID][0~2] = 0: 사용여부(True or ""), 1: RI, 2: activity(Bq/cm3)
    std::vector<QPushButton*> RI_ListDelete_sourceFD_QButton;
    std::vector<QPushButton*> RI_ListInfo_sourceFD_QButton;
    std::vector<QPushButton*> ES_ListDelete_sourceFD_QButton;
    std::vector<QPushButton*> ES_ListInfo_sourceFD_QButton;
    int List_sourceFD_MakingIndex = 0; // radionulcide에서 현재 입력한 핵종 개수	

private slots:
    // --- Slot Functions (기존 FunctionPanelRight의 함수들) ---
    void slot_FloorSourceVisualization_ButtonClicked();
    void slot_FloorSetDefaultGeom_ButtonClicked();
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

private:
    // --- UI Components: Main GroupBox ---
    QGroupBox* m_sourceGeometryObjectVolumeGroupBox = Q_NULLPTR;
    
    // --- Visualization ---
    QPushButton* m_ObjectVolumeSourceVisualizationButton = Q_NULLPTR;

    // --- Object List Area (Scroll Area) ---
    QHBoxLayout* layout_sourceOV_ObjectList = Q_NULLPTR;
    QScrollBar* sourceOV_ObjectScrollBar = Q_NULLPTR;
    QPushButton* sourceOV_ObjectAddButton = Q_NULLPTR;
    QPushButton* sourceOV_ObjectDeleteButton = Q_NULLPTR;

    // --- Type Selection ---
    QRadioButton* sourceOV_Radionuclide_radioButton = Q_NULLPTR;
    QRadioButton* sourceOV_Energyspectrum_radioButton = Q_NULLPTR;

    // --- Radionuclide UI ---
    QLabel* RI_Title_sourceOV_QLabel = Q_NULLPTR;
    QLineEdit* RI_Radionuclide_sourceOV_QLineEdit = Q_NULLPTR;
    QLabel* RI_Activity_sourceOV_QLabel = Q_NULLPTR;
    QLineEdit* RI_Activity_sourceOV_QLineEdit = Q_NULLPTR;
    QLabel* RI_List_sourceOV_QLabel = Q_NULLPTR;
    QPushButton* sourceOV_RadionuclideAddButton = Q_NULLPTR;
    QListWidget* RI_List_sourceOV_QListWidget = Q_NULLPTR;

    // --- Energy Spectrum UI ---
    QLabel* ES_Title_sourceOV_QLabel = Q_NULLPTR;
    QPushButton* sourceOV_EnergyspectrumFileLoadButton = Q_NULLPTR;
    QLabel* ES_FileName_sourceOV_QLabel = Q_NULLPTR;
    QTextEdit* sourceOV_EnergyspectrumFileNameText = Q_NULLPTR;
    QLabel* ES_Intensity_sourceOV_QLabel = Q_NULLPTR;
    QLineEdit* ES_Intensity_sourceOV_QLineEdit = Q_NULLPTR;
    QLabel* ES_List_sourceOV_QLabel = Q_NULLPTR;
    QPushButton* sourceOV_EnergyspectrumAddButton = Q_NULLPTR;
    QListWidget* ES_List_sourceOV_QListWidget = Q_NULLPTR;

    // --- Modal Dialog UI (Object Adding Setting) ---
    // 다이얼로그는 멤버로 가지고 있어야 제어가 가능합니다.
    MultipleUIDialog* sourceOV_ObjectAddingDialog = Q_NULLPTR;
    QComboBox* m_comboBoxSetting_sourceOV_objectList = Q_NULLPTR;
    QPushButton* m_sourceOV_SettingOKButton = Q_NULLPTR;

    // --- Data Management (중요: 복잡한 데이터 구조 이관) ---
    // 1. 선택된 Object 관리
    std::vector<int> m_sourceOV_objectSequenceVector; // 추가된 오브젝트 ID 목록 // m_sourceOV_objectSequenceVector[ID] = Object ID
    std::map<int, QPushButton*> sourceOV_ObjectButton; // 오브젝트별 버튼 관리
    std::map<int, std::vector<double>> sourceOV_objectBound; // 오브젝트 바운딩 박스 정보 sourceOV_objectBound[ID][0~5] = 0: Xmin, 1: Xmax, 2: Ymin, 3: Ymax, 4: Zmin, 5: Zmax

    // 2. 오브젝트별 소스 리스트 관리 (Map <ObjID, Vector <SourceInfo Map>>)
    std::map<int, std::vector<std::map<int, QString>>> ListInfo_sourceOV; // RI_Info_sourceOV[objectID][RIindex][1~2] = 0: 사용여부, 1: RI, 2: activity(Bq/cm3)
    int List_sourceOV_MakingIndex = 0; // radionulcide에서 현재 입력한 핵종 개수

    // 3. 리스트 아이템별 버튼 관리
    std::vector<QPushButton*> RI_ListDelete_sourceOV_QButton;
    std::vector<QPushButton*> RI_ListInfo_sourceOV_QButton;
    std::vector<QPushButton*> ES_ListDelete_sourceOV_QButton;
    std::vector<QPushButton*> ES_ListInfo_sourceOV_QButton;

    // 4. 상태 플래그
    bool b_IsSourceLocationVisualized_sourceOV = false;
    bool b_IsSourceOV_AddingSettingClosed = false;
    bool b_IsSourceOV_AddingSettingOKClicked = false;
    bool b_sourceOV_InObjectSelect = false;

private:
    // --- Helper Functions (Private Logic) ---
    void ExecuteSourceOVAddRoutines();
    void sourceOV_ObjectAddingSetting_Create();

private slots:
    // --- Slot Functions ---
    void slot_ObjectVolumeSourceVisualization_ButtonClicked();
    
    // Object List Management
    void slot_sourceOV_ObjectAdd_ButtonClicked();
    void slot_sourceOV_ObjectDelete_ButtonClicked(); // 기존 코드에 구현부 빈 상태, 확인 필요
    void slot_sourceOV_ObjectSelect_ButtonClicked();

    // Type Selection
    void slot_sourceOV_RadionuclideSelect_RadioButtonClicked();
    void slot_sourceOV_EnergySpectrumSelect_RadioButtonClicked();

    // Dialog Interaction
    void slot_sourceOV_ObjectSettingChangeCombo(int idx);
    void slot_sourceOV_SettingOK_ButtonClicked();

    // Radionuclide Operations
    void slot_sourceOV_RadionuclideAdd_ButtonClicked();
    void slot_sourceOV_RadionuclideDelete_ButtonClicked();
    void slot_sourceOV_RadionuclideInfo_ButtonClicked();

    // Energy Spectrum Operations
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

private:
    // --- UI Components: Main GroupBox ---
    QGroupBox* m_sourceGeometryPhaseSpaceGroupBox = Q_NULLPTR;
    
    // --- Visualization ---
    QPushButton* SourceVisualziation_sourePS_QButton = Q_NULLPTR;
    QCheckBox* DirectionCheck_sourcePS_QCheckBox = Q_NULLPTR;

    // --- Format Selection Buttons ---
    QPushButton* sourcePS_MCNP_button = Q_NULLPTR;
    QPushButton* sourcePS_PHITS_button = Q_NULLPTR;
    QPushButton* sourcePS_FLUKA_button = Q_NULLPTR;
    QPushButton* sourcePS_IAEA_button = Q_NULLPTR; // (New)
    QPushButton* sourcePS_USER_Button = Q_NULLPTR;

    // --- File Loading UI ---
    QPushButton* sourcePS_loadPSFButton = Q_NULLPTR;
    QTextEdit* sourcePS_PSFname = Q_NULLPTR;

private slots:
    // --- Slot Functions ---
    // Visualization Control
    void slot_PhaseSpaceSourceVisualization_ButtonClicked();
    void slot_DirectionCheck_sourcePS_QCheckBox_ButtonClicked();

    // Format Selection (Radio behavior using PushButtons)
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

private:
    // --- UI Components: Main GroupBox ---
    QGroupBox* m_sourceGeometryHotParticlePointGroupBox = Q_NULLPTR;

    // --- Visualization & Size Control ---
    QPushButton* SourceHP_SphereSize_Minus_QPushButton = Q_NULLPTR;
    QPushButton* SourceHP_SphereSize_Plus_QPushButton = Q_NULLPTR;
    QPushButton* m_HotParticleSourceVisualizationButton = Q_NULLPTR;

    // --- Geometry & Interaction ---
    QLineEdit* PosX_SourceHP_QLineEdit = Q_NULLPTR;
    QLineEdit* PosY_SourceHP_QLineEdit = Q_NULLPTR;
    QLineEdit* PosZ_SourceHP_QLineEdit = Q_NULLPTR;
    QLineEdit* SeparationDist_SourceHP_QLineEdit = Q_NULLPTR;
    
    QPushButton* Pick3D_HP_QButton = Q_NULLPTR;
    QPushButton* UpdatePosition_sourceHP_QButton = Q_NULLPTR;

    // --- Type Selection ---
    QRadioButton* RI_Select_sourceHP_QRadioButton = Q_NULLPTR;
    QRadioButton* ES_Select_sourceHP_QRadioButton = Q_NULLPTR;

    // --- Radionuclide UI ---
    QLabel* RI_Title_sourceHP_QLabel = Q_NULLPTR;
    QLineEdit* RI_Radionuclide_sourceHP_QLineEdit = Q_NULLPTR;
    QLabel* RI_Activity_sourceHP_QLabel = Q_NULLPTR;
    QLineEdit* RI_Activity_sourceHP_QLineEdit = Q_NULLPTR;
    QLabel* RI_List_sourceHP_QLabel = Q_NULLPTR;
    QPushButton* RIList_Add_sourceHP_QButton = Q_NULLPTR;
    QListWidget* RI_List_sourceHP_QListWidget = Q_NULLPTR;

    // --- Energy Spectrum UI ---
    QLabel* ES_Title_sourceHP_QLabel = Q_NULLPTR;
    QPushButton* sourceHP_EnergyspectrumFileLoadButton = Q_NULLPTR;
    QLabel* ES_FileName_sourceHP_QLabel = Q_NULLPTR;
    QTextEdit* sourceHP_EnergyspectrumFileNameText = Q_NULLPTR;
    QLabel* ES_Intensity_sourceHP_QLabel = Q_NULLPTR;
    QLineEdit* ES_Intensity_sourceHP_QLineEdit = Q_NULLPTR;
    QLabel* ES_List_sourceHP_QLabel = Q_NULLPTR;
    QPushButton* ESList_Add_sourceHP_QButton = Q_NULLPTR;
    QListWidget* ES_List_sourceHP_QListWidget = Q_NULLPTR;

    // --- Data Management (리스트 및 로직 변수) ---
    // 리스트 관리
    std::vector<std::map<int, QString>> ListInfo_sourceHP; // RI_Info_sourceRC[pointID][0~2] = 0: 사용여부(True or ""), 1: RI, 2: activity(Bq/cm3)
    std::vector<QPushButton*> RIList_Delete_sourceHP_QButton;
    std::vector<QPushButton*> RIList_Info_sourceHP_QButton;
    std::vector<QPushButton*> ESList_Delete_sourceHP_QButton;
    std::vector<QPushButton*> ESList_Info_sourceHP_QButton;
    int RIList_sourceHP_MakingIndex = 0; // radionulcide에서 현재 입력한 핵종 개수

    // 3D Pick 로직용 벡터 (기존 코드 로직상 필요함)
    double SelectedPointNormalVector_sourceHP[3] = { 0.0, 0.0, 0.0 }; // Hot particle 선택한 지점의 노말벡터

private slots:
    // --- Slot Functions ---
    
    // Visualization & Size
    void slot_SourceHP_SphereSize_Minus_ButtonClicked();
    void slot_SourceHP_SphereSize_Plus_ButtonClicked();
    void slot_HotParticleSourceVisualization_ButtonClicked();

    // 3D Pick & Update
    void slot_HotParticle_Pick3D_ButtonClicked();
    void slot_Update_Position_HP_ButtonClicked();

    // Type Selection
    void slot_RI_Select_sourceHP_RadioButtonClicked();
    void slot_ES_Select_sourceHP_RadioButtonClicked();

    // Radionuclide Operations
    void slot_RIList_Add_sourceHP_ButtonClicked();
    void slot_RIList_Delete_sourceHP_ButtonClicked();
    void slot_RIList_Info_sourceHP_ButtonClicked();

    // Energy Spectrum Operations
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

private:
    // --- UI Components: Main GroupBox ---
    QGroupBox* m_sourceGeometryConeBeamGroupBox = Q_NULLPTR;

    // --- Visualization ---
    QPushButton* m_ConeBeamSourceVisualizationButton = Q_NULLPTR;
    QCheckBox* sourceCB_dirCheckBox = Q_NULLPTR;

    // --- Geometry: Position (XYZ) ---
    QLineEdit* m_lineEditConeBeamPointX = Q_NULLPTR;
    QLineEdit* m_lineEditConeBeamPointY = Q_NULLPTR;
    QLineEdit* m_lineEditConeBeamPointZ = Q_NULLPTR;

    // --- Geometry: Direction Vector (XYZ) ---
    QLineEdit* m_lineEditConeBeamDirectionX = Q_NULLPTR;
    QLineEdit* m_lineEditConeBeamDirectionY = Q_NULLPTR;
    QLineEdit* m_lineEditConeBeamDirectionZ = Q_NULLPTR;

    // --- Geometry: Solid Angle ---
    QLineEdit* m_lineEditConeBeamDirectionSolidAngle = Q_NULLPTR;

    // --- Action Buttons ---
    QPushButton* pickConeBeam3DButton = Q_NULLPTR;
    QPushButton* sourceCB_UpdateButton = Q_NULLPTR;

    // --- Type Selection ---
    QRadioButton* sourceCB_Radionuclide_radioButton = Q_NULLPTR;
    QRadioButton* sourceCB_Energyspectrum_radioButton = Q_NULLPTR;

    // --- Radionuclide UI ---
    QLabel* labelConeBeamRadionuclideTitle = Q_NULLPTR;
    QLineEdit* m_lineEditConeBeamRadionuclide = Q_NULLPTR;
    QLabel* labelConeBeamRadionuclideActivity = Q_NULLPTR;
    QLineEdit* m_lineEditConeBeamActivity = Q_NULLPTR;
    QLabel* labelConeBeamRadionuclideList = Q_NULLPTR;
    QPushButton* ConeBeamRadionuclideAddButton = Q_NULLPTR;
    QListWidget* RI_List_sourceCB_QListWidget = Q_NULLPTR;

    // --- Energy Spectrum UI ---
    QLabel* labelConeBeamEnergyspectrumTitle = Q_NULLPTR;
    QPushButton* ConeBeamEnergyspectrumFileLoadButton = Q_NULLPTR;
    QLabel* labelConeBeamEnergyspectrumFileName = Q_NULLPTR;
    QTextEdit* sourceCB_EnergyspectrumFileNameText = Q_NULLPTR; // Note: QTextEdit
    QLabel* labelConeBeamEnergyspectrumIntensity = Q_NULLPTR;
    QLineEdit* m_lineEditConeBeamIntensity = Q_NULLPTR;
    QLabel* labelConeBeamEnergyspectrumList = Q_NULLPTR;
    QPushButton* ConeBeamEnergyspectrumAddButton = Q_NULLPTR;
    QListWidget* ES_List_sourceCB_QListWidget = Q_NULLPTR;

    // --- Data Management (리스트 및 로직 변수) ---
    
    // Radionuclide List Data
    std::vector<std::map<int, QString>> sourceCB_info; // 기존 코드 변수명 유지 // ListInfo_sourceEP[pointID][0~5] = 0: 사용여부(True or ""), 1: posX, 2: posY, 3: posZ, 4: RI, 5: activity(Bq)
    std::vector<QPushButton*> m_sourceCB_listDeleteButton;
    std::vector<QPushButton*> m_sourceCB_listInfoButton;
    int RIList_sourceCB_MakingIndex = 0; // radionulcide에서 현재 입력한 핵종 개수

    // Energy Spectrum List Data (기존 코드에 없던 벡터들도 필요시 추가)
    // ConeBeam 코드는 특이하게 로컬 변수나 즉석 생성 방식을 썼을 수 있으나, 
    // 관리를 위해 아래 벡터들을 멤버로 두는 것을 추천합니다.
    int EnergyspectrumListTotalCount_CB = 0; // radionulcide에서 현재 입력한 핵종 개수

    // Visualization Flag
    bool b_IsSourceLocationVisualized_sourceCB = false;

private slots:
    // --- Slot Functions ---

    // Visualization
    void slot_ConeBeamSourceVisualization_ButtonClicked();
    
    // Geometry & Interaction
    void slot_ConeBeam_Pick3D_ButtonClicked();
    void slot_ConeBeamUpdate_ButtonClicked();
    void slot_CBreset_ButtonClicked();

    // Type Selection
    void slot_ConeBeamRadionuclide_ButtonClicked();
    void slot_ConeBeamEnergyspectrum_ButtonClicked();

    // Radionuclide Operations
    void slot_ConeBeamRadionuclideAdd_ButtonClicked();
    // [중요] 기존 코드에서는 EP(ExternalPoint) 함수를 호출하고 있었으나,
    // 독립적인 위젯이 되려면 CB 전용 함수가 필요합니다.
    void slot_ConeBeamRadionuclideDelete_ButtonClicked(); 
    void slot_ConeBeamRadionuclideInfo_ButtonClicked();

    // Energy Spectrum Operations
    void slot_ConeBeamEnergyspectrumFileLoad_ButtonClicked();
    void slot_ConeBeamEnergyspectrumAdd_ButtonClicked();
    // [중요] ES 리스트의 Delete/Info도 별도 구현 필요
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

private:
    // --- UI Components: Main GroupBox ---
    QGroupBox* m_sourceGeometryRoomContaminationGroupBox = Q_NULLPTR;

    // --- Visualization ---
    QPushButton* m_RoomContaminationSourceVisualizationButton = Q_NULLPTR;

    // --- Geometry: Box Dimensions (Half Length) ---
    QLabel* label_sourceRC_BoxLength = Q_NULLPTR;
    QLineEdit* Box_HalfLengthX_sourceRC_QLineEdit = Q_NULLPTR;
    QLineEdit* Box_HalfLengthY_sourceRC_QLineEdit = Q_NULLPTR;
    QLineEdit* Box_HalfLengthZ_sourceRC_QLineEdit = Q_NULLPTR;

    // --- Geometry: Box Center ---
    QLabel* label_sourceRC_BoxCenter = Q_NULLPTR;
    QLineEdit* Box_CenterX_sourceRC_QLineEdit = Q_NULLPTR;
    QLineEdit* Box_CenterY_sourceRC_QLineEdit = Q_NULLPTR;
    QLineEdit* Box_CenterZ_sourceRC_QLineEdit = Q_NULLPTR;

    // --- Action Button ---
    QPushButton* UpdateRCposButton = Q_NULLPTR;

    // --- Type Selection ---
    QRadioButton* RI_Select_sourceRC_QRadioButton = Q_NULLPTR;
    QRadioButton* ES_Select_sourceRC_QRadioButton = Q_NULLPTR;

    // --- Radionuclide UI ---
    // (Title, LineEdit, Activity, List 등)
    // 기존 코드에서는 전용 변수명을 썼으므로 그대로 유지
    QLineEdit* RI_sourceRC_QLineEdit = Q_NULLPTR;       // Radionuclide Name
    QLineEdit* Activity_sourceRC_QLineEdit = Q_NULLPTR; // Activity
    QLabel* RadionuclideList_sourceRC_QLabel = Q_NULLPTR;
    QPushButton* RIList_Add_sourceRC_QButton = Q_NULLPTR;
    QListWidget* RI_List_sourceRC_QListWidget = Q_NULLPTR;

    // --- Data Management ---
    std::vector<std::map<int, QString>> RI_Info_sourceRC; // RI_Info_sourceRC[pointID][0~2] = 0: 사용여부(True or ""), 1: RI, 2: activity(Bq/cm3)
    std::vector<QPushButton*> RIList_Delete_sourceRC_QButton;
    std::vector<QPushButton*> RIList_Info_sourceRC_QButton;
    int RIList_sourceRC_MakingIndex = 0; // radionulcide에서 현재 입력한 핵종 개수		

private slots:
    // --- Slot Functions ---

    // Visualization
    void slot_RoomContaminationSourceVisualization_ButtonClicked();

    // Geometry Update
    void slot_UpdateRCpos_ButtonClicked();

    // Type Selection (현재 비어있지만 선언 유지)
    void slot_RI_Select_sourceRC_RadioButtonClicked();
    void slot_ES_Select_sourceRC_RadioButtonClicked();

    // Radionuclide Operations
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

private:
    // --- Visualization ---
    QPushButton* m_EnvironmentalSourceVisualizationButton = Q_NULLPTR;

    // --- Type Selection ---
    QComboBox* m_comboBoxEnvironmentalSourceType = Q_NULLPTR;

    // --- Depth Input (Visible only for Soil) ---
    QLabel* labelEnvironmentalSoilDepth = Q_NULLPTR;
    QLineEdit* m_lineEditEnvironmentalSoilDepth = Q_NULLPTR;

    // --- Radionuclide Input ---
    QLabel* labelEnvironmentalRadionuclideTitle = Q_NULLPTR;
    QLineEdit* m_lineEditEnvironmentalRadionuclide = Q_NULLPTR;
    
    // --- Activity Input ---
    QLabel* labelEnvironmentalRadionuclideActivity = Q_NULLPTR;
    QLineEdit* m_lineEditEnvironmentalActivity = Q_NULLPTR;

    // --- Add Button & List ---
    QLabel* labelEnvironmentalRadionuclideList = Q_NULLPTR;
    QPushButton* EnvironmentalRadionuclideAddButton = Q_NULLPTR;
    QListWidget* listWidget_E_Radionuclide = Q_NULLPTR;

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

private:
    // --- UI Components: Main GroupBox ---
    QGroupBox* m_sourceGeometryParallelBeamGroupBox = Q_NULLPTR;

    // --- Visualization ---
    QPushButton* m_ParallelBeamSourceVisualizationButton = Q_NULLPTR;
    // [주의] 기존 코드에서 ConeBeam의 변수명을 그대로 복사해 쓴 것으로 보임.
    // 에러 방지를 위해 변수명을 기존 코드와 동일하게 유지합니다.
    QCheckBox* sourceCB_dirCheckBox = Q_NULLPTR; 

    // --- Geometry: Center Position (XYZ) ---
    QLineEdit* m_lineEditParallelBeamPointX = Q_NULLPTR;
    QLineEdit* m_lineEditParallelBeamPointY = Q_NULLPTR;
    QLineEdit* m_lineEditParallelBeamPointZ = Q_NULLPTR;

    // --- Geometry: Dimensions ---
    QLineEdit* m_lineEditParallelBeamRadius = Q_NULLPTR;

    // --- Geometry: Direction (Spherical Coordinates) ---
    QLineEdit* m_lineEditParallelBeamDirectionTheta = Q_NULLPTR;
    QLineEdit* m_lineEditParallelBeamDirectionPhi = Q_NULLPTR;

    // --- Action Buttons ---
    QPushButton* pickParallelBeam3DButton = Q_NULLPTR;
    QPushButton* sourcePB_UpdateButton = Q_NULLPTR;

    // --- Type Selection ---
    QRadioButton* sourcePB_MonoEnergy_radioButton = Q_NULLPTR;
    QRadioButton* sourcePB_EnergySpectrum_radioButton = Q_NULLPTR;

    // --- Mono Energy UI ---
    QLabel* labelParallelBeamParticleTypeTitle = Q_NULLPTR;
    QComboBox* m_comboBoxParallelBeamParticleType = Q_NULLPTR;
    QLabel* labelParallelBeamEnergyTitle = Q_NULLPTR;
    QLineEdit* m_lineEditParallelBeamEnergy = Q_NULLPTR;
    QLabel* labelParallelBeamIntensityTitle = Q_NULLPTR;
    QLineEdit* m_lineEditParallelBeamIntensity_MonoEnergy = Q_NULLPTR;

    // --- Energy Spectrum UI ---
    QLabel* labelParallelBeamEnergyspectrumTitle = Q_NULLPTR;
    QPushButton* ParallelBeamEnergyspectrumFileLoadButton = Q_NULLPTR;
    QLabel* labelParallelBeamEnergyspectrumFileName = Q_NULLPTR;
    QTextEdit* sourcePB_EnergyspectrumFileNameText = Q_NULLPTR; // QTextEdit
    QLabel* labelParallelBeamEnergyspectrumIntensity = Q_NULLPTR;
    QLineEdit* m_lineEditParallelBeamIntensity_EnergySpectrum = Q_NULLPTR;
    QLabel* labelParallelBeamEnergyspectrumList = Q_NULLPTR;
    QPushButton* ParallelBeamEnergyspectrumAddButton = Q_NULLPTR;
    QListWidget* ES_List_sourcePB_QListWidget = Q_NULLPTR;

    // --- Data Management ---
    bool b_IsSourceLocationVisualized_sourcePB = false;

    // Energy Spectrum List Management (기존 코드엔 구현이 비어있으나, 필요할 것으로 예상되어 추가)
    std::vector<std::map<int, QString>> sourcePB_ES_info;
    std::vector<QPushButton*> m_sourcePB_ES_listDeleteButton;
    std::vector<QPushButton*> m_sourcePB_ES_listInfoButton;
    int ESList_sourcePB_MakingIndex = 0;

private slots:
    // --- Slot Functions ---

    // [Existing] 기존에 있던 시각화 함수
    void slot_ParallelBeamSourceVisualization_ButtonClicked();
    
    // [Existing] 기존에 있던 3D Pick 및 Update 함수
    void slot_ParallelBeam_Pick3D_ButtonClicked();
    void slot_ParallelBeamUpdate_ButtonClicked();

    // [NEW] 라디오 버튼 (UI 토글용)
    // 원본 코드: connect(..., SLOT()); -> 슬롯이 비어 있었음 -> 새로 이름 부여함
    void slot_PB_MonoEnergy_RadioButtonClicked();
    void slot_PB_EnergySpectrum_RadioButtonClicked();

    // [Existing] 파일 로드 및 추가 버튼
    void slot_ParallelBeamEnergyspectrumFileLoad_ButtonClicked();
    void slot_ParallelBeamEnergyspectrumAdd_ButtonClicked();
    
    // [NEW] 리스트 삭제 및 정보 버튼
    // 원본 코드: connect(..., SLOT(slot_RIList_Delete_sourceEP...)); -> EP(External Point) 함수를 쓰고 있었음
    // 수정: PB 전용 함수로 교체
    void slot_ParallelBeamEnergyspectrumDelete_ButtonClicked();
    void slot_ParallelBeamEnergyspectrumInfo_ButtonClicked();
};
//////////////////////////////////////////////////////////////////////
class SourceGeometryWidgetFactory
{
public:
	static SourceGeometryWidget* CreateSourceGeometry(const std::string& name);
};

