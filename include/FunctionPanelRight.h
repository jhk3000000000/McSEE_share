#pragma once

#include <QtWidgets>
#include <ETHuman3D.h>
#include <qlist.h>
//#include "ETHuman3DApp.h"
#include "ETInteractorStyle.h"
#include "StyleSheet.h"
#include <map>
#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include "qcustomplot.h"

#include <qcompleter.h>
#include <qstringlist.h>
#include <DBServerConnect.h>

#include <QFile>
#include <QTextStream>
#include <QMutex>
#include <QDateTime>
#include <QDebug>
#include <QCoreApplication>
#include <QMutexLocker>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "SceneDataTypes.h"

#include "FunctionPanelPreDefined.h"

// Qt6
#include <QEnterEvent>

using namespace utils::ui;

class ETHuman3DApp;
class Manager_Calculation;
class PhantomObjects; // 필요하다면 추가
class SourceObjects;  // 필요하다면 추가
class SourceGeometryWidget;
class PhantomWidget;

class CustomButton;
class LeftButton;
class RightButton;
class MultipleButtonScorllArea;
class CalculationSettingDialog;
class MultipleUIDialog;
class ObjectMaterialTextWindow;
class FlexibleValidator;

//************************************** Pre-defined Class/Struct **************************************//
class FunctionPanelRight : public QWidget
{
	Q_OBJECT

	//friend class Manager_Calculation;
	friend class PhantomWidget;
    friend class SourceGeometryWidget;

public:
	explicit FunctionPanelRight(QWidget *parent = Q_NULLPTR);
	virtual ~FunctionPanelRight();

	void PhantomList_button_generate();
	
//************************************** Variables **************************************//
public:
	// UI Resolution
	QScreen *SCREEN = QApplication::primaryScreen();	
	// double Window_width = QApplication::desktop()->screenGeometry().width();
	// double Window_height = QApplication::desktop()->screenGeometry().height();
	double Window_width = SCREEN->geometry().width();
	double Window_height = SCREEN->geometry().height();
	double RatioWtoH = Window_width / Window_height;
	qreal LogicalDPI = SCREEN->logicalDotsPerInch();
	qreal PhysicalDPI = SCREEN->physicalDotsPerInch();
	QSizeF PhysicalSizeMM = SCREEN->physicalSize();
	qreal PhysicalWidthInInches = PhysicalSizeMM.width() / 25.4;
	qreal PhysicalHeightInInches = PhysicalSizeMM.height() / 25.4;
	double PhysicalSizeInches = std::sqrt(PhysicalWidthInInches * PhysicalWidthInInches + PhysicalHeightInInches * PhysicalHeightInInches);
	double PhysFactor = PhysicalSizeInches * PhysicalDPI;
	double Default_window_width = 2560.;
	double Default_window_height = 1440.;
	double Default_RatioWtoH = 2560. / 1440.; // this is just 16:9
	double Default_logicalDPI = 144.; // 150% 배율
	double Default_physFactor = 32. * 93.; 
	/*
	32 Inch * 93 PhysicalDPI (32inch, QHD(2560 * 1440))
	Special calibration for target error scroll bar, time-dose graph
	*/
	double SpecialWidgetSizeFactor = std::pow(PhysFactor / Default_physFactor, 0.1) * std::pow(Default_logicalDPI / LogicalDPI, 0.1);
	//============================================================================================================================//

	// Font
	int FontSizeScaling(int DefaultFontSize)// FontSizeScaling Function
	{		
		double PhysPower = 1.2;
		double logiDPIPower = 0.9;
		double ResRatioPower = 0.6;
		double scale_factor = std::pow(PhysFactor / Default_physFactor, PhysPower) * std::pow(Default_logicalDPI / LogicalDPI, logiDPIPower) * std::pow(Default_RatioWtoH / RatioWtoH, ResRatioPower);
		int calibration = 0;
		if (RatioWtoH <= 1.5) calibration++; // 해상도 세로가 많이 길 때
		if (Window_width <= 1500.) calibration++; // 가로 해상도 자체가 많이 작을 때
		return std::floor(DefaultFontSize * scale_factor) - calibration;
	}
	QFont font_D_BTN1;
	QFont font_D_BTN2;
	QFont font_D_BTN3;
	QFont font_D_BTN4;
	QFont font_D_BTN5;
	QFont font_D_BTN6;
	QFont font_D_BTN7;
	QFont font_D_BTN8;
	QFont font_D_BTN9;
	QFont font_D_BTN10;
	QFont font_D_BTN11;
	QFont font_D_BTN12;
	QFont font_D_BTN13;
	QFont font_D_BTN14;
	QFont font_D_BTN15;
	QFont font_D_BTN16;
	QFont font_D_DE1;
	QFont font_D_RB1;
	QFont font_D_RB2;
	QFont font_D_RB3;
	QFont font_D_GBW1;
	QFont font_D_GBW2;
	QFont font_D_GBW3;
	QFont font_D_GBW4;
	QFont font_D_GBW5;
	QFont font_D_GBW6;
	QFont font_D_GBW7;
	QFont font_D_GBW8;
	QFont font_D_GBW9;
	QFont font_D_GBW10;
	QFont font_D_GBW11;
	QFont font_D_TBW1;
	QFont font_D_LBL1;
	QFont font_D_LBL2;
	QFont font_D_LBL3;
	QFont font_D_LBL4;
	QFont font_D_LBL5;
	QFont font_D_CB1;
	QFont font_D_CB2;
	QFont font_D_CB3;
	QFont font_D_LW1;
	QFont font_D_LW2;
	QFont font_D_LE1;
	QFont font_D_LE2;
	QFont font_D_LE3;
	QFont font_D_LE4;
	QFont font_D_LE5;
	QFont font_D_MAT;
	//============================================================================================================================//
	
	// Common Variables
	QList<QPushButton*> Buttons_FunctionPanelRight; // button 총 집합
	QList<QPushButton*> TempEnabledButtons; // button 총 집합
	QList<QPushButton*> TempDisabledButtons; // button 총 집합
	std::vector<QString> OrganNameList = { "RBM", "BS", "Colon", "Lungs", "Stomach", "Breasts", "Testes/Ovaries",
										"UrinaryBladder", "Oesophagus", "Liver", "Thyroid", "Brain", "SalivaryGlands", "Skin(target)", "Adrenal",
										"ET1", "ET2", "GallBladder", "Heart", "Kidneys", "LymphNodes", "Muscle", "OralMucosa", "Pancreas", "Prostate/Uterus",
										"SmallIntestine", "Spleen", "Thymus", "EyeLens", "Enamel(teeth)", "Eff.dose", "WholeBodyDose" };
	//============================================================================================================================//

	// Main Widget
	QPushButton* Phantom_QPushButton = nullptr;
	QPushButton* Geometry_QPushButton = nullptr;
	QPushButton* Source_QPushButton = nullptr;
	QPushButton* Output_QPushButton = nullptr;
	//============================================================================================================================//

	// Phantom Widget
	QGroupBox* PhantomPanel_QGroupBox = nullptr; /* 전체 팬텀패널 */
	PhantomWidget* m_phantoms = nullptr;
	
	//============================================================================================================================//
			
	// Geometry Widget
	QGroupBox* GeometryPanel_QGroupBox = nullptr;

		// Geometry Widget - Environment - Global Variables

		// Geometry Widget - Environment - Qt Variables
		QComboBox* Geometry_EnvironmentType_QComboBox = nullptr;
		//-----------------------------------------------------------------------------------------------------------------//

		// Geometry Widget - All Objects - Global Variables
		int const_Object_MaximumCount = 500; // Geometry 패널의 Object 최대 입력 개수
		int m_Object_MakingIndex = 0; // delete된것은 고려하지 않고 add 할때마다 늘어나는 생성 인덱스 -> Group도 포함
		int m_Object_SelectedIndex = -1;
		std::vector<int> m_Object_SequenceVector; // m_Object_SequenceVector = {1,3,2,5 ...} ->현재 UI 상에 위치한 object 버튼 순서대로 실제 ID가 적힌 것 {1, 3, 2, 5 ...}

		std::map<int, int> m_ObjectType; // m_ObjectType[objectMakingIndex] = {0,1,1,0,2,1,1} ->making index 순서대로 object type 정보(0: Box, 1: Sphere, 2: Cylinder)
		enum ObjectType {
			E_OBJECTTYPE_BOX = 0,
			E_OBJECTTYPE_SPHERE = 1,
			E_OBJECTTYPE_CYLINDER = 2
		};
		std::map<int, QString> m_Object_ButtonName; // m_Object_ButtonName[objectMakingIndex] = {"MyBox"}
		std::map<int, std::map<int, double>> m_Object_MainInfo; // m_Object_MainInfo[m_Object_MakingIndex][0~:정보]
		enum ObjectMainInfo {
			E_OBJECTMAININFO_USED = 0, // 정보사용여부
			E_OBJECTMAININFO_POS_X = 1, 
			E_OBJECTMAININFO_POS_Y = 2,
			E_OBJECTMAININFO_POS_Z = 3, 
			E_OBJECTMAININFO_ROT_X = 4,
			E_OBJECTMAININFO_ROT_Y = 5,
			E_OBJECTMAININFO_ROT_Z = 6,
			E_OBJECTMAININFO_HALFLEN_X = 7,
			E_OBJECTMAININFO_HALFLEN_Y = 8,
			E_OBJECTMAININFO_HALFLEN_Z = 9,
			E_OBJECTMAININFO_THICKNESS_X = 10,
			E_OBJECTMAININFO_THICKNESS_Y = 11,
			E_OBJECTMAININFO_THICKNESS_Z = 12, 
			E_OBJECTMAININFO_DENSITY = 13,
			E_OBJECTMAININFO_MINRAD_SPH = 14,
			E_OBJECTMAININFO_MAXRAD_SPH = 15,
			E_OBJECTMAININFO_STARTING_AZI = 16,
			E_OBJECTMAININFO_DELTA_AZI = 17,
			E_OBJECTMAININFO_STARTING_POL = 18,
			E_OBJECTMAININFO_DELTA_POL = 19,
			E_OBJECTMAININFO_MINRAD_CYL = 20,
			E_OBJECTMAININFO_MAXRAD_CYL = 21,
			E_OBJECTMAININFO_HALFHT_X = 22,
			E_OBJECTMAININFO_STARTING_ANG = 23,
			E_OBJECTMAININFO_DELTA_ANG = 24
		};
		std::map<int, QString> m_Object_MaterialInfo; // m_Object_MaterialInfo[m_Object_MakingIndex] = Material text	
		std::map<int, int> m_Object_MaterialFractionType; // m_ObjectMaterialFractionTypeVector[objectMakingIndex] = 0 or 1 (mass fraction / atom fraction)
		std::map<int, bool> m_Object_ChargedParticleTransport; // m_Object_ChargedParticleTransport[m_Object_MakingIndex] = True or false
		std::map<int, double> m_Object_Transparency; // m_Object_Transparency[m_Object_SelectedIndex] = 50;

			// Geometry Widget - All Objects - Global Variables - Setting
			bool b_IsObjectSettingClosed = false; //
			bool b_IsObjectSettingOKClicked = true; // 

		// Geometry Widget - All Objects - Qt Variables
		/**/
			// Geometry Widget - All Objects - Qt Variables - List
			std::map<int, QPushButton*> ObjectSelect_QPushButton;			
			QPushButton* ObjectGroup_QPushButton = nullptr;
			QPushButton* ObjectAdd_QPushButton = nullptr;
			QPushButton* ObjectDelete_QPushButton = nullptr;
			QPushButton* ObjectMoveLeft_UserQPushButton = nullptr;
			QPushButton* ObjectMoveRight_UserQPushButton = nullptr;
			QScrollBar*  ObjectList_QScrollBar = nullptr;
			QHBoxLayout* ObjectList_QHBoxLayout = nullptr;
			
			// Geometry Widget - All Objects - Qt Variables - Setting
			QLabel* ObjectSetting_BoxLength_QLabel = nullptr;
			QLabel* ObjectSetting_BoxThickness_QLabel = nullptr;
			QLabel* ObjectSetting_BoxCenter_QLabel = nullptr;
			QLabel* ObjectSetting_BoxRotate_QLabel = nullptr;
			QLabel* ObjectSetting_SphereRadius_QLabel = nullptr;
			QLabel* ObjectSetting_SphereAzimuthalPhi_QLabel = nullptr;
			QLabel* ObjectSetting_SPolarTheta_QLabel = nullptr;
			QLabel* ObjectSetting_SCenter_QLabel = nullptr;
			QLabel* ObjectSetting_SRotate_QLabel = nullptr;
			QLabel* ObjectSetting_CRadius_QLabel = nullptr;
			QLabel* ObjectSetting_CHeightZ_QLabel = nullptr;
			QLabel* ObjectSetting_CAngle_QLabel = nullptr;
			QLabel* ObjectSetting_CCenter_QLabel = nullptr;
			QLabel* ObjectSetting_CRotate_QLabel = nullptr;
			QLabel* ObjectSetting_Material_QLabel = nullptr;
			QLineEdit* ObjectSetting_Name_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_BoxLengthX_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_BoxLengthY_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_BoxLengthZ_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_BoxThicknessX_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_BoxThicknessY_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_BoxThicknessZ_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_BoxCenterX_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_BoxCenterY_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_BoxCenterZ_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_BoxRotateX_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_BoxRotateY_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_BoxRotateZ_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_SphereInnerRadius_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_SphereOuterRadius_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_SphereStartingAzimuthalPhi_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_SphereDeltaAzimuthalPhi_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_SphereStartingPolarTheta_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_SphereDeltaPolarTheta_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_SphereCenterX_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_SphereCenterY_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_SphereCenterZ_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_SphereRotateX_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_SphereRotateY_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_SphereRotateZ_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_CylinderInnerRadius_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_CylinderOuterRadius_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_CylinderHeightZ_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_CylinderStartingAngle_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_CylinderDeltaAngle_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_CylinderCenterX_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_CylinderCenterY_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_CylinderCenterZ_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_CylinderRotateX_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_CylinderRotateY_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_CylinderRotateZ_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_Density_QLineEdit = nullptr;
			QPushButton* ObjectSetting_MaterialEdit_QPushButton = nullptr;
			QPushButton* ObjectAddingOK_QPushButton = nullptr;
			QComboBox* ObjectSetting_Type_QComboBox = nullptr;
			QRadioButton* ObjectSetting_MaterialMassFraction_QRadioButton = nullptr;
			QRadioButton* ObjectSetting_MaterialAtomFraction_QRadioButton = nullptr;
			QHBoxLayout* ObjectSetting_BoxLength_QHBoxLayout = nullptr;
			QHBoxLayout* ObjectSetting_BoxThickness_QHBoxLayout = nullptr;
			QHBoxLayout* ObjectSetting_BoxCenter_QHBoxLayout = nullptr;
			QHBoxLayout* ObjectSetting_BoxRotate_QHBoxLayout = nullptr;
			QHBoxLayout* ObjectSetting_SphereRadius_QHBoxLayout = nullptr;
			QHBoxLayout* ObjectSetting_SphereAzimuthalPhi_QHBoxLayout = nullptr;
			QHBoxLayout* ObjectSetting_SpherePolarTheta_QHBoxLayout = nullptr;
			QHBoxLayout* ObjectSetting_SphereCenter_QHBoxLayout = nullptr;
			QHBoxLayout* ObjectSetting_SphereRotate_QHBoxLayout = nullptr;
			QHBoxLayout* ObjectSetting_CylinderRadius_QHBoxLayout = nullptr;
			QHBoxLayout* ObjectSetting_CylinderHeightZ_QHBoxLayout = nullptr;
			QHBoxLayout* ObjectSetting_CylinderAngle_QHBoxLayout = nullptr;
			QHBoxLayout* ObjectSetting_CylinderCenter_QHBoxLayout = nullptr;
			QHBoxLayout* ObjectSetting_CylinderRotate_QHBoxLayout = nullptr;
			ObjectMaterialTextWindow* ObjectSetting_MaterialPanelTextWindow_UserQDialog = nullptr;
			MultipleUIDialog* ObjectAdding_UserQDialog = nullptr;

		//-----------------------------------------------------------------------------------------------------------------//
					   
		// Geometry Widget - Box - Global Variables

			// Geometry Widget - Box - Global Variables - Setting
			/**/

		// Geometry Widget - Box - Qt Variables
		/**/
			// Geometry Widget - Box - Qt Variables - Status
			QGroupBox* ObjectBox_QGroupBox = nullptr;
			QLabel* ObjectBox_Type_QLabel = nullptr;
			QLabel* ObjectBox_Length_QLabel = nullptr;
			QLabel* ObjectBox_Thickness_QLabel = nullptr;
			QLabel* ObjectBox_Center_QLabel = nullptr;
			QLabel* ObjectBox_Rotate_QLabel = nullptr;
			QLabel* ObjectBox_Material_QLabel = nullptr;
			QLabel* ObjectBox_Density_QLabel = nullptr;
			QLabel* ObjectBox_Transparency_QLabel = nullptr;
			QLabel* ObjectBox_ChargedParticleTransport_QLabel = nullptr;
			QLineEdit* ObjectBox_Type_QLineEdit = nullptr;
			QLineEdit* ObjectBox_CenterX_QLineEdit = nullptr;
			QLineEdit* ObjectBox_CenterY_QLineEdit = nullptr;
			QLineEdit* ObjectBox_CenterZ_QLineEdit = nullptr;
			QLineEdit* ObjectBox_LengthX_QLineEdit = nullptr;
			QLineEdit* ObjectBox_LengthY_QLineEdit = nullptr;
			QLineEdit* ObjectBox_LengthZ_QLineEdit = nullptr;
			QLineEdit* ObjectBox_RotateX_QLineEdit = nullptr;
			QLineEdit* ObjectBox_RotateY_QLineEdit = nullptr;
			QLineEdit* ObjectBox_RotateZ_QLineEdit = nullptr;
			QLineEdit* ObjectBox_ThicknessX_QLineEdit = nullptr;
			QLineEdit* ObjectBox_ThicknessY_QLineEdit = nullptr;
			QLineEdit* ObjectBox_ThicknessZ_QLineEdit = nullptr;
			QLineEdit* ObjectBox_Density_QLineEdit = nullptr;
			QPushButton* Object_BoxMaterialEdit_QPushButton = nullptr;
			QPushButton* Object_BoxUpdate_QPushButton = nullptr;
			QPushButton* Object_Box_ChargedParticleTransport_QPushButton = nullptr;
			QSlider* Object_BoxTransparency_QSlider = nullptr;
			QRadioButton* ObjectBox_MaterialMassFraction_QRadioButton = nullptr;
			QRadioButton* ObjectBox_MaterialAtomFraction_QRadioButton = nullptr;
			ObjectMaterialTextWindow* ObjectBox_MaterialPanelTextWindow_UserQDialog = nullptr;

		//-----------------------------------------------------------------------------------------------------------------//

		// Geometry Widget - Sphere - Global Variables

			// Geometry Widget - Sphere - Global Variables - Setting
			/**/

		// Geometry Widget - Sphere - Qt Variables
		/**/
			// Geometry Widget - Sphere - Qt Variables - Status
			QGroupBox* ObjectCylinder_QGroupBox = nullptr;
			QLabel* ObjectSphere_Type_QLabel = nullptr;
			QLabel* ObjectSphere_Radius_QLabel = nullptr;
			QLabel* ObjectSphere_AzimuthalPhi_QLabel = nullptr;
			QLabel* ObjectSphere_PolarTheta_QLabel = nullptr;
			QLabel* ObjectSphere_Center_QLabel = nullptr;
			QLabel* ObjectSphere_Rotate_QLabel = nullptr;
			QLabel* ObjectSphere_Material_QLabel = nullptr;
			QLabel* ObjectSphere_Density_QLabel = nullptr;
			QLabel* ObjectSphere_Transparency_QLabel = nullptr;
			QLabel* ObjectSphere_ChargedParticleTransport_QLabel = nullptr;
			QLineEdit* ObjectSphere_Type_QLineEdit = nullptr;
			QLineEdit* ObjectSphere_CenterX_QLineEdit = nullptr;
			QLineEdit* ObjectSphere_CenterY_QLineEdit = nullptr;
			QLineEdit* ObjectSphere_CenterZ_QLineEdit = nullptr;
			QLineEdit* ObjectSphere_OuterRadius_QLineEdit = nullptr;
			QLineEdit* ObjectSphere_InnerRadius_QLineEdit = nullptr;
			QLineEdit* ObjectSphere_StartingAzimuthalPhi_QLineEdit = nullptr;
			QLineEdit* ObjectSphere_DeltaAzimuthalPhi_QLineEdit = nullptr;
			QLineEdit* ObjectSphere_StartingPolarTheta_QLineEdit = nullptr;
			QLineEdit* ObjectSphere_DeltaPolarTheta_QLineEdit = nullptr;
			QLineEdit* ObjectSphere_RotateX_QLineEdit = nullptr;
			QLineEdit* ObjectSphere_RotateY_QLineEdit = nullptr;
			QLineEdit* ObjectSphere_RotateZ_QLineEdit = nullptr;
			QLineEdit* ObjectSphere_Density_QLineEdit = nullptr;
			QPushButton* Object_SphereMaterialEdit_QPushButton = nullptr;
			QPushButton* Object_SphereUpdate_QPushButton = nullptr;
			QPushButton* Object_Sphere_ChargedParticleTransport_QPushButton = nullptr;
			QSlider* Object_SphereTransparency_QSlider = nullptr;
			QRadioButton* ObjectSphere_MaterialMassFraction_QRadioButton = nullptr;
			QRadioButton* ObjectSphere_MaterialAtomFraction_QRadioButton = nullptr;
			ObjectMaterialTextWindow* ObjectSphere_MaterialPanelTextWindow_UserQDialog = nullptr;

		//-----------------------------------------------------------------------------------------------------------------//

		// Geometry Widget - Cylinder - Global Variables

			// Geometry Widget - Cylinder - Global Variables - Setting
			/**/

		// Geometry Widget - Cylinder - Qt Variables
		/**/
			// Geometry Widget - Cylinder - Qt Variables - Status
			QGroupBox* ObjectSphere_QGroupBox = nullptr;
			QLabel* ObjectCylinder_Type_QLabel = nullptr;
			QLabel* ObjectCylinder_Radius_QLabel = nullptr;
			QLabel* ObjectCylinder_HeightZ_QLabel = nullptr;
			QLabel* ObjectCylinder_Angle_QLabel = nullptr;
			QLabel* ObjectCylinder_Center_QLabel = nullptr;
			QLabel* ObjectCylinder_Rotate_QLabel = nullptr;
			QLabel* ObjectCylinder_Material_QLabel = nullptr;
			QLabel* ObjectCylinder_Density_QLabel = nullptr;
			QLabel* ObjectCylinder_Transparency_QLabel = nullptr;
			QLabel* ObjectCylinder_ChargedParticleTransport_QLabel = nullptr;
			QLineEdit* ObjectCylinder_Type_QLineEdit = nullptr;
			QLineEdit* ObjectCylinder_CenterX_QLineEdit = nullptr;
			QLineEdit* ObjectCylinder_CenterY_QLineEdit = nullptr;
			QLineEdit* ObjectCylinder_CenterZ_QLineEdit = nullptr;
			QLineEdit* ObjectCylinder_RotateX_QLineEdit = nullptr;
			QLineEdit* ObjectCylinder_RotateY_QLineEdit = nullptr;
			QLineEdit* ObjectCylinder_RotateZ_QLineEdit = nullptr;
			QLineEdit* ObjectCylinder_OuterRadius_QLineEdit = nullptr;
			QLineEdit* ObjectCylinder_InnerRadius_QLineEdit = nullptr;
			QLineEdit* ObjectCylinder_HeightZ_QLineEdit = nullptr;
			QLineEdit* ObjectCylinder_StartingAngle_QLineEdit = nullptr;
			QLineEdit* ObjectCylinder_DeltaAngle_QLineEdit = nullptr;
			QLineEdit* ObjectCylinder_Density_QLineEdit = nullptr;
			QPushButton* Object_CylinderMaterialEdit_QPushButton = nullptr;
			QPushButton* Object_CylinderUpdate_QPushButton = nullptr;
			QPushButton* Object_Cylinder_ChargedParticleTransport_QPushButton = nullptr;
			QSlider* Object_CylinderTransparency_QSlider = nullptr;
			QRadioButton* ObjectCylinder_MaterialMassFraction_QRadioButton = nullptr;
			QRadioButton* ObjectCylinder_MaterialAtomFraction_QRadioButton = nullptr;
			ObjectMaterialTextWindow* ObjectCylinder_MaterialPanelTextWindow_UserQDialog = nullptr;

		//============================================================================================================================//

	//-----------------------------------------------------------------------------------------------------------------//

////////////////// Source Widget ////////////////// 

// Global int	
	int CurrentSourceType = -1;	
	/*변수이동필요*/int PhaseSpaceFileRow = 0; // Phase Space File의 입자 수(line 수)


		

// Global double

	/*변수이동필요*/double PSF_FileSize; // PSF의 파일 크기
	

// Global bool
	


// QPushButton
	QPushButton* m_sourceOKandResetButton = nullptr; //
	
	QPushButton* ConeBeamRadionuclideButton = nullptr;
	QPushButton* ConeBeamEnergyspectrumButton = nullptr;

// QComboBox
	QComboBox* m_comboBoxSourceGeometry = nullptr;

// QGroupBox
	QGroupBox* SourcePanel_QGroupBox = nullptr;
	QVBoxLayout* m_sourceGeometryContainerLayout = nullptr;
	QList<SourceGeometryWidget*> m_sources;

template <typename T>
    T* getSourceWidget() 
	{
        for (SourceGeometryWidget* widget : m_sources) {
            // 리스트에 있는 위젯을 T 타입으로 변환 시도
            T* specificWidget = qobject_cast<T*>(widget);
            
            // 변환 성공하면(nullptr가 아니면) 그 위젯을 찾은 것임!
            if (specificWidget) {
                return specificWidget;
            }
        }
        return nullptr; // 못 찾았으면 null 반환
    }

	SourceGeometryWidget* getCurrentSourceWidget();

// QStringList
	QStringList RIsourceList; // Completer(자동완성)
//////////////////////////////////////////////////////

////////////////// Output Widget ////////////////// 

// Global int	
	int OutputPhantomSelectedIndex = 0; // Output 패널 상에서 선택된 팬텀 인덱스

// QGroupBox
	QGroupBox* OutputPanel_QGroupBox = nullptr;
	QGroupBox* OutputPanel_OutputPhantomButtons_QGroupBox = nullptr;
	QGroupBox* OutputPanel_Results_QGroupBox = nullptr;
	QGroupBox* InnerResultPanel_Dose_QGroupBox = nullptr;
	QGroupBox* InnerResultPanel_Dosimeter_QGroupBox = nullptr;
	QGroupBox* InnerResultPanel_H_AK_QGroupBox = nullptr;
	QGroupBox* InnerResultPanel_LocalSkin_QGroupBox = nullptr;
// QLabel	
	std::map<int, QLabel*> m_outputPersonalLabelEP;	
// QLineEdit
	std::map<QString, QLineEdit*> OutputPanel_DoseResult_QLineEdit;
	std::map<QString, QLineEdit*> m_outputFluenceB_tmp;
	std::map<QString, QLineEdit*> m_outputAirKermaB;
	std::map<int, QLineEdit*> OutputPanel_DosimeterResult_QLineEdit;
	QLineEdit* m_outputEffDoseforAK_B = nullptr;
	//
	std::map<QString, QLineEdit*> m_outputDoseRateEP;
	std::map<QString, QLineEdit*> m_outputAirKermaEP;
	std::map<int, QLineEdit*> m_outputPersonalEP;
	QLineEdit* m_outputEffDoseforAK_EP = nullptr;
	//	
	std::map<QString, QLineEdit*> m_outputDoseRateFD;
	std::map<QString, QLineEdit*> m_outputAirKermaFD;
	std::map<int, QLineEdit*> m_outputPersonalFD;
	QLineEdit* m_outputEffDoseforAK_FD = nullptr;
	//
	std::map<QString, QLineEdit*> m_outputDoseRateOV;
	std::map<QString, QLineEdit*> m_outputAirKermaOV;
	std::map<int, QLineEdit*> m_outputPersonalOV;
	QLineEdit* m_outputEffDoseforAK_OV = nullptr;
	//
	std::map<QString, QLineEdit*> m_outputDosePS;
	std::map<QString, QLineEdit*> m_outputAirKermaPS;
	std::map<int, QLineEdit*> m_outputPersonalPS;
	QLineEdit* m_outputEffDoseforAK_PS = nullptr;
	//
	std::map<QString, QLineEdit*> m_outputDoseRateHP;
	std::map<QString, QLineEdit*> m_outputAirKermaHP;
	std::map<int, QLineEdit*> m_outputPersonalHP;
	QLineEdit* m_outputEffDoseforAK_HP = nullptr;
	//
	std::map<QString, QLineEdit*> m_outputDoseRC;	
	std::map<QString, QLineEdit*> m_outputAirKermaRC;
	std::map<int, QLineEdit*> m_outputPersonalRC;
	QLineEdit* m_outputEffDoseforAK_RC = nullptr;
// QPushButton
	std::map<int, QPushButton*> OutputPhantomButton;
	QPushButton* m_skinDoseVisualizationButton = nullptr;
// QComboBox	
	QComboBox* m_comboBox_resultType = nullptr;
	QComboBox* m_comboOutputBoxExternalPoint = nullptr;
	QComboBox* m_comboOutputBoxHotParticle = nullptr;
	QComboBox* m_comboOutputBoxFloorDisk = nullptr;
	QComboBox* m_comboOutputBoxObjectVolume = nullptr;
	QComboBox* m_comboOutputBoxPhaseSpace = nullptr;
// QHBoxLayout
	QHBoxLayout* layoutOutputPhantomButtonList = nullptr;
//////////////////////////////////////////////////////

////////////////// Calculation Setting Dialog ////////////////// 
	std::map<QString, QLineEdit*> CalculationSetting_TargetError_QLineEdit;
	QLineEdit* CalculationSetting_TargetNPS_QLineEdit;
	QLineEdit* CalculationSetting_ResultFileName_QLineEdit;
	QLineEdit* CalculationSetting_Thread_QLineEdit;
	QRadioButton* m_RadioButton_SkinDepthDoseOff = nullptr;
	QRadioButton* m_RadioButton_SkinDepthDoseOn = nullptr;
	QRadioButton* m_radioVisualizationModeOff = nullptr;
	QRadioButton* m_radioVisualizationModeOn = nullptr;
	QPushButton* m_settingSaveButton = nullptr;
	std::map<int, QString> m_CalculationSetting_TargetError;
	QString m_CalculationSetting_TargetNPS = "";
	QString m_CalculationSetting_ResultFileName = "";
	QString m_CalculationSetting_Thread = "";
	int SettingVisualizationModeInfo = 0; // Default: Off
	int SettingSkinDepthModeInfo = 0; // Default: Off

//////////////////////////////////////////////////////

////////////////// Calculation Widget ////////////////// 

// Global int	
	int Time_Interval = 4.9; // Calculation 시 UI update 주기 (s)
	int RunningIndex = 0; // UI가 돌아가고 있는 지 누적해서 더해가는 변수
	// /*변수이동필요*/FILETIME OrgandoseFile_PRE_ModifiedTime = { 0,0 };
	// /*변수이동필요*/FILETIME SkindoseFile_PRE_ModifiedTime = { 0,0 };

// Global Qstring
	QString DoseUnit_QString; // 피부선량 환산 unit
	/*변수이동필요*/QString resultDirectoryQstr; // DB에서 받아오는 result 폴더명

// Global bool	
	bool b_IsTargetErrorAbsent = false; // 지정한 Target error가 없으면 true
	bool b_IsTargetNPSAbsent = false; // 지정한 Target nps가 없으면 true
	bool b_IsCalculationExecuted = false; // 계산버튼을 눌러서 실행된 상태
	bool b_IsRealtimeCalculation = false; // 실시간계산중이면 true -> realtime panel show/hide 하기 위함
	bool b_IsOutputResultLoaded = false; // Output이 최초로 한 번이라도 load 되었으면 true -> 아웃풋패널 클릭 혹은 팬텀버튼 클릭 시 결과 업데이트하기 위함

// Global double
	double ProgressBarValue = 0; // ProgressBar의 진행률 값
	double ProgressFactor; // 지정한 업데이트 간격당 진행률 증가정도
	std::uint64_t nEvent = 0; // 현재까지 Geant4에서 수송된 NPS 값	

// Global string
	///*변수이동필요*/std::string timeStamp;

	// For server
	std::string PreCumulative = "NULL"; // 이전의 Cumulative 선량결과 텍스트
	std::string ResultFileName_forServer; // 현재 시간이 기록된 result 텍스트 파일(Simul_info 뒤에 덧붙여짐)	
	std::string ResultFileName_notused; // McSEE 결과파일 이름
			
// QLineEdit	
	QLineEdit* m_Output_H_AK_AmbientDoseEquivaelnt = nullptr;
	QLineEdit* m_Output_H_AK_AirKerma = nullptr;
	QLineEdit* m_Output_H_AK_posX = nullptr;
	QLineEdit* m_Output_H_AK_posY= nullptr;
	QLineEdit* m_Output_H_AK_posZ = nullptr;
	QLineEdit* m_Output_H_AK_radius = nullptr;
	QLineEdit* m_lineEditResultFileName = nullptr;
// QPushButton
	QPushButton* m_CalculateButton = nullptr;
// QVBoxLayout
	QVBoxLayout* OrganDose_BarGraph_QVBoxLayout = nullptr;
	QVBoxLayout* DosimeterDose_BarGraph_QVBoxLayout = nullptr;
	QVBoxLayout* H_AK_BarGraph_QVBoxLayout = nullptr;
	QVBoxLayout* SkinDepthDose_LineGraph_QVBoxLayout = nullptr;
	//
	QVBoxLayout* layoutInnerIn_graphEP = nullptr;
	QVBoxLayout* layoutInnerIn_graphHP = nullptr;
	QVBoxLayout* layoutInnerIn_graphFD = nullptr;
	QVBoxLayout* layoutInnerIn_graphOV = nullptr;
	QVBoxLayout* layoutInnerIn_graphPS = nullptr;
	QVBoxLayout* layoutInnerIn_graphRC = nullptr;
//
	DBServerConnect* serverConnect = nullptr;
	QCustomPlot* OrganDose_BarGraph_QCustomPlot = nullptr;
	QCustomPlot* Dosimeter_BarGraph_QCustomPlot = nullptr;
	QCustomPlot* H_AK_BarGraph_QCustomPlot = nullptr;
	std::map<int, QCustomPlot*> SkinDepthDose_LineGraph_QCustomPlot;
	
//////////////////////////////////////////////////////
	
////////////////// Information Container ////////////////// 
	
// Source
		
	

	
	std::vector<std::map<int, QString>> ListInfo_sourceHP; // RI_Info_sourceRC[pointID][0~2] = 0: 사용여부(True or ""), 1: RI, 2: activity(Bq/cm3)
	
	std::vector<std::map<int, QString>> MultipleHPinfoVector; // MultipleHPinfoVector[point#] = 0: posX, 1: posY, 2: posZ, 3: sepaDist, 4: RI, 5: activity(Bq)	
//////////////////////////////////////////////////////

// Calculation	
	std::map<int, std::map<int, std::pair<QString, double>>> m_graphInfo_OrganDose_map; //m_graphInfo_time_dose_map[phantomID][organ/dosimeter ID] = {OrganNameList, Now dose}
	std::map<int, std::map<int, std::vector<std::pair<double, double>>>> m_graphInfo_TimeDose_map; //m_graphInfo_time_dose_map[phantomID][organ/dosimeter ID][vectorIdx] = {time, dose}
	std::map<int, std::map<int, double>> m_graphInfo_Error_map; //m_graphInfo_Error_map[phantomID][organ/dosimeter ID] = Now error	
	std::map<std::string, double> TargetOrganError_Map;
	QString SkinDose_BinaryFilePath;

//Reconstruction
	bool b_Is_Reconstruction = false;
	void ClosePhantomUIdialog_Recon();
	void CloseClothingUIdialog_Recon();
	void CloseObjectUIdialog_Recon();

//************************************** Common Util **************************************//
public:
	void ExecuteAllButttonClicked();

//************************************** Create MainWidget **************************************//
protected:
	void CreateWidgets();

//************************************** CreateWidget **************************************//
public:
	void CreatePhantomWidgets();

	void Create_Phantom_ListWidgets(QVBoxLayout* layout);	

	QString DefinePreWearablePath(int wearableIndex); // Pre-defined 의복을 입력하는 과정에서 사전제작 의복 obj 파일 경로를 받아오는 함수
	
	void CreateGeometryWidgets();
	void CreateObjectBoxWidgets(QVBoxLayout* layout);
	void CreateObjectSphereWidgets(QVBoxLayout* layout);
	void CreateObjectCylinderWidgets(QVBoxLayout* layout);	
	
	void CreateSourceWidgets();

	void CreateOutputWidgets();
	void CreateOutputPhantomButtonWidget(QVBoxLayout* layout);
	void CreateOutputResultWidget(QVBoxLayout* layout);

	void CreateOutput_Dose(QVBoxLayout* layout);
	void CreateOutput_Dosimeter(QVBoxLayout* layout);
	void CreateOutput_H_AK(QVBoxLayout* layout);
	void CreateOutput_LocalSkin(QVBoxLayout* layout);
	void Create_Dose_BarGraphWidgets(QVBoxLayout* layoutIn_graph); // Output 패널 상에 장기별 선량 막대그래프 초기화
	void Create_Dosimeter_BarGraphWidgets(QVBoxLayout* layoutIn_graph); // Output 패널 상에 선량계 막대그래프 초기화
	void Create_H_AK_BarGraphWidgets(QVBoxLayout* layoutIn_graph); // Output 패널 상에 에어커마/주위선량당량 막대그래프 초기화
	void Create_Skindose_LineGraphWidgets(QVBoxLayout* layout); // Output 패널 상에 Hot particle 피부깊이선량분포 꺾은선그래프 초기화
	

//************************************** Functions **************************************//
public:
	void AllButtonEnabled();
	void AllButtonDisabled(); // 가시화창과 마우스 상호작용 시, 해당 작업이 끝날 때 까지 UI 내의 버튼들 비활성화

	void SetGeometryPanelMode();
	void SetPhantomPanelMode();
	void SetOutputPanelMode();
	void SetSourcePanelMode();	

	void SaveObject_InfoData_InAddingSetting(int objectIndex); // Adding Setting의 데이터를 InfoData로 저장	
	void SaveObject_InfoData(int objectIndex); // Info Status의 데이터를 InfoData로 저장
	void SaveObject_InfoData_InMouseControl(int objectIndex); // 움직인 actor의 데이터를 InfoData로 저장
	void UpdateObject_InfoStatus(int objectIndex); // InfoData의 데이터를 Info Status에 업데이트
	void SetInitialObjectMode();
	void SetBoxObjectMode();
	void SetSphereObjectMode();
	void SetCylinderObjectMode();
	void ObjectAddingSetting_Create();
	void ExecuteObjectAddRoutines();
		
	void UpdateObject_Mode(int objectIndex);
	void UpdateObject_ListButton(int objectIndex);
	

	void SetSourceGeometryType(int index);

	void SetResultType(int user);
	


	void triggerCalculate(); // 가시화창 우측 하단 실행 버튼 클릭 시 실행
	void triggerSetting(); // 가시화창 우측 하단 설정 버튼 클릭 시 실행	
	void Update_SkinDose_1_10cm2_GraphWidgets();
	void Update_SkinDose_HP_LineGraphWidgets(); // checkProcessInfo_Local로 선량계산결과 주기적으로 업데이트 중, Hot particle 선원 피부선량깊이분포 꺾은선그래프 결과 업데이트



//************************************** Slots **************************************//
public slots:	
	void slot_allButtonClicked();

	void slot_Phantom_ButtonClicked();
	void slot_Geometry_ButtonClicked();
	void slot_Source_ButtonClicked();
	void slot_Output_ButtonClicked();
	
	void slot_ChangeGeometeryEnvironment(int idx);
	void slot_ObjectTransparencySlider(int value);
	void slot_ObjectChargedParticleTransport_ButtonClicked();
	void slot_ChangeObjectCombo(int idx);
	void slot_ObjectAddingOK_ButtonClicked();
	
	void slot_ObjectPanel_MaterialEdit_ButtonClicked();
	void slot_ObjectSetting_MaterialEdit_ButtonClicked();	
	void moveMaximumScrollbar_object();
	void slot_OutputPhantomSelect_ButtonClicked();
	void slot_ChangeSourceGeometryCombo(int idx);
	
	void slot_ObjectSelect_ButtonClicked();
	void slot_ObjectAdd_ButtonClicked();
	void slot_ObjectDelete_ButtonClicked();
	void slot_ObjectMoveLeft_ButtonClicked();
	void slot_ObjectMoveRight_ButtonClicked();
	void slot_ObjectUpdate_ButtonClicked();
	
	void slot_SourceOKandReset_ButtonClicked();
	// void slot_BUpdate_ButtonClicked();

	void slot_SkinDoseVisualization_ButtonClicked();
	void slot_Output_resultType(int user);
	
	void slot_settingSave_ButtonClicked();
	void slot_Calculate_ButtonClicked();
	
	/*
	void DoseCalculation_StopAndSave(); // 가시화창 우측 하단 Stop 버튼 클릭 시 실행

	void uiRunning();
	void ServerInitialization();
	void checkProcessInfo_Local(); // 선량계산 중 주기적 결과 업데이트(Local 버전)
	void checkProcessInfo_Server();
	void DataInitialization_Local(); // 선량계산 실행을 위한 데이터 초기화(Local 버전)
	void DataInitialization_Server();
	void Calculate_Local(); // 선량계산 실행(Local 버전)
	void Calculate_Server();

	void handleProcessError(const QString& errorMessage);
	std::vector<std::pair<double, double>> performAdaptiveDownsampling(const std::vector<std::pair<double, double>>& originalData, int targetSize);
	*/

	void Write_TextResultFile_SimulationInfo(std::string filepath);
	void Write_mcsee_File(QString filePath);	
	void Gather_PhantomData_mcsee_File(
		QJsonArray& outPhantomsArray,
		QJsonObject& outManifestObject,
		QStringList& outOrderedResourceIds,
		QMap<QString, QString>& outResourcePathMap,
		qint64& inoutCurrentOffset
	);
	void Gather_GeometryData_mcsee_File(QJsonObject& outGeometryPanelObject);
	void Load_parse_mcsee_FileData(SceneData& outSceneData);
	void Load_commit_mcsee_FileData(const SceneData& sceneData);
	// void Commit_PhantomSetting(const PhantomData& pData);
	void Commit_GeometrySetting(const GeometryData& gData);
	void LoadReconsturctionFile_previous(QString dir); // 피폭상황 및 선량결과 모두 초기화(프로그램 재시작)하고 Recon 파일을 통해 불러오기
	void ResetCalculation(); // 계산 후, Reset 버튼을 통해 입력한 피폭상황을 놔두고, 선량결과만 초기화

	void show_Skindose_LineGraph_ValueTooltip(QMouseEvent *event); // Hot particle 피부깊이선량분포 꺾은선그래프에서 마우스 갖다 대면 피부선량 표시
	void setElidedButtonText(QPushButton* btn); // 버튼 이름 길 때 자동으로 "…" 처럼 생략되고 마우스 오버로 확인 가능

	void MultipleUICloseTrigger(); // @@@@
	
//************************************** Timer **************************************//
public:
	/*
	QTimer *timer_DataInitialization_Local;
	QTimer *timer_DataInitialization_Server;	
	QTimer *timer_uiRunning;
	QTimer *timer_checkProcessInfo_Local;
	QTimer *timer_checkProcessInfo_Server;
	*/
	QTimer *timer_scroll_multiplePhantom;
	QTimer *timer_scroll_dosimeter;
	QTimer *timer_scroll_clothing;
	QTimer *timer_scroll_clothingLayer;
	QTimer *timer_scroll_object;
	QTimer *timer_Close_PhantomUIdialog_Recon;
	QTimer *timer_Close_ClothingUIdialog_Recon;
	QTimer *timer_Close_ObjectUIdialog_Recon;

//*************************************Memory Usage Expectation**************************************//
	double Volume_to_Memory_full(int num_phantom, double volume);
	double Volume_to_Memory_dummy(int num_phantom, double volume);

//*************************************PhantomBox Bound Data**************************************//
	double maxX_PhantomBox;
	double maxY_PhantomBox;
	double maxZ_PhantomBox;
	double minX_PhantomBox;
	double minY_PhantomBox;
	double minZ_PhantomBox;

public slots:
    // [추가] 매니저의 신호를 받을 슬롯
    void onShowProgress(QString message);
    void onHideProgress();
private:
	QMessageBox* m_progressMsgBox = nullptr;
};