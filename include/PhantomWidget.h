#pragma once

#include <QWidget>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QTimer>
#include <QString>
#include <QFileDialog>
#include <QFileInfo>
#include <QDebug>
#include <vector> // std::vector 사용
#include <map>    // std::map 사용

// Windows API (메모리 체크용)
#include <windows.h> 
#include <cmath>

// 커스텀 위젯 및 모델 헤더
#include "QHoverSensitiveButton.h"
#include "StyleSheet.h"
#include "PhantomModels.h" // 데이터 모델 헤더 포함

#include "SceneDataTypes.h"

class FunctionPanelRight;
class MultipleUIDialog;
class MultipleButtonScorllArea;

class PhantomWidget : public QGroupBox
{
    Q_OBJECT

public:
    explicit PhantomWidget(QWidget* parent = nullptr);
    virtual ~PhantomWidget();

    // 초기화 함수
    bool initialize();

    // 부모 패널 포인터
    FunctionPanelRight* panel = nullptr;

    // 데이터 모델 접근자 (필요 시)
    PhantomModel& getModel() { return m_model; }
    enum class ButtonType {
        Phantom,
        Clothing,
        ClothingLayer,
        Dosimeter
    };

    std::map<int, QPushButton*>& getQPushButtonMap(ButtonType type);

    void WritePhantomMacro(std::ofstream& ofpPhantom);
    void WritePhantomInfo(std::ofstream& ofp_info);
    void ReadPhantomInfo(std::ifstream& ifp);
    void SetLoadingBaseDir(const QString& dir);

    void Eyewear_Generate(int PhantomIdx);
    void Dosimeter_Generate(int PhantomIdx, int DosimeterIdxo);

    void temporal_Dosimeter_SetText(double x, double y, double z);

    QLabel* getPhantomRAMUsageLabel() const;

private:
    // UI -> Model 데이터 동기화 함수
    void updateModelFromUI();

    QString m_CurrentLoadingDir; // void SetLoadingBaseDir(const QString& dir);

public slots:
    // --- Phantom Control Slots ---
    void slot_PhantomAdd_ButtonClicked();
    void slot_PhantomDelete_ButtonClicked();
    void slot_PhantomSelect_ButtonClicked();
    void slot_PhantomMoveLeft_ButtonClicked();
    void slot_PhantomMoveRight_ButtonClicked();
    void slot_PhantomUpdate_ButtonClicked();
    void slot_DummyPhantomCheckBoxClicked();

    // --- Phantom Setting Dialog Slots ---
    void slot_PhantomAddingOK_ButtonClicked();
    void slot_PhantomSetting_ComputationlPhantom_RadioButtonClicked();
    void slot_PhantomSetting_AirPhantom_RadioButtonClicked();
    void slot_PhantomSetting_Import_ButtonClicked();
    void slot_ChangePhantomTypeCombo_Setting(int idx);
    void slot_ChangePhantomDummyCombo_Setting(int idx);
    void slot_ChangePhantomGenderCombo_Setting(int idx);
    void slot_ChangePhantomAgeCombo_Setting(int idx);
    void slot_ChangePhantomPostureCombo_Setting(int idx);
    void slot_ChangePhantomFetalAgeCombo_Setting(int idx);

    // --- Clothing Slots ---
    void slot_ClothingAdd_ButtonClicked();
    void slot_ClothingDelete_ButtonClicked();
    void slot_ClothingSelect_ButtonClicked();
    
    // --- Clothing Setting Dialog Slots ---
    void slot_ChangeClothingSettingTypeCombo(int idx);
    void slot_ClothingAddingOK_ButtonClicked();

    // --- Clothing Layer Slots ---
    void slot_ClothingLayerAdd_ButtonClicked();
    void slot_ClothingLayerDelete_ButtonClicked();
    void slot_ClothingLayerSelect_ButtonClicked();
    void slot_ClothingLayerUpdate_ButtonClicked();
    
    // --- Clothing Layer Setting Dialog Slots ---
    void slot_ClothingLayerAddingOK_ButtonClicked();

    // --- Eyewear Slots ---
    void slot_Add_Delete_Eyewear_ButtonClicked();
    void slot_GlassesUpdate_ButtonClicked();
    void slot_Glasses3DPick_ButtonClicked();

    // --- Eyewear Setting Dialog Slots ---
    void slot_EyewearSetting_Type_ComboChanged(int idx);
    void slot_EyewearSetting_Composition_ComboChanged(int idx);
    void slot_EyewearSetting_AddingOK_ButtonClicked();

    // --- Dosimeter Slots ---
    void slot_DosimeterAdd_ButtonClicked();
    void slot_DosimeterDelete_ButtonClicked();
    void slot_DosimeterSelect_ButtonClicked();
    void slot_DosimeterUpdate_ButtonClicked();
    void slot_DosimeterLocationUpdate_ButtonClicked();
    void slot_DosimeterPick3D_ButtonClicked();

    // --- Move to Public---
    void UpdatePhantom_ListButton(int phantomIndex); // 현재 선택 객체 인덱스를 기반으로 List 버튼 업데이트
    void SavePhantom_InfoData_InMouseControl(int phantomIndex);
    void UpdatePhantom_InfoStatus(int phantomIndex); // InfoData의 데이터를 Info Status에 업데이트
    void PhantomAddingSetting_Create();
    void Commit_PhantomSetting(const PhantomData& pData);
    void ExecutePhantomAddRoutines();
    void SaveClothingInformation_InClothingGenerate();  // 의복 제작 단계에서, 의복정보 컨테이너에 clothing setting창에서 입력한 정보를 저장
    void SetClothingPanelInfo(int clothingIndex, int clothingLayerIndex); // 팬텀, 의복 추가, 제거, 선택하는 과정 전반에서 clothing 패널 정보를 업데이트
    void InitializeClothingPanel_InClothingAdd(); // 최초 의복 입력 시, 의복 제작할 때 clothing panel의 기본구조를 초기화
    
private:
    // ========================================================================
    // [Data Model]
    // ========================================================================
    // 모든 데이터 변수(SequenceVector, MainInfo 등)와 Enum은 여기로 이동됨
    PhantomModel m_model; 

    // ========================================================================
    // [UI Components] (View)
    // ========================================================================

    // 1. RAM Usage
    QLabel* Phantom_RAMusage_QLabel = nullptr;
    // m_Phantom_RAM_Usage는 모델로 이동

    // 2. Phantom List Area
    QGroupBox* PhantomList_QGroupBox;
    QHBoxLayout* PhantomList_QHBoxLayout;
    QScrollBar* PhantomList_Horizontal_QScrollBar;
    QScrollBar* PhantomList_Vertical_QScrollBar;
    
    // 버튼 '객체'는 View 영역이므로 여기에 남겨둡니다.
    std::map<int, QPushButton*> PhantomSelect_QPushButton; 
    QPushButton* PhantomAdd_QPushButton;
    QPushButton* PhantomDelete_QPushButton;
    QPushButton* PhantomMoveLeft_UserQPushButton; 
    QPushButton* PhantomMoveRight_UserQPushButton; 

    // 3. Phantom Info Status Area
    QLineEdit* PhantomType_QLineEdit;
    QCheckBox* PhantomDummy_QCheckBox;
    QLineEdit* PhantomGender_QLineEdit;
    QLineEdit* PhantomAge_QLineEdit;
    QLineEdit* PhantomPosture_QLineEdit;
    QLineEdit* PhantomFetalAge_QLineEdit;
    QLineEdit* PhantomHeight_QLineEdit;
    QLineEdit* PhantomWeight_QLineEdit;
    
    // Phantom Pos/Rot
    QLineEdit* PhantomPosX_QLineEdit;
    QLineEdit* PhantomPosY_QLineEdit;
    QLineEdit* PhantomPosZ_QLineEdit;
    QLineEdit* PhantomRotX_QLineEdit;
    QLineEdit* PhantomRotY_QLineEdit;
    QLineEdit* PhantomRotZ_QLineEdit;
    QPushButton* PhantomUpdate_QPushButton;

    // 4. Clothing Area
    QLabel* ClothingTitle_QLabel;
    QHBoxLayout* ClothingList_QHBoxLayout;
    QScrollBar* ClothingList_QScrollBar;
    std::map<int, QPushButton*> ClothingSelect_QPushButton;
    QPushButton* ClothingAdd_QPushButton;
    QPushButton* ClothingDelete_QPushButton;

    // 5. Clothing Layer Area
    QLabel* ClothingLayerTitle_QLabel;
    QHBoxLayout* ClothingLayerList_QHBoxLayout;
    QScrollBar* ClothingLayerList_QScrollBar;
    std::map<int, QPushButton*> ClothingLayerSelect_QPushButton;
    QPushButton* ClothingLayerAdd_QPushButton;
    QPushButton* ClothingLayerDelete_QPushButton;

    // 6. Clothing Layer Info
    QLineEdit* ClothingThickness_QLineEdit;
    QLineEdit* ClothingDensity_QLineEdit;
    QComboBox* ClothingComposition_QComboBox;
    QPushButton* ClothingUpdate_QPushButton;

    // 7. Eyewear Area
    QPushButton* Eyewear_AddDelete_QPushButton;
    QLineEdit* EyewearType_QLineEdit;
    QLineEdit* EyewearDistance_QLineEdit;
    QLineEdit* EyewearRadius_QLineEdit;
    
    QLabel* EyewearThickness_QLabel;
    QLineEdit* EyewearThickness_QLineEdit;
    QLabel* EyewearThickness_Front_QLabel;
    QLineEdit* EyewearThickness_Front_QLineEdit;
    QLabel* EyewearThickness_Side_QLabel;
    QLineEdit* EyewearThickness_Side_QLineEdit;
    
    QLineEdit* EyewearDensity_QLineEdit;
    QComboBox* EyewearComposition_QComboBox;
    QPushButton* EyewearUpdate_QPushButton;
    QPushButton* Eyewear_3DPick_QPushButton;

    // 8. Dosimeter Area
    QHBoxLayout* DosimeterList_QHBoxLayout;
    QScrollBar* DosimeterList_QScrollBar;
    std::map<int, QPushButton*> DosimeterSelect_QPushButton;
    QPushButton* DosimeterAdd_QPushButton;
    QPushButton* DosimeterDelete_QPushButton;
    
    QLineEdit* DosimeterPosX_QLineEdit;
    QLineEdit* DosimeterPosY_QLineEdit;
    QLineEdit* DosimeterPosZ_QLineEdit;
    QLineEdit* DosimeterRadius_QLineEdit;
    QLineEdit* DosimeterSeparationDistance_QLineEdit;
    QPushButton* DosimeterUpdate_QPushButton;

    // ========================================================================
    // [Dialogs & Settings UI]
    // ========================================================================

    // 1. Phantom Adding Setting Dialog
    MultipleUIDialog* PhantomSetting_UserQDialog;
    QLabel* PhantomSetting_Category_QLabel;
    QRadioButton* PhantomSetting_ComputationlPhantom_QRadioButton;
    QRadioButton* PhantomSetting_AirPhantom_QRadioButton;
    QLabel* PhantomSetting_Name_QLabel;
    QLineEdit* PhantomSetting_Name_QLineEdit;
    QHBoxLayout* PhantomSetting_Type_QHBoxLayout;
    QLabel* PhantomSetting_Type_QLabel;
    QComboBox* PhantomSetting_Type_QComboBox;
    QLineEdit* PhantomSetting_ImportedFileName_QLineEdit;
    QPushButton* PhantomSetting_PhantomImport_QPushButton;
    QHBoxLayout* PhantomSetting_Dummy_QHBoxLayout;
    QLabel* PhantomSetting_Dummy_QLabel;
    QComboBox* PhantomSetting_Dummy_QComboBox;
    QHBoxLayout* PhantomSetting_Gender_QHBoxLayout;
    QLabel* PhantomSetting_Gender_QLabel;
    QComboBox* PhantomSetting_Gender_QComboBox;
    QHBoxLayout* PhantomSetting_Age_QHBoxLayout;
    QLabel* PhantomSetting_Age_QLabel;
    QComboBox* PhantomSetting_Age_QComboBox;
    QHBoxLayout* PhantomSetting_Posture_QHBoxLayout;
    QLabel* PhantomSetting_Posture_QLabel;
    QComboBox* PhantomSetting_Posture_QComboBox;
    QHBoxLayout* PhantomSetting_FetalAge_QHBoxLayout;
    QLabel* PhantomSetting_FetalAge_QLabel;
    QComboBox* PhantomSetting_FetalAge_QComboBox;
    QHBoxLayout* PhantomSetting_Height_QHBoxLayout;
    QLabel* PhantomSetting_Height_QLabel;
    QLineEdit* PhantomSetting_Height_QLineEdit;
    QHBoxLayout* PhantomSetting_Weight_QHBoxLayout;
    QLabel* PhantomSetting_Weight_QLabel;
    QLineEdit* PhantomSetting_Weight_QLineEdit;
    QLabel* PhantomSetting_Position_QLabel;
    QLineEdit* PhantomSetting_PosX_QLineEdit;
    QLineEdit* PhantomSetting_PosY_QLineEdit;
    QLineEdit* PhantomSetting_PosZ_QLineEdit;
    QHBoxLayout* PhantomSetting_Rot_QHBoxLayout;
    QLabel* PhantomSetting_Rotation_QLabel;
    QLineEdit* PhantomSetting_RotX_QLineEdit;
    QLineEdit* PhantomSetting_RotY_QLineEdit;
    QLineEdit* PhantomSetting_RotZ_QLineEdit;
    QPushButton* PhantomSetting_AddingOK_QPushButton;

    // 2. Clothing Adding Setting Dialog
    MultipleUIDialog* ClothingSetting_UserQDialog;
    QComboBox* ClothingSetting_Type_QComboBox;
    QLineEdit* ClothingSetting_Name_QLineEdit;
    QLineEdit* ClothingSetting_Layername_QLineEdit;
    QLineEdit* ClothingSetting_Thickness_QLineEdit;
    QLineEdit* ClothingSetting_Density_QLineEdit;
    QComboBox* ClothingSetting_Composition_QComboBox;
    QPushButton* ClothingSetting_AddingOK_QPushButton;

    // 3. Clothing Layer Adding Setting Dialog
    MultipleUIDialog* ClothingLayerSetting_UserQDialog;
    QLineEdit* ClothingLayerSetting_Layername_QLineEdit;
    QLineEdit* ClothingLayerSetting_Thickness_QLineEdit;
    QLineEdit* ClothingLayerSetting_Density_QLineEdit;
    QComboBox* ClothingLayerSetting_Composition_QComboBox;
    QPushButton* ClothingLayerSetting_AddingOK_QPushButton;

    // 4. Eyewear Adding Setting Dialog
    MultipleUIDialog* EyewearAdding_UserQDialog;
    QComboBox* EyewearSetting_Type_QComboBox;
    QLineEdit* EyewearSetting_Distance_QLineEdit;
    QLineEdit* EyewearSetting_Radius_QLineEdit;
    QLabel* EyewearSetting_Thickness_QLabel;
    QLineEdit* EyewearSetting_Thickness_QLineEdit;
    QLabel* EyewearSetting_Thickness_Front_QLabel;
    QLineEdit* EyewearSetting_Thickness_Front_QLineEdit;
    QLabel* EyewearSetting_Thickness_Side_QLabel;
    QLineEdit* EyewearSetting_Thickness_Side_QLineEdit;
    QLineEdit* EyewearSetting_Density_QLineEdit;
    QComboBox* EyewearSetting_Composition_QComboBox;
    QPushButton* EyewearSetting_AddingOK_QPushButton;

    // ========================================================================
    // [Utilities]
    // ========================================================================
    
    // 타이머는 UI 동작(스크롤)을 제어하므로 Widget에 남겨둡니다.
    QTimer* timer_scroll_multiplePhantom = nullptr;
    QTimer* timer_scroll_clothing = nullptr;
    QTimer* timer_scroll_clothingLayer = nullptr;
    QTimer* timer_scroll_dosimeter = nullptr;
    
    QTimer* timer_Close_PhantomUIdialog_Recon = nullptr;
    QTimer* timer_Close_ClothingUIdialog_Recon = nullptr;

    // ========================================================================
    // [Helper Functions]
    // ========================================================================
private:
    void Create_Phantom_ListWidgets(QVBoxLayout* layout);
    
    // Phantom Logic Helpers
    void SavePhantom_InfoData_InAddingSetting(int phantomIndex); // AddingSetting창의 데이터를 InfoData 컨테이너에 업데이트
    void SavePhantom_InfoData(int phantomIndex); // InfoStatus의 데이터를 InfoData 컨테이너에 업데이트
    void moveMaximumScrollbar_phantom();

    // Clothing Logic Helpers
    void RemoveClothingPanel();
    void SetClothingPanel_inPhantomDelete();
    void SetClothingPanel_inPhantomSelect();
    void ClothingAddingSetting(); // Phantom 패널 내 Clothing Add 버튼 클릭 시 Clothing Setting 창을 띄우는 함수

    QString DefinePreWearablePath(int clothingSelectedType_Index);
    void moveMaximumScrollbar_clothing();

    // Clothing Layer Logic Helpers
    void ClothingLayerAddingSetting(); // Phantom 패널 내 Clothing Layer Add 버튼 클릭 시 Clothing Layer Setting 창을 띄우는 함수
    void SaveClothingInformation_InLayerGenerate(); // 의복 레이어를 추가하는 단계에서, 의복정보 컨테이너에 clothing layer setting창에서 입력한 정보를 저장


    void moveMaximumScrollbar_clothingLayer();

    // Eyewear Logic Helpers
    void EyewearAddingSetting();

    // Dosimeter Logic Helpers
    void moveMaximumScrollbar_dosimeter();
    
    // UI Util

    void ClosePhantomUIdialog_Recon();
    void CloseClothingUIdialog_Recon();

};