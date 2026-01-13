//FunctionPanelRight.cpp include QtComponents (Buttons_FunctionPanelRight, label, comobox, etc.), Slot functions, QtComponents-related functions, and Dose calculation functions (local and server)

#include <windows.h>

#include "ETHuman3DApp.h"
#include "GeometryObjects.h"
#include "PhantomObjects.h"
#include "SourceObjects.h"
#include "Manager_Calculation.h"
#include "pch.h"
#include "FunctionPanelRight.h"
#include "ProcessInformationPanel.h"
#include "RealTimeResultPanel.h"
#include "ETQVTKWidget.h"
#include <direct.h>
#include <time.h>
#include <qstring.h>
#include <qstringalgorithms.h>
#include "QHoverSensitiveButton.h"
#include "vtkSmoothPolyDataFilter.h"
#include "vtkDecimatePro.h"
#include "vtkQuadricDecimation.h"
#include "vtkRemoveDuplicatePolys.h"
#include "vtkMergePoints.h"
#include <unordered_map>
#include <unordered_set>
#include "vtkTriangle.h"
#include "vtkIntersectionPolyDataFilter.h"
#include "vtkKdTree.h"
#include "vtkCubeSource.h"
#include "vtkClipPolyData.h"
#include "qprocess.h"
#include <qtimer.h>
#include <qfile.h>
#include <qdir.h>
#include <qvalidator.h>
#include <string>
#include <qdebug.h>
#include "QHoverSensitiveButton.h"
#include <Windows.h>
#include <Psapi.h>
#include "SimpleLogger.h"

#include "vtkOBJWriter.h"

#include "Util.h"

#include <vtkUnstructuredGridReader.h>
#include <vtkDataSetMapper.h>
#include <vtkLookupTable.h>

//************************************** Pre-defined Class/Struct **************************************//

//************************************** Pre-defined Functions/Variables **************************************//
#ifndef getmax
#define getmax(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef getmin
#define getmin(a,b)            (((a) < (b)) ? (a) : (b))
#endif


//************************************** Common Util **************************************//
void FunctionPanelRight::ExecuteAllButttonClicked()
{
	slot_allButtonClicked();
}
void FunctionPanelRight::AllButtonEnabled()
{	 
	for (QList<QPushButton*>::iterator it = Buttons_FunctionPanelRight.begin(); it != Buttons_FunctionPanelRight.end(); ++it) 
	{
		(*it)->setEnabled(true);
	}
}
void FunctionPanelRight::AllButtonDisabled()
{
	for (QList<QPushButton*>::iterator it = Buttons_FunctionPanelRight.begin(); it != Buttons_FunctionPanelRight.end(); ++it)
	{
		(*it)->setDisabled(true);
	}
}
void FunctionPanelRight::setElidedButtonText(QPushButton* btn)
{
	// 1) 버튼에 이미 들어있는 text를 원본(fullText)라 가정
	const QString fullText = btn->text();

	// 2) elide 처리
	QFontMetrics fm(btn->font());
	const auto margins = btn->contentsMargins();
	int availW = btn->width() - margins.left() - margins.right();
	QString elided = fm.elidedText(fullText, Qt::ElideRight, availW);
	btn->setText(elided);

	// 3) 기본 툴팁 폰트 크기 불러오기
	QFont defaultTipFont = QToolTip::font();
	qreal baseSize = defaultTipFont.pointSizeF();
	if (baseSize < 0) {
		// pointSizeF()가 유효하지 않다면 pixelSize() 사용
		baseSize = defaultTipFont.pixelSize();
	}
	qreal scaledSize = baseSize * 1.5;

	// 4) rich HTML로 폰트 크기 적용
	QString richTip = QString(
		"<qt><span style=\"font-size:%1pt;\">%2</span></qt>"
	).arg(scaledSize).arg(fullText);

	btn->setToolTip(richTip);
}

//************************************** Constructor and Destructor **************************************//
FunctionPanelRight::FunctionPanelRight(QWidget *parent)
	: QWidget(parent)
{
	// ===== 로깅 시스템 초기화 (맨 처음에 추가) =====
	QString logPath = QCoreApplication::applicationDirPath() + "/process_debug.log";
	SimpleLogger::initialize();
	LOG_INFO("FunctionPanelRight constructor started");

////////////////// UI resolution //////////////////
	Window_width *= Default_RatioWtoH / RatioWtoH; // 화면비 calibration
	
	//theApp.SetMessageBox("Logical DPI: " + QString::number(LogicalDPI) + " Physical DPI: " + QString::number(PhysicalDPI) + " Window_width: " + QString::number(Window_width) + " Window_height: " + QString::number(Window_height));
	//theApp.SetMessageBox("PhysicalSizeInches: " + QString::number(PhysicalSizeInches) + " PhysFactor: " + QString::number(PhysFactor));
	//theApp.SetMessageBox("FontSize: " + QString::number(FontSizeScaling(10)));
	font_D_BTN1.setPointSize(FontSizeScaling(10));
	font_D_BTN2.setPointSize(FontSizeScaling(10));
	font_D_BTN3.setPointSize(FontSizeScaling(10));
	font_D_BTN4.setPointSize(FontSizeScaling(10));
	font_D_BTN5.setPointSize(FontSizeScaling(10));
	font_D_BTN6.setPointSize(FontSizeScaling(10));
	font_D_BTN7.setPointSize(FontSizeScaling(10));
	font_D_BTN8.setPointSize(FontSizeScaling(10));
	font_D_BTN9.setPointSize(FontSizeScaling(10));
	font_D_BTN10.setPointSize(FontSizeScaling(10));
	font_D_BTN11.setPointSize(FontSizeScaling(10));
	font_D_BTN12.setPointSize(FontSizeScaling(10));
	font_D_BTN13.setPointSize(FontSizeScaling(7));
	font_D_BTN14.setPointSize(FontSizeScaling(8));
	font_D_BTN15.setPointSize(FontSizeScaling(5));
	font_D_BTN16.setPointSize(FontSizeScaling(8));
	font_D_DE1.setPointSize(FontSizeScaling(10));
	font_D_RB1.setPointSize(FontSizeScaling(11));
	font_D_RB2.setPointSize(FontSizeScaling(8));
	font_D_RB3.setPointSize(FontSizeScaling(9));
	font_D_GBW1.setPointSize(FontSizeScaling(10));
	font_D_GBW2.setPointSize(FontSizeScaling(10));
	font_D_GBW3.setPointSize(FontSizeScaling(10));
	font_D_GBW4.setPointSize(FontSizeScaling(10));
	font_D_GBW5.setPointSize(FontSizeScaling(10));
	font_D_GBW6.setPointSize(FontSizeScaling(10));
	font_D_GBW7.setPointSize(FontSizeScaling(10));
	font_D_GBW8.setPointSize(FontSizeScaling(10));
	font_D_GBW9.setPointSize(FontSizeScaling(10));
	font_D_GBW10.setPointSize(FontSizeScaling(10));
	font_D_GBW11.setPointSize(FontSizeScaling(10));
	font_D_TBW1.setPointSize(FontSizeScaling(10));
	font_D_LBL1.setPointSize(FontSizeScaling(10));
	font_D_LBL2.setPointSize(FontSizeScaling(10));
	font_D_LBL3.setPointSize(FontSizeScaling(10));
	font_D_LBL4.setPointSize(FontSizeScaling(7));
	font_D_LBL5.setPointSize(FontSizeScaling(8));
	font_D_CB1.setPointSize(FontSizeScaling(10));
	font_D_CB2.setPointSize(FontSizeScaling(10));
	font_D_CB3.setPointSize(FontSizeScaling(8));
	font_D_LW1.setPointSize(FontSizeScaling(12));
	font_D_LW2.setPointSize(FontSizeScaling(7));
	font_D_LE1.setPointSize(FontSizeScaling(10));
	font_D_LE2.setPointSize(FontSizeScaling(10));
	font_D_LE3.setPointSize(FontSizeScaling(10));
	font_D_LE4.setPointSize(FontSizeScaling(8));
	font_D_LE5.setPointSize(FontSizeScaling(7));
	font_D_MAT.setPointSize(FontSizeScaling(10));
	//////////////////////////////////////////////////////

	// 자동완성 리스트
	QDir dir("./data/radioisotopes");
	QStringList fileNameList = dir.entryList(QDir::Files);
	for (auto itr : fileNameList)
	{
		QFileInfo fileInfo(itr);
		RIsourceList << fileInfo.baseName(); // without Extension(.txt)
	}

	////////////////// Timer //////////////////
	/*
	timer_uiRunning = new QTimer(this);
	connect(timer_uiRunning, &QTimer::timeout, this, &FunctionPanelRight::uiRunning);

	theApp.managerCalculation->m_timer_LocalCheck = new QTimer(this);
	connect(theApp.managerCalculation->m_timer_LocalCheck, &QTimer::timeout, this, &FunctionPanelRight::checkProcessInfo_Local);

	timer_checkProcessInfo_Server = new QTimer(this);
	connect(timer_checkProcessInfo_Server, &QTimer::timeout, this, &FunctionPanelRight::checkProcessInfo_Server);

	theApp.managerCalculation->m_timer_LocalInit = new QTimer(this);
	connect(theApp.managerCalculation->m_timer_LocalInit, &QTimer::timeout, this, &FunctionPanelRight::DataInitialization_Local);

	timer_DataInitialization_Server = new QTimer(this);
	connect(timer_DataInitialization_Server, &QTimer::timeout, this, &FunctionPanelRight::DataInitialization_Server);
	*/

	//Timer for moving scroll bar just after something...
	timer_scroll_multiplePhantom = new QTimer(this);
	connect(timer_scroll_multiplePhantom, &QTimer::timeout, this, &FunctionPanelRight::moveMaximumScrollbar_phantom);

	timer_scroll_dosimeter = new QTimer(this);
	connect(timer_scroll_dosimeter, &QTimer::timeout, this, &FunctionPanelRight::moveMaximumScrollbar_dosimeter);

	timer_scroll_clothing = new QTimer(this);
	connect(timer_scroll_clothing, &QTimer::timeout, this, &FunctionPanelRight::moveMaximumScrollbar_clothing);

	timer_scroll_clothingLayer = new QTimer(this);
	connect(timer_scroll_clothingLayer, &QTimer::timeout, this, &FunctionPanelRight::moveMaximumScrollbar_clothingLayer);
	
	timer_scroll_object = new QTimer(this);
	connect(timer_scroll_object, &QTimer::timeout, this, &FunctionPanelRight::moveMaximumScrollbar_object);

	timer_Close_PhantomUIdialog_Recon = new QTimer(this);
	timer_Close_PhantomUIdialog_Recon->setSingleShot(true);
	connect(timer_Close_PhantomUIdialog_Recon, &QTimer::timeout, this, &FunctionPanelRight::ClosePhantomUIdialog_Recon);

	timer_Close_ClothingUIdialog_Recon = new QTimer(this);
	timer_Close_ClothingUIdialog_Recon->setSingleShot(true);
	connect(timer_Close_ClothingUIdialog_Recon, &QTimer::timeout, this, &FunctionPanelRight::CloseClothingUIdialog_Recon);

	timer_Close_ObjectUIdialog_Recon = new QTimer(this);
	timer_Close_ObjectUIdialog_Recon->setSingleShot(true);
	connect(timer_Close_ObjectUIdialog_Recon, &QTimer::timeout, this, &FunctionPanelRight::CloseObjectUIdialog_Recon);
	//////////////////////////////////////////////////////

//////////////////////////////////////// Set widget ////////////////////////////////////////////

	//Set parent UI
	setParent(parent);
	setStyleSheet(ControlPanelSTyle::BACKGROUND_WIDGET);
	this->setContextMenuPolicy(Qt::PreventContextMenu);

	// size
	setFixedWidth(Window_width * 0.2083);
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

	// 생성
	CreateWidgets();
	//////////////////////////////////////////////////////

	m_progressMsgBox = new QMessageBox(this);
    m_progressMsgBox->setWindowFlag(Qt::WindowStaysOnTopHint);
    m_progressMsgBox->setStandardButtons(QMessageBox::NoButton);
    m_progressMsgBox->setWindowTitle("Processing");
    
    QFont font;
    font.setFamily("Arial");
    font.setPointSize(FontSizeScaling(10)); 
    font.setWeight(QFont::Bold);
    m_progressMsgBox->setFont(font);

    // [2] 매니저와 신호 연결 (Manager 생성 후)
	if (theApp.managerCalculation) 
    {
        connect(theApp.managerCalculation, &Manager_Calculation::requestShowProgress, 
                this, &FunctionPanelRight::onShowProgress);
                
        connect(theApp.managerCalculation, &Manager_Calculation::requestHideProgress, 
                this, &FunctionPanelRight::onHideProgress);
    }
    else 
    {
        // 만약 순서 문제로 아직 생성이 안 되었다면 로그를 남기거나 예외 처리가 필요할 수 있습니다.
        // qDebug() << "Error: Manager_Calculation is not initialized yet!";
    }

}
FunctionPanelRight::~FunctionPanelRight()
{
	SAFE_DELETE(Phantom_QPushButton);
	SAFE_DELETE(Geometry_QPushButton);
	SAFE_DELETE(Source_QPushButton);
	SAFE_DELETE(Output_QPushButton);

	SAFE_DELETE(PhantomHeight_QLineEdit);
	SAFE_DELETE(PhantomWeight_QLineEdit);
	
	SAFE_DELETE(EyewearDistance_QLineEdit);
	SAFE_DELETE(EyewearRadius_QLineEdit);
	SAFE_DELETE(EyewearThickness_QLineEdit);
	SAFE_DELETE(EyewearDensity_QLineEdit);
	SAFE_DELETE(EyewearComposition_QComboBox);
		
	SAFE_DELETE(ObjectBox_CenterX_QLineEdit);
	SAFE_DELETE(ObjectBox_CenterY_QLineEdit);
	SAFE_DELETE(ObjectBox_CenterZ_QLineEdit);
	SAFE_DELETE(ObjectBox_LengthX_QLineEdit);
	SAFE_DELETE(ObjectBox_LengthY_QLineEdit);
	SAFE_DELETE(ObjectBox_LengthZ_QLineEdit);
	SAFE_DELETE(ObjectBox_RotateX_QLineEdit);
	SAFE_DELETE(ObjectBox_RotateY_QLineEdit);
	SAFE_DELETE(ObjectBox_RotateZ_QLineEdit);

	SAFE_DELETE(ObjectSphere_CenterX_QLineEdit);
	SAFE_DELETE(ObjectSphere_CenterY_QLineEdit);
	SAFE_DELETE(ObjectSphere_CenterZ_QLineEdit);
	SAFE_DELETE(ObjectSphere_OuterRadius_QLineEdit);
	SAFE_DELETE(ObjectSphere_InnerRadius_QLineEdit);

	SAFE_DELETE(ObjectCylinder_CenterX_QLineEdit);
	SAFE_DELETE(ObjectCylinder_CenterY_QLineEdit);
	SAFE_DELETE(ObjectCylinder_CenterZ_QLineEdit);
	SAFE_DELETE(ObjectCylinder_HeightZ_QLineEdit);
	SAFE_DELETE(ObjectCylinder_RotateX_QLineEdit);
	SAFE_DELETE(ObjectCylinder_RotateY_QLineEdit);
	SAFE_DELETE(ObjectCylinder_RotateZ_QLineEdit);
	SAFE_DELETE(ObjectCylinder_OuterRadius_QLineEdit);
	SAFE_DELETE(ObjectCylinder_InnerRadius_QLineEdit);

	SAFE_DELETE(m_comboBoxSourceGeometry);

	SAFE_DELETE(m_CalculateButton);	
	SAFE_DELETE(PhantomPanel_QGroupBox);
	SAFE_DELETE(GeometryPanel_QGroupBox);
	SAFE_DELETE(SourcePanel_QGroupBox);
	SAFE_DELETE(OutputPanel_QGroupBox);

	qDeleteAll(m_sources);
	m_sources.clear();

	LOG_INFO("FunctionPanelRight destructor called");
	SimpleLogger::cleanup();
}

//************************************** Create Main Widget **************************************//
void FunctionPanelRight::CreateWidgets()
{
	QVBoxLayout* mainLayout = new QVBoxLayout;
	mainLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	mainLayout->setContentsMargins(Window_width * 0.0015, Window_height * 0.002, Window_width * 0.0015, Window_height * 0.002);

	// 상단 팬텀, 소스 버튼 레이아웃 및 위젯
	{
		// 가로레이아웃 생성
		QHBoxLayout* layout = new QHBoxLayout;
		layout->setSpacing(Window_width * 0.003);
		layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
		layout->setContentsMargins(Window_width * 0.002, Window_height * 0.002, 0, 0);
				
		// Shielding 버튼생성
		Geometry_QPushButton = new QPushButton(QString::fromLocal8Bit("Geometry"));
		Geometry_QPushButton->setObjectName("pGeometry");
		Buttons_FunctionPanelRight.append(Geometry_QPushButton);
		Geometry_QPushButton->setFixedSize(Window_width * 0.049, Window_height * 0.037);
		Geometry_QPushButton->setCheckable(true);
		Geometry_QPushButton->setStyleSheet(DialogStyle::MENU_BTN4);	
		Geometry_QPushButton->setFont(font_D_BTN4);

		// Phantom 버튼생성
		Phantom_QPushButton = new QPushButton(QString::fromLocal8Bit("Phantom"));
		Phantom_QPushButton->setObjectName("pPhantom");
		Buttons_FunctionPanelRight.append(Phantom_QPushButton);
		Phantom_QPushButton->setFixedSize(Window_width * 0.049, Window_height * 0.037);
		Phantom_QPushButton->setCheckable(true);
		Phantom_QPushButton->setStyleSheet(DialogStyle::MENU_BTN4);
		Phantom_QPushButton->setFont(font_D_BTN4);

		// Source 버튼생성
		Source_QPushButton = new QPushButton(QString::fromLocal8Bit("Source"));
		Source_QPushButton->setObjectName("pSource");
		Buttons_FunctionPanelRight.append(Source_QPushButton);
		Source_QPushButton->setFixedSize(Window_width * 0.049, Window_height * 0.037);
		Source_QPushButton->setCheckable(true);
		Source_QPushButton->setStyleSheet(DialogStyle::MENU_BTN4);
		Source_QPushButton->setFont(font_D_BTN4);

		// Output 버튼생성
		Output_QPushButton = new QPushButton(QString::fromLocal8Bit("Output"));
		Output_QPushButton->setObjectName("pOutput");
		Buttons_FunctionPanelRight.append(Output_QPushButton);
		Output_QPushButton->setFixedSize(Window_width * 0.049, Window_height * 0.037);
		Output_QPushButton->setCheckable(true);
		Output_QPushButton->setStyleSheet(DialogStyle::MENU_BTN4);
		Output_QPushButton->setFont(font_D_BTN4);

		if (E_PHANTOM == theApp.PanelMode)
		{
			Phantom_QPushButton->setChecked(true);
			Geometry_QPushButton->setChecked(false);
			Source_QPushButton->setChecked(false);
			Output_QPushButton->setChecked(false);
		}
		else if (E_GEOMETRY == theApp.PanelMode)
		{
			Phantom_QPushButton->setChecked(false);
			Geometry_QPushButton->setChecked(true);
			Source_QPushButton->setChecked(false);
			Output_QPushButton->setChecked(false);
		}
		else if (E_SOURCE == theApp.PanelMode)
		{
			Phantom_QPushButton->setChecked(false);
			Geometry_QPushButton->setChecked(false);
			Source_QPushButton->setChecked(true);
			Output_QPushButton->setChecked(false);
		}
		else if (E_OUTPUT == theApp.PanelMode)
		{
			Phantom_QPushButton->setChecked(false);
			Geometry_QPushButton->setChecked(false);
			Source_QPushButton->setChecked(false);
			Output_QPushButton->setChecked(true);
		}

		// 시그널 이벤트
		connect(Phantom_QPushButton, SIGNAL(clicked()), this, SLOT(slot_Phantom_ButtonClicked())); connect(Phantom_QPushButton, SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
		connect(Geometry_QPushButton, SIGNAL(clicked()), this, SLOT(slot_Geometry_ButtonClicked())); connect(Geometry_QPushButton, SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
		connect(Source_QPushButton, SIGNAL(clicked()), this, SLOT(slot_Source_ButtonClicked())); connect(Source_QPushButton, SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
		connect(Output_QPushButton, SIGNAL(clicked()), this, SLOT(slot_Output_ButtonClicked())); connect(Output_QPushButton, SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));

		// 위젯 추가
		layout->addWidget(Geometry_QPushButton);
		layout->addWidget(Phantom_QPushButton);		
		layout->addWidget(Source_QPushButton);
		layout->addWidget(Output_QPushButton);

		// 레이아웃 메인레이아웃에 추가
		mainLayout->addLayout(layout);
	}
	
	// 버튼 하단 그룹 위젯 생성	
	theApp.LoadingBar->setFormat("Constructing Geometry Panel");
	theApp.LoadingBar->setValue(20);
	CreateGeometryWidgets();
	theApp.LoadingBar->setFormat("Constructing Phantom Panel");
	theApp.LoadingBar->setValue(65);
	CreatePhantomWidgets();
	theApp.LoadingBar->setFormat("Constructing Source Panel");
	theApp.LoadingBar->setValue(80);
	CreateSourceWidgets();
	theApp.LoadingBar->setFormat("Constructing Output Panel");
	theApp.LoadingBar->setValue(90);
	CreateOutputWidgets();

	mainLayout->addWidget(GeometryPanel_QGroupBox);
	mainLayout->addWidget(PhantomPanel_QGroupBox);	
	mainLayout->addWidget(SourcePanel_QGroupBox);	
	mainLayout->addWidget(OutputPanel_QGroupBox);	

	setLayout(mainLayout);

	GeometryPanel_QGroupBox->show();
	PhantomPanel_QGroupBox->hide();	
	SourcePanel_QGroupBox->hide();
	OutputPanel_QGroupBox->hide();	
}
void FunctionPanelRight::slot_Phantom_ButtonClicked()
{
	// Exceptions
	if (theApp.PanelMode == E_PHANTOM) {
		Phantom_QPushButton->setChecked(true);
		return;
	}
	SetPhantomPanelMode();	
		
	for (auto itr_phantom : m_Phantom_SequenceVector) // 최대 팬텀 개수
	{
		if (itr_phantom == m_Phantom_SelectedIndex) // 선택된 팬텀버튼
		{
			PhantomSelect_QPushButton[itr_phantom]->setChecked(true);			
		}
		else // 나머지 팬텀버튼
		{
			PhantomSelect_QPushButton[itr_phantom]->setChecked(false);
			
		}
	}
	if (m_Phantom_SelectedIndex != -1) // 생성된 팬텀이 있을 때
	{
		UpdatePhantom_InfoStatus(m_Phantom_SelectedIndex);

		// Wearable 정보 불러오기 

		////////////////////////////////////////////////Wearable 초기화 		

		theApp.UpdatePhantom_ActorHighlighted(m_Phantom_SelectedIndex); // 다시 팬텀패널로 돌아왔을때 현재 선택된 팬텀만 pickable and highlight
	}
	
}
void FunctionPanelRight::slot_Geometry_ButtonClicked()
{	
	if (theApp.PanelMode == E_GEOMETRY) {
		Geometry_QPushButton->setChecked(true);
		return;
	}

	SetGeometryPanelMode();
	
	theApp.UpdatePhantom_ActorHighlighted(const_Phantom_MaximumCount); // 전부 highlight and pickable 끄기
}
void FunctionPanelRight::slot_Source_ButtonClicked()
{
	// Exceptions	
	if (theApp.PanelMode == E_SOURCE) {
		Source_QPushButton->setChecked(true);
		return;
	}

	SetSourcePanelMode();	

	if (m_comboBoxSourceGeometry->currentIndex() == 5) // Hot particle exception
	{
		if (m_Phantom_SequenceVector.size() != 1)
		{
			theApp.SetMessageBox("Hot particle source can be used only for the single phantom!");
			// RESET PROCESS
			SetSourceGeometryType(-1);
			m_comboBoxSourceGeometry->setEnabled(true);
			m_sourceOKandResetButton->setText("OK");
			theApp.st_Pick3D_sourceCB = false;
			theApp.st_Pick3D_sourceEP = false;
			// etc....
		}
		else
		{
			theApp.UpdatePhantom_ActorHighlighted(m_Phantom_SequenceVector[0]);
		}
	}

	
}
void FunctionPanelRight::slot_Output_ButtonClicked()
{
	// Exceptions
	if (!b_IsCalculationExecuted) // 결과는 Calculation 실행 이후에 접근 가능
	{
		Output_QPushButton->setChecked(false);
		return;
	}

	if (theApp.PanelMode == E_OUTPUT) {
		Output_QPushButton->setChecked(true);
		return;
	}

	SetOutputPanelMode();

	if (b_IsCalculationExecuted) // Calculate 이후에는 쭉 output 팬텀버튼과 동기화상태
	{
		if (m_Phantom_MainInfo[m_Phantom_SelectedIndex][E_PHANTOMMAININFO_DUMMY] == E_PHANTOMDUMMY_YES) // phantom 패널에서 더미팬텀을 선택하고 있을때
		{
			for (auto itr_phantomID : m_Phantom_SequenceVector)
			{
				if (m_Phantom_MainInfo[itr_phantomID][E_PHANTOMMAININFO_DUMMY] == E_PHANTOMDUMMY_YES)
				{
					OutputPhantomSelectedIndex = itr_phantomID;
					break;
				}
			}
		}
		else OutputPhantomSelectedIndex = m_Phantom_SelectedIndex; //팬텀패널에서 풀팬텀 선택하고 있으면 그대로 아웃풋팬텀 인덱스
		
		for (auto phantomID: m_Phantom_SequenceVector) // 최대 팬텀 개수
		{
			if (phantomID == OutputPhantomSelectedIndex) // 선택된 팬텀버튼
			{
				OutputPhantomButton[phantomID]->setChecked(true);
			}
			else // 나머지 팬텀버튼
			{
				OutputPhantomButton[phantomID]->setChecked(false);
			}
		}
		theApp.UpdatePhantom_ActorHighlighted(OutputPhantomSelectedIndex);
	}

	if (b_IsOutputResultLoaded) // 실시간 선량계산 중 최초로 output load된 이후 
	{
		// 선량결과 output 보이기
		for (int i = 0; i < OrganNameList.size(); i++)
		{
			//theApp.SetMessageBox(theApp.multiple_outputDoseRateEP[OutputPhantomSelectedIndex][i][0]);
			OutputPanel_DoseResult_QLineEdit[theApp.OutputDose_data[OutputPhantomSelectedIndex][i][0]]->setText(theApp.OutputDose_data[OutputPhantomSelectedIndex][i][1]);
		}
		// Time-Dose, organ-dose 그래프 보이기
		if (b_IsRealtimeCalculation) theApp.pRslt->ForcedGraphUpdate();
		if (!b_IsRealtimeCalculation) theApp.pRslt->ForcedGraphUpdate_OrganDose();
	}
}

//************************************** Phantom Widget **************************************//
void FunctionPanelRight::CreatePhantomWidgets()
{
	// 그룹박스생성
	PhantomPanel_QGroupBox = new QGroupBox();
	PhantomPanel_QGroupBox->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	PhantomPanel_QGroupBox->setStyleSheet(DialogStyle::GROUPBOX_WIDGET);
	PhantomPanel_QGroupBox->setFont(font_D_GBW1);
	PhantomPanel_QGroupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	QVBoxLayout* layout = new QVBoxLayout;
	layout->setContentsMargins(Window_width * 0.003, Window_height * 0.008, Window_width * 0.003, 0);
	layout->setSpacing(Window_width * 0.003);
	layout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

	PhantomPanel_QGroupBox->setLayout(layout);

	{
		// Calculate physicalRAM in Windows PC
		MEMORYSTATUSEX statex;
		statex.dwLength = sizeof(statex);
		GlobalMemoryStatusEx(&statex);
		double totalPhysMemGB = (double)statex.ullTotalPhys / (1024 * 1024 * 1024);
		m_Phantom_RAM_Usage = static_cast<int>(std::ceil(totalPhysMemGB));

		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(0, Window_height * 0.001, Window_width * 0.005, Window_height * 0.001);
		
		QLabel* labelRAMtitle = new QLabel;
		labelRAMtitle->setFixedSize(Window_width * 0.115, Window_height * 0.018);
		labelRAMtitle->setStyleSheet(DialogStyle::DATA_LABEL);
		labelRAMtitle->setFont(font_D_LBL1);
		labelRAMtitle->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		labelRAMtitle->setText(QString::fromLocal8Bit("Memory Usage: "));

		Phantom_RAMusage_QLabel = new QLabel;
		Phantom_RAMusage_QLabel->setFixedSize(Window_width * 0.085, Window_height * 0.018);
		Phantom_RAMusage_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		Phantom_RAMusage_QLabel->setFont(font_D_LBL1);
		Phantom_RAMusage_QLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		Phantom_RAMusage_QLabel->setText(QString::fromLocal8Bit(" GB"));

		hLayout->addWidget(labelRAMtitle);
		hLayout->addWidget(Phantom_RAMusage_QLabel);

		layout->addLayout(hLayout);
	}

	// Phantom
	Create_Phantom_ListWidgets(layout);	// Multiple phantom widget 생성

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.005); // 지금 vertical line에서 생성된 버튼들 사이의 horizontal spacing
		hLayout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		hLayout->setContentsMargins(Window_width * 0.005, Window_height * 0.005, Window_width * 0.005, Window_height * 0.005); // 최초 버튼 생성하는 마진
		
		// 버튼생성
		PhantomAdd_QPushButton = new QPushButton(QString::fromLocal8Bit("Add"));
		Buttons_FunctionPanelRight.append(PhantomAdd_QPushButton);
		PhantomAdd_QPushButton->setFixedSize(Window_width * 0.030, Window_height * 0.025);
		PhantomAdd_QPushButton->setCheckable(false);
		PhantomAdd_QPushButton->setStyleSheet(DialogStyle::MENU_BTN3);
		PhantomAdd_QPushButton->setFont(font_D_BTN3);

		// 버튼생성
		PhantomDelete_QPushButton = new QPushButton(QString::fromLocal8Bit("Delete"));
		Buttons_FunctionPanelRight.append(PhantomDelete_QPushButton);
		PhantomDelete_QPushButton->setFixedSize(Window_width * 0.030, Window_height * 0.025);
		PhantomDelete_QPushButton->setCheckable(false);
		PhantomDelete_QPushButton->setStyleSheet(DialogStyle::MENU_BTN3);
		PhantomDelete_QPushButton->setFont(font_D_BTN3);
		
		// 버튼생성
		PhantomMoveLeft_UserQPushButton = new LeftButton(this);
		Buttons_FunctionPanelRight.append(PhantomMoveLeft_UserQPushButton);
		PhantomMoveLeft_UserQPushButton->setFixedSize(Window_width * 0.012, Window_height * 0.025);
		// Apply stylesheet for color and shape
		PhantomMoveLeft_UserQPushButton->setStyleSheet("QPushButton {"
			"background-color: white;"
			"border: 0px;" // Half of the width/height for circular shape
			"}");

		// 버튼생성
		PhantomMoveRight_UserQPushButton = new RightButton(this);
		Buttons_FunctionPanelRight.append(PhantomMoveRight_UserQPushButton);
		PhantomMoveRight_UserQPushButton->setFixedSize(Window_width * 0.012, Window_height * 0.025);
		// Apply stylesheet for color and shape
		PhantomMoveRight_UserQPushButton->setStyleSheet("QPushButton {"
			"background-color: white;"
			"border: 0px;" // Half of the width/height for circular shape
			"}");
		
		connect(PhantomAdd_QPushButton, SIGNAL(clicked()), this, SLOT(slot_PhantomAdd_ButtonClicked())); connect(PhantomAdd_QPushButton, SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
		connect(PhantomDelete_QPushButton, SIGNAL(clicked()), this, SLOT(slot_PhantomDelete_ButtonClicked())); connect(PhantomDelete_QPushButton, SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
		connect(PhantomMoveLeft_UserQPushButton, SIGNAL(clicked()), this, SLOT(slot_PhantomMoveLeft_ButtonClicked())); connect(PhantomMoveLeft_UserQPushButton, SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
		connect(PhantomMoveRight_UserQPushButton, SIGNAL(clicked()), this, SLOT(slot_PhantomMoveRight_ButtonClicked())); connect(PhantomMoveRight_UserQPushButton, SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
			
		
		hLayout->addWidget(PhantomAdd_QPushButton);
		hLayout->addWidget(PhantomDelete_QPushButton);
		/*hLayout->addWidget(PhantomMoveLeft_UserQPushButton);
		hLayout->addWidget(PhantomMoveRight_UserQPushButton);*/

		layout->addLayout(hLayout);
	}

	QGroupBox* GroupBoxIn = new QGroupBox;
	GroupBoxIn->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	GroupBoxIn->setStyleSheet(DialogStyle::GROUPBOX_WIDGET);
	GroupBoxIn->setFont(font_D_GBW1);
	GroupBoxIn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	QGroupBox* GroupBoxOut = new QGroupBox;
	GroupBoxOut->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	GroupBoxOut->setStyleSheet(DialogStyle::GROUPBOX_WIDGET);
	GroupBoxOut->setFont(font_D_GBW1);
	GroupBoxOut->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	QVBoxLayout* layoutIn = new QVBoxLayout();
	layoutIn->setContentsMargins(Window_width * 0.003, 0, Window_width * 0.003, 0);
	layoutIn->setSpacing(Window_width * 0.003);
	layoutIn->setAlignment(Qt::AlignLeft | Qt::AlignTop);

	QVBoxLayout* layoutOut = new QVBoxLayout();
	layoutOut->setContentsMargins(Window_width * 0.003, 0, Window_width * 0.003, 0);
	layoutOut->setSpacing(Window_width * 0.003);
	layoutOut->setAlignment(Qt::AlignLeft | Qt::AlignTop);

	QScrollArea* scrollArea = new QScrollArea();
	scrollArea->setStyleSheet(DialogStyle::GROUPBOX_WIDGET);
	scrollArea->setFont(font_D_GBW1);
	scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	PhantomList_Vertical_QScrollBar = scrollArea->verticalScrollBar();

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(0, Window_height * 0.005, 0, 0);

		QLabel* labelType = new QLabel;
		labelType->setFixedSize(Window_width * 0.022, Window_height * 0.028);
		labelType->setStyleSheet(DialogStyle::DATA_LABEL);
		labelType->setFont(font_D_LBL5);
		labelType->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		labelType->setText(QString::fromLocal8Bit("Type:"));

		PhantomType_QLineEdit = new QLineEdit;
		PhantomType_QLineEdit->setFixedSize(Window_width * 0.084, Window_height * 0.028);
		PhantomType_QLineEdit->setAlignment(Qt::AlignCenter);
		PhantomType_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		PhantomType_QLineEdit->setFont(font_D_LE4);
		PhantomType_QLineEdit->setText("");
		PhantomType_QLineEdit->setReadOnly(true);		

		QLabel* labelSpacer = new QLabel;
		labelSpacer->setFixedSize(Window_width * 0.01, Window_height * 0.028);
		labelSpacer->setStyleSheet(DialogStyle::DATA_LABEL);
		labelSpacer->setFont(font_D_LBL5);
		labelSpacer->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		labelSpacer->setText(QString::fromLocal8Bit(""));

		PhantomDummy_QCheckBox = new QCheckBox;
		PhantomDummy_QCheckBox->setEnabled(true);
		PhantomDummy_QCheckBox->setChecked(false);
		PhantomDummy_QCheckBox->setFixedSize(Window_width * 0.006, Window_height * 0.02);
		PhantomDummy_QCheckBox->setStyleSheet("QCheckBox { background-color: transparent; }");

		QLabel* labelDummy = new QLabel;
		labelDummy->setFixedSize(Window_width * 0.03, Window_height * 0.028);
		labelDummy->setStyleSheet(DialogStyle::DATA_LABEL);
		labelDummy->setFont(font_D_LBL4);
		labelDummy->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		labelDummy->setText(QString::fromLocal8Bit("Dummy\nPhantom"));
		connect(PhantomDummy_QCheckBox, SIGNAL(clicked()), this, SLOT(slot_DummyPhantomCheckBoxClicked())); connect(PhantomDummy_QCheckBox, SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));

		hLayout->addWidget(labelType);
		hLayout->addWidget(PhantomType_QLineEdit);
		hLayout->addWidget(labelSpacer);
		hLayout->addWidget(PhantomDummy_QCheckBox);
		hLayout->addWidget(labelDummy);	

		layoutIn->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(0, Window_height * 0.005, 0, 0);

		QLabel* labelGender = new QLabel;
		labelGender->setFixedSize(Window_width * 0.028, Window_height * 0.028);
		labelGender->setStyleSheet(DialogStyle::DATA_LABEL);
		labelGender->setFont(font_D_LBL5);
		labelGender->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		labelGender->setText(QString::fromLocal8Bit("Gender:"));

		PhantomGender_QLineEdit = new QLineEdit;
		PhantomGender_QLineEdit->setFixedSize(Window_width * 0.048, Window_height * 0.028);
		PhantomGender_QLineEdit->setAlignment(Qt::AlignCenter);
		PhantomGender_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		PhantomGender_QLineEdit->setFont(font_D_LE4);
		PhantomGender_QLineEdit->setText("");
		PhantomGender_QLineEdit->setReadOnly(true);

		QLabel* labelSpacer = new QLabel;
		labelSpacer->setFixedSize(Window_width * 0.01, Window_height * 0.028);
		labelSpacer->setStyleSheet(DialogStyle::DATA_LABEL);
		labelSpacer->setFont(font_D_LBL5);
		labelSpacer->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		labelSpacer->setText(QString::fromLocal8Bit(""));

		QLabel* labelAge = new QLabel;
		labelAge->setFixedSize(Window_width * 0.02, Window_height * 0.028);
		labelAge->setStyleSheet(DialogStyle::DATA_LABEL);
		labelAge->setFont(font_D_LBL5);
		labelAge->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		labelAge->setText(QString::fromLocal8Bit("Age:"));

		PhantomAge_QLineEdit = new QLineEdit;
		PhantomAge_QLineEdit->setFixedSize(Window_width * 0.048, Window_height * 0.028);
		PhantomAge_QLineEdit->setAlignment(Qt::AlignCenter);
		PhantomAge_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		PhantomAge_QLineEdit->setFont(font_D_LE4);
		PhantomAge_QLineEdit->setText("");
		PhantomAge_QLineEdit->setReadOnly(true);

		hLayout->addWidget(labelGender);
		hLayout->addWidget(PhantomGender_QLineEdit);
		hLayout->addWidget(labelSpacer);
		hLayout->addWidget(labelAge);
		hLayout->addWidget(PhantomAge_QLineEdit);

		layoutIn->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(0, Window_height * 0.005, 0, 0);

		QLabel* labelPosture = new QLabel;
		labelPosture->setFixedSize(Window_width * 0.028, Window_height * 0.028);
		labelPosture->setStyleSheet(DialogStyle::DATA_LABEL);
		labelPosture->setFont(font_D_LBL5);
		labelPosture->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		labelPosture->setText(QString::fromLocal8Bit("Posture:"));
		
		PhantomPosture_QLineEdit = new QLineEdit;
		PhantomPosture_QLineEdit->setFixedSize(Window_width * 0.048, Window_height * 0.028);
		PhantomPosture_QLineEdit->setAlignment(Qt::AlignCenter);
		PhantomPosture_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		PhantomPosture_QLineEdit->setFont(font_D_LE4);
		PhantomPosture_QLineEdit->setText("");
		PhantomPosture_QLineEdit->setReadOnly(true);
				
		QLabel* labelSpacer = new QLabel;
		labelSpacer->setFixedSize(Window_width * 0.0001, Window_height * 0.028);
		labelSpacer->setStyleSheet(DialogStyle::DATA_LABEL);
		labelSpacer->setFont(font_D_LBL5);
		labelSpacer->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		labelSpacer->setText(QString::fromLocal8Bit(""));

		QLabel* labelFetalAge = new QLabel;
		labelFetalAge->setFixedSize(Window_width * 0.03, Window_height * 0.028);
		labelFetalAge->setStyleSheet(DialogStyle::DATA_LABEL);
		labelFetalAge->setFont(font_D_LBL5);
		labelFetalAge->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		labelFetalAge->setText(QString::fromLocal8Bit("Fetal Age:"));

		PhantomFetalAge_QLineEdit = new QLineEdit;
		PhantomFetalAge_QLineEdit->setFixedSize(Window_width * 0.048, Window_height * 0.028);
		PhantomFetalAge_QLineEdit->setAlignment(Qt::AlignCenter);
		PhantomFetalAge_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		PhantomFetalAge_QLineEdit->setFont(font_D_LE4);
		PhantomFetalAge_QLineEdit->setText("");
		PhantomFetalAge_QLineEdit->setReadOnly(true);	

		hLayout->addWidget(labelPosture);
		hLayout->addWidget(PhantomPosture_QLineEdit);
		hLayout->addWidget(labelSpacer);
		hLayout->addWidget(labelFetalAge);
		hLayout->addWidget(PhantomFetalAge_QLineEdit);
	
		layoutIn->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(0, Window_height * 0.005, 0, 0);

		QLabel* labelHeight = new QLabel;
		labelHeight->setFixedSize(Window_width * 0.028, Window_height * 0.028);
		labelHeight->setStyleSheet(DialogStyle::DATA_LABEL);
		labelHeight->setFont(font_D_LBL5);
		labelHeight->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		labelHeight->setText(QString::fromLocal8Bit("Height:"));

		PhantomHeight_QLineEdit = new QLineEdit;
		PhantomHeight_QLineEdit->setFixedSize(Window_width * 0.048, Window_height * 0.028);
		PhantomHeight_QLineEdit->setAlignment(Qt::AlignCenter);
		PhantomHeight_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		PhantomHeight_QLineEdit->setFont(font_D_LE4);
		PhantomHeight_QLineEdit->setText("");
		PhantomHeight_QLineEdit->setReadOnly(true);

		QLabel* labelSpacer = new QLabel;
		labelSpacer->setFixedSize(Window_width * 0.002, Window_height * 0.028);
		labelSpacer->setStyleSheet(DialogStyle::DATA_LABEL);
		labelSpacer->setFont(font_D_LBL5);
		labelSpacer->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		labelSpacer->setText(QString::fromLocal8Bit(""));

		QLabel* labelWeight = new QLabel;
		labelWeight->setFixedSize(Window_width * 0.028, Window_height * 0.028);
		labelWeight->setStyleSheet(DialogStyle::DATA_LABEL);
		labelWeight->setFont(font_D_LBL5);
		labelWeight->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		labelWeight->setText(QString::fromLocal8Bit("Weight:"));
		
		PhantomWeight_QLineEdit = new QLineEdit;				
		PhantomWeight_QLineEdit->setFixedSize(Window_width * 0.048, Window_height * 0.028);
		PhantomWeight_QLineEdit->setAlignment(Qt::AlignCenter);
		PhantomWeight_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		PhantomWeight_QLineEdit->setFont(font_D_LE4);
		PhantomWeight_QLineEdit->setText("");
		PhantomWeight_QLineEdit->setReadOnly(true);

		hLayout->addWidget(labelHeight);
		hLayout->addWidget(PhantomHeight_QLineEdit);
		hLayout->addWidget(labelSpacer);
		hLayout->addWidget(labelWeight);
		hLayout->addWidget(PhantomWeight_QLineEdit);

		layoutIn->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(0, Window_height * 0.005, 0, 0);

		QLabel* labelPhantomPosRot = new QLabel;
		labelPhantomPosRot->setFixedSize(Window_width * 0.104, Window_height * 0.028);
		labelPhantomPosRot->setStyleSheet(DialogStyle::DATA_LABEL);
		labelPhantomPosRot->setFont(font_D_LBL1);
		labelPhantomPosRot->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		labelPhantomPosRot->setText(QString::fromLocal8Bit("Phantom Pos/Rot"));

		hLayout->addWidget(labelPhantomPosRot);

		layoutIn->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.002);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(Window_width * 0.015, Window_height * 0.005, 0, 0);

		QLabel* labelPhantomPosX = new QLabel;
		labelPhantomPosX->setFixedSize(Window_width * 0.0478, Window_height * 0.028);
		labelPhantomPosX->setStyleSheet(DialogStyle::DATA_LABEL);
		labelPhantomPosX->setFont(font_D_LBL1);
		labelPhantomPosX->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		labelPhantomPosX->setText(QString::fromLocal8Bit("Pos. XYZ:"));

		PhantomPosX_QLineEdit = new QLineEdit;
		PhantomPosX_QLineEdit->setFixedSize(Window_width * 0.035, Window_height * 0.028);
		PhantomPosX_QLineEdit->setAlignment(Qt::AlignCenter);
		PhantomPosX_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME4);
		PhantomPosX_QLineEdit->setFont(font_D_LE4);
		PhantomPosX_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(cm)"));

		PhantomPosY_QLineEdit = new QLineEdit;
		PhantomPosY_QLineEdit->setFixedSize(Window_width * 0.035, Window_height * 0.028);
		PhantomPosY_QLineEdit->setAlignment(Qt::AlignCenter);
		PhantomPosY_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME4);
		PhantomPosY_QLineEdit->setFont(font_D_LE4);
		PhantomPosY_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(cm)"));

		PhantomPosZ_QLineEdit = new QLineEdit;
		PhantomPosZ_QLineEdit->setFixedSize(Window_width * 0.035, Window_height * 0.028);
		PhantomPosZ_QLineEdit->setAlignment(Qt::AlignCenter);
		PhantomPosZ_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME4);
		PhantomPosZ_QLineEdit->setFont(font_D_LE4);
		PhantomPosZ_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(cm)"));

		hLayout->addWidget(labelPhantomPosX);
		hLayout->addWidget(PhantomPosX_QLineEdit);
		hLayout->addWidget(PhantomPosY_QLineEdit);
		hLayout->addWidget(PhantomPosZ_QLineEdit);

		layoutIn->addLayout(hLayout);
	}
	
	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.002);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(Window_width * 0.015, Window_height * 0.005, 0, 0);

		QLabel* labelPhantomRotateX = new QLabel;
		labelPhantomRotateX->setFixedSize(Window_width * 0.0478, Window_height * 0.028);
		labelPhantomRotateX->setStyleSheet(DialogStyle::DATA_LABEL);
		labelPhantomRotateX->setFont(font_D_LBL1);
		labelPhantomRotateX->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		labelPhantomRotateX->setText(QString::fromLocal8Bit("Rot. XYZ:"));

		PhantomRotX_QLineEdit = new QLineEdit;
		PhantomRotX_QLineEdit->setFixedSize(Window_width * 0.035, Window_height * 0.028);
		PhantomRotX_QLineEdit->setAlignment(Qt::AlignCenter);
		PhantomRotX_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME4);
		PhantomRotX_QLineEdit->setFont(font_D_LE4);
		PhantomRotX_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(deg)"));

		PhantomRotY_QLineEdit = new QLineEdit;
		PhantomRotY_QLineEdit->setFixedSize(Window_width * 0.035, Window_height * 0.028);
		PhantomRotY_QLineEdit->setAlignment(Qt::AlignCenter);
		PhantomRotY_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME4);
		PhantomRotY_QLineEdit->setFont(font_D_LE4);
		PhantomRotY_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(deg)"));

		PhantomRotZ_QLineEdit = new QLineEdit;
		PhantomRotZ_QLineEdit->setFixedSize(Window_width * 0.035, Window_height * 0.028);
		PhantomRotZ_QLineEdit->setAlignment(Qt::AlignCenter);
		PhantomRotZ_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME4);
		PhantomRotZ_QLineEdit->setFont(font_D_LE4);
		PhantomRotZ_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(deg)"));

		hLayout->addWidget(labelPhantomRotateX);
		hLayout->addWidget(PhantomRotX_QLineEdit);
		hLayout->addWidget(PhantomRotY_QLineEdit);
		hLayout->addWidget(PhantomRotZ_QLineEdit);
		
		layoutIn->addLayout(hLayout);
	}
	   

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		hLayout->setContentsMargins(0, Window_height * 0.009, Window_width * 0.007, Window_height * 0.009);

		PhantomUpdate_QPushButton = new QPushButton(QString::fromLocal8Bit("Update"));		
		Buttons_FunctionPanelRight.append(PhantomUpdate_QPushButton);
		PhantomUpdate_QPushButton->setFixedSize(Window_width * 0.042, Window_height * 0.037);
		PhantomUpdate_QPushButton->setCheckable(false);
		PhantomUpdate_QPushButton->setStyleSheet(DialogStyle::MENU_BTN2);
		PhantomUpdate_QPushButton->setFont(font_D_BTN2);
		PhantomUpdate_QPushButton->setChecked(false);

		connect(PhantomUpdate_QPushButton, SIGNAL(clicked()), this, SLOT(slot_PhantomUpdate_ButtonClicked())); connect(PhantomUpdate_QPushButton, SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));

		hLayout->addWidget(PhantomUpdate_QPushButton);

		layoutIn->addLayout(hLayout);
	}

	////////////// Clothing panel ////////////////
	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(0, Window_height * 0.005, 0, 0);

		ClothingTitle_QLabel = new QLabel;
		ClothingTitle_QLabel->setFixedSize(Window_width * 0.104, Window_height * 0.028);
		ClothingTitle_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		ClothingTitle_QLabel->setFont(font_D_LBL1);
		ClothingTitle_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		ClothingTitle_QLabel->setText(QString::fromLocal8Bit("Clothing"));

		hLayout->addWidget(ClothingTitle_QLabel);

		layoutIn->addLayout(hLayout);
	}
	{
		QGroupBox* GroupBoxClothingList = new QGroupBox;
		GroupBoxClothingList->setAlignment(Qt::AlignCenter | Qt::AlignLeft);
		GroupBoxClothingList->setStyleSheet(DialogStyle::GROUPBOX_WIDGET10);
		GroupBoxClothingList->setFont(font_D_GBW10);
		GroupBoxClothingList->setFixedHeight(Window_height * 0.060);
		GroupBoxClothingList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed); // 가로, 세로 방향 크기
		GroupBoxClothingList->setContentsMargins(Window_width * 0.001, 0, Window_width * 0.001, 0);

		layoutIn->addWidget(GroupBoxClothingList);

		QVBoxLayout* layoutOut = new QVBoxLayout();
		layoutOut->setContentsMargins(Window_width * 0.001, 0, Window_width * 0.005, Window_height * 0.005);
		layoutOut->setSpacing(Window_width * 0.005);
		layoutOut->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QGroupBox* GroupBoxIn = new QGroupBox;
		GroupBoxIn->setAlignment(Qt::AlignCenter | Qt::AlignLeft);
		GroupBoxIn->setStyleSheet(DialogStyle::GROUPBOX_WIDGET12);
		GroupBoxIn->setFont(font_D_GBW10);
		GroupBoxIn->setFixedHeight(0.060 * Window_height);
		GroupBoxIn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

		ClothingList_QHBoxLayout = new QHBoxLayout();
		ClothingList_QHBoxLayout->setContentsMargins(Window_width * 0.001, Window_height * 0.005, Window_width * 0.001, Window_height * 0.005); // 왼쪽, 위, 오른쪽, 아래의 여유공간(해당 layout 안에서의)
		ClothingList_QHBoxLayout->setSpacing(Window_width * 0.003);
		ClothingList_QHBoxLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		MultipleButtonScorllArea* scrollArea = new MultipleButtonScorllArea();
		scrollArea->setStyleSheet(DialogStyle::GROUPBOX_WIDGET10);
		scrollArea->setFont(font_D_GBW10);
		scrollArea->setFixedHeight(Window_height * 0.060);
		scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // Do not show vertical scrollbar
		ClothingList_QScrollBar = scrollArea->horizontalScrollBar();

		GroupBoxIn->setLayout(ClothingList_QHBoxLayout);
		scrollArea->setWidget(GroupBoxIn);
		scrollArea->setWidgetResizable(true);
		layoutOut->addWidget(scrollArea);
		GroupBoxClothingList->setLayout(layoutOut);
		GroupBoxClothingList->hide();
		GroupBoxClothingList->show();
	}
	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		hLayout->setContentsMargins(0, Window_height * 0.005, Window_width * 0.010, 0);

		hLayout->addStretch();

		ClothingAdd_QPushButton = new QPushButton(QString::fromLocal8Bit("Add"));
		Buttons_FunctionPanelRight.append(ClothingAdd_QPushButton);
		ClothingAdd_QPushButton->setFixedSize(Window_width * 0.030, Window_height * 0.025);
		ClothingAdd_QPushButton->setStyleSheet(DialogStyle::MENU_BTN2);
		ClothingAdd_QPushButton->setFont(font_D_BTN2);
		ClothingAdd_QPushButton->setCheckable(false);

		ClothingDelete_QPushButton = new QPushButton(QString::fromLocal8Bit("Delete"));
		Buttons_FunctionPanelRight.append(ClothingDelete_QPushButton);
		ClothingDelete_QPushButton->setFixedSize(Window_width * 0.030, Window_height * 0.025);
		ClothingDelete_QPushButton->setStyleSheet(DialogStyle::MENU_BTN2);
		ClothingDelete_QPushButton->setFont(font_D_BTN2);
		ClothingDelete_QPushButton->setCheckable(false);

		connect(ClothingAdd_QPushButton, SIGNAL(clicked()), this, SLOT(slot_ClothingAdd_ButtonClicked())); connect(ClothingAdd_QPushButton, SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
		connect(ClothingDelete_QPushButton, SIGNAL(clicked()), this, SLOT(slot_ClothingDelete_ButtonClicked())); connect(ClothingDelete_QPushButton, SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));

		hLayout->addWidget(ClothingAdd_QPushButton);
		hLayout->addWidget(ClothingDelete_QPushButton);

		layoutIn->addLayout(hLayout);
	}
	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(Window_width * 0.025, Window_height * 0.005, 0, 0);

		ClothingLayerTitle_QLabel = new QLabel;
		ClothingLayerTitle_QLabel->setFixedSize(Window_width * 0.104, Window_height * 0.028);
		ClothingLayerTitle_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		ClothingLayerTitle_QLabel->setFont(font_D_LBL1);
		ClothingLayerTitle_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		ClothingLayerTitle_QLabel->setText(QString::fromLocal8Bit("Clothing Layer"));

		hLayout->addWidget(ClothingLayerTitle_QLabel);

		layoutIn->addLayout(hLayout);
	}
	{
		QGroupBox* GroupBoxClothingLayerList = new QGroupBox;
		GroupBoxClothingLayerList->setAlignment(Qt::AlignCenter | Qt::AlignLeft);
		GroupBoxClothingLayerList->setStyleSheet(DialogStyle::GROUPBOX_WIDGET10);
		GroupBoxClothingLayerList->setFont(font_D_GBW10);
		GroupBoxClothingLayerList->setFixedHeight(0.060 * Window_height);
		GroupBoxClothingLayerList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed); // 가로, 세로 방향 크기
		GroupBoxClothingLayerList->setContentsMargins(Window_width * 0.001, 0, Window_width * 0.001, 0);

		layoutIn->addWidget(GroupBoxClothingLayerList);

		QVBoxLayout* layoutOut = new QVBoxLayout();
		layoutOut->setContentsMargins(Window_width * 0.02, 0, Window_width * 0.005, Window_height * 0.005); // List groupbox의 좌상우하 여백
		layoutOut->setSpacing(Window_width * 0.005);
		layoutOut->setAlignment(Qt::AlignRight | Qt::AlignTop);

		QGroupBox* GroupBoxIn = new QGroupBox;
		GroupBoxIn->setAlignment(Qt::AlignCenter | Qt::AlignLeft);
		GroupBoxIn->setStyleSheet(DialogStyle::GROUPBOX_WIDGET12);
		GroupBoxIn->setFont(font_D_GBW10);
		GroupBoxIn->setFixedHeight(0.060 * Window_height);
		GroupBoxIn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

		ClothingLayerList_QHBoxLayout = new QHBoxLayout();
		ClothingLayerList_QHBoxLayout->setContentsMargins(Window_width * 0.001, Window_height * 0.005, Window_width * 0.001, Window_height * 0.005); // 왼쪽, 위, 오른쪽, 아래의 여유공간(해당 layout 안에서의)
		ClothingLayerList_QHBoxLayout->setSpacing(Window_width * 0.003);
		ClothingLayerList_QHBoxLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		MultipleButtonScorllArea* scrollArea = new MultipleButtonScorllArea();
		scrollArea->setStyleSheet(DialogStyle::GROUPBOX_WIDGET10);
		scrollArea->setFont(font_D_GBW10);
		scrollArea->setFixedHeight(Window_height * 0.060);
		scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // Do not show vertical scrollbar
		ClothingLayerList_QScrollBar = scrollArea->horizontalScrollBar();

		GroupBoxIn->setLayout(ClothingLayerList_QHBoxLayout);
		scrollArea->setWidget(GroupBoxIn);
		scrollArea->setWidgetResizable(true);
		layoutOut->addWidget(scrollArea);
		GroupBoxClothingLayerList->setLayout(layoutOut);
		GroupBoxClothingLayerList->hide();
		GroupBoxClothingLayerList->show();
	}
	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		hLayout->setContentsMargins(0, Window_height * 0.005, Window_width * 0.010, Window_height * 0.005);

		hLayout->addStretch();

		ClothingLayerAdd_QPushButton = new QPushButton(QString::fromLocal8Bit("Add"));
		Buttons_FunctionPanelRight.append(ClothingLayerAdd_QPushButton);
		ClothingLayerAdd_QPushButton->setFixedSize(Window_width * 0.030, Window_height * 0.025);
		ClothingLayerAdd_QPushButton->setStyleSheet(DialogStyle::MENU_BTN2);
		ClothingLayerAdd_QPushButton->setFont(font_D_BTN2);
		ClothingLayerAdd_QPushButton->setCheckable(false);

		ClothingLayerDelete_QPushButton = new QPushButton(QString::fromLocal8Bit("Delete"));
		Buttons_FunctionPanelRight.append(ClothingLayerDelete_QPushButton);
		ClothingLayerDelete_QPushButton->setFixedSize(Window_width * 0.030, Window_height * 0.025);
		ClothingLayerDelete_QPushButton->setStyleSheet(DialogStyle::MENU_BTN2);
		ClothingLayerDelete_QPushButton->setFont(font_D_BTN2);
		ClothingLayerDelete_QPushButton->setCheckable(false);

		connect(ClothingLayerAdd_QPushButton, SIGNAL(clicked()), this, SLOT(slot_ClothingLayerAdd_ButtonClicked())); connect(ClothingLayerAdd_QPushButton, SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
		connect(ClothingLayerDelete_QPushButton, SIGNAL(clicked()), this, SLOT(slot_ClothingLayerDelete_ButtonClicked())); connect(ClothingLayerDelete_QPushButton, SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));

		hLayout->addWidget(ClothingLayerAdd_QPushButton);
		hLayout->addWidget(ClothingLayerDelete_QPushButton);

		layoutIn->addLayout(hLayout);
	}
	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(Window_width * 0.018, Window_height * 0.005, 0, Window_height * 0.005);

		QLabel* labelThickness = new QLabel;
		labelThickness->setFixedSize(Window_width * 0.05, Window_height * 0.028);
		labelThickness->setStyleSheet(DialogStyle::DATA_LABEL);
		labelThickness->setFont(font_D_LBL1);
		labelThickness->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		labelThickness->setText(QString::fromLocal8Bit("Thickness:"));

		ClothingThickness_QLineEdit = new QLineEdit;
		ClothingThickness_QLineEdit->setFixedSize(Window_width * 0.094, Window_height * 0.028);
		ClothingThickness_QLineEdit->setAlignment(Qt::AlignCenter);
		ClothingThickness_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		ClothingThickness_QLineEdit->setFont(font_D_LE1);
		ClothingThickness_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("cm"));

		hLayout->addWidget(labelThickness);
		hLayout->addWidget(ClothingThickness_QLineEdit);

		layoutIn->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(Window_width * 0.018, Window_height * 0.005, 0, Window_height * 0.005);

		QLabel* labelDensity = new QLabel;
		labelDensity->setFixedSize(Window_width * 0.05, Window_height * 0.028);
		labelDensity->setStyleSheet(DialogStyle::DATA_LABEL);
		labelDensity->setFont(font_D_LBL1);
		labelDensity->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		labelDensity->setText(QString::fromLocal8Bit("Density:"));

		ClothingDensity_QLineEdit = new QLineEdit;
		ClothingDensity_QLineEdit->setFixedSize(Window_width * 0.094, Window_height * 0.028);
		ClothingDensity_QLineEdit->setAlignment(Qt::AlignCenter);
		ClothingDensity_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		ClothingDensity_QLineEdit->setFont(font_D_LE1);
		ClothingDensity_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("g/cm3"));

		hLayout->addWidget(labelDensity);
		hLayout->addWidget(ClothingDensity_QLineEdit);

		layoutIn->addLayout(hLayout);
	}
	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(Window_width * 0.018, Window_height * 0.005, 0, 0);

		QLabel* labelLayer = new QLabel;
		labelLayer->setFixedSize(Window_width * 0.05, Window_height * 0.028);
		labelLayer->setStyleSheet(DialogStyle::DATA_LABEL);
		labelLayer->setFont(font_D_LBL1);
		labelLayer->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		labelLayer->setText(QString::fromLocal8Bit("Composition:"));

		ClothingComposition_QComboBox = new QComboBox;
		ClothingComposition_QComboBox->setFixedSize(Window_width * 0.095, Window_height * 0.028);
		ClothingComposition_QComboBox->setStyleSheet(DialogStyle::COMBOBOX);
		ClothingComposition_QComboBox->setFont(font_D_CB1);

		ClothingComposition_QComboBox->setEditable(true);
		ClothingComposition_QComboBox->lineEdit()->setReadOnly(true);
		ClothingComposition_QComboBox->lineEdit()->setAlignment(Qt::AlignCenter);
		ClothingComposition_QComboBox->lineEdit()->setFont(font_D_CB1);

		ClothingComposition_QComboBox->addItem("Cloth");
		ClothingComposition_QComboBox->addItem("Lead");
		ClothingComposition_QComboBox->addItem("air");
		ClothingComposition_QComboBox->setCurrentIndex(0);

		hLayout->addWidget(labelLayer);
		hLayout->addWidget(ClothingComposition_QComboBox);

		layoutIn->addLayout(hLayout);
	}
	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(Window_width * 0.123, Window_height * 0.009, 0, Window_height * 0.027);

		// 버튼생성
		ClothingUpdate_QPushButton = new QPushButton(QString::fromLocal8Bit("Update"));
		Buttons_FunctionPanelRight.append(ClothingUpdate_QPushButton);
		ClothingUpdate_QPushButton->setFixedSize(Window_width * 0.042, Window_height * 0.037);
		ClothingUpdate_QPushButton->setCheckable(false);
		ClothingUpdate_QPushButton->setStyleSheet(DialogStyle::MENU_BTN2);
		ClothingUpdate_QPushButton->setFont(font_D_BTN2);
		ClothingUpdate_QPushButton->setChecked(false);

		connect(ClothingUpdate_QPushButton, SIGNAL(clicked()), this, SLOT(slot_ClothingLayerUpdate_ButtonClicked())); connect(ClothingUpdate_QPushButton, SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
		
		hLayout->addWidget(ClothingUpdate_QPushButton);

		layoutIn->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(0, Window_height * 0.005, 0, 0);

		QLabel* labelGlasses = new QLabel;
		labelGlasses->setFixedSize(Window_width * 0.052, Window_height * 0.028);
		labelGlasses->setStyleSheet(DialogStyle::DATA_LABEL);
		labelGlasses->setFont(font_D_LBL1);
		labelGlasses->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		labelGlasses->setText(QString::fromLocal8Bit("Eyewear"));

		Eyewear_AddDelete_QPushButton = new QPushButton(QString::fromLocal8Bit("Add"));
		Buttons_FunctionPanelRight.append(Eyewear_AddDelete_QPushButton);
		Eyewear_AddDelete_QPushButton->setFixedSize(Window_width * 0.030, Window_height * 0.025);
		Eyewear_AddDelete_QPushButton->setStyleSheet(DialogStyle::MENU_BTN2);
		Eyewear_AddDelete_QPushButton->setFont(font_D_BTN2);
		Eyewear_AddDelete_QPushButton->setCheckable(false);

		connect(Eyewear_AddDelete_QPushButton, SIGNAL(clicked()), this, SLOT(slot_Add_Delete_Eyewear_ButtonClicked())); connect(Eyewear_AddDelete_QPushButton, SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));

		hLayout->addWidget(labelGlasses);
		QSpacerItem *spacer1 = new QSpacerItem(Window_width * 0.081, Window_height * 0.028); // Adjust spacing
		hLayout->addSpacerItem(spacer1);
		hLayout->addWidget(Eyewear_AddDelete_QPushButton);

		layoutIn->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(Window_width * 0.015, Window_height * 0.005, 0, 0);

		QLabel* labelLayer = new QLabel;
		labelLayer->setFixedSize(Window_width * 0.052, Window_height * 0.028);
		labelLayer->setStyleSheet(DialogStyle::DATA_LABEL);
		labelLayer->setFont(font_D_LBL1);
		labelLayer->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		labelLayer->setText(QString::fromLocal8Bit("Type:"));

		EyewearType_QLineEdit = new QLineEdit;
		EyewearType_QLineEdit->setFixedSize(Window_width * 0.094, Window_height * 0.028);
		EyewearType_QLineEdit->setAlignment(Qt::AlignCenter);
		EyewearType_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		EyewearType_QLineEdit->setFont(font_D_LE1);
		EyewearType_QLineEdit->setText("Glasses");

		hLayout->addWidget(labelLayer);
		hLayout->addWidget(EyewearType_QLineEdit);

		layoutIn->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(Window_width * 0.015, Window_height * 0.005, 0, 0);

		QLabel* labelGlassesDistance = new QLabel;
		labelGlassesDistance->setFixedSize(Window_width * 0.052, Window_height * 0.028);
		labelGlassesDistance->setStyleSheet(DialogStyle::DATA_LABEL);
		labelGlassesDistance->setFont(font_D_LBL1);
		labelGlassesDistance->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		labelGlassesDistance->setText(QString::fromLocal8Bit("Distance:"));

		EyewearDistance_QLineEdit = new QLineEdit;
		EyewearDistance_QLineEdit->setFixedSize(Window_width * 0.094, Window_height * 0.028);
		EyewearDistance_QLineEdit->setAlignment(Qt::AlignCenter);
		EyewearDistance_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		EyewearDistance_QLineEdit->setFont(font_D_LE1);
		EyewearDistance_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("cm"));

		hLayout->addWidget(labelGlassesDistance);
		hLayout->addWidget(EyewearDistance_QLineEdit);

		layoutIn->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(Window_width * 0.015, Window_height * 0.005, 0, 0);

		QLabel* labelGlassesRadius = new QLabel;
		labelGlassesRadius->setFixedSize(Window_width * 0.052, Window_height * 0.028);
		labelGlassesRadius->setStyleSheet(DialogStyle::DATA_LABEL);
		labelGlassesRadius->setFont(font_D_LBL1);
		labelGlassesRadius->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		labelGlassesRadius->setText(QString::fromLocal8Bit("Radius:"));

		EyewearRadius_QLineEdit = new QLineEdit;
		EyewearRadius_QLineEdit->setFixedSize(Window_width * 0.094, Window_height * 0.028);
		EyewearRadius_QLineEdit->setAlignment(Qt::AlignCenter);
		EyewearRadius_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		EyewearRadius_QLineEdit->setFont(font_D_LE1);
		EyewearRadius_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("cm"));

		hLayout->addWidget(labelGlassesRadius);
		hLayout->addWidget(EyewearRadius_QLineEdit);

		layoutIn->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(Window_width * 0.015, Window_height * 0.005, 0, 0);

		EyewearThickness_QLabel = new QLabel;
		EyewearThickness_QLabel->setFixedSize(Window_width * 0.052, Window_height * 0.028);
		EyewearThickness_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		EyewearThickness_QLabel->setFont(font_D_LBL1);
		EyewearThickness_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		EyewearThickness_QLabel->setText(QString::fromLocal8Bit("Thickness:"));

		EyewearThickness_QLineEdit = new QLineEdit;
		EyewearThickness_QLineEdit->setFixedSize(Window_width * 0.094, Window_height * 0.028);
		EyewearThickness_QLineEdit->setAlignment(Qt::AlignCenter);
		EyewearThickness_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		EyewearThickness_QLineEdit->setFont(font_D_LE1);
		EyewearThickness_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("cm"));

		hLayout->addWidget(EyewearThickness_QLabel);
		hLayout->addWidget(EyewearThickness_QLineEdit);

		layoutIn->addLayout(hLayout);
	}


	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(Window_width * 0.015, Window_height * 0.005, 0, 0);

		EyewearThickness_Front_QLabel = new QLabel;
		EyewearThickness_Front_QLabel->setFixedSize(Window_width * 0.052, Window_height * 0.028); // hard coding 된 거 
		EyewearThickness_Front_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		EyewearThickness_Front_QLabel->setFont(font_D_LBL1);
		EyewearThickness_Front_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		EyewearThickness_Front_QLabel->setText(QString::fromLocal8Bit("Thick. Front:"));

		EyewearThickness_Front_QLineEdit = new QLineEdit;
		EyewearThickness_Front_QLineEdit->setFixedSize(Window_width * 0.094, Window_height * 0.028);
		EyewearThickness_Front_QLineEdit->setAlignment(Qt::AlignCenter);
		EyewearThickness_Front_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		EyewearThickness_Front_QLineEdit->setFont(font_D_LE1);
		EyewearThickness_Front_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("cm"));

		hLayout->addWidget(EyewearThickness_Front_QLabel);
		hLayout->addWidget(EyewearThickness_Front_QLineEdit);

		EyewearThickness_Front_QLabel->hide();
		EyewearThickness_Front_QLineEdit->hide();

		layoutIn->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(Window_width * 0.015, Window_height * 0.005, 0, 0);

		EyewearThickness_Side_QLabel = new QLabel;
		EyewearThickness_Side_QLabel->setFixedSize(Window_width * 0.052, Window_height * 0.028);
		EyewearThickness_Side_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		EyewearThickness_Side_QLabel->setFont(font_D_LBL1);
		EyewearThickness_Side_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		EyewearThickness_Side_QLabel->setText(QString::fromLocal8Bit("Thick. Side:"));

		EyewearThickness_Side_QLineEdit = new QLineEdit;
		EyewearThickness_Side_QLineEdit->setFixedSize(Window_width * 0.094, Window_height * 0.028);
		EyewearThickness_Side_QLineEdit->setAlignment(Qt::AlignCenter);
		EyewearThickness_Side_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		EyewearThickness_Side_QLineEdit->setFont(font_D_LE1);
		EyewearThickness_Side_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("cm"));

		hLayout->addWidget(EyewearThickness_Side_QLabel);
		hLayout->addWidget(EyewearThickness_Side_QLineEdit);

		EyewearThickness_Side_QLabel->hide();
		EyewearThickness_Side_QLineEdit->hide();

		layoutIn->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(Window_width * 0.015, Window_height * 0.005, 0, 0);

		QLabel* labelGlassesDensity = new QLabel;
		labelGlassesDensity->setFixedSize(Window_width * 0.052, Window_height * 0.028);
		labelGlassesDensity->setStyleSheet(DialogStyle::DATA_LABEL);
		labelGlassesDensity->setFont(font_D_LBL1);
		labelGlassesDensity->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		labelGlassesDensity->setText(QString::fromLocal8Bit("Density:"));

		EyewearDensity_QLineEdit = new QLineEdit;
		EyewearDensity_QLineEdit->setFixedSize(Window_width * 0.094, Window_height * 0.028);
		EyewearDensity_QLineEdit->setAlignment(Qt::AlignCenter);
		EyewearDensity_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		EyewearDensity_QLineEdit->setFont(font_D_LE1);
		EyewearDensity_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("g/cm3"));

		hLayout->addWidget(labelGlassesDensity);
		hLayout->addWidget(EyewearDensity_QLineEdit);

		layoutIn->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(Window_width * 0.015, Window_height * 0.005, 0, 0);

		QLabel* labelLayer = new QLabel;
		labelLayer->setFixedSize(Window_width * 0.052, Window_height * 0.028);
		labelLayer->setStyleSheet(DialogStyle::DATA_LABEL);
		labelLayer->setFont(font_D_LBL1);
		labelLayer->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		labelLayer->setText(QString::fromLocal8Bit("Composition:"));

		EyewearComposition_QComboBox = new QComboBox;
		EyewearComposition_QComboBox->setFixedSize(Window_width * 0.095, Window_height * 0.028);
		EyewearComposition_QComboBox->setStyleSheet(DialogStyle::COMBOBOX);
		EyewearComposition_QComboBox->setFont(font_D_CB1);

		EyewearComposition_QComboBox->setEditable(true);
		EyewearComposition_QComboBox->lineEdit()->setReadOnly(true);
		EyewearComposition_QComboBox->lineEdit()->setAlignment(Qt::AlignCenter);
		EyewearComposition_QComboBox->lineEdit()->setFont(font_D_CB1);
				
		EyewearComposition_QComboBox->addItem("Lead");
		EyewearComposition_QComboBox->addItem("Glass");
		EyewearComposition_QComboBox->setCurrentIndex(0);

		hLayout->addWidget(labelLayer);
		hLayout->addWidget(EyewearComposition_QComboBox);

		layoutIn->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(Window_width * 0.123, Window_height * 0.005, 0, 0);

		EyewearUpdate_QPushButton = new QPushButton(QString::fromLocal8Bit("Update"));
		Buttons_FunctionPanelRight.append(EyewearUpdate_QPushButton);
		EyewearUpdate_QPushButton->setFixedSize(Window_width * 0.042, Window_height * 0.037);
		EyewearUpdate_QPushButton->setCheckable(false);
		EyewearUpdate_QPushButton->setStyleSheet(DialogStyle::MENU_BTN2);
		EyewearUpdate_QPushButton->setFont(font_D_BTN2);
		EyewearUpdate_QPushButton->setChecked(false);
		
		connect(EyewearUpdate_QPushButton, SIGNAL(clicked()), this, SLOT(slot_GlassesUpdate_ButtonClicked())); connect(EyewearUpdate_QPushButton, SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
				
		hLayout->addWidget(EyewearUpdate_QPushButton);

		layoutIn->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setContentsMargins(0, Window_width * 0.005, 0, 0);
		hLayout->setSpacing(Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

		QLabel* labelDosimetry = new QLabel;
		labelDosimetry->setFixedSize(Window_width * 0.056, Window_height * 0.028);
		labelDosimetry->setStyleSheet(DialogStyle::DATA_LABEL);
		labelDosimetry->setFont(font_D_LBL1);
		labelDosimetry->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		labelDosimetry->setText(QString::fromLocal8Bit("Dosimeter"));

		hLayout->addWidget(labelDosimetry);

		layoutIn->addLayout(hLayout);
	}

	{
		QGroupBox* GroupBoxDosimeterList = new QGroupBox;
		GroupBoxDosimeterList->setAlignment(Qt::AlignCenter | Qt::AlignLeft);
		GroupBoxDosimeterList->setStyleSheet(DialogStyle::GROUPBOX_WIDGET10);
		GroupBoxDosimeterList->setFont(font_D_GBW10);
		GroupBoxDosimeterList->setFixedHeight(0.060 * Window_height);
		GroupBoxDosimeterList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed); // 가로, 세로 방향 크기
		GroupBoxDosimeterList->setContentsMargins(Window_width * 0.001, 0, Window_width * 0.001, 0);

		layoutIn->addWidget(GroupBoxDosimeterList);

		QVBoxLayout* layoutOut = new QVBoxLayout();
		layoutOut->setContentsMargins(Window_width * 0.001, 0, Window_width * 0.005, Window_height * 0.005);
		layoutOut->setSpacing(Window_width * 0.005);
		layoutOut->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QGroupBox* GroupBoxIn = new QGroupBox;
		GroupBoxIn->setAlignment(Qt::AlignCenter | Qt::AlignLeft);
		GroupBoxIn->setStyleSheet(DialogStyle::GROUPBOX_WIDGET12);
		GroupBoxIn->setFont(font_D_GBW10);
		GroupBoxIn->setFixedHeight(0.060 * Window_height);
		GroupBoxIn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

		DosimeterList_QHBoxLayout = new QHBoxLayout();
		DosimeterList_QHBoxLayout->setContentsMargins(Window_width * 0.001, Window_height * 0.005, Window_width * 0.001, Window_height * 0.005); // 왼쪽, 위, 오른쪽, 아래의 여유공간(해당 layout 안에서의)
		DosimeterList_QHBoxLayout->setSpacing(Window_width * 0.003);
		DosimeterList_QHBoxLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		MultipleButtonScorllArea* scrollArea = new MultipleButtonScorllArea();
		scrollArea->setStyleSheet(DialogStyle::GROUPBOX_WIDGET10);
		scrollArea->setFont(font_D_GBW10);
		scrollArea->setFixedHeight(Window_height * 0.060);
		scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // Do not show vertical scrollbar
		DosimeterList_QScrollBar = scrollArea->horizontalScrollBar();

		GroupBoxIn->setLayout(DosimeterList_QHBoxLayout);
		scrollArea->setWidget(GroupBoxIn);
		scrollArea->setWidgetResizable(true);
		layoutOut->addWidget(scrollArea);
		GroupBoxDosimeterList->setLayout(layoutOut);
		GroupBoxDosimeterList->hide();
		GroupBoxDosimeterList->show();
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		hLayout->setContentsMargins(0, Window_height * 0.005, Window_width * 0.010, Window_height * 0.005);
	
		DosimeterAdd_QPushButton = new QPushButton(QString::fromLocal8Bit("Add"));
		Buttons_FunctionPanelRight.append(DosimeterAdd_QPushButton);
		DosimeterAdd_QPushButton->setFixedSize(Window_width * 0.030, Window_height * 0.025);
		DosimeterAdd_QPushButton->setStyleSheet(DialogStyle::MENU_BTN2);
		DosimeterAdd_QPushButton->setFont(font_D_BTN2);	
		DosimeterAdd_QPushButton->setCheckable(false);

		DosimeterDelete_QPushButton = new QPushButton(QString::fromLocal8Bit("Delete"));
		Buttons_FunctionPanelRight.append(DosimeterDelete_QPushButton);
		DosimeterDelete_QPushButton->setFixedSize(Window_width * 0.030, Window_height * 0.025);
		DosimeterDelete_QPushButton->setStyleSheet(DialogStyle::MENU_BTN2);
		DosimeterDelete_QPushButton->setFont(font_D_BTN2);
		DosimeterDelete_QPushButton->setCheckable(false);

		connect(DosimeterAdd_QPushButton, SIGNAL(clicked()), this, SLOT(slot_DosimeterAdd_ButtonClicked())); connect(DosimeterAdd_QPushButton, SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
		connect(DosimeterDelete_QPushButton, SIGNAL(clicked()), this, SLOT(slot_DosimeterDelete_ButtonClicked())); connect(DosimeterDelete_QPushButton, SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));

		hLayout->addStretch();
		hLayout->addWidget(DosimeterAdd_QPushButton);
		hLayout->addWidget(DosimeterDelete_QPushButton);

		layoutIn->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setContentsMargins(0, Window_height * 0.005, Window_width * 0.010, 0);
		hLayout->setSpacing(Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QLabel* label0 = new QLabel;
		label0->setFixedSize(Window_width * 0.038, Window_height * 0.028);
		label0->setStyleSheet(DialogStyle::DATA_LABEL);
		label0->setFont(font_D_LBL1);
		label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		label0->setText(QString::fromLocal8Bit("Pos. XYZ "));

		DosimeterPosX_QLineEdit = new QLineEdit;
		DosimeterPosX_QLineEdit->setFixedSize(Window_width * 0.038, Window_height * 0.028);
		DosimeterPosX_QLineEdit->setAlignment(Qt::AlignCenter);
		DosimeterPosX_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME2);
		DosimeterPosX_QLineEdit->setFont(font_D_LE2);
		DosimeterPosX_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(cm)"));

		DosimeterPosY_QLineEdit = new QLineEdit;
		DosimeterPosY_QLineEdit->setFixedSize(Window_width * 0.038, Window_height * 0.028);
		DosimeterPosY_QLineEdit->setAlignment(Qt::AlignCenter);
		DosimeterPosY_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME2);
		DosimeterPosY_QLineEdit->setFont(font_D_LE2);
		DosimeterPosY_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(cm)"));

		DosimeterPosZ_QLineEdit = new QLineEdit;
		DosimeterPosZ_QLineEdit->setFixedSize(Window_width * 0.038, Window_height * 0.028);
		DosimeterPosZ_QLineEdit->setAlignment(Qt::AlignCenter);
		DosimeterPosZ_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME2);
		DosimeterPosZ_QLineEdit->setFont(font_D_LE2);
		DosimeterPosZ_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(cm)"));

		hLayout->addStretch();
		hLayout->addWidget(label0);
		hLayout->addWidget(DosimeterPosX_QLineEdit);
		hLayout->addWidget(DosimeterPosY_QLineEdit);
		hLayout->addWidget(DosimeterPosZ_QLineEdit);

		layoutIn->addLayout(hLayout);
	}	

	{// Dosimeter radius
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setContentsMargins(0, Window_height * 0.005, Window_width * 0.010, 0);
		hLayout->setSpacing(Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QLabel* label0 = new QLabel;
		label0->setFixedSize(Window_width * 0.038, Window_height * 0.028);
		label0->setStyleSheet(DialogStyle::DATA_LABEL);
		label0->setFont(font_D_LBL1);
		label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		label0->setText(QString::fromLocal8Bit("Radius "));

		DosimeterRadius_QLineEdit = new QLineEdit;
		DosimeterRadius_QLineEdit->setFixedSize(Window_width * 0.104, Window_height * 0.028);
		DosimeterRadius_QLineEdit->setAlignment(Qt::AlignCenter);
		DosimeterRadius_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME2);
		DosimeterRadius_QLineEdit->setFont(font_D_LE2);
		DosimeterRadius_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("Dosimeter radius (cm)"));

		hLayout->addWidget(label0);
		hLayout->addStretch();
		hLayout->addWidget(DosimeterRadius_QLineEdit);

		layoutIn->addLayout(hLayout);
	}

	{// Separation Distance
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setContentsMargins(0, Window_height * 0.005, Window_width * 0.010, 0);
		hLayout->setSpacing(Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QLabel* label0 = new QLabel;
		label0->setFixedSize(Window_width * 0.038, Window_height * 0.028);
		label0->setStyleSheet(DialogStyle::DATA_LABEL);
		label0->setFont(font_D_LBL1);
		label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		label0->setText(QString::fromLocal8Bit("Distance"));

		DosimeterSeparationDistance_QLineEdit = new QLineEdit;
		DosimeterSeparationDistance_QLineEdit->setFixedSize(Window_width * 0.104, Window_height * 0.028);
		DosimeterSeparationDistance_QLineEdit->setAlignment(Qt::AlignCenter);
		DosimeterSeparationDistance_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME2);
		DosimeterSeparationDistance_QLineEdit->setFont(font_D_LE2);
		DosimeterSeparationDistance_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("separation dist. (cm)"));

		hLayout->addWidget(label0);
		hLayout->addStretch();
		hLayout->addWidget(DosimeterSeparationDistance_QLineEdit);

		layoutIn->addLayout(hLayout);
	}

	{// 3D Pick and update
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setContentsMargins(0, Window_height * 0.005, Window_width * 0.010, 0);
		hLayout->setSpacing(Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignRight | Qt::AlignTop);

		DosimeterUpdate_QPushButton = new QPushButton(QString::fromLocal8Bit("Update"));
		Buttons_FunctionPanelRight.append(DosimeterUpdate_QPushButton);
		DosimeterUpdate_QPushButton->setFixedSize(Window_width * 0.042, Window_height * 0.037);
		DosimeterUpdate_QPushButton->setStyleSheet(DialogStyle::MENU_BTN2);
		DosimeterUpdate_QPushButton->setFont(font_D_BTN2);

		connect(DosimeterUpdate_QPushButton, SIGNAL(clicked()), this, SLOT(slot_DosimeterUpdate_ButtonClicked())); connect(DosimeterUpdate_QPushButton, SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));

		hLayout->addStretch();
		hLayout->addWidget(DosimeterUpdate_QPushButton);

		layoutIn->addLayout(hLayout);
	}	

	GroupBoxIn->setLayout(layoutIn);
	scrollArea->setWidget(GroupBoxIn);
	scrollArea->setWidgetResizable(true);
	layoutOut->addWidget(scrollArea);
	GroupBoxOut->setLayout(layoutOut);
	layout->addWidget(GroupBoxOut);
}

//////////////////////// Phantoms //////////////////////// 

// Main function - List
void FunctionPanelRight::Create_Phantom_ListWidgets(QVBoxLayout* layout) // MultiplePhantom button 생성 (scroll area)
{
	PhantomList_QGroupBox = new QGroupBox;
	PhantomList_QGroupBox->setAlignment(Qt::AlignCenter | Qt::AlignLeft);
	PhantomList_QGroupBox->setStyleSheet(DialogStyle::GROUPBOX_WIDGET10);
	PhantomList_QGroupBox->setFont(font_D_GBW10);
	PhantomList_QGroupBox->setFixedHeight(Window_height * 0.060);
	PhantomList_QGroupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed); // 가로, 세로 방향 크기
	PhantomList_QGroupBox->setContentsMargins(Window_width * 0.001, 0, Window_width * 0.001, 0);

	layout->addWidget(PhantomList_QGroupBox);

	QVBoxLayout* layoutOut = new QVBoxLayout();
	layoutOut->setContentsMargins(Window_width * 0.001, 0, Window_width * 0.005, Window_height * 0.005);
	layoutOut->setSpacing(Window_width * 0.005);
	layoutOut->setAlignment(Qt::AlignLeft | Qt::AlignTop);

	QGroupBox* GroupBoxIn = new QGroupBox;	
	GroupBoxIn->setAlignment(Qt::AlignCenter | Qt::AlignLeft);
	GroupBoxIn->setStyleSheet(DialogStyle::GROUPBOX_WIDGET12);
	GroupBoxIn->setFont(font_D_GBW10);
	GroupBoxIn->setFixedHeight(0.060 * Window_height);
	GroupBoxIn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	PhantomList_QHBoxLayout = new QHBoxLayout();
	PhantomList_QHBoxLayout->setContentsMargins(Window_width * 0.001, Window_height * 0.005, Window_width * 0.001, Window_height * 0.005); // 왼쪽, 위, 오른쪽, 아래의 여유공간(해당 layout 안에서의)
	PhantomList_QHBoxLayout->setSpacing(Window_width * 0.003);
	PhantomList_QHBoxLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

	MultipleButtonScorllArea* scrollArea = new MultipleButtonScorllArea();
	scrollArea->setStyleSheet(DialogStyle::GROUPBOX_WIDGET10);
	scrollArea->setFont(font_D_GBW10);
	scrollArea->setFixedHeight(Window_height * 0.060);
	scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // Do not show vertical scrollbar
	PhantomList_Horizontal_QScrollBar = scrollArea->horizontalScrollBar();

	GroupBoxIn->setLayout(PhantomList_QHBoxLayout);
	scrollArea->setWidget(GroupBoxIn);
	scrollArea->setWidgetResizable(true);
	layoutOut->addWidget(scrollArea);
	PhantomList_QGroupBox->setLayout(layoutOut);
	PhantomList_QGroupBox->hide();
	PhantomList_QGroupBox->show();
}
void FunctionPanelRight::slot_PhantomAdd_ButtonClicked()
{
	// == Adding setting dialog 실행 == 
	PhantomAddingSetting_Create(); 
	PhantomSetting_UserQDialog->exec();
	// == Adding setting 완료 == 

	// == Add 루틴들 실행 ==
	ExecutePhantomAddRoutines();	
}
void FunctionPanelRight::ExecutePhantomAddRoutines()
{
	{
		if (m_Is_PhantomSetting_Closed == true) // X버튼으로 나갔을때
		{
			m_Is_PhantomSetting_Closed = false;
			return;
		}
	}

	// == 객체 polydata/actor 생성 ==
	{
		if (PhantomSetting_AirPhantom_QRadioButton->isChecked() == false) //Computational human phantom
		{
			// Generate a human phantom
			BodySizeInfo BodySizeInfo_for_ThisPhantom = theApp.phantomObjects->CalcBoydSizeScaleFactor(PhantomSetting_Type_QComboBox->currentIndex(),
				PhantomSetting_Gender_QComboBox->currentIndex(),
				PhantomSetting_Age_QComboBox->currentIndex(),
				PhantomSetting_Posture_QComboBox->currentIndex(),
				PhantomSetting_Height_QLineEdit->text().toDouble(),
				PhantomSetting_Weight_QLineEdit->text().toDouble());

			QString phantom_path = theApp.phantomObjects->GetPhantomFile_AbsolutePath(PhantomSetting_Type_QComboBox->currentIndex(),
				PhantomSetting_Gender_QComboBox->currentIndex(),
				PhantomSetting_Age_QComboBox->currentIndex(),
				PhantomSetting_Posture_QComboBox->currentIndex(),
				PhantomSetting_FetalAge_QComboBox->currentIndex(),
				m_Phantom_MakingIndex,
				BodySizeInfo_for_ThisPhantom);

			theApp.phantomObjects->PhantomPolydataActor_Generate(phantom_path, m_Phantom_MakingIndex, BodySizeInfo_for_ThisPhantom);
		}
		else if (PhantomSetting_AirPhantom_QRadioButton->isChecked() == true) // Air phantom
		{
			// Generate a air sphere phantom
			vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
			sphereSource->SetCenter(0.0, 0.0, 0.0);
			sphereSource->SetRadius(15); // 직경 30 cm 고정
			sphereSource->SetPhiResolution(60);
			sphereSource->SetThetaResolution(30);
			vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
			mapper->SetInputConnection(sphereSource->GetOutputPort());
			vtkSmartPointer<vtkNamedColors> colors = vtkSmartPointer<vtkNamedColors>::New();
			vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
			actor->SetMapper(mapper);
			actor->SetProperty(Util::GetProperty());
			actor->GetProperty()->SetColor(colors->GetColor3d("Magenta").GetData());
			actor->GetProperty()->SetOpacity(0.5);
			theApp.PhantomPanelActor[m_Phantom_MakingIndex] = actor;
			theApp.PhantomPanelActor[m_Phantom_MakingIndex]->SetPosition(m_Phantom_MainInfo[m_Phantom_MakingIndex][6], m_Phantom_MainInfo[m_Phantom_MakingIndex][7], m_Phantom_MainInfo[m_Phantom_MakingIndex][8]);
			theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(theApp.PhantomPanelActor[m_Phantom_MakingIndex]);
			theApp.m_pVTKWidget->renderWindow()->Render();
		}
	}

	// == List 버튼 생성 ==
	{
		if (m_Phantom_MainInfo[m_Phantom_MakingIndex][E_PHANTOMMAININFO_CATEGORY] == E_PHANTOMCATEGORY_AIR) // 에어 팬텀이면 
		{
			if (PhantomSetting_Name_QLineEdit->text().isEmpty()) PhantomSelect_QPushButton[m_Phantom_MakingIndex] = new QPushButton("AirSphere_" + QString::number(m_Phantom_MakingIndex));
			else PhantomSelect_QPushButton[m_Phantom_MakingIndex] = new QPushButton(PhantomSetting_Name_QLineEdit->text());
		}
		else // 전산팬텀이면
		{
			if (PhantomSetting_Name_QLineEdit->text().isEmpty()) PhantomSelect_QPushButton[m_Phantom_MakingIndex] = new QPushButton("Phantom_" + QString::number(m_Phantom_MakingIndex));
			else PhantomSelect_QPushButton[m_Phantom_MakingIndex] = new QPushButton(PhantomSetting_Name_QLineEdit->text());
		}
		// Generate button
		Buttons_FunctionPanelRight.append(PhantomSelect_QPushButton[m_Phantom_MakingIndex]);
		PhantomSelect_QPushButton[m_Phantom_MakingIndex]->setObjectName(QString::number(m_Phantom_MakingIndex));
		PhantomSelect_QPushButton[m_Phantom_MakingIndex]->setFixedSize(Window_width * 0.05, Window_height * 0.040);
		PhantomSelect_QPushButton[m_Phantom_MakingIndex]->setStyleSheet(DialogStyle::MENU_BTN7);
		PhantomSelect_QPushButton[m_Phantom_MakingIndex]->setFont(font_D_BTN14);
		setElidedButtonText(PhantomSelect_QPushButton[m_Phantom_MakingIndex]); // setFont 뒤에 해야 함!
		PhantomSelect_QPushButton[m_Phantom_MakingIndex]->setCheckable(true);
		PhantomSelect_QPushButton[m_Phantom_MakingIndex]->setChecked(true);
		connect(PhantomSelect_QPushButton[m_Phantom_MakingIndex], SIGNAL(clicked()), this, SLOT(slot_PhantomSelect_ButtonClicked())); connect(PhantomSelect_QPushButton[m_Phantom_MakingIndex], SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
		PhantomList_QHBoxLayout->addWidget(PhantomSelect_QPushButton[m_Phantom_MakingIndex]);
		timer_scroll_multiplePhantom->start(1); // List 스크롤
		if (m_Phantom_SequenceVector.size() == const_Phantom_MaximumCount) PhantomAdd_QPushButton->setEnabled(false); // 최대치 도달 시 Add 불가능하게
	}


	// Clothing 패널
	{
		RemoveClothingPanel();
	}

	// Glasses 패널
	{

	}

	// Dosimeter 패널
	{

	}

	//=================================================================

	// Set global variables 
	{
		m_Phantom_SelectedIndex = m_Phantom_MakingIndex;
		m_Phantom_SequenceVector.push_back(m_Phantom_SelectedIndex);
		m_Phantom_MakingIndex++;
	}
	// 객체 업데이트
	{
		// 팬텀 list 버튼 업데이트
		UpdatePhantom_ListButton(m_Phantom_SelectedIndex);

		// 팬텀 actor 업데이트
		theApp.UpdatePhantom_ActorHighlighted(m_Phantom_SelectedIndex); // 현재 선택된 팬텀만 pickable 및 highlight

		// 팬텀 info status 업데이트
		UpdatePhantom_InfoStatus(m_Phantom_SelectedIndex);
	}

	// Dummy on 이면 dummy checkbox 클릭 및 더미로 가시화
	{
		if (PhantomSetting_Dummy_QComboBox->currentIndex() == 0)
		{
			PhantomDummy_QCheckBox->setChecked(true);
			slot_DummyPhantomCheckBoxClicked();
		}
	}
}
void FunctionPanelRight::slot_PhantomDelete_ButtonClicked()
{
	if (m_Phantom_SequenceVector.size() < 1) // 팬텀이 1개도 없을 때는 delete declined
	{
		theApp.SetMessageBox("There is no phantom to delete!");
		return;
	}
	
	// 기존 버튼 지우기	
	int originalposition = theApp.removeElement(m_Phantom_SequenceVector, m_Phantom_SelectedIndex); //{0 1 2} -> {0 2}, origianl index는 지워진 1의 자리	
	PhantomList_QHBoxLayout->removeWidget(PhantomSelect_QPushButton[m_Phantom_SelectedIndex]);
	Buttons_FunctionPanelRight.removeOne(PhantomSelect_QPushButton[m_Phantom_SelectedIndex]);
	delete PhantomSelect_QPushButton[m_Phantom_SelectedIndex];
	PhantomSelect_QPushButton.erase(m_Phantom_SelectedIndex);

	// Remove actor
	{
		theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(theApp.PhantomPanelActor[m_Phantom_SelectedIndex]);
		theApp.m_pVTKWidget->renderWindow()->Render();
		theApp.PhantomPanelActor[m_Phantom_SelectedIndex] = nullptr;
	}
	
	// Remove polyata
	{
		theApp.PhantomPanelPolyData_Collection[m_Phantom_SelectedIndex][0][0] = nullptr;
		for (auto itr_PolyData : theApp.PhantomPanelPolyData_Collection[m_Phantom_SelectedIndex][1]) // Clothing
		{
			theApp.PhantomPanelPolyData_Collection[m_Phantom_SelectedIndex][1][itr_PolyData.first] = nullptr;
		}
		for (auto itr_PolyData : theApp.PhantomPanelPolyData_Collection[m_Phantom_SelectedIndex][2]) // Dosimeter
		{
			theApp.PhantomPanelPolyData_Collection[m_Phantom_SelectedIndex][2][itr_PolyData.first] = nullptr;
		}
		for (auto itr_PolyData : theApp.PhantomPanelPolyData_Collection[m_Phantom_SelectedIndex][3]) // Glasses
		{
			theApp.PhantomPanelPolyData_Collection[m_Phantom_SelectedIndex][3][itr_PolyData.first] = nullptr;
		}
	}

	// Phantom 패널
	{
		m_Phantom_MainInfo[m_Phantom_SelectedIndex].clear(); // phantom info data 지우기
	}
	

	// Clothing 패널
	{
		// Clothing
		RemoveClothingPanel(); // Clothing 패널 버튼 제거
		m_Clothing_SequenceVector[m_Phantom_SelectedIndex].clear(); // Clothing 벡터 초기화 -> 벡터 초기화로 모두 초기화됨
	}


	///////////////////////////// remove 완료, 새로 만들기///////////////////

	// SelectedIndex 설정
	{
		if (m_Phantom_SequenceVector.size() == 0) m_Phantom_SelectedIndex = -1; // Phantom 개수 0일때 현재 선택된 index = -1(Invalid)로 설정
		else m_Phantom_SelectedIndex = m_Phantom_SequenceVector[originalposition]; // 새로운 인덱스로 설정
	}
	
	// 팬텀 list 버튼 업데이트
	{
		UpdatePhantom_ListButton(m_Phantom_SelectedIndex);
	}

	// InfoStatus 업데이트
	{
		UpdatePhantom_InfoStatus(m_Phantom_SelectedIndex); // Info Status창 업데이트
	}
	
	// 팬텀 actor 업데이트
	{
		theApp.UpdatePhantom_ActorHighlighted(m_Phantom_SelectedIndex); // 현재 선택된 팬텀만 highlight
	}
	
	// 의복 업데이트
	{
		SetClothingPanel_inPhantomDelete();
	}
}
void FunctionPanelRight::slot_PhantomSelect_ButtonClicked() // 다른 팬텀 선택 버튼
{
	QPushButton* buttonSender = qobject_cast<QPushButton*>(sender());

	// 원래 버튼 클릭 시 다시 check하고 돌아가기
	if (buttonSender->objectName().toInt() == m_Phantom_SelectedIndex)
	{
		PhantomSelect_QPushButton[m_Phantom_SelectedIndex]->setChecked(true);
		return;
	}

	// Clothing 패널 제거
	{
		RemoveClothingPanel();
	}

	// Glasses 패널 제거
	{

	}

	// Dosimeter 패널 제거
	{

	}

	////////////////// phantom seleceted index 설정/////////////////////
	m_Phantom_SelectedIndex = buttonSender->objectName().toInt();
	int id = m_Phantom_SelectedIndex;

	// 객체 업데이트
	{
		// 팬텀 list 버튼 업데이트
		UpdatePhantom_ListButton(id);

		// 팬텀 actor 업데이트
		theApp.UpdatePhantom_ActorHighlighted(id); // 현재 선택된 팬텀만 pickable 및 highlight

		// 팬텀 info status 업데이트
		UpdatePhantom_InfoStatus(id);
	}

	// Clothing 정보 업데이트
	{
		SetClothingPanel_inPhantomSelect();
	}
	
}
void FunctionPanelRight::slot_PhantomMoveLeft_ButtonClicked()
{
	if (m_Phantom_SequenceVector.size() < 2) return; // 객체가 2개 미만이면 함수를 즉시 종료합니다.

	int swappedIndex = theApp.transposeElement(m_Phantom_SequenceVector, m_Phantom_SelectedIndex, "left");
	bool removeButton = false;
	for (auto itr_phantomSequence : m_Phantom_SequenceVector)
	{
		if (itr_phantomSequence == swappedIndex) removeButton = true;
		if (removeButton)
		{
			PhantomList_QHBoxLayout->removeWidget(PhantomSelect_QPushButton[itr_phantomSequence]);
			Buttons_FunctionPanelRight.removeOne(PhantomSelect_QPushButton[itr_phantomSequence]);
			delete PhantomSelect_QPushButton[itr_phantomSequence];
			PhantomSelect_QPushButton.erase(itr_phantomSequence);
		}
	}
	bool generateButton = false;
	for (auto itr_phantomSequence : m_Phantom_SequenceVector)
	{
		if (itr_phantomSequence == swappedIndex) generateButton = true;
		if (generateButton)
		{
			PhantomSelect_QPushButton[itr_phantomSequence] = new QPushButton(m_Phantom_ButtonName[itr_phantomSequence]);
			Buttons_FunctionPanelRight.append(PhantomSelect_QPushButton[itr_phantomSequence]);
			PhantomSelect_QPushButton[itr_phantomSequence]->setObjectName(QString::number(itr_phantomSequence));
			PhantomSelect_QPushButton[itr_phantomSequence]->setFixedSize(Window_width * 0.05, Window_height * 0.040);
			PhantomSelect_QPushButton[itr_phantomSequence]->setStyleSheet(DialogStyle::MENU_BTN7);
			PhantomSelect_QPushButton[itr_phantomSequence]->setFont(font_D_BTN14);
			setElidedButtonText(PhantomSelect_QPushButton[itr_phantomSequence]); // setFont 뒤에 해야 함!
			PhantomSelect_QPushButton[itr_phantomSequence]->setCheckable(true);
			connect(PhantomSelect_QPushButton[itr_phantomSequence], SIGNAL(clicked()), this, SLOT(slot_PhantomSelect_ButtonClicked())); connect(PhantomSelect_QPushButton[itr_phantomSequence], SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
			PhantomList_QHBoxLayout->addWidget(PhantomSelect_QPushButton[itr_phantomSequence]);
		}
	}
	PhantomSelect_QPushButton[m_Phantom_SelectedIndex]->setChecked(true);
}
void FunctionPanelRight::slot_PhantomMoveRight_ButtonClicked()
{
	if (m_Phantom_SequenceVector.size() < 2) return; // 객체가 2개 미만이면 함수를 즉시 종료합니다.

	int swappedIndex = theApp.transposeElement(m_Phantom_SequenceVector, m_Phantom_SelectedIndex, "right");
	bool removeButton = false;
	for (auto itr_phantomSequence : m_Phantom_SequenceVector)
	{
		if (itr_phantomSequence == m_Phantom_SelectedIndex) removeButton = true;
		if (removeButton)
		{
			PhantomList_QHBoxLayout->removeWidget(PhantomSelect_QPushButton[itr_phantomSequence]);
			Buttons_FunctionPanelRight.removeOne(PhantomSelect_QPushButton[itr_phantomSequence]);
			delete PhantomSelect_QPushButton[itr_phantomSequence];
			PhantomSelect_QPushButton.erase(itr_phantomSequence);
		}
	}
	bool generateButton = false;
	for (auto itr_phantomSequence : m_Phantom_SequenceVector)
	{
		if (itr_phantomSequence == m_Phantom_SelectedIndex) generateButton = true;
		if (generateButton)
		{
			PhantomSelect_QPushButton[itr_phantomSequence] = new QPushButton(m_Phantom_ButtonName[itr_phantomSequence]);
			Buttons_FunctionPanelRight.append(PhantomSelect_QPushButton[itr_phantomSequence]);
			PhantomSelect_QPushButton[itr_phantomSequence]->setObjectName(QString::number(itr_phantomSequence));
			PhantomSelect_QPushButton[itr_phantomSequence]->setFixedSize(Window_width * 0.05, Window_height * 0.040);
			PhantomSelect_QPushButton[itr_phantomSequence]->setStyleSheet(DialogStyle::MENU_BTN7);
			PhantomSelect_QPushButton[itr_phantomSequence]->setFont(font_D_BTN14);
			setElidedButtonText(PhantomSelect_QPushButton[itr_phantomSequence]); // setFont 뒤에 해야 함!
			PhantomSelect_QPushButton[itr_phantomSequence]->setCheckable(true);
			connect(PhantomSelect_QPushButton[itr_phantomSequence], SIGNAL(clicked()), this, SLOT(slot_PhantomSelect_ButtonClicked())); connect(PhantomSelect_QPushButton[itr_phantomSequence], SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
			PhantomList_QHBoxLayout->addWidget(PhantomSelect_QPushButton[itr_phantomSequence]);
		}
	}
	PhantomSelect_QPushButton[m_Phantom_SelectedIndex]->setChecked(true);
}

// Main function - Adding Setting
void FunctionPanelRight::PhantomAddingSetting_Create()
{
	m_Is_PhantomSetting_OKbutton_Clicked = false;
	// Create a new dialog
	PhantomSetting_UserQDialog = new MultipleUIDialog(this);
	PhantomSetting_UserQDialog->setStyleSheet("background-color: rgb(205, 222, 243);");
	PhantomSetting_UserQDialog->setFixedWidth(Window_width * 0.3);
	PhantomSetting_UserQDialog->setFixedHeight(Window_height * 0.72);
	PhantomSetting_UserQDialog->setWindowTitle("Phantom settings");
	PhantomSetting_UserQDialog->setFocusPolicy(Qt::StrongFocus);
	QScrollArea* scrollArea = new QScrollArea(PhantomSetting_UserQDialog);
	scrollArea->setWidgetResizable(true);
	scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	QWidget* containerWidget = new QWidget;
	QVBoxLayout* layout = new QVBoxLayout(containerWidget);

	{// Name
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);
		hLayout->setSpacing(Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		PhantomSetting_Category_QLabel = new QLabel;
		PhantomSetting_Category_QLabel->setFixedSize(Window_width * 0.090, Window_height * 0.028);
		PhantomSetting_Category_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		PhantomSetting_Category_QLabel->setFont(font_D_LBL1);
		PhantomSetting_Category_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		PhantomSetting_Category_QLabel->setText(QString::fromLocal8Bit("Category:"));

		// 버튼생성
		PhantomSetting_ComputationlPhantom_QRadioButton = new QRadioButton(QString::fromLocal8Bit("Computational phantom"));
		PhantomSetting_ComputationlPhantom_QRadioButton->setStyleSheet(DialogStyle::RADIOBUTTON_WIDGET);
		PhantomSetting_ComputationlPhantom_QRadioButton->setFont(font_D_RB3);
		PhantomSetting_ComputationlPhantom_QRadioButton->setFixedSize(Window_width * 0.099, Window_height * 0.028);
		PhantomSetting_ComputationlPhantom_QRadioButton->setChecked(true);

		// 버튼생성
		PhantomSetting_AirPhantom_QRadioButton = new QRadioButton(QString::fromLocal8Bit("Air sphere phantom"));
		PhantomSetting_AirPhantom_QRadioButton->setStyleSheet(DialogStyle::RADIOBUTTON_WIDGET);
		PhantomSetting_AirPhantom_QRadioButton->setFont(font_D_RB3);
		PhantomSetting_AirPhantom_QRadioButton->setFixedSize(Window_width * 0.099, Window_height * 0.028);
		PhantomSetting_AirPhantom_QRadioButton->setChecked(false);

		connect(PhantomSetting_ComputationlPhantom_QRadioButton, SIGNAL(clicked()), this, SLOT(slot_PhantomSetting_ComputationlPhantom_RadioButtonClicked())); connect(PhantomSetting_ComputationlPhantom_QRadioButton, SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
		connect(PhantomSetting_AirPhantom_QRadioButton, SIGNAL(clicked()), this, SLOT(slot_PhantomSetting_AirPhantom_RadioButtonClicked())); connect(PhantomSetting_AirPhantom_QRadioButton, SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));

		hLayout->addWidget(PhantomSetting_Category_QLabel);
		hLayout->addWidget(PhantomSetting_ComputationlPhantom_QRadioButton);
		hLayout->addWidget(PhantomSetting_AirPhantom_QRadioButton);

		layout->addLayout(hLayout);
	}

	{// Name
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);
		hLayout->setSpacing(Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		PhantomSetting_Name_QLabel = new QLabel;
		PhantomSetting_Name_QLabel->setFixedSize(Window_width * 0.090, Window_height * 0.028);
		PhantomSetting_Name_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		PhantomSetting_Name_QLabel->setFont(font_D_LBL1);
		PhantomSetting_Name_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		PhantomSetting_Name_QLabel->setText(QString::fromLocal8Bit("Name:"));

		PhantomSetting_Name_QLineEdit = new QLineEdit;
		PhantomSetting_Name_QLineEdit->setFixedSize(Window_width * 0.170, Window_height * 0.028);
		PhantomSetting_Name_QLineEdit->setAlignment(Qt::AlignCenter);
		PhantomSetting_Name_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		PhantomSetting_Name_QLineEdit->setFont(font_D_LE1);
		PhantomSetting_Name_QLineEdit->setPlaceholderText("Phantom_" + QString::number(m_Phantom_MakingIndex));

		hLayout->addWidget(PhantomSetting_Name_QLabel);
		hLayout->addWidget(PhantomSetting_Name_QLineEdit);

		layout->addLayout(hLayout);
	}

	{// Type
		PhantomSetting_Type_QHBoxLayout = new QHBoxLayout;
		PhantomSetting_Type_QHBoxLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);
		PhantomSetting_Type_QHBoxLayout->setSpacing(Window_width * 0.003);
		PhantomSetting_Type_QHBoxLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		PhantomSetting_Type_QLabel = new QLabel;
		PhantomSetting_Type_QLabel->setFixedSize(Window_width * 0.090, Window_height * 0.028);
		PhantomSetting_Type_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		PhantomSetting_Type_QLabel->setFont(font_D_LBL1);
		PhantomSetting_Type_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		PhantomSetting_Type_QLabel->setText(QString::fromLocal8Bit("Type:"));

		PhantomSetting_Type_QComboBox = new QComboBox;
		PhantomSetting_Type_QComboBox->setFixedSize(Window_width * 0.170, Window_height * 0.028);
		PhantomSetting_Type_QComboBox->setStyleSheet(DialogStyle::COMBOBOX);
		PhantomSetting_Type_QComboBox->setFont(font_D_CB1);

		PhantomSetting_Type_QComboBox->addItem("Adult MRCPs");
		PhantomSetting_Type_QComboBox->addItem("Pediatric MRCPs");
		PhantomSetting_Type_QComboBox->addItem("Pregnant-female MRCPs");
		PhantomSetting_Type_QComboBox->addItem("Transformed Phantom");
		PhantomSetting_Type_QComboBox->addItem("Imported Phantom");
		PhantomSetting_Type_QComboBox->setCurrentIndex(0);

		PhantomSetting_Type_QComboBox->setFocusPolicy(Qt::StrongFocus); // For focusing when scroll the comobox
		connect(PhantomSetting_Type_QComboBox, SIGNAL(activated(int)), this, SLOT(slot_ChangePhantomTypeCombo_Setting(int)));

		PhantomSetting_Type_QHBoxLayout->addWidget(PhantomSetting_Type_QLabel);
		PhantomSetting_Type_QHBoxLayout->addWidget(PhantomSetting_Type_QComboBox);

		layout->addLayout(PhantomSetting_Type_QHBoxLayout);
	}

	{ // Phantom Import
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(Window_width * 0.003, 0, Window_width * 0.003, 0);

		PhantomSetting_ImportedFileName_QLineEdit = new QLineEdit;
		PhantomSetting_ImportedFileName_QLineEdit->setFixedSize(Window_width * 0.195, Window_height * 0.028);
		PhantomSetting_ImportedFileName_QLineEdit->setAlignment(Qt::AlignCenter);
		PhantomSetting_ImportedFileName_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		PhantomSetting_ImportedFileName_QLineEdit->setFont(font_D_LE5);
		PhantomSetting_ImportedFileName_QLineEdit->setPlaceholderText("None");
		PhantomSetting_ImportedFileName_QLineEdit->setReadOnly(true);
		PhantomSetting_ImportedFileName_QLineEdit->hide();

		PhantomSetting_PhantomImport_QPushButton = new QPushButton(QString::fromLocal8Bit("Import"));
		Buttons_FunctionPanelRight.append(PhantomSetting_PhantomImport_QPushButton);
		PhantomSetting_PhantomImport_QPushButton->setFixedSize(Window_width * 0.065, Window_height * 0.028);
		PhantomSetting_PhantomImport_QPushButton->setCheckable(false);
		PhantomSetting_PhantomImport_QPushButton->setStyleSheet(DialogStyle::MENU_BTN3);
		PhantomSetting_PhantomImport_QPushButton->setFont(font_D_BTN3);
		PhantomSetting_PhantomImport_QPushButton->hide();

		connect(PhantomSetting_PhantomImport_QPushButton, SIGNAL(clicked()), this, SLOT(slot_PhantomSetting_Import_ButtonClicked())); connect(PhantomSetting_PhantomImport_QPushButton, SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));

		hLayout->addWidget(PhantomSetting_ImportedFileName_QLineEdit);
		hLayout->addWidget(PhantomSetting_PhantomImport_QPushButton);

		layout->addLayout(hLayout);
	}

	{// Dummy
		PhantomSetting_Dummy_QHBoxLayout = new QHBoxLayout;
		PhantomSetting_Dummy_QHBoxLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);
		PhantomSetting_Dummy_QHBoxLayout->setSpacing(Window_width * 0.003);
		PhantomSetting_Dummy_QHBoxLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		PhantomSetting_Dummy_QLabel = new QLabel;
		PhantomSetting_Dummy_QLabel->setFixedSize(Window_width * 0.090, Window_height * 0.028);
		PhantomSetting_Dummy_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		PhantomSetting_Dummy_QLabel->setFont(font_D_LBL1);
		PhantomSetting_Dummy_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		PhantomSetting_Dummy_QLabel->setText(QString::fromLocal8Bit("Dummy phantom:"));

		PhantomSetting_Dummy_QComboBox = new QComboBox;
		PhantomSetting_Dummy_QComboBox->setFixedSize(Window_width * 0.170, Window_height * 0.028);
		PhantomSetting_Dummy_QComboBox->setStyleSheet(DialogStyle::COMBOBOX);
		PhantomSetting_Dummy_QComboBox->setFont(font_D_CB1);

		PhantomSetting_Dummy_QComboBox->addItem("Yes");
		PhantomSetting_Dummy_QComboBox->addItem("No (Full phantom)");
		PhantomSetting_Dummy_QComboBox->setCurrentIndex(1);

		PhantomSetting_Dummy_QComboBox->setFocusPolicy(Qt::StrongFocus); // For focusing when scroll the comobox
		connect(PhantomSetting_Dummy_QComboBox, SIGNAL(activated(int)), this, SLOT(slot_ChangePhantomDummyCombo_Setting(int)));

		PhantomSetting_Dummy_QHBoxLayout->addWidget(PhantomSetting_Dummy_QLabel);
		PhantomSetting_Dummy_QHBoxLayout->addWidget(PhantomSetting_Dummy_QComboBox);

		layout->addLayout(PhantomSetting_Dummy_QHBoxLayout);
	}

	{// Gender
		PhantomSetting_Gender_QHBoxLayout = new QHBoxLayout;
		PhantomSetting_Gender_QHBoxLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);
		PhantomSetting_Gender_QHBoxLayout->setSpacing(Window_width * 0.003);
		PhantomSetting_Gender_QHBoxLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		PhantomSetting_Gender_QLabel = new QLabel;
		PhantomSetting_Gender_QLabel->setFixedSize(Window_width * 0.090, Window_height * 0.028);
		PhantomSetting_Gender_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		PhantomSetting_Gender_QLabel->setFont(font_D_LBL1);
		PhantomSetting_Gender_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		PhantomSetting_Gender_QLabel->setText(QString::fromLocal8Bit("Gender:"));

		PhantomSetting_Gender_QComboBox = new QComboBox;
		PhantomSetting_Gender_QComboBox->setFixedSize(Window_width * 0.170, Window_height * 0.028);
		PhantomSetting_Gender_QComboBox->setStyleSheet(DialogStyle::COMBOBOX);
		PhantomSetting_Gender_QComboBox->setFont(font_D_CB1);

		PhantomSetting_Gender_QComboBox->addItem("Male");
		PhantomSetting_Gender_QComboBox->addItem("Female");
		PhantomSetting_Gender_QComboBox->setCurrentIndex(0);

		PhantomSetting_Gender_QComboBox->setFocusPolicy(Qt::StrongFocus); // For focusing when scroll the comobox
		connect(PhantomSetting_Gender_QComboBox, SIGNAL(activated(int)), this, SLOT(slot_ChangePhantomGenderCombo_Setting(int)));

		PhantomSetting_Gender_QHBoxLayout->addWidget(PhantomSetting_Gender_QLabel);
		PhantomSetting_Gender_QHBoxLayout->addWidget(PhantomSetting_Gender_QComboBox);

		layout->addLayout(PhantomSetting_Gender_QHBoxLayout);
	}

	{// Age
		PhantomSetting_Age_QHBoxLayout = new QHBoxLayout;
		PhantomSetting_Age_QHBoxLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);
		PhantomSetting_Age_QHBoxLayout->setSpacing(Window_width * 0.003);
		PhantomSetting_Age_QHBoxLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		PhantomSetting_Age_QLabel = new QLabel;
		PhantomSetting_Age_QLabel->setFixedSize(Window_width * 0.090, Window_height * 0.028);
		PhantomSetting_Age_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		PhantomSetting_Age_QLabel->setFont(font_D_LBL1);
		PhantomSetting_Age_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		PhantomSetting_Age_QLabel->setText(QString::fromLocal8Bit("Age:"));

		PhantomSetting_Age_QComboBox = new QComboBox;
		PhantomSetting_Age_QComboBox->setFixedSize(Window_width * 0.170, Window_height * 0.028);
		PhantomSetting_Age_QComboBox->setStyleSheet(DialogStyle::COMBOBOX);
		PhantomSetting_Age_QComboBox->setFont(font_D_CB1);

		PhantomSetting_Age_QComboBox->addItem("Adult");
		PhantomSetting_Age_QComboBox->addItem("15y");
		PhantomSetting_Age_QComboBox->addItem("10y");
		PhantomSetting_Age_QComboBox->addItem("5y");
		PhantomSetting_Age_QComboBox->addItem("1y");
		PhantomSetting_Age_QComboBox->addItem("0y");
		PhantomSetting_Age_QComboBox->setCurrentIndex(0);

		PhantomSetting_Age_QComboBox->setFocusPolicy(Qt::StrongFocus); // For focusing when scroll the comobox
		connect(PhantomSetting_Age_QComboBox, SIGNAL(activated(int)), this, SLOT(slot_ChangePhantomAgeCombo_Setting(int)));

		PhantomSetting_Age_QHBoxLayout->addWidget(PhantomSetting_Age_QLabel);
		PhantomSetting_Age_QHBoxLayout->addWidget(PhantomSetting_Age_QComboBox);

		layout->addLayout(PhantomSetting_Age_QHBoxLayout);
	}

	{// Posture
		PhantomSetting_Posture_QHBoxLayout = new QHBoxLayout;
		PhantomSetting_Posture_QHBoxLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);
		PhantomSetting_Posture_QHBoxLayout->setSpacing(Window_width * 0.003);
		PhantomSetting_Posture_QHBoxLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		PhantomSetting_Posture_QLabel = new QLabel;
		PhantomSetting_Posture_QLabel->setFixedSize(Window_width * 0.090, Window_height * 0.028);
		PhantomSetting_Posture_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		PhantomSetting_Posture_QLabel->setFont(font_D_LBL1);
		PhantomSetting_Posture_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		PhantomSetting_Posture_QLabel->setText(QString::fromLocal8Bit("Posture:"));

		PhantomSetting_Posture_QComboBox = new QComboBox;
		PhantomSetting_Posture_QComboBox->setFixedSize(Window_width * 0.170, Window_height * 0.028);
		PhantomSetting_Posture_QComboBox->setStyleSheet(DialogStyle::COMBOBOX);
		PhantomSetting_Posture_QComboBox->setFont(font_D_CB1);

		PhantomSetting_Posture_QComboBox->addItem("Standing");
		PhantomSetting_Posture_QComboBox->addItem("Bending");
		PhantomSetting_Posture_QComboBox->addItem("Kneeling");
		PhantomSetting_Posture_QComboBox->addItem("Sitting");
		PhantomSetting_Posture_QComboBox->addItem("Squatting");
		PhantomSetting_Posture_QComboBox->addItem("Walking");
		PhantomSetting_Posture_QComboBox->setCurrentIndex(0);

		PhantomSetting_Posture_QComboBox->setFocusPolicy(Qt::StrongFocus); // For focusing when scroll the comobox
		connect(PhantomSetting_Posture_QComboBox, SIGNAL(activated(int)), this, SLOT(slot_ChangePhantomPostureCombo_Setting(int)));

		PhantomSetting_Posture_QHBoxLayout->addWidget(PhantomSetting_Posture_QLabel);
		PhantomSetting_Posture_QHBoxLayout->addWidget(PhantomSetting_Posture_QComboBox);

		PhantomSetting_Posture_QComboBox->setDisabled(true);

		layout->addLayout(PhantomSetting_Posture_QHBoxLayout);
	}

	{// Fetal age
		PhantomSetting_FetalAge_QHBoxLayout = new QHBoxLayout;
		PhantomSetting_FetalAge_QHBoxLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);
		PhantomSetting_FetalAge_QHBoxLayout->setSpacing(Window_width * 0.003);
		PhantomSetting_FetalAge_QHBoxLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		PhantomSetting_FetalAge_QLabel = new QLabel;
		PhantomSetting_FetalAge_QLabel->setFixedSize(Window_width * 0.090, Window_height * 0.028);
		PhantomSetting_FetalAge_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		PhantomSetting_FetalAge_QLabel->setFont(font_D_LBL1);
		PhantomSetting_FetalAge_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		PhantomSetting_FetalAge_QLabel->setText(QString::fromLocal8Bit("Fetal Age:"));

		PhantomSetting_FetalAge_QComboBox = new QComboBox;
		PhantomSetting_FetalAge_QComboBox->setFixedSize(Window_width * 0.170, Window_height * 0.028);
		PhantomSetting_FetalAge_QComboBox->setStyleSheet(DialogStyle::COMBOBOX);
		PhantomSetting_FetalAge_QComboBox->setFont(font_D_CB1);

		PhantomSetting_FetalAge_QComboBox->addItem("08w");
		PhantomSetting_FetalAge_QComboBox->addItem("10w");
		PhantomSetting_FetalAge_QComboBox->addItem("15w");
		PhantomSetting_FetalAge_QComboBox->addItem("20w");
		PhantomSetting_FetalAge_QComboBox->addItem("25w");
		PhantomSetting_FetalAge_QComboBox->addItem("30w");
		PhantomSetting_FetalAge_QComboBox->addItem("35w");
		PhantomSetting_FetalAge_QComboBox->addItem("38w");
		PhantomSetting_FetalAge_QComboBox->setCurrentIndex(0);

		PhantomSetting_FetalAge_QComboBox->setFocusPolicy(Qt::StrongFocus); // For focusing when scroll the comobox
		connect(PhantomSetting_FetalAge_QComboBox, SIGNAL(activated(int)), this, SLOT(slot_ChangePhantomFetalAgeCombo_Setting(int)));

		PhantomSetting_FetalAge_QHBoxLayout->addWidget(PhantomSetting_FetalAge_QLabel);
		PhantomSetting_FetalAge_QHBoxLayout->addWidget(PhantomSetting_FetalAge_QComboBox);

		PhantomSetting_FetalAge_QComboBox->setDisabled(true);

		layout->addLayout(PhantomSetting_FetalAge_QHBoxLayout);
	}

	{// Height
		PhantomSetting_Height_QHBoxLayout = new QHBoxLayout;
		PhantomSetting_Height_QHBoxLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);
		PhantomSetting_Height_QHBoxLayout->setSpacing(Window_width * 0.003);
		PhantomSetting_Height_QHBoxLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		PhantomSetting_Height_QLabel = new QLabel;
		PhantomSetting_Height_QLabel->setFixedSize(Window_width * 0.090, Window_height * 0.028);
		PhantomSetting_Height_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		PhantomSetting_Height_QLabel->setFont(font_D_LBL1);
		PhantomSetting_Height_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		PhantomSetting_Height_QLabel->setText(QString::fromLocal8Bit("Height:"));

		PhantomSetting_Height_QLineEdit = new QLineEdit;
		PhantomSetting_Height_QLineEdit->setFixedSize(Window_width * 0.170, Window_height * 0.028);
		PhantomSetting_Height_QLineEdit->setAlignment(Qt::AlignCenter);
		PhantomSetting_Height_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		PhantomSetting_Height_QLineEdit->setFont(font_D_LE1);
		PhantomSetting_Height_QLineEdit->setPlaceholderText("cm");

		PhantomSetting_Height_QHBoxLayout->addWidget(PhantomSetting_Height_QLabel);
		PhantomSetting_Height_QHBoxLayout->addWidget(PhantomSetting_Height_QLineEdit);

		PhantomSetting_Height_QLineEdit->setDisabled(true);

		layout->addLayout(PhantomSetting_Height_QHBoxLayout);
	}

	{// Weight
		PhantomSetting_Weight_QHBoxLayout = new QHBoxLayout;
		PhantomSetting_Weight_QHBoxLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);
		PhantomSetting_Weight_QHBoxLayout->setSpacing(Window_width * 0.003);
		PhantomSetting_Weight_QHBoxLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		PhantomSetting_Weight_QLabel = new QLabel;
		PhantomSetting_Weight_QLabel->setFixedSize(Window_width * 0.090, Window_height * 0.028);
		PhantomSetting_Weight_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		PhantomSetting_Weight_QLabel->setFont(font_D_LBL1);
		PhantomSetting_Weight_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		PhantomSetting_Weight_QLabel->setText(QString::fromLocal8Bit("Weight:"));

		PhantomSetting_Weight_QLineEdit = new QLineEdit;
		PhantomSetting_Weight_QLineEdit->setFixedSize(Window_width * 0.170, Window_height * 0.028);
		PhantomSetting_Weight_QLineEdit->setAlignment(Qt::AlignCenter);
		PhantomSetting_Weight_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		PhantomSetting_Weight_QLineEdit->setFont(font_D_LE1);
		PhantomSetting_Weight_QLineEdit->setPlaceholderText("kg");

		PhantomSetting_Weight_QHBoxLayout->addWidget(PhantomSetting_Weight_QLabel);
		PhantomSetting_Weight_QHBoxLayout->addWidget(PhantomSetting_Weight_QLineEdit);

		PhantomSetting_Weight_QLineEdit->setDisabled(true);

		layout->addLayout(PhantomSetting_Weight_QHBoxLayout);
	}

	{// Center
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);
		hLayout->setSpacing(Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		PhantomSetting_Position_QLabel = new QLabel;
		PhantomSetting_Position_QLabel->setFixedSize(Window_width * 0.090, Window_height * 0.028);
		PhantomSetting_Position_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		PhantomSetting_Position_QLabel->setFont(font_D_LBL1);
		PhantomSetting_Position_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		PhantomSetting_Position_QLabel->setText(QString::fromLocal8Bit("Center XYZ:"));

		PhantomSetting_PosX_QLineEdit = new QLineEdit;
		PhantomSetting_PosX_QLineEdit->setFixedSize(Window_width * 0.0545, Window_height * 0.028);
		PhantomSetting_PosX_QLineEdit->setAlignment(Qt::AlignCenter);
		PhantomSetting_PosX_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		PhantomSetting_PosX_QLineEdit->setFont(font_D_LE1);
		PhantomSetting_PosX_QLineEdit->setPlaceholderText("cm");

		PhantomSetting_PosY_QLineEdit = new QLineEdit;
		PhantomSetting_PosY_QLineEdit->setFixedSize(Window_width * 0.0545, Window_height * 0.028);
		PhantomSetting_PosY_QLineEdit->setAlignment(Qt::AlignCenter);
		PhantomSetting_PosY_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		PhantomSetting_PosY_QLineEdit->setFont(font_D_LE1);
		PhantomSetting_PosY_QLineEdit->setPlaceholderText("cm");

		PhantomSetting_PosZ_QLineEdit = new QLineEdit;
		PhantomSetting_PosZ_QLineEdit->setFixedSize(Window_width * 0.0545, Window_height * 0.028);
		PhantomSetting_PosZ_QLineEdit->setAlignment(Qt::AlignCenter);
		PhantomSetting_PosZ_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		PhantomSetting_PosZ_QLineEdit->setFont(font_D_LE1);
		PhantomSetting_PosZ_QLineEdit->setPlaceholderText("cm");

		hLayout->addWidget(PhantomSetting_Position_QLabel);
		hLayout->addWidget(PhantomSetting_PosX_QLineEdit);
		hLayout->addWidget(PhantomSetting_PosY_QLineEdit);
		hLayout->addWidget(PhantomSetting_PosZ_QLineEdit);

		layout->addLayout(hLayout);
	}

	{// Rotation 
		PhantomSetting_Rot_QHBoxLayout = new QHBoxLayout;
		PhantomSetting_Rot_QHBoxLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);
		PhantomSetting_Rot_QHBoxLayout->setSpacing(Window_width * 0.003);
		PhantomSetting_Rot_QHBoxLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		PhantomSetting_Rotation_QLabel = new QLabel;
		PhantomSetting_Rotation_QLabel->setFixedSize(Window_width * 0.090, Window_height * 0.028);
		PhantomSetting_Rotation_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		PhantomSetting_Rotation_QLabel->setFont(font_D_LBL1);
		PhantomSetting_Rotation_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		PhantomSetting_Rotation_QLabel->setText(QString::fromLocal8Bit("Rotate XYZ:"));

		PhantomSetting_RotX_QLineEdit = new QLineEdit;
		PhantomSetting_RotX_QLineEdit->setFixedSize(Window_width * 0.0545, Window_height * 0.028);
		PhantomSetting_RotX_QLineEdit->setAlignment(Qt::AlignCenter);
		PhantomSetting_RotX_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		PhantomSetting_RotX_QLineEdit->setFont(font_D_LE1);
		PhantomSetting_RotX_QLineEdit->setPlaceholderText("deg");

		PhantomSetting_RotY_QLineEdit = new QLineEdit;
		PhantomSetting_RotY_QLineEdit->setFixedSize(Window_width * 0.0545, Window_height * 0.028);
		PhantomSetting_RotY_QLineEdit->setAlignment(Qt::AlignCenter);
		PhantomSetting_RotY_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		PhantomSetting_RotY_QLineEdit->setFont(font_D_LE1);
		PhantomSetting_RotY_QLineEdit->setPlaceholderText("deg");

		PhantomSetting_RotZ_QLineEdit = new QLineEdit;
		PhantomSetting_RotZ_QLineEdit->setFixedSize(Window_width * 0.0545, Window_height * 0.028);
		PhantomSetting_RotZ_QLineEdit->setAlignment(Qt::AlignCenter);
		PhantomSetting_RotZ_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		PhantomSetting_RotZ_QLineEdit->setFont(font_D_LE1);
		PhantomSetting_RotZ_QLineEdit->setPlaceholderText("deg");

		PhantomSetting_Rot_QHBoxLayout->addWidget(PhantomSetting_Rotation_QLabel);
		PhantomSetting_Rot_QHBoxLayout->addWidget(PhantomSetting_RotX_QLineEdit);
		PhantomSetting_Rot_QHBoxLayout->addWidget(PhantomSetting_RotY_QLineEdit);
		PhantomSetting_Rot_QHBoxLayout->addWidget(PhantomSetting_RotZ_QLineEdit);

		layout->addLayout(PhantomSetting_Rot_QHBoxLayout);
	}

	{// OK Button
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		hLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.018, 0);

		PhantomSetting_AddingOK_QPushButton = new QPushButton(QString::fromLocal8Bit("OK"));
		Buttons_FunctionPanelRight.append(PhantomSetting_AddingOK_QPushButton);
		PhantomSetting_AddingOK_QPushButton->setFixedSize(Window_width * 0.104, Window_height * 0.028);
		PhantomSetting_AddingOK_QPushButton->setCheckable(false);
		PhantomSetting_AddingOK_QPushButton->setStyleSheet(DialogStyle::MENU_BTN3);
		PhantomSetting_AddingOK_QPushButton->setFont(font_D_BTN3);

		connect(PhantomSetting_AddingOK_QPushButton, SIGNAL(clicked()), this, SLOT(slot_PhantomAddingOK_ButtonClicked())); connect(PhantomSetting_AddingOK_QPushButton, SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));

		hLayout->addWidget(PhantomSetting_AddingOK_QPushButton);

		layout->addLayout(hLayout);
	}

	scrollArea->setWidget(containerWidget);
	QVBoxLayout* dialogLayout = new QVBoxLayout(PhantomSetting_UserQDialog);
	dialogLayout->addWidget(scrollArea);
	PhantomSetting_UserQDialog->setLayout(dialogLayout);
	PhantomSetting_UserQDialog->adjustSize();

	//PhantomSetting_UserQDialog->exec(); // Use exec() for a modal dialog or show() for a non-modal dialog
}
void FunctionPanelRight::slot_PhantomAddingOK_ButtonClicked()
{
	SavePhantom_InfoData_InAddingSetting(m_Phantom_MakingIndex); // Phantom info 컨테이너에 정보 저장	

	// AddingOK에 Setting widget 정상 종료를 위해 반드시 필요함
	m_Is_PhantomSetting_OKbutton_Clicked = true;	
	PhantomSetting_UserQDialog->close();
}
void FunctionPanelRight::slot_PhantomSetting_ComputationlPhantom_RadioButtonClicked()
{
	PhantomSetting_ComputationlPhantom_QRadioButton->setChecked(true);
	PhantomSetting_AirPhantom_QRadioButton->setChecked(false);

	// Label, buttons show
	PhantomSetting_Name_QLineEdit->setPlaceholderText("Phantom_" + QString::number(m_Phantom_MakingIndex));

	PhantomSetting_Type_QHBoxLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);
	PhantomSetting_Type_QHBoxLayout->setSpacing(Window_width * 0.003);
	PhantomSetting_Type_QLabel->show();
	PhantomSetting_Type_QComboBox->show();

	PhantomSetting_Dummy_QHBoxLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);
	PhantomSetting_Dummy_QHBoxLayout->setSpacing(Window_width * 0.003);
	PhantomSetting_Dummy_QLabel->show();
	PhantomSetting_Dummy_QComboBox->show();

	PhantomSetting_Gender_QHBoxLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);
	PhantomSetting_Gender_QHBoxLayout->setSpacing(Window_width * 0.003);
	PhantomSetting_Gender_QLabel->show();
	PhantomSetting_Gender_QComboBox->show();

	PhantomSetting_Age_QHBoxLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);
	PhantomSetting_Age_QHBoxLayout->setSpacing(Window_width * 0.003);
	PhantomSetting_Age_QLabel->show();
	PhantomSetting_Age_QComboBox->show();

	PhantomSetting_Posture_QHBoxLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);
	PhantomSetting_Posture_QHBoxLayout->setSpacing(Window_width * 0.003);
	PhantomSetting_Posture_QLabel->show();
	PhantomSetting_Posture_QComboBox->show();

	PhantomSetting_Height_QHBoxLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);
	PhantomSetting_Height_QHBoxLayout->setSpacing(Window_width * 0.003);
	PhantomSetting_Height_QLabel->show();
	PhantomSetting_Height_QLineEdit->show();

	PhantomSetting_Weight_QHBoxLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);
	PhantomSetting_Weight_QHBoxLayout->setSpacing(Window_width * 0.003);
	PhantomSetting_Weight_QLabel->show();
	PhantomSetting_Weight_QLineEdit->show();

	PhantomSetting_Rot_QHBoxLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);
	PhantomSetting_Rot_QHBoxLayout->setSpacing(Window_width * 0.003);
	PhantomSetting_Rotation_QLabel->show();
	PhantomSetting_RotX_QLineEdit->show();
	PhantomSetting_RotY_QLineEdit->show();
	PhantomSetting_RotZ_QLineEdit->show();

	// Phantom setting 창크기 조절
	PhantomSetting_UserQDialog->setFixedWidth(Window_width * 0.3);
	PhantomSetting_UserQDialog->setFixedHeight(Window_height * 0.7);
}
void FunctionPanelRight::slot_PhantomSetting_AirPhantom_RadioButtonClicked()
{
	PhantomSetting_ComputationlPhantom_QRadioButton->setChecked(false);
	PhantomSetting_AirPhantom_QRadioButton->setChecked(true);

	// Label, buttons show	
	PhantomSetting_Name_QLineEdit->setPlaceholderText("AirSphere_" + QString::number(m_Phantom_MakingIndex));

	PhantomSetting_Type_QHBoxLayout->setContentsMargins(0, 0, 0, 0);
	PhantomSetting_Type_QHBoxLayout->setSpacing(0);
	PhantomSetting_Type_QLabel->hide();
	PhantomSetting_Type_QComboBox->hide();

	PhantomSetting_Dummy_QHBoxLayout->setContentsMargins(0, 0, 0, 0);
	PhantomSetting_Dummy_QHBoxLayout->setSpacing(0);
	PhantomSetting_Dummy_QLabel->hide();
	PhantomSetting_Dummy_QComboBox->hide();

	PhantomSetting_Gender_QHBoxLayout->setContentsMargins(0, 0, 0, 0);
	PhantomSetting_Gender_QHBoxLayout->setSpacing(0);
	PhantomSetting_Gender_QLabel->hide();
	PhantomSetting_Gender_QComboBox->hide();

	PhantomSetting_Age_QHBoxLayout->setContentsMargins(0, 0, 0, 0);
	PhantomSetting_Age_QHBoxLayout->setSpacing(0);
	PhantomSetting_Age_QLabel->hide();
	PhantomSetting_Age_QComboBox->hide();

	PhantomSetting_Posture_QHBoxLayout->setContentsMargins(0, 0, 0, 0);
	PhantomSetting_Posture_QHBoxLayout->setSpacing(0);
	PhantomSetting_Posture_QLabel->hide();
	PhantomSetting_Posture_QComboBox->hide();

	PhantomSetting_FetalAge_QHBoxLayout->setContentsMargins(0, 0, 0, 0);
	PhantomSetting_FetalAge_QHBoxLayout->setSpacing(0);
	PhantomSetting_FetalAge_QLabel->hide();
	PhantomSetting_FetalAge_QComboBox->hide();

	PhantomSetting_Height_QHBoxLayout->setContentsMargins(0, 0, 0, 0);
	PhantomSetting_Height_QHBoxLayout->setSpacing(0);
	PhantomSetting_Height_QLabel->hide();
	PhantomSetting_Height_QLineEdit->hide();

	PhantomSetting_Weight_QHBoxLayout->setContentsMargins(0, 0, 0, 0);
	PhantomSetting_Weight_QHBoxLayout->setSpacing(0);
	PhantomSetting_Weight_QLabel->hide();
	PhantomSetting_Weight_QLineEdit->hide();

	PhantomSetting_Rot_QHBoxLayout->setContentsMargins(0, 0, 0, 0);
	PhantomSetting_Rot_QHBoxLayout->setSpacing(0);
	PhantomSetting_Rotation_QLabel->hide();
	PhantomSetting_RotX_QLineEdit->hide();
	PhantomSetting_RotY_QLineEdit->hide();
	PhantomSetting_RotZ_QLineEdit->hide();

	// Phantom setting 창크기 조절
	PhantomSetting_UserQDialog->setFixedWidth(Window_width * 0.3);
	PhantomSetting_UserQDialog->setFixedHeight(Window_height * 0.250);
}
void FunctionPanelRight::slot_PhantomSetting_Import_ButtonClicked()
{
	// 필터 문자열은 여러 파일 형식을 선택할 수 있도록 유지합니다.
	QString filter = tr("Model Files (*.obj *.ele *.node);;"
		"Polygon Files (*.obj);;"
		"Element Files (*.ele);;"
		"Node Files (*.node);;"
		"All Files (*)");

	QString selectedFile = QFileDialog::getOpenFileName(
		this,
		tr("Select a model file to open"), // 사용자 안내 메시지 명확화
		"",
		filter
	);

	// 사용자가 파일을 선택하지 않고 취소한 경우 함수를 종료합니다.
	if (selectedFile.isEmpty())
	{
		return;
	}

	// 1. 선택된 파일의 경로 정보 분석
	QFileInfo fileInfo(selectedFile);
	QString path = fileInfo.path();             // 파일이 위치한 디렉토리 경로
	QString baseName = fileInfo.baseName();     // 확장자를 제외한 파일 이름

	// 2. 동일한 경로에 필요한 모든 파일이 있는지 확인
	QStringList requiredExtensions = { ".obj", ".ele", ".node" };
	QStringList missingFiles;
	bool allFilesFound = true;

	for (const QString& ext : requiredExtensions)
	{
		QString filePathToCheck = path + "/" + baseName + ext;
		QFileInfo checkInfo(filePathToCheck);

		// 파일이 존재하지 않거나 읽을 수 없는 경우
		if (!checkInfo.exists() || !checkInfo.isReadable())
		{
			allFilesFound = false;
			// 오류 메시지에 표시할 파일 이름(경로 제외)을 리스트에 추가
			missingFiles.append(baseName + ext);
		}
	}

	// 3. 확인 결과에 따라 처리
	if (allFilesFound)
	{
		// 모든 파일이 존재하면, QLineEdit에 확장자를 제외한 경로와 파일 이름을 설정합니다.
		QString finalPath = path + "/" + baseName;
		PhantomSetting_ImportedFileName_QLineEdit->setText(finalPath);
	}
	else
	{
		// If one or more files are missing, create the error message string
		QString missingFilesString = missingFiles.join("\n");
		QString errorMessage = tr("One or more required files are missing or cannot be read.\n"
			"Please ensure all of the following files are in the same directory:\n\n"
			"%1.obj\n%1.ele\n%1.node\n\n"
			"Missing file(s):\n%2")
			.arg(baseName)
			.arg(missingFilesString);

		// Call your custom messagebox method with the prepared string
		theApp.SetMessageBox(errorMessage);
	}
}
void FunctionPanelRight::slot_ChangePhantomTypeCombo_Setting(int idx)
{
	PhantomSetting_Type_QComboBox->hide();	PhantomSetting_Type_QComboBox->show(); // This is for focusing (Do not show white blank)
	PhantomSetting_Dummy_QComboBox->hide();	PhantomSetting_Dummy_QComboBox->show(); // This is for focusing (Do not show white blank)
	PhantomSetting_Gender_QComboBox->hide(); PhantomSetting_Gender_QComboBox->show(); // This is for focusing (Do not show white blank)
	PhantomSetting_Age_QComboBox->hide();	PhantomSetting_Age_QComboBox->show(); // This is for focusing (Do not show white blank)
	PhantomSetting_Posture_QComboBox->hide();	PhantomSetting_Posture_QComboBox->show(); // This is for focusing (Do not show white blank)
	PhantomSetting_FetalAge_QComboBox->hide();	PhantomSetting_FetalAge_QComboBox->show(); // This is for focusing (Do not show white blank)

	if (PhantomSetting_Type_QComboBox->currentIndex() == E_PHANTOMTYPE_ADULTMRCP || PhantomSetting_Type_QComboBox->currentIndex() == E_PHANTOMTYPE_PEDIATRICMRCP) // MRCP
	{
		PhantomSetting_ImportedFileName_QLineEdit->hide();
		PhantomSetting_PhantomImport_QPushButton->hide();
		PhantomSetting_Dummy_QComboBox->setCurrentIndex(1);
		PhantomSetting_Dummy_QComboBox->setEnabled(true);
		PhantomSetting_Posture_QComboBox->setCurrentIndex(0);
		PhantomSetting_Posture_QComboBox->setDisabled(true);
		PhantomSetting_Gender_QComboBox->setCurrentIndex(0);
		PhantomSetting_Gender_QComboBox->setEnabled(true);
		PhantomSetting_Age_QComboBox->setEnabled(true);
		PhantomSetting_FetalAge_QComboBox->setCurrentIndex(0);
		PhantomSetting_FetalAge_QComboBox->setDisabled(true);
		PhantomSetting_Height_QLineEdit->clear(); PhantomSetting_Height_QLineEdit->setDisabled(true);
		PhantomSetting_Weight_QLineEdit->clear(); PhantomSetting_Weight_QLineEdit->setDisabled(true);
	}

	if (PhantomSetting_Type_QComboBox->currentIndex() == E_PHANTOMTYPE_PFMRCP) // pfMRCP
	{
		PhantomSetting_ImportedFileName_QLineEdit->hide();
		PhantomSetting_PhantomImport_QPushButton->hide();
		PhantomSetting_Dummy_QComboBox->setCurrentIndex(1);
		PhantomSetting_Dummy_QComboBox->setDisabled(true);
		PhantomSetting_Posture_QComboBox->setCurrentIndex(0);
		PhantomSetting_Posture_QComboBox->setDisabled(true);
		PhantomSetting_Gender_QComboBox->setCurrentIndex(1);
		PhantomSetting_Gender_QComboBox->setDisabled(true);
		PhantomSetting_Age_QComboBox->setCurrentIndex(0);
		PhantomSetting_Age_QComboBox->setDisabled(true);
		PhantomSetting_FetalAge_QComboBox->setCurrentIndex(0);
		PhantomSetting_FetalAge_QComboBox->setEnabled(true);
		PhantomSetting_Height_QLineEdit->clear(); PhantomSetting_Height_QLineEdit->setDisabled(true);
		PhantomSetting_Weight_QLineEdit->clear(); PhantomSetting_Weight_QLineEdit->setDisabled(true);
	}

	if (PhantomSetting_Type_QComboBox->currentIndex() == E_PHANTOMTYPE_TRANSFORMED) // Deformed-phantom
	{
		PhantomSetting_ImportedFileName_QLineEdit->hide();
		PhantomSetting_PhantomImport_QPushButton->hide();
		PhantomSetting_Dummy_QComboBox->setCurrentIndex(1);
		PhantomSetting_Dummy_QComboBox->setEnabled(true);
		PhantomSetting_Posture_QComboBox->setEnabled(true); // This is for focusing (Do not show white blank)
		PhantomSetting_Gender_QComboBox->setCurrentIndex(0);
		PhantomSetting_Gender_QComboBox->setEnabled(true);
		PhantomSetting_Age_QComboBox->setEnabled(true);
		PhantomSetting_FetalAge_QComboBox->setCurrentIndex(0);
		PhantomSetting_FetalAge_QComboBox->setDisabled(true);
		PhantomSetting_Height_QLineEdit->setEnabled(true);
		PhantomSetting_Weight_QLineEdit->setEnabled(true);
	}

	if (PhantomSetting_Type_QComboBox->currentIndex() == E_PHANTOMTYPE_IMPORTED) // Imported
	{
		PhantomSetting_ImportedFileName_QLineEdit->show();
		PhantomSetting_PhantomImport_QPushButton->show();
		PhantomSetting_Dummy_QComboBox->setCurrentIndex(1);
		PhantomSetting_Dummy_QComboBox->setDisabled(true);
		PhantomSetting_Posture_QComboBox->setCurrentIndex(0);
		PhantomSetting_Posture_QComboBox->setDisabled(true);
		PhantomSetting_Gender_QComboBox->setCurrentIndex(0);
		PhantomSetting_Gender_QComboBox->setEnabled(true);
		PhantomSetting_Age_QComboBox->setEnabled(true);
		PhantomSetting_FetalAge_QComboBox->setCurrentIndex(0);
		PhantomSetting_FetalAge_QComboBox->setDisabled(true);
		PhantomSetting_Height_QLineEdit->clear(); PhantomSetting_Height_QLineEdit->setDisabled(true);
		PhantomSetting_Weight_QLineEdit->clear(); PhantomSetting_Weight_QLineEdit->setDisabled(true);
	}
}
void FunctionPanelRight::slot_ChangePhantomDummyCombo_Setting(int idx)
{
	PhantomSetting_Type_QComboBox->hide();	PhantomSetting_Type_QComboBox->show(); // This is for focusing (Do not show white blank)
	PhantomSetting_Dummy_QComboBox->hide();	PhantomSetting_Dummy_QComboBox->show(); // This is for focusing (Do not show white blank)
	PhantomSetting_Gender_QComboBox->hide(); PhantomSetting_Gender_QComboBox->show(); // This is for focusing (Do not show white blank)
	PhantomSetting_Age_QComboBox->hide();	PhantomSetting_Age_QComboBox->show(); // This is for focusing (Do not show white blank)
	PhantomSetting_Posture_QComboBox->hide();	PhantomSetting_Posture_QComboBox->show(); // This is for focusing (Do not show white blank)
	PhantomSetting_FetalAge_QComboBox->hide();	PhantomSetting_FetalAge_QComboBox->show(); // This is for focusing (Do not show white blank)
}
void FunctionPanelRight::slot_ChangePhantomGenderCombo_Setting(int idx)
{
	PhantomSetting_Type_QComboBox->hide();	PhantomSetting_Type_QComboBox->show(); // This is for focusing (Do not show white blank)
	PhantomSetting_Dummy_QComboBox->hide();	PhantomSetting_Dummy_QComboBox->show(); // This is for focusing (Do not show white blank)
	PhantomSetting_Gender_QComboBox->hide(); PhantomSetting_Gender_QComboBox->show(); // This is for focusing (Do not show white blank)
	PhantomSetting_Age_QComboBox->hide();	PhantomSetting_Age_QComboBox->show(); // This is for focusing (Do not show white blank)
	PhantomSetting_Posture_QComboBox->hide();	PhantomSetting_Posture_QComboBox->show(); // This is for focusing (Do not show white blank)
	PhantomSetting_FetalAge_QComboBox->hide();	PhantomSetting_FetalAge_QComboBox->show(); // This is for focusing (Do not show white blank)
}
void FunctionPanelRight::slot_ChangePhantomAgeCombo_Setting(int idx)
{
	PhantomSetting_Type_QComboBox->hide();	PhantomSetting_Type_QComboBox->show(); // This is for focusing (Do not show white blank)
	PhantomSetting_Dummy_QComboBox->hide();	PhantomSetting_Dummy_QComboBox->show(); // This is for focusing (Do not show white blank)
	PhantomSetting_Gender_QComboBox->hide(); PhantomSetting_Gender_QComboBox->show(); // This is for focusing (Do not show white blank)
	PhantomSetting_Age_QComboBox->hide();	PhantomSetting_Age_QComboBox->show(); // This is for focusing (Do not show white blank)
	PhantomSetting_Posture_QComboBox->hide();	PhantomSetting_Posture_QComboBox->show(); // This is for focusing (Do not show white blank)
	PhantomSetting_FetalAge_QComboBox->hide();	PhantomSetting_FetalAge_QComboBox->show(); // This is for focusing (Do not show white blank)

	if (idx == 0) //성인은 자세변형 가능
	{
		PhantomSetting_Posture_QComboBox->setEnabled(true);
	}
	else //소아는 자세변형 불가능
	{
		PhantomSetting_Posture_QComboBox->setCurrentIndex(0);
		PhantomSetting_Posture_QComboBox->setEnabled(false);		
	}
}
void FunctionPanelRight::slot_ChangePhantomPostureCombo_Setting(int idx)
{
	PhantomSetting_Type_QComboBox->hide();	PhantomSetting_Type_QComboBox->show(); // This is for focusing (Do not show white blank)
	PhantomSetting_Dummy_QComboBox->hide();	PhantomSetting_Dummy_QComboBox->show(); // This is for focusing (Do not show white blank)
	PhantomSetting_Gender_QComboBox->hide(); PhantomSetting_Gender_QComboBox->show(); // This is for focusing (Do not show white blank)
	PhantomSetting_Age_QComboBox->hide();	PhantomSetting_Age_QComboBox->show(); // This is for focusing (Do not show white blank)
	PhantomSetting_Posture_QComboBox->hide();	PhantomSetting_Posture_QComboBox->show(); // This is for focusing (Do not show white blank)
	PhantomSetting_FetalAge_QComboBox->hide();	PhantomSetting_FetalAge_QComboBox->show(); // This is for focusing (Do not show white blank)

	if (idx == 0) // Standing
	{
		PhantomSetting_Height_QLineEdit->setEnabled(true);
		PhantomSetting_Weight_QLineEdit->setEnabled(true);
	}
	else
	{
		PhantomSetting_Height_QLineEdit->clear(); PhantomSetting_Height_QLineEdit->setDisabled(true);
		PhantomSetting_Weight_QLineEdit->clear(); PhantomSetting_Weight_QLineEdit->setDisabled(true);
	}
}
void FunctionPanelRight::slot_ChangePhantomFetalAgeCombo_Setting(int idx)
{
	PhantomSetting_Type_QComboBox->hide();	PhantomSetting_Type_QComboBox->show(); // This is for focusing (Do not show white blank)
	PhantomSetting_Dummy_QComboBox->hide();	PhantomSetting_Dummy_QComboBox->show(); // This is for focusing (Do not show white blank)
	PhantomSetting_Gender_QComboBox->hide(); PhantomSetting_Gender_QComboBox->show(); // This is for focusing (Do not show white blank)
	PhantomSetting_Age_QComboBox->hide();	PhantomSetting_Age_QComboBox->show(); // This is for focusing (Do not show white blank)
	PhantomSetting_Posture_QComboBox->hide();	PhantomSetting_Posture_QComboBox->show(); // This is for focusing (Do not show white blank)
	PhantomSetting_FetalAge_QComboBox->hide();	PhantomSetting_FetalAge_QComboBox->show(); // This is for focusing (Do not show white blank)
}

// Main function - Info Status
void FunctionPanelRight::slot_DummyPhantomCheckBoxClicked()
{
	int id = m_Phantom_SelectedIndex;

	// 현재 선택 index가 invalid면 false로 돌리고 return
	if (id < 0)
	{
		PhantomDummy_QCheckBox->setChecked(false);
		return;
	}
	else if (m_Phantom_MainInfo[id][E_PHANTOMMAININFO_CATEGORY] == E_PHANTOMCATEGORY_AIR
		|| m_Phantom_MainInfo[id][E_PHANTOMMAININFO_TYPE] == E_PHANTOMTYPE_IMPORTED) // 에어팬텀이거나 imported 팬텀이면 false로 돌리고 return
	{
		PhantomDummy_QCheckBox->setChecked(false);
		return;
	}

	if (PhantomDummy_QCheckBox->isChecked() == true)
	{
		m_Phantom_MainInfo[id][E_PHANTOMMAININFO_DUMMY] = E_PHANTOMDUMMY_YES;

		theApp.FullPhantomActor[id] = theApp.PhantomPanelActor[id]; // FullphantomActor에 기존 팬텀(의복 등 포함) 저장		
		theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(theApp.PhantomPanelActor[id]);
		theApp.PhantomPanelActor[id] = theApp.DummyPhantomActor[id];
		// Translate actor
		theApp.PhantomPanelActor[id]->SetPosition(PhantomPosX_QLineEdit->text().toDouble(), PhantomPosY_QLineEdit->text().toDouble(), PhantomPosZ_QLineEdit->text().toDouble());
		theApp.PhantomPanelActor[id]->SetOrientation(PhantomRotX_QLineEdit->text().toDouble(), PhantomRotY_QLineEdit->text().toDouble(), PhantomRotZ_QLineEdit->text().toDouble());
		theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(theApp.PhantomPanelActor[id]);
		theApp.m_pVTKWidget->renderWindow()->Render();

		// 추후 업데이트: 의복류 추가 못하게
	}
	else
	{			
		m_Phantom_MainInfo[id][E_PHANTOMMAININFO_DUMMY] = E_PHANTOMDUMMY_NO;

		theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(theApp.PhantomPanelActor[id]);
		theApp.PhantomPanelActor[id] = theApp.FullPhantomActor[id];
		// Translate actor
		theApp.PhantomPanelActor[id]->SetPosition(PhantomPosX_QLineEdit->text().toDouble(), PhantomPosY_QLineEdit->text().toDouble(), PhantomPosZ_QLineEdit->text().toDouble());
		theApp.PhantomPanelActor[id]->SetOrientation(PhantomRotX_QLineEdit->text().toDouble(), PhantomRotY_QLineEdit->text().toDouble(), PhantomRotZ_QLineEdit->text().toDouble());
		theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(theApp.PhantomPanelActor[id]);
		theApp.m_pVTKWidget->renderWindow()->Render();
	}
}
void FunctionPanelRight::slot_PhantomUpdate_ButtonClicked()
{
	if (m_Phantom_SequenceVector.size() == 0)
	{
		theApp.SetMessageBox("Phantom should be added first.");
		return;
	}
	// Phantom Pos/Rot update
	SavePhantom_InfoData(m_Phantom_SelectedIndex);

	theApp.PhantomPanelActor[m_Phantom_SelectedIndex]->SetPosition(m_Phantom_MainInfo[m_Phantom_SelectedIndex][6], m_Phantom_MainInfo[m_Phantom_SelectedIndex][7], m_Phantom_MainInfo[m_Phantom_SelectedIndex][8]);
	theApp.PhantomPanelActor[m_Phantom_SelectedIndex]->SetOrientation(m_Phantom_MainInfo[m_Phantom_SelectedIndex][9], m_Phantom_MainInfo[m_Phantom_SelectedIndex][10], m_Phantom_MainInfo[m_Phantom_SelectedIndex][11]);
	
	theApp.m_pVTKWidget->renderWindow()->Render();
}

// Sub-function
void FunctionPanelRight::moveMaximumScrollbar_phantom() // called by (1) slot_PhantomAdd_ButtonClicked
{
	timer_scroll_multiplePhantom->stop();
	PhantomList_Horizontal_QScrollBar->setValue(PhantomList_Horizontal_QScrollBar->maximum());
	PhantomList_Vertical_QScrollBar->setValue(0);
}
void FunctionPanelRight::SavePhantom_InfoData_InAddingSetting(int phantomIndex)
{
	// Phantom category
	if (PhantomSetting_AirPhantom_QRadioButton->isChecked() == false) m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_CATEGORY] = E_PHANTOMCATEGORY_HUMAN;
	else if (PhantomSetting_AirPhantom_QRadioButton->isChecked() == true)
	{
		if (PhantomSetting_Name_QLineEdit->text().isEmpty()) m_Phantom_ButtonName[phantomIndex] = "AirSphere_" + QString::number(phantomIndex);
		else m_Phantom_ButtonName[phantomIndex] = PhantomSetting_Name_QLineEdit->text();

		m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_CATEGORY] = E_PHANTOMCATEGORY_AIR;
		m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_POS_X] = PhantomSetting_PosX_QLineEdit->text().toDouble(); // posX
		m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_POS_X] = PhantomSetting_PosY_QLineEdit->text().toDouble(); // posY
		m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_POS_X] = PhantomSetting_PosZ_QLineEdit->text().toDouble(); // posZ
		m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_DUMMY] = E_PHANTOMDUMMY_NO;
	}

	// Phantom name
	if (PhantomSetting_Name_QLineEdit->text().isEmpty()) m_Phantom_ButtonName[phantomIndex] = "Phantom_" + QString::number(phantomIndex);
	else m_Phantom_ButtonName[phantomIndex] = PhantomSetting_Name_QLineEdit->text();
	
	m_Phantom_MainInfo[phantomIndex][1] = PhantomSetting_Gender_QComboBox->currentIndex(); // Gender
	m_Phantom_MainInfo[phantomIndex][2] = PhantomSetting_Type_QComboBox->currentIndex(); // Type	
	if (PhantomSetting_Type_QComboBox->currentIndex() == E_PHANTOMTYPE_IMPORTED) theApp.m_ImportedPhantomFilePath_NoExtention[phantomIndex] = PhantomSetting_ImportedFileName_QLineEdit->text(); // Imported phantom file name
	m_Phantom_MainInfo[phantomIndex][3] = PhantomSetting_Posture_QComboBox->currentIndex(); // Posture

	// Height/Weight
	if (PhantomSetting_Type_QComboBox->currentIndex() == E_PHANTOMTYPE_ADULTMRCP) // Adult MRCP or Adult Posture-dependent-phantom
	{
		if (PhantomSetting_Gender_QComboBox->currentIndex() == E_PHANTOMGENDER_MALE) { m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_HEIGHT] = 176; m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_WEIGHT] = 73; } // adult male
		if (PhantomSetting_Gender_QComboBox->currentIndex() == E_PHANTOMGENDER_FEMALE) { m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_HEIGHT] = 163; m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_WEIGHT] = 60; } // adult female
	}
	else if (PhantomSetting_Type_QComboBox->currentIndex() == E_PHANTOMTYPE_PEDIATRICMRCP) // Pediatric MRCP
	{
		if (PhantomSetting_Gender_QComboBox->currentIndex() == E_PHANTOMGENDER_MALE && PhantomSetting_Age_QComboBox->currentIndex() == E_PHANTOMAGE_15y) { m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_HEIGHT] = 167; m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_WEIGHT] = 56; } // 15y male
		if (PhantomSetting_Gender_QComboBox->currentIndex() == E_PHANTOMGENDER_MALE && PhantomSetting_Age_QComboBox->currentIndex() == E_PHANTOMAGE_10y) { m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_HEIGHT] = 138; m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_WEIGHT] = 32; } // 10y male
		if (PhantomSetting_Gender_QComboBox->currentIndex() == E_PHANTOMGENDER_MALE && PhantomSetting_Age_QComboBox->currentIndex() == E_PHANTOMAGE_5y) { m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_HEIGHT] = 109; m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_WEIGHT] = 19; } // 05y male
		if (PhantomSetting_Gender_QComboBox->currentIndex() == E_PHANTOMGENDER_MALE && PhantomSetting_Age_QComboBox->currentIndex() == E_PHANTOMAGE_1y) { m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_HEIGHT] = 76; m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_WEIGHT] = 10; } // 01y male
		if (PhantomSetting_Gender_QComboBox->currentIndex() == E_PHANTOMGENDER_MALE && PhantomSetting_Age_QComboBox->currentIndex() == E_PHANTOMAGE_0y) { m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_HEIGHT] = 51; m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_WEIGHT] = 3.5; } // 00y male

		if (PhantomSetting_Gender_QComboBox->currentIndex() == E_PHANTOMGENDER_FEMALE && PhantomSetting_Age_QComboBox->currentIndex() == E_PHANTOMAGE_15y) { m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_HEIGHT] = 161; m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_WEIGHT] = 53; } // 15y female
		if (PhantomSetting_Gender_QComboBox->currentIndex() == E_PHANTOMGENDER_FEMALE && PhantomSetting_Age_QComboBox->currentIndex() == E_PHANTOMAGE_10y) { m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_HEIGHT] = 138; m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_WEIGHT] = 32; } // 10y female
		if (PhantomSetting_Gender_QComboBox->currentIndex() == E_PHANTOMGENDER_FEMALE && PhantomSetting_Age_QComboBox->currentIndex() == E_PHANTOMAGE_5y)  { m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_HEIGHT] = 109; m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_WEIGHT] = 19; } // 05y female
		if (PhantomSetting_Gender_QComboBox->currentIndex() == E_PHANTOMGENDER_FEMALE && PhantomSetting_Age_QComboBox->currentIndex() == E_PHANTOMAGE_1y)  { m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_HEIGHT] = 76; m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_WEIGHT] = 10; } // 01y female
		if (PhantomSetting_Gender_QComboBox->currentIndex() == E_PHANTOMGENDER_FEMALE && PhantomSetting_Age_QComboBox->currentIndex() == E_PHANTOMAGE_0y)  { m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_HEIGHT] = 51; m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_WEIGHT] = 3.5; } // 00y female
	}
	else if (PhantomSetting_Type_QComboBox->currentIndex() == E_PHANTOMTYPE_PFMRCP) // PFMRCP
	{
		m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_HEIGHT] = 163; m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_WEIGHT] = 60;
	}
	else if (PhantomSetting_Type_QComboBox->currentIndex() == E_PHANTOMTYPE_TRANSFORMED && PhantomSetting_Posture_QComboBox->currentIndex() != E_PHANTOMPOSTURE_STANDING) // Adult posture-dependent-phantom
	{
		if (PhantomSetting_Gender_QComboBox->currentIndex() == E_PHANTOMGENDER_MALE && PhantomSetting_Age_QComboBox->currentIndex() == E_PHANTOMAGE_ADULT) { m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_HEIGHT] = 176; m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_WEIGHT] = 73; } // adult male
		if (PhantomSetting_Gender_QComboBox->currentIndex() == E_PHANTOMGENDER_FEMALE && PhantomSetting_Age_QComboBox->currentIndex() == E_PHANTOMAGE_ADULT) { m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_HEIGHT] = 163; m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_WEIGHT] = 60; } // adult female
	}
	else if (PhantomSetting_Type_QComboBox->currentIndex() == E_PHANTOMTYPE_TRANSFORMED && PhantomSetting_Posture_QComboBox->currentIndex() == E_PHANTOMPOSTURE_STANDING) //Body-size-dependent phantom
	{
		m_Phantom_MainInfo[phantomIndex][4] = PhantomSetting_Height_QLineEdit->text().toDouble(); // Height
		m_Phantom_MainInfo[phantomIndex][5] = PhantomSetting_Weight_QLineEdit->text().toDouble(); // Weight
	}
	if (PhantomSetting_Type_QComboBox->currentIndex() == E_PHANTOMTYPE_IMPORTED) //Imported phantom
	{
		m_Phantom_MainInfo[phantomIndex][4] = -1; // Invalid Height
		m_Phantom_MainInfo[phantomIndex][5] = -1; // Invalid Weight
	}

	m_Phantom_MainInfo[phantomIndex][6] = PhantomSetting_PosX_QLineEdit->text().toDouble(); // posX
	m_Phantom_MainInfo[phantomIndex][7] = PhantomSetting_PosY_QLineEdit->text().toDouble(); // posY
	m_Phantom_MainInfo[phantomIndex][8] = PhantomSetting_PosZ_QLineEdit->text().toDouble(); // posZ
	m_Phantom_MainInfo[phantomIndex][9] = PhantomSetting_RotX_QLineEdit->text().toDouble(); // rotX
	m_Phantom_MainInfo[phantomIndex][10] = PhantomSetting_RotY_QLineEdit->text().toDouble(); // rotY
	m_Phantom_MainInfo[phantomIndex][11] = PhantomSetting_RotZ_QLineEdit->text().toDouble(); // rotZ

	m_Phantom_MainInfo[phantomIndex][12] = PhantomSetting_Age_QComboBox->currentIndex(); // Age
	m_Phantom_MainInfo[phantomIndex][14] = PhantomSetting_FetalAge_QComboBox->currentIndex(); // Fetal Age

	// Dummy phantom
	if (PhantomSetting_Dummy_QComboBox->currentIndex() == 0) m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_DUMMY] = E_PHANTOMDUMMY_YES;
	if (PhantomSetting_Dummy_QComboBox->currentIndex() == 1) m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_DUMMY] = E_PHANTOMDUMMY_NO;
}
void FunctionPanelRight::SavePhantom_InfoData(int phantomIndex)
{
	m_Phantom_MainInfo[phantomIndex][6] = PhantomPosX_QLineEdit->text().toDouble(); // posX
	m_Phantom_MainInfo[phantomIndex][7] = PhantomPosY_QLineEdit->text().toDouble(); // posY
	m_Phantom_MainInfo[phantomIndex][8] = PhantomPosZ_QLineEdit->text().toDouble(); // posZ
	m_Phantom_MainInfo[phantomIndex][9] = PhantomRotX_QLineEdit->text().toDouble(); // rotX
	m_Phantom_MainInfo[phantomIndex][10] = PhantomRotY_QLineEdit->text().toDouble(); // rotY
	m_Phantom_MainInfo[phantomIndex][11] = PhantomRotZ_QLineEdit->text().toDouble(); // rotZ
}
void FunctionPanelRight::SavePhantom_InfoData_InMouseControl(int phantomIndex)
{
	double* pCenter = theApp.PhantomPanelActor[phantomIndex]->GetPosition();
	double TransformPos[3];
	TransformPos[0] = pCenter[0];
	TransformPos[1] = pCenter[1];
	TransformPos[2] = pCenter[2];

	double* pRotation = theApp.PhantomPanelActor[phantomIndex]->GetOrientation();
	double TransformRot[3];
	TransformRot[0] = pRotation[0];
	TransformRot[1] = pRotation[1];
	TransformRot[2] = pRotation[2];

	m_Phantom_MainInfo[phantomIndex][6] = TransformPos[0];
	m_Phantom_MainInfo[phantomIndex][7] = TransformPos[1];
	m_Phantom_MainInfo[phantomIndex][8] = TransformPos[2];
	m_Phantom_MainInfo[phantomIndex][9] = TransformRot[0];
	m_Phantom_MainInfo[phantomIndex][10] = TransformRot[1];
	m_Phantom_MainInfo[phantomIndex][11] = TransformRot[2];
}
void FunctionPanelRight::UpdatePhantom_ListButton(int phantomIndex)
{
	for (auto itr_phantom : m_Phantom_SequenceVector) // 최대 팬텀 개수
	{
		if (itr_phantom != m_Phantom_SelectedIndex)
		{
			PhantomSelect_QPushButton[itr_phantom]->setChecked(false);
		}
	}
	if (phantomIndex >= 0) PhantomSelect_QPushButton[phantomIndex]->setChecked(true);
}
void FunctionPanelRight::UpdatePhantom_InfoStatus(int phantomIndex)
{
	if (phantomIndex < 0) // 현재 팬텀 index가 invalid 일때 초기화
	{
		PhantomType_QLineEdit->clear();
		PhantomGender_QLineEdit->clear();
		PhantomAge_QLineEdit->clear();
		PhantomPosture_QLineEdit->clear();
		PhantomFetalAge_QLineEdit->clear();
		PhantomHeight_QLineEdit->clear();
		PhantomWeight_QLineEdit->clear();
		PhantomPosX_QLineEdit->clear();
		PhantomPosY_QLineEdit->clear();
		PhantomPosZ_QLineEdit->clear();
		PhantomRotX_QLineEdit->clear();
		PhantomRotY_QLineEdit->clear();
		PhantomRotZ_QLineEdit->clear();

		return;
	}

	if (m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_CATEGORY] == E_PHANTOMCATEGORY_AIR) // Air phantom 일 때
	{
		PhantomGender_QLineEdit->clear();
		PhantomType_QLineEdit->clear();
		PhantomPosture_QLineEdit->clear();
		PhantomAge_QLineEdit->clear();
		PhantomFetalAge_QLineEdit->clear();
		PhantomHeight_QLineEdit->clear();
		PhantomWeight_QLineEdit->clear();

		// Pos, Rot
		PhantomPosX_QLineEdit->setText(QString::number(m_Phantom_MainInfo[phantomIndex][6]));
		PhantomPosY_QLineEdit->setText(QString::number(m_Phantom_MainInfo[phantomIndex][7]));
		PhantomPosZ_QLineEdit->setText(QString::number(m_Phantom_MainInfo[phantomIndex][8]));
		PhantomRotX_QLineEdit->setText(QString::number(m_Phantom_MainInfo[phantomIndex][9]));
		PhantomRotY_QLineEdit->setText(QString::number(m_Phantom_MainInfo[phantomIndex][10]));
		PhantomRotZ_QLineEdit->setText(QString::number(m_Phantom_MainInfo[phantomIndex][11]));

		return;
	}

	// Gender
	if (m_Phantom_MainInfo[phantomIndex][1] == 0) { PhantomGender_QLineEdit->setText("Male"); }
	if (m_Phantom_MainInfo[phantomIndex][1] == 1) { PhantomGender_QLineEdit->setText("Female"); }

	// Phantom Type
	if (m_Phantom_MainInfo[phantomIndex][2] == E_PHANTOMTYPE_ADULTMRCP) { PhantomType_QLineEdit->setText("Adult MRCP"); }
	if (m_Phantom_MainInfo[phantomIndex][2] == E_PHANTOMTYPE_PEDIATRICMRCP) { PhantomType_QLineEdit->setText("Pediatric MRCP"); }
	if (m_Phantom_MainInfo[phantomIndex][2] == E_PHANTOMTYPE_PFMRCP) { PhantomType_QLineEdit->setText("pregnant-female MRCP"); }
	if (m_Phantom_MainInfo[phantomIndex][2] == E_PHANTOMTYPE_TRANSFORMED) { PhantomType_QLineEdit->setText("Transformed phantom"); }
	if (m_Phantom_MainInfo[phantomIndex][2] == E_PHANTOMTYPE_IMPORTED) { PhantomType_QLineEdit->setText("Imported phantom"); }

	// Posture
	if (m_Phantom_MainInfo[phantomIndex][3] == 0) { PhantomPosture_QLineEdit->setText("Standing"); }
	if (m_Phantom_MainInfo[phantomIndex][3] == 1) { PhantomPosture_QLineEdit->setText("Bending"); }
	if (m_Phantom_MainInfo[phantomIndex][3] == 2) { PhantomPosture_QLineEdit->setText("Kneeling"); }
	if (m_Phantom_MainInfo[phantomIndex][3] == 3) { PhantomPosture_QLineEdit->setText("Sitting"); }
	if (m_Phantom_MainInfo[phantomIndex][3] == 4) { PhantomPosture_QLineEdit->setText("Squatting"); }
	if (m_Phantom_MainInfo[phantomIndex][3] == 5) { PhantomPosture_QLineEdit->setText("Walking"); }
	if (m_Phantom_MainInfo[phantomIndex][2] == E_PHANTOMTYPE_IMPORTED) { PhantomPosture_QLineEdit->setText(" "); }

	// Height
	PhantomHeight_QLineEdit->setText(QString::number(m_Phantom_MainInfo[phantomIndex][4]));
	if (m_Phantom_MainInfo[phantomIndex][2] == E_PHANTOMTYPE_IMPORTED) PhantomHeight_QLineEdit->setText(" ");

	// Weight
	PhantomWeight_QLineEdit->setText(QString::number(m_Phantom_MainInfo[phantomIndex][5]));
	if (m_Phantom_MainInfo[phantomIndex][2] == E_PHANTOMTYPE_IMPORTED) PhantomWeight_QLineEdit->setText(" ");

	// Pos, Rot
	PhantomPosX_QLineEdit->setText(QString::number(m_Phantom_MainInfo[phantomIndex][6]));
	PhantomPosY_QLineEdit->setText(QString::number(m_Phantom_MainInfo[phantomIndex][7]));
	PhantomPosZ_QLineEdit->setText(QString::number(m_Phantom_MainInfo[phantomIndex][8]));
	PhantomRotX_QLineEdit->setText(QString::number(m_Phantom_MainInfo[phantomIndex][9]));
	PhantomRotY_QLineEdit->setText(QString::number(m_Phantom_MainInfo[phantomIndex][10]));
	PhantomRotZ_QLineEdit->setText(QString::number(m_Phantom_MainInfo[phantomIndex][11]));

	// Age
	if (m_Phantom_MainInfo[phantomIndex][12] == 0) { PhantomAge_QLineEdit->setText("Adult"); }
	if (m_Phantom_MainInfo[phantomIndex][12] == 1) { PhantomAge_QLineEdit->setText("15y"); }
	if (m_Phantom_MainInfo[phantomIndex][12] == 2) { PhantomAge_QLineEdit->setText("10y"); }
	if (m_Phantom_MainInfo[phantomIndex][12] == 3) { PhantomAge_QLineEdit->setText("5y"); }
	if (m_Phantom_MainInfo[phantomIndex][12] == 4) { PhantomAge_QLineEdit->setText("1y"); }
	if (m_Phantom_MainInfo[phantomIndex][12] == 5) { PhantomAge_QLineEdit->setText("0y"); }

	// Fetal Age
	if (m_Phantom_MainInfo[phantomIndex][14] == 0) { PhantomFetalAge_QLineEdit->setText("08w"); }
	if (m_Phantom_MainInfo[phantomIndex][14] == 1) { PhantomFetalAge_QLineEdit->setText("10w"); }
	if (m_Phantom_MainInfo[phantomIndex][14] == 2) { PhantomFetalAge_QLineEdit->setText("15w"); }
	if (m_Phantom_MainInfo[phantomIndex][14] == 3) { PhantomFetalAge_QLineEdit->setText("20w"); }
	if (m_Phantom_MainInfo[phantomIndex][14] == 4) { PhantomFetalAge_QLineEdit->setText("25w"); }
	if (m_Phantom_MainInfo[phantomIndex][14] == 5) { PhantomFetalAge_QLineEdit->setText("30w"); }
	if (m_Phantom_MainInfo[phantomIndex][14] == 6) { PhantomFetalAge_QLineEdit->setText("35w"); }
	if (m_Phantom_MainInfo[phantomIndex][14] == 7) { PhantomFetalAge_QLineEdit->setText("38w"); }

	// Dummy phantom
	if (m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_DUMMY] == E_PHANTOMDUMMY_YES) PhantomDummy_QCheckBox->setChecked(true);
	if (m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_DUMMY] == E_PHANTOMDUMMY_NO) PhantomDummy_QCheckBox->setChecked(false);

}

void FunctionPanelRight::RemoveClothingPanel() // called by (1) slot_PhantomSelect_ButtonClicked, (2) slot_PhantomAdd_ButtonClicked, (3) slot_PhantomDelete_ButtonClicked
{
	// Remove clothing list
	for (auto itr_clothing : m_Clothing_SequenceVector[m_Phantom_SelectedIndex])
	{
		ClothingList_QHBoxLayout->removeWidget(ClothingSelect_QPushButton[itr_clothing]);
		Buttons_FunctionPanelRight.removeOne(ClothingSelect_QPushButton[itr_clothing]);
		delete ClothingSelect_QPushButton[itr_clothing];
		ClothingSelect_QPushButton.erase(itr_clothing);
	}
	// Remove clothing layer list
	for (auto itr_layer : m_ClothingLayer_SequenceVector[m_Phantom_SelectedIndex][m_Clothing_SelectedIndex])
	{
		ClothingLayerList_QHBoxLayout->removeWidget(ClothingLayerSelect_QPushButton[itr_layer]);
		Buttons_FunctionPanelRight.removeOne(ClothingLayerSelect_QPushButton[itr_layer]);
		delete ClothingLayerSelect_QPushButton[itr_layer];
		ClothingLayerSelect_QPushButton.erase(itr_layer);
	}
	// Set panel info
	if (m_Clothing_SelectedIndex >= 0) SetClothingPanelInfo(m_Clothing_SelectedIndex, m_ClothingLayer_SelectedIndex); // Clothing 존재할 때
	else
	{
		ClothingThickness_QLineEdit->setText(""); // thickness
		ClothingDensity_QLineEdit->setText(""); // density		
		ClothingComposition_QComboBox->setCurrentIndex(0);// Composition
	}

	m_Clothing_SelectedIndex = -1;
	m_Clothing_MakingIndex = 0;
	m_ClothingLayer_SelectedIndex = -1;
	m_ClothingLayer_MakingIndex = 0;
}
void FunctionPanelRight::SetClothingPanel_inPhantomDelete() // called by (1) slot_PhantomDelete_ButtonClicked
{
	// Find clothing index
	if (m_Clothing_SequenceVector[m_Phantom_SelectedIndex].size() == 0) m_Clothing_SelectedIndex = -1;
	else m_Clothing_SelectedIndex = m_Clothing_SequenceVector[m_Phantom_SelectedIndex][0];// First index
	m_Clothing_MakingIndex = -1; // Biggest is last index, find the biggest element in SequenceVector
	for (auto itr : m_Clothing_SequenceVector[m_Phantom_SelectedIndex])
	{
		if (itr > m_Clothing_MakingIndex) m_Clothing_MakingIndex = itr;
	}
	m_Clothing_MakingIndex++; // 가장 큰 원소보다 1 더 큰 값

	// Generate clothing list
	for (auto itr : m_Clothing_SequenceVector[m_Phantom_SelectedIndex])
	{
		ClothingSelect_QPushButton[itr] = new QPushButton(m_Clothing_ButtonName[m_Phantom_SelectedIndex][itr]); // 이름 	
		Buttons_FunctionPanelRight.append(ClothingSelect_QPushButton[itr]);
		ClothingSelect_QPushButton[itr]->setObjectName(QString::number(itr));
		ClothingSelect_QPushButton[itr]->setFixedSize(Window_width * 0.05, Window_height * 0.040);
		ClothingSelect_QPushButton[itr]->setStyleSheet(DialogStyle::MENU_BTN7);
		ClothingSelect_QPushButton[itr]->setFont(font_D_BTN14);
		ClothingSelect_QPushButton[itr]->setCheckable(true);
		ClothingSelect_QPushButton[itr]->setChecked(true);
		connect(ClothingSelect_QPushButton[itr], SIGNAL(clicked()), this, SLOT(slot_ClothingSelect_ButtonClicked())); connect(ClothingSelect_QPushButton[itr], SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
		ClothingList_QHBoxLayout->addWidget(ClothingSelect_QPushButton[itr]);
	}
	// Set checked clothing button
	for (auto itr : m_Clothing_SequenceVector[m_Phantom_SelectedIndex])
	{
		if (itr != m_Clothing_SelectedIndex)
		{
			ClothingSelect_QPushButton[itr]->setChecked(false);
		}
		else ClothingSelect_QPushButton[itr]->setChecked(true);
	}

	// Clothing layer

	// Find clothing layer index
	if (m_ClothingLayer_SequenceVector[m_Phantom_SelectedIndex][m_Clothing_SelectedIndex].size() == 0) m_ClothingLayer_SelectedIndex = -1;
	else m_ClothingLayer_SelectedIndex = m_ClothingLayer_SequenceVector[m_Phantom_SelectedIndex][m_Clothing_SelectedIndex][0];// First index
	m_ClothingLayer_MakingIndex = -1; // Biggest is last index, find the biggest element in m_ClothingLayer_SequenceVector
	for (auto itr : m_ClothingLayer_SequenceVector[m_Phantom_SelectedIndex][m_Clothing_SelectedIndex])
	{
		if (itr > m_ClothingLayer_MakingIndex) m_ClothingLayer_MakingIndex = itr;
	}
	m_ClothingLayer_MakingIndex++; // 가장 큰 원소보다 1 더 큰 값


	// Generate clothing layer list
	for (auto itr : m_ClothingLayer_SequenceVector[m_Phantom_SelectedIndex][m_Clothing_SelectedIndex])
	{		
		ClothingLayerSelect_QPushButton[itr] = new QPushButton(m_ClothingLayer_ButtonName[m_Phantom_SelectedIndex][m_Clothing_SelectedIndex][itr]); // 이름 		
		Buttons_FunctionPanelRight.append(ClothingLayerSelect_QPushButton[itr]);
		ClothingLayerSelect_QPushButton[itr]->setObjectName(QString::number(itr));
		ClothingLayerSelect_QPushButton[itr]->setFixedSize(Window_width * 0.04, Window_height * 0.025);
		ClothingLayerSelect_QPushButton[itr]->setStyleSheet(DialogStyle::MENU_BTN7);
		ClothingLayerSelect_QPushButton[itr]->setFont(font_D_BTN14);
		ClothingLayerSelect_QPushButton[itr]->setCheckable(true);
		ClothingLayerSelect_QPushButton[itr]->setChecked(true);
		connect(ClothingLayerSelect_QPushButton[itr], SIGNAL(clicked()), this, SLOT(slot_ClothingLayerSelect_ButtonClicked())); connect(ClothingLayerSelect_QPushButton[itr], SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
		ClothingLayerList_QHBoxLayout->addWidget(ClothingLayerSelect_QPushButton[itr]);
	}
	// Set checked clothing layer button
	for (auto itr : m_ClothingLayer_SequenceVector[m_Phantom_SelectedIndex][m_Clothing_SelectedIndex])
	{
		if (itr != m_ClothingLayer_SelectedIndex)
		{
			ClothingLayerSelect_QPushButton[itr]->setChecked(false);
		}
		else ClothingLayerSelect_QPushButton[itr]->setChecked(true);
	}

	// Set panel info
	if (m_Clothing_SelectedIndex >= 0) SetClothingPanelInfo(m_Clothing_SelectedIndex, m_ClothingLayer_SelectedIndex); // Clothing 존재할 때
	else
	{
		ClothingThickness_QLineEdit->setText(""); // thickness
		ClothingDensity_QLineEdit->setText(""); // density		
		ClothingComposition_QComboBox->setCurrentIndex(0);// Composition
	}
}
void FunctionPanelRight::SetClothingPanel_inPhantomSelect() // called by (1) slot_PhantomSelect_ButtonClicked
{
	// Find clothing index
	if (m_Clothing_SequenceVector[m_Phantom_SelectedIndex].size() == 0) m_Clothing_SelectedIndex = -1;
	else m_Clothing_SelectedIndex = m_Clothing_SequenceVector[m_Phantom_SelectedIndex][0];// First index
	m_Clothing_MakingIndex = -1; // Biggest is last index, find the biggest element in SequenceVector
	for (auto itr : m_Clothing_SequenceVector[m_Phantom_SelectedIndex])
	{
		if (itr > m_Clothing_MakingIndex) m_Clothing_MakingIndex = itr;
	}
	m_Clothing_MakingIndex++; // 가장 큰 원소보다 1 더 큰 값	

	// Generate clothing list
	for (auto itr : m_Clothing_SequenceVector[m_Phantom_SelectedIndex])
	{
		ClothingSelect_QPushButton[itr] = new QPushButton(m_Clothing_ButtonName[m_Phantom_SelectedIndex][itr]); // 이름 	
		Buttons_FunctionPanelRight.append(ClothingSelect_QPushButton[itr]);
		ClothingSelect_QPushButton[itr]->setObjectName(QString::number(itr));
		ClothingSelect_QPushButton[itr]->setFixedSize(Window_width * 0.05, Window_height * 0.040);
		ClothingSelect_QPushButton[itr]->setStyleSheet(DialogStyle::MENU_BTN7);
		ClothingSelect_QPushButton[itr]->setFont(font_D_BTN14);
		ClothingSelect_QPushButton[itr]->setCheckable(true);
		ClothingSelect_QPushButton[itr]->setChecked(true);
		connect(ClothingSelect_QPushButton[itr], SIGNAL(clicked()), this, SLOT(slot_ClothingSelect_ButtonClicked())); connect(ClothingSelect_QPushButton[itr], SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
		ClothingList_QHBoxLayout->addWidget(ClothingSelect_QPushButton[itr]);
	}
	// Set checked clothing button
	for (auto itr : m_Clothing_SequenceVector[m_Phantom_SelectedIndex])
	{
		if (itr != m_Clothing_SelectedIndex)
		{
			ClothingSelect_QPushButton[itr]->setChecked(false);
		}
		else ClothingSelect_QPushButton[itr]->setChecked(true);
	}

	// Clothing layer

	// Find clothing layer index
	if (m_ClothingLayer_SequenceVector[m_Phantom_SelectedIndex][m_Clothing_SelectedIndex].size() == 0) m_ClothingLayer_SelectedIndex = -1;
	else m_ClothingLayer_SelectedIndex = m_ClothingLayer_SequenceVector[m_Phantom_SelectedIndex][m_Clothing_SelectedIndex][0];// First index
	m_ClothingLayer_MakingIndex = -1; // Biggest is last index, find the biggest element in m_ClothingLayer_SequenceVector
	for (auto itr : m_ClothingLayer_SequenceVector[m_Phantom_SelectedIndex][m_Clothing_SelectedIndex])
	{
		if (itr > m_ClothingLayer_MakingIndex) m_ClothingLayer_MakingIndex = itr;
	}
	m_ClothingLayer_MakingIndex++; // 가장 큰 원소보다 1 더 큰 값


	// Generate clothing layer list
	for (auto itr : m_ClothingLayer_SequenceVector[m_Phantom_SelectedIndex][m_Clothing_SelectedIndex])
	{		
		ClothingLayerSelect_QPushButton[itr] = new QPushButton(m_ClothingLayer_ButtonName[m_Phantom_SelectedIndex][m_Clothing_SelectedIndex][itr]); // 이름 		
		Buttons_FunctionPanelRight.append(ClothingLayerSelect_QPushButton[itr]);
		ClothingLayerSelect_QPushButton[itr]->setObjectName(QString::number(itr));
		ClothingLayerSelect_QPushButton[itr]->setFixedSize(Window_width * 0.04, Window_height * 0.025);
		ClothingLayerSelect_QPushButton[itr]->setStyleSheet(DialogStyle::MENU_BTN7);
		ClothingLayerSelect_QPushButton[itr]->setFont(font_D_BTN14);
		ClothingLayerSelect_QPushButton[itr]->setCheckable(true);
		ClothingLayerSelect_QPushButton[itr]->setChecked(true);
		connect(ClothingLayerSelect_QPushButton[itr], SIGNAL(clicked()), this, SLOT(slot_ClothingLayerSelect_ButtonClicked())); connect(ClothingLayerSelect_QPushButton[itr], SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
		ClothingLayerList_QHBoxLayout->addWidget(ClothingLayerSelect_QPushButton[itr]);
	}
	// Set checked clothing layer button
	for (auto itr : m_ClothingLayer_SequenceVector[m_Phantom_SelectedIndex][m_Clothing_SelectedIndex])
	{
		if (itr != m_ClothingLayer_SelectedIndex)
		{
			ClothingLayerSelect_QPushButton[itr]->setChecked(false);
		}
		else ClothingLayerSelect_QPushButton[itr]->setChecked(true);
	}

	// Set panel info
	if (m_Clothing_SelectedIndex >= 0) SetClothingPanelInfo(m_Clothing_SelectedIndex, m_ClothingLayer_SelectedIndex); // Clothing 존재할 때
	else
	{
		ClothingThickness_QLineEdit->setText(""); // thickness
		ClothingDensity_QLineEdit->setText(""); // density		
		ClothingComposition_QComboBox->setCurrentIndex(0);// Composition
	}
}


////////////////////////// Clothing ////////////////////////// 

// Main function - List
void FunctionPanelRight::slot_ClothingAdd_ButtonClicked()
{
	if (m_Clothing_SequenceVector.size() == const_Clothing_MaximumCount)
	{
		theApp.SetMessageBox("The maximum number of clothing has been reached.");
		return;
	}
	ClothingAddingSetting();
	if (m_Is_ClothingSetting_Closed == true) // X버튼으로 나갔을때
	{
		m_Is_ClothingSetting_Closed = false;
		return;
	}
}
void FunctionPanelRight::slot_ClothingDelete_ButtonClicked()
{
	if (m_Clothing_SequenceVector[m_Phantom_SelectedIndex].size() < 1)
	{
		theApp.SetMessageBox("There is no clothing to delete!");
		return;
	}
	// Clothing 버튼 지우기	
	int originalposition = theApp.removeElement(m_Clothing_SequenceVector[m_Phantom_SelectedIndex], m_Clothing_SelectedIndex); // SequenceVector의 원소 지우기: {0 1 2} -> {0 2}, origianl index는 지워진 1의 자리	
	ClothingList_QHBoxLayout->removeWidget(ClothingSelect_QPushButton[m_Clothing_SelectedIndex]);
	Buttons_FunctionPanelRight.removeOne(ClothingSelect_QPushButton[m_Clothing_SelectedIndex]);
	delete ClothingSelect_QPushButton[m_Clothing_SelectedIndex];
	ClothingSelect_QPushButton.erase(m_Clothing_SelectedIndex);

	// Remove actor
	theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(theApp.PhantomPanelActor[m_Phantom_SelectedIndex]);
	theApp.m_pVTKWidget->renderWindow()->Render();
	theApp.PhantomPanelActor[m_Phantom_SelectedIndex] = nullptr;

	// Remove polydata
	theApp.PhantomPanelPolyData_Collection[m_Phantom_SelectedIndex][1][m_Clothing_SelectedIndex] = nullptr;

	// Appending and generate actor

	theApp.phantomObjects->AppendPhantomPolyData_GeneratePhantomActor(m_Phantom_SelectedIndex);

	// Layer 버튼 모두 지우기			
	for (auto itr_layer : m_ClothingLayer_SequenceVector[m_Phantom_SelectedIndex][m_Clothing_SelectedIndex])
	{
		ClothingLayerList_QHBoxLayout->removeWidget(ClothingLayerSelect_QPushButton[itr_layer]);
		Buttons_FunctionPanelRight.removeOne(ClothingLayerSelect_QPushButton[itr_layer]);
		delete ClothingLayerSelect_QPushButton[itr_layer];
		ClothingLayerSelect_QPushButton.erase(itr_layer);
	}

	// clothing panel 업데이트	
	if (m_Clothing_SequenceVector[m_Phantom_SelectedIndex].size() == 0) // clothing 개수 0일때 현재 선택된 clothing index = -1(Invalid)
	{
		ClothingThickness_QLineEdit->setText(""); // thickness
		ClothingDensity_QLineEdit->setText(""); // density
		ClothingComposition_QComboBox->setCurrentIndex(0); // Composition

		m_Clothing_SelectedIndex = -1;
		m_ClothingLayer_SelectedIndex = -1;
		m_ClothingLayer_MakingIndex = 0;
	}
	else // Clothing 개수 남아있을때 
	{
		m_Clothing_SelectedIndex = m_Clothing_SequenceVector[m_Phantom_SelectedIndex][originalposition]; // 1번 index 자리(2번째 자리)가 지워짐 -> 새로 바뀐 벡터의 1번 index의 원소가 새로운 id
		// Find clothing layer index
		if (m_ClothingLayer_SequenceVector[m_Phantom_SelectedIndex][m_Clothing_SelectedIndex].size() == 0) m_ClothingLayer_SelectedIndex = -1;
		else m_ClothingLayer_SelectedIndex = m_ClothingLayer_SequenceVector[m_Phantom_SelectedIndex][m_Clothing_SelectedIndex][0];// First index		
		for (auto itr_layer : m_ClothingLayer_SequenceVector[m_Phantom_SelectedIndex][m_Clothing_SelectedIndex]) //Generate clothing layer button
		{
			// Generate clothing layer button
			ClothingLayerSelect_QPushButton[itr_layer] = new QPushButton(m_ClothingLayer_ButtonName[m_Phantom_SelectedIndex][m_Clothing_SelectedIndex][itr_layer]);
			Buttons_FunctionPanelRight.append(ClothingLayerSelect_QPushButton[itr_layer]);
			ClothingLayerSelect_QPushButton[itr_layer]->setObjectName(QString::number(itr_layer));
			ClothingLayerSelect_QPushButton[itr_layer]->setFixedSize(Window_width * 0.04, Window_height * 0.025);
			ClothingLayerSelect_QPushButton[itr_layer]->setStyleSheet(DialogStyle::MENU_BTN7);
			ClothingLayerSelect_QPushButton[itr_layer]->setFont(font_D_BTN14);
			ClothingLayerSelect_QPushButton[itr_layer]->setCheckable(true);
			ClothingLayerSelect_QPushButton[itr_layer]->setChecked(false);
			connect(ClothingLayerSelect_QPushButton[itr_layer], SIGNAL(clicked()), this, SLOT(slot_ClothingLayerSelect_ButtonClicked())); connect(ClothingLayerSelect_QPushButton[itr_layer], SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
			ClothingLayerList_QHBoxLayout->addWidget(ClothingLayerSelect_QPushButton[itr_layer]);
		}
		ClothingLayerSelect_QPushButton[m_ClothingLayer_SelectedIndex]->setChecked(true);
		SetClothingPanelInfo(m_Clothing_SelectedIndex, m_ClothingLayer_SelectedIndex); // First index로 panel info 업데이트		
		m_ClothingLayer_MakingIndex = -1; // Biggest is last index, find the biggest element in m_ClothingLayer_SequenceVector
		for (auto itr : m_ClothingLayer_SequenceVector[m_Phantom_SelectedIndex][m_Clothing_SelectedIndex])
		{
			if (itr > m_ClothingLayer_MakingIndex) m_ClothingLayer_MakingIndex = itr;
		}
		m_ClothingLayer_MakingIndex++; // 가장 큰 원소보다 1 더 큰 값
	}
	//  button check
	for (auto itr_clothing : m_Clothing_SequenceVector[m_Phantom_SelectedIndex])
	{
		if (itr_clothing != m_Clothing_SelectedIndex)
		{
			ClothingSelect_QPushButton[itr_clothing]->setChecked(false);
		}
	}
	if (m_Clothing_SelectedIndex >= 0) ClothingSelect_QPushButton[m_Clothing_SelectedIndex]->setChecked(true);
}
void FunctionPanelRight::slot_ClothingSelect_ButtonClicked()
{
	// Get global variables 	
	QPushButton* buttonSender = qobject_cast<QPushButton*>(sender());

	// 원래 버튼 클릭 시 다시 check하고 돌아가기
	if (buttonSender->objectName().toInt() == m_Clothing_SelectedIndex)
	{
		ClothingSelect_QPushButton[m_Clothing_SelectedIndex]->setChecked(true);
		return;
	}

	// Remove clothing layer list
	for (auto itr_layer : m_ClothingLayer_SequenceVector[m_Phantom_SelectedIndex][m_Clothing_SelectedIndex])
	{
		ClothingLayerList_QHBoxLayout->removeWidget(ClothingLayerSelect_QPushButton[itr_layer]);
		Buttons_FunctionPanelRight.removeOne(ClothingLayerSelect_QPushButton[itr_layer]);
		delete ClothingLayerSelect_QPushButton[itr_layer];
		ClothingLayerSelect_QPushButton.erase(itr_layer);
	}

	/////////////////////// Clothing index 변경///////////////////////////
	m_Clothing_SelectedIndex = buttonSender->objectName().toInt();
	int ClothingLayerTotalCount = m_ClothingLayer_SequenceVector[m_Phantom_SelectedIndex][m_Clothing_SelectedIndex].size();

	// Set clicked ClothingSelect_QPushButton
	for (auto itr : m_Clothing_SequenceVector[m_Phantom_SelectedIndex])
	{
		if (itr != m_Clothing_SelectedIndex)
		{
			ClothingSelect_QPushButton[itr]->setChecked(false);
		}
	}

	// Generate clothing layer list
	for (auto itr : m_ClothingLayer_SequenceVector[m_Phantom_SelectedIndex][m_Clothing_SelectedIndex])
	{
		ClothingLayerSelect_QPushButton[itr] = new QPushButton(m_ClothingLayer_ButtonName[m_Phantom_SelectedIndex][m_Clothing_SelectedIndex][itr]);
		Buttons_FunctionPanelRight.append(ClothingLayerSelect_QPushButton[itr]);
		ClothingLayerSelect_QPushButton[itr]->setObjectName(QString::number(itr));
		ClothingLayerSelect_QPushButton[itr]->setFixedSize(Window_width * 0.04, Window_height * 0.025);
		ClothingLayerSelect_QPushButton[itr]->setStyleSheet(DialogStyle::MENU_BTN7);
		ClothingLayerSelect_QPushButton[itr]->setFont(font_D_BTN14);
		ClothingLayerSelect_QPushButton[itr]->setCheckable(true);
		ClothingLayerSelect_QPushButton[itr]->setChecked(true);
		connect(ClothingLayerSelect_QPushButton[itr], SIGNAL(clicked()), this, SLOT(slot_ClothingLayerSelect_ButtonClicked())); connect(ClothingLayerSelect_QPushButton[itr], SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
		ClothingLayerList_QHBoxLayout->addWidget(ClothingLayerSelect_QPushButton[itr]);
	}

	// Find clothing layer index
	if (m_ClothingLayer_SequenceVector[m_Phantom_SelectedIndex][m_Clothing_SelectedIndex].size() == 0) m_ClothingLayer_SelectedIndex = -1;
	else m_ClothingLayer_SelectedIndex = m_ClothingLayer_SequenceVector[m_Phantom_SelectedIndex][m_Clothing_SelectedIndex][0];// First index
	m_ClothingLayer_MakingIndex = -1; // Biggest is last index, find the biggest element in m_ClothingLayer_SequenceVector
	for (auto itr : m_ClothingLayer_SequenceVector[m_Phantom_SelectedIndex][m_Clothing_SelectedIndex])
	{
		if (itr > m_ClothingLayer_MakingIndex) m_ClothingLayer_MakingIndex = itr;
	}
	m_ClothingLayer_MakingIndex++; // 가장 큰 원소보다 1 더 큰 값

	// Set clicked ClothingLayerSelect_QPushButton
	for (auto itr : m_ClothingLayer_SequenceVector[m_Phantom_SelectedIndex][m_Clothing_SelectedIndex])
	{
		if (itr == m_ClothingLayer_SequenceVector[m_Phantom_SelectedIndex][m_Clothing_SelectedIndex][m_ClothingLayer_SelectedIndex]) // 다른 clothing 버튼 클릭 시 layer는 첫번째로 고정
		{
			ClothingLayerSelect_QPushButton[itr]->setChecked(true);
		}
		else ClothingLayerSelect_QPushButton[itr]->setChecked(false);
	}
	SetClothingPanelInfo(m_Clothing_SelectedIndex, m_ClothingLayer_SelectedIndex); // Set clothing panel info -> 다른 clothing 버튼 클릭 시 layer는 0번으로 고정됨
}
void FunctionPanelRight::slot_ClothingLayerAdd_ButtonClicked()
{
	if (m_ClothingLayer_SequenceVector[m_Phantom_SelectedIndex][m_Clothing_SelectedIndex].size() == const_ClothingLayer_MaximumCount)
	{
		theApp.SetMessageBox("The maximum number of clothing layer has been reached.");
		return;
	}
	ClothingLayerAddingSetting();
	if (m_Is_ClothingLayer_Setting_Closed == true) // X버튼으로 나갔을때
	{
		m_Is_ClothingLayer_Setting_Closed = false;
		return;
	}

	// Get global variables 
	m_ClothingLayer_SequenceVector[m_Phantom_SelectedIndex][m_Clothing_SelectedIndex].push_back(m_ClothingLayer_MakingIndex);
	//
	ClothingLayerSelect_QPushButton[m_ClothingLayer_MakingIndex] = new QPushButton(m_ClothingLayer_ButtonName[m_Phantom_SelectedIndex][m_Clothing_SelectedIndex][m_ClothingLayer_MakingIndex]); // 이름 		
	Buttons_FunctionPanelRight.append(ClothingLayerSelect_QPushButton[m_ClothingLayer_MakingIndex]);
	ClothingLayerSelect_QPushButton[m_ClothingLayer_MakingIndex]->setObjectName(QString::number(m_ClothingLayer_MakingIndex));
	ClothingLayerSelect_QPushButton[m_ClothingLayer_MakingIndex]->setFixedSize(Window_width * 0.04, Window_height * 0.025);
	ClothingLayerSelect_QPushButton[m_ClothingLayer_MakingIndex]->setStyleSheet(DialogStyle::MENU_BTN7);
	ClothingLayerSelect_QPushButton[m_ClothingLayer_MakingIndex]->setFont(font_D_BTN14);
	ClothingLayerSelect_QPushButton[m_ClothingLayer_MakingIndex]->setCheckable(true);
	ClothingLayerSelect_QPushButton[m_ClothingLayer_MakingIndex]->setChecked(true);
	connect(ClothingLayerSelect_QPushButton[m_ClothingLayer_MakingIndex], SIGNAL(clicked()), this, SLOT(slot_ClothingLayerSelect_ButtonClicked())); connect(ClothingLayerSelect_QPushButton[m_ClothingLayer_MakingIndex], SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
	ClothingLayerList_QHBoxLayout->addWidget(ClothingLayerSelect_QPushButton[m_ClothingLayer_MakingIndex]);

	for (auto itr : m_ClothingLayer_SequenceVector[m_Phantom_SelectedIndex][m_Clothing_SelectedIndex])
	{
		if (itr != m_ClothingLayer_MakingIndex)
		{
			ClothingLayerSelect_QPushButton[itr]->setChecked(false);
		}
	}
	//Dosimeter scorll bar max 이동
	timer_scroll_clothingLayer->start(1); // 1ms 이후 이동 -> 그냥 함수에 넣으면 multiplephantombutton 추가가 더 늦어서 최대로 안 감

	// Set global variables 
	m_ClothingLayer_SelectedIndex = m_ClothingLayer_MakingIndex;
	m_ClothingLayer_MakingIndex++;
}
void FunctionPanelRight::slot_ClothingLayerDelete_ButtonClicked()
{
	if (m_ClothingLayer_SequenceVector[m_Phantom_SelectedIndex][m_Clothing_SelectedIndex].size() <= 1) //
	{
		theApp.SetMessageBox("At least one layer of clothing needed. To remove clothing, click Delete button of clothing.");
		return;
	}

	// 버튼 지우기	
	int originalposition = theApp.removeElement(m_ClothingLayer_SequenceVector[m_Phantom_SelectedIndex][m_Clothing_SelectedIndex], m_ClothingLayer_SelectedIndex); //{0 1 2} -> {0 2}, origianl index는 지워진 1의 자리	
	ClothingLayerList_QHBoxLayout->removeWidget(ClothingLayerSelect_QPushButton[m_ClothingLayer_SelectedIndex]);
	Buttons_FunctionPanelRight.removeOne(ClothingLayerSelect_QPushButton[m_ClothingLayer_SelectedIndex]);
	delete ClothingLayerSelect_QPushButton[m_ClothingLayer_SelectedIndex];
	ClothingLayerSelect_QPushButton.erase(m_ClothingLayer_SelectedIndex);

	// clothingPanel 업데이트
	m_ClothingLayer_SelectedIndex = m_ClothingLayer_SequenceVector[m_Phantom_SelectedIndex][m_Clothing_SelectedIndex][originalposition]; //1번 index 자리(2번째 자리)가 지워짐 -> 새로 바뀐 벡터의 1번 index의 원소가 새로운 id
	SetClothingPanelInfo(m_Clothing_SelectedIndex, m_ClothingLayer_SelectedIndex);

	// button check
	for (auto itr_clothing : m_ClothingLayer_SequenceVector[m_Phantom_SelectedIndex][m_Clothing_SelectedIndex])
	{
		if (itr_clothing != m_ClothingLayer_SelectedIndex)
		{
			ClothingLayerSelect_QPushButton[itr_clothing]->setChecked(false);
		}
	}
	if (m_ClothingLayer_SelectedIndex >= 0) ClothingLayerSelect_QPushButton[m_ClothingLayer_SelectedIndex]->setChecked(true);
}
void FunctionPanelRight::slot_ClothingLayerSelect_ButtonClicked()
{
	QPushButton* buttonSender = qobject_cast<QPushButton*>(sender());
	// 원래 버튼 클릭 시 다시 check하고 돌아가기
	if (buttonSender->objectName().toInt() == m_ClothingLayer_SelectedIndex)
	{
		ClothingLayerSelect_QPushButton[m_ClothingLayer_SelectedIndex]->setChecked(true);
		return;
	}

	m_ClothingLayer_SelectedIndex = buttonSender->objectName().toInt();
	for (auto itr : m_ClothingLayer_SequenceVector[m_Phantom_SelectedIndex][m_Clothing_SelectedIndex])
	{
		if (itr != m_ClothingLayer_SelectedIndex)
		{
			ClothingLayerSelect_QPushButton[itr]->setChecked(false);
		}
		else ClothingLayerSelect_QPushButton[itr]->setChecked(true);
	}

	SetClothingPanelInfo(m_Clothing_SelectedIndex, m_ClothingLayer_SelectedIndex); // Set clothing panel info
}

// Main function - Setting
void FunctionPanelRight::ClothingAddingSetting()
{
	m_Is_ClothingSetting_OKbutton_Clicked = false;

	// Create a new dialog
	ClothingSetting_UserQDialog = new MultipleUIDialog(this);
	ClothingSetting_UserQDialog->setStyleSheet("background-color: rgb(205, 222, 243);");
	ClothingSetting_UserQDialog->setFixedWidth(Window_width * 0.3);
	ClothingSetting_UserQDialog->setFixedHeight(Window_height * 0.35);
	ClothingSetting_UserQDialog->setWindowTitle("Clothing settings");
	ClothingSetting_UserQDialog->setFocusPolicy(Qt::StrongFocus);
	QScrollArea* scrollArea = new QScrollArea(ClothingSetting_UserQDialog);
	scrollArea->setWidgetResizable(true);
	scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	QWidget* containerWidget = new QWidget;
	QVBoxLayout* layout = new QVBoxLayout(containerWidget);

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);
		hLayout->setSpacing(Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QLabel* label0 = new QLabel;
		label0->setFixedSize(Window_width * 0.090, Window_height * 0.028);
		label0->setStyleSheet(DialogStyle::DATA_LABEL);
		label0->setFont(font_D_LBL1);
		label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		label0->setText(QString::fromLocal8Bit("Type:"));

		ClothingSetting_Type_QComboBox = new QComboBox;
		ClothingSetting_Type_QComboBox->setFixedSize(Window_width * 0.105, Window_height * 0.028);
		ClothingSetting_Type_QComboBox->setStyleSheet(DialogStyle::COMBOBOX);
		ClothingSetting_Type_QComboBox->setFont(font_D_CB1);

		ClothingSetting_Type_QComboBox->addItem("User-defined"); // Disable user-defined clothing
		ClothingSetting_Type_QComboBox->addItem("Upper head");
		ClothingSetting_Type_QComboBox->addItem("Upper arms");
		ClothingSetting_Type_QComboBox->addItem("Lower arms");
		ClothingSetting_Type_QComboBox->addItem("Hands");
		ClothingSetting_Type_QComboBox->addItem("Front trunk");
		ClothingSetting_Type_QComboBox->addItem("Back trunk");
		ClothingSetting_Type_QComboBox->addItem("Pelvis");
		ClothingSetting_Type_QComboBox->addItem("Upper legs");
		ClothingSetting_Type_QComboBox->addItem("Lower legs");
		ClothingSetting_Type_QComboBox->addItem("Feet");

		ClothingSetting_Type_QComboBox->setCurrentIndex(0);
		ClothingSetting_Type_QComboBox->setFocusPolicy(Qt::StrongFocus); // For focusing when scroll the comobox
		connect(ClothingSetting_Type_QComboBox, SIGNAL(activated(int)), this, SLOT(slot_ChangeClothingSettingTypeCombo(int)));

		hLayout->addWidget(label0);
		hLayout->addWidget(ClothingSetting_Type_QComboBox);

		layout->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);
		hLayout->setSpacing(Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QLabel* label0 = new QLabel;
		label0->setFixedSize(Window_width * 0.090, Window_height * 0.028);
		label0->setStyleSheet(DialogStyle::DATA_LABEL);
		label0->setFont(font_D_LBL1);
		label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		label0->setText(QString::fromLocal8Bit("Clothing name:"));

		ClothingSetting_Name_QLineEdit = new QLineEdit;
		ClothingSetting_Name_QLineEdit->setFixedSize(Window_width * 0.170, Window_height * 0.028);
		ClothingSetting_Name_QLineEdit->setAlignment(Qt::AlignCenter);
		ClothingSetting_Name_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		ClothingSetting_Name_QLineEdit->setFont(font_D_LE1);
		ClothingSetting_Name_QLineEdit->setPlaceholderText("Clothing_" + QString::number(m_Clothing_MakingIndex));

		hLayout->addWidget(label0);
		hLayout->addWidget(ClothingSetting_Name_QLineEdit);

		layout->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);
		hLayout->setSpacing(Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QLabel* label0 = new QLabel;
		label0->setFixedSize(Window_width * 0.090, Window_height * 0.028);
		label0->setStyleSheet(DialogStyle::DATA_LABEL);
		label0->setFont(font_D_LBL1);
		label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		label0->setText(QString::fromLocal8Bit("Layer"));

		hLayout->addWidget(label0);

		layout->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setContentsMargins(Window_width * 0.053, Window_height * 0.005, Window_width * 0.003, 0);
		hLayout->setSpacing(Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QLabel* label0 = new QLabel;
		label0->setFixedSize(Window_width * 0.080, Window_height * 0.028);
		label0->setStyleSheet(DialogStyle::DATA_LABEL);
		label0->setFont(font_D_LBL1);
		label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		label0->setText(QString::fromLocal8Bit("Layer name:"));

		ClothingSetting_Layername_QLineEdit = new QLineEdit;
		ClothingSetting_Layername_QLineEdit->setFixedSize(Window_width * 0.120, Window_height * 0.028);
		ClothingSetting_Layername_QLineEdit->setAlignment(Qt::AlignCenter);
		ClothingSetting_Layername_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		ClothingSetting_Layername_QLineEdit->setFont(font_D_LE1);
		ClothingSetting_Layername_QLineEdit->setPlaceholderText("Layer_0"); // Clothing을 새로 제작할 때는 항상 0번 레이어		

		hLayout->addWidget(label0);
		hLayout->addWidget(ClothingSetting_Layername_QLineEdit);

		layout->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setContentsMargins(Window_width * 0.053, Window_height * 0.005, Window_width * 0.003, 0);
		hLayout->setSpacing(Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QLabel* label0 = new QLabel;
		label0->setFixedSize(Window_width * 0.080, Window_height * 0.028);
		label0->setStyleSheet(DialogStyle::DATA_LABEL);
		label0->setFont(font_D_LBL1);
		label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		label0->setText(QString::fromLocal8Bit("Thickness:"));

		ClothingSetting_Thickness_QLineEdit = new QLineEdit;
		ClothingSetting_Thickness_QLineEdit->setFixedSize(Window_width * 0.120, Window_height * 0.028);
		ClothingSetting_Thickness_QLineEdit->setAlignment(Qt::AlignCenter);
		ClothingSetting_Thickness_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		ClothingSetting_Thickness_QLineEdit->setFont(font_D_LE1);
		ClothingSetting_Thickness_QLineEdit->setPlaceholderText("cm");

		hLayout->addWidget(label0);
		hLayout->addWidget(ClothingSetting_Thickness_QLineEdit);

		layout->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setContentsMargins(Window_width * 0.053, Window_height * 0.005, Window_width * 0.003, 0);
		hLayout->setSpacing(Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QLabel* label0 = new QLabel;
		label0->setFixedSize(Window_width * 0.080, Window_height * 0.028);
		label0->setStyleSheet(DialogStyle::DATA_LABEL);
		label0->setFont(font_D_LBL1);
		label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		label0->setText(QString::fromLocal8Bit("Density:"));

		ClothingSetting_Density_QLineEdit = new QLineEdit;
		ClothingSetting_Density_QLineEdit->setFixedSize(Window_width * 0.120, Window_height * 0.028);
		ClothingSetting_Density_QLineEdit->setAlignment(Qt::AlignCenter);
		ClothingSetting_Density_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		ClothingSetting_Density_QLineEdit->setFont(font_D_LE1);
		ClothingSetting_Density_QLineEdit->setPlaceholderText("g/cm3");

		hLayout->addWidget(label0);
		hLayout->addWidget(ClothingSetting_Density_QLineEdit);

		layout->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setContentsMargins(Window_width * 0.053, Window_height * 0.005, Window_width * 0.003, 0);
		hLayout->setSpacing(Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QLabel* label0 = new QLabel;
		label0->setFixedSize(Window_width * 0.080, Window_height * 0.028);
		label0->setStyleSheet(DialogStyle::DATA_LABEL);
		label0->setFont(font_D_LBL1);
		label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		label0->setText(QString::fromLocal8Bit("Composition:"));

		ClothingSetting_Composition_QComboBox = new QComboBox;
		ClothingSetting_Composition_QComboBox->setFixedSize(Window_width * 0.120, Window_height * 0.028);
		ClothingSetting_Composition_QComboBox->setStyleSheet(DialogStyle::COMBOBOX);
		ClothingSetting_Composition_QComboBox->setFont(font_D_CB1);

		ClothingSetting_Composition_QComboBox->setEditable(true);
		ClothingSetting_Composition_QComboBox->lineEdit()->setReadOnly(true);
		ClothingSetting_Composition_QComboBox->lineEdit()->setAlignment(Qt::AlignCenter);
		ClothingSetting_Composition_QComboBox->lineEdit()->setFont(font_D_CB1);

		ClothingSetting_Composition_QComboBox->addItem("Cloth");
		ClothingSetting_Composition_QComboBox->addItem("Lead");
		ClothingSetting_Composition_QComboBox->addItem("Air");
		ClothingSetting_Composition_QComboBox->setCurrentIndex(0);

		hLayout->addWidget(label0);
		hLayout->addWidget(ClothingSetting_Composition_QComboBox);

		layout->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		hLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);

		ClothingSetting_AddingOK_QPushButton = new QPushButton(QString::fromLocal8Bit("OK"));
		Buttons_FunctionPanelRight.append(ClothingSetting_AddingOK_QPushButton);
		ClothingSetting_AddingOK_QPushButton->setFixedSize(Window_width * 0.104, Window_height * 0.028);
		ClothingSetting_AddingOK_QPushButton->setCheckable(false);
		ClothingSetting_AddingOK_QPushButton->setStyleSheet(DialogStyle::MENU_BTN3);
		ClothingSetting_AddingOK_QPushButton->setFont(font_D_BTN3);

		connect(ClothingSetting_AddingOK_QPushButton, SIGNAL(clicked()), this, SLOT(slot_ClothingAddingOK_ButtonClicked())); connect(ClothingSetting_AddingOK_QPushButton, SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));

		hLayout->addWidget(ClothingSetting_AddingOK_QPushButton);

		layout->addLayout(hLayout);
	}

	scrollArea->setWidget(containerWidget);
	QVBoxLayout* dialogLayout = new QVBoxLayout(ClothingSetting_UserQDialog);
	dialogLayout->addWidget(scrollArea);
	ClothingSetting_UserQDialog->setLayout(dialogLayout);
	ClothingSetting_UserQDialog->adjustSize();

	ClothingSetting_UserQDialog->exec(); // Use exec() for a modal dialog or show() for a non-modal dialog
}
void FunctionPanelRight::slot_ChangeClothingSettingTypeCombo(int idx) // called by (1) ClothingAddingSetting
{
	ClothingSetting_Type_QComboBox->hide();	ClothingSetting_Type_QComboBox->show(); // This is for focusing (Do not show white blank)
	if (idx != 0)
	{
		ClothingSetting_Name_QLineEdit->setText(ClothingSetting_Type_QComboBox->itemText(idx));
		ClothingSetting_Name_QLineEdit->setDisabled(true);
	}
	else
	{
		ClothingSetting_Name_QLineEdit->clear();
		ClothingSetting_Name_QLineEdit->setEnabled(true);
	}
}
void FunctionPanelRight::slot_ClothingAddingOK_ButtonClicked()
{
	if (m_Phantom_SelectedIndex == -1)
	{
		theApp.SetMessageBox("Phantom is not generated!");
		return;
	}

	int SelectedClothingTypeIndex = ClothingSetting_Type_QComboBox->currentIndex();

	if (SelectedClothingTypeIndex != 0) // Pre-defined 의복을 입력할 때
	{
		for (auto itr_clothingIndex : m_Clothing_SequenceVector[m_Phantom_SelectedIndex])
		{
			if (SelectedClothingTypeIndex == m_Clothing_Type[m_Phantom_SelectedIndex][itr_clothingIndex])
			{
				theApp.SetMessageBox("Same clothing type is already generated!");
				return;
			}
		}
		QString strPath = DefinePreWearablePath(SelectedClothingTypeIndex);

		theApp.phantomObjects->PhantomClothingGenerate(strPath, true);
	}
	else // User-defined 의복을 입력할 때
	{
		// Wearable making을 위해 camera 모드로 전환
		theApp.MouseControl_toCamera();
		theApp.phantomObjects->RubberBandInitialization();
		AllButtonDisabled();
		// 이후, ETInteractorStyleRubberBand가 활성화되어, 완료 후 PhantomClothingGenerate 실행됨.
	}

	m_Is_ClothingSetting_OKbutton_Clicked = true; // OK 버튼이 클릭됨
	ClothingSetting_UserQDialog->close();
}
void FunctionPanelRight::ClothingLayerAddingSetting()
{
	m_Is_ClothingLayer_Setting_OKbutton_Clicked = false;

	// Create a new dialog
	ClothingLayerSetting_UserQDialog = new MultipleUIDialog(this);
	ClothingLayerSetting_UserQDialog->setStyleSheet("background-color: rgb(205, 222, 243);");
	ClothingLayerSetting_UserQDialog->setFixedWidth(Window_width * 0.3);
	ClothingLayerSetting_UserQDialog->setFixedHeight(Window_height * 0.25);
	ClothingLayerSetting_UserQDialog->setWindowTitle("Clothing settings");
	ClothingLayerSetting_UserQDialog->setFocusPolicy(Qt::StrongFocus);
	QScrollArea* scrollArea = new QScrollArea(ClothingLayerSetting_UserQDialog);
	scrollArea->setWidgetResizable(true);
	scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	QWidget* containerWidget = new QWidget;
	QVBoxLayout* layout = new QVBoxLayout(containerWidget);

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);
		hLayout->setSpacing(Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QLabel* label0 = new QLabel;
		label0->setFixedSize(Window_width * 0.090, Window_height * 0.028);
		label0->setStyleSheet(DialogStyle::DATA_LABEL);
		label0->setFont(font_D_LBL1);
		label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		label0->setText(QString::fromLocal8Bit("Layer name:"));

		ClothingLayerSetting_Layername_QLineEdit = new QLineEdit;
		ClothingLayerSetting_Layername_QLineEdit->setFixedSize(Window_width * 0.120, Window_height * 0.028);
		ClothingLayerSetting_Layername_QLineEdit->setAlignment(Qt::AlignCenter);
		ClothingLayerSetting_Layername_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		ClothingLayerSetting_Layername_QLineEdit->setFont(font_D_LE1);
		ClothingLayerSetting_Layername_QLineEdit->setPlaceholderText("Layer_" + QString::number(m_ClothingLayer_MakingIndex));		

		hLayout->addWidget(label0);
		hLayout->addWidget(ClothingLayerSetting_Layername_QLineEdit);

		layout->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);
		hLayout->setSpacing(Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QLabel* label0 = new QLabel;
		label0->setFixedSize(Window_width * 0.090, Window_height * 0.028);
		label0->setStyleSheet(DialogStyle::DATA_LABEL);
		label0->setFont(font_D_LBL1);
		label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		label0->setText(QString::fromLocal8Bit("Thickness:"));

		ClothingLayerSetting_Thickness_QLineEdit = new QLineEdit;
		ClothingLayerSetting_Thickness_QLineEdit->setFixedSize(Window_width * 0.120, Window_height * 0.028);
		ClothingLayerSetting_Thickness_QLineEdit->setAlignment(Qt::AlignCenter);
		ClothingLayerSetting_Thickness_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		ClothingLayerSetting_Thickness_QLineEdit->setFont(font_D_LE1);
		ClothingLayerSetting_Thickness_QLineEdit->setPlaceholderText("cm");

		hLayout->addWidget(label0);
		hLayout->addWidget(ClothingLayerSetting_Thickness_QLineEdit);

		layout->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);
		hLayout->setSpacing(Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QLabel* label0 = new QLabel;
		label0->setFixedSize(Window_width * 0.090, Window_height * 0.028);
		label0->setStyleSheet(DialogStyle::DATA_LABEL);
		label0->setFont(font_D_LBL1);
		label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		label0->setText(QString::fromLocal8Bit("Density:"));

		ClothingLayerSetting_Density_QLineEdit = new QLineEdit;
		ClothingLayerSetting_Density_QLineEdit->setFixedSize(Window_width * 0.120, Window_height * 0.028);
		ClothingLayerSetting_Density_QLineEdit->setAlignment(Qt::AlignCenter);
		ClothingLayerSetting_Density_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		ClothingLayerSetting_Density_QLineEdit->setFont(font_D_LE1);
		ClothingLayerSetting_Density_QLineEdit->setPlaceholderText("g/cm3");

		hLayout->addWidget(label0);
		hLayout->addWidget(ClothingLayerSetting_Density_QLineEdit);

		layout->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);
		hLayout->setSpacing(Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QLabel* label0 = new QLabel;
		label0->setFixedSize(Window_width * 0.090, Window_height * 0.028);
		label0->setStyleSheet(DialogStyle::DATA_LABEL);
		label0->setFont(font_D_LBL1);
		label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		label0->setText(QString::fromLocal8Bit("Composition:"));

		ClothingLayerSetting_Composition_QComboBox = new QComboBox;
		ClothingLayerSetting_Composition_QComboBox->setFixedSize(Window_width * 0.120, Window_height * 0.028);
		ClothingLayerSetting_Composition_QComboBox->setStyleSheet(DialogStyle::COMBOBOX);
		ClothingLayerSetting_Composition_QComboBox->setFont(font_D_CB1);

		ClothingLayerSetting_Composition_QComboBox->setEditable(true);
		ClothingLayerSetting_Composition_QComboBox->lineEdit()->setReadOnly(true);
		ClothingLayerSetting_Composition_QComboBox->lineEdit()->setAlignment(Qt::AlignCenter);
		ClothingLayerSetting_Composition_QComboBox->lineEdit()->setFont(font_D_CB1);

		ClothingLayerSetting_Composition_QComboBox->addItem("Cloth");
		ClothingLayerSetting_Composition_QComboBox->addItem("Lead");
		ClothingLayerSetting_Composition_QComboBox->addItem("Air");
		ClothingLayerSetting_Composition_QComboBox->setCurrentIndex(0);

		hLayout->addWidget(label0);
		hLayout->addWidget(ClothingLayerSetting_Composition_QComboBox);

		layout->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		hLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);

		ClothingLayerSetting_AddingOK_QPushButton = new QPushButton(QString::fromLocal8Bit("OK"));
		Buttons_FunctionPanelRight.append(ClothingLayerSetting_AddingOK_QPushButton);
		ClothingLayerSetting_AddingOK_QPushButton->setFixedSize(Window_width * 0.104, Window_height * 0.028);
		ClothingLayerSetting_AddingOK_QPushButton->setCheckable(false);
		ClothingLayerSetting_AddingOK_QPushButton->setStyleSheet(DialogStyle::MENU_BTN3);
		ClothingLayerSetting_AddingOK_QPushButton->setFont(font_D_BTN3);

		connect(ClothingLayerSetting_AddingOK_QPushButton, SIGNAL(clicked()), this, SLOT(slot_ClothingLayerAddingOK_ButtonClicked())); connect(ClothingLayerSetting_AddingOK_QPushButton, SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));

		hLayout->addWidget(ClothingLayerSetting_AddingOK_QPushButton);

		layout->addLayout(hLayout);
	}

	scrollArea->setWidget(containerWidget);
	QVBoxLayout* dialogLayout = new QVBoxLayout(ClothingLayerSetting_UserQDialog);
	dialogLayout->addWidget(scrollArea);
	ClothingLayerSetting_UserQDialog->setLayout(dialogLayout);
	ClothingLayerSetting_UserQDialog->adjustSize();

	ClothingLayerSetting_UserQDialog->exec(); // Use exec() for a modal dialog or show() for a non-modal dialog

	// Set global variables 

}
void FunctionPanelRight::slot_ClothingLayerAddingOK_ButtonClicked()
{
	SaveClothingInformation_InLayerGenerate();
	SetClothingPanelInfo(m_Clothing_SelectedIndex, m_ClothingLayer_MakingIndex);

	m_Is_ClothingLayer_Setting_OKbutton_Clicked = true; // OK 버튼이 클릭됨
	ClothingLayerSetting_UserQDialog->close();
}

// Main function - Status
void FunctionPanelRight::slot_ClothingLayerUpdate_ButtonClicked()
{
	m_Clothing_MainInfo[m_Phantom_SelectedIndex][m_Clothing_SelectedIndex][m_ClothingLayer_SelectedIndex][0] = ClothingThickness_QLineEdit->text().toDouble();
	m_Clothing_MainInfo[m_Phantom_SelectedIndex][m_Clothing_SelectedIndex][m_ClothingLayer_SelectedIndex][1] = ClothingDensity_QLineEdit->text().toDouble();
	m_Clothing_MainInfo[m_Phantom_SelectedIndex][m_Clothing_SelectedIndex][m_ClothingLayer_SelectedIndex][2] = ClothingComposition_QComboBox->currentIndex();
}

// Sub-function (1)
void FunctionPanelRight::SaveClothingInformation_InClothingGenerate() // called by (1) ETHuman3DApp::PhantomClothingGenerate
{
	// Clothing name
	if (ClothingSetting_Name_QLineEdit->text().isEmpty()) m_Clothing_ButtonName[m_Phantom_SelectedIndex][m_Clothing_MakingIndex] = "Clothing_" + QString::number(m_Clothing_MakingIndex);
	else m_Clothing_ButtonName[m_Phantom_SelectedIndex][m_Clothing_MakingIndex] = ClothingSetting_Name_QLineEdit->text();

	// Clothing Layer name
	if (ClothingSetting_Layername_QLineEdit->text().isEmpty()) m_ClothingLayer_ButtonName[m_Phantom_SelectedIndex][m_Clothing_MakingIndex][0] = "Layer_0";
	else m_ClothingLayer_ButtonName[m_Phantom_SelectedIndex][m_Clothing_MakingIndex][0] = ClothingSetting_Layername_QLineEdit->text();

	m_Clothing_Type[m_Phantom_SelectedIndex][m_Clothing_MakingIndex] = ClothingSetting_Type_QComboBox->currentIndex(); // Type

	// ClothingLayerIndex is always 0 (First layer of clothing)
	m_Clothing_MainInfo[m_Phantom_SelectedIndex][m_Clothing_MakingIndex][0][-1] = 1; // 사용여부
	m_Clothing_MainInfo[m_Phantom_SelectedIndex][m_Clothing_MakingIndex][0][0] = ClothingSetting_Thickness_QLineEdit->text().toDouble();  // thickness
	m_Clothing_MainInfo[m_Phantom_SelectedIndex][m_Clothing_MakingIndex][0][1] = ClothingSetting_Density_QLineEdit->text().toDouble(); // density
	m_Clothing_MainInfo[m_Phantom_SelectedIndex][m_Clothing_MakingIndex][0][2] = ClothingSetting_Composition_QComboBox->currentIndex(); // composition	
}
void FunctionPanelRight::InitializeClothingPanel_InClothingAdd() // called by (1) ETHuman3DApp::PhantomClothingGenerate
{
	m_Clothing_SequenceVector[m_Phantom_SelectedIndex].push_back(m_Clothing_MakingIndex);
	int ClothingTotalCount = m_Clothing_SequenceVector[m_Phantom_SelectedIndex].size();
		
	ClothingSelect_QPushButton[m_Clothing_MakingIndex] = new QPushButton(m_Clothing_ButtonName[m_Phantom_SelectedIndex][m_Clothing_MakingIndex]); // 이름 	
	Buttons_FunctionPanelRight.append(ClothingSelect_QPushButton[m_Clothing_MakingIndex]);
	ClothingSelect_QPushButton[m_Clothing_MakingIndex]->setObjectName(QString::number(m_Clothing_MakingIndex));
	ClothingSelect_QPushButton[m_Clothing_MakingIndex]->setFixedSize(Window_width * 0.05, Window_height * 0.040);
	ClothingSelect_QPushButton[m_Clothing_MakingIndex]->setStyleSheet(DialogStyle::MENU_BTN7);
	ClothingSelect_QPushButton[m_Clothing_MakingIndex]->setFont(font_D_BTN14);
	ClothingSelect_QPushButton[m_Clothing_MakingIndex]->setCheckable(true);
	ClothingSelect_QPushButton[m_Clothing_MakingIndex]->setChecked(true);
	connect(ClothingSelect_QPushButton[m_Clothing_MakingIndex], SIGNAL(clicked()), this, SLOT(slot_ClothingSelect_ButtonClicked())); connect(ClothingSelect_QPushButton[m_Clothing_MakingIndex], SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
	ClothingList_QHBoxLayout->addWidget(ClothingSelect_QPushButton[m_Clothing_MakingIndex]);
	for (auto itr : m_Clothing_SequenceVector[m_Phantom_SelectedIndex])
	{
		if (itr != m_Clothing_MakingIndex)
		{
			ClothingSelect_QPushButton[itr]->setChecked(false);
		}
	}
	// scorll bar max 이동
	timer_scroll_clothing->start(1); // 1ms 이후 이동 -> 그냥 함수에 넣으면 multiplephantombutton 추가가 더 늦어서 최대로 안 감

	// Clothing layer 초기화
	for (auto itr_layer : m_ClothingLayer_SequenceVector[m_Phantom_SelectedIndex][m_Clothing_SelectedIndex])
	{
		ClothingLayerList_QHBoxLayout->removeWidget(ClothingLayerSelect_QPushButton[itr_layer]);
		Buttons_FunctionPanelRight.removeOne(ClothingLayerSelect_QPushButton[itr_layer]);
		delete ClothingLayerSelect_QPushButton[itr_layer];
		ClothingLayerSelect_QPushButton.erase(itr_layer);
	}

	// Get global variables 
	m_ClothingLayer_SequenceVector[m_Phantom_SelectedIndex][m_Clothing_MakingIndex].clear();
	m_ClothingLayer_SequenceVector[m_Phantom_SelectedIndex][m_Clothing_MakingIndex].push_back(0);
	//	
	ClothingLayerSelect_QPushButton[0] = new QPushButton(m_ClothingLayer_ButtonName[m_Phantom_SelectedIndex][m_Clothing_MakingIndex][0]); // 이름 	
	Buttons_FunctionPanelRight.append(ClothingLayerSelect_QPushButton[0]);
	ClothingLayerSelect_QPushButton[0]->setObjectName(QString::number(0));
	ClothingLayerSelect_QPushButton[0]->setFixedSize(Window_width * 0.04, Window_height * 0.025);
	ClothingLayerSelect_QPushButton[0]->setStyleSheet(DialogStyle::MENU_BTN7);
	ClothingLayerSelect_QPushButton[0]->setFont(font_D_BTN14);
	ClothingLayerSelect_QPushButton[0]->setCheckable(true);
	ClothingLayerSelect_QPushButton[0]->setChecked(true);
	connect(ClothingLayerSelect_QPushButton[0], SIGNAL(clicked()), this, SLOT(slot_ClothingLayerSelect_ButtonClicked())); connect(ClothingLayerSelect_QPushButton[0], SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
	ClothingLayerList_QHBoxLayout->addWidget(ClothingLayerSelect_QPushButton[0]);

	//Dosimeter scorll bar max 이동
	timer_scroll_clothingLayer->start(1); // 1ms 이후 이동 -> 그냥 함수에 넣으면 multiplephantombutton 추가가 더 늦어서 최대로 안 감

	// Set global variables 
	m_Clothing_SelectedIndex = m_Clothing_MakingIndex; // Set m_Clothing_SelectedIndex to be m_Clothing_MakingIndex
	m_Clothing_MakingIndex++;
	m_ClothingLayer_MakingIndex = 1; // 0번은 기본으로 만듦
	m_ClothingLayer_SelectedIndex = 0;
}
void FunctionPanelRight::SaveClothingInformation_InLayerGenerate() // called by (1) slot_ClothingLayerAddingOK_ButtonClicked
{
	// Clothing Layer name
	if (ClothingLayerSetting_Layername_QLineEdit->text().isEmpty()) m_ClothingLayer_ButtonName[m_Phantom_SelectedIndex][m_Clothing_SelectedIndex][m_ClothingLayer_MakingIndex] = "Layer_" + QString::number(m_ClothingLayer_MakingIndex);
	else m_ClothingLayer_ButtonName[m_Phantom_SelectedIndex][m_Clothing_SelectedIndex][m_ClothingLayer_MakingIndex] = ClothingLayerSetting_Layername_QLineEdit->text();
		
	m_Clothing_MainInfo[m_Phantom_SelectedIndex][m_Clothing_SelectedIndex][m_ClothingLayer_MakingIndex][0] = ClothingLayerSetting_Thickness_QLineEdit->text().toDouble();  // thickness
	m_Clothing_MainInfo[m_Phantom_SelectedIndex][m_Clothing_SelectedIndex][m_ClothingLayer_MakingIndex][1] = ClothingLayerSetting_Density_QLineEdit->text().toDouble(); // density
	m_Clothing_MainInfo[m_Phantom_SelectedIndex][m_Clothing_SelectedIndex][m_ClothingLayer_MakingIndex][2] = ClothingLayerSetting_Composition_QComboBox->currentIndex(); // composition	
}
void FunctionPanelRight::SetClothingPanelInfo(int clothingIndex, int clothingLayerIndex) 
/* called by (1) ETHuman3DApp::PhantomClothingGenerate, (2) RemoveClothingPanel, (3) SetClothingPanel_inPhantomDelete, (4) SetClothingPanel_inPhantomSelect, (5) slot_ClothingDelete_ButtonClicked,
(6) slot_ClothingSelect_ButtonClicked, (7) slot_ClothingLayerAddingOK_ButtonClicked, (8) slot_ClothingLayerDelete_ButtonClicked, (9) slot_ClothingLayerSelect_ButtonClicked */
{
	ClothingThickness_QLineEdit->setText(QString::number(m_Clothing_MainInfo[m_Phantom_SelectedIndex][clothingIndex][clothingLayerIndex][0])); // thickness
	ClothingDensity_QLineEdit->setText(QString::number(m_Clothing_MainInfo[m_Phantom_SelectedIndex][clothingIndex][clothingLayerIndex][1])); // density

	// Composition
	int CompositionIndex = m_Clothing_MainInfo[m_Phantom_SelectedIndex][clothingIndex][clothingLayerIndex][2];
	ClothingComposition_QComboBox->setCurrentIndex(CompositionIndex);
}
QString FunctionPanelRight::DefinePreWearablePath(int clothingSelectedType_Index) // called by (1) slot_ClothingAddingOK_ButtonClicked, (2) LoadReconsturctionFile_previous
{
	QString strPath = "";
	QString baseDir = "./data/wearable/";
	QString gender, age, posture;
	QString parts;
	QString candidates[] = { "Invalid", "head", "upper_arm", "lower_arm", "hands", "front_trunk", "back_trunk", "pelvis", "upper_leg", "lower_leg", "feet" };

	parts = (clothingSelectedType_Index >= 1 && clothingSelectedType_Index <= 10) ? candidates[clothingSelectedType_Index] : "Invalid";

	if (m_Phantom_MainInfo[m_Phantom_SelectedIndex][1] == 0) // Male
		gender = "M";
	else if (m_Phantom_MainInfo[m_Phantom_SelectedIndex][1] == 1) // Female
		gender = "F";

	if (m_Phantom_MainInfo[m_Phantom_SelectedIndex][12] == 0) // Adult
	{
		age = "A";
	}
	else if (m_Phantom_MainInfo[m_Phantom_SelectedIndex][12] == 1) // 15
	{
		age = "15";
	}
	else if (m_Phantom_MainInfo[m_Phantom_SelectedIndex][12] == 2) // 10
	{
		age = "10";
	}
	else if (m_Phantom_MainInfo[m_Phantom_SelectedIndex][12] == 3) // 5
	{
		age = "05";
	}
	else if (m_Phantom_MainInfo[m_Phantom_SelectedIndex][12] == 4) // 1
	{
		age = "01";
	}
	else if (m_Phantom_MainInfo[m_Phantom_SelectedIndex][12] == 5) // 0
	{
		age = "00";
	}

	if (m_Phantom_MainInfo[m_Phantom_SelectedIndex][2] == E_PHANTOMTYPE_ADULTMRCP || m_Phantom_MainInfo[m_Phantom_SelectedIndex][2] == E_PHANTOMTYPE_PEDIATRICMRCP) // MRCP
	{
		strPath = baseDir + "MRCP_" + age + gender + "/MRCP_" + age + gender + "_" + parts + ".obj";
	}
	if (m_Phantom_MainInfo[m_Phantom_SelectedIndex][2] == E_PHANTOMTYPE_TRANSFORMED) // Posture
	{
		if (m_Phantom_MainInfo[m_Phantom_SelectedIndex][3] == 1) posture = "bending";
		else if (m_Phantom_MainInfo[m_Phantom_SelectedIndex][3] == 2) posture = "kneeling";
		else if (m_Phantom_MainInfo[m_Phantom_SelectedIndex][3] == 3) posture = "sitting";
		else if (m_Phantom_MainInfo[m_Phantom_SelectedIndex][3] == 4) posture = "squatting";
		else if (m_Phantom_MainInfo[m_Phantom_SelectedIndex][3] == 5) posture = "walking";

		strPath = baseDir + "MRCP_" + age + gender + "_" + posture + "/MRCP_" + age + gender + "_" + posture + "_" + parts + ".obj";
	}
	if (m_Phantom_MainInfo[m_Phantom_SelectedIndex][2] == E_PHANTOMTYPE_TRANSFORMED) // Body-size
	{
		strPath = baseDir + theApp.PhantomFileTitle[m_Phantom_SelectedIndex] + "/" + theApp.PhantomFileTitle[m_Phantom_SelectedIndex] + "_" + parts + ".obj";
	}

	if (parts == "Invalid")
	{
		strPath = "Invalid";
	}
	return strPath;
}

// Sub-function (2)
void FunctionPanelRight::moveMaximumScrollbar_clothing() // called by (1) InitializeClothingPanel_InClothingAdd
{
	timer_scroll_clothing->stop();
	ClothingList_QScrollBar->setValue(ClothingList_QScrollBar->maximum());
}
void FunctionPanelRight::moveMaximumScrollbar_clothingLayer() // called by (1) InitializeClothingPanel_InClothingAdd, (2) slot_ClothingLayerAdd_ButtonClicked
{
	timer_scroll_clothingLayer->stop();
	ClothingLayerList_QScrollBar->setValue(ClothingLayerList_QScrollBar->maximum());
}

/////////////////////// Eyewear /////////////////////// 
void FunctionPanelRight::slot_Add_Delete_Eyewear_ButtonClicked()
{
	// Add process
	if (Eyewear_AddDelete_QPushButton->text() == "Add")
	{
		Eyewear_AddDelete_QPushButton->setText("Delete");

		// 여기서 AddingSetting OK까지 이루어짐
		EyewearAddingSetting();
	}

	// Delete process

	if (Eyewear_AddDelete_QPushButton->text() == "Delete")
	{
		Eyewear_AddDelete_QPushButton->setText("Add");
	}
}
void FunctionPanelRight::EyewearAddingSetting()
{
	m_Is_EyewearSetting_OKClicked = false;

	// Create a new dialog
	EyewearAdding_UserQDialog = new MultipleUIDialog(this);
	EyewearAdding_UserQDialog->setStyleSheet("background-color: rgb(205, 222, 243);");
	EyewearAdding_UserQDialog->setFixedWidth(Window_width * 0.3);
	EyewearAdding_UserQDialog->setFixedHeight(Window_height * 0.45);
	EyewearAdding_UserQDialog->setWindowTitle("Eyewear settings");
	EyewearAdding_UserQDialog->setFocusPolicy(Qt::StrongFocus);
	QScrollArea* scrollArea = new QScrollArea(EyewearAdding_UserQDialog);
	scrollArea->setWidgetResizable(true);
	scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	QWidget* containerWidget = new QWidget;
	QVBoxLayout* layout = new QVBoxLayout(containerWidget);

	{// Type
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);
		hLayout->setSpacing(Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QLabel* label0 = new QLabel;
		label0->setFixedSize(Window_width * 0.090, Window_height * 0.028);
		label0->setStyleSheet(DialogStyle::DATA_LABEL);
		label0->setFont(font_D_LBL1);
		label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		label0->setText(QString::fromLocal8Bit("Type:"));

		EyewearSetting_Type_QComboBox = new QComboBox;
		EyewearSetting_Type_QComboBox->setFixedSize(Window_width * 0.170, Window_height * 0.028);
		EyewearSetting_Type_QComboBox->setStyleSheet(DialogStyle::COMBOBOX);
		EyewearSetting_Type_QComboBox->setFont(font_D_CB1);
		EyewearSetting_Type_QComboBox->addItem("Glasses");
		EyewearSetting_Type_QComboBox->addItem("Wraparound Goggle");
		EyewearSetting_Type_QComboBox->setCurrentIndex(0);
		EyewearSetting_Type_QComboBox->setFocusPolicy(Qt::StrongFocus); // For focusing when scroll the comobox

		connect(EyewearSetting_Type_QComboBox, SIGNAL(activated(int)), this, SLOT(slot_EyewearSetting_Type_ComboChanged(int)));

		hLayout->addWidget(label0);
		hLayout->addWidget(EyewearSetting_Type_QComboBox);

		layout->addLayout(hLayout);
	}
	{// Distance
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);
		hLayout->setSpacing(Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QLabel* label0 = new QLabel;
		label0->setFixedSize(Window_width * 0.090, Window_height * 0.028);
		label0->setStyleSheet(DialogStyle::DATA_LABEL);
		label0->setFont(font_D_LBL1);
		label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		label0->setText(QString::fromLocal8Bit("Distance:"));

		EyewearSetting_Distance_QLineEdit = new QLineEdit;
		EyewearSetting_Distance_QLineEdit->setFixedSize(Window_width * 0.170, Window_height * 0.028);
		EyewearSetting_Distance_QLineEdit->setAlignment(Qt::AlignCenter);
		EyewearSetting_Distance_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		EyewearSetting_Distance_QLineEdit->setFont(font_D_LE1);
		EyewearSetting_Distance_QLineEdit->setPlaceholderText("cm");

		hLayout->addWidget(label0);
		hLayout->addWidget(EyewearSetting_Distance_QLineEdit);

		layout->addLayout(hLayout);
	}
	{// Radius 
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);
		hLayout->setSpacing(Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QLabel* label0 = new QLabel;
		label0->setFixedSize(Window_width * 0.090, Window_height * 0.028);
		label0->setStyleSheet(DialogStyle::DATA_LABEL);
		label0->setFont(font_D_LBL1);
		label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		label0->setText(QString::fromLocal8Bit("Radius:"));

		EyewearSetting_Radius_QLineEdit = new QLineEdit;
		EyewearSetting_Radius_QLineEdit->setFixedSize(Window_width * 0.170, Window_height * 0.028);
		EyewearSetting_Radius_QLineEdit->setAlignment(Qt::AlignCenter);
		EyewearSetting_Radius_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		EyewearSetting_Radius_QLineEdit->setFont(font_D_LE1);
		EyewearSetting_Radius_QLineEdit->setPlaceholderText("cm");

		hLayout->addWidget(label0);
		hLayout->addWidget(EyewearSetting_Radius_QLineEdit);

		layout->addLayout(hLayout);
	}
	{// Thickness
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);
		hLayout->setSpacing(Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		EyewearSetting_Thickness_QLabel = new QLabel;
		EyewearSetting_Thickness_QLabel->setFixedSize(Window_width * 0.090, Window_height * 0.028);
		EyewearSetting_Thickness_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		EyewearSetting_Thickness_QLabel->setFont(font_D_LBL1);
		EyewearSetting_Thickness_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		EyewearSetting_Thickness_QLabel->setText(QString::fromLocal8Bit("Thickness:"));

		EyewearSetting_Thickness_QLineEdit = new QLineEdit;
		EyewearSetting_Thickness_QLineEdit->setFixedSize(Window_width * 0.170, Window_height * 0.028);
		EyewearSetting_Thickness_QLineEdit->setAlignment(Qt::AlignCenter);
		EyewearSetting_Thickness_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		EyewearSetting_Thickness_QLineEdit->setFont(font_D_LE1);
		EyewearSetting_Thickness_QLineEdit->setPlaceholderText("cm");

		hLayout->addWidget(EyewearSetting_Thickness_QLabel);
		hLayout->addWidget(EyewearSetting_Thickness_QLineEdit);

		layout->addLayout(hLayout);
	}
	{// Thickness_Front
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);
		hLayout->setSpacing(Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		EyewearSetting_Thickness_Front_QLabel = new QLabel;
		EyewearSetting_Thickness_Front_QLabel->setFixedSize(Window_width * 0.090, Window_height * 0.028);
		EyewearSetting_Thickness_Front_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		EyewearSetting_Thickness_Front_QLabel->setFont(font_D_LBL1);
		EyewearSetting_Thickness_Front_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		EyewearSetting_Thickness_Front_QLabel->setText(QString::fromLocal8Bit("Front Thickness:"));
		EyewearSetting_Thickness_Front_QLabel->hide();

		EyewearSetting_Thickness_Front_QLineEdit = new QLineEdit;
		EyewearSetting_Thickness_Front_QLineEdit->setFixedSize(Window_width * 0.170, Window_height * 0.028);
		EyewearSetting_Thickness_Front_QLineEdit->setAlignment(Qt::AlignCenter);
		EyewearSetting_Thickness_Front_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		EyewearSetting_Thickness_Front_QLineEdit->setFont(font_D_LE1);
		EyewearSetting_Thickness_Front_QLineEdit->setPlaceholderText("cm");
		EyewearSetting_Thickness_Front_QLineEdit->hide();

		hLayout->addWidget(EyewearSetting_Thickness_Front_QLabel);
		hLayout->addWidget(EyewearSetting_Thickness_Front_QLineEdit);

		layout->addLayout(hLayout);
	}
	{// Thickness_Side
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);
		hLayout->setSpacing(Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		EyewearSetting_Thickness_Side_QLabel = new QLabel;
		EyewearSetting_Thickness_Side_QLabel->setFixedSize(Window_width * 0.090, Window_height * 0.028);
		EyewearSetting_Thickness_Side_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		EyewearSetting_Thickness_Side_QLabel->setFont(font_D_LBL1);
		EyewearSetting_Thickness_Side_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		EyewearSetting_Thickness_Side_QLabel->setText(QString::fromLocal8Bit("Side Thickness:"));
		EyewearSetting_Thickness_Side_QLabel->hide();

		EyewearSetting_Thickness_Side_QLineEdit = new QLineEdit;
		EyewearSetting_Thickness_Side_QLineEdit->setFixedSize(Window_width * 0.170, Window_height * 0.028);
		EyewearSetting_Thickness_Side_QLineEdit->setAlignment(Qt::AlignCenter);
		EyewearSetting_Thickness_Side_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		EyewearSetting_Thickness_Side_QLineEdit->setFont(font_D_LE1);
		EyewearSetting_Thickness_Side_QLineEdit->setPlaceholderText("cm");
		EyewearSetting_Thickness_Side_QLineEdit->hide();

		hLayout->addWidget(EyewearSetting_Thickness_Side_QLabel);
		hLayout->addWidget(EyewearSetting_Thickness_Side_QLineEdit);

		layout->addLayout(hLayout);
	}
	{// Density
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);
		hLayout->setSpacing(Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QLabel* label0 = new QLabel;
		label0->setFixedSize(Window_width * 0.090, Window_height * 0.028);
		label0->setStyleSheet(DialogStyle::DATA_LABEL);
		label0->setFont(font_D_LBL1);
		label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		label0->setText(QString::fromLocal8Bit("Density:"));

		EyewearSetting_Density_QLineEdit = new QLineEdit;
		EyewearSetting_Density_QLineEdit->setFixedSize(Window_width * 0.170, Window_height * 0.028);
		EyewearSetting_Density_QLineEdit->setAlignment(Qt::AlignCenter);
		EyewearSetting_Density_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		EyewearSetting_Density_QLineEdit->setFont(font_D_LE1);
		EyewearSetting_Density_QLineEdit->setPlaceholderText("cm");

		hLayout->addWidget(label0);
		hLayout->addWidget(EyewearSetting_Density_QLineEdit);
		 
		layout->addLayout(hLayout);
	}
	{// Composition
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);
		hLayout->setSpacing(Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QLabel* label0 = new QLabel;
		label0->setFixedSize(Window_width * 0.090, Window_height * 0.028);
		label0->setStyleSheet(DialogStyle::DATA_LABEL);
		label0->setFont(font_D_LBL1);
		label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		label0->setText(QString::fromLocal8Bit("Composition:"));

		EyewearSetting_Composition_QComboBox = new QComboBox;
		EyewearSetting_Composition_QComboBox->setFixedSize(Window_width * 0.170, Window_height * 0.028);
		EyewearSetting_Composition_QComboBox->setStyleSheet(DialogStyle::COMBOBOX);
		EyewearSetting_Composition_QComboBox->setFont(font_D_CB1);
		EyewearSetting_Composition_QComboBox->addItem("Lead");
		EyewearSetting_Composition_QComboBox->addItem("Glass");
		EyewearSetting_Composition_QComboBox->setCurrentIndex(0);
		EyewearSetting_Composition_QComboBox->setFocusPolicy(Qt::StrongFocus); // For focusing when scroll the comobox
		
		connect(EyewearSetting_Composition_QComboBox, SIGNAL(activated(int)), this, SLOT(slot_EyewearSetting_Composition_ComboChanged(int)));

		hLayout->addWidget(label0);
		hLayout->addWidget(EyewearSetting_Composition_QComboBox);

		layout->addLayout(hLayout);
	}
	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		hLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);

		EyewearSetting_AddingOK_QPushButton = new QPushButton(QString::fromLocal8Bit("OK"));
		Buttons_FunctionPanelRight.append(EyewearSetting_AddingOK_QPushButton);
		EyewearSetting_AddingOK_QPushButton->setFixedSize(Window_width * 0.104, Window_height * 0.028);
		EyewearSetting_AddingOK_QPushButton->setCheckable(false);
		EyewearSetting_AddingOK_QPushButton->setStyleSheet(DialogStyle::MENU_BTN3);
		EyewearSetting_AddingOK_QPushButton->setFont(font_D_BTN3);

		connect(EyewearSetting_AddingOK_QPushButton, SIGNAL(clicked()), this, SLOT(slot_EyewearSetting_AddingOK_ButtonClicked())); connect(EyewearSetting_AddingOK_QPushButton, SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));

		hLayout->addWidget(EyewearSetting_AddingOK_QPushButton);

		layout->addLayout(hLayout);
	}
	scrollArea->setWidget(containerWidget);
	QVBoxLayout* dialogLayout = new QVBoxLayout(EyewearAdding_UserQDialog);
	dialogLayout->addWidget(scrollArea);
	EyewearAdding_UserQDialog->setLayout(dialogLayout);
	EyewearAdding_UserQDialog->adjustSize();

	EyewearAdding_UserQDialog->exec(); // Use exec() for a modal dialog or show() for a non-modal dialog
}
void FunctionPanelRight::slot_Glasses3DPick_ButtonClicked()
{
	if (Eyewear_3DPick_QPushButton->isChecked()) theApp.m_GlassesPick3D = true;
	else theApp.m_GlassesPick3D = false;

	if (theApp.m_GlassesPick3D) // pick3D 상태일떄
	{
		theApp.MouseControl_toCamera();
		theApp.SetMessageBox("Pick right and left glasses position");
	}
	else // pick3D 상태 아닐때 
	{
		if (theApp.Glasses1_Point) theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(theApp.Glasses1_Point);
		if (theApp.Glasses2_Point) theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(theApp.Glasses2_Point);
		theApp.m_pVTKWidget->renderWindow()->Render();

	}
	
}
void FunctionPanelRight::slot_GlassesUpdate_ButtonClicked()
{
	if (!theApp.Glasses1_Point || !theApp.Glasses1_Point)
	{
		theApp.SetMessageBox("Pick the center of eyes to generate glasses!");
		return;
	}
	if (EyewearDistance_QLineEdit->text().toDouble() <= 0 || EyewearRadius_QLineEdit->text().toDouble() <= 0 || EyewearThickness_QLineEdit->text().toDouble() <= 0 || EyewearDensity_QLineEdit->text().toDouble() <= 0)
	{
		theApp.SetMessageBox("Please enter aprropriate galsses parameters!");
		return;
	}
		// Picked point actor 삭제
		theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(theApp.Glasses2_Point);
		theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(theApp.Glasses1_Point);
		theApp.m_pVTKWidget->renderWindow()->Render();
		if (EyewearSetting_Type_QComboBox->currentIndex() == 0) theApp.phantomObjects->PhantomFlatGlassesGenerate();
		if (EyewearSetting_Type_QComboBox->currentIndex() == 1) theApp.phantomObjects->PhantomWraparoundGlassesGenerate();

		theApp.m_GlassesPick3D = false;
		Eyewear_3DPick_QPushButton->setChecked(false);

		//현재는 안경 update 하면 수정불가하게 막기
		//EyewearUpdate_QPushButton->setDisabled(true);
		//EyewearComposition_QComboBox->setDisabled(true);


}
void FunctionPanelRight::slot_EyewearSetting_Type_ComboChanged(int idx)
{
	EyewearSetting_Type_QComboBox->hide(); EyewearSetting_Type_QComboBox->show();	
	if(idx == 0) // Flat glasses
	{ 
		EyewearSetting_Thickness_QLabel->show();
		EyewearSetting_Thickness_Front_QLabel->hide();
		EyewearSetting_Thickness_Side_QLabel->hide();

		EyewearSetting_Thickness_QLineEdit->show();
		EyewearSetting_Thickness_Front_QLineEdit->hide();
		EyewearSetting_Thickness_Side_QLineEdit->hide();
	}
	if (idx == 1) // Wraparound goggles
	{
		EyewearSetting_Thickness_QLabel->hide();
		EyewearSetting_Thickness_Front_QLabel->show();
		EyewearSetting_Thickness_Side_QLabel->show();

		EyewearSetting_Thickness_QLineEdit->hide();
		EyewearSetting_Thickness_Front_QLineEdit->show();
		EyewearSetting_Thickness_Side_QLineEdit->show();
	}
}
void FunctionPanelRight::slot_EyewearSetting_Composition_ComboChanged(int idx)
{
	EyewearSetting_Composition_QComboBox->hide(); EyewearSetting_Composition_QComboBox->show();
}
void FunctionPanelRight::slot_EyewearSetting_AddingOK_ButtonClicked()
{

}

////////////////////////// Dosimeter ////////////////////////// 

// Main function - List
void FunctionPanelRight::slot_DosimeterAdd_ButtonClicked()
{
	m_DosimeterSequenceVector[m_Phantom_SelectedIndex].push_back(m_Dosimeter_Making_Index);
	int DosimeterTotalCount = m_DosimeterSequenceVector[m_Phantom_SelectedIndex].size();
	m_Dosimeter_Selected_Index = m_Dosimeter_Making_Index;

	DosimeterSelect_QPushButton[m_Dosimeter_Selected_Index] = new QPushButton(QString::fromLocal8Bit("Dosimeter_") + QString::number(m_Dosimeter_Selected_Index));
	Buttons_FunctionPanelRight.append(DosimeterSelect_QPushButton[m_Dosimeter_Selected_Index]);
	DosimeterSelect_QPushButton[m_Dosimeter_Selected_Index]->setObjectName(QString::number(m_Dosimeter_Selected_Index));
	DosimeterSelect_QPushButton[m_Dosimeter_Selected_Index]->setFixedSize(Window_width * 0.05, Window_height * 0.040);
	DosimeterSelect_QPushButton[m_Dosimeter_Selected_Index]->setStyleSheet(DialogStyle::MENU_BTN7);
	DosimeterSelect_QPushButton[m_Dosimeter_Selected_Index]->setFont(font_D_BTN14);
	DosimeterSelect_QPushButton[m_Dosimeter_Selected_Index]->setCheckable(true);
	DosimeterSelect_QPushButton[m_Dosimeter_Selected_Index]->setChecked(true);
	connect(DosimeterSelect_QPushButton[m_Dosimeter_Selected_Index], SIGNAL(clicked()), this, SLOT(slot_DosimeterSelect_ButtonClicked())); connect(DosimeterSelect_QPushButton[m_Dosimeter_Selected_Index], SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
	DosimeterList_QHBoxLayout->addWidget(DosimeterSelect_QPushButton[m_Dosimeter_Selected_Index]);

	for (auto itr : m_DosimeterSequenceVector[m_Phantom_SelectedIndex])
	{
		if (itr != m_Dosimeter_Selected_Index)
		{
			DosimeterSelect_QPushButton[itr]->setChecked(false);
		}
	}

	//Dosimeter scorll bar max 이동
	timer_scroll_dosimeter->start(1); // 1ms 이후 이동 -> 그냥 함수에 넣으면 multiplephantombutton 추가가 더 늦어서 최대로 안 감

	if (DosimeterTotalCount == m_Dosimeter_Maximum_Count - 1) DosimeterAdd_QPushButton->setEnabled(false);

	m_Dosimeter_Making_Index++;
}
void FunctionPanelRight::slot_DosimeterDelete_ButtonClicked()
{
	int originalposition = theApp.removeElement(m_DosimeterSequenceVector[m_Phantom_SelectedIndex], m_Dosimeter_Selected_Index); //{0 1 2} -> {0 2}, origianl index는 지워진 1의 자리
	int DosimeterTotalCount = m_DosimeterSequenceVector[m_Phantom_SelectedIndex].size();

	DosimeterAdd_QPushButton->setEnabled(true);

	// Remove dosimeter button
	DosimeterList_QHBoxLayout->removeWidget(DosimeterSelect_QPushButton[m_Dosimeter_Selected_Index]);
	Buttons_FunctionPanelRight.removeOne(DosimeterSelect_QPushButton[m_Dosimeter_Selected_Index]);
	delete DosimeterSelect_QPushButton[m_Dosimeter_Selected_Index];
	DosimeterSelect_QPushButton.erase(m_Dosimeter_Selected_Index);

	// dosimeterinfo 업데이트	
	m_Clothing_MainInfo[m_Phantom_SelectedIndex][1000][m_Dosimeter_Selected_Index][0] = 0; // 해당 dosimeterSelectedIndex의 dosimInfo map의 데이터는 사용 X
	// dosimeter UI 값 업데이트
	if (DosimeterTotalCount == 0) // Dosimeter 개수 0일때 현재 선택된 dosimeterIndex = -1(Invalid)
	{
		m_Dosimeter_Selected_Index = -1;
		DosimeterRadius_QLineEdit->clear();
		DosimeterSeparationDistance_QLineEdit->clear();
		DosimeterPosX_QLineEdit->clear();
		DosimeterPosY_QLineEdit->clear();
		DosimeterPosZ_QLineEdit->clear();
	}
	else
	{
		m_Dosimeter_Selected_Index = m_DosimeterSequenceVector[m_Phantom_SelectedIndex][originalposition]; //1번 index 자리(2번째 자리)가 지워짐 -> 새로 바뀐 벡터의 1번 index의 원소가 새로운 id
		DosimeterRadius_QLineEdit->setText(QString::number(m_Clothing_MainInfo[m_Phantom_SelectedIndex][1000][m_Dosimeter_Selected_Index][1]));
		DosimeterSeparationDistance_QLineEdit->setText(QString::number(m_Clothing_MainInfo[m_Phantom_SelectedIndex][1000][m_Dosimeter_Selected_Index][2]));
		DosimeterPosX_QLineEdit->setText(QString::number(m_Clothing_MainInfo[m_Phantom_SelectedIndex][1000][m_Dosimeter_Selected_Index][3]));
		DosimeterPosY_QLineEdit->setText(QString::number(m_Clothing_MainInfo[m_Phantom_SelectedIndex][1000][m_Dosimeter_Selected_Index][4]));
		DosimeterPosZ_QLineEdit->setText(QString::number(m_Clothing_MainInfo[m_Phantom_SelectedIndex][1000][m_Dosimeter_Selected_Index][5]));
	}
}
void FunctionPanelRight::slot_DosimeterSelect_ButtonClicked()
{
	QPushButton* buttonSender = qobject_cast<QPushButton*>(sender());
	m_Dosimeter_Selected_Index = buttonSender->objectName().toInt();

	for (auto itr : m_DosimeterSequenceVector[m_Phantom_SelectedIndex])
	{
		if (itr != m_Dosimeter_Selected_Index)
		{
			DosimeterSelect_QPushButton[itr]->setChecked(false);
		}
	}
	DosimeterRadius_QLineEdit->setText(QString::number(m_Clothing_MainInfo[m_Phantom_SelectedIndex][1000][m_Dosimeter_Selected_Index][1]));
	DosimeterSeparationDistance_QLineEdit->setText(QString::number(m_Clothing_MainInfo[m_Phantom_SelectedIndex][1000][m_Dosimeter_Selected_Index][2]));
	DosimeterPosX_QLineEdit->setText(QString::number(m_Clothing_MainInfo[m_Phantom_SelectedIndex][1000][m_Dosimeter_Selected_Index][3]));
	DosimeterPosY_QLineEdit->setText(QString::number(m_Clothing_MainInfo[m_Phantom_SelectedIndex][1000][m_Dosimeter_Selected_Index][4]));
	DosimeterPosZ_QLineEdit->setText(QString::number(m_Clothing_MainInfo[m_Phantom_SelectedIndex][1000][m_Dosimeter_Selected_Index][5]));
}
// Main function - Setting

// Main function - Status
void FunctionPanelRight::slot_DosimeterPick3D_ButtonClicked()
{
	theApp.m_DosimeterPick3D = !theApp.m_DosimeterPick3D;

}
void FunctionPanelRight::slot_DosimeterLocationUpdate_ButtonClicked()
{
	double pickedpos[3];
	pickedpos[0] = DosimeterPosX_QLineEdit->text().toDouble();
	pickedpos[1] = DosimeterPosY_QLineEdit->text().toDouble();
	pickedpos[2] = DosimeterPosZ_QLineEdit->text().toDouble();

	double PosX = m_Phantom_MainInfo[m_Phantom_SelectedIndex][5];
	double PosY = m_Phantom_MainInfo[m_Phantom_SelectedIndex][6];
	double PosZ = m_Phantom_MainInfo[m_Phantom_SelectedIndex][7];

	double RotX = m_Phantom_MainInfo[m_Phantom_SelectedIndex][8];
	double RotY = m_Phantom_MainInfo[m_Phantom_SelectedIndex][9];
	double RotZ = m_Phantom_MainInfo[m_Phantom_SelectedIndex][10];
	const double PI = 3.141592 / 180; // 라디안 변환 
	double RadianX = RotX * PI;
	double RadianY = RotY * PI;
	double RadianZ = RotZ * PI;

	// 입력한 picked 좌표(실제 위치)는 pCenter 이동시킨 polydata_base에 맞게끔 되어 있으므로 이 상태에서 pointID만 얻기

	vtkSmartPointer<vtkKdTree> kdTree = vtkSmartPointer<vtkKdTree>::New();
	kdTree->BuildLocatorFromPoints(theApp.m_3DHumanData_MultiplePhantom[m_Phantom_SelectedIndex].polydata_base); // picked 좌표에서 polydata_base의 존재하는 가장 가까운 점을 찾을거임
	double closestPointDist;
	int closestPointID = kdTree->FindClosestPoint(pickedpos[0], pickedpos[1], pickedpos[2], closestPointDist); // 이 ID가 polydata_base 존재하는 점 중 가장 가까운 점의 ID
	double closestPoint[3]; // 이 점이 polydata_base에 존재하는 점 중 picked된 점을 이동/회전을 역적용한뒤의 점과 가장 가까운 점을 찾음
	theApp.m_3DHumanData_MultiplePhantom[m_Phantom_SelectedIndex].polydata_base->GetPoint(closestPointID, closestPoint);

	theApp.m_DosimeterInfo.Dosimeter_PointID = closestPointID;
	
	// 빨간점 만들기 위해 원점이동시킨 상대좌표를 다시 현재 위치로 옮기기, picked 좌표(실제 위치)를 polydata_base에 일치하는 위치로 옮기기(즉, 이동 및 회전 진행)
	Eigen::Vector3d translation(PosX, PosY, PosZ);
	// Create the rotation matrices for each axis
	Eigen::Matrix3d rotation_x;
	rotation_x = Eigen::AngleAxisd(RadianX, Eigen::Vector3d::UnitX());
	Eigen::Matrix3d rotation_y;
	rotation_y = Eigen::AngleAxisd(RadianY, Eigen::Vector3d::UnitY());
	Eigen::Matrix3d rotation_z;
	rotation_z = Eigen::AngleAxisd(RadianZ, Eigen::Vector3d::UnitZ());
	// Combine the rotation matrices
	Eigen::Matrix3d rotation = rotation_z * rotation_x * rotation_y; //Rotation Z -> X -> Y

	// Create the combined transformation matrix
	Eigen::Matrix4d transform = Eigen::Matrix4d::Identity();
	transform.block<3, 3>(0, 0) = rotation; // rotation -> scaling
	Eigen::Vector4d point;
	point = Eigen::Vector4d(pickedpos[0], pickedpos[1], pickedpos[2], 1.0);
	point = transform * point; // rot -> scaling
	point.head<3>() += translation.head<3>(); // translation2
	double ReversedTransformedPicekdPos[3]; // 입력한 좌표는 팬텀_base (0,0,0)에 대한 상대좌표이므로, 이동 및 회전 변환 해주어야 함.
	ReversedTransformedPicekdPos[0] = point[0];
	ReversedTransformedPicekdPos[1] = point[1];
	ReversedTransformedPicekdPos[2] = point[2];
	theApp.phantomObjects->RefreshDosimeter3DShpere(ReversedTransformedPicekdPos, m_Dosimeter_Selected_Index);
}
void FunctionPanelRight::slot_DosimeterUpdate_ButtonClicked()
{
	theApp.phantomObjects->RemoveDosimeter3DShpere();

	theApp.m_DosimeterPick3D = !theApp.m_DosimeterPick3D;

	theApp.phantomObjects->DosimeterGenerate(m_Dosimeter_Selected_Index);
}

// Sub-function (1)
void FunctionPanelRight::moveMaximumScrollbar_dosimeter() // called by (1) slot_DosimeterAdd_ButtonClicked
{
	timer_scroll_dosimeter->stop();
	DosimeterList_QScrollBar->setValue(DosimeterList_QScrollBar->maximum());
}

//************************************** Geometry Widget **************************************//
void FunctionPanelRight::CreateGeometryWidgets()
{
	// 그룹박스생성
	GeometryPanel_QGroupBox = new QGroupBox();
	GeometryPanel_QGroupBox->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	GeometryPanel_QGroupBox->setStyleSheet(DialogStyle::GROUPBOX_WIDGET7);
	GeometryPanel_QGroupBox->setFont(font_D_GBW7);
	GeometryPanel_QGroupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	QVBoxLayout* layout = new QVBoxLayout;
	layout->setContentsMargins(Window_width * 0.003, Window_height * 0.005, Window_width * 0.003, Window_height * 0.005);
	layout->setSpacing(Window_width * 0.003);
	layout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

	GeometryPanel_QGroupBox->setLayout(layout);
	
	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(0, Window_height * 0.009, 0, Window_height * 0.009);

		QLabel* labelText = new QLabel;
		labelText->setFixedSize(Window_width * 0.07, Window_height * 0.028);
		labelText->setStyleSheet(DialogStyle::DATA_LABEL);
		labelText->setFont(font_D_LBL1);
		labelText->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		labelText->setText(QString::fromLocal8Bit("Environment: "));

		Geometry_EnvironmentType_QComboBox = new QComboBox;
		Geometry_EnvironmentType_QComboBox->setFixedSize(Window_width * 0.1, Window_height * 0.028);
		Geometry_EnvironmentType_QComboBox->setStyleSheet(DialogStyle::COMBOBOX2);
		Geometry_EnvironmentType_QComboBox->setFont(font_D_CB2);
		
		Geometry_EnvironmentType_QComboBox->setEditable(true);
		Geometry_EnvironmentType_QComboBox->lineEdit()->setReadOnly(true);
		Geometry_EnvironmentType_QComboBox->lineEdit()->setAlignment(Qt::AlignCenter);
		Geometry_EnvironmentType_QComboBox->lineEdit()->setFont(font_D_CB2);

		Geometry_EnvironmentType_QComboBox->addItem("Vacuum");
		Geometry_EnvironmentType_QComboBox->addItem("Air");
		Geometry_EnvironmentType_QComboBox->addItem("Water");

		Geometry_EnvironmentType_QComboBox->setCurrentIndex(1); // Air is Default

		connect(Geometry_EnvironmentType_QComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_ChangeGeometeryEnvironment(int)));

		hLayout->addWidget(labelText);
		hLayout->addWidget(Geometry_EnvironmentType_QComboBox);

		layout->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setContentsMargins(0, Window_width * 0.005, 0, 0);
		hLayout->setSpacing(Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

		QLabel* label = new QLabel;
		label->setFixedSize(Window_width * 0.056, Window_height * 0.028);
		label->setStyleSheet(DialogStyle::DATA_LABEL);
		label->setFont(font_D_LBL1);
		label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		label->setText(QString::fromLocal8Bit("Object"));

		hLayout->addWidget(label);

		layout->addLayout(hLayout);
	}

	{
		QGroupBox* GroupBoxObjectList = new QGroupBox;
		GroupBoxObjectList->setAlignment(Qt::AlignCenter | Qt::AlignLeft);
		GroupBoxObjectList->setStyleSheet(DialogStyle::GROUPBOX_WIDGET7);
		GroupBoxObjectList->setFont(font_D_GBW7);
		GroupBoxObjectList->setFixedHeight(Window_height * 0.060);
		GroupBoxObjectList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed); // 가로, 세로 방향 크기
		GroupBoxObjectList->setContentsMargins(Window_width * 0.001, 0, Window_width * 0.001, 0);
		GroupBoxObjectList->setAutoFillBackground(true); 

		layout->addWidget(GroupBoxObjectList);

		QVBoxLayout* layoutOut = new QVBoxLayout();
		layoutOut->setContentsMargins(Window_width * 0.001, 0, Window_width * 0.005, Window_height * 0.005);
		layoutOut->setSpacing(Window_width * 0.005);
		layoutOut->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QGroupBox* GroupBoxIn = new QGroupBox;
		GroupBoxIn->setAlignment(Qt::AlignCenter | Qt::AlignLeft);
		GroupBoxIn->setStyleSheet(DialogStyle::GROUPBOX_WIDGET13);
		GroupBoxIn->setFont(font_D_GBW7);
		GroupBoxIn->setFixedHeight(0.060 * Window_height);
		GroupBoxIn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

		ObjectList_QHBoxLayout = new QHBoxLayout();
		ObjectList_QHBoxLayout->setContentsMargins(Window_width * 0.001, Window_height * 0.005, Window_width * 0.001, Window_height * 0.005); // 왼쪽, 위, 오른쪽, 아래의 여유공간(해당 layout 안에서의)
		ObjectList_QHBoxLayout->setSpacing(Window_width * 0.003);
		ObjectList_QHBoxLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		MultipleButtonScorllArea* scrollArea = new MultipleButtonScorllArea();
		scrollArea->setStyleSheet(DialogStyle::GROUPBOX_WIDGET7);
		scrollArea->setFont(font_D_GBW7);
		scrollArea->setFixedHeight(Window_height * 0.060);
		scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // Do not show vertical scrollbar
		ObjectList_QScrollBar = scrollArea->horizontalScrollBar();

		GroupBoxIn->setLayout(ObjectList_QHBoxLayout);
		scrollArea->setWidget(GroupBoxIn);
		scrollArea->setWidgetResizable(true);
		layoutOut->addWidget(scrollArea);
		GroupBoxObjectList->setLayout(layoutOut);
		GroupBoxObjectList->hide();
		GroupBoxObjectList->show();
	}
	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		hLayout->setContentsMargins(0, Window_height * 0.005, Window_width * 0.010, Window_height * 0.005);

		ObjectAdd_QPushButton = new QPushButton(QString::fromLocal8Bit("Add"));
		Buttons_FunctionPanelRight.append(ObjectAdd_QPushButton);
		ObjectAdd_QPushButton->setFixedSize(Window_width * 0.030, Window_height * 0.025);
		ObjectAdd_QPushButton->setStyleSheet(DialogStyle::MENU_BTN2);
		ObjectAdd_QPushButton->setFont(font_D_BTN2);
		ObjectAdd_QPushButton->setCheckable(false);

		ObjectDelete_QPushButton = new QPushButton(QString::fromLocal8Bit("Delete"));
		Buttons_FunctionPanelRight.append(ObjectDelete_QPushButton);
		ObjectDelete_QPushButton->setFixedSize(Window_width * 0.030, Window_height * 0.025);
		ObjectDelete_QPushButton->setStyleSheet(DialogStyle::MENU_BTN2);
		ObjectDelete_QPushButton->setFont(font_D_BTN2);
		ObjectDelete_QPushButton->setCheckable(false);

		// 버튼생성
		ObjectMoveLeft_UserQPushButton = new LeftButton(this);
		Buttons_FunctionPanelRight.append(ObjectMoveLeft_UserQPushButton);
		ObjectMoveLeft_UserQPushButton->setFixedSize(Window_width * 0.012, Window_height * 0.025);
		// Apply stylesheet for color and shape
		ObjectMoveLeft_UserQPushButton->setStyleSheet("QPushButton {"
			"background-color: white;"
			"border: 0px;" // Half of the width/height for circular shape
			"}");

		// 버튼생성
		ObjectMoveRight_UserQPushButton = new RightButton(this);
		Buttons_FunctionPanelRight.append(ObjectMoveRight_UserQPushButton);
		ObjectMoveRight_UserQPushButton->setFixedSize(Window_width * 0.012, Window_height * 0.025);
		// Apply stylesheet for color and shape
		ObjectMoveRight_UserQPushButton->setStyleSheet("QPushButton {"
			"background-color: white;"
			"border: 0px;" // Half of the width/height for circular shape
			"}");

		connect(ObjectAdd_QPushButton, SIGNAL(clicked()), this, SLOT(slot_ObjectAdd_ButtonClicked())); connect(ObjectAdd_QPushButton, SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
		connect(ObjectDelete_QPushButton, SIGNAL(clicked()), this, SLOT(slot_ObjectDelete_ButtonClicked())); connect(ObjectDelete_QPushButton, SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
		connect(ObjectMoveLeft_UserQPushButton, SIGNAL(clicked()), this, SLOT(slot_ObjectMoveLeft_ButtonClicked())); connect(ObjectMoveLeft_UserQPushButton, SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
		connect(ObjectMoveRight_UserQPushButton, SIGNAL(clicked()), this, SLOT(slot_ObjectMoveRight_ButtonClicked())); connect(ObjectMoveRight_UserQPushButton, SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));


		hLayout->addWidget(ObjectAdd_QPushButton);
		hLayout->addWidget(ObjectDelete_QPushButton);
		/*hLayout->addWidget(ObjectMoveLeft_UserQPushButton);
		hLayout->addWidget(ObjectMoveRight_UserQPushButton);*/


		layout->addLayout(hLayout);
	}
	// Create Start!
	CreateObjectBoxWidgets(layout);
	CreateObjectSphereWidgets(layout);
	CreateObjectCylinderWidgets(layout);

	QGroupBox* GroupBoxIn = new QGroupBox;
	GroupBoxIn->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	GroupBoxIn->setStyleSheet(DialogStyle::GROUPBOX_WIDGET7);
	GroupBoxIn->setFont(font_D_GBW7);
	GroupBoxIn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	QGroupBox* GroupBoxOut = new QGroupBox;
	GroupBoxOut->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	GroupBoxOut->setStyleSheet(DialogStyle::GROUPBOX_WIDGET7);
	GroupBoxOut->setFont(font_D_GBW7);
	GroupBoxOut->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	QVBoxLayout* layoutIn = new QVBoxLayout();
	layoutIn->setContentsMargins(Window_width * 0.003, 0, Window_width * 0.003, 0);
	layoutIn->setSpacing(Window_width * 0.003);
	layoutIn->setAlignment(Qt::AlignLeft | Qt::AlignTop);

	QVBoxLayout* layoutOut = new QVBoxLayout();
	layoutOut->setContentsMargins(Window_width * 0.003, 0, Window_width * 0.003, 0);
	layoutOut->setSpacing(Window_width * 0.003);
	layoutOut->setAlignment(Qt::AlignLeft | Qt::AlignTop);

	QScrollArea* scrollArea = new QScrollArea();
	scrollArea->setStyleSheet(DialogStyle::GROUPBOX_WIDGET7);
	scrollArea->setFont(font_D_GBW7);
	scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	GroupBoxIn->setLayout(layoutIn);
	scrollArea->setWidget(GroupBoxIn);
	scrollArea->setWidgetResizable(true);
	layoutOut->addWidget(scrollArea);
	GroupBoxOut->setLayout(layoutOut);
	layout->addWidget(GroupBoxOut);

	theApp.LoadingBar->setFormat("Constructing Geometry Panel");
	theApp.LoadingBar->setValue(30);

	theApp.LoadingBar->setFormat("Constructing Geometry Panel");
	theApp.LoadingBar->setValue(60);	

	SetInitialObjectMode();
}

////////////////// Environment ////////////////// 
void FunctionPanelRight::slot_ChangeGeometeryEnvironment(int idx)
{

}

////////////////// All Objects ////////////////// 

// Main function - List
void FunctionPanelRight::slot_ObjectAdd_ButtonClicked()
{
	// == Adding setting dialog 실행 == 
	ObjectAddingSetting_Create();
	ObjectAdding_UserQDialog->exec(); // Use exec() for a modal dialog or show() for a non-modal dialog
	// == Adding setting 완료 == 

	// == Add 루틴 ==
	ExecuteObjectAddRoutines();	
}
void FunctionPanelRight::slot_ObjectDelete_ButtonClicked()
{
	if (m_Object_SequenceVector.size() < 1)
	{
		theApp.SetMessageBox("There is no obejct to delete!");
		return;
	}

	// 기존 버튼 지우기	
	int originalposition = theApp.removeElement(m_Object_SequenceVector, m_Object_SelectedIndex); //{0 1 2} -> {0 2}, origianl index는 지워진 1의 자리	
	ObjectList_QHBoxLayout->removeWidget(ObjectSelect_QPushButton[m_Object_SelectedIndex]);
	Buttons_FunctionPanelRight.removeOne(ObjectSelect_QPushButton[m_Object_SelectedIndex]);
	delete ObjectSelect_QPushButton[m_Object_SelectedIndex];
	ObjectSelect_QPushButton.erase(m_Object_SelectedIndex);

	// Remove actor
	theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(theApp.ObjectPanelActors[m_Object_SelectedIndex]);
	theApp.m_pVTKWidget->renderWindow()->Render();
	theApp.ObjectPanelActors[m_Object_SelectedIndex] = nullptr;

	///////////////////////////// remove 완료, 새로 만들기///////////////////

	// SelectedIndex 설정
	{
		if (m_Object_SequenceVector.size() == 0) m_Object_SelectedIndex = -1; // Phantom 개수 0일때 현재 선택된 index = -1(Invalid)로 설정
		else m_Object_SelectedIndex = m_Object_SequenceVector[originalposition]; // 새로운 인덱스로 설정
	}

	// Object mode 설정
	{
		UpdateObject_Mode(m_Object_SelectedIndex);
	}	

	// list 버튼 업데이트
	{
		UpdateObject_ListButton(m_Object_SelectedIndex);
	}

	// InfoStatus 업데이트
	{
		UpdateObject_InfoStatus(m_Object_SelectedIndex); // Info Status창 업데이트
	}

	// actor 업데이트
	{
		theApp.UpdateObject_ActorHighlighted(m_Object_SelectedIndex); // 현재 선택된 객체만 highlight
	}

	
}
void FunctionPanelRight::slot_ObjectSelect_ButtonClicked()
{
	QPushButton* buttonSender = qobject_cast<QPushButton*>(sender());
	// 원래 버튼 클릭 시 다시 check하고 돌아가기
	if (buttonSender->objectName().toInt() == m_Object_SelectedIndex)
	{
		ObjectSelect_QPushButton[m_Object_SelectedIndex]->setChecked(true);
		return;
	}

	m_Object_SelectedIndex = buttonSender->objectName().toInt();
	int id = m_Object_SelectedIndex;

	// Object mode 설정
	UpdateObject_Mode(id);

	// Object list 버튼 업데이트
	UpdateObject_ListButton(id);

	// Object actor 업데이트
	theApp.UpdateObject_ActorHighlighted(id);

	// Object info status 업데이트
	UpdateObject_InfoStatus(id);
}
void FunctionPanelRight::slot_ObjectMoveLeft_ButtonClicked()
{
	if (m_Object_SequenceVector.size() < 2 ) return; // 객체가 2개 미만이면 함수를 즉시 종료합니다.

	int swappedIndex = theApp.transposeElement(m_Object_SequenceVector, m_Object_SelectedIndex, "left");
	bool removeButton = false;
	for (auto itr_ObjectSequence : m_Object_SequenceVector)
	{
		if (itr_ObjectSequence == swappedIndex) removeButton = true;
		if (removeButton)
		{
			ObjectList_QHBoxLayout->removeWidget(ObjectSelect_QPushButton[itr_ObjectSequence]);
			Buttons_FunctionPanelRight.removeOne(ObjectSelect_QPushButton[itr_ObjectSequence]);
			delete ObjectSelect_QPushButton[itr_ObjectSequence];
			ObjectSelect_QPushButton.erase(itr_ObjectSequence);
		}
	}
	bool generateButton = false;
	for (auto itr_ObjectSequence : m_Object_SequenceVector)
	{
		if (itr_ObjectSequence == swappedIndex) generateButton = true;
		if (generateButton)
		{
			ObjectSelect_QPushButton[itr_ObjectSequence] = new QPushButton(m_Object_ButtonName[itr_ObjectSequence]);
			Buttons_FunctionPanelRight.append(ObjectSelect_QPushButton[itr_ObjectSequence]);
			ObjectSelect_QPushButton[itr_ObjectSequence]->setObjectName(QString::number(itr_ObjectSequence));
			ObjectSelect_QPushButton[itr_ObjectSequence]->setFixedSize(Window_width * 0.05, Window_height * 0.040);
			ObjectSelect_QPushButton[itr_ObjectSequence]->setStyleSheet(DialogStyle::MENU_BTN7);
			ObjectSelect_QPushButton[itr_ObjectSequence]->setFont(font_D_BTN14);
			setElidedButtonText(ObjectSelect_QPushButton[itr_ObjectSequence]); // setFont 뒤에 해야 함!
			ObjectSelect_QPushButton[itr_ObjectSequence]->setCheckable(true);
			connect(ObjectSelect_QPushButton[itr_ObjectSequence], SIGNAL(clicked()), this, SLOT(slot_ObjectSelect_ButtonClicked())); connect(ObjectSelect_QPushButton[itr_ObjectSequence], SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
			ObjectList_QHBoxLayout->addWidget(ObjectSelect_QPushButton[itr_ObjectSequence]);
		}
	}
	ObjectSelect_QPushButton[m_Object_SelectedIndex]->setChecked(true);
}
void FunctionPanelRight::slot_ObjectMoveRight_ButtonClicked()
{
	if (m_Object_SequenceVector.size() < 2) return; // 객체가 2개 미만이면 함수를 즉시 종료합니다.

	int swappedIndex = theApp.transposeElement(m_Object_SequenceVector, m_Object_SelectedIndex, "right");
	bool removeButton = false;
	for (auto itr_ObjectSequence : m_Object_SequenceVector)
	{
		if (itr_ObjectSequence == m_Object_SelectedIndex) removeButton = true;
		if (removeButton)
		{
			ObjectList_QHBoxLayout->removeWidget(ObjectSelect_QPushButton[itr_ObjectSequence]);
			Buttons_FunctionPanelRight.removeOne(ObjectSelect_QPushButton[itr_ObjectSequence]);
			delete ObjectSelect_QPushButton[itr_ObjectSequence];
			ObjectSelect_QPushButton.erase(itr_ObjectSequence);
		}
	}
	bool generateButton = false;
	for (auto itr_ObjectSequence : m_Object_SequenceVector)
	{
		if (itr_ObjectSequence == m_Object_SelectedIndex) generateButton = true;
		if (generateButton)
		{
			ObjectSelect_QPushButton[itr_ObjectSequence] = new QPushButton(m_Object_ButtonName[itr_ObjectSequence]);
			Buttons_FunctionPanelRight.append(ObjectSelect_QPushButton[itr_ObjectSequence]);
			ObjectSelect_QPushButton[itr_ObjectSequence]->setObjectName(QString::number(itr_ObjectSequence));
			ObjectSelect_QPushButton[itr_ObjectSequence]->setFixedSize(Window_width * 0.05, Window_height * 0.040);
			ObjectSelect_QPushButton[itr_ObjectSequence]->setStyleSheet(DialogStyle::MENU_BTN7);
			ObjectSelect_QPushButton[itr_ObjectSequence]->setFont(font_D_BTN14);
			setElidedButtonText(ObjectSelect_QPushButton[itr_ObjectSequence]); // setFont 뒤에 해야 함!
			ObjectSelect_QPushButton[itr_ObjectSequence]->setCheckable(true);
			connect(ObjectSelect_QPushButton[itr_ObjectSequence], SIGNAL(clicked()), this, SLOT(slot_ObjectSelect_ButtonClicked())); connect(ObjectSelect_QPushButton[itr_ObjectSequence], SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
			ObjectList_QHBoxLayout->addWidget(ObjectSelect_QPushButton[itr_ObjectSequence]);
		}
	}
	ObjectSelect_QPushButton[m_Object_SelectedIndex]->setChecked(true);
}

// Main function - Adding Setting
void FunctionPanelRight::ObjectAddingSetting_Create()
{
	b_IsObjectSettingOKClicked = false;

	// Create a new dialog
	ObjectAdding_UserQDialog = new MultipleUIDialog(this);
	ObjectAdding_UserQDialog->setStyleSheet("background-color: rgb(185, 215, 196);");
	ObjectAdding_UserQDialog->setFixedWidth(Window_width * 0.3);
	ObjectAdding_UserQDialog->setFixedHeight(Window_height * 0.55);
	ObjectAdding_UserQDialog->setWindowTitle("Object settings");
	ObjectAdding_UserQDialog->setFocusPolicy(Qt::StrongFocus);
	QScrollArea* scrollArea = new QScrollArea(ObjectAdding_UserQDialog);
	scrollArea->setWidgetResizable(true);
	scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	QWidget* containerWidget = new QWidget;
	QVBoxLayout* layout = new QVBoxLayout(containerWidget);

	{// Type
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);
		hLayout->setSpacing(Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QLabel* label0 = new QLabel;
		label0->setFixedSize(Window_width * 0.090, Window_height * 0.028);
		label0->setStyleSheet(DialogStyle::DATA_LABEL);
		label0->setFont(font_D_LBL1);
		label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		label0->setText(QString::fromLocal8Bit("Type:"));

		ObjectSetting_Type_QComboBox = new QComboBox;
		ObjectSetting_Type_QComboBox->setFixedSize(Window_width * 0.105, Window_height * 0.028);
		ObjectSetting_Type_QComboBox->setStyleSheet(DialogStyle::COMBOBOX);
		ObjectSetting_Type_QComboBox->setFont(font_D_CB1);

		ObjectSetting_Type_QComboBox->addItem("Box");
		ObjectSetting_Type_QComboBox->addItem("Sphere");
		ObjectSetting_Type_QComboBox->addItem("Cylinder");
		ObjectSetting_Type_QComboBox->setCurrentIndex(0);

		ObjectSetting_Type_QComboBox->setFocusPolicy(Qt::StrongFocus); // For focusing when scroll the comobox
		connect(ObjectSetting_Type_QComboBox, SIGNAL(activated(int)), this, SLOT(slot_ChangeObjectCombo(int)));

		hLayout->addWidget(label0);
		hLayout->addWidget(ObjectSetting_Type_QComboBox);

		layout->addLayout(hLayout);
	}

	{// 
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);
		hLayout->setSpacing(Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QLabel* label0 = new QLabel;
		label0->setFixedSize(Window_width * 0.090, Window_height * 0.028);
		label0->setStyleSheet(DialogStyle::DATA_LABEL);
		label0->setFont(font_D_LBL1);
		label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		label0->setText(QString::fromLocal8Bit("Object name:"));

		ObjectSetting_Name_QLineEdit = new QLineEdit;
		ObjectSetting_Name_QLineEdit->setFixedSize(Window_width * 0.170, Window_height * 0.028);
		ObjectSetting_Name_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectSetting_Name_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		ObjectSetting_Name_QLineEdit->setFont(font_D_LE1);
		ObjectSetting_Name_QLineEdit->setPlaceholderText("Object_" + QString::number(m_Object_MakingIndex));
		ObjectSetting_Name_QLineEdit->setValidator(ValFactory::makeNoSpace(ObjectSetting_Name_QLineEdit));

		hLayout->addWidget(label0);
		hLayout->addWidget(ObjectSetting_Name_QLineEdit);

		layout->addLayout(hLayout);
	}

	{// 
		ObjectSetting_BoxLength_QHBoxLayout = new QHBoxLayout;
		ObjectSetting_BoxLength_QHBoxLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);
		ObjectSetting_BoxLength_QHBoxLayout->setSpacing(Window_width * 0.003);
		ObjectSetting_BoxLength_QHBoxLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		ObjectSetting_BoxLength_QLabel = new QLabel;
		ObjectSetting_BoxLength_QLabel->setFixedSize(Window_width * 0.090, Window_height * 0.028);
		ObjectSetting_BoxLength_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		ObjectSetting_BoxLength_QLabel->setFont(font_D_LBL1);
		ObjectSetting_BoxLength_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		ObjectSetting_BoxLength_QLabel->setText(QString::fromLocal8Bit("Half length XYZ:"));

		ObjectSetting_BoxLengthX_QLineEdit = new QLineEdit;
		ObjectSetting_BoxLengthX_QLineEdit->setFixedSize(Window_width * 0.0545, Window_height * 0.028);
		ObjectSetting_BoxLengthX_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectSetting_BoxLengthX_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		ObjectSetting_BoxLengthX_QLineEdit->setFont(font_D_LE1);
		ObjectSetting_BoxLengthX_QLineEdit->setPlaceholderText("cm");
		ObjectSetting_BoxLengthX_QLineEdit->setValidator(ValFactory::makePositiveReal(ObjectSetting_BoxLengthX_QLineEdit));

		ObjectSetting_BoxLengthY_QLineEdit = new QLineEdit;
		ObjectSetting_BoxLengthY_QLineEdit->setFixedSize(Window_width * 0.0545, Window_height * 0.028);
		ObjectSetting_BoxLengthY_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectSetting_BoxLengthY_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		ObjectSetting_BoxLengthY_QLineEdit->setFont(font_D_LE1);
		ObjectSetting_BoxLengthY_QLineEdit->setPlaceholderText("cm");
		ObjectSetting_BoxLengthY_QLineEdit->setValidator(ValFactory::makePositiveReal(ObjectSetting_BoxLengthY_QLineEdit));

		ObjectSetting_BoxLengthZ_QLineEdit = new QLineEdit;
		ObjectSetting_BoxLengthZ_QLineEdit->setFixedSize(Window_width * 0.0545, Window_height * 0.028);
		ObjectSetting_BoxLengthZ_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectSetting_BoxLengthZ_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		ObjectSetting_BoxLengthZ_QLineEdit->setFont(font_D_LE1);
		ObjectSetting_BoxLengthZ_QLineEdit->setPlaceholderText("cm");
		ObjectSetting_BoxLengthZ_QLineEdit->setValidator(ValFactory::makePositiveReal(ObjectSetting_BoxLengthZ_QLineEdit));

		ObjectSetting_BoxLength_QHBoxLayout->addWidget(ObjectSetting_BoxLength_QLabel);
		ObjectSetting_BoxLength_QHBoxLayout->addWidget(ObjectSetting_BoxLengthX_QLineEdit);
		ObjectSetting_BoxLength_QHBoxLayout->addWidget(ObjectSetting_BoxLengthY_QLineEdit);
		ObjectSetting_BoxLength_QHBoxLayout->addWidget(ObjectSetting_BoxLengthZ_QLineEdit);

		layout->addLayout(ObjectSetting_BoxLength_QHBoxLayout);
	}

	{// 
		ObjectSetting_BoxThickness_QHBoxLayout = new QHBoxLayout;
		ObjectSetting_BoxThickness_QHBoxLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);
		ObjectSetting_BoxThickness_QHBoxLayout->setSpacing(Window_width * 0.003);
		ObjectSetting_BoxThickness_QHBoxLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		ObjectSetting_BoxThickness_QLabel = new QLabel;
		ObjectSetting_BoxThickness_QLabel->setFixedSize(Window_width * 0.090, Window_height * 0.028);
		ObjectSetting_BoxThickness_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		ObjectSetting_BoxThickness_QLabel->setFont(font_D_LBL1);
		ObjectSetting_BoxThickness_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		ObjectSetting_BoxThickness_QLabel->setText(QString::fromLocal8Bit("Thickness XYZ:"));

		ObjectSetting_BoxThicknessX_QLineEdit = new QLineEdit;
		ObjectSetting_BoxThicknessX_QLineEdit->setFixedSize(Window_width * 0.0545, Window_height * 0.028);
		ObjectSetting_BoxThicknessX_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectSetting_BoxThicknessX_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		ObjectSetting_BoxThicknessX_QLineEdit->setFont(font_D_LE1);
		ObjectSetting_BoxThicknessX_QLineEdit->setPlaceholderText("cm");
		ObjectSetting_BoxThicknessX_QLineEdit->setValidator(ValFactory::makePositiveReal(ObjectSetting_BoxThicknessX_QLineEdit));

		ObjectSetting_BoxThicknessY_QLineEdit = new QLineEdit;
		ObjectSetting_BoxThicknessY_QLineEdit->setFixedSize(Window_width * 0.0545, Window_height * 0.028);
		ObjectSetting_BoxThicknessY_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectSetting_BoxThicknessY_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		ObjectSetting_BoxThicknessY_QLineEdit->setFont(font_D_LE1);
		ObjectSetting_BoxThicknessY_QLineEdit->setPlaceholderText("cm");
		ObjectSetting_BoxThicknessY_QLineEdit->setValidator(ValFactory::makePositiveReal(ObjectSetting_BoxThicknessY_QLineEdit));

		ObjectSetting_BoxThicknessZ_QLineEdit = new QLineEdit;
		ObjectSetting_BoxThicknessZ_QLineEdit->setFixedSize(Window_width * 0.0545, Window_height * 0.028);
		ObjectSetting_BoxThicknessZ_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectSetting_BoxThicknessZ_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		ObjectSetting_BoxThicknessZ_QLineEdit->setFont(font_D_LE1);
		ObjectSetting_BoxThicknessZ_QLineEdit->setPlaceholderText("cm");
		ObjectSetting_BoxThicknessZ_QLineEdit->setValidator(ValFactory::makePositiveReal(ObjectSetting_BoxThicknessZ_QLineEdit));

		ObjectSetting_BoxThickness_QHBoxLayout->addWidget(ObjectSetting_BoxThickness_QLabel);
		ObjectSetting_BoxThickness_QHBoxLayout->addWidget(ObjectSetting_BoxThicknessX_QLineEdit);
		ObjectSetting_BoxThickness_QHBoxLayout->addWidget(ObjectSetting_BoxThicknessY_QLineEdit);
		ObjectSetting_BoxThickness_QHBoxLayout->addWidget(ObjectSetting_BoxThicknessZ_QLineEdit);

		layout->addLayout(ObjectSetting_BoxThickness_QHBoxLayout);
	}

	{// 
		ObjectSetting_BoxCenter_QHBoxLayout = new QHBoxLayout;
		ObjectSetting_BoxCenter_QHBoxLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);
		ObjectSetting_BoxCenter_QHBoxLayout->setSpacing(Window_width * 0.003);
		ObjectSetting_BoxCenter_QHBoxLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		ObjectSetting_BoxCenter_QLabel = new QLabel;
		ObjectSetting_BoxCenter_QLabel->setFixedSize(Window_width * 0.090, Window_height * 0.028);
		ObjectSetting_BoxCenter_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		ObjectSetting_BoxCenter_QLabel->setFont(font_D_LBL1);
		ObjectSetting_BoxCenter_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		ObjectSetting_BoxCenter_QLabel->setText(QString::fromLocal8Bit("Center XYZ:"));

		ObjectSetting_BoxCenterX_QLineEdit = new QLineEdit;
		ObjectSetting_BoxCenterX_QLineEdit->setFixedSize(Window_width * 0.0545, Window_height * 0.028);
		ObjectSetting_BoxCenterX_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectSetting_BoxCenterX_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		ObjectSetting_BoxCenterX_QLineEdit->setFont(font_D_LE1);
		ObjectSetting_BoxCenterX_QLineEdit->setPlaceholderText("cm");
		ObjectSetting_BoxCenterX_QLineEdit->setValidator(ValFactory::makeReal(ObjectSetting_BoxCenterX_QLineEdit));

		ObjectSetting_BoxCenterY_QLineEdit = new QLineEdit;
		ObjectSetting_BoxCenterY_QLineEdit->setFixedSize(Window_width * 0.0545, Window_height * 0.028);
		ObjectSetting_BoxCenterY_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectSetting_BoxCenterY_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		ObjectSetting_BoxCenterY_QLineEdit->setFont(font_D_LE1);
		ObjectSetting_BoxCenterY_QLineEdit->setPlaceholderText("cm");
		ObjectSetting_BoxCenterY_QLineEdit->setValidator(ValFactory::makeReal(ObjectSetting_BoxCenterY_QLineEdit));

		ObjectSetting_BoxCenterZ_QLineEdit = new QLineEdit;
		ObjectSetting_BoxCenterZ_QLineEdit->setFixedSize(Window_width * 0.0545, Window_height * 0.028);
		ObjectSetting_BoxCenterZ_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectSetting_BoxCenterZ_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		ObjectSetting_BoxCenterZ_QLineEdit->setFont(font_D_LE1);
		ObjectSetting_BoxCenterZ_QLineEdit->setPlaceholderText("cm");
		ObjectSetting_BoxCenterZ_QLineEdit->setValidator(ValFactory::makeReal(ObjectSetting_BoxCenterZ_QLineEdit));

		ObjectSetting_BoxCenter_QHBoxLayout->addWidget(ObjectSetting_BoxCenter_QLabel);
		ObjectSetting_BoxCenter_QHBoxLayout->addWidget(ObjectSetting_BoxCenterX_QLineEdit);
		ObjectSetting_BoxCenter_QHBoxLayout->addWidget(ObjectSetting_BoxCenterY_QLineEdit);
		ObjectSetting_BoxCenter_QHBoxLayout->addWidget(ObjectSetting_BoxCenterZ_QLineEdit);

		layout->addLayout(ObjectSetting_BoxCenter_QHBoxLayout);
	}

	{// 
		ObjectSetting_BoxRotate_QHBoxLayout = new QHBoxLayout;
		ObjectSetting_BoxRotate_QHBoxLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);
		ObjectSetting_BoxRotate_QHBoxLayout->setSpacing(Window_width * 0.003);
		ObjectSetting_BoxRotate_QHBoxLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		ObjectSetting_BoxRotate_QLabel = new QLabel;
		ObjectSetting_BoxRotate_QLabel->setFixedSize(Window_width * 0.090, Window_height * 0.028);
		ObjectSetting_BoxRotate_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		ObjectSetting_BoxRotate_QLabel->setFont(font_D_LBL1);
		ObjectSetting_BoxRotate_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		ObjectSetting_BoxRotate_QLabel->setText(QString::fromLocal8Bit("Rotate XYZ:"));

		ObjectSetting_BoxRotateX_QLineEdit = new QLineEdit;
		ObjectSetting_BoxRotateX_QLineEdit->setFixedSize(Window_width * 0.0545, Window_height * 0.028);
		ObjectSetting_BoxRotateX_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectSetting_BoxRotateX_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		ObjectSetting_BoxRotateX_QLineEdit->setFont(font_D_LE1);
		ObjectSetting_BoxRotateX_QLineEdit->setPlaceholderText("deg");
		ObjectSetting_BoxCenterZ_QLineEdit->setValidator(ValFactory::makePositiveReal(ObjectSetting_BoxCenterZ_QLineEdit));

		ObjectSetting_BoxRotateY_QLineEdit = new QLineEdit;
		ObjectSetting_BoxRotateY_QLineEdit->setFixedSize(Window_width * 0.0545, Window_height * 0.028);
		ObjectSetting_BoxRotateY_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectSetting_BoxRotateY_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		ObjectSetting_BoxRotateY_QLineEdit->setFont(font_D_LE1);
		ObjectSetting_BoxRotateY_QLineEdit->setPlaceholderText("deg");
		ObjectSetting_BoxCenterZ_QLineEdit->setValidator(ValFactory::makePositiveReal(ObjectSetting_BoxCenterZ_QLineEdit));

		ObjectSetting_BoxRotateZ_QLineEdit = new QLineEdit;
		ObjectSetting_BoxRotateZ_QLineEdit->setFixedSize(Window_width * 0.0545, Window_height * 0.028);
		ObjectSetting_BoxRotateZ_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectSetting_BoxRotateZ_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		ObjectSetting_BoxRotateZ_QLineEdit->setFont(font_D_LE1);
		ObjectSetting_BoxRotateZ_QLineEdit->setPlaceholderText("deg");
		ObjectSetting_BoxCenterZ_QLineEdit->setValidator(ValFactory::makePositiveReal(ObjectSetting_BoxCenterZ_QLineEdit));

		ObjectSetting_BoxRotate_QHBoxLayout->addWidget(ObjectSetting_BoxRotate_QLabel);
		ObjectSetting_BoxRotate_QHBoxLayout->addWidget(ObjectSetting_BoxRotateX_QLineEdit);
		ObjectSetting_BoxRotate_QHBoxLayout->addWidget(ObjectSetting_BoxRotateY_QLineEdit);
		ObjectSetting_BoxRotate_QHBoxLayout->addWidget(ObjectSetting_BoxRotateZ_QLineEdit);

		layout->addLayout(ObjectSetting_BoxRotate_QHBoxLayout);
	}

	{// 
		ObjectSetting_SphereRadius_QHBoxLayout = new QHBoxLayout;
		ObjectSetting_SphereRadius_QHBoxLayout->setContentsMargins(Window_width * 0.003, 0, Window_width * 0.003, 0);
		ObjectSetting_SphereRadius_QHBoxLayout->setSpacing(Window_width * 0.003);
		ObjectSetting_SphereRadius_QHBoxLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		ObjectSetting_SphereRadius_QLabel = new QLabel;
		ObjectSetting_SphereRadius_QLabel->setFixedSize(Window_width * 0.090, Window_height * 0.028);
		ObjectSetting_SphereRadius_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		ObjectSetting_SphereRadius_QLabel->setFont(font_D_LBL1);
		ObjectSetting_SphereRadius_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		ObjectSetting_SphereRadius_QLabel->setText(QString::fromLocal8Bit("Radius min/max:"));

		ObjectSetting_SphereInnerRadius_QLineEdit = new QLineEdit;
		ObjectSetting_SphereInnerRadius_QLineEdit->setFixedSize(Window_width * 0.0545, Window_height * 0.028);
		ObjectSetting_SphereInnerRadius_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectSetting_SphereInnerRadius_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		ObjectSetting_SphereInnerRadius_QLineEdit->setFont(font_D_LE1);
		ObjectSetting_SphereInnerRadius_QLineEdit->setPlaceholderText("cm");
		ObjectSetting_SphereInnerRadius_QLineEdit->setValidator(ValFactory::makePositiveReal(ObjectSetting_SphereInnerRadius_QLineEdit));

		ObjectSetting_SphereOuterRadius_QLineEdit = new QLineEdit;
		ObjectSetting_SphereOuterRadius_QLineEdit->setFixedSize(Window_width * 0.0545, Window_height * 0.028);
		ObjectSetting_SphereOuterRadius_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectSetting_SphereOuterRadius_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		ObjectSetting_SphereOuterRadius_QLineEdit->setFont(font_D_LE1);
		ObjectSetting_SphereOuterRadius_QLineEdit->setPlaceholderText("cm");
		ObjectSetting_SphereOuterRadius_QLineEdit->setValidator(ValFactory::makePositiveReal(ObjectSetting_SphereOuterRadius_QLineEdit));

		ObjectSetting_SphereRadius_QLabel->hide();
		ObjectSetting_SphereInnerRadius_QLineEdit->hide();
		ObjectSetting_SphereOuterRadius_QLineEdit->hide();

		ObjectSetting_SphereRadius_QHBoxLayout->addWidget(ObjectSetting_SphereRadius_QLabel);
		ObjectSetting_SphereRadius_QHBoxLayout->addWidget(ObjectSetting_SphereInnerRadius_QLineEdit);
		ObjectSetting_SphereRadius_QHBoxLayout->addWidget(ObjectSetting_SphereOuterRadius_QLineEdit);

		layout->addLayout(ObjectSetting_SphereRadius_QHBoxLayout);
	}

	{// 
		ObjectSetting_SphereAzimuthalPhi_QHBoxLayout = new QHBoxLayout;
		ObjectSetting_SphereAzimuthalPhi_QHBoxLayout->setContentsMargins(Window_width * 0.003, 0, Window_width * 0.003, 0);
		ObjectSetting_SphereAzimuthalPhi_QHBoxLayout->setSpacing(Window_width * 0.003);
		ObjectSetting_SphereAzimuthalPhi_QHBoxLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		ObjectSetting_SphereAzimuthalPhi_QLabel = new QLabel;
		ObjectSetting_SphereAzimuthalPhi_QLabel->setFixedSize(Window_width * 0.090, Window_height * 0.028);
		ObjectSetting_SphereAzimuthalPhi_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		ObjectSetting_SphereAzimuthalPhi_QLabel->setFont(font_D_LBL1);
		ObjectSetting_SphereAzimuthalPhi_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		ObjectSetting_SphereAzimuthalPhi_QLabel->setText(QString::fromLocal8Bit("S./D. azmthl ang.:"));

		ObjectSetting_SphereStartingAzimuthalPhi_QLineEdit = new QLineEdit;
		ObjectSetting_SphereStartingAzimuthalPhi_QLineEdit->setFixedSize(Window_width * 0.0545, Window_height * 0.028);
		ObjectSetting_SphereStartingAzimuthalPhi_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectSetting_SphereStartingAzimuthalPhi_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		ObjectSetting_SphereStartingAzimuthalPhi_QLineEdit->setFont(font_D_LE1);
		ObjectSetting_SphereStartingAzimuthalPhi_QLineEdit->setPlaceholderText("deg");
		ObjectSetting_SphereStartingAzimuthalPhi_QLineEdit->setValidator(ValFactory::makePositiveReal(ObjectSetting_SphereStartingAzimuthalPhi_QLineEdit));

		ObjectSetting_SphereDeltaAzimuthalPhi_QLineEdit = new QLineEdit;
		ObjectSetting_SphereDeltaAzimuthalPhi_QLineEdit->setFixedSize(Window_width * 0.0545, Window_height * 0.028);
		ObjectSetting_SphereDeltaAzimuthalPhi_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectSetting_SphereDeltaAzimuthalPhi_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		ObjectSetting_SphereDeltaAzimuthalPhi_QLineEdit->setFont(font_D_LE1);
		ObjectSetting_SphereDeltaAzimuthalPhi_QLineEdit->setPlaceholderText("deg");
		ObjectSetting_SphereDeltaAzimuthalPhi_QLineEdit->setValidator(ValFactory::makePositiveReal(ObjectSetting_SphereDeltaAzimuthalPhi_QLineEdit));

		ObjectSetting_SphereAzimuthalPhi_QLabel->hide();
		ObjectSetting_SphereStartingAzimuthalPhi_QLineEdit->hide();
		ObjectSetting_SphereDeltaAzimuthalPhi_QLineEdit->hide();

		ObjectSetting_SphereAzimuthalPhi_QHBoxLayout->addWidget(ObjectSetting_SphereAzimuthalPhi_QLabel);
		ObjectSetting_SphereAzimuthalPhi_QHBoxLayout->addWidget(ObjectSetting_SphereStartingAzimuthalPhi_QLineEdit);
		ObjectSetting_SphereAzimuthalPhi_QHBoxLayout->addWidget(ObjectSetting_SphereDeltaAzimuthalPhi_QLineEdit);

		layout->addLayout(ObjectSetting_SphereAzimuthalPhi_QHBoxLayout);
	}

	{// 
		ObjectSetting_SpherePolarTheta_QHBoxLayout = new QHBoxLayout;
		ObjectSetting_SpherePolarTheta_QHBoxLayout->setContentsMargins(Window_width * 0.003, 0, Window_width * 0.003, 0);
		ObjectSetting_SpherePolarTheta_QHBoxLayout->setSpacing(Window_width * 0.003);
		ObjectSetting_SpherePolarTheta_QHBoxLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		ObjectSetting_SPolarTheta_QLabel = new QLabel;
		ObjectSetting_SPolarTheta_QLabel->setFixedSize(Window_width * 0.090, Window_height * 0.028);
		ObjectSetting_SPolarTheta_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		ObjectSetting_SPolarTheta_QLabel->setFont(font_D_LBL1);
		ObjectSetting_SPolarTheta_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		ObjectSetting_SPolarTheta_QLabel->setText(QString::fromLocal8Bit("S./D. polar ang.:"));

		ObjectSetting_SphereStartingPolarTheta_QLineEdit = new QLineEdit;
		ObjectSetting_SphereStartingPolarTheta_QLineEdit->setFixedSize(Window_width * 0.0545, Window_height * 0.028);
		ObjectSetting_SphereStartingPolarTheta_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectSetting_SphereStartingPolarTheta_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		ObjectSetting_SphereStartingPolarTheta_QLineEdit->setFont(font_D_LE1);
		ObjectSetting_SphereStartingPolarTheta_QLineEdit->setPlaceholderText("deg");
		ObjectSetting_SphereStartingPolarTheta_QLineEdit->setValidator(ValFactory::makePositiveReal(ObjectSetting_SphereStartingPolarTheta_QLineEdit));

		ObjectSetting_SphereDeltaPolarTheta_QLineEdit = new QLineEdit;
		ObjectSetting_SphereDeltaPolarTheta_QLineEdit->setFixedSize(Window_width * 0.0545, Window_height * 0.028);
		ObjectSetting_SphereDeltaPolarTheta_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectSetting_SphereDeltaPolarTheta_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		ObjectSetting_SphereDeltaPolarTheta_QLineEdit->setFont(font_D_LE1);
		ObjectSetting_SphereDeltaPolarTheta_QLineEdit->setPlaceholderText("deg");
		ObjectSetting_SphereDeltaPolarTheta_QLineEdit->setValidator(ValFactory::makePositiveReal(ObjectSetting_SphereDeltaPolarTheta_QLineEdit));

		ObjectSetting_SPolarTheta_QLabel->hide();
		ObjectSetting_SphereStartingPolarTheta_QLineEdit->hide();
		ObjectSetting_SphereDeltaPolarTheta_QLineEdit->hide();

		ObjectSetting_SpherePolarTheta_QHBoxLayout->addWidget(ObjectSetting_SPolarTheta_QLabel);
		ObjectSetting_SpherePolarTheta_QHBoxLayout->addWidget(ObjectSetting_SphereStartingPolarTheta_QLineEdit);
		ObjectSetting_SpherePolarTheta_QHBoxLayout->addWidget(ObjectSetting_SphereDeltaPolarTheta_QLineEdit);

		layout->addLayout(ObjectSetting_SpherePolarTheta_QHBoxLayout);
	}

	{// 
		ObjectSetting_SphereCenter_QHBoxLayout = new QHBoxLayout;
		ObjectSetting_SphereCenter_QHBoxLayout->setContentsMargins(Window_width * 0.003, 0, Window_width * 0.003, 0);
		ObjectSetting_SphereCenter_QHBoxLayout->setSpacing(Window_width * 0.003);
		ObjectSetting_SphereCenter_QHBoxLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		ObjectSetting_SCenter_QLabel = new QLabel;
		ObjectSetting_SCenter_QLabel->setFixedSize(Window_width * 0.090, Window_height * 0.028);
		ObjectSetting_SCenter_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		ObjectSetting_SCenter_QLabel->setFont(font_D_LBL1);
		ObjectSetting_SCenter_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		ObjectSetting_SCenter_QLabel->setText(QString::fromLocal8Bit("Center XYZ:"));

		ObjectSetting_SphereCenterX_QLineEdit = new QLineEdit;
		ObjectSetting_SphereCenterX_QLineEdit->setFixedSize(Window_width * 0.0545, Window_height * 0.028);
		ObjectSetting_SphereCenterX_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectSetting_SphereCenterX_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		ObjectSetting_SphereCenterX_QLineEdit->setFont(font_D_LE1);
		ObjectSetting_SphereCenterX_QLineEdit->setPlaceholderText("cm");
		ObjectSetting_SphereCenterX_QLineEdit->setValidator(ValFactory::makeReal(ObjectSetting_SphereCenterX_QLineEdit));

		ObjectSetting_SphereCenterY_QLineEdit = new QLineEdit;
		ObjectSetting_SphereCenterY_QLineEdit->setFixedSize(Window_width * 0.0545, Window_height * 0.028);
		ObjectSetting_SphereCenterY_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectSetting_SphereCenterY_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		ObjectSetting_SphereCenterY_QLineEdit->setFont(font_D_LE1);
		ObjectSetting_SphereCenterY_QLineEdit->setPlaceholderText("cm");
		ObjectSetting_SphereCenterY_QLineEdit->setValidator(ValFactory::makeReal(ObjectSetting_SphereCenterY_QLineEdit));

		ObjectSetting_SphereCenterZ_QLineEdit = new QLineEdit;
		ObjectSetting_SphereCenterZ_QLineEdit->setFixedSize(Window_width * 0.0545, Window_height * 0.028);
		ObjectSetting_SphereCenterZ_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectSetting_SphereCenterZ_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		ObjectSetting_SphereCenterZ_QLineEdit->setFont(font_D_LE1);
		ObjectSetting_SphereCenterZ_QLineEdit->setPlaceholderText("cm");
		ObjectSetting_SphereCenterZ_QLineEdit->setValidator(ValFactory::makeReal(ObjectSetting_SphereCenterZ_QLineEdit));

		ObjectSetting_SCenter_QLabel->hide();
		ObjectSetting_SphereCenterX_QLineEdit->hide();
		ObjectSetting_SphereCenterY_QLineEdit->hide();
		ObjectSetting_SphereCenterZ_QLineEdit->hide();

		ObjectSetting_SphereCenter_QHBoxLayout->addWidget(ObjectSetting_SCenter_QLabel);
		ObjectSetting_SphereCenter_QHBoxLayout->addWidget(ObjectSetting_SphereCenterX_QLineEdit);
		ObjectSetting_SphereCenter_QHBoxLayout->addWidget(ObjectSetting_SphereCenterY_QLineEdit);
		ObjectSetting_SphereCenter_QHBoxLayout->addWidget(ObjectSetting_SphereCenterZ_QLineEdit);

		layout->addLayout(ObjectSetting_SphereCenter_QHBoxLayout);
	}

	{// 
		ObjectSetting_SphereRotate_QHBoxLayout = new QHBoxLayout;
		ObjectSetting_SphereRotate_QHBoxLayout->setContentsMargins(Window_width * 0.003, 0, Window_width * 0.003, 0);
		ObjectSetting_SphereRotate_QHBoxLayout->setSpacing(Window_width * 0.003);
		ObjectSetting_SphereRotate_QHBoxLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		ObjectSetting_SRotate_QLabel = new QLabel;
		ObjectSetting_SRotate_QLabel->setFixedSize(Window_width * 0.090, Window_height * 0.028);
		ObjectSetting_SRotate_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		ObjectSetting_SRotate_QLabel->setFont(font_D_LBL1);
		ObjectSetting_SRotate_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		ObjectSetting_SRotate_QLabel->setText(QString::fromLocal8Bit("Rotate XYZ:"));

		ObjectSetting_SphereRotateX_QLineEdit = new QLineEdit;
		ObjectSetting_SphereRotateX_QLineEdit->setFixedSize(Window_width * 0.0545, Window_height * 0.028);
		ObjectSetting_SphereRotateX_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectSetting_SphereRotateX_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		ObjectSetting_SphereRotateX_QLineEdit->setFont(font_D_LE1);
		ObjectSetting_SphereRotateX_QLineEdit->setPlaceholderText("deg");
		ObjectSetting_SphereRotateX_QLineEdit->setValidator(ValFactory::makePositiveReal(ObjectSetting_SphereRotateX_QLineEdit));

		ObjectSetting_SphereRotateY_QLineEdit = new QLineEdit;
		ObjectSetting_SphereRotateY_QLineEdit->setFixedSize(Window_width * 0.0545, Window_height * 0.028);
		ObjectSetting_SphereRotateY_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectSetting_SphereRotateY_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		ObjectSetting_SphereRotateY_QLineEdit->setFont(font_D_LE1);
		ObjectSetting_SphereRotateY_QLineEdit->setPlaceholderText("deg");
		ObjectSetting_SphereRotateY_QLineEdit->setValidator(ValFactory::makePositiveReal(ObjectSetting_SphereRotateY_QLineEdit));

		ObjectSetting_SphereRotateZ_QLineEdit = new QLineEdit;
		ObjectSetting_SphereRotateZ_QLineEdit->setFixedSize(Window_width * 0.0545, Window_height * 0.028);
		ObjectSetting_SphereRotateZ_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectSetting_SphereRotateZ_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		ObjectSetting_SphereRotateZ_QLineEdit->setFont(font_D_LE1);
		ObjectSetting_SphereRotateZ_QLineEdit->setPlaceholderText("deg");
		ObjectSetting_SphereRotateZ_QLineEdit->setValidator(ValFactory::makePositiveReal(ObjectSetting_SphereRotateZ_QLineEdit));

		ObjectSetting_SRotate_QLabel->hide();
		ObjectSetting_SphereRotateX_QLineEdit->hide();
		ObjectSetting_SphereRotateY_QLineEdit->hide();
		ObjectSetting_SphereRotateZ_QLineEdit->hide();

		ObjectSetting_SphereRotate_QHBoxLayout->addWidget(ObjectSetting_SRotate_QLabel);
		ObjectSetting_SphereRotate_QHBoxLayout->addWidget(ObjectSetting_SphereRotateX_QLineEdit);
		ObjectSetting_SphereRotate_QHBoxLayout->addWidget(ObjectSetting_SphereRotateY_QLineEdit);
		ObjectSetting_SphereRotate_QHBoxLayout->addWidget(ObjectSetting_SphereRotateZ_QLineEdit);

		layout->addLayout(ObjectSetting_SphereRotate_QHBoxLayout);
	}

	{// 
		ObjectSetting_CylinderRadius_QHBoxLayout = new QHBoxLayout;
		ObjectSetting_CylinderRadius_QHBoxLayout->setContentsMargins(Window_width * 0.003, 0, Window_width * 0.003, 0);
		ObjectSetting_CylinderRadius_QHBoxLayout->setSpacing(Window_width * 0.003);
		ObjectSetting_CylinderRadius_QHBoxLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		ObjectSetting_CRadius_QLabel = new QLabel;
		ObjectSetting_CRadius_QLabel->setFixedSize(Window_width * 0.090, Window_height * 0.028);
		ObjectSetting_CRadius_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		ObjectSetting_CRadius_QLabel->setFont(font_D_LBL1);
		ObjectSetting_CRadius_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		ObjectSetting_CRadius_QLabel->setText(QString::fromLocal8Bit("Radius min/max:"));

		ObjectSetting_CylinderInnerRadius_QLineEdit = new QLineEdit;
		ObjectSetting_CylinderInnerRadius_QLineEdit->setFixedSize(Window_width * 0.0545, Window_height * 0.028);
		ObjectSetting_CylinderInnerRadius_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectSetting_CylinderInnerRadius_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		ObjectSetting_CylinderInnerRadius_QLineEdit->setFont(font_D_LE1);
		ObjectSetting_CylinderInnerRadius_QLineEdit->setPlaceholderText("cm");
		ObjectSetting_CylinderInnerRadius_QLineEdit->setValidator(ValFactory::makePositiveReal(ObjectSetting_CylinderInnerRadius_QLineEdit));

		ObjectSetting_CylinderOuterRadius_QLineEdit = new QLineEdit;
		ObjectSetting_CylinderOuterRadius_QLineEdit->setFixedSize(Window_width * 0.0545, Window_height * 0.028);
		ObjectSetting_CylinderOuterRadius_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectSetting_CylinderOuterRadius_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		ObjectSetting_CylinderOuterRadius_QLineEdit->setFont(font_D_LE1);
		ObjectSetting_CylinderOuterRadius_QLineEdit->setPlaceholderText("cm");
		ObjectSetting_CylinderOuterRadius_QLineEdit->setValidator(ValFactory::makePositiveReal(ObjectSetting_CylinderOuterRadius_QLineEdit));

		ObjectSetting_CRadius_QLabel->hide();
		ObjectSetting_CylinderInnerRadius_QLineEdit->hide();
		ObjectSetting_CylinderOuterRadius_QLineEdit->hide();

		ObjectSetting_CylinderRadius_QHBoxLayout->addWidget(ObjectSetting_CRadius_QLabel);
		ObjectSetting_CylinderRadius_QHBoxLayout->addWidget(ObjectSetting_CylinderInnerRadius_QLineEdit);
		ObjectSetting_CylinderRadius_QHBoxLayout->addWidget(ObjectSetting_CylinderOuterRadius_QLineEdit);

		layout->addLayout(ObjectSetting_CylinderRadius_QHBoxLayout);
	}

	{// 
		ObjectSetting_CylinderHeightZ_QHBoxLayout = new QHBoxLayout;
		ObjectSetting_CylinderHeightZ_QHBoxLayout->setContentsMargins(Window_width * 0.003, 0, Window_width * 0.003, 0);
		ObjectSetting_CylinderHeightZ_QHBoxLayout->setSpacing(Window_width * 0.003);
		ObjectSetting_CylinderHeightZ_QHBoxLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		ObjectSetting_CHeightZ_QLabel = new QLabel;
		ObjectSetting_CHeightZ_QLabel->setFixedSize(Window_width * 0.090, Window_height * 0.028);
		ObjectSetting_CHeightZ_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		ObjectSetting_CHeightZ_QLabel->setFont(font_D_LBL1);
		ObjectSetting_CHeightZ_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		ObjectSetting_CHeightZ_QLabel->setText(QString::fromLocal8Bit("Half heightZ:"));

		ObjectSetting_CylinderHeightZ_QLineEdit = new QLineEdit;
		ObjectSetting_CylinderHeightZ_QLineEdit->setFixedSize(Window_width * 0.0545, Window_height * 0.028);
		ObjectSetting_CylinderHeightZ_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectSetting_CylinderHeightZ_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		ObjectSetting_CylinderHeightZ_QLineEdit->setFont(font_D_LE1);
		ObjectSetting_CylinderHeightZ_QLineEdit->setPlaceholderText("cm");
		ObjectSetting_CylinderHeightZ_QLineEdit->setValidator(ValFactory::makePositiveReal(ObjectSetting_CylinderHeightZ_QLineEdit));

		ObjectSetting_CHeightZ_QLabel->hide();
		ObjectSetting_CylinderHeightZ_QLineEdit->hide();
		ObjectSetting_CylinderOuterRadius_QLineEdit->hide();

		ObjectSetting_CylinderHeightZ_QHBoxLayout->addWidget(ObjectSetting_CHeightZ_QLabel);
		ObjectSetting_CylinderHeightZ_QHBoxLayout->addWidget(ObjectSetting_CylinderHeightZ_QLineEdit);

		layout->addLayout(ObjectSetting_CylinderHeightZ_QHBoxLayout);
	}

	{// 
		ObjectSetting_CylinderAngle_QHBoxLayout = new QHBoxLayout;
		ObjectSetting_CylinderAngle_QHBoxLayout->setContentsMargins(Window_width * 0.003, 0, Window_width * 0.003, 0);
		ObjectSetting_CylinderAngle_QHBoxLayout->setSpacing(Window_width * 0.003);
		ObjectSetting_CylinderAngle_QHBoxLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		ObjectSetting_CAngle_QLabel = new QLabel;
		ObjectSetting_CAngle_QLabel->setFixedSize(Window_width * 0.090, Window_height * 0.028);
		ObjectSetting_CAngle_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		ObjectSetting_CAngle_QLabel->setFont(font_D_LBL1);
		ObjectSetting_CAngle_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		ObjectSetting_CAngle_QLabel->setText(QString::fromLocal8Bit("S./D. ang."));

		ObjectSetting_CylinderStartingAngle_QLineEdit = new QLineEdit;
		ObjectSetting_CylinderStartingAngle_QLineEdit->setFixedSize(Window_width * 0.0545, Window_height * 0.028);
		ObjectSetting_CylinderStartingAngle_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectSetting_CylinderStartingAngle_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		ObjectSetting_CylinderStartingAngle_QLineEdit->setFont(font_D_LE1);
		ObjectSetting_CylinderStartingAngle_QLineEdit->setPlaceholderText("deg");
		ObjectSetting_CylinderStartingAngle_QLineEdit->setValidator(ValFactory::makePositiveReal(ObjectSetting_CylinderStartingAngle_QLineEdit));

		ObjectSetting_CylinderDeltaAngle_QLineEdit = new QLineEdit;
		ObjectSetting_CylinderDeltaAngle_QLineEdit->setFixedSize(Window_width * 0.0545, Window_height * 0.028);
		ObjectSetting_CylinderDeltaAngle_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectSetting_CylinderDeltaAngle_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		ObjectSetting_CylinderDeltaAngle_QLineEdit->setFont(font_D_LE1);
		ObjectSetting_CylinderDeltaAngle_QLineEdit->setPlaceholderText("deg");
		ObjectSetting_CylinderDeltaAngle_QLineEdit->setValidator(ValFactory::makePositiveReal(ObjectSetting_CylinderDeltaAngle_QLineEdit));

		ObjectSetting_CAngle_QLabel->hide();
		ObjectSetting_CylinderStartingAngle_QLineEdit->hide();
		ObjectSetting_CylinderDeltaAngle_QLineEdit->hide();

		ObjectSetting_CylinderAngle_QHBoxLayout->addWidget(ObjectSetting_CAngle_QLabel);
		ObjectSetting_CylinderAngle_QHBoxLayout->addWidget(ObjectSetting_CylinderStartingAngle_QLineEdit);
		ObjectSetting_CylinderAngle_QHBoxLayout->addWidget(ObjectSetting_CylinderDeltaAngle_QLineEdit);

		layout->addLayout(ObjectSetting_CylinderAngle_QHBoxLayout);
	}

	{// 
		ObjectSetting_CylinderCenter_QHBoxLayout = new QHBoxLayout;
		ObjectSetting_CylinderCenter_QHBoxLayout->setContentsMargins(Window_width * 0.003, 0, Window_width * 0.003, 0);
		ObjectSetting_CylinderCenter_QHBoxLayout->setSpacing(Window_width * 0.003);
		ObjectSetting_CylinderCenter_QHBoxLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		ObjectSetting_CCenter_QLabel = new QLabel;
		ObjectSetting_CCenter_QLabel->setFixedSize(Window_width * 0.090, Window_height * 0.028);
		ObjectSetting_CCenter_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		ObjectSetting_CCenter_QLabel->setFont(font_D_LBL1);
		ObjectSetting_CCenter_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		ObjectSetting_CCenter_QLabel->setText(QString::fromLocal8Bit("Center XYZ:"));

		ObjectSetting_CylinderCenterX_QLineEdit = new QLineEdit;
		ObjectSetting_CylinderCenterX_QLineEdit->setFixedSize(Window_width * 0.0545, Window_height * 0.028);
		ObjectSetting_CylinderCenterX_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectSetting_CylinderCenterX_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		ObjectSetting_CylinderCenterX_QLineEdit->setFont(font_D_LE1);
		ObjectSetting_CylinderCenterX_QLineEdit->setPlaceholderText("cm");
		ObjectSetting_CylinderCenterX_QLineEdit->setValidator(ValFactory::makeReal(ObjectSetting_CylinderCenterX_QLineEdit));

		ObjectSetting_CylinderCenterY_QLineEdit = new QLineEdit;
		ObjectSetting_CylinderCenterY_QLineEdit->setFixedSize(Window_width * 0.0545, Window_height * 0.028);
		ObjectSetting_CylinderCenterY_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectSetting_CylinderCenterY_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		ObjectSetting_CylinderCenterY_QLineEdit->setFont(font_D_LE1);
		ObjectSetting_CylinderCenterY_QLineEdit->setPlaceholderText("cm");
		ObjectSetting_CylinderCenterY_QLineEdit->setValidator(ValFactory::makeReal(ObjectSetting_CylinderCenterY_QLineEdit));

		ObjectSetting_CylinderCenterZ_QLineEdit = new QLineEdit;
		ObjectSetting_CylinderCenterZ_QLineEdit->setFixedSize(Window_width * 0.0545, Window_height * 0.028);
		ObjectSetting_CylinderCenterZ_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectSetting_CylinderCenterZ_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		ObjectSetting_CylinderCenterZ_QLineEdit->setFont(font_D_LE1);
		ObjectSetting_CylinderCenterZ_QLineEdit->setPlaceholderText("cm");
		ObjectSetting_CylinderCenterZ_QLineEdit->setValidator(ValFactory::makeReal(ObjectSetting_CylinderCenterZ_QLineEdit));

		ObjectSetting_CCenter_QLabel->hide();
		ObjectSetting_CylinderCenterX_QLineEdit->hide();
		ObjectSetting_CylinderCenterY_QLineEdit->hide();
		ObjectSetting_CylinderCenterZ_QLineEdit->hide();

		ObjectSetting_CylinderCenter_QHBoxLayout->addWidget(ObjectSetting_CCenter_QLabel);
		ObjectSetting_CylinderCenter_QHBoxLayout->addWidget(ObjectSetting_CylinderCenterX_QLineEdit);
		ObjectSetting_CylinderCenter_QHBoxLayout->addWidget(ObjectSetting_CylinderCenterY_QLineEdit);
		ObjectSetting_CylinderCenter_QHBoxLayout->addWidget(ObjectSetting_CylinderCenterZ_QLineEdit);

		layout->addLayout(ObjectSetting_CylinderCenter_QHBoxLayout);
	}

	{// 
		ObjectSetting_CylinderRotate_QHBoxLayout = new QHBoxLayout;
		ObjectSetting_CylinderRotate_QHBoxLayout->setContentsMargins(Window_width * 0.003, 0, Window_width * 0.003, 0);
		ObjectSetting_CylinderRotate_QHBoxLayout->setSpacing(Window_width * 0.003);
		ObjectSetting_CylinderRotate_QHBoxLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		ObjectSetting_CRotate_QLabel = new QLabel;
		ObjectSetting_CRotate_QLabel->setFixedSize(Window_width * 0.090, Window_height * 0.028);
		ObjectSetting_CRotate_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		ObjectSetting_CRotate_QLabel->setFont(font_D_LBL1);
		ObjectSetting_CRotate_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		ObjectSetting_CRotate_QLabel->setText(QString::fromLocal8Bit("Rotate XYZ:"));

		ObjectSetting_CylinderRotateX_QLineEdit = new QLineEdit;
		ObjectSetting_CylinderRotateX_QLineEdit->setFixedSize(Window_width * 0.0545, Window_height * 0.028);
		ObjectSetting_CylinderRotateX_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectSetting_CylinderRotateX_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		ObjectSetting_CylinderRotateX_QLineEdit->setFont(font_D_LE1);
		ObjectSetting_CylinderRotateX_QLineEdit->setPlaceholderText("deg");
		ObjectSetting_CylinderRotateX_QLineEdit->setValidator(ValFactory::makePositiveReal(ObjectSetting_CylinderRotateX_QLineEdit));

		ObjectSetting_CylinderRotateY_QLineEdit = new QLineEdit;
		ObjectSetting_CylinderRotateY_QLineEdit->setFixedSize(Window_width * 0.0545, Window_height * 0.028);
		ObjectSetting_CylinderRotateY_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectSetting_CylinderRotateY_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		ObjectSetting_CylinderRotateY_QLineEdit->setFont(font_D_LE1);
		ObjectSetting_CylinderRotateY_QLineEdit->setPlaceholderText("deg");
		ObjectSetting_CylinderRotateY_QLineEdit->setValidator(ValFactory::makePositiveReal(ObjectSetting_CylinderRotateY_QLineEdit));

		ObjectSetting_CylinderRotateZ_QLineEdit = new QLineEdit;
		ObjectSetting_CylinderRotateZ_QLineEdit->setFixedSize(Window_width * 0.0545, Window_height * 0.028);
		ObjectSetting_CylinderRotateZ_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectSetting_CylinderRotateZ_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		ObjectSetting_CylinderRotateZ_QLineEdit->setFont(font_D_LE1);
		ObjectSetting_CylinderRotateZ_QLineEdit->setPlaceholderText("deg");
		ObjectSetting_CylinderRotateZ_QLineEdit->setValidator(ValFactory::makePositiveReal(ObjectSetting_CylinderRotateZ_QLineEdit));

		ObjectSetting_CRotate_QLabel->hide();
		ObjectSetting_CylinderRotateX_QLineEdit->hide();
		ObjectSetting_CylinderRotateY_QLineEdit->hide();
		ObjectSetting_CylinderRotateZ_QLineEdit->hide();

		ObjectSetting_CylinderRotate_QHBoxLayout->addWidget(ObjectSetting_CRotate_QLabel);
		ObjectSetting_CylinderRotate_QHBoxLayout->addWidget(ObjectSetting_CylinderRotateX_QLineEdit);
		ObjectSetting_CylinderRotate_QHBoxLayout->addWidget(ObjectSetting_CylinderRotateY_QLineEdit);
		ObjectSetting_CylinderRotate_QHBoxLayout->addWidget(ObjectSetting_CylinderRotateZ_QLineEdit);

		layout->addLayout(ObjectSetting_CylinderRotate_QHBoxLayout);
	}

	{// 
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);
		hLayout->setSpacing(Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QLabel* label = new QLabel;
		label->setFixedSize(Window_width * 0.090, Window_height * 0.028);
		label->setStyleSheet(DialogStyle::DATA_LABEL);
		label->setFont(font_D_LBL1);
		label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		label->setText(QString::fromLocal8Bit("Density:"));

		ObjectSetting_Density_QLineEdit = new QLineEdit;
		ObjectSetting_Density_QLineEdit->setFixedSize(Window_width * 0.0545, Window_height * 0.028);
		ObjectSetting_Density_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectSetting_Density_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		ObjectSetting_Density_QLineEdit->setFont(font_D_LE1);
		ObjectSetting_Density_QLineEdit->setPlaceholderText("g/cm3");
		ObjectSetting_Density_QLineEdit->setValidator(ValFactory::makePositiveReal(ObjectSetting_Density_QLineEdit));

		hLayout->addWidget(label);
		hLayout->addWidget(ObjectSetting_Density_QLineEdit);

		layout->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);
		hLayout->setSpacing(Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		ObjectSetting_Material_QLabel = new QLabel;
		ObjectSetting_Material_QLabel->setFixedSize(Window_width * 0.090, Window_height * 0.028);
		ObjectSetting_Material_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		ObjectSetting_Material_QLabel->setFont(font_D_LBL1);
		ObjectSetting_Material_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		ObjectSetting_Material_QLabel->setText(QString::fromLocal8Bit("Material:"));

		ObjectSetting_MaterialEdit_QPushButton = new QPushButton(QString::fromLocal8Bit("Edit"));
		Buttons_FunctionPanelRight.append(ObjectSetting_MaterialEdit_QPushButton);
		ObjectSetting_MaterialEdit_QPushButton->setFixedSize(Window_width * 0.045, Window_height * 0.025);
		ObjectSetting_MaterialEdit_QPushButton->setStyleSheet(DialogStyle::MENU_BTN2);
		ObjectSetting_MaterialEdit_QPushButton->setFont(font_D_BTN2);
		ObjectSetting_MaterialEdit_QPushButton->setCheckable(false);
		ObjectSetting_MaterialPanelTextWindow_UserQDialog = new ObjectMaterialTextWindow(this); // Initialize TextWindow
		ObjectSetting_MaterialPanelTextWindow_UserQDialog->textEdit->setFont(font_D_MAT);

		ObjectSetting_MaterialMassFraction_QRadioButton = new QRadioButton(QString::fromLocal8Bit("Mass\nFraction"));
		ObjectSetting_MaterialMassFraction_QRadioButton->setStyleSheet(DialogStyle::RADIOBUTTON_WIDGET);
		ObjectSetting_MaterialMassFraction_QRadioButton->setFont(font_D_RB2);
		ObjectSetting_MaterialMassFraction_QRadioButton->setFixedSize(Window_width * 0.060, Window_height * 0.028);

		ObjectSetting_MaterialAtomFraction_QRadioButton = new QRadioButton(QString::fromLocal8Bit("Atom\nFraction"));
		ObjectSetting_MaterialAtomFraction_QRadioButton->setStyleSheet(DialogStyle::RADIOBUTTON_WIDGET);
		ObjectSetting_MaterialAtomFraction_QRadioButton->setFont(font_D_RB2);
		ObjectSetting_MaterialAtomFraction_QRadioButton->setFixedSize(Window_width * 0.060, Window_height * 0.028);

		QButtonGroup* formatGroup = new QButtonGroup;
		formatGroup->addButton(ObjectSetting_MaterialMassFraction_QRadioButton);
		formatGroup->addButton(ObjectSetting_MaterialAtomFraction_QRadioButton);
		ObjectSetting_MaterialMassFraction_QRadioButton->setChecked(true);

		connect(ObjectSetting_MaterialEdit_QPushButton, SIGNAL(clicked()), this, SLOT(slot_ObjectSetting_MaterialEdit_ButtonClicked())); connect(ObjectSetting_MaterialEdit_QPushButton, SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));

		hLayout->addWidget(ObjectSetting_Material_QLabel);
		hLayout->addWidget(ObjectSetting_MaterialEdit_QPushButton);
		hLayout->addWidget(ObjectSetting_MaterialMassFraction_QRadioButton);
		hLayout->addWidget(ObjectSetting_MaterialAtomFraction_QRadioButton);

		layout->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		hLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);

		ObjectAddingOK_QPushButton = new QPushButton(QString::fromLocal8Bit("OK"));
		Buttons_FunctionPanelRight.append(ObjectAddingOK_QPushButton);
		ObjectAddingOK_QPushButton->setFixedSize(Window_width * 0.104, Window_height * 0.028);
		ObjectAddingOK_QPushButton->setCheckable(false);
		ObjectAddingOK_QPushButton->setStyleSheet(DialogStyle::MENU_BTN3);
		ObjectAddingOK_QPushButton->setFont(font_D_BTN3);

		connect(ObjectAddingOK_QPushButton, SIGNAL(clicked()), this, SLOT(slot_ObjectAddingOK_ButtonClicked())); connect(ObjectAddingOK_QPushButton, SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));

		hLayout->addWidget(ObjectAddingOK_QPushButton);

		layout->addLayout(hLayout);
	}

	scrollArea->setWidget(containerWidget);
	QVBoxLayout* dialogLayout = new QVBoxLayout(ObjectAdding_UserQDialog);
	dialogLayout->addWidget(scrollArea);
	ObjectAdding_UserQDialog->setLayout(dialogLayout);
	ObjectAdding_UserQDialog->adjustSize();

	// Set global variables 

}
void FunctionPanelRight::ExecuteObjectAddRoutines()
{
	{
		if (b_IsObjectSettingClosed == true) // X버튼으로 나갔을때
		{
			b_IsObjectSettingClosed = false;
			return;
		}
	}

	// == 객체 polydata/actor 생성 ==
	{
		// Make object actor
		// if (m_ObjectType[m_Object_MakingIndex] == 0) theApp.ObjectGenerate_Box(m_Object_MakingIndex);
		// if (m_ObjectType[m_Object_MakingIndex] == 1) theApp.ObjectGenerate_Sphere(m_Object_MakingIndex);
		// if (m_ObjectType[m_Object_MakingIndex] == 2) theApp.ObjectGenerate_Cylinder(m_Object_MakingIndex);
		// 1. 현재 만들려는 객체의 ID와 타입(0:Box, 1:Sphere...)을 가져옵니다.
		int id = m_Object_MakingIndex;
		int type = m_ObjectType[id];

		// 2. 공장(Factory)에 주문을 넣습니다. (알아서 Box, Sphere, Cylinder 중 맞는 걸 줍니다)
		auto geometry = GeometryFactory::Create(type);

		// 3. "만들어라(Generate)" 명령을 내립니다. (누구든 상관없이 알아서 동작합니다)
		if (geometry) 
		{
			geometry->Generate(id);
		}
		theApp.m_pVTKWidget->GetSceneRenderer()->ResetCamera();                // 카메라 위치·방향 리셋
		theApp.m_pVTKWidget->GetSceneRenderer()->ResetCameraClippingRange();   // near/far 클리핑면 자동 재계산
		theApp.m_pVTKWidget->renderWindow()->Render();
	}

	// == List 버튼 생성 ==
	{
		if (ObjectSetting_Name_QLineEdit->text().isEmpty()) ObjectSelect_QPushButton[m_Object_MakingIndex] = new QPushButton("Object_" + QString::number(m_Object_MakingIndex)); // 구조물 이름 입력안했을 때 기본값
		else ObjectSelect_QPushButton[m_Object_MakingIndex] = new QPushButton(ObjectSetting_Name_QLineEdit->text()); // 구조물 이름 
		Buttons_FunctionPanelRight.append(ObjectSelect_QPushButton[m_Object_MakingIndex]);
		ObjectSelect_QPushButton[m_Object_MakingIndex]->setObjectName(QString::number(m_Object_MakingIndex));
		ObjectSelect_QPushButton[m_Object_MakingIndex]->setFixedSize(Window_width * 0.05, Window_height * 0.040);
		ObjectSelect_QPushButton[m_Object_MakingIndex]->setStyleSheet(DialogStyle::MENU_BTN7);
		ObjectSelect_QPushButton[m_Object_MakingIndex]->setFont(font_D_BTN14);
		setElidedButtonText(ObjectSelect_QPushButton[m_Object_MakingIndex]); // setFont 뒤에 해야 함!
		ObjectSelect_QPushButton[m_Object_MakingIndex]->setCheckable(true);
		ObjectSelect_QPushButton[m_Object_MakingIndex]->setChecked(true);
		connect(ObjectSelect_QPushButton[m_Object_MakingIndex], SIGNAL(clicked()), this, SLOT(slot_ObjectSelect_ButtonClicked())); connect(ObjectSelect_QPushButton[m_Object_MakingIndex], SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
		ObjectList_QHBoxLayout->addWidget(ObjectSelect_QPushButton[m_Object_MakingIndex]);
		m_Object_Transparency[m_Object_MakingIndex] = 50;
		timer_scroll_object->start(1); // 1ms 이후 이동 
		if (m_Object_SequenceVector.size() == const_Object_MaximumCount) ObjectAdd_QPushButton->setEnabled(false); // 최대치 도달 시 Add 불가능하게
	}

	// Set global variables 	
	{
		m_Object_SelectedIndex = m_Object_MakingIndex;
		m_Object_SequenceVector.push_back(m_Object_SelectedIndex);
		m_Object_MakingIndex++;
	}

	// 객체 업데이트
	{
		// Object 패널모드 설정
		if (m_ObjectType[m_Object_SelectedIndex] == 0)
		{
			SetBoxObjectMode();
			Object_BoxTransparency_QSlider->setValue(50);
		}
		if (m_ObjectType[m_Object_SelectedIndex] == 1)
		{
			SetSphereObjectMode();
			Object_SphereTransparency_QSlider->setValue(50);
		}
		if (m_ObjectType[m_Object_SelectedIndex] == 2)
		{
			SetCylinderObjectMode();
			Object_CylinderTransparency_QSlider->setValue(50);
		}

		// list 버튼 업데이트
		UpdateObject_ListButton(m_Object_SelectedIndex);

		// actor 업데이트
		theApp.UpdateObject_ActorHighlighted(m_Object_SelectedIndex);

		// info status 업데이트
		UpdateObject_InfoStatus(m_Object_SelectedIndex); // Panel 값 업데이트
	}
}
void FunctionPanelRight::slot_ChangeObjectCombo(int idx)
{
	ObjectSetting_Type_QComboBox->hide();	ObjectSetting_Type_QComboBox->show(); // This is for focusing (Do not show white blank)

	if (idx == 0) // Select box
	{
		// layout
		ObjectSetting_BoxLength_QHBoxLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);
		ObjectSetting_BoxThickness_QHBoxLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);
		ObjectSetting_BoxCenter_QHBoxLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);
		ObjectSetting_BoxRotate_QHBoxLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);
		ObjectSetting_SphereRadius_QHBoxLayout->setContentsMargins(Window_width * 0.003, 0, Window_width * 0.003, 0);
		ObjectSetting_SphereAzimuthalPhi_QHBoxLayout->setContentsMargins(Window_width * 0.003, 0, Window_width * 0.003, 0);
		ObjectSetting_SpherePolarTheta_QHBoxLayout->setContentsMargins(Window_width * 0.003, 0, Window_width * 0.003, 0);
		ObjectSetting_SphereCenter_QHBoxLayout->setContentsMargins(Window_width * 0.003, 0, Window_width * 0.003, 0);
		ObjectSetting_SphereRotate_QHBoxLayout->setContentsMargins(Window_width * 0.003, 0, Window_width * 0.003, 0);
		ObjectSetting_CylinderRadius_QHBoxLayout->setContentsMargins(Window_width * 0.003, 0, Window_width * 0.003, 0);
		ObjectSetting_CylinderHeightZ_QHBoxLayout->setContentsMargins(Window_width * 0.003, 0, Window_width * 0.003, 0);
		ObjectSetting_CylinderAngle_QHBoxLayout->setContentsMargins(Window_width * 0.003, 0, Window_width * 0.003, 0);
		ObjectSetting_CylinderCenter_QHBoxLayout->setContentsMargins(Window_width * 0.003, 0, Window_width * 0.003, 0);
		ObjectSetting_CylinderRotate_QHBoxLayout->setContentsMargins(Window_width * 0.003, 0, Window_width * 0.003, 0);

		// QLabel
		ObjectSetting_BoxLength_QLabel->show();
		ObjectSetting_BoxThickness_QLabel->show();
		ObjectSetting_BoxCenter_QLabel->show();
		ObjectSetting_BoxRotate_QLabel->show();
		ObjectSetting_SphereRadius_QLabel->hide();
		ObjectSetting_SphereAzimuthalPhi_QLabel->hide();
		ObjectSetting_SPolarTheta_QLabel->hide();
		ObjectSetting_SCenter_QLabel->hide();
		ObjectSetting_SRotate_QLabel->hide();
		ObjectSetting_CRadius_QLabel->hide();
		ObjectSetting_CHeightZ_QLabel->hide();
		ObjectSetting_CAngle_QLabel->hide();
		ObjectSetting_CCenter_QLabel->hide();
		ObjectSetting_CRotate_QLabel->hide();

		// QLineEdit
		ObjectSetting_BoxLengthX_QLineEdit->show();
		ObjectSetting_BoxLengthY_QLineEdit->show();
		ObjectSetting_BoxLengthZ_QLineEdit->show();
		ObjectSetting_BoxThicknessX_QLineEdit->show();
		ObjectSetting_BoxThicknessY_QLineEdit->show();
		ObjectSetting_BoxThicknessZ_QLineEdit->show();
		ObjectSetting_BoxCenterX_QLineEdit->show();
		ObjectSetting_BoxCenterY_QLineEdit->show();
		ObjectSetting_BoxCenterZ_QLineEdit->show();
		ObjectSetting_BoxRotateX_QLineEdit->show();
		ObjectSetting_BoxRotateY_QLineEdit->show();
		ObjectSetting_BoxRotateZ_QLineEdit->show();
		ObjectSetting_SphereInnerRadius_QLineEdit->hide();
		ObjectSetting_SphereOuterRadius_QLineEdit->hide();
		ObjectSetting_SphereStartingAzimuthalPhi_QLineEdit->hide();
		ObjectSetting_SphereDeltaAzimuthalPhi_QLineEdit->hide();
		ObjectSetting_SphereStartingPolarTheta_QLineEdit->hide();
		ObjectSetting_SphereDeltaPolarTheta_QLineEdit->hide();
		ObjectSetting_SphereCenterX_QLineEdit->hide();
		ObjectSetting_SphereCenterY_QLineEdit->hide();
		ObjectSetting_SphereCenterZ_QLineEdit->hide();
		ObjectSetting_SphereRotateX_QLineEdit->hide();
		ObjectSetting_SphereRotateY_QLineEdit->hide();
		ObjectSetting_SphereRotateZ_QLineEdit->hide();
		ObjectSetting_CylinderInnerRadius_QLineEdit->hide();
		ObjectSetting_CylinderOuterRadius_QLineEdit->hide();
		ObjectSetting_CylinderHeightZ_QLineEdit->hide();
		ObjectSetting_CylinderStartingAngle_QLineEdit->hide();
		ObjectSetting_CylinderDeltaAngle_QLineEdit->hide();
		ObjectSetting_CylinderCenterX_QLineEdit->hide();
		ObjectSetting_CylinderCenterY_QLineEdit->hide();
		ObjectSetting_CylinderCenterZ_QLineEdit->hide();
		ObjectSetting_CylinderRotateX_QLineEdit->hide();
		ObjectSetting_CylinderRotateY_QLineEdit->hide();
		ObjectSetting_CylinderRotateZ_QLineEdit->hide();
	}

	if (idx == 1) // Select sphere
	{
		// layout
		ObjectSetting_BoxLength_QHBoxLayout->setContentsMargins(Window_width * 0.003, 0, Window_width * 0.003, 0);
		ObjectSetting_BoxThickness_QHBoxLayout->setContentsMargins(Window_width * 0.003, 0, Window_width * 0.003, 0);
		ObjectSetting_BoxCenter_QHBoxLayout->setContentsMargins(Window_width * 0.003, 0, Window_width * 0.003, 0);
		ObjectSetting_BoxRotate_QHBoxLayout->setContentsMargins(Window_width * 0.003, 0, Window_width * 0.003, 0);
		ObjectSetting_SphereRadius_QHBoxLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);
		ObjectSetting_SphereAzimuthalPhi_QHBoxLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);
		ObjectSetting_SpherePolarTheta_QHBoxLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);
		ObjectSetting_SphereCenter_QHBoxLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);
		ObjectSetting_SphereRotate_QHBoxLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);
		ObjectSetting_CylinderRadius_QHBoxLayout->setContentsMargins(Window_width * 0.003, 0, Window_width * 0.003, 0);
		ObjectSetting_CylinderHeightZ_QHBoxLayout->setContentsMargins(Window_width * 0.003, 0, Window_width * 0.003, 0);
		ObjectSetting_CylinderAngle_QHBoxLayout->setContentsMargins(Window_width * 0.003, 0, Window_width * 0.003, 0);
		ObjectSetting_CylinderCenter_QHBoxLayout->setContentsMargins(Window_width * 0.003, 0, Window_width * 0.003, 0);
		ObjectSetting_CylinderRotate_QHBoxLayout->setContentsMargins(Window_width * 0.003, 0, Window_width * 0.003, 0);

		// QLabel
		ObjectSetting_BoxLength_QLabel->hide();
		ObjectSetting_BoxThickness_QLabel->hide();
		ObjectSetting_BoxCenter_QLabel->hide();
		ObjectSetting_BoxRotate_QLabel->hide();
		ObjectSetting_SphereRadius_QLabel->show();
		ObjectSetting_SphereAzimuthalPhi_QLabel->show();
		ObjectSetting_SPolarTheta_QLabel->show();
		ObjectSetting_SCenter_QLabel->show();
		ObjectSetting_SRotate_QLabel->show();
		ObjectSetting_CRadius_QLabel->hide();
		ObjectSetting_CHeightZ_QLabel->hide();
		ObjectSetting_CAngle_QLabel->hide();
		ObjectSetting_CCenter_QLabel->hide();
		ObjectSetting_CRotate_QLabel->hide();

		// QLineEdit
		ObjectSetting_BoxLengthX_QLineEdit->hide();
		ObjectSetting_BoxLengthY_QLineEdit->hide();
		ObjectSetting_BoxLengthZ_QLineEdit->hide();
		ObjectSetting_BoxThicknessX_QLineEdit->hide();
		ObjectSetting_BoxThicknessY_QLineEdit->hide();
		ObjectSetting_BoxThicknessZ_QLineEdit->hide();
		ObjectSetting_BoxCenterX_QLineEdit->hide();
		ObjectSetting_BoxCenterY_QLineEdit->hide();
		ObjectSetting_BoxCenterZ_QLineEdit->hide();
		ObjectSetting_BoxRotateX_QLineEdit->hide();
		ObjectSetting_BoxRotateY_QLineEdit->hide();
		ObjectSetting_BoxRotateZ_QLineEdit->hide();
		ObjectSetting_SphereInnerRadius_QLineEdit->show();
		ObjectSetting_SphereOuterRadius_QLineEdit->show();
		ObjectSetting_SphereStartingAzimuthalPhi_QLineEdit->show();
		ObjectSetting_SphereDeltaAzimuthalPhi_QLineEdit->show();
		ObjectSetting_SphereStartingPolarTheta_QLineEdit->show();
		ObjectSetting_SphereDeltaPolarTheta_QLineEdit->show();
		ObjectSetting_SphereCenterX_QLineEdit->show();
		ObjectSetting_SphereCenterY_QLineEdit->show();
		ObjectSetting_SphereCenterZ_QLineEdit->show();
		ObjectSetting_SphereRotateX_QLineEdit->show();
		ObjectSetting_SphereRotateY_QLineEdit->show();
		ObjectSetting_SphereRotateZ_QLineEdit->show();
		ObjectSetting_CylinderInnerRadius_QLineEdit->hide();
		ObjectSetting_CylinderOuterRadius_QLineEdit->hide();
		ObjectSetting_CylinderHeightZ_QLineEdit->hide();
		ObjectSetting_CylinderStartingAngle_QLineEdit->hide();
		ObjectSetting_CylinderDeltaAngle_QLineEdit->hide();
		ObjectSetting_CylinderCenterX_QLineEdit->hide();
		ObjectSetting_CylinderCenterY_QLineEdit->hide();
		ObjectSetting_CylinderCenterZ_QLineEdit->hide();
		ObjectSetting_CylinderRotateX_QLineEdit->hide();
		ObjectSetting_CylinderRotateY_QLineEdit->hide();
		ObjectSetting_CylinderRotateZ_QLineEdit->hide();
	}

	if (idx == 2) // Select cylinder
	{
		// layout
		ObjectSetting_BoxLength_QHBoxLayout->setContentsMargins(Window_width * 0.003, 0, Window_width * 0.003, 0);
		ObjectSetting_BoxThickness_QHBoxLayout->setContentsMargins(Window_width * 0.003, 0, Window_width * 0.003, 0);
		ObjectSetting_BoxCenter_QHBoxLayout->setContentsMargins(Window_width * 0.003, 0, Window_width * 0.003, 0);
		ObjectSetting_BoxRotate_QHBoxLayout->setContentsMargins(Window_width * 0.003, 0, Window_width * 0.003, 0);
		ObjectSetting_SphereRadius_QHBoxLayout->setContentsMargins(Window_width * 0.003, 0, Window_width * 0.003, 0);
		ObjectSetting_SphereAzimuthalPhi_QHBoxLayout->setContentsMargins(Window_width * 0.003, 0, Window_width * 0.003, 0);
		ObjectSetting_SpherePolarTheta_QHBoxLayout->setContentsMargins(Window_width * 0.003, 0, Window_width * 0.003, 0);
		ObjectSetting_SphereCenter_QHBoxLayout->setContentsMargins(Window_width * 0.003, 0, Window_width * 0.003, 0);
		ObjectSetting_SphereRotate_QHBoxLayout->setContentsMargins(Window_width * 0.003, 0, Window_width * 0.003, 0);
		ObjectSetting_CylinderRadius_QHBoxLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);
		ObjectSetting_CylinderHeightZ_QHBoxLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);
		ObjectSetting_CylinderAngle_QHBoxLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);
		ObjectSetting_CylinderCenter_QHBoxLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);
		ObjectSetting_CylinderRotate_QHBoxLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);

		// QLabel
		ObjectSetting_BoxLength_QLabel->hide();
		ObjectSetting_BoxThickness_QLabel->hide();
		ObjectSetting_BoxCenter_QLabel->hide();
		ObjectSetting_BoxRotate_QLabel->hide();
		ObjectSetting_SphereRadius_QLabel->hide();
		ObjectSetting_SphereAzimuthalPhi_QLabel->hide();
		ObjectSetting_SPolarTheta_QLabel->hide();
		ObjectSetting_SCenter_QLabel->hide();
		ObjectSetting_SRotate_QLabel->hide();
		ObjectSetting_CRadius_QLabel->show();
		ObjectSetting_CHeightZ_QLabel->show();
		ObjectSetting_CAngle_QLabel->show();
		ObjectSetting_CCenter_QLabel->show();
		ObjectSetting_CRotate_QLabel->show();

		// QLineEdit
		ObjectSetting_BoxLengthX_QLineEdit->hide();
		ObjectSetting_BoxLengthY_QLineEdit->hide();
		ObjectSetting_BoxLengthZ_QLineEdit->hide();
		ObjectSetting_BoxThicknessX_QLineEdit->hide();
		ObjectSetting_BoxThicknessY_QLineEdit->hide();
		ObjectSetting_BoxThicknessZ_QLineEdit->hide();
		ObjectSetting_BoxCenterX_QLineEdit->hide();
		ObjectSetting_BoxCenterY_QLineEdit->hide();
		ObjectSetting_BoxCenterZ_QLineEdit->hide();
		ObjectSetting_BoxRotateX_QLineEdit->hide();
		ObjectSetting_BoxRotateY_QLineEdit->hide();
		ObjectSetting_BoxRotateZ_QLineEdit->hide();
		ObjectSetting_SphereInnerRadius_QLineEdit->hide();
		ObjectSetting_SphereOuterRadius_QLineEdit->hide();
		ObjectSetting_SphereStartingAzimuthalPhi_QLineEdit->hide();
		ObjectSetting_SphereDeltaAzimuthalPhi_QLineEdit->hide();
		ObjectSetting_SphereStartingPolarTheta_QLineEdit->hide();
		ObjectSetting_SphereDeltaPolarTheta_QLineEdit->hide();
		ObjectSetting_SphereCenterX_QLineEdit->hide();
		ObjectSetting_SphereCenterY_QLineEdit->hide();
		ObjectSetting_SphereCenterZ_QLineEdit->hide();
		ObjectSetting_SphereRotateX_QLineEdit->hide();
		ObjectSetting_SphereRotateY_QLineEdit->hide();
		ObjectSetting_SphereRotateZ_QLineEdit->hide();
		ObjectSetting_CylinderInnerRadius_QLineEdit->show();
		ObjectSetting_CylinderOuterRadius_QLineEdit->show();
		ObjectSetting_CylinderHeightZ_QLineEdit->show();
		ObjectSetting_CylinderStartingAngle_QLineEdit->show();
		ObjectSetting_CylinderDeltaAngle_QLineEdit->show();
		ObjectSetting_CylinderCenterX_QLineEdit->show();
		ObjectSetting_CylinderCenterY_QLineEdit->show();
		ObjectSetting_CylinderCenterZ_QLineEdit->show();
		ObjectSetting_CylinderRotateX_QLineEdit->show();
		ObjectSetting_CylinderRotateY_QLineEdit->show();
		ObjectSetting_CylinderRotateZ_QLineEdit->show();
	}
}
void FunctionPanelRight::slot_ObjectAddingOK_ButtonClicked()
{
	SaveObject_InfoData_InAddingSetting(m_Object_MakingIndex); // info data 컨테이너에 정보 저장
	ObjectSetting_MaterialPanelTextWindow_UserQDialog->setText("");

	// AddingOK에 Setting widget 정상 종료를 위해 반드시 필요함
	b_IsObjectSettingOKClicked = true;
	ObjectAdding_UserQDialog->close();
}
void FunctionPanelRight::slot_ObjectSetting_MaterialEdit_ButtonClicked()
{
	ObjectSetting_MaterialPanelTextWindow_UserQDialog->setText(ObjectSetting_MaterialPanelTextWindow_UserQDialog->getText());
	// Show the dialog
	ObjectSetting_MaterialPanelTextWindow_UserQDialog->exec(); // Use exec() for a modal dialog or show() for a non-modal dialog
}

// Main function - Info Status
void FunctionPanelRight::slot_ObjectUpdate_ButtonClicked()
{
    if (m_Object_SequenceVector.size() == 0)
    {
        theApp.SetMessageBox("Object should be added first.");
        return;
    }
    
    // 1. 업데이트할 대상의 번호표(ID)를 가져옵니다.
    int id = m_Object_SelectedIndex;

    SaveObject_InfoData(id); // Info status 기반으로 Info data 업데이트

    // [수정된 부분] Factory 패턴 적용 (if-else 제거)
    // 2. 이 ID가 어떤 도형인지(Box? Sphere?) 타입을 확인합니다.
    int type = m_ObjectType[id]; 
    
    // 3. 공장에 주문해서 알맞은 전문가(객체)를 데려옵니다.
    auto geometry = GeometryFactory::Create(type);
    
    // 4. "다시 만들어(Generate)"라고 명령합니다.
    if (geometry) {
        geometry->Generate(id);
    }

    // actor 업데이트 (하이라이트 등 후처리)
    theApp.UpdateObject_ActorHighlighted(id);
    theApp.m_pVTKWidget->renderWindow()->Render();
}
void FunctionPanelRight::slot_ObjectPanel_MaterialEdit_ButtonClicked()
{
	if (m_ObjectType[m_Object_SelectedIndex] == 0) // Box
	{
		ObjectBox_MaterialPanelTextWindow_UserQDialog->setText(ObjectBox_MaterialPanelTextWindow_UserQDialog->getText());
		ObjectBox_MaterialPanelTextWindow_UserQDialog->exec(); // Use exec() for a modal dialog or show() for a non-modal dialog
	}
	if (m_ObjectType[m_Object_SelectedIndex] == 1) // Sphere
	{
		ObjectSphere_MaterialPanelTextWindow_UserQDialog->setText(ObjectSphere_MaterialPanelTextWindow_UserQDialog->getText());
		ObjectSphere_MaterialPanelTextWindow_UserQDialog->exec(); // Use exec() for a modal dialog or show() for a non-modal dialog
	}
	if (m_ObjectType[m_Object_SelectedIndex] == 2) // Cylinder
	{
		ObjectCylinder_MaterialPanelTextWindow_UserQDialog->setText(ObjectCylinder_MaterialPanelTextWindow_UserQDialog->getText());
		ObjectCylinder_MaterialPanelTextWindow_UserQDialog->exec(); // Use exec() for a modal dialog or show() for a non-modal dialog
	}

}
void FunctionPanelRight::slot_ObjectTransparencySlider(int value)
{
	QSlider* SelectedSlider = qobject_cast<QSlider*>(sender());
	double TransparencyValue = 1 - (double(SelectedSlider->value()) / 100);
	theApp.ObjectPanelActors[m_Object_SelectedIndex]->GetProperty()->SetOpacity(TransparencyValue);
	theApp.m_pVTKWidget->renderWindow()->Render();

	m_Object_Transparency[m_Object_SelectedIndex] = value;
}
void FunctionPanelRight::slot_ObjectChargedParticleTransport_ButtonClicked()
{
	QPushButton* buttonSender = qobject_cast<QPushButton*>(sender());
	QString buttonText = buttonSender->text();
	if (buttonText == "On") // On -> Off
	{
		buttonSender->setChecked(false);
		buttonSender->setText("Off");
		m_Object_ChargedParticleTransport[m_Object_SelectedIndex] = false;
	}
	if (buttonText == "Off")
	{
		buttonSender->setChecked(true);
		buttonSender->setText("On");
		m_Object_ChargedParticleTransport[m_Object_SelectedIndex] = true;
	}
}

// Sub-function (1)
void FunctionPanelRight::moveMaximumScrollbar_object() // called by slot_ObjectAdd_ButtonClicked()
{
	timer_scroll_object->stop();
	ObjectList_QScrollBar->setValue(ObjectList_QScrollBar->maximum());
}
void FunctionPanelRight::UpdateObject_Mode(int objectIndex)
{
	if (m_Object_SequenceVector.size() == 0) SetInitialObjectMode();

	else if (m_ObjectType[objectIndex] == 0)
	{
		SetBoxObjectMode();
		Object_BoxTransparency_QSlider->setValue(m_Object_Transparency[objectIndex]);
	}
	else if (m_ObjectType[objectIndex] == 1)
	{
		SetSphereObjectMode();
		Object_SphereTransparency_QSlider->setValue(m_Object_Transparency[objectIndex]);
	}
	else if (m_ObjectType[objectIndex] == 2)
	{
		SetCylinderObjectMode();
		Object_CylinderTransparency_QSlider->setValue(m_Object_Transparency[objectIndex]);
	}
}
void FunctionPanelRight::UpdateObject_InfoStatus(int objectIndex) // called by (1) slot_ObjectSelect_ButtonClicked, (2) slot_ObjectAdd_ButtonClicked, (3) slot_ObjectDelete_ButtonClicked, (4) LoadReconsturctionFile_previous
{
	if (objectIndex < 0) // 객체 index가 invalid일때 Info status 정보 초기화
	{
		ObjectBox_CenterX_QLineEdit->clear();
		ObjectBox_CenterY_QLineEdit->clear();
		ObjectBox_CenterZ_QLineEdit->clear();
		ObjectBox_RotateX_QLineEdit->clear();
		ObjectBox_RotateY_QLineEdit->clear();
		ObjectBox_RotateZ_QLineEdit->clear();
		ObjectBox_LengthX_QLineEdit->clear();
		ObjectBox_LengthY_QLineEdit->clear();
		ObjectBox_LengthZ_QLineEdit->clear();
		ObjectBox_ThicknessX_QLineEdit->clear();
		ObjectBox_ThicknessY_QLineEdit->clear();
		ObjectBox_ThicknessZ_QLineEdit->clear();
		ObjectBox_Density_QLineEdit->clear();
		ObjectBox_MaterialPanelTextWindow_UserQDialog->setText("");
		ObjectBox_MaterialMassFraction_QRadioButton->setChecked(true);
	}

	if (m_ObjectType[objectIndex] == 0) // Box
	{
		ObjectBox_CenterX_QLineEdit->setText(QString::number(m_Object_MainInfo[objectIndex][1]));
		ObjectBox_CenterY_QLineEdit->setText(QString::number(m_Object_MainInfo[objectIndex][2]));
		ObjectBox_CenterZ_QLineEdit->setText(QString::number(m_Object_MainInfo[objectIndex][3]));
		ObjectBox_RotateX_QLineEdit->setText(QString::number(m_Object_MainInfo[objectIndex][4]));
		ObjectBox_RotateY_QLineEdit->setText(QString::number(m_Object_MainInfo[objectIndex][5]));
		ObjectBox_RotateZ_QLineEdit->setText(QString::number(m_Object_MainInfo[objectIndex][6]));
		ObjectBox_LengthX_QLineEdit->setText(QString::number(m_Object_MainInfo[objectIndex][7]));
		ObjectBox_LengthY_QLineEdit->setText(QString::number(m_Object_MainInfo[objectIndex][8]));
		ObjectBox_LengthZ_QLineEdit->setText(QString::number(m_Object_MainInfo[objectIndex][9]));
		ObjectBox_ThicknessX_QLineEdit->setText(QString::number(m_Object_MainInfo[objectIndex][10]));
		ObjectBox_ThicknessY_QLineEdit->setText(QString::number(m_Object_MainInfo[objectIndex][11]));
		ObjectBox_ThicknessZ_QLineEdit->setText(QString::number(m_Object_MainInfo[objectIndex][12]));
		ObjectBox_Density_QLineEdit->setText(QString::number(m_Object_MainInfo[objectIndex][13]));
		ObjectBox_MaterialPanelTextWindow_UserQDialog->setText(m_Object_MaterialInfo[objectIndex]);
		if (m_Object_MaterialFractionType[objectIndex] == 0) ObjectBox_MaterialMassFraction_QRadioButton->setChecked(true);
		if (m_Object_MaterialFractionType[objectIndex] == 1) ObjectBox_MaterialAtomFraction_QRadioButton->setChecked(true);
	}
	if (m_ObjectType[objectIndex] == 1) // Sphere
	{
		ObjectSphere_CenterX_QLineEdit->setText(QString::number(m_Object_MainInfo[objectIndex][1]));
		ObjectSphere_CenterY_QLineEdit->setText(QString::number(m_Object_MainInfo[objectIndex][2]));
		ObjectSphere_CenterZ_QLineEdit->setText(QString::number(m_Object_MainInfo[objectIndex][3]));
		ObjectSphere_RotateX_QLineEdit->setText(QString::number(m_Object_MainInfo[objectIndex][4]));
		ObjectSphere_RotateY_QLineEdit->setText(QString::number(m_Object_MainInfo[objectIndex][5]));
		ObjectSphere_RotateZ_QLineEdit->setText(QString::number(m_Object_MainInfo[objectIndex][6]));
		ObjectSphere_InnerRadius_QLineEdit->setText(QString::number(m_Object_MainInfo[objectIndex][14]));
		ObjectSphere_OuterRadius_QLineEdit->setText(QString::number(m_Object_MainInfo[objectIndex][15]));
		ObjectSphere_StartingAzimuthalPhi_QLineEdit->setText(QString::number(m_Object_MainInfo[objectIndex][16]));
		ObjectSphere_DeltaAzimuthalPhi_QLineEdit->setText(QString::number(m_Object_MainInfo[objectIndex][17]));
		ObjectSphere_StartingPolarTheta_QLineEdit->setText(QString::number(m_Object_MainInfo[objectIndex][18]));
		ObjectSphere_DeltaPolarTheta_QLineEdit->setText(QString::number(m_Object_MainInfo[objectIndex][19]));
		ObjectSphere_Density_QLineEdit->setText(QString::number(m_Object_MainInfo[objectIndex][13]));
		ObjectSphere_MaterialPanelTextWindow_UserQDialog->setText(m_Object_MaterialInfo[objectIndex]);
		if (m_Object_MaterialFractionType[objectIndex] == 0) ObjectSphere_MaterialMassFraction_QRadioButton->setChecked(true);
		if (m_Object_MaterialFractionType[objectIndex] == 1) ObjectSphere_MaterialAtomFraction_QRadioButton->setChecked(true);
	}
	if (m_ObjectType[objectIndex] == 2) // Cylinder
	{
		ObjectCylinder_CenterX_QLineEdit->setText(QString::number(m_Object_MainInfo[objectIndex][1]));
		ObjectCylinder_CenterY_QLineEdit->setText(QString::number(m_Object_MainInfo[objectIndex][2]));
		ObjectCylinder_CenterZ_QLineEdit->setText(QString::number(m_Object_MainInfo[objectIndex][3]));
		ObjectCylinder_RotateX_QLineEdit->setText(QString::number(m_Object_MainInfo[objectIndex][4]));
		ObjectCylinder_RotateY_QLineEdit->setText(QString::number(m_Object_MainInfo[objectIndex][5]));
		ObjectCylinder_RotateZ_QLineEdit->setText(QString::number(m_Object_MainInfo[objectIndex][6]));
		ObjectCylinder_InnerRadius_QLineEdit->setText(QString::number(m_Object_MainInfo[objectIndex][20]));
		ObjectCylinder_OuterRadius_QLineEdit->setText(QString::number(m_Object_MainInfo[objectIndex][21]));
		ObjectCylinder_HeightZ_QLineEdit->setText(QString::number(m_Object_MainInfo[objectIndex][22]));
		ObjectCylinder_StartingAngle_QLineEdit->setText(QString::number(m_Object_MainInfo[objectIndex][23]));
		ObjectCylinder_DeltaAngle_QLineEdit->setText(QString::number(m_Object_MainInfo[objectIndex][24]));
		ObjectCylinder_Density_QLineEdit->setText(QString::number(m_Object_MainInfo[objectIndex][13]));
		ObjectCylinder_MaterialPanelTextWindow_UserQDialog->setText(m_Object_MaterialInfo[objectIndex]);
		if (m_Object_MaterialFractionType[objectIndex] == 0) ObjectCylinder_MaterialMassFraction_QRadioButton->setChecked(true);
		if (m_Object_MaterialFractionType[objectIndex] == 1) ObjectCylinder_MaterialAtomFraction_QRadioButton->setChecked(true);
	}
}
void FunctionPanelRight::UpdateObject_ListButton(int objectIndex)
{
	for (auto itr_objectID : m_Object_SequenceVector)
	{
		if (itr_objectID != m_Object_SelectedIndex)
		{
			ObjectSelect_QPushButton[itr_objectID]->setChecked(false);
		}
	}
	if (objectIndex >= 0) ObjectSelect_QPushButton[objectIndex]->setChecked(true);
}
void FunctionPanelRight::SaveObject_InfoData_InAddingSetting(int objectIndex) // called by (1) slot_ObjectAddingOK_ButtonClicked, (2) LoadReconsturctionFile_previous
{
	// Exception
	QString GeometryObjectType;
	if (ObjectSetting_Type_QComboBox->currentIndex() == 0) GeometryObjectType = "Box";
	if (ObjectSetting_Type_QComboBox->currentIndex() == 1) GeometryObjectType = "Sphere";
	if (ObjectSetting_Type_QComboBox->currentIndex() == 2) GeometryObjectType = "Cylinder";
	if (GeometryObjectType == "Box")// Box 일때
	{
		// 길이는 모두 0 이상이어야 함
		if (ObjectSetting_BoxLengthX_QLineEdit->text().toDouble() <= 0 || ObjectSetting_BoxLengthY_QLineEdit->text().toDouble() <= 0 || ObjectSetting_BoxLengthZ_QLineEdit->text().toDouble() <= 0)
		{
			theApp.SetMessageBox("Thickness should be bigger than zero");
			return;
		}
		// X,Y,Z 두께가 각 축의 half length 보다 작아야 함
		if (ObjectSetting_BoxThicknessX_QLineEdit->text().toDouble() >= ObjectSetting_BoxLengthX_QLineEdit->text().toDouble()
			|| ObjectSetting_BoxThicknessY_QLineEdit->text().toDouble() >= ObjectSetting_BoxLengthY_QLineEdit->text().toDouble()
			|| ObjectSetting_BoxThicknessZ_QLineEdit->text().toDouble() >= ObjectSetting_BoxLengthZ_QLineEdit->text().toDouble())
		{
			theApp.SetMessageBox("Thickness should be smaller than HalfLength");
			return;
		}
		// X,Y,Z 두께가 0이면 불가능(두께가 존재해야하며, 예외적으로 모두 0,0,0 이면 두께가 없는 꽉찬 Box로 정의)
		if ((ObjectSetting_BoxThicknessX_QLineEdit->text().toDouble() == 0 || ObjectSetting_BoxThicknessY_QLineEdit->text().toDouble() == 0 || ObjectSetting_BoxThicknessZ_QLineEdit->text().toDouble() == 0)
			&& !(ObjectSetting_BoxThicknessX_QLineEdit->text().toDouble() == 0 && ObjectSetting_BoxThicknessY_QLineEdit->text().toDouble() == 0 && ObjectSetting_BoxThicknessZ_QLineEdit->text().toDouble() == 0))
		{
			theApp.SetMessageBox("Thickness should be non-zero value. If all XYZ thicknesses are left blank or entered as 0, it will be defined as a box that is completely filled.");
			return;
		}
	}
	if (GeometryObjectType == "Sphere")// Sphere 일때
	{
		// Inner radius는 Outer radius보다 작아야 함(같아도 안 됨)
		if (ObjectSetting_SphereInnerRadius_QLineEdit->text().toDouble() >= ObjectSetting_SphereOuterRadius_QLineEdit->text().toDouble())
		{
			theApp.SetMessageBox("Rmin should be lower than Rmax");
			return;
		}
	}
	if (GeometryObjectType == "Cylinder")// Cylinder 일때
	{
		// Inner radius는 Outer radius보다 작아야 함(같아도 안 됨)
		if (ObjectSetting_CylinderInnerRadius_QLineEdit->text().toDouble() >= ObjectSetting_CylinderOuterRadius_QLineEdit->text().toDouble())
		{
			theApp.SetMessageBox("Rmin should be lower than Rmax.");
			return;
		}
		// 길이 0 이면 안 됨
		if (ObjectSetting_CylinderHeightZ_QLineEdit->text().toDouble() <= 0)
		{
			theApp.SetMessageBox("HalfHeightZ should not be zero.");
			return;
		}
		// Starting angle이 존재하는데 delta angle이 0이면 안 됨
		if (ObjectSetting_CylinderStartingAngle_QLineEdit->text().toDouble() != 0 && ObjectSetting_CylinderDeltaAngle_QLineEdit->text().toDouble() == 0)
		{
			theApp.SetMessageBox("DeltaAngle should not be zero. If both InitialAngle and DeltaAngle are left blank or entered as 0, it will be defined as full cylinder or tubes");
			return;
		}
	}
	// Save information
	if (ObjectSetting_Name_QLineEdit->text().isEmpty()) m_Object_ButtonName[objectIndex] = "Object_" + QString::number(objectIndex); // 구조물 이름 입력안했을 때 기본값
	else m_Object_ButtonName[objectIndex] = ObjectSetting_Name_QLineEdit->text(); // 구조물 이름 
	m_Object_ChargedParticleTransport[objectIndex] = true; // 하전입자 수송옵션
	if (GeometryObjectType == "Box")
	{

		m_ObjectType[objectIndex] = ObjectSetting_Type_QComboBox->currentIndex();
		m_Object_MainInfo[objectIndex][0] = 1; // 해당 데이터 사용
		m_Object_MainInfo[objectIndex][1] = ObjectSetting_BoxCenterX_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][2] = ObjectSetting_BoxCenterY_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][3] = ObjectSetting_BoxCenterZ_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][4] = ObjectSetting_BoxRotateX_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][5] = ObjectSetting_BoxRotateY_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][6] = ObjectSetting_BoxRotateZ_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][7] = ObjectSetting_BoxLengthX_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][8] = ObjectSetting_BoxLengthY_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][9] = ObjectSetting_BoxLengthZ_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][10] = ObjectSetting_BoxThicknessX_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][11] = ObjectSetting_BoxThicknessY_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][12] = ObjectSetting_BoxThicknessZ_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][13] = ObjectSetting_Density_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MaterialInfo[objectIndex] = ObjectSetting_MaterialPanelTextWindow_UserQDialog->getText(); // 미입력시 ""		
		if (ObjectSetting_MaterialMassFraction_QRadioButton->isChecked()) m_Object_MaterialFractionType[objectIndex] = 0;
		if (ObjectSetting_MaterialAtomFraction_QRadioButton->isChecked()) m_Object_MaterialFractionType[objectIndex] = 1;
	}
	if (GeometryObjectType == "Sphere")
	{
		m_ObjectType[objectIndex] = ObjectSetting_Type_QComboBox->currentIndex();
		m_Object_MainInfo[objectIndex][0] = 1; // 해당 데이터 사용
		m_Object_MainInfo[objectIndex][1] = ObjectSetting_SphereCenterX_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][2] = ObjectSetting_SphereCenterY_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][3] = ObjectSetting_SphereCenterZ_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][4] = ObjectSetting_SphereRotateX_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][5] = ObjectSetting_SphereRotateY_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][6] = ObjectSetting_SphereRotateZ_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][14] = ObjectSetting_SphereInnerRadius_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][15] = ObjectSetting_SphereOuterRadius_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][16] = ObjectSetting_SphereStartingAzimuthalPhi_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][17] = ObjectSetting_SphereDeltaAzimuthalPhi_QLineEdit->text().toDouble(); // 미입력시 0
		if (m_Object_MainInfo[objectIndex][17] == 0) m_Object_MainInfo[objectIndex][17] = 360.;
		m_Object_MainInfo[objectIndex][18] = ObjectSetting_SphereStartingPolarTheta_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][19] = ObjectSetting_SphereDeltaPolarTheta_QLineEdit->text().toDouble(); // 미입력시 0
		if (m_Object_MainInfo[objectIndex][19] == 0)  m_Object_MainInfo[objectIndex][19] = 180.;
		m_Object_MainInfo[objectIndex][13] = ObjectSetting_Density_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MaterialInfo[objectIndex] = ObjectSetting_MaterialPanelTextWindow_UserQDialog->getText(); // 미입력시 ""
		if (ObjectSetting_MaterialMassFraction_QRadioButton->isChecked()) m_Object_MaterialFractionType[objectIndex] = 0;
		if (ObjectSetting_MaterialAtomFraction_QRadioButton->isChecked()) m_Object_MaterialFractionType[objectIndex] = 1;
	}
	if (GeometryObjectType == "Cylinder")
	{
		m_ObjectType[objectIndex] = ObjectSetting_Type_QComboBox->currentIndex();
		m_Object_MainInfo[objectIndex][0] = 1; // 해당 데이터 사용
		m_Object_MainInfo[objectIndex][1] = ObjectSetting_CylinderCenterX_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][2] = ObjectSetting_CylinderCenterY_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][3] = ObjectSetting_CylinderCenterZ_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][4] = ObjectSetting_CylinderRotateX_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][5] = ObjectSetting_CylinderRotateY_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][6] = ObjectSetting_CylinderRotateZ_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][20] = ObjectSetting_CylinderInnerRadius_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][21] = ObjectSetting_CylinderOuterRadius_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][22] = ObjectSetting_CylinderHeightZ_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][23] = ObjectSetting_CylinderStartingAngle_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][24] = ObjectSetting_CylinderDeltaAngle_QLineEdit->text().toDouble(); // 미입력시 0
		if (m_Object_MainInfo[objectIndex][24] == 0) m_Object_MainInfo[objectIndex][24] = 360.;
		m_Object_MainInfo[objectIndex][13] = ObjectSetting_Density_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MaterialInfo[objectIndex] = ObjectSetting_MaterialPanelTextWindow_UserQDialog->getText(); // 미입력시 ""
		if (ObjectSetting_MaterialMassFraction_QRadioButton->isChecked()) m_Object_MaterialFractionType[objectIndex] = 0;
		if (ObjectSetting_MaterialAtomFraction_QRadioButton->isChecked()) m_Object_MaterialFractionType[objectIndex] = 1;
	}
}
void FunctionPanelRight::SaveObject_InfoData(int objectIndex) // called by (1) slot_ObjectUpdate_ButtonClicked
{
	// Exception
	QString GeometryObjectType;
	if (m_ObjectType[objectIndex] == 0) GeometryObjectType = "Box";
	if (m_ObjectType[objectIndex] == 1) GeometryObjectType = "Sphere";
	if (m_ObjectType[objectIndex] == 2) GeometryObjectType = "Cylinder";
	if (GeometryObjectType == "Box")// Box 일때
	{
		// 길이는 모두 0 이상이어야 함
		if (ObjectBox_LengthX_QLineEdit->text().toDouble() <= 0 || ObjectBox_LengthY_QLineEdit->text().toDouble() <= 0 || ObjectBox_LengthZ_QLineEdit->text().toDouble() <= 0)
		{
			theApp.SetMessageBox("Thickness should be bigger than zero");
			return;
		}
		// X,Y,Z 두께가 각 축의 half length 보다 작아야 함
		if (ObjectBox_ThicknessX_QLineEdit->text().toDouble() >= ObjectBox_LengthX_QLineEdit->text().toDouble()
			|| ObjectBox_ThicknessY_QLineEdit->text().toDouble() >= ObjectBox_LengthY_QLineEdit->text().toDouble()
			|| ObjectBox_ThicknessZ_QLineEdit->text().toDouble() >= ObjectBox_LengthZ_QLineEdit->text().toDouble())
		{
			theApp.SetMessageBox("Thickness should be smaller than HalfLength");
			return;
		}
		// X,Y,Z 두께가 0이면 불가능(두께가 존재해야하며, 예외적으로 모두 0,0,0 이면 두께가 없는 꽉찬 Box로 정의)
		if ((ObjectBox_ThicknessX_QLineEdit->text().toDouble() == 0 || ObjectBox_ThicknessY_QLineEdit->text().toDouble() == 0 || ObjectBox_ThicknessZ_QLineEdit->text().toDouble() == 0)
			&& !(ObjectBox_ThicknessX_QLineEdit->text().toDouble() == 0 && ObjectBox_ThicknessZ_QLineEdit->text().toDouble() == 0 && ObjectBox_ThicknessZ_QLineEdit->text().toDouble() == 0))
		{
			theApp.SetMessageBox("Thickness should be non-zero value. If all XYZ thicknesses are left blank or entered as 0, it will be defined as a box that is completely filled.");
			return;
		}
	}
	if (GeometryObjectType == "Sphere")// Sphere 일때
	{
		// Inner radius는 Outer radius보다 작아야 함(같아도 안 됨)
		if (ObjectSphere_InnerRadius_QLineEdit->text().toDouble() >= ObjectSphere_OuterRadius_QLineEdit->text().toDouble())
		{
			theApp.SetMessageBox("Rmin should be lower than Rmax");
			return;
		}
	}
	if (GeometryObjectType == "Cylinder")// Cylinder 일때
	{
		// Inner radius는 Outer radius보다 작아야 함(같아도 안 됨)
		if (ObjectCylinder_InnerRadius_QLineEdit->text().toDouble() >= ObjectCylinder_OuterRadius_QLineEdit->text().toDouble())
		{
			theApp.SetMessageBox("Rmin should be lower than Rmax.");
			return;
		}
		// 길이 0 이면 안 됨
		if (ObjectCylinder_HeightZ_QLineEdit->text().toDouble() <= 0)
		{
			theApp.SetMessageBox("HalfHeightZ should not be zero.");
			return;
		}
		// Starting angle이 존재하는데 delta angle이 0이면 안 됨
		if (ObjectCylinder_StartingAngle_QLineEdit->text().toDouble() != 0 && ObjectCylinder_DeltaAngle_QLineEdit->text().toDouble() == 0)
		{
			theApp.SetMessageBox("DeltaAngle should not be zero. If both InitialAngle and DeltaAngle are left blank or entered as 0, it will be defined as full cylinder or tubes");
			return;
		}
	}
	// Save information
	if (GeometryObjectType == "Box")
	{
		m_Object_MainInfo[objectIndex][1] = ObjectBox_CenterX_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][2] = ObjectBox_CenterY_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][3] = ObjectBox_CenterZ_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][4] = ObjectBox_RotateX_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][5] = ObjectBox_RotateY_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][6] = ObjectBox_RotateZ_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][7] = ObjectBox_LengthX_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][8] = ObjectBox_LengthY_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][9] = ObjectBox_LengthZ_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][10] = ObjectBox_ThicknessX_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][11] = ObjectBox_ThicknessY_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][12] = ObjectBox_ThicknessZ_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][13] = ObjectBox_Density_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MaterialInfo[objectIndex] = ObjectBox_MaterialPanelTextWindow_UserQDialog->getText(); // 미입력시 ""		
		if (ObjectBox_MaterialMassFraction_QRadioButton->isChecked()) m_Object_MaterialFractionType[objectIndex] = 0;
		if (ObjectBox_MaterialAtomFraction_QRadioButton->isChecked()) m_Object_MaterialFractionType[objectIndex] = 1;
	}
	if (GeometryObjectType == "Sphere")
	{
		m_Object_MainInfo[objectIndex][1] = ObjectSphere_CenterX_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][2] = ObjectSphere_CenterY_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][3] = ObjectSphere_CenterZ_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][4] = ObjectSphere_RotateX_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][5] = ObjectSphere_RotateY_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][6] = ObjectSphere_RotateZ_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][14] = ObjectSphere_InnerRadius_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][15] = ObjectSphere_OuterRadius_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][16] = ObjectSphere_StartingAzimuthalPhi_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][17] = ObjectSphere_DeltaAzimuthalPhi_QLineEdit->text().toDouble(); // 미입력시 0
		if (m_Object_MainInfo[objectIndex][17] == 0) m_Object_MainInfo[objectIndex][17] = 360.;
		m_Object_MainInfo[objectIndex][18] = ObjectSphere_StartingPolarTheta_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][19] = ObjectSphere_DeltaPolarTheta_QLineEdit->text().toDouble(); // 미입력시 0
		if (m_Object_MainInfo[objectIndex][19] == 0)  m_Object_MainInfo[objectIndex][19] = 180.;
		m_Object_MainInfo[objectIndex][13] = ObjectSphere_Density_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MaterialInfo[objectIndex] = ObjectSphere_MaterialPanelTextWindow_UserQDialog->getText(); // 미입력시 ""
		if (ObjectSphere_MaterialMassFraction_QRadioButton->isChecked()) m_Object_MaterialFractionType[objectIndex] = 0;
		if (ObjectSphere_MaterialAtomFraction_QRadioButton->isChecked()) m_Object_MaterialFractionType[objectIndex] = 1;
	}
	if (GeometryObjectType == "Cylinder")
	{
		m_Object_MainInfo[objectIndex][1] = ObjectCylinder_CenterX_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][2] = ObjectCylinder_CenterY_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][3] = ObjectCylinder_CenterZ_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][4] = ObjectCylinder_RotateX_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][5] = ObjectCylinder_RotateY_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][6] = ObjectCylinder_RotateZ_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][20] = ObjectCylinder_InnerRadius_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][21] = ObjectCylinder_OuterRadius_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][22] = ObjectCylinder_HeightZ_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][23] = ObjectCylinder_StartingAngle_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MainInfo[objectIndex][24] = ObjectCylinder_DeltaAngle_QLineEdit->text().toDouble(); // 미입력시 0
		if (m_Object_MainInfo[objectIndex][24] == 0) m_Object_MainInfo[objectIndex][24] = 360.;
		m_Object_MainInfo[objectIndex][13] = ObjectCylinder_Density_QLineEdit->text().toDouble(); // 미입력시 0
		m_Object_MaterialInfo[objectIndex] = ObjectCylinder_MaterialPanelTextWindow_UserQDialog->getText(); // 미입력시 ""
		if (ObjectCylinder_MaterialMassFraction_QRadioButton->isChecked()) m_Object_MaterialFractionType[objectIndex] = 0;
		if (ObjectCylinder_MaterialAtomFraction_QRadioButton->isChecked()) m_Object_MaterialFractionType[objectIndex] = 1;
	}
}
void FunctionPanelRight::SaveObject_InfoData_InMouseControl(int objectIndex) // called by (1) slot_ObjectUpdate_ButtonClicked
{	
	double* pCenter = theApp.ObjectPanelActors[objectIndex]->GetPosition();
	double TransformPos[3];
	TransformPos[0] = pCenter[0];
	TransformPos[1] = pCenter[1];
	TransformPos[2] = pCenter[2];

	double* pRotation = theApp.ObjectPanelActors[objectIndex]->GetOrientation();
	double TransformRot[3];
	TransformRot[0] = pRotation[0];
	TransformRot[1] = pRotation[1];
	TransformRot[2] = pRotation[2];
		
	m_Object_MainInfo[objectIndex][1] = TransformPos[0];
	m_Object_MainInfo[objectIndex][2] = TransformPos[1];
	m_Object_MainInfo[objectIndex][3] = TransformPos[2];
	m_Object_MainInfo[objectIndex][4] = TransformRot[0];
	m_Object_MainInfo[objectIndex][5] = TransformRot[1];
	m_Object_MainInfo[objectIndex][6] = TransformRot[2];
}

////////////////// Box ////////////////// 

// Main function - Status
void FunctionPanelRight::CreateObjectBoxWidgets(QVBoxLayout* layout)
{
	ObjectBox_QGroupBox = new QGroupBox();
	ObjectBox_QGroupBox->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	ObjectBox_QGroupBox->setStyleSheet(DialogStyle::GROUPBOX_WIDGET7);
	ObjectBox_QGroupBox->setFont(font_D_GBW7);
	ObjectBox_QGroupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	ObjectBox_QGroupBox->setFixedHeight(Window_height * 0.33);

	QVBoxLayout* subLayout = new QVBoxLayout;
	subLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.005, Window_width * 0.003, Window_height * 0.005);
	subLayout->setSpacing(Window_width * 0.003);
	subLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(Window_width * 0.005, Window_height * 0.005, 0, 0);

		ObjectBox_Type_QLabel = new QLabel;
		ObjectBox_Type_QLabel->setFixedSize(Window_width * 0.065, Window_height * 0.028);
		ObjectBox_Type_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		ObjectBox_Type_QLabel->setFont(font_D_LBL1);
		ObjectBox_Type_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		ObjectBox_Type_QLabel->setText(QString::fromLocal8Bit("Type:"));

		ObjectBox_Type_QLineEdit = new QLineEdit;
		ObjectBox_Type_QLineEdit->setFixedSize(Window_width * 0.115, Window_height * 0.028);
		ObjectBox_Type_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectBox_Type_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME4);
		ObjectBox_Type_QLineEdit->setFont(font_D_LE4);
		ObjectBox_Type_QLineEdit->setReadOnly(true);
		ObjectBox_Type_QLineEdit->setText("Box");

		hLayout->addWidget(ObjectBox_Type_QLabel);
		hLayout->addWidget(ObjectBox_Type_QLineEdit);

		subLayout->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(Window_width * 0.005, Window_height * 0.005, 0, 0);

		ObjectBox_Length_QLabel = new QLabel;
		ObjectBox_Length_QLabel->setFixedSize(Window_width * 0.065, Window_height * 0.028);
		ObjectBox_Length_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		ObjectBox_Length_QLabel->setFont(font_D_LBL1);
		ObjectBox_Length_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		ObjectBox_Length_QLabel->setText(QString::fromLocal8Bit("Half len. XYZ:"));

		ObjectBox_LengthX_QLineEdit = new QLineEdit;
		ObjectBox_LengthX_QLineEdit->setFixedSize(Window_width * 0.035, Window_height * 0.028);
		ObjectBox_LengthX_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectBox_LengthX_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME4);
		ObjectBox_LengthX_QLineEdit->setFont(font_D_LE4);
		ObjectBox_LengthX_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(cm)"));
		ObjectBox_LengthX_QLineEdit->setValidator(ValFactory::makePositiveReal(ObjectBox_LengthX_QLineEdit));

		ObjectBox_LengthY_QLineEdit = new QLineEdit;
		ObjectBox_LengthY_QLineEdit->setFixedSize(Window_width * 0.035, Window_height * 0.028);
		ObjectBox_LengthY_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectBox_LengthY_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME4);
		ObjectBox_LengthY_QLineEdit->setFont(font_D_LE4);
		ObjectBox_LengthY_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(cm)"));
		ObjectBox_LengthY_QLineEdit->setValidator(ValFactory::makePositiveReal(ObjectBox_LengthY_QLineEdit));

		ObjectBox_LengthZ_QLineEdit = new QLineEdit;
		ObjectBox_LengthZ_QLineEdit->setFixedSize(Window_width * 0.035, Window_height * 0.028);
		ObjectBox_LengthZ_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectBox_LengthZ_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME4);
		ObjectBox_LengthZ_QLineEdit->setFont(font_D_LE4);
		ObjectBox_LengthZ_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(cm)"));
		ObjectBox_LengthZ_QLineEdit->setValidator(ValFactory::makePositiveReal(ObjectBox_LengthZ_QLineEdit));

		hLayout->addWidget(ObjectBox_Length_QLabel);
		hLayout->addWidget(ObjectBox_LengthX_QLineEdit);
		hLayout->addWidget(ObjectBox_LengthY_QLineEdit);
		hLayout->addWidget(ObjectBox_LengthZ_QLineEdit);

		subLayout->addLayout(hLayout);
	}
	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(Window_width * 0.005, Window_height * 0.005, 0, 0);

		ObjectBox_Thickness_QLabel = new QLabel;
		ObjectBox_Thickness_QLabel->setFixedSize(Window_width * 0.065, Window_height * 0.028);
		ObjectBox_Thickness_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		ObjectBox_Thickness_QLabel->setFont(font_D_LBL1);
		ObjectBox_Thickness_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		ObjectBox_Thickness_QLabel->setText(QString::fromLocal8Bit("Thickness XYZ:"));

		ObjectBox_ThicknessX_QLineEdit = new QLineEdit;
		ObjectBox_ThicknessX_QLineEdit->setFixedSize(Window_width * 0.035, Window_height * 0.028);
		ObjectBox_ThicknessX_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectBox_ThicknessX_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME4);
		ObjectBox_ThicknessX_QLineEdit->setFont(font_D_LE4);
		ObjectBox_ThicknessX_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(cm)"));
		ObjectBox_ThicknessX_QLineEdit->setValidator(ValFactory::makePositiveReal(ObjectBox_ThicknessX_QLineEdit));

		ObjectBox_ThicknessY_QLineEdit = new QLineEdit;
		ObjectBox_ThicknessY_QLineEdit->setFixedSize(Window_width * 0.035, Window_height * 0.028);
		ObjectBox_ThicknessY_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectBox_ThicknessY_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME4);
		ObjectBox_ThicknessY_QLineEdit->setFont(font_D_LE4);
		ObjectBox_ThicknessY_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(cm)"));
		ObjectBox_ThicknessY_QLineEdit->setValidator(ValFactory::makePositiveReal(ObjectBox_ThicknessY_QLineEdit));

		ObjectBox_ThicknessZ_QLineEdit = new QLineEdit;
		ObjectBox_ThicknessZ_QLineEdit->setFixedSize(Window_width * 0.035, Window_height * 0.028);
		ObjectBox_ThicknessZ_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectBox_ThicknessZ_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME4);
		ObjectBox_ThicknessZ_QLineEdit->setFont(font_D_LE4);
		ObjectBox_ThicknessZ_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(cm)"));
		ObjectBox_ThicknessZ_QLineEdit->setValidator(ValFactory::makePositiveReal(ObjectBox_ThicknessZ_QLineEdit));

		hLayout->addWidget(ObjectBox_Thickness_QLabel);
		hLayout->addWidget(ObjectBox_ThicknessX_QLineEdit);
		hLayout->addWidget(ObjectBox_ThicknessY_QLineEdit);
		hLayout->addWidget(ObjectBox_ThicknessZ_QLineEdit);

		subLayout->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(Window_width * 0.005, Window_height * 0.005, 0, 0);

		ObjectBox_Center_QLabel = new QLabel;
		ObjectBox_Center_QLabel->setFixedSize(Window_width * 0.065, Window_height * 0.028);
		ObjectBox_Center_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		ObjectBox_Center_QLabel->setFont(font_D_LBL1);
		ObjectBox_Center_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		ObjectBox_Center_QLabel->setText(QString::fromLocal8Bit("Center XYZ:"));

		ObjectBox_CenterX_QLineEdit = new QLineEdit;
		ObjectBox_CenterX_QLineEdit->setFixedSize(Window_width * 0.035, Window_height * 0.028);
		ObjectBox_CenterX_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectBox_CenterX_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME4);
		ObjectBox_CenterX_QLineEdit->setFont(font_D_LE4);
		ObjectBox_CenterX_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(cm)"));
		ObjectBox_CenterX_QLineEdit->setValidator(ValFactory::makeReal(ObjectBox_CenterX_QLineEdit));

		ObjectBox_CenterY_QLineEdit = new QLineEdit;
		ObjectBox_CenterY_QLineEdit->setFixedSize(Window_width * 0.035, Window_height * 0.028);
		ObjectBox_CenterY_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectBox_CenterY_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME4);
		ObjectBox_CenterY_QLineEdit->setFont(font_D_LE4);
		ObjectBox_CenterY_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(cm)"));
		ObjectBox_CenterY_QLineEdit->setValidator(ValFactory::makeReal(ObjectBox_CenterY_QLineEdit));

		ObjectBox_CenterZ_QLineEdit = new QLineEdit;
		ObjectBox_CenterZ_QLineEdit->setFixedSize(Window_width * 0.035, Window_height * 0.028);
		ObjectBox_CenterZ_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectBox_CenterZ_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME4);
		ObjectBox_CenterZ_QLineEdit->setFont(font_D_LE4);
		ObjectBox_CenterZ_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(cm)"));
		ObjectBox_CenterZ_QLineEdit->setValidator(ValFactory::makeReal(ObjectBox_CenterZ_QLineEdit));

		hLayout->addWidget(ObjectBox_Center_QLabel);
		hLayout->addWidget(ObjectBox_CenterX_QLineEdit);
		hLayout->addWidget(ObjectBox_CenterY_QLineEdit);
		hLayout->addWidget(ObjectBox_CenterZ_QLineEdit);

		subLayout->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(Window_width * 0.005, Window_height * 0.005, 0, 0);

		ObjectBox_Rotate_QLabel = new QLabel;
		ObjectBox_Rotate_QLabel->setFixedSize(Window_width * 0.065, Window_height * 0.028);
		ObjectBox_Rotate_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		ObjectBox_Rotate_QLabel->setFont(font_D_LBL1);
		ObjectBox_Rotate_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		ObjectBox_Rotate_QLabel->setText(QString::fromLocal8Bit("Rotate XYZ:"));

		ObjectBox_RotateX_QLineEdit = new QLineEdit;
		ObjectBox_RotateX_QLineEdit->setFixedSize(Window_width * 0.035, Window_height * 0.028);
		ObjectBox_RotateX_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectBox_RotateX_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME4);
		ObjectBox_RotateX_QLineEdit->setFont(font_D_LE4);
		ObjectBox_RotateX_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(deg)"));
		ObjectBox_RotateX_QLineEdit->setValidator(ValFactory::makePositiveReal(ObjectBox_RotateX_QLineEdit));

		ObjectBox_RotateY_QLineEdit = new QLineEdit;
		ObjectBox_RotateY_QLineEdit->setFixedSize(Window_width * 0.035, Window_height * 0.028);
		ObjectBox_RotateY_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectBox_RotateY_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME4);
		ObjectBox_RotateY_QLineEdit->setFont(font_D_LE4);
		ObjectBox_RotateY_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(deg)"));
		ObjectBox_RotateY_QLineEdit->setValidator(ValFactory::makePositiveReal(ObjectBox_RotateY_QLineEdit));

		ObjectBox_RotateZ_QLineEdit = new QLineEdit;
		ObjectBox_RotateZ_QLineEdit->setFixedSize(Window_width * 0.035, Window_height * 0.028);
		ObjectBox_RotateZ_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectBox_RotateZ_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME4);
		ObjectBox_RotateZ_QLineEdit->setFont(font_D_LE4);
		ObjectBox_RotateZ_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(deg)"));
		ObjectBox_RotateZ_QLineEdit->setValidator(ValFactory::makePositiveReal(ObjectBox_RotateZ_QLineEdit));

		hLayout->addWidget(ObjectBox_Rotate_QLabel);
		hLayout->addWidget(ObjectBox_RotateX_QLineEdit);
		hLayout->addWidget(ObjectBox_RotateY_QLineEdit);
		hLayout->addWidget(ObjectBox_RotateZ_QLineEdit);

		subLayout->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(Window_width * 0.005, Window_height * 0.005, 0, 0);

		ObjectBox_Density_QLabel = new QLabel;
		ObjectBox_Density_QLabel->setFixedSize(Window_width * 0.065, Window_height * 0.028);
		ObjectBox_Density_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		ObjectBox_Density_QLabel->setFont(font_D_LBL1);
		ObjectBox_Density_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		ObjectBox_Density_QLabel->setText(QString::fromLocal8Bit("Density:"));

		ObjectBox_Density_QLineEdit = new QLineEdit;
		ObjectBox_Density_QLineEdit->setFixedSize(Window_width * 0.115, Window_height * 0.028);
		ObjectBox_Density_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectBox_Density_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME4);
		ObjectBox_Density_QLineEdit->setFont(font_D_LE4);
		ObjectBox_Density_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(g/cm3)"));
		ObjectBox_Density_QLineEdit->setValidator(ValFactory::makePositiveReal(ObjectBox_Density_QLineEdit));

		hLayout->addWidget(ObjectBox_Density_QLabel);
		hLayout->addWidget(ObjectBox_Density_QLineEdit);

		subLayout->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(Window_width * 0.005, Window_height * 0.005, 0, 0);

		ObjectBox_Material_QLabel = new QLabel;
		ObjectBox_Material_QLabel->setFixedSize(Window_width * 0.065, Window_height * 0.028);
		ObjectBox_Material_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		ObjectBox_Material_QLabel->setFont(font_D_LBL1);
		ObjectBox_Material_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		ObjectBox_Material_QLabel->setText(QString::fromLocal8Bit("Material:"));

		Object_BoxMaterialEdit_QPushButton = new QPushButton(QString::fromLocal8Bit("Edit"));
		Buttons_FunctionPanelRight.append(Object_BoxMaterialEdit_QPushButton);
		Object_BoxMaterialEdit_QPushButton->setFixedSize(Window_width * 0.028, Window_height * 0.025);
		Object_BoxMaterialEdit_QPushButton->setStyleSheet(DialogStyle::MENU_BTN2);
		Object_BoxMaterialEdit_QPushButton->setFont(font_D_BTN2);
		Object_BoxMaterialEdit_QPushButton->setCheckable(false);
		ObjectBox_MaterialPanelTextWindow_UserQDialog = new ObjectMaterialTextWindow(this); // Initialize TextWindow
		ObjectBox_MaterialPanelTextWindow_UserQDialog->textEdit->setFont(font_D_MAT);

		ObjectBox_MaterialMassFraction_QRadioButton = new QRadioButton(QString::fromLocal8Bit("Mass\nFraction"));
		ObjectBox_MaterialMassFraction_QRadioButton->setStyleSheet(DialogStyle::RADIOBUTTON_WIDGET);
		ObjectBox_MaterialMassFraction_QRadioButton->setFont(font_D_RB2);
		ObjectBox_MaterialMassFraction_QRadioButton->setFixedSize(Window_width * 0.039, Window_height * 0.028);

		ObjectBox_MaterialAtomFraction_QRadioButton = new QRadioButton(QString::fromLocal8Bit("Atom\nFraction"));
		ObjectBox_MaterialAtomFraction_QRadioButton->setStyleSheet(DialogStyle::RADIOBUTTON_WIDGET);
		ObjectBox_MaterialAtomFraction_QRadioButton->setFont(font_D_RB2);
		ObjectBox_MaterialAtomFraction_QRadioButton->setFixedSize(Window_width * 0.039, Window_height * 0.028);

		QButtonGroup* formatGroup = new QButtonGroup;
		formatGroup->addButton(ObjectBox_MaterialMassFraction_QRadioButton);
		formatGroup->addButton(ObjectBox_MaterialAtomFraction_QRadioButton);
		ObjectBox_MaterialMassFraction_QRadioButton->setChecked(true);

		connect(Object_BoxMaterialEdit_QPushButton, SIGNAL(clicked()), this, SLOT(slot_ObjectPanel_MaterialEdit_ButtonClicked())); connect(Object_BoxMaterialEdit_QPushButton, SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));

		hLayout->addWidget(ObjectBox_Material_QLabel);
		hLayout->addWidget(Object_BoxMaterialEdit_QPushButton);
		hLayout->addWidget(ObjectBox_MaterialMassFraction_QRadioButton);
		hLayout->addWidget(ObjectBox_MaterialAtomFraction_QRadioButton);

		subLayout->addLayout(hLayout);
	}

	{// Update
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setContentsMargins(Window_width * 0.005, Window_height * 0.005, Window_width * 0.003, 0);
		hLayout->setSpacing(Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

		Object_BoxUpdate_QPushButton = new QPushButton(QString::fromLocal8Bit("Update"));
		Buttons_FunctionPanelRight.append(Object_BoxUpdate_QPushButton);
		Object_BoxUpdate_QPushButton->setFixedSize(Window_width * 0.104, Window_height * 0.028);
		Object_BoxUpdate_QPushButton->setStyleSheet(DialogStyle::MENU_BTN2);
		Object_BoxUpdate_QPushButton->setFont(font_D_BTN2);
		connect(Object_BoxUpdate_QPushButton, SIGNAL(clicked()), this, SLOT(slot_ObjectUpdate_ButtonClicked())); connect(Object_BoxUpdate_QPushButton, SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));

		hLayout->addWidget(Object_BoxUpdate_QPushButton);

		subLayout->addLayout(hLayout);
	}
	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(Window_width * 0.005, Window_height * 0.005, 0, 0);

		ObjectBox_Transparency_QLabel = new QLabel;
		ObjectBox_Transparency_QLabel->setFixedSize(Window_width * 0.042, Window_height * 0.035);
		ObjectBox_Transparency_QLabel->setStyleSheet(DialogStyle::DATA_LABEL4);
		ObjectBox_Transparency_QLabel->setFont(font_D_LBL4);
		ObjectBox_Transparency_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignCenter);
		ObjectBox_Transparency_QLabel->setText(QString::fromLocal8Bit("Transparency:"));

		Object_BoxTransparency_QSlider = new QSlider(Qt::Horizontal);
		Object_BoxTransparency_QSlider->setFixedSize(Window_width * 0.135, Window_height * 0.018);
		Object_BoxTransparency_QSlider->setTickPosition(QSlider::TicksAbove);
		Object_BoxTransparency_QSlider->setTickInterval(25);
		Object_BoxTransparency_QSlider->setSingleStep(1);
		Object_BoxTransparency_QSlider->setMinimum(0);
		Object_BoxTransparency_QSlider->setMaximum(100);
		Object_BoxTransparency_QSlider->setStyleSheet("QSlider{background: #B9D7C4;}\
												QSlider::handle:horizontal {background-color: B9B9FF;}");
		Object_BoxTransparency_QSlider->setValue(50);

		connect(Object_BoxTransparency_QSlider, SIGNAL(valueChanged(int)), this, SLOT(slot_ObjectTransparencySlider(int))); connect(Object_BoxTransparency_QSlider, SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));

		hLayout->addWidget(ObjectBox_Transparency_QLabel);
		hLayout->addWidget(Object_BoxTransparency_QSlider);

		subLayout->addLayout(hLayout);
	}
	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(Window_width * 0.005, Window_height * 0.005, 0, 0);

		ObjectBox_ChargedParticleTransport_QLabel = new QLabel;
		ObjectBox_ChargedParticleTransport_QLabel->setFixedSize(Window_width * 0.105, Window_height * 0.028);
		ObjectBox_ChargedParticleTransport_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		ObjectBox_ChargedParticleTransport_QLabel->setFont(font_D_LBL1);
		ObjectBox_ChargedParticleTransport_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignCenter);
		ObjectBox_ChargedParticleTransport_QLabel->setText(QString::fromLocal8Bit("Charged particle transport:"));

		Object_Box_ChargedParticleTransport_QPushButton = new QPushButton(QString::fromLocal8Bit("On"));
		Buttons_FunctionPanelRight.append(Object_Box_ChargedParticleTransport_QPushButton);
		Object_Box_ChargedParticleTransport_QPushButton->setFixedSize(Window_width * 0.028, Window_height * 0.028);
		Object_Box_ChargedParticleTransport_QPushButton->setCheckable(true);
		Object_Box_ChargedParticleTransport_QPushButton->setChecked(true);
		Object_Box_ChargedParticleTransport_QPushButton->setStyleSheet(DialogStyle::MENU_BTN3);
		Object_Box_ChargedParticleTransport_QPushButton->setFont(font_D_BTN3);

		connect(Object_Box_ChargedParticleTransport_QPushButton, SIGNAL(clicked()), this, SLOT(slot_ObjectChargedParticleTransport_ButtonClicked())); connect(Object_Box_ChargedParticleTransport_QPushButton, SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));

		hLayout->addWidget(ObjectBox_ChargedParticleTransport_QLabel);
		hLayout->addWidget(Object_Box_ChargedParticleTransport_QPushButton);

		subLayout->addLayout(hLayout);
	}
	ObjectBox_QGroupBox->setLayout(subLayout);
	layout->addWidget(ObjectBox_QGroupBox);
}

////////////////// Sphere ////////////////// 

// Main function - Status
void FunctionPanelRight::CreateObjectSphereWidgets(QVBoxLayout* layout)
{
	ObjectSphere_QGroupBox = new QGroupBox();
	ObjectSphere_QGroupBox->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	ObjectSphere_QGroupBox->setStyleSheet(DialogStyle::GROUPBOX_WIDGET7);
	ObjectSphere_QGroupBox->setFont(font_D_GBW7);
	ObjectSphere_QGroupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	ObjectSphere_QGroupBox->setFixedHeight(Window_height * 0.36);

	QVBoxLayout* subLayout = new QVBoxLayout;
	subLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.005, Window_width * 0.003, Window_height * 0.005);
	subLayout->setSpacing(Window_width * 0.003);
	subLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(Window_width * 0.005, Window_height * 0.005, 0, 0);

		ObjectSphere_Type_QLabel = new QLabel;
		ObjectSphere_Type_QLabel->setFixedSize(Window_width * 0.065, Window_height * 0.028);
		ObjectSphere_Type_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		ObjectSphere_Type_QLabel->setFont(font_D_LBL1);
		ObjectSphere_Type_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		ObjectSphere_Type_QLabel->setText(QString::fromLocal8Bit("Type:"));

		ObjectSphere_Type_QLineEdit = new QLineEdit;
		ObjectSphere_Type_QLineEdit->setFixedSize(Window_width * 0.115, Window_height * 0.028);
		ObjectSphere_Type_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectSphere_Type_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME4);
		ObjectSphere_Type_QLineEdit->setFont(font_D_LE4);
		ObjectSphere_Type_QLineEdit->setReadOnly(true);
		ObjectSphere_Type_QLineEdit->setText("Sphere");

		hLayout->addWidget(ObjectSphere_Type_QLabel);
		hLayout->addWidget(ObjectSphere_Type_QLineEdit);

		subLayout->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(Window_width * 0.005, Window_height * 0.005, 0, 0);

		ObjectSphere_Radius_QLabel = new QLabel;
		ObjectSphere_Radius_QLabel->setFixedSize(Window_width * 0.065, Window_height * 0.028);
		ObjectSphere_Radius_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		ObjectSphere_Radius_QLabel->setFont(font_D_LBL1);
		ObjectSphere_Radius_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		ObjectSphere_Radius_QLabel->setText(QString::fromLocal8Bit("R min/max:"));

		ObjectSphere_InnerRadius_QLineEdit = new QLineEdit;
		ObjectSphere_InnerRadius_QLineEdit->setFixedSize(Window_width * 0.035, Window_height * 0.028);
		ObjectSphere_InnerRadius_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectSphere_InnerRadius_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME4);
		ObjectSphere_InnerRadius_QLineEdit->setFont(font_D_LE4);
		ObjectSphere_InnerRadius_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(cm)"));
		ObjectSphere_InnerRadius_QLineEdit->setValidator(ValFactory::makePositiveReal(ObjectSphere_InnerRadius_QLineEdit));

		ObjectSphere_OuterRadius_QLineEdit = new QLineEdit;
		ObjectSphere_OuterRadius_QLineEdit->setFixedSize(Window_width * 0.035, Window_height * 0.028);
		ObjectSphere_OuterRadius_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectSphere_OuterRadius_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME4);
		ObjectSphere_OuterRadius_QLineEdit->setFont(font_D_LE4);
		ObjectSphere_OuterRadius_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(cm)"));
		ObjectSphere_OuterRadius_QLineEdit->setValidator(ValFactory::makePositiveReal(ObjectSphere_OuterRadius_QLineEdit));

		hLayout->addWidget(ObjectSphere_Radius_QLabel);
		hLayout->addWidget(ObjectSphere_InnerRadius_QLineEdit);
		hLayout->addWidget(ObjectSphere_OuterRadius_QLineEdit);

		subLayout->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(Window_width * 0.005, Window_height * 0.005, 0, 0);

		ObjectSphere_AzimuthalPhi_QLabel = new QLabel;
		ObjectSphere_AzimuthalPhi_QLabel->setFixedSize(Window_width * 0.065, Window_height * 0.028);
		ObjectSphere_AzimuthalPhi_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		ObjectSphere_AzimuthalPhi_QLabel->setFont(font_D_LBL1);
		ObjectSphere_AzimuthalPhi_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		ObjectSphere_AzimuthalPhi_QLabel->setText(QString::fromLocal8Bit("S./D. azmthl ang.:"));

		ObjectSphere_StartingAzimuthalPhi_QLineEdit = new QLineEdit;
		ObjectSphere_StartingAzimuthalPhi_QLineEdit->setFixedSize(Window_width * 0.035, Window_height * 0.028);
		ObjectSphere_StartingAzimuthalPhi_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectSphere_StartingAzimuthalPhi_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME4);
		ObjectSphere_StartingAzimuthalPhi_QLineEdit->setFont(font_D_LE4);
		ObjectSphere_StartingAzimuthalPhi_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(deg)"));
		ObjectSphere_StartingAzimuthalPhi_QLineEdit->setValidator(ValFactory::makePositiveReal(ObjectSphere_StartingAzimuthalPhi_QLineEdit));

		ObjectSphere_DeltaAzimuthalPhi_QLineEdit = new QLineEdit;
		ObjectSphere_DeltaAzimuthalPhi_QLineEdit->setFixedSize(Window_width * 0.035, Window_height * 0.028);
		ObjectSphere_DeltaAzimuthalPhi_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectSphere_DeltaAzimuthalPhi_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME4);
		ObjectSphere_DeltaAzimuthalPhi_QLineEdit->setFont(font_D_LE4);
		ObjectSphere_DeltaAzimuthalPhi_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(deg)"));
		ObjectSphere_DeltaAzimuthalPhi_QLineEdit->setValidator(ValFactory::makePositiveReal(ObjectSphere_DeltaAzimuthalPhi_QLineEdit));

		hLayout->addWidget(ObjectSphere_AzimuthalPhi_QLabel);
		hLayout->addWidget(ObjectSphere_StartingAzimuthalPhi_QLineEdit);
		hLayout->addWidget(ObjectSphere_DeltaAzimuthalPhi_QLineEdit);

		subLayout->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(Window_width * 0.005, Window_height * 0.005, 0, 0);

		ObjectSphere_PolarTheta_QLabel = new QLabel;
		ObjectSphere_PolarTheta_QLabel->setFixedSize(Window_width * 0.065, Window_height * 0.028);
		ObjectSphere_PolarTheta_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		ObjectSphere_PolarTheta_QLabel->setFont(font_D_LBL1);
		ObjectSphere_PolarTheta_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		ObjectSphere_PolarTheta_QLabel->setText(QString::fromLocal8Bit("S./D. polar ang.:"));

		ObjectSphere_StartingPolarTheta_QLineEdit = new QLineEdit;
		ObjectSphere_StartingPolarTheta_QLineEdit->setFixedSize(Window_width * 0.035, Window_height * 0.028);
		ObjectSphere_StartingPolarTheta_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectSphere_StartingPolarTheta_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME4);
		ObjectSphere_StartingPolarTheta_QLineEdit->setFont(font_D_LE4);
		ObjectSphere_StartingPolarTheta_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(deg)"));
		ObjectSphere_StartingPolarTheta_QLineEdit->setValidator(ValFactory::makePositiveReal(ObjectSphere_StartingPolarTheta_QLineEdit));

		ObjectSphere_DeltaPolarTheta_QLineEdit = new QLineEdit;
		ObjectSphere_DeltaPolarTheta_QLineEdit->setFixedSize(Window_width * 0.035, Window_height * 0.028);
		ObjectSphere_DeltaPolarTheta_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectSphere_DeltaPolarTheta_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME4);
		ObjectSphere_DeltaPolarTheta_QLineEdit->setFont(font_D_LE4);
		ObjectSphere_DeltaPolarTheta_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(deg)"));
		ObjectSphere_DeltaPolarTheta_QLineEdit->setValidator(ValFactory::makePositiveReal(ObjectSphere_DeltaPolarTheta_QLineEdit));

		hLayout->addWidget(ObjectSphere_PolarTheta_QLabel);
		hLayout->addWidget(ObjectSphere_StartingPolarTheta_QLineEdit);
		hLayout->addWidget(ObjectSphere_DeltaPolarTheta_QLineEdit);

		subLayout->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(Window_width * 0.005, Window_height * 0.005, 0, 0);

		ObjectSphere_Center_QLabel = new QLabel;
		ObjectSphere_Center_QLabel->setFixedSize(Window_width * 0.065, Window_height * 0.028);
		ObjectSphere_Center_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		ObjectSphere_Center_QLabel->setFont(font_D_LBL1);
		ObjectSphere_Center_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		ObjectSphere_Center_QLabel->setText(QString::fromLocal8Bit("Center XYZ:"));

		ObjectSphere_CenterX_QLineEdit = new QLineEdit;
		ObjectSphere_CenterX_QLineEdit->setFixedSize(Window_width * 0.035, Window_height * 0.028);
		ObjectSphere_CenterX_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectSphere_CenterX_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME4);
		ObjectSphere_CenterX_QLineEdit->setFont(font_D_LE4);
		ObjectSphere_CenterX_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(cm)"));
		ObjectSphere_CenterX_QLineEdit->setValidator(ValFactory::makeReal(ObjectSphere_CenterX_QLineEdit));

		ObjectSphere_CenterY_QLineEdit = new QLineEdit;
		ObjectSphere_CenterY_QLineEdit->setFixedSize(Window_width * 0.035, Window_height * 0.028);
		ObjectSphere_CenterY_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectSphere_CenterY_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME4);
		ObjectSphere_CenterY_QLineEdit->setFont(font_D_LE4);
		ObjectSphere_CenterY_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(cm)"));
		ObjectSphere_CenterY_QLineEdit->setValidator(ValFactory::makeReal(ObjectSphere_CenterY_QLineEdit));

		ObjectSphere_CenterZ_QLineEdit = new QLineEdit;
		ObjectSphere_CenterZ_QLineEdit->setFixedSize(Window_width * 0.035, Window_height * 0.028);
		ObjectSphere_CenterZ_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectSphere_CenterZ_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME4);
		ObjectSphere_CenterZ_QLineEdit->setFont(font_D_LE4);
		ObjectSphere_CenterZ_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(cm)"));
		ObjectSphere_CenterZ_QLineEdit->setValidator(ValFactory::makeReal(ObjectSphere_CenterZ_QLineEdit));

		hLayout->addWidget(ObjectSphere_Center_QLabel);
		hLayout->addWidget(ObjectSphere_CenterX_QLineEdit);
		hLayout->addWidget(ObjectSphere_CenterY_QLineEdit);
		hLayout->addWidget(ObjectSphere_CenterZ_QLineEdit);

		subLayout->addLayout(hLayout);
	}
	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(Window_width * 0.005, Window_height * 0.005, 0, 0);

		ObjectSphere_Rotate_QLabel = new QLabel;
		ObjectSphere_Rotate_QLabel->setFixedSize(Window_width * 0.065, Window_height * 0.028);
		ObjectSphere_Rotate_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		ObjectSphere_Rotate_QLabel->setFont(font_D_LBL1);
		ObjectSphere_Rotate_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		ObjectSphere_Rotate_QLabel->setText(QString::fromLocal8Bit("Rotate XYZ:"));

		ObjectSphere_RotateX_QLineEdit = new QLineEdit;
		ObjectSphere_RotateX_QLineEdit->setFixedSize(Window_width * 0.035, Window_height * 0.028);
		ObjectSphere_RotateX_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectSphere_RotateX_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME4);
		ObjectSphere_RotateX_QLineEdit->setFont(font_D_LE4);
		ObjectSphere_RotateX_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(deg)"));
		ObjectSphere_RotateX_QLineEdit->setValidator(ValFactory::makePositiveReal(ObjectSphere_RotateX_QLineEdit));

		ObjectSphere_RotateY_QLineEdit = new QLineEdit;
		ObjectSphere_RotateY_QLineEdit->setFixedSize(Window_width * 0.035, Window_height * 0.028);
		ObjectSphere_RotateY_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectSphere_RotateY_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME4);
		ObjectSphere_RotateY_QLineEdit->setFont(font_D_LE4);
		ObjectSphere_RotateY_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(deg)"));
		ObjectSphere_RotateY_QLineEdit->setValidator(ValFactory::makePositiveReal(ObjectSphere_RotateY_QLineEdit));

		ObjectSphere_RotateZ_QLineEdit = new QLineEdit;
		ObjectSphere_RotateZ_QLineEdit->setFixedSize(Window_width * 0.035, Window_height * 0.028);
		ObjectSphere_RotateZ_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectSphere_RotateZ_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME4);
		ObjectSphere_RotateZ_QLineEdit->setFont(font_D_LE4);
		ObjectSphere_RotateZ_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(deg)"));
		ObjectSphere_RotateZ_QLineEdit->setValidator(ValFactory::makePositiveReal(ObjectSphere_RotateZ_QLineEdit));

		hLayout->addWidget(ObjectSphere_Rotate_QLabel);
		hLayout->addWidget(ObjectSphere_RotateX_QLineEdit);
		hLayout->addWidget(ObjectSphere_RotateY_QLineEdit);
		hLayout->addWidget(ObjectSphere_RotateZ_QLineEdit);

		subLayout->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(Window_width * 0.005, Window_height * 0.005, 0, 0);

		ObjectSphere_Material_QLabel = new QLabel;
		ObjectSphere_Material_QLabel->setFixedSize(Window_width * 0.065, Window_height * 0.028);
		ObjectSphere_Material_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		ObjectSphere_Material_QLabel->setFont(font_D_LBL1);
		ObjectSphere_Material_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		ObjectSphere_Material_QLabel->setText(QString::fromLocal8Bit("Material:"));

		Object_SphereMaterialEdit_QPushButton = new QPushButton(QString::fromLocal8Bit("Edit"));
		Buttons_FunctionPanelRight.append(Object_SphereMaterialEdit_QPushButton);
		Object_SphereMaterialEdit_QPushButton->setFixedSize(Window_width * 0.028, Window_height * 0.025);
		Object_SphereMaterialEdit_QPushButton->setStyleSheet(DialogStyle::MENU_BTN2);
		Object_SphereMaterialEdit_QPushButton->setFont(font_D_BTN2);
		Object_SphereMaterialEdit_QPushButton->setCheckable(false);
		ObjectSphere_MaterialPanelTextWindow_UserQDialog = new ObjectMaterialTextWindow(this); // Initialize TextWindow
		ObjectSphere_MaterialPanelTextWindow_UserQDialog->textEdit->setFont(font_D_MAT);

		ObjectSphere_MaterialMassFraction_QRadioButton = new QRadioButton(QString::fromLocal8Bit("Mass\nFraction"));
		ObjectSphere_MaterialMassFraction_QRadioButton->setStyleSheet(DialogStyle::RADIOBUTTON_WIDGET);
		ObjectSphere_MaterialMassFraction_QRadioButton->setFont(font_D_RB2);
		ObjectSphere_MaterialMassFraction_QRadioButton->setFixedSize(Window_width * 0.039, Window_height * 0.028);

		ObjectSphere_MaterialAtomFraction_QRadioButton = new QRadioButton(QString::fromLocal8Bit("Atom\nFraction"));
		ObjectSphere_MaterialAtomFraction_QRadioButton->setStyleSheet(DialogStyle::RADIOBUTTON_WIDGET);
		ObjectSphere_MaterialAtomFraction_QRadioButton->setFont(font_D_RB2);
		ObjectSphere_MaterialAtomFraction_QRadioButton->setFixedSize(Window_width * 0.039, Window_height * 0.028);

		QButtonGroup* formatGroup = new QButtonGroup;
		formatGroup->addButton(ObjectSphere_MaterialMassFraction_QRadioButton);
		formatGroup->addButton(ObjectSphere_MaterialAtomFraction_QRadioButton);
		ObjectSphere_MaterialMassFraction_QRadioButton->setChecked(true);

		connect(Object_SphereMaterialEdit_QPushButton, SIGNAL(clicked()), this, SLOT(slot_ObjectPanel_MaterialEdit_ButtonClicked())); connect(Object_SphereMaterialEdit_QPushButton, SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));

		hLayout->addWidget(ObjectSphere_Material_QLabel);
		hLayout->addWidget(Object_SphereMaterialEdit_QPushButton);
		hLayout->addWidget(ObjectSphere_MaterialMassFraction_QRadioButton);
		hLayout->addWidget(ObjectSphere_MaterialAtomFraction_QRadioButton);

		subLayout->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(Window_width * 0.005, Window_height * 0.005, 0, 0);

		ObjectSphere_Density_QLabel = new QLabel;
		ObjectSphere_Density_QLabel->setFixedSize(Window_width * 0.065, Window_height * 0.028);
		ObjectSphere_Density_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		ObjectSphere_Density_QLabel->setFont(font_D_LBL1);
		ObjectSphere_Density_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		ObjectSphere_Density_QLabel->setText(QString::fromLocal8Bit("Density:"));

		ObjectSphere_Density_QLineEdit = new QLineEdit;
		ObjectSphere_Density_QLineEdit->setFixedSize(Window_width * 0.115, Window_height * 0.028);
		ObjectSphere_Density_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectSphere_Density_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME4);
		ObjectSphere_Density_QLineEdit->setFont(font_D_LE4);
		ObjectSphere_Density_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(g/cm3)"));
		ObjectSphere_Density_QLineEdit->setValidator(ValFactory::makePositiveReal(ObjectSphere_Density_QLineEdit));

		hLayout->addWidget(ObjectSphere_Density_QLabel);
		hLayout->addWidget(ObjectSphere_Density_QLineEdit);

		subLayout->addLayout(hLayout);
	}

	{// Update
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setContentsMargins(Window_width * 0.005, Window_height * 0.005, 0, 0);
		hLayout->setSpacing(Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

		Object_SphereUpdate_QPushButton = new QPushButton(QString::fromLocal8Bit("Object Update"));
		Buttons_FunctionPanelRight.append(Object_SphereUpdate_QPushButton);
		Object_SphereUpdate_QPushButton->setFixedSize(Window_width * 0.104, Window_height * 0.028);
		Object_SphereUpdate_QPushButton->setStyleSheet(DialogStyle::MENU_BTN2);
		Object_SphereUpdate_QPushButton->setFont(font_D_BTN2);
		connect(Object_SphereUpdate_QPushButton, SIGNAL(clicked()), this, SLOT(slot_ObjectUpdate_ButtonClicked())); connect(Object_SphereUpdate_QPushButton, SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
		hLayout->addWidget(Object_SphereUpdate_QPushButton);

		subLayout->addLayout(hLayout);
	}
	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(Window_width * 0.005, Window_height * 0.005, 0, 0);

		ObjectSphere_Transparency_QLabel = new QLabel;
		ObjectSphere_Transparency_QLabel->setFixedSize(Window_width * 0.042, Window_height * 0.035);
		ObjectSphere_Transparency_QLabel->setStyleSheet(DialogStyle::DATA_LABEL4);
		ObjectSphere_Transparency_QLabel->setFont(font_D_LBL4);
		ObjectSphere_Transparency_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignCenter);
		ObjectSphere_Transparency_QLabel->setText(QString::fromLocal8Bit("Transparency:"));

		Object_SphereTransparency_QSlider = new QSlider(Qt::Horizontal);
		Object_SphereTransparency_QSlider->setFixedSize(Window_width * 0.135, Window_height * 0.018);
		Object_SphereTransparency_QSlider->setTickPosition(QSlider::TicksAbove);
		Object_SphereTransparency_QSlider->setTickInterval(25);
		Object_SphereTransparency_QSlider->setSingleStep(1);
		Object_SphereTransparency_QSlider->setMinimum(0);
		Object_SphereTransparency_QSlider->setMaximum(100);
		Object_SphereTransparency_QSlider->setStyleSheet("QSlider{background: #B9D7C4;}\
												QSlider::handle:horizontal {background-color: B9B9FF;}");
		Object_SphereTransparency_QSlider->setValue(50);

		connect(Object_SphereTransparency_QSlider, SIGNAL(valueChanged(int)), this, SLOT(slot_ObjectTransparencySlider(int))); connect(Object_SphereTransparency_QSlider, SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));

		hLayout->addWidget(ObjectSphere_Transparency_QLabel);
		hLayout->addWidget(Object_SphereTransparency_QSlider);

		subLayout->addLayout(hLayout);
	}
	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(Window_width * 0.005, Window_height * 0.005, 0, 0);

		ObjectSphere_ChargedParticleTransport_QLabel = new QLabel;
		ObjectSphere_ChargedParticleTransport_QLabel->setFixedSize(Window_width * 0.105, Window_height * 0.028);
		ObjectSphere_ChargedParticleTransport_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		ObjectSphere_ChargedParticleTransport_QLabel->setFont(font_D_LBL1);
		ObjectSphere_ChargedParticleTransport_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignCenter);
		ObjectSphere_ChargedParticleTransport_QLabel->setText(QString::fromLocal8Bit("Charged particle transport:"));

		Object_Sphere_ChargedParticleTransport_QPushButton = new QPushButton(QString::fromLocal8Bit("On"));
		Buttons_FunctionPanelRight.append(Object_Sphere_ChargedParticleTransport_QPushButton);
		Object_Sphere_ChargedParticleTransport_QPushButton->setFixedSize(Window_width * 0.028, Window_height * 0.028);
		Object_Sphere_ChargedParticleTransport_QPushButton->setCheckable(true);
		Object_Sphere_ChargedParticleTransport_QPushButton->setChecked(true);
		Object_Sphere_ChargedParticleTransport_QPushButton->setStyleSheet(DialogStyle::MENU_BTN3);
		Object_Sphere_ChargedParticleTransport_QPushButton->setFont(font_D_BTN3);

		connect(Object_Sphere_ChargedParticleTransport_QPushButton, SIGNAL(clicked()), this, SLOT(slot_ObjectChargedParticleTransport_ButtonClicked())); connect(Object_Sphere_ChargedParticleTransport_QPushButton, SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));

		hLayout->addWidget(ObjectSphere_ChargedParticleTransport_QLabel);
		hLayout->addWidget(Object_Sphere_ChargedParticleTransport_QPushButton);

		subLayout->addLayout(hLayout);
	}
	ObjectSphere_QGroupBox->setLayout(subLayout);
	layout->addWidget(ObjectSphere_QGroupBox);
}

////////////////// Cylinder ////////////////// 

// Main function - Status
void FunctionPanelRight::CreateObjectCylinderWidgets(QVBoxLayout* layout)
{
	ObjectCylinder_QGroupBox = new QGroupBox();
	ObjectCylinder_QGroupBox->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	ObjectCylinder_QGroupBox->setStyleSheet(DialogStyle::GROUPBOX_WIDGET7);
	ObjectCylinder_QGroupBox->setFont(font_D_GBW7);
	ObjectCylinder_QGroupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	ObjectCylinder_QGroupBox->setFixedHeight(Window_height * 0.36);

	QVBoxLayout* subLayout = new QVBoxLayout;
	subLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.005, Window_width * 0.003, Window_height * 0.005);
	subLayout->setSpacing(Window_width * 0.003);
	subLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(Window_width * 0.005, Window_height * 0.005, 0, 0);

		ObjectCylinder_Type_QLabel = new QLabel;
		ObjectCylinder_Type_QLabel->setFixedSize(Window_width * 0.065, Window_height * 0.028);
		ObjectCylinder_Type_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		ObjectCylinder_Type_QLabel->setFont(font_D_LBL1);
		ObjectCylinder_Type_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		ObjectCylinder_Type_QLabel->setText(QString::fromLocal8Bit("Type:"));

		ObjectCylinder_Type_QLineEdit = new QLineEdit;
		ObjectCylinder_Type_QLineEdit->setFixedSize(Window_width * 0.115, Window_height * 0.028);
		ObjectCylinder_Type_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectCylinder_Type_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME4);
		ObjectCylinder_Type_QLineEdit->setFont(font_D_LE4);
		ObjectCylinder_Type_QLineEdit->setReadOnly(true);
		ObjectCylinder_Type_QLineEdit->setText("Cylinder");

		hLayout->addWidget(ObjectCylinder_Type_QLabel);
		hLayout->addWidget(ObjectCylinder_Type_QLineEdit);

		subLayout->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(Window_width * 0.005, Window_height * 0.005, 0, 0);

		ObjectCylinder_Radius_QLabel = new QLabel;
		ObjectCylinder_Radius_QLabel->setFixedSize(Window_width * 0.065, Window_height * 0.028);
		ObjectCylinder_Radius_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		ObjectCylinder_Radius_QLabel->setFont(font_D_LBL1);
		ObjectCylinder_Radius_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		ObjectCylinder_Radius_QLabel->setText(QString::fromLocal8Bit("R min/max:"));

		ObjectCylinder_InnerRadius_QLineEdit = new QLineEdit;
		ObjectCylinder_InnerRadius_QLineEdit->setFixedSize(Window_width * 0.035, Window_height * 0.028);
		ObjectCylinder_InnerRadius_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectCylinder_InnerRadius_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME4);
		ObjectCylinder_InnerRadius_QLineEdit->setFont(font_D_LE4);
		ObjectCylinder_InnerRadius_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(cm)"));
		ObjectCylinder_InnerRadius_QLineEdit->setValidator(ValFactory::makePositiveReal(ObjectCylinder_InnerRadius_QLineEdit));

		ObjectCylinder_OuterRadius_QLineEdit = new QLineEdit;
		ObjectCylinder_OuterRadius_QLineEdit->setFixedSize(Window_width * 0.035, Window_height * 0.028);
		ObjectCylinder_OuterRadius_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectCylinder_OuterRadius_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME4);
		ObjectCylinder_OuterRadius_QLineEdit->setFont(font_D_LE4);
		ObjectCylinder_OuterRadius_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(cm)"));
		ObjectCylinder_OuterRadius_QLineEdit->setValidator(ValFactory::makePositiveReal(ObjectCylinder_OuterRadius_QLineEdit));

		hLayout->addWidget(ObjectCylinder_Radius_QLabel);
		hLayout->addWidget(ObjectCylinder_InnerRadius_QLineEdit);
		hLayout->addWidget(ObjectCylinder_OuterRadius_QLineEdit);

		subLayout->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(Window_width * 0.005, Window_height * 0.005, 0, 0);

		ObjectCylinder_HeightZ_QLabel = new QLabel;
		ObjectCylinder_HeightZ_QLabel->setFixedSize(Window_width * 0.065, Window_height * 0.028);
		ObjectCylinder_HeightZ_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		ObjectCylinder_HeightZ_QLabel->setFont(font_D_LBL1);
		ObjectCylinder_HeightZ_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		ObjectCylinder_HeightZ_QLabel->setText(QString::fromLocal8Bit("Half height Z:"));

		ObjectCylinder_HeightZ_QLineEdit = new QLineEdit;
		ObjectCylinder_HeightZ_QLineEdit->setFixedSize(Window_width * 0.035, Window_height * 0.028);
		ObjectCylinder_HeightZ_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectCylinder_HeightZ_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME4);
		ObjectCylinder_HeightZ_QLineEdit->setFont(font_D_LE4);
		ObjectCylinder_HeightZ_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(cm)"));
		ObjectCylinder_HeightZ_QLineEdit->setValidator(ValFactory::makePositiveReal(ObjectCylinder_HeightZ_QLineEdit));

		hLayout->addWidget(ObjectCylinder_HeightZ_QLabel);
		hLayout->addWidget(ObjectCylinder_HeightZ_QLineEdit);

		subLayout->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(Window_width * 0.005, Window_height * 0.005, 0, 0);

		ObjectCylinder_Angle_QLabel = new QLabel;
		ObjectCylinder_Angle_QLabel->setFixedSize(Window_width * 0.065, Window_height * 0.028);
		ObjectCylinder_Angle_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		ObjectCylinder_Angle_QLabel->setFont(font_D_LBL1);
		ObjectCylinder_Angle_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		ObjectCylinder_Angle_QLabel->setText(QString::fromLocal8Bit("S./D. ang.:"));

		ObjectCylinder_StartingAngle_QLineEdit = new QLineEdit;
		ObjectCylinder_StartingAngle_QLineEdit->setFixedSize(Window_width * 0.035, Window_height * 0.028);
		ObjectCylinder_StartingAngle_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectCylinder_StartingAngle_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME4);
		ObjectCylinder_StartingAngle_QLineEdit->setFont(font_D_LE4);
		ObjectCylinder_StartingAngle_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(deg)"));
		ObjectCylinder_StartingAngle_QLineEdit->setValidator(ValFactory::makePositiveReal(ObjectCylinder_StartingAngle_QLineEdit));

		ObjectCylinder_DeltaAngle_QLineEdit = new QLineEdit;
		ObjectCylinder_DeltaAngle_QLineEdit->setFixedSize(Window_width * 0.035, Window_height * 0.028);
		ObjectCylinder_DeltaAngle_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectCylinder_DeltaAngle_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME4);
		ObjectCylinder_DeltaAngle_QLineEdit->setFont(font_D_LE4);
		ObjectCylinder_DeltaAngle_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(deg)"));
		ObjectCylinder_DeltaAngle_QLineEdit->setValidator(ValFactory::makePositiveReal(ObjectCylinder_DeltaAngle_QLineEdit));

		hLayout->addWidget(ObjectCylinder_Angle_QLabel);
		hLayout->addWidget(ObjectCylinder_StartingAngle_QLineEdit);
		hLayout->addWidget(ObjectCylinder_DeltaAngle_QLineEdit);

		subLayout->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(Window_width * 0.005, Window_height * 0.005, 0, 0);

		ObjectCylinder_Center_QLabel = new QLabel;
		ObjectCylinder_Center_QLabel->setFixedSize(Window_width * 0.065, Window_height * 0.028);
		ObjectCylinder_Center_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		ObjectCylinder_Center_QLabel->setFont(font_D_LBL1);
		ObjectCylinder_Center_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		ObjectCylinder_Center_QLabel->setText(QString::fromLocal8Bit("Center XYZ:"));

		ObjectCylinder_CenterX_QLineEdit = new QLineEdit;
		ObjectCylinder_CenterX_QLineEdit->setFixedSize(Window_width * 0.035, Window_height * 0.028);
		ObjectCylinder_CenterX_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectCylinder_CenterX_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME4);
		ObjectCylinder_CenterX_QLineEdit->setFont(font_D_LE4);
		ObjectCylinder_CenterX_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(cm)"));
		ObjectCylinder_CenterX_QLineEdit->setValidator(ValFactory::makeReal(ObjectCylinder_CenterX_QLineEdit));

		ObjectCylinder_CenterY_QLineEdit = new QLineEdit;
		ObjectCylinder_CenterY_QLineEdit->setFixedSize(Window_width * 0.035, Window_height * 0.028);
		ObjectCylinder_CenterY_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectCylinder_CenterY_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME4);
		ObjectCylinder_CenterY_QLineEdit->setFont(font_D_LE4);
		ObjectCylinder_CenterY_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(cm)"));
		ObjectCylinder_CenterY_QLineEdit->setValidator(ValFactory::makeReal(ObjectCylinder_CenterY_QLineEdit));

		ObjectCylinder_CenterZ_QLineEdit = new QLineEdit;
		ObjectCylinder_CenterZ_QLineEdit->setFixedSize(Window_width * 0.035, Window_height * 0.028);
		ObjectCylinder_CenterZ_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectCylinder_CenterZ_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME4);
		ObjectCylinder_CenterZ_QLineEdit->setFont(font_D_LE4);
		ObjectCylinder_CenterZ_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(cm)"));
		ObjectCylinder_CenterZ_QLineEdit->setValidator(ValFactory::makeReal(ObjectCylinder_CenterZ_QLineEdit));

		hLayout->addWidget(ObjectCylinder_Center_QLabel);
		hLayout->addWidget(ObjectCylinder_CenterX_QLineEdit);
		hLayout->addWidget(ObjectCylinder_CenterY_QLineEdit);
		hLayout->addWidget(ObjectCylinder_CenterZ_QLineEdit);

		subLayout->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(Window_width * 0.005, Window_height * 0.005, 0, 0);

		ObjectCylinder_Rotate_QLabel = new QLabel;
		ObjectCylinder_Rotate_QLabel->setFixedSize(Window_width * 0.065, Window_height * 0.028);
		ObjectCylinder_Rotate_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		ObjectCylinder_Rotate_QLabel->setFont(font_D_LBL1);
		ObjectCylinder_Rotate_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		ObjectCylinder_Rotate_QLabel->setText(QString::fromLocal8Bit("Rotate XYZ:"));

		ObjectCylinder_RotateX_QLineEdit = new QLineEdit;
		ObjectCylinder_RotateX_QLineEdit->setFixedSize(Window_width * 0.035, Window_height * 0.028);
		ObjectCylinder_RotateX_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectCylinder_RotateX_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME4);
		ObjectCylinder_RotateX_QLineEdit->setFont(font_D_LE4);
		ObjectCylinder_RotateX_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(deg)"));
		ObjectCylinder_RotateX_QLineEdit->setValidator(ValFactory::makePositiveReal(ObjectCylinder_RotateX_QLineEdit));

		ObjectCylinder_RotateY_QLineEdit = new QLineEdit;
		ObjectCylinder_RotateY_QLineEdit->setFixedSize(Window_width * 0.035, Window_height * 0.028);
		ObjectCylinder_RotateY_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectCylinder_RotateY_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME4);
		ObjectCylinder_RotateY_QLineEdit->setFont(font_D_LE4);
		ObjectCylinder_RotateY_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(deg)"));
		ObjectCylinder_RotateY_QLineEdit->setValidator(ValFactory::makePositiveReal(ObjectCylinder_RotateY_QLineEdit));

		ObjectCylinder_RotateZ_QLineEdit = new QLineEdit;
		ObjectCylinder_RotateZ_QLineEdit->setFixedSize(Window_width * 0.035, Window_height * 0.028);
		ObjectCylinder_RotateZ_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectCylinder_RotateZ_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME4);
		ObjectCylinder_RotateZ_QLineEdit->setFont(font_D_LE4);
		ObjectCylinder_RotateZ_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(deg)"));
		ObjectCylinder_RotateZ_QLineEdit->setValidator(ValFactory::makePositiveReal(ObjectCylinder_RotateZ_QLineEdit));

		hLayout->addWidget(ObjectCylinder_Rotate_QLabel);
		hLayout->addWidget(ObjectCylinder_RotateX_QLineEdit);
		hLayout->addWidget(ObjectCylinder_RotateY_QLineEdit);
		hLayout->addWidget(ObjectCylinder_RotateZ_QLineEdit);

		subLayout->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(Window_width * 0.005, Window_height * 0.005, 0, 0);

		ObjectCylinder_Material_QLabel = new QLabel;
		ObjectCylinder_Material_QLabel->setFixedSize(Window_width * 0.065, Window_height * 0.028);
		ObjectCylinder_Material_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		ObjectCylinder_Material_QLabel->setFont(font_D_LBL1);
		ObjectCylinder_Material_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		ObjectCylinder_Material_QLabel->setText(QString::fromLocal8Bit("Material:"));

		Object_CylinderMaterialEdit_QPushButton = new QPushButton(QString::fromLocal8Bit("Edit"));
		Buttons_FunctionPanelRight.append(Object_CylinderMaterialEdit_QPushButton);
		Object_CylinderMaterialEdit_QPushButton->setFixedSize(Window_width * 0.028, Window_height * 0.025);
		Object_CylinderMaterialEdit_QPushButton->setStyleSheet(DialogStyle::MENU_BTN2);
		Object_CylinderMaterialEdit_QPushButton->setFont(font_D_BTN2);
		Object_CylinderMaterialEdit_QPushButton->setCheckable(false);
		ObjectCylinder_MaterialPanelTextWindow_UserQDialog = new ObjectMaterialTextWindow(this); // Initialize TextWindow
		ObjectCylinder_MaterialPanelTextWindow_UserQDialog->textEdit->setFont(font_D_MAT);

		ObjectCylinder_MaterialMassFraction_QRadioButton = new QRadioButton(QString::fromLocal8Bit("Mass\nFraction"));
		ObjectCylinder_MaterialMassFraction_QRadioButton->setStyleSheet(DialogStyle::RADIOBUTTON_WIDGET);
		ObjectCylinder_MaterialMassFraction_QRadioButton->setFont(font_D_RB2);
		ObjectCylinder_MaterialMassFraction_QRadioButton->setFixedSize(Window_width * 0.039, Window_height * 0.028);

		ObjectCylinder_MaterialAtomFraction_QRadioButton = new QRadioButton(QString::fromLocal8Bit("Atom\nFraction"));
		ObjectCylinder_MaterialAtomFraction_QRadioButton->setStyleSheet(DialogStyle::RADIOBUTTON_WIDGET);
		ObjectCylinder_MaterialAtomFraction_QRadioButton->setFont(font_D_RB2);
		ObjectCylinder_MaterialAtomFraction_QRadioButton->setFixedSize(Window_width * 0.039, Window_height * 0.028);

		QButtonGroup* formatGroup = new QButtonGroup;
		formatGroup->addButton(ObjectCylinder_MaterialMassFraction_QRadioButton);
		formatGroup->addButton(ObjectCylinder_MaterialAtomFraction_QRadioButton);
		ObjectCylinder_MaterialMassFraction_QRadioButton->setChecked(true);

		connect(Object_CylinderMaterialEdit_QPushButton, SIGNAL(clicked()), this, SLOT(slot_ObjectPanel_MaterialEdit_ButtonClicked())); connect(Object_CylinderMaterialEdit_QPushButton, SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));

		hLayout->addWidget(ObjectCylinder_Material_QLabel);
		hLayout->addWidget(Object_CylinderMaterialEdit_QPushButton);
		hLayout->addWidget(ObjectCylinder_MaterialMassFraction_QRadioButton);
		hLayout->addWidget(ObjectCylinder_MaterialAtomFraction_QRadioButton);

		subLayout->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(Window_width * 0.005, Window_height * 0.005, 0, 0);

		ObjectCylinder_Density_QLabel = new QLabel;
		ObjectCylinder_Density_QLabel->setFixedSize(Window_width * 0.065, Window_height * 0.028);
		ObjectCylinder_Density_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		ObjectCylinder_Density_QLabel->setFont(font_D_LBL1);
		ObjectCylinder_Density_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		ObjectCylinder_Density_QLabel->setText(QString::fromLocal8Bit("Density:"));

		ObjectCylinder_Density_QLineEdit = new QLineEdit;
		ObjectCylinder_Density_QLineEdit->setFixedSize(Window_width * 0.115, Window_height * 0.028);
		ObjectCylinder_Density_QLineEdit->setAlignment(Qt::AlignCenter);
		ObjectCylinder_Density_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME4);
		ObjectCylinder_Density_QLineEdit->setFont(font_D_LE4);
		ObjectCylinder_Density_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(g/cm3)"));
		ObjectCylinder_Density_QLineEdit->setValidator(ValFactory::makePositiveReal(ObjectCylinder_Density_QLineEdit));

		hLayout->addWidget(ObjectCylinder_Density_QLabel);
		hLayout->addWidget(ObjectCylinder_Density_QLineEdit);

		subLayout->addLayout(hLayout);
	}

	{// Update
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setContentsMargins(Window_width * 0.005, Window_height * 0.005, 0, 0);
		hLayout->setSpacing(Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

		Object_CylinderUpdate_QPushButton = new QPushButton(QString::fromLocal8Bit("Object Update"));
		Buttons_FunctionPanelRight.append(Object_CylinderUpdate_QPushButton);
		Object_CylinderUpdate_QPushButton->setFixedSize(Window_width * 0.104, Window_height * 0.028);
		Object_CylinderUpdate_QPushButton->setStyleSheet(DialogStyle::MENU_BTN2);
		Object_CylinderUpdate_QPushButton->setFont(font_D_BTN2);
		connect(Object_CylinderUpdate_QPushButton, SIGNAL(clicked()), this, SLOT(slot_ObjectUpdate_ButtonClicked())); connect(Object_CylinderUpdate_QPushButton, SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
		hLayout->addWidget(Object_CylinderUpdate_QPushButton);

		subLayout->addLayout(hLayout);
	}
	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(Window_width * 0.005, Window_height * 0.005, 0, 0);

		ObjectCylinder_Transparency_QLabel = new QLabel;
		ObjectCylinder_Transparency_QLabel->setFixedSize(Window_width * 0.042, Window_height * 0.035);
		ObjectCylinder_Transparency_QLabel->setStyleSheet(DialogStyle::DATA_LABEL4);
		ObjectCylinder_Transparency_QLabel->setFont(font_D_LBL4);
		ObjectCylinder_Transparency_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignCenter);
		ObjectCylinder_Transparency_QLabel->setText(QString::fromLocal8Bit("Transparency:"));

		Object_CylinderTransparency_QSlider = new QSlider(Qt::Horizontal);
		Object_CylinderTransparency_QSlider->setFixedSize(Window_width * 0.135, Window_height * 0.018);
		Object_CylinderTransparency_QSlider->setTickPosition(QSlider::TicksAbove);
		Object_CylinderTransparency_QSlider->setTickInterval(25);
		Object_CylinderTransparency_QSlider->setSingleStep(1);
		Object_CylinderTransparency_QSlider->setMinimum(0);
		Object_CylinderTransparency_QSlider->setMaximum(100);
		Object_CylinderTransparency_QSlider->setStyleSheet("QSlider{background: #B9D7C4;}\
												QSlider::handle:horizontal {background-color: B9B9FF;}");
		Object_CylinderTransparency_QSlider->setValue(50);

		connect(Object_CylinderTransparency_QSlider, SIGNAL(valueChanged(int)), this, SLOT(slot_ObjectTransparencySlider(int))); connect(Object_CylinderTransparency_QSlider, SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));

		hLayout->addWidget(ObjectCylinder_Transparency_QLabel);
		hLayout->addWidget(Object_CylinderTransparency_QSlider);

		subLayout->addLayout(hLayout);
	}
	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(Window_width * 0.005, Window_height * 0.005, 0, 0);

		ObjectCylinder_ChargedParticleTransport_QLabel = new QLabel;
		ObjectCylinder_ChargedParticleTransport_QLabel->setFixedSize(Window_width * 0.105, Window_height * 0.028);
		ObjectCylinder_ChargedParticleTransport_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		ObjectCylinder_ChargedParticleTransport_QLabel->setFont(font_D_LBL1);
		ObjectCylinder_ChargedParticleTransport_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignCenter);
		ObjectCylinder_ChargedParticleTransport_QLabel->setText(QString::fromLocal8Bit("Charged particle transport:"));

		Object_Cylinder_ChargedParticleTransport_QPushButton = new QPushButton(QString::fromLocal8Bit("On"));
		Buttons_FunctionPanelRight.append(Object_Cylinder_ChargedParticleTransport_QPushButton);
		Object_Cylinder_ChargedParticleTransport_QPushButton->setFixedSize(Window_width * 0.028, Window_height * 0.028);
		Object_Cylinder_ChargedParticleTransport_QPushButton->setCheckable(true);
		Object_Cylinder_ChargedParticleTransport_QPushButton->setChecked(true);
		Object_Cylinder_ChargedParticleTransport_QPushButton->setStyleSheet(DialogStyle::MENU_BTN3);
		Object_Cylinder_ChargedParticleTransport_QPushButton->setFont(font_D_BTN3);

		connect(Object_Cylinder_ChargedParticleTransport_QPushButton, SIGNAL(clicked()), this, SLOT(slot_ObjectChargedParticleTransport_ButtonClicked())); connect(Object_Cylinder_ChargedParticleTransport_QPushButton, SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));

		hLayout->addWidget(ObjectCylinder_ChargedParticleTransport_QLabel);
		hLayout->addWidget(Object_Cylinder_ChargedParticleTransport_QPushButton);

		subLayout->addLayout(hLayout);
	}
	ObjectCylinder_QGroupBox->setLayout(subLayout);
	layout->addWidget(ObjectCylinder_QGroupBox);
}


//************************************** Source Widget **************************************//
void FunctionPanelRight::CreateSourceWidgets()
{
    // 1. 그룹박스 생성 (기존 유지)
    SourcePanel_QGroupBox = new QGroupBox();
    SourcePanel_QGroupBox->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    SourcePanel_QGroupBox->setStyleSheet(DialogStyle::GROUPBOX_WIDGET2);
    SourcePanel_QGroupBox->setFont(font_D_GBW2);
    SourcePanel_QGroupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // 2. 메인 레이아웃 설정 (기존 유지)
    QVBoxLayout* layout = new QVBoxLayout;
    layout->setContentsMargins(Window_width * 0.003, Window_height * 0.005, Window_width * 0.003, Window_height * 0.005);
    layout->setSpacing(Window_width * 0.003);
    layout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    SourcePanel_QGroupBox->setLayout(layout);

    // 3. 상단 콤보박스 및 OK 버튼 영역 (기존 코드 거의 유지)
    {     
        QHBoxLayout* hLayout = new QHBoxLayout;
        hLayout->setSpacing(Window_width * 0.005);
        hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        hLayout->setContentsMargins(0, Window_height * 0.009, 0, Window_height * 0.009);

        QLabel* labelText = new QLabel;
        labelText->setFixedSize(Window_width * 0.051, Window_height * 0.028);
        labelText->setStyleSheet(DialogStyle::DATA_LABEL);
        labelText->setFont(font_D_LBL1);
        labelText->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        labelText->setText(QString::fromLocal8Bit("Source type:"));

        m_comboBoxSourceGeometry = new QComboBox;
        m_comboBoxSourceGeometry->setFixedSize(Window_width * 0.104, Window_height * 0.028);
        m_comboBoxSourceGeometry->setStyleSheet(DialogStyle::COMBOBOX);
        QFont tmpfont2;
        tmpfont2.setPointSize(FontSizeScaling(9));
        m_comboBoxSourceGeometry->setFont(tmpfont2); 

        m_comboBoxSourceGeometry->setEditable(true); 
        m_comboBoxSourceGeometry->lineEdit()->setReadOnly(true);
        m_comboBoxSourceGeometry->lineEdit()->setAlignment(Qt::AlignCenter);
        QFont tmpfont;
        tmpfont.setPointSize(FontSizeScaling(8));
        m_comboBoxSourceGeometry->lineEdit()->setFont(tmpfont); 
    
        // [중요] 콤보박스 순서가 m_sources 리스트 순서와 일치해야 합니다!
        m_comboBoxSourceGeometry->addItem("ICRP standard geometries"); // 0: BroadBeam
        m_comboBoxSourceGeometry->addItem("Point source");             // 1: ExternalPoint
        m_comboBoxSourceGeometry->addItem("Floor disk source");        // 2: FloorDisk
        m_comboBoxSourceGeometry->addItem("Volume source");            // 3: ObjectVolume
        m_comboBoxSourceGeometry->addItem("Phase-space file");         // 4: PhaseSpace
        m_comboBoxSourceGeometry->addItem("Hot particle");             // 5: HotParticle
        m_comboBoxSourceGeometry->addItem("Cone beam");                // 6: ConeBeam
        m_comboBoxSourceGeometry->addItem("Room air contamination");   // 7: RoomContamination
        m_comboBoxSourceGeometry->addItem("ICRP environmental source"); // 8: EnvironmentalContamination
        m_comboBoxSourceGeometry->addItem("Parallel beam");            // 9: ParallelBeam
        m_comboBoxSourceGeometry->setCurrentIndex(0);

        m_sourceOKandResetButton = new QPushButton(QString::fromLocal8Bit("OK"));

        Buttons_FunctionPanelRight.append(m_sourceOKandResetButton);
        m_sourceOKandResetButton->setFixedSize(Window_width * 0.034, Window_height * 0.028);
        m_sourceOKandResetButton->setCheckable(false);
        m_sourceOKandResetButton->setStyleSheet(DialogStyle::MENU_BTN3);
        m_sourceOKandResetButton->setFont(font_D_BTN3);

        // 슬롯 이름 변경 주의 (ChangeSourceGeometryCombo -> SetSourceGeometryType 등 이름 맞추기)
        connect(m_comboBoxSourceGeometry, SIGNAL(currentIndexChanged(int)), this, SLOT(SetSourceGeometryType(int)));
        connect(m_sourceOKandResetButton, SIGNAL(clicked()), this, SLOT(slot_SourceOKandReset_ButtonClicked())); 
        connect(m_sourceOKandResetButton, SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));

        hLayout->addWidget(labelText);
        hLayout->addWidget(m_comboBoxSourceGeometry);
        hLayout->addWidget(m_sourceOKandResetButton);

        layout->addLayout(hLayout);
    }

    // =================================================================================
    // [리팩토링 핵심 부분] 기존 Create... 함수들을 지우고 리스트 초기화 코드로 대체합니다.
    // =================================================================================
    
// 4. 위젯들을 담을 컨테이너 레이아웃 생성
    m_sourceGeometryContainerLayout = new QVBoxLayout;
    m_sourceGeometryContainerLayout->setContentsMargins(0, 0, 0, 0);
    m_sourceGeometryContainerLayout->setSpacing(0);
    layout->addLayout(m_sourceGeometryContainerLayout);

    // 5. 리스트 초기화
    qDeleteAll(m_sources);
    m_sources.clear();

    // 6. 위젯 생성 (Factory 인자에서 this 제거!)
    m_sources.append(SourceGeometryWidgetFactory::CreateSourceGeometry("BroadBeam"));       // 0
    m_sources.append(SourceGeometryWidgetFactory::CreateSourceGeometry("ExternalPoint"));   // 1
    m_sources.append(SourceGeometryWidgetFactory::CreateSourceGeometry("FloorDisk"));       // 2
    m_sources.append(SourceGeometryWidgetFactory::CreateSourceGeometry("ObjectVolume"));    // 3
    m_sources.append(SourceGeometryWidgetFactory::CreateSourceGeometry("PhaseSpace"));      // 4
    m_sources.append(SourceGeometryWidgetFactory::CreateSourceGeometry("HotParticle"));     // 5
    m_sources.append(SourceGeometryWidgetFactory::CreateSourceGeometry("ConeBeam"));        // 6
    m_sources.append(SourceGeometryWidgetFactory::CreateSourceGeometry("RoomContamination")); // 7
    m_sources.append(SourceGeometryWidgetFactory::CreateSourceGeometry("EnvironmentalContamination")); // 8
    m_sources.append(SourceGeometryWidgetFactory::CreateSourceGeometry("ParallelBeam"));    // 9

    // 7. 생성된 위젯 초기화 및 배치
    for (auto* widget : m_sources) {
        if (widget) {
            // [매우 중요!] Factory에서 안 넘겨줬으므로, 여기서 수동으로 연결해야 합니다.
            widget->panel = this; 

            widget->initialize(); // 내부 UI 생성
            m_sourceGeometryContainerLayout->addWidget(widget); // 화면에 추가
            widget->hide();       // 일단 숨김
        }
    }

    // 8. 초기 화면 설정
    SetSourceGeometryType(0); // 0번(BroadBeam)을 기본값으로 설정
}
SourceGeometryWidget* FunctionPanelRight::getCurrentSourceWidget()
{
	int index = m_comboBoxSourceGeometry->currentIndex();

	if(index >= 0 && index < m_sources.size())
	{
		return m_sources[index];
	}
	else return nullptr;
}
void FunctionPanelRight::slot_ChangeSourceGeometryCombo(int idx)
{

}
void FunctionPanelRight::slot_SourceOKandReset_ButtonClicked()
{
	if (m_sourceOKandResetButton->text() == "OK")
	{
		if (m_comboBoxSourceGeometry->currentIndex() == 5) // Hot particle exception
		{
			if (m_Phantom_SequenceVector.size() != 1)
			{
				theApp.SetMessageBox("Hot particle source can be used only for the single phantom!");
				return;
			}
			else
			{
				theApp.UpdatePhantom_ActorHighlighted(m_Phantom_SequenceVector[0]);
			}
		}
		SetSourceGeometryType(m_comboBoxSourceGeometry->currentIndex());
		m_comboBoxSourceGeometry->setDisabled(true);
		m_sourceOKandResetButton->setText("Reset");
	}
	else if (m_sourceOKandResetButton->text() == "Reset")
	{
		if (QMessageBox::Yes == theApp.SetMessageBox_YesNo("Entered source information will be reset. Do you want to reset the source type?"))
		{
			// RESET PROCESS
			SetSourceGeometryType(-1);
			m_comboBoxSourceGeometry->setEnabled(true);
			m_sourceOKandResetButton->setText("OK");
			theApp.st_Pick3D_sourceCB = false;
			theApp.st_Pick3D_sourceEP = false;
			theApp.st_Pick3D_sourceHP = false;

		}
	}	
}//
void FunctionPanelRight::SetSourceGeometryType(int index)
{
	if (-1 == index)
	{
		CurrentSourceType = -1;
		for(int i = 0; i < m_sources.size(); ++i)
		{
			m_sources[i]->hide();
		}
	}
	else
	{
		for(int i = 0; i < m_sources.size(); ++i)
		{
			if (i == index)
			{
				CurrentSourceType = i;
				m_sources[i]->show();
			}
			else
			{
				m_sources[i]->hide();
			}
		}
	}
}


/////////////////////////////////////////////////////

//************************************** Output Widget **************************************//
void FunctionPanelRight::CreateOutputWidgets()
{
	// 그룹박스생성
	OutputPanel_QGroupBox = new QGroupBox();
	OutputPanel_QGroupBox->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	OutputPanel_QGroupBox->setStyleSheet(DialogStyle::GROUPBOX_WIDGET4);
	OutputPanel_QGroupBox->setFont(font_D_GBW4);
	OutputPanel_QGroupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	QVBoxLayout* layout = new QVBoxLayout;
	layout->setContentsMargins(Window_width * 0.003, Window_height * 0.005, Window_width * 0.003, Window_height * 0.005);
	layout->setSpacing(Window_width * 0.003);
	layout->setAlignment(Qt::AlignLeft | Qt::AlignTop);	

	OutputPanel_QGroupBox->setLayout(layout);	
	
	CreateOutputPhantomButtonWidget(layout); // Multiple phantom for output panel generation
	
	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(0, Window_height * 0.009, 0, Window_height * 0.009);		

		QLabel* labelResultFileName = new QLabel;
		labelResultFileName->setFixedSize(Window_width * 0.065, Window_height * 0.028);
		labelResultFileName->setStyleSheet(DialogStyle::DATA_LABEL);
		labelResultFileName->setFont(font_D_LBL1);
		labelResultFileName->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		labelResultFileName->setText(QString::fromLocal8Bit("Result file name:"));

		m_lineEditResultFileName = new QLineEdit;
		m_lineEditResultFileName->setFixedSize(Window_width * 0.135, Window_height * 0.022);
		m_lineEditResultFileName->setAlignment(Qt::AlignLeft);
		m_lineEditResultFileName->setStyleSheet(DialogStyle::LINEEDIT_NAME4);
		m_lineEditResultFileName->setFont(font_D_LE4);
		m_lineEditResultFileName->setReadOnly(true);

		QSpacerItem *spacer1 = new QSpacerItem(Window_width * 0.035, Window_height * 0.028); // Adjust spacing

		m_skinDoseVisualizationButton = new QPushButton(QString::fromLocal8Bit("3D Skin dose"));
		Buttons_FunctionPanelRight.append(m_skinDoseVisualizationButton);
		m_skinDoseVisualizationButton->setFixedSize(Window_width * 0.068, Window_height * 0.028);
		m_skinDoseVisualizationButton->setStyleSheet(DialogStyle::MENU_BTN3);
		m_skinDoseVisualizationButton->setFont(font_D_BTN3);
		m_skinDoseVisualizationButton->setCheckable(true);
		m_skinDoseVisualizationButton->setChecked(false);
		m_skinDoseVisualizationButton->setDisabled(true);

		connect(m_skinDoseVisualizationButton, SIGNAL(clicked()), this, SLOT(slot_SkinDoseVisualization_ButtonClicked())); connect(m_skinDoseVisualizationButton, SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
	
		hLayout->addWidget(labelResultFileName);
		hLayout->addWidget(m_lineEditResultFileName);
		hLayout->addSpacerItem(spacer1);
		hLayout->addWidget(m_skinDoseVisualizationButton);		
		layout->addLayout(hLayout);
	}
	CreateOutputResultWidget(layout);	
	
}
void FunctionPanelRight::CreateOutputPhantomButtonWidget(QVBoxLayout* layout)
{
	OutputPanel_OutputPhantomButtons_QGroupBox = new QGroupBox;
	OutputPanel_OutputPhantomButtons_QGroupBox->setAlignment(Qt::AlignCenter | Qt::AlignLeft);
	OutputPanel_OutputPhantomButtons_QGroupBox->setStyleSheet(DialogStyle::GROUPBOX_WIDGET11);
	OutputPanel_OutputPhantomButtons_QGroupBox->setFont(font_D_GBW11);
	OutputPanel_OutputPhantomButtons_QGroupBox->setFixedHeight(0.082 * Window_height);
	OutputPanel_OutputPhantomButtons_QGroupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed); // 가로, 세로 방향 크기	
	OutputPanel_OutputPhantomButtons_QGroupBox->setContentsMargins(Window_width * 0.001, 0, Window_width * 0.001, 0);

	layout->addWidget(OutputPanel_OutputPhantomButtons_QGroupBox);

	QVBoxLayout* layoutOut = new QVBoxLayout();
	layoutOut->setContentsMargins(Window_width * 0.001, 0, Window_width * 0.005, Window_height * 0.005);
	layoutOut->setSpacing(Window_width * 0.005);
	layoutOut->setAlignment(Qt::AlignLeft | Qt::AlignTop);

	QGroupBox* GroupBoxIn = new QGroupBox;
	GroupBoxIn->setAlignment(Qt::AlignCenter | Qt::AlignLeft);
	GroupBoxIn->setStyleSheet(DialogStyle::GROUPBOX_WIDGET15);
	GroupBoxIn->setFont(font_D_GBW11);
	GroupBoxIn->setFixedHeight(0.060 * Window_height);
	GroupBoxIn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	layoutOutputPhantomButtonList = new QHBoxLayout();
	layoutOutputPhantomButtonList->setContentsMargins(Window_width * 0.001, Window_height * 0.005, Window_width * 0.001, Window_height * 0.005); // 왼쪽, 위, 오른쪽, 아래의 여유공간(해당 layout 안에서의)
	layoutOutputPhantomButtonList->setSpacing(Window_width * 0.003);
	layoutOutputPhantomButtonList->setAlignment(Qt::AlignLeft | Qt::AlignTop);

	QScrollArea* scrollArea = new QScrollArea();
	scrollArea->setStyleSheet(DialogStyle::GROUPBOX_WIDGET11);
	scrollArea->setFont(font_D_GBW11);
	scrollArea->setMinimumHeight(0.060 * Window_height);
	scrollArea->setMaximumHeight(0.060 * Window_height);
	scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // do not show scroll bar

	GroupBoxIn->setLayout(layoutOutputPhantomButtonList);
	scrollArea->setWidget(GroupBoxIn);
	scrollArea->setWidgetResizable(true);
	layoutOut->addWidget(scrollArea);
	OutputPanel_OutputPhantomButtons_QGroupBox->setLayout(layoutOut);
	OutputPanel_OutputPhantomButtons_QGroupBox->hide();
	OutputPanel_OutputPhantomButtons_QGroupBox->show();
}
void FunctionPanelRight::slot_OutputPhantomSelect_ButtonClicked()
{
	QPushButton* buttonSender = qobject_cast<QPushButton*>(sender());
	OutputPhantomSelectedIndex = buttonSender->objectName().toInt();

	for (auto itr_phantom : m_Phantom_SequenceVector) // 최대 팬텀 개수
	{
		if (itr_phantom != OutputPhantomSelectedIndex)
		{
			OutputPhantomButton[itr_phantom]->setChecked(false);
		}
		else OutputPhantomButton[itr_phantom]->setChecked(true);
	}

	if (b_IsOutputResultLoaded)
	{
		// 선량결과 output 보이기
		for (int i = 0; i < OrganNameList.size(); i++)
		{
			//theApp.SetMessageBox(theApp.multiple_outputDoseRateEP[OutputPhantomSelectedIndex][i][0]);
			OutputPanel_DoseResult_QLineEdit[theApp.OutputDose_data[OutputPhantomSelectedIndex][i][0]]->setText(theApp.OutputDose_data[OutputPhantomSelectedIndex][i][1]);
		}
		// Time-Dose, organ-dose 그래프 보이기
		if(b_IsRealtimeCalculation) theApp.pRslt->ForcedGraphUpdate();
		if(!b_IsRealtimeCalculation) theApp.pRslt->ForcedGraphUpdate_OrganDose();

	}
	theApp.UpdatePhantom_ActorHighlighted(OutputPhantomSelectedIndex);
}
void FunctionPanelRight::slot_SkinDoseVisualization_ButtonClicked()
{
	if (m_skinDoseVisualizationButton->isChecked()) // Visualization ON
	{
		theApp.b_IsSkinDoseVisualizationOn = true;	
		
		for (auto phantomID : m_Phantom_SequenceVector)
		{
			if (m_Phantom_MainInfo[phantomID][E_PHANTOMMAININFO_DUMMY] == E_PHANTOMDUMMY_YES) continue;
			if (m_Phantom_MainInfo[phantomID][E_PHANTOMMAININFO_CATEGORY] == E_PHANTOMCATEGORY_AIR) continue;

			theApp.PhantomPanelActor[phantomID]->VisibilityOff();
			theApp.SkinPhantomActor[phantomID]->VisibilityOn();
			theApp.SkinDoseVisualization_ScalarBar->VisibilityOn();
			theApp.SkinDoseVisualization_Label0percent->VisibilityOn();
			theApp.SkinDoseVisualization_Label20percent->VisibilityOn();
			theApp.SkinDoseVisualization_Label40percent->VisibilityOn();
			theApp.SkinDoseVisualization_Label60percent->VisibilityOn();
			theApp.SkinDoseVisualization_Label80percent->VisibilityOn();
			theApp.SkinDoseVisualization_Label100percent->VisibilityOn();
		}
		theApp.m_pVTKWidget->renderWindow()->Render();
	}
	else // Skin 3D Visualization OFF
	{
		theApp.b_IsSkinDoseVisualizationOn = false;

		for (auto phantomID : m_Phantom_SequenceVector)
		{
			if (m_Phantom_MainInfo[phantomID][E_PHANTOMMAININFO_DUMMY] == E_PHANTOMDUMMY_YES) continue;
			if (m_Phantom_MainInfo[phantomID][E_PHANTOMMAININFO_CATEGORY] == E_PHANTOMCATEGORY_AIR) continue;

			theApp.PhantomPanelActor[phantomID]->VisibilityOn();
			theApp.SkinPhantomActor[phantomID]->VisibilityOff();
			theApp.SkinDoseVisualization_ScalarBar->VisibilityOff();
			theApp.SkinDoseVisualization_Label0percent->VisibilityOff();
			theApp.SkinDoseVisualization_Label20percent->VisibilityOff();
			theApp.SkinDoseVisualization_Label40percent->VisibilityOff();
			theApp.SkinDoseVisualization_Label60percent->VisibilityOff();
			theApp.SkinDoseVisualization_Label80percent->VisibilityOff();
			theApp.SkinDoseVisualization_Label100percent->VisibilityOff();
		}
		theApp.m_pVTKWidget->renderWindow()->Render();
	}
}

////////////////// Result //////////////////
void FunctionPanelRight::CreateOutputResultWidget(QVBoxLayout* layout)
{
	OutputPanel_Results_QGroupBox = new QGroupBox();
	OutputPanel_Results_QGroupBox->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	OutputPanel_Results_QGroupBox->setStyleSheet(DialogStyle::GROUPBOX_WIDGET5);
	OutputPanel_Results_QGroupBox->setFont(font_D_GBW5);
	OutputPanel_Results_QGroupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	QVBoxLayout* subLayout = new QVBoxLayout;
	subLayout->setContentsMargins(Window_width * 0.005, Window_height * 0.005, Window_width * 0.005, Window_height * 0.005);
	subLayout->setSpacing(Window_width * 0.003);
	subLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.005, 0, 0);
		hLayout->setSpacing(Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);


		QLabel* label0 = new QLabel;
		label0->setFixedSize(Window_width * 0.035, Window_height * 0.028);
		label0->setStyleSheet(DialogStyle::DATA_LABEL);
		label0->setFont(font_D_LBL1);
		label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		label0->setText(QString::fromLocal8Bit("Result:"));

		m_comboBox_resultType = new QComboBox;
		m_comboBox_resultType->setFixedSize(Window_width * 0.240, Window_height * 0.028);
		m_comboBox_resultType->setStyleSheet(DialogStyle::COMBOBOX3);
		m_comboBox_resultType->setFont(font_D_CB3);

		m_comboBox_resultType->setEditable(true);
		m_comboBox_resultType->lineEdit()->setReadOnly(true);
		m_comboBox_resultType->lineEdit()->setAlignment(Qt::AlignCenter);
		m_comboBox_resultType->lineEdit()->setFont(font_D_CB3);

		m_comboBox_resultType->addItem("Organ and tissue dose (pGy cm2 or pSv cm2)"); // Organ dose + detriment-weighted dose + WBD -> 초기화시에만 cm2이고, 이후 calculate 버튼 클릭시 선원종류에 따라 바뀜
		m_comboBox_resultType->addItem("Personal dose equivalent (pSv cm2)"); // Personal equivalent dose + detriment-weighted dose + WBD
		m_comboBox_resultType->addItem("Ambient dose equivalent / Air kerma (pGy cm2)");
		m_comboBox_resultType->addItem("Local skin dose distribution (pGy cm2)");
		m_comboBox_resultType->setCurrentIndex(0);

		connect(m_comboBox_resultType, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_Output_resultType(int)));

		hLayout->addWidget(label0);
		hLayout->addWidget(m_comboBox_resultType);

		subLayout->addLayout(hLayout);
	}

	CreateOutput_Dose(subLayout);
	CreateOutput_Dosimeter(subLayout);
	CreateOutput_H_AK(subLayout); 
	CreateOutput_LocalSkin(subLayout);

	OutputPanel_Results_QGroupBox->setLayout(subLayout);

	layout->addWidget(OutputPanel_Results_QGroupBox);	
}
void FunctionPanelRight::slot_Output_resultType(int index)
{
	SetResultType(index);
}
void FunctionPanelRight::CreateOutput_Dose(QVBoxLayout* layout)
{
	InnerResultPanel_Dose_QGroupBox = new QGroupBox();	
	InnerResultPanel_Dose_QGroupBox->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	InnerResultPanel_Dose_QGroupBox->setStyleSheet(DialogStyle::GROUPBOX_WIDGET5);
	InnerResultPanel_Dose_QGroupBox->setFont(font_D_GBW5);
	InnerResultPanel_Dose_QGroupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	QVBoxLayout* subLayout = new QVBoxLayout;
	subLayout->setContentsMargins(0, Window_height * 0.005, 0, 0);
	subLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

	QGroupBox* GroupBoxIn = new QGroupBox;
	GroupBoxIn->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	GroupBoxIn->setStyleSheet(DialogStyle::GROUPBOX_WIDGET5);
	GroupBoxIn->setFont(font_D_GBW5);
	GroupBoxIn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	QGroupBox* GroupBoxOut = new QGroupBox;
	GroupBoxOut->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	GroupBoxOut->setStyleSheet(DialogStyle::GROUPBOX_WIDGET5);
	GroupBoxOut->setFont(font_D_GBW5);
	GroupBoxOut->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	QVBoxLayout* layoutIn = new QVBoxLayout();
	layoutIn->setAlignment(Qt::AlignLeft | Qt::AlignTop);

	QVBoxLayout* layoutOut = new QVBoxLayout();
	layoutOut->setAlignment(Qt::AlignLeft | Qt::AlignTop);

	QScrollArea* scroll = new QScrollArea();
	scroll->setStyleSheet(DialogStyle::GROUPBOX_WIDGET5);
	scroll->setFont(font_D_GBW5);
	scroll->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	{
		QGroupBox* GroupBoxInnerIn = new QGroupBox;
		GroupBoxInnerIn->setAlignment(Qt::AlignTop | Qt::AlignLeft);
		GroupBoxInnerIn->setStyleSheet(DialogStyle::GROUPBOX_WIDGET5);
		GroupBoxInnerIn->setFont(font_D_GBW5);
		GroupBoxInnerIn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

		QGroupBox* GroupBoxInnerOut = new QGroupBox;
		GroupBoxInnerOut->setAlignment(Qt::AlignTop | Qt::AlignLeft);
		GroupBoxInnerOut->setStyleSheet(DialogStyle::GROUPBOX_WIDGET5);
		GroupBoxInnerOut->setFont(font_D_GBW5);
		GroupBoxInnerOut->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

		QVBoxLayout* layoutInnerIn = new QVBoxLayout();
		layoutInnerIn->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QVBoxLayout* layoutInnerOut = new QVBoxLayout();
		layoutInnerOut->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QScrollArea* scrollArea = new QScrollArea();
		scrollArea->setStyleSheet(DialogStyle::GROUPBOX_WIDGET5);
		scrollArea->setFont(font_D_GBW5);
		scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

		// Creating dose result upper half output panel
		for (int i = 0; i < OrganNameList.size(); i++) 
		{
			{
				QHBoxLayout* hLayout = new QHBoxLayout;
				hLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.002, Window_width * 0.003, Window_height * 0.002);
				hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

				QLabel* label0 = new QLabel;
				label0->setFixedSize(Window_width * 0.085, Window_height * 0.028);
				label0->setStyleSheet(DialogStyle::DATA_LABEL);
				label0->setFont(font_D_LBL1);
				label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
				label0->setText(OrganNameList[i]);

				OutputPanel_DoseResult_QLineEdit[OrganNameList[i]] = new QLineEdit;
				OutputPanel_DoseResult_QLineEdit[OrganNameList[i]]->setFixedSize(Window_width * 0.07 * SpecialWidgetSizeFactor, Window_height * 0.028);
				OutputPanel_DoseResult_QLineEdit[OrganNameList[i]]->setAlignment(Qt::AlignCenter);
				OutputPanel_DoseResult_QLineEdit[OrganNameList[i]]->setStyleSheet(DialogStyle::LINEEDIT_NAME);
				OutputPanel_DoseResult_QLineEdit[OrganNameList[i]]->setFont(font_D_LE1);
				OutputPanel_DoseResult_QLineEdit[OrganNameList[i]]->setPlaceholderText(QString::fromLocal8Bit("pGy or pSv")); 
				OutputPanel_DoseResult_QLineEdit[OrganNameList[i]]->setReadOnly(true);

				hLayout->addWidget(label0);
				hLayout->addStretch();
				hLayout->addWidget(OutputPanel_DoseResult_QLineEdit[OrganNameList[i]]);

				layoutInnerIn->addLayout(hLayout);
			}

			GroupBoxInnerIn->setLayout(layoutInnerIn);
			scrollArea->setWidget(GroupBoxInnerIn);
			scrollArea->setWidgetResizable(true);
			layoutInnerOut->addWidget(scrollArea);
			GroupBoxInnerOut->setLayout(layoutInnerOut);
		}
		
		layoutIn->addWidget(GroupBoxInnerOut);
	}

	// Creating dose result upper half output panel
	{
		QGroupBox* GroupBoxInnerIn2 = new QGroupBox;
		GroupBoxInnerIn2->setAlignment(Qt::AlignTop | Qt::AlignLeft);
		GroupBoxInnerIn2->setStyleSheet(DialogStyle::GROUPBOX_WIDGET5);
		GroupBoxInnerIn2->setFont(font_D_GBW5);
		GroupBoxInnerIn2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

		QGroupBox* GroupBoxInnerOut2 = new QGroupBox;
		GroupBoxInnerOut2->setAlignment(Qt::AlignTop | Qt::AlignLeft);
		GroupBoxInnerOut2->setStyleSheet(DialogStyle::GROUPBOX_WIDGET5);
		GroupBoxInnerOut2->setFont(font_D_GBW5);
		GroupBoxInnerOut2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

		OrganDose_BarGraph_QVBoxLayout = new QVBoxLayout();
		OrganDose_BarGraph_QVBoxLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QVBoxLayout* layoutInnerOut2 = new QVBoxLayout();
		layoutInnerOut2->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QScrollArea* scrollArea2 = new QScrollArea();
		scrollArea2->setStyleSheet(DialogStyle::GROUPBOX_WIDGET5);
		scrollArea2->setFont(font_D_GBW5);
		scrollArea2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		scrollArea2->setMinimumHeight(Window_height * 0.290);
		scrollArea2->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

		GroupBoxInnerIn2->setLayout(OrganDose_BarGraph_QVBoxLayout);
		scrollArea2->setWidget(GroupBoxInnerIn2);
		scrollArea2->setWidgetResizable(true);
		layoutInnerOut2->addWidget(scrollArea2);
		GroupBoxInnerOut2->setLayout(layoutInnerOut2);
		layoutIn->addWidget(GroupBoxInnerOut2);
	}
	GroupBoxIn->setLayout(layoutIn);
	scroll->setWidget(GroupBoxIn);
	scroll->setWidgetResizable(true);
	layoutOut->addWidget(scroll);
	GroupBoxOut->setLayout(layoutOut);
	subLayout->addWidget(GroupBoxOut);

	InnerResultPanel_Dose_QGroupBox->setLayout(subLayout);

	layout->addWidget(InnerResultPanel_Dose_QGroupBox);
}
void FunctionPanelRight::CreateOutput_Dosimeter(QVBoxLayout* layout)
{
	InnerResultPanel_Dosimeter_QGroupBox = new QGroupBox();
	InnerResultPanel_Dosimeter_QGroupBox->hide();
	InnerResultPanel_Dosimeter_QGroupBox->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	InnerResultPanel_Dosimeter_QGroupBox->setStyleSheet(DialogStyle::GROUPBOX_WIDGET5);
	InnerResultPanel_Dosimeter_QGroupBox->setFont(font_D_GBW5);
	InnerResultPanel_Dosimeter_QGroupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	QVBoxLayout* subLayout = new QVBoxLayout;
	subLayout->setContentsMargins(0, Window_height * 0.005, 0, 0);
	subLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

	QGroupBox* GroupBoxIn = new QGroupBox;
	GroupBoxIn->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	GroupBoxIn->setStyleSheet(DialogStyle::GROUPBOX_WIDGET5);
	GroupBoxIn->setFont(font_D_GBW5);
	GroupBoxIn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	QGroupBox* GroupBoxOut = new QGroupBox;
	GroupBoxOut->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	GroupBoxOut->setStyleSheet(DialogStyle::GROUPBOX_WIDGET5);
	GroupBoxOut->setFont(font_D_GBW5);
	GroupBoxOut->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	QVBoxLayout* layoutIn = new QVBoxLayout();
	layoutIn->setAlignment(Qt::AlignLeft | Qt::AlignTop);

	QVBoxLayout* layoutOut = new QVBoxLayout();
	layoutOut->setAlignment(Qt::AlignLeft | Qt::AlignTop);

	QScrollArea* scroll = new QScrollArea();
	scroll->setStyleSheet(DialogStyle::GROUPBOX_WIDGET5);
	scroll->setFont(font_D_GBW5);
	scroll->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	{
		QGroupBox* GroupBoxInnerIn = new QGroupBox;
		GroupBoxInnerIn->setAlignment(Qt::AlignTop | Qt::AlignLeft);
		GroupBoxInnerIn->setStyleSheet(DialogStyle::GROUPBOX_WIDGET5);
		GroupBoxInnerIn->setFont(font_D_GBW5);
		GroupBoxInnerIn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

		QGroupBox* GroupBoxInnerOut = new QGroupBox;
		GroupBoxInnerOut->setAlignment(Qt::AlignTop | Qt::AlignLeft);
		GroupBoxInnerOut->setStyleSheet(DialogStyle::GROUPBOX_WIDGET5);
		GroupBoxInnerOut->setFont(font_D_GBW5);
		GroupBoxInnerOut->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

		QVBoxLayout* layoutInnerIn = new QVBoxLayout();
		layoutInnerIn->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QVBoxLayout* layoutInnerOut = new QVBoxLayout();
		layoutInnerOut->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QScrollArea* scrollArea = new QScrollArea();
		scrollArea->setStyleSheet(DialogStyle::GROUPBOX_WIDGET5);
		scrollArea->setFont(font_D_GBW5);
		scrollArea->setFixedHeight(Window_height * 0.27);
		scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

		for (int i = 0; i < m_Dosimeter_Maximum_Count; i++) {
			{
				QHBoxLayout* hLayout = new QHBoxLayout;
				hLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.002, Window_width * 0.003, Window_height * 0.002);
				hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

				QLabel* label0 = new QLabel;
				label0->setFixedSize(Window_width * 0.085, Window_height * 0.028);
				label0->setStyleSheet(DialogStyle::DATA_LABEL);
				label0->setFont(font_D_LBL1);
				label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
				label0->setText("Dosimeter"); // 각 도시미터 이름

				OutputPanel_DosimeterResult_QLineEdit[i] = new QLineEdit;
				OutputPanel_DosimeterResult_QLineEdit[i]->setFixedSize(Window_width * 0.120, Window_height * 0.028);
				OutputPanel_DosimeterResult_QLineEdit[i]->setAlignment(Qt::AlignCenter);
				OutputPanel_DosimeterResult_QLineEdit[i]->setStyleSheet(DialogStyle::LINEEDIT_NAME);
				OutputPanel_DosimeterResult_QLineEdit[i]->setFont(font_D_LE1);
				OutputPanel_DosimeterResult_QLineEdit[i]->setPlaceholderText(QString::fromLocal8Bit("pSv"));
				OutputPanel_DosimeterResult_QLineEdit[i]->setReadOnly(true);

				hLayout->addWidget(label0);
				hLayout->addStretch();
				hLayout->addWidget(OutputPanel_DosimeterResult_QLineEdit[i]);


				layoutInnerIn->addLayout(hLayout);
			}

			GroupBoxInnerIn->setLayout(layoutInnerIn);
			scrollArea->setWidget(GroupBoxInnerIn);
			scrollArea->setWidgetResizable(true);
			layoutInnerOut->addWidget(scrollArea);
			GroupBoxInnerOut->setLayout(layoutInnerOut);
		}
		layoutIn->addWidget(GroupBoxInnerOut);
	}

	{
		QGroupBox* GroupBoxInnerIn2 = new QGroupBox;
		GroupBoxInnerIn2->setAlignment(Qt::AlignTop | Qt::AlignLeft);
		GroupBoxInnerIn2->setStyleSheet(DialogStyle::GROUPBOX_WIDGET5);
		GroupBoxInnerIn2->setFont(font_D_GBW5);
		GroupBoxInnerIn2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

		QGroupBox* GroupBoxInnerOut2 = new QGroupBox;
		GroupBoxInnerOut2->setAlignment(Qt::AlignTop | Qt::AlignLeft);
		GroupBoxInnerOut2->setStyleSheet(DialogStyle::GROUPBOX_WIDGET5);
		GroupBoxInnerOut2->setFont(font_D_GBW5);
		GroupBoxInnerOut2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

		DosimeterDose_BarGraph_QVBoxLayout = new QVBoxLayout();
		DosimeterDose_BarGraph_QVBoxLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QVBoxLayout* layoutInnerOut2 = new QVBoxLayout();
		layoutInnerOut2->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QScrollArea* scrollArea2 = new QScrollArea();
		scrollArea2->setStyleSheet(DialogStyle::GROUPBOX_WIDGET5);
		scrollArea2->setFont(font_D_GBW5);
		scrollArea2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		scrollArea2->setMinimumHeight(Window_height * 0.290);
		scrollArea2->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

		GroupBoxInnerIn2->setLayout(DosimeterDose_BarGraph_QVBoxLayout);
		scrollArea2->setWidget(GroupBoxInnerIn2);
		scrollArea2->setWidgetResizable(true);
		layoutInnerOut2->addWidget(scrollArea2);
		GroupBoxInnerOut2->setLayout(layoutInnerOut2);
		layoutIn->addWidget(GroupBoxInnerOut2);
	}

	GroupBoxIn->setLayout(layoutIn);
	scroll->setWidget(GroupBoxIn);
	scroll->setWidgetResizable(true);
	layoutOut->addWidget(scroll);
	GroupBoxOut->setLayout(layoutOut);
	subLayout->addWidget(GroupBoxOut);

	InnerResultPanel_Dosimeter_QGroupBox->setLayout(subLayout);

	layout->addWidget(InnerResultPanel_Dosimeter_QGroupBox);
}
void FunctionPanelRight::CreateOutput_H_AK(QVBoxLayout* layout)
{
	InnerResultPanel_H_AK_QGroupBox = new QGroupBox();
	InnerResultPanel_H_AK_QGroupBox->hide();
	InnerResultPanel_H_AK_QGroupBox->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	InnerResultPanel_H_AK_QGroupBox->setStyleSheet(DialogStyle::GROUPBOX_WIDGET5);
	InnerResultPanel_H_AK_QGroupBox->setFont(font_D_GBW5);
	InnerResultPanel_H_AK_QGroupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	QVBoxLayout* subLayout = new QVBoxLayout;
	subLayout->setContentsMargins(0, Window_height * 0.005, 0, 0);
	subLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

	QGroupBox* GroupBoxIn = new QGroupBox;
	GroupBoxIn->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	GroupBoxIn->setStyleSheet(DialogStyle::GROUPBOX_WIDGET5);
	GroupBoxIn->setFont(font_D_GBW5);
	GroupBoxIn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	QGroupBox* GroupBoxOut = new QGroupBox;
	GroupBoxOut->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	GroupBoxOut->setStyleSheet(DialogStyle::GROUPBOX_WIDGET5);
	GroupBoxOut->setFont(font_D_GBW5);
	GroupBoxOut->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	QVBoxLayout* layoutIn = new QVBoxLayout();
	layoutIn->setAlignment(Qt::AlignLeft | Qt::AlignTop);

	QVBoxLayout* layoutOut = new QVBoxLayout();
	layoutOut->setAlignment(Qt::AlignLeft | Qt::AlignTop);

	QScrollArea* scroll = new QScrollArea();
	scroll->setStyleSheet(DialogStyle::GROUPBOX_WIDGET5);
	scroll->setFont(font_D_GBW5);
	scroll->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	{
		QGroupBox* GroupBoxInnerIn = new QGroupBox;
		GroupBoxInnerIn->setAlignment(Qt::AlignTop | Qt::AlignLeft);
		GroupBoxInnerIn->setStyleSheet(DialogStyle::GROUPBOX_WIDGET5);
		GroupBoxInnerIn->setFont(font_D_GBW5);
		GroupBoxInnerIn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

		QGroupBox* GroupBoxInnerOut = new QGroupBox;
		GroupBoxInnerOut->setAlignment(Qt::AlignTop | Qt::AlignLeft);
		GroupBoxInnerOut->setStyleSheet(DialogStyle::GROUPBOX_WIDGET5);
		GroupBoxInnerOut->setFont(font_D_GBW5);
		GroupBoxInnerOut->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

		QVBoxLayout* layoutInnerIn = new QVBoxLayout();
		layoutInnerIn->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QVBoxLayout* layoutInnerOut = new QVBoxLayout();
		layoutInnerOut->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QScrollArea* scrollArea = new QScrollArea();
		scrollArea->setStyleSheet(DialogStyle::GROUPBOX_WIDGET5);
		scrollArea->setFont(font_D_GBW5);
		scrollArea->setFixedHeight(Window_height * 0.26);
		scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
		scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

		{
			QHBoxLayout* hLayout = new QHBoxLayout;
			hLayout->setContentsMargins(Window_width * 0.002, Window_height * 0.002, Window_width * 0.003, Window_height * 0.002);
			//hLayout->setSpacing(Window_width * 0.021);
			hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

			QLabel* label0 = new QLabel;
			label0->setFixedSize(Window_width * 0.065, Window_height * 0.028);
			label0->setStyleSheet(DialogStyle::DATA_LABEL);
			label0->setFont(font_D_LBL1);
			label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
			label0->setText("Position");

			m_Output_H_AK_posX = new QLineEdit;
			m_Output_H_AK_posX->setFixedSize(Window_width * 0.0545, Window_height * 0.028);
			m_Output_H_AK_posX->setAlignment(Qt::AlignCenter);
			m_Output_H_AK_posX->setStyleSheet(DialogStyle::LINEEDIT_NAME);
			m_Output_H_AK_posX->setFont(font_D_LE1);
			m_Output_H_AK_posX->setPlaceholderText("cm");
			//m_Output_H_AK_posX->setReadOnly(true);

			m_Output_H_AK_posY = new QLineEdit;
			m_Output_H_AK_posY->setFixedSize(Window_width * 0.0545, Window_height * 0.028);
			m_Output_H_AK_posY->setAlignment(Qt::AlignCenter);
			m_Output_H_AK_posY->setStyleSheet(DialogStyle::LINEEDIT_NAME);
			m_Output_H_AK_posY->setFont(font_D_LE1);
			m_Output_H_AK_posY->setPlaceholderText("cm");
			//m_Output_H_AK_posY->setReadOnly(true);

			m_Output_H_AK_posZ = new QLineEdit;
			m_Output_H_AK_posZ->setFixedSize(Window_width * 0.0545, Window_height * 0.028);
			m_Output_H_AK_posZ->setAlignment(Qt::AlignCenter);
			m_Output_H_AK_posZ->setStyleSheet(DialogStyle::LINEEDIT_NAME);
			m_Output_H_AK_posZ->setFont(font_D_LE1);
			m_Output_H_AK_posZ->setPlaceholderText("cm");
			//m_Output_H_AK_posZ->setReadOnly(true);

			hLayout->addWidget(label0);
			hLayout->addWidget(m_Output_H_AK_posX);
			hLayout->addWidget(m_Output_H_AK_posY);
			hLayout->addWidget(m_Output_H_AK_posZ);

			layoutInnerIn->addLayout(hLayout);
		}		
		
		{
			QHBoxLayout* hLayout = new QHBoxLayout;
			hLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.002, Window_width * 0.003, Window_height * 0.002);
			//hLayout->setSpacing(Window_width * 0.021);
			hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

			QLabel* label0 = new QLabel;
			label0->setFixedSize(Window_width * 0.115, Window_height * 0.028);
			label0->setStyleSheet(DialogStyle::DATA_LABEL);
			label0->setFont(font_D_LBL1);
			label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
			label0->setText("Ambient Dose Equivalent");

			m_Output_H_AK_AmbientDoseEquivaelnt = new QLineEdit;
			m_Output_H_AK_AmbientDoseEquivaelnt->setFixedSize(Window_width * 0.100, Window_height * 0.028);
			m_Output_H_AK_AmbientDoseEquivaelnt->setAlignment(Qt::AlignCenter);
			m_Output_H_AK_AmbientDoseEquivaelnt->setStyleSheet(DialogStyle::LINEEDIT_NAME);
			m_Output_H_AK_AmbientDoseEquivaelnt->setFont(font_D_LE1);
			m_Output_H_AK_AmbientDoseEquivaelnt->setPlaceholderText("pSv");
			//m_Output_H_AK_AmbientDoseEquivaelnt->setReadOnly(true);

			hLayout->addWidget(label0);
			hLayout->addWidget(m_Output_H_AK_AmbientDoseEquivaelnt);

			layoutInnerIn->addLayout(hLayout);
		}

		{
			QHBoxLayout* hLayout = new QHBoxLayout;
			hLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.002, Window_width * 0.003, Window_height * 0.002);
			//hLayout->setSpacing(Window_width * 0.021);
			hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

			QLabel* label0 = new QLabel;
			label0->setFixedSize(Window_width * 0.115, Window_height * 0.028);
			label0->setStyleSheet(DialogStyle::DATA_LABEL);
			label0->setFont(font_D_LBL1);
			label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
			label0->setText("Air Kerma");

			m_Output_H_AK_AirKerma = new QLineEdit;
			m_Output_H_AK_AirKerma->setFixedSize(Window_width * 0.100, Window_height * 0.028);
			m_Output_H_AK_AirKerma->setAlignment(Qt::AlignCenter);
			m_Output_H_AK_AirKerma->setStyleSheet(DialogStyle::LINEEDIT_NAME);
			m_Output_H_AK_AirKerma->setFont(font_D_LE1);
			m_Output_H_AK_AirKerma->setPlaceholderText("pGy");
			//m_Output_H_AK_AirKerma->setReadOnly(true);

			hLayout->addWidget(label0);
			hLayout->addWidget(m_Output_H_AK_AirKerma);

			layoutInnerIn->addLayout(hLayout);
		}

		GroupBoxInnerIn->setLayout(layoutInnerIn);
		scrollArea->setWidget(GroupBoxInnerIn);
		scrollArea->setWidgetResizable(true);
		layoutInnerOut->addWidget(scrollArea);
		GroupBoxInnerOut->setLayout(layoutInnerOut);
		layoutIn->addWidget(GroupBoxInnerOut);
	}

	{
		QGroupBox* GroupBoxInnerIn2 = new QGroupBox;
		GroupBoxInnerIn2->setAlignment(Qt::AlignTop | Qt::AlignLeft);
		GroupBoxInnerIn2->setStyleSheet(DialogStyle::GROUPBOX_WIDGET5);
		GroupBoxInnerIn2->setFont(font_D_GBW5);
		GroupBoxInnerIn2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

		QGroupBox* GroupBoxInnerOut2 = new QGroupBox;
		GroupBoxInnerOut2->setAlignment(Qt::AlignTop | Qt::AlignLeft);
		GroupBoxInnerOut2->setStyleSheet(DialogStyle::GROUPBOX_WIDGET5);
		GroupBoxInnerOut2->setFont(font_D_GBW5);
		GroupBoxInnerOut2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

		H_AK_BarGraph_QVBoxLayout = new QVBoxLayout();
		H_AK_BarGraph_QVBoxLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QVBoxLayout* layoutInnerOut2 = new QVBoxLayout();
		layoutInnerOut2->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QScrollArea* scrollArea2 = new QScrollArea();
		scrollArea2->setStyleSheet(DialogStyle::GROUPBOX_WIDGET5);
		scrollArea2->setFont(font_D_GBW5);
		scrollArea2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		scrollArea2->setMinimumHeight(Window_height * 0.290);
		scrollArea2->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

		GroupBoxInnerIn2->setLayout(H_AK_BarGraph_QVBoxLayout);
		scrollArea2->setWidget(GroupBoxInnerIn2);
		scrollArea2->setWidgetResizable(true);
		layoutInnerOut2->addWidget(scrollArea2);
		GroupBoxInnerOut2->setLayout(layoutInnerOut2);
		layoutIn->addWidget(GroupBoxInnerOut2);
	}

	GroupBoxIn->setLayout(layoutIn);
	scroll->setWidget(GroupBoxIn);
	scroll->setWidgetResizable(true);
	layoutOut->addWidget(scroll);
	GroupBoxOut->setLayout(layoutOut);
	subLayout->addWidget(GroupBoxOut);

	InnerResultPanel_H_AK_QGroupBox->setLayout(subLayout);

	layout->addWidget(InnerResultPanel_H_AK_QGroupBox);
}
void FunctionPanelRight::CreateOutput_LocalSkin(QVBoxLayout* layout)
{
	InnerResultPanel_LocalSkin_QGroupBox = new QGroupBox();
	InnerResultPanel_LocalSkin_QGroupBox->hide();
	InnerResultPanel_LocalSkin_QGroupBox->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	InnerResultPanel_LocalSkin_QGroupBox->setStyleSheet(DialogStyle::GROUPBOX_WIDGET5);
	InnerResultPanel_LocalSkin_QGroupBox->setFont(font_D_GBW5);
	InnerResultPanel_LocalSkin_QGroupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	QVBoxLayout* subLayout = new QVBoxLayout;
	subLayout->setContentsMargins(0, Window_height * 0.001, 0, 0);
	subLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

	QGroupBox* GroupBoxIn = new QGroupBox;
	GroupBoxIn->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	GroupBoxIn->setStyleSheet(DialogStyle::GROUPBOX_WIDGET5);
	GroupBoxIn->setFont(font_D_GBW5);
	GroupBoxIn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	QGroupBox* GroupBoxOut = new QGroupBox;
	GroupBoxOut->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	GroupBoxOut->setStyleSheet(DialogStyle::GROUPBOX_WIDGET5);
	GroupBoxOut->setFont(font_D_GBW5);
	GroupBoxOut->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	QVBoxLayout* layoutIn = new QVBoxLayout();
	layoutIn->setAlignment(Qt::AlignLeft | Qt::AlignTop);

	QVBoxLayout* layoutOut = new QVBoxLayout();
	layoutOut->setAlignment(Qt::AlignLeft | Qt::AlignTop);

	QScrollArea* scroll = new QScrollArea();
	scroll->setStyleSheet(DialogStyle::GROUPBOX_WIDGET5);
	scroll->setFont(font_D_GBW5);
	scroll->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	SkinDepthDose_LineGraph_QVBoxLayout = new QVBoxLayout();
	SkinDepthDose_LineGraph_QVBoxLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

	{
		QGroupBox* GroupBoxInnerIn = new QGroupBox;
		GroupBoxInnerIn->setAlignment(Qt::AlignTop | Qt::AlignLeft);
		GroupBoxInnerIn->setStyleSheet(DialogStyle::GROUPBOX_WIDGET5);
		GroupBoxInnerIn->setFont(font_D_GBW5);
		GroupBoxInnerIn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

		QGroupBox* GroupBoxInnerOut = new QGroupBox;
		GroupBoxInnerOut->setAlignment(Qt::AlignTop | Qt::AlignLeft);
		GroupBoxInnerOut->setStyleSheet(DialogStyle::GROUPBOX_WIDGET5);
		GroupBoxInnerOut->setFont(font_D_GBW5);
		GroupBoxInnerOut->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

		QVBoxLayout* layoutInnerOut = new QVBoxLayout();
		layoutInnerOut->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QScrollArea* scrollArea = new QScrollArea();
		scrollArea->setStyleSheet(DialogStyle::GROUPBOX_WIDGET5);
		scrollArea->setFont(font_D_GBW5);
		scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		scrollArea->setMinimumHeight(Window_height * 0.580);
		scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

		GroupBoxInnerIn->setLayout(SkinDepthDose_LineGraph_QVBoxLayout);
		scrollArea->setWidget(GroupBoxInnerIn);
		scrollArea->setWidgetResizable(true);
		layoutInnerOut->addWidget(scrollArea);
		GroupBoxInnerOut->setLayout(layoutInnerOut);
		layoutIn->addWidget(GroupBoxInnerOut);
	}

	GroupBoxIn->setLayout(layoutIn);
	scroll->setWidget(GroupBoxIn);
	scroll->setWidgetResizable(true);
	layoutOut->addWidget(scroll);
	GroupBoxOut->setLayout(layoutOut);
	subLayout->addWidget(GroupBoxOut);

	InnerResultPanel_LocalSkin_QGroupBox->setLayout(subLayout);

	layout->addWidget(InnerResultPanel_LocalSkin_QGroupBox);
}
void FunctionPanelRight::SetResultType(int index)
{
	if (0 == index) // Organ/eff dose -> 실시간
	{
		InnerResultPanel_Dose_QGroupBox->show();
		InnerResultPanel_Dosimeter_QGroupBox->hide();
		InnerResultPanel_H_AK_QGroupBox->hide();
		InnerResultPanel_LocalSkin_QGroupBox->hide();

		if (b_IsRealtimeCalculation)
		{
			theApp.pInf->show();
			theApp.pRslt->show();
			theApp.pRslt->RealtimeResultScroll_OrganDose_QscrollArea->show();
			theApp.pRslt->RealtimeResultScroll_Dosimeter_QscrollArea->hide();
		}
		else
		{
			theApp.pInf->hide();
			theApp.pRslt->hide();
		}

		for (auto itr_phantomID : m_Phantom_SequenceVector)
		{
			if (m_Phantom_MainInfo[itr_phantomID][E_PHANTOMMAININFO_CATEGORY] == E_PHANTOMCATEGORY_AIR) OutputPhantomButton[itr_phantomID]->setDisabled(true);
			if (m_Phantom_MainInfo[itr_phantomID][E_PHANTOMMAININFO_DUMMY] == E_PHANTOMDUMMY_YES) OutputPhantomButton[itr_phantomID]->setDisabled(true);
		}
	}
	else if (1 == index) { // 개인선량당량 -> 실시간
		InnerResultPanel_Dose_QGroupBox->hide();
		InnerResultPanel_Dosimeter_QGroupBox->show();
		InnerResultPanel_H_AK_QGroupBox->hide();
		InnerResultPanel_LocalSkin_QGroupBox->hide();

		if (b_IsRealtimeCalculation)
		{
			theApp.pInf->show();
			theApp.pRslt->show();
			theApp.pRslt->RealtimeResultScroll_OrganDose_QscrollArea->hide();
			theApp.pRslt->RealtimeResultScroll_Dosimeter_QscrollArea->show();
		}
		else
		{
			theApp.pInf->hide();
			theApp.pRslt->hide();
		}

		for (auto itr_phantomID : m_Phantom_SequenceVector)
		{
			if (m_Phantom_MainInfo[itr_phantomID][E_PHANTOMMAININFO_CATEGORY] == E_PHANTOMCATEGORY_AIR) OutputPhantomButton[itr_phantomID]->setDisabled(true);
			if (m_Phantom_MainInfo[itr_phantomID][E_PHANTOMMAININFO_DUMMY] == E_PHANTOMDUMMY_YES) OutputPhantomButton[itr_phantomID]->setDisabled(true);
		}
	}
	else if (2 == index) { // H_AK -> 가장 먼저 계산 완료(not 실시간)
		InnerResultPanel_Dose_QGroupBox->hide();
		InnerResultPanel_Dosimeter_QGroupBox->hide();
		InnerResultPanel_H_AK_QGroupBox->show();
		InnerResultPanel_LocalSkin_QGroupBox->hide();

		theApp.pInf->hide();
		theApp.pRslt->hide();
		theApp.pRslt->RealtimeResultScroll_OrganDose_QscrollArea->hide();
		theApp.pRslt->RealtimeResultScroll_Dosimeter_QscrollArea->hide();

		for (auto itr_phantomID : m_Phantom_SequenceVector)
		{

			if (m_Phantom_MainInfo[itr_phantomID][E_PHANTOMMAININFO_CATEGORY] == E_PHANTOMCATEGORY_HUMAN) OutputPhantomButton[itr_phantomID]->setDisabled(true); // Human phantom Disable
		}
	}
	else if (3 == index) { // 피부깊이선량분포 -> 계산 종료 후에만 출력(수정 필요), not 실시간
		InnerResultPanel_Dose_QGroupBox->hide();
		InnerResultPanel_Dosimeter_QGroupBox->hide();
		InnerResultPanel_H_AK_QGroupBox->hide();
		InnerResultPanel_LocalSkin_QGroupBox->show();

		theApp.pInf->hide();
		theApp.pRslt->hide();
		theApp.pRslt->RealtimeResultScroll_OrganDose_QscrollArea->hide();
		theApp.pRslt->RealtimeResultScroll_Dosimeter_QscrollArea->hide();

		for (auto itr_phantomID : m_Phantom_SequenceVector)
		{
			if (m_Phantom_MainInfo[itr_phantomID][E_PHANTOMMAININFO_CATEGORY] == E_PHANTOMCATEGORY_AIR) OutputPhantomButton[itr_phantomID]->setDisabled(true);
			if (m_Phantom_MainInfo[itr_phantomID][E_PHANTOMMAININFO_DUMMY] == E_PHANTOMDUMMY_YES) OutputPhantomButton[itr_phantomID]->setDisabled(true);
		}
	}
}

//************************************** Calculation Process **************************************//
void FunctionPanelRight::triggerSetting() // GUI
{
	// Create a new dialog
	CalculationSettingDialog* settingDialog = new CalculationSettingDialog(this);
	settingDialog->setStyleSheet("background-color: rgb(210, 210, 210);");
	settingDialog->setFixedWidth(Window_width * 0.3);
	settingDialog->setFixedHeight(Window_height * 0.9);
	settingDialog->setWindowTitle("Calculation settings");
	QScrollArea* scrollArea = new QScrollArea(settingDialog);
	scrollArea->setWidgetResizable(true);
	scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	QWidget* containerWidget = new QWidget;
	QVBoxLayout* layout = new QVBoxLayout(containerWidget);

	QButtonGroup* m_skinDepthDoseGroup;
	QButtonGroup* m_visualizationGroup;

	{// ERR
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setContentsMargins(Window_width * 0.003, 0, Window_width * 0.003, 0);
		hLayout->setSpacing(Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QLabel* label0 = new QLabel;
		label0->setFixedSize(Window_width * 0.130, Window_height * 0.028);
		label0->setStyleSheet(DialogStyle::DATA_LABEL);
		label0->setFont(font_D_LBL1);
		label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		label0->setText(QString::fromLocal8Bit("Target statistical error:"));

		hLayout->addWidget(label0);
		layout->addLayout(hLayout);
	}

	{
		for (int i = 0; i < OrganNameList.size(); i++) {
			QHBoxLayout* hLayout = new QHBoxLayout;
			hLayout->setContentsMargins(Window_width * 0.003, 0, Window_width * 0.003, 0);
			hLayout->setSpacing(Window_width * 0.003);
			hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

			QLabel* label0 = new QLabel;
			label0->setFixedSize(Window_width * 0.068, Window_height * 0.028);
			label0->setStyleSheet(DialogStyle::DATA_LABEL);
			label0->setFont(font_D_LBL1);
			label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
			label0->setText(OrganNameList[i]);

			CalculationSetting_TargetError_QLineEdit[OrganNameList[i]] = new QLineEdit;
			CalculationSetting_TargetError_QLineEdit[OrganNameList[i]]->setFixedSize(Window_width * 0.048, Window_height * 0.028);
			CalculationSetting_TargetError_QLineEdit[OrganNameList[i]]->setAlignment(Qt::AlignCenter);
			CalculationSetting_TargetError_QLineEdit[OrganNameList[i]]->setStyleSheet(DialogStyle::LINEEDIT_NAME);
			CalculationSetting_TargetError_QLineEdit[OrganNameList[i]]->setFont(font_D_LE1);
			CalculationSetting_TargetError_QLineEdit[OrganNameList[i]]->setPlaceholderText(QString::fromLocal8Bit("(%)"));
			CalculationSetting_TargetError_QLineEdit[OrganNameList[i]]->setValidator(ValFactory::makePositiveReal(CalculationSetting_TargetError_QLineEdit[OrganNameList[i]]));

			if (i >= OrganNameList.size() - 1)
			{
				hLayout->setContentsMargins(Window_width * 0.007, 0, 0, 0);
				hLayout->addWidget(label0);
				hLayout->addWidget(CalculationSetting_TargetError_QLineEdit[OrganNameList[i]]);
				layout->addLayout(hLayout);
				break;
			}

			QLabel* label1 = new QLabel;
			label1->setFixedSize(Window_width * 0.068, Window_height * 0.028);
			label1->setStyleSheet(DialogStyle::DATA_LABEL);
			label1->setFont(font_D_LBL1);
			label1->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
			label1->setText(OrganNameList[i + 1]);

			CalculationSetting_TargetError_QLineEdit[OrganNameList[i + 1]] = new QLineEdit;
			CalculationSetting_TargetError_QLineEdit[OrganNameList[i + 1]]->setFixedSize(Window_width * 0.048, Window_height * 0.028);
			CalculationSetting_TargetError_QLineEdit[OrganNameList[i + 1]]->setAlignment(Qt::AlignCenter);
			CalculationSetting_TargetError_QLineEdit[OrganNameList[i + 1]]->setStyleSheet(DialogStyle::LINEEDIT_NAME);
			CalculationSetting_TargetError_QLineEdit[OrganNameList[i + 1]]->setFont(font_D_LE1);
			CalculationSetting_TargetError_QLineEdit[OrganNameList[i + 1]]->setPlaceholderText(QString::fromLocal8Bit("(%)"));
			CalculationSetting_TargetError_QLineEdit[OrganNameList[i + 1]]->setValidator(ValFactory::makePositiveReal(CalculationSetting_TargetError_QLineEdit[OrganNameList[i + 1]]));

			hLayout->addWidget(label0);
			hLayout->addWidget(CalculationSetting_TargetError_QLineEdit[OrganNameList[i]]);
			QSpacerItem *spacer = new QSpacerItem(Window_width * 0.01, Window_height * 0.028); // Adjust spacing
			hLayout->addSpacerItem(spacer);
			hLayout->addWidget(label1);
			hLayout->addWidget(CalculationSetting_TargetError_QLineEdit[OrganNameList[i + 1]]);
			layout->addLayout(hLayout);

			i++;
		}
	}

	{// NPS
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);
		hLayout->setSpacing(Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QLabel* label0 = new QLabel;
		label0->setFixedSize(Window_width * 0.090, Window_height * 0.028);
		label0->setStyleSheet(DialogStyle::DATA_LABEL);
		label0->setFont(font_D_LBL1);
		label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		label0->setText(QString::fromLocal8Bit("Target NPS:"));

		CalculationSetting_TargetNPS_QLineEdit = new QLineEdit;
		CalculationSetting_TargetNPS_QLineEdit->setFixedSize(Window_width * 0.170, Window_height * 0.028);
		CalculationSetting_TargetNPS_QLineEdit->setAlignment(Qt::AlignCenter);
		CalculationSetting_TargetNPS_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		CalculationSetting_TargetNPS_QLineEdit->setFont(font_D_LE1);
		CalculationSetting_TargetNPS_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("nps"));
		CalculationSetting_TargetNPS_QLineEdit->setValidator(ValFactory::makePositiveInt(CalculationSetting_TargetNPS_QLineEdit));

		hLayout->addWidget(label0);
		hLayout->addWidget(CalculationSetting_TargetNPS_QLineEdit);

		layout->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);

		QLabel* label = new QLabel;
		label->setFixedSize(Window_width * 0.085, Window_height * 0.028);
		label->setStyleSheet(DialogStyle::DATA_LABEL);
		label->setFont(font_D_LBL1);
		label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		label->setText(QString::fromLocal8Bit("Skin Depth Dose: "));

		m_RadioButton_SkinDepthDoseOff = new QRadioButton(QString::fromLocal8Bit("Off (Default)"));
		m_RadioButton_SkinDepthDoseOff->setStyleSheet(DialogStyle::RADIOBUTTON_WIDGET);
		m_RadioButton_SkinDepthDoseOff->setFont(font_D_RB1);
		m_RadioButton_SkinDepthDoseOff->setFixedSize(Window_width * 0.090, Window_height * 0.028);

		m_RadioButton_SkinDepthDoseOn = new QRadioButton(QString::fromLocal8Bit("On"));
		m_RadioButton_SkinDepthDoseOn->setStyleSheet(DialogStyle::RADIOBUTTON_WIDGET);
		m_RadioButton_SkinDepthDoseOn->setFont(font_D_RB1);
		m_RadioButton_SkinDepthDoseOn->setFixedSize(Window_width * 0.090, Window_height * 0.028);

		m_RadioButton_SkinDepthDoseOff->setChecked(true);

		m_skinDepthDoseGroup = new QButtonGroup(this);
		m_skinDepthDoseGroup->addButton(m_RadioButton_SkinDepthDoseOff);
		m_skinDepthDoseGroup->addButton(m_RadioButton_SkinDepthDoseOn);

		hLayout->addWidget(label);
		hLayout->addWidget(m_RadioButton_SkinDepthDoseOff);
		hLayout->addWidget(m_RadioButton_SkinDepthDoseOn);

		layout->addLayout(hLayout);
	}

	{// Enter result file name
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);
		hLayout->setSpacing(Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QLabel* label0 = new QLabel;
		label0->setFixedSize(Window_width * 0.090, Window_height * 0.028);
		label0->setStyleSheet(DialogStyle::DATA_LABEL);
		label0->setFont(font_D_LBL1);
		label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		label0->setText(QString::fromLocal8Bit("Result file name:"));

		CalculationSetting_ResultFileName_QLineEdit = new QLineEdit;
		CalculationSetting_ResultFileName_QLineEdit->setFixedSize(Window_width * 0.170, Window_height * 0.028);
		CalculationSetting_ResultFileName_QLineEdit->setAlignment(Qt::AlignCenter);
		CalculationSetting_ResultFileName_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		CalculationSetting_ResultFileName_QLineEdit->setFont(font_D_LE1);
		CalculationSetting_ResultFileName_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("YYYY_MM_DD_HH-MM-SS (Default)"));
		CalculationSetting_ResultFileName_QLineEdit->setValidator(ValFactory::makeNoSpace(CalculationSetting_ResultFileName_QLineEdit));

		hLayout->addWidget(label0);
		hLayout->addWidget(CalculationSetting_ResultFileName_QLineEdit);

		layout->addLayout(hLayout);
	}

	{// Enter number of threads
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);
		hLayout->setSpacing(Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QLabel* label0 = new QLabel;
		label0->setFixedSize(Window_width * 0.090, Window_height * 0.028);
		label0->setStyleSheet(DialogStyle::DATA_LABEL);
		label0->setFont(font_D_LBL1);
		label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		label0->setText(QString::fromLocal8Bit("Number of threads:"));

		CalculationSetting_Thread_QLineEdit = new QLineEdit;
		CalculationSetting_Thread_QLineEdit->setFixedSize(Window_width * 0.170, Window_height * 0.028);
		CalculationSetting_Thread_QLineEdit->setAlignment(Qt::AlignCenter);
		CalculationSetting_Thread_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		CalculationSetting_Thread_QLineEdit->setFont(font_D_LE1);
		CalculationSetting_Thread_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("Use 50% of CPU Threads (Default)"));
		CalculationSetting_Thread_QLineEdit->setValidator(ValFactory::makePositiveInt(CalculationSetting_Thread_QLineEdit));

		hLayout->addWidget(label0);
		hLayout->addWidget(CalculationSetting_Thread_QLineEdit);

		layout->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);

		QLabel* label = new QLabel;
		label->setFixedSize(Window_width * 0.085, Window_height * 0.028);
		label->setStyleSheet(DialogStyle::DATA_LABEL);
		label->setFont(font_D_LBL1);
		label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		label->setText(QString::fromLocal8Bit("Visualization: "));

		m_radioVisualizationModeOff = new QRadioButton(QString::fromLocal8Bit("Off (Default)"));
		m_radioVisualizationModeOff->setStyleSheet(DialogStyle::RADIOBUTTON_WIDGET);
		m_radioVisualizationModeOff->setFont(font_D_RB1);
		m_radioVisualizationModeOff->setFixedSize(Window_width * 0.090, Window_height * 0.028);

		m_radioVisualizationModeOn = new QRadioButton(QString::fromLocal8Bit("On"));
		m_radioVisualizationModeOn->setStyleSheet(DialogStyle::RADIOBUTTON_WIDGET);
		m_radioVisualizationModeOn->setFont(font_D_RB1);
		m_radioVisualizationModeOn->setFixedSize(Window_width * 0.090, Window_height * 0.028);

		m_radioVisualizationModeOff->setChecked(true);

		m_visualizationGroup = new QButtonGroup(this);
		m_visualizationGroup->addButton(m_radioVisualizationModeOff);
		m_visualizationGroup->addButton(m_radioVisualizationModeOn);

		hLayout->addWidget(label);
		hLayout->addWidget(m_radioVisualizationModeOff);
		hLayout->addWidget(m_radioVisualizationModeOn);

		layout->addLayout(hLayout);
	}


	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		hLayout->setContentsMargins(Window_width * 0.003, Window_height * 0.015, Window_width * 0.003, 0);

		m_settingSaveButton = new QPushButton(QString::fromLocal8Bit("Save"));
		Buttons_FunctionPanelRight.append(m_settingSaveButton);
		m_settingSaveButton->setFixedSize(Window_width * 0.104, Window_height * 0.028);
		m_settingSaveButton->setCheckable(false);
		m_settingSaveButton->setStyleSheet(DialogStyle::MENU_BTN3);
		m_settingSaveButton->setFont(font_D_BTN3);

		connect(m_settingSaveButton, SIGNAL(clicked()), this, SLOT(slot_settingSave_ButtonClicked())); connect(m_settingSaveButton, SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));

		hLayout->addWidget(m_settingSaveButton);

		layout->addLayout(hLayout);
	}

	scrollArea->setWidget(containerWidget);
	QVBoxLayout* dialogLayout = new QVBoxLayout(settingDialog);
	dialogLayout->addWidget(scrollArea);
	settingDialog->setLayout(dialogLayout);
	settingDialog->adjustSize();

	// Load data if saved data exists
	for (int i = 0; i < OrganNameList.size(); i++)
	{
		if (m_CalculationSetting_TargetError[i] != "") CalculationSetting_TargetError_QLineEdit[OrganNameList[i]]->setText(m_CalculationSetting_TargetError[i]);
	}
	if (m_CalculationSetting_TargetNPS != "") CalculationSetting_TargetNPS_QLineEdit->setText(m_CalculationSetting_TargetNPS);
	if (m_CalculationSetting_ResultFileName != "") CalculationSetting_ResultFileName_QLineEdit->setText(m_CalculationSetting_ResultFileName);
	if (m_CalculationSetting_Thread != "") CalculationSetting_Thread_QLineEdit->setText(m_CalculationSetting_Thread);
	if (SettingSkinDepthModeInfo == 0)
	{
		m_RadioButton_SkinDepthDoseOff->setChecked(true);
		m_RadioButton_SkinDepthDoseOn->setChecked(false);
	}
	if (SettingSkinDepthModeInfo == 1)
	{
		m_RadioButton_SkinDepthDoseOff->setChecked(false);
		m_RadioButton_SkinDepthDoseOn->setChecked(true);
	}
	if (SettingVisualizationModeInfo == 0)
	{
		m_radioVisualizationModeOff->setChecked(true);
		m_radioVisualizationModeOn->setChecked(false);
	}
	if (SettingVisualizationModeInfo == 1)
	{
		m_radioVisualizationModeOff->setChecked(false);
		m_radioVisualizationModeOn->setChecked(true);
	}

	settingDialog->exec(); // Use exec() for a modal dialog or show() for a non-modal dialog
}
void FunctionPanelRight::slot_settingSave_ButtonClicked() // called by (1) triggerSetting
{
	// Input Validator	
	if (CalculationSetting_TargetNPS_QLineEdit->text() != "") // Target NPS
	{
		if (CalculationSetting_TargetNPS_QLineEdit->text().toDouble() > 2147483647) // NPS 정수 최대값 넘을 때
		{
			theApp.SetMessageBox("Target NPS value exceeds the maximum integer limit (2,147,483,647).");
			return;
		}
		if (CalculationSetting_TargetNPS_QLineEdit->text().toDouble() == 0) // NPS가 0 일 때
		{
			theApp.SetMessageBox("Target NPS value should not be zero");
			return;
		}
	}
	for (int i = 0; i < OrganNameList.size(); i++) // Target error
	{
		if (CalculationSetting_TargetError_QLineEdit[OrganNameList[i]]->text() != "")
		{
			if (CalculationSetting_TargetError_QLineEdit[OrganNameList[i]]->text().toDouble() == 0) // Target error가 0 일 때
			{
				theApp.SetMessageBox(OrganNameList[i] + ": Target error value should not be zero");
				return;
			}
			if (CalculationSetting_TargetError_QLineEdit[OrganNameList[i]]->text().toDouble() < 0 || CalculationSetting_TargetError_QLineEdit[OrganNameList[i]]->text().toDouble() > 100) // Target error가 0 일 때
			{
				theApp.SetMessageBox(OrganNameList[i] + ": Target error must be greater than 0 and no more than 100.");
				return;
			}
		}

	}
	if (CalculationSetting_Thread_QLineEdit->text() != "") // Number of thread
	{
		if (CalculationSetting_Thread_QLineEdit->text().toDouble() > std::thread::hardware_concurrency()) // 개수가 최대 쓰레드 개수를 넘을 때
		{
			theApp.SetMessageBox("Maximum allowed thread count: " + QString::number(std::thread::hardware_concurrency()));
			return;
		}
	}
	
	for (int i = 0; i < OrganNameList.size(); i++)
	{
		m_CalculationSetting_TargetError[i] = CalculationSetting_TargetError_QLineEdit[OrganNameList[i]]->text();
	}
	m_CalculationSetting_TargetNPS = CalculationSetting_TargetNPS_QLineEdit->text();
	m_CalculationSetting_ResultFileName = CalculationSetting_ResultFileName_QLineEdit->text();
	m_CalculationSetting_Thread = CalculationSetting_Thread_QLineEdit->text();
	if (m_RadioButton_SkinDepthDoseOff->isChecked()) SettingSkinDepthModeInfo = 0;
	if (m_RadioButton_SkinDepthDoseOn->isChecked()) SettingSkinDepthModeInfo = 1;
	if (m_radioVisualizationModeOff->isChecked()) SettingVisualizationModeInfo = 0;
	if (m_radioVisualizationModeOn->isChecked()) SettingVisualizationModeInfo = 1;

	theApp.SetMessageBox("Saved!");
}
void FunctionPanelRight::triggerCalculate() // called by (1) ETQVTKWidget::runButtonClicked()
{
	if (m_Phantom_SequenceVector.size() < 1)
	{
		theApp.SetMessageBox("At least 1 phantom must be installed");
		return;
	}
	slot_Calculate_ButtonClicked();
}
void FunctionPanelRight::slot_Calculate_ButtonClicked()
{
	// Unit 설정		
	if (m_comboBoxSourceGeometry->currentIndex() == 0) DoseUnit_QString = "*cm2";
	if (m_comboBoxSourceGeometry->currentIndex() == 1) DoseUnit_QString = "/h";
	if (m_comboBoxSourceGeometry->currentIndex() == 2) DoseUnit_QString = "/h";
	if (m_comboBoxSourceGeometry->currentIndex() == 3) DoseUnit_QString = "/h";
	if (m_comboBoxSourceGeometry->currentIndex() == 4) DoseUnit_QString = "/nps";
	if (m_comboBoxSourceGeometry->currentIndex() == 5) DoseUnit_QString = "/h";
	if (m_comboBoxSourceGeometry->currentIndex() == 6) DoseUnit_QString = "/h";
	if (m_comboBoxSourceGeometry->currentIndex() == 7) DoseUnit_QString = "/h";
	if (m_comboBoxSourceGeometry->currentIndex() == 9) DoseUnit_QString = "/h";

	m_comboBox_resultType->setItemText(0, "Organ and effective dose (pGy" + DoseUnit_QString + " or pSv" + DoseUnit_QString + ")"); // Organ dose + detriment-weighted dose + WBD
	m_comboBox_resultType->setItemText(1, "Personal dose equivalent (pSv" + DoseUnit_QString + ")"); // Personal equivalent dose + detriment-weighted dose + WBD
	m_comboBox_resultType->setItemText(2, "Ambient dose equivalent / Air kerma (pGy" + DoseUnit_QString + " or pSv" + DoseUnit_QString + ")");
	m_comboBox_resultType->setItemText(3, "Local skin dose distribution (pGy" + DoseUnit_QString + ")");

	theApp.managerCalculation->Calculate_Local(); // 현재는 Local 모드만 지우너
}

/*
void FunctionPanelRight::DoseCalculation_StopAndSave() // 계산 중단 로직 - manager called by (1) ETQVTKWidget::stopButtonClicked
{
	QString tmpdata_path = "./result/" + m_CalculationSetting_ResultFileName + "/tmpdata/";
	std::ofstream ofs_stop(tmpdata_path.toStdString() + "StopSignal_UI");
	ofs_stop << "Stop";
	ofs_stop.close();
}
*/
////////////////// Calculate_Local ////////////////// 

// Main function

void FunctionPanelRight::PhantomList_button_generate()
{
	// Generate phantom list button
	for (auto itr_phantomIndex : m_Phantom_SequenceVector)
	{
		OutputPhantomButton[itr_phantomIndex] = new QPushButton(m_Phantom_ButtonName[itr_phantomIndex]);
		Buttons_FunctionPanelRight.append(OutputPhantomButton[itr_phantomIndex]);
		OutputPhantomButton[itr_phantomIndex]->setObjectName(QString::number(itr_phantomIndex));
		OutputPhantomButton[itr_phantomIndex]->setFixedSize(Window_width * 0.05, Window_height * 0.040);
		OutputPhantomButton[itr_phantomIndex]->setStyleSheet(DialogStyle::MENU_BTN3);
		OutputPhantomButton[itr_phantomIndex]->setFont(font_D_BTN3);
		OutputPhantomButton[itr_phantomIndex]->setCheckable(true);
		if (m_Phantom_MainInfo[itr_phantomIndex][E_PHANTOMMAININFO_DUMMY] == E_PHANTOMDUMMY_YES || m_Phantom_MainInfo[itr_phantomIndex][E_PHANTOMMAININFO_CATEGORY] == E_PHANTOMCATEGORY_AIR) OutputPhantomButton[itr_phantomIndex]->setDisabled(true);

		connect(OutputPhantomButton[itr_phantomIndex], SIGNAL(clicked()), this, SLOT(slot_OutputPhantomSelect_ButtonClicked())); connect(OutputPhantomButton[itr_phantomIndex], SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
		layoutOutputPhantomButtonList->addWidget(OutputPhantomButton[itr_phantomIndex]);
	}
	OutputPhantomButton[OutputPhantomSelectedIndex]->setChecked(true);
}

/*
void FunctionPanelRight::Calculate_Local()
void FunctionPanelRight::DataInitialization_Local()
void FunctionPanelRight::checkProcessInfo_Local()
*/

//// Sub-function (1)
void FunctionPanelRight::Create_Dose_BarGraphWidgets(QVBoxLayout* layoutIn_graph) // called by (1) Calculate_Local, (2) LoadReconsturctionFile_previous
{
	QHBoxLayout* Layout = new QHBoxLayout;
	Layout->setContentsMargins(Window_width * 0.001, Window_height * 0.001, 0, 0);
	Layout->setSpacing(0);
	Layout->setAlignment(Qt::AlignCenter | Qt::AlignTop);
	OrganDose_BarGraph_QCustomPlot = new QCustomPlot();
	OrganDose_BarGraph_QCustomPlot->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	OrganDose_BarGraph_QCustomPlot->setFixedSize(Window_width * 0.780, Window_height * 0.244 * SpecialWidgetSizeFactor);
	Layout->addWidget(OrganDose_BarGraph_QCustomPlot);
	layoutIn_graph->addLayout(Layout);
	// Graph layout added
	QVector<QString> organs;
	for (auto itr_organName : OrganNameList)
	{
		organs.push_back(itr_organName);
	}
	QVector<double> doses(OrganNameList.size(), 0);
	QCPBars *bars = new QCPBars(OrganDose_BarGraph_QCustomPlot->xAxis, OrganDose_BarGraph_QCustomPlot->yAxis);
	// Set data
	QVector<double> ticks;
	for (int i = 0; i < organs.size() + 1; ++i)
		ticks << i + 1;
	bars->setData(ticks, doses);
	//X 축 정보
	OrganDose_BarGraph_QCustomPlot->xAxis->setTicker(QSharedPointer<QCPAxisTickerText>(new QCPAxisTickerText));
	QSharedPointer<QCPAxisTickerText> textTicker = qSharedPointerCast<QCPAxisTickerText>(OrganDose_BarGraph_QCustomPlot->xAxis->ticker());
	textTicker->addTicks(ticks, organs);
	OrganDose_BarGraph_QCustomPlot->xAxis->setTickLabelFont(QFont("sans", FontSizeScaling(8), QFont::Bold)); // X축 장기 폰트크기
	OrganDose_BarGraph_QCustomPlot->xAxis->setTickLabelRotation(17);
	OrganDose_BarGraph_QCustomPlot->xAxis->setSubTicks(false);
	OrganDose_BarGraph_QCustomPlot->xAxis->setTickLength(0, 1); // x 축 바깥으로 나오는 틱 길이
	OrganDose_BarGraph_QCustomPlot->xAxis->setBasePen(QPen(Qt::black));
	OrganDose_BarGraph_QCustomPlot->xAxis->setTickPen(QPen(Qt::black));
	OrganDose_BarGraph_QCustomPlot->xAxis->grid()->setVisible(true);
	OrganDose_BarGraph_QCustomPlot->xAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));
	OrganDose_BarGraph_QCustomPlot->xAxis->setRange(0, organs.size() + 3); // +3 so there's space(3 ticks) after the last bar

	// Y축 정보
	OrganDose_BarGraph_QCustomPlot->yAxis->setScaleType(QCPAxis::stLinear); // Set to linear scale
	QSharedPointer<QCPAxisTicker> linearTicker(new QCPAxisTicker); // Use a standard ticker	
	OrganDose_BarGraph_QCustomPlot->yAxis->setTicker(linearTicker); // Set the ticker to the standard one for linear scale
	OrganDose_BarGraph_QCustomPlot->yAxis->setNumberFormat("e");  // "f" for fixed-point notation, typical for linear scales
	OrganDose_BarGraph_QCustomPlot->yAxis->setNumberPrecision(1); // You can adjust precision as needed
	OrganDose_BarGraph_QCustomPlot->yAxis->setTickLabelFont(QFont("sans", FontSizeScaling(8))); // Y축 값 폰트크기 (Y-axis font size)
	OrganDose_BarGraph_QCustomPlot->yAxis->setBasePen(QPen(Qt::black));
	OrganDose_BarGraph_QCustomPlot->yAxis->setTickPen(QPen(Qt::black));
	OrganDose_BarGraph_QCustomPlot->yAxis->setSubTickPen(QPen(Qt::black));
	OrganDose_BarGraph_QCustomPlot->yAxis->grid()->setSubGridVisible(true);
	OrganDose_BarGraph_QCustomPlot->yAxis->setTickLabelColor(Qt::black);
	OrganDose_BarGraph_QCustomPlot->yAxis->setLabelColor(Qt::black);

	OrganDose_BarGraph_QCustomPlot->yAxis->setLabel("Dose (pGy" + DoseUnit_QString + " or pSv" + DoseUnit_QString + ")");
	OrganDose_BarGraph_QCustomPlot->yAxis->setLabelFont(QFont("sans", FontSizeScaling(9))); // Y축 label 폰트크기
	OrganDose_BarGraph_QCustomPlot->yAxis->setLabelColor(Qt::black);
	OrganDose_BarGraph_QCustomPlot->yAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::SolidLine));
	OrganDose_BarGraph_QCustomPlot->yAxis->grid()->setSubGridPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));
	for (int i = 0; i < doses.size(); ++i) {
		QCPItemText *valueLabel = new QCPItemText(OrganDose_BarGraph_QCustomPlot);
		valueLabel->setText(QString::number(doses.at(i), 'e', 1));  // scientific, precision "1"
		valueLabel->position->setType(QCPItemPosition::ptPlotCoords);
		valueLabel->position->setCoords(ticks.at(i), doses.at(i) * 1.35);  // Place it on top of the bar
		valueLabel->setFont(QFont("sans", FontSizeScaling(7), QFont::Bold));  // Optional: set font, size, etc.
	}
	//QCPTextElement *title = new QCPTextElement(OrganDose_BarGraph_QCustomPlot, " Organ Dose", QFont("sans", FontSizeScaling(12), QFont::Bold)); // Replace 'Your Graph Title' with your desired title
	QCPTextElement *title = new QCPTextElement(OrganDose_BarGraph_QCustomPlot, " Organ Dose / Effective Dose", QFont("sans", FontSizeScaling(12), QFont::Bold)); // Replace 'Your Graph Title' with your desired title
	OrganDose_BarGraph_QCustomPlot->plotLayout()->insertRow(0);
	OrganDose_BarGraph_QCustomPlot->plotLayout()->addElement(0, 0, title); // Place the title in the empty row
	title->setTextFlags(Qt::AlignLeft);
	title->setMargins(QMargins(0, 0, 0, 10));
	// Rescale the y-axis to fit the dose data
	bars->rescaleValueAxis();
	double upperScaleFactor = 2;  // e.g., add 1 tick(2곱하기) to the upper bound
	double upperBound = OrganDose_BarGraph_QCustomPlot->yAxis->range().upper;
	OrganDose_BarGraph_QCustomPlot->yAxis->setRangeUpper(upperBound * upperScaleFactor);
	theApp.pRt->OrganDose_BarGraph_QCustomPlot->yAxis->setRangeLower(0);
	// Set background
	OrganDose_BarGraph_QCustomPlot->setBackground(QColor(240, 240, 240));

	//// Make bars look nice
	bars->setWidth(0.4); // 막대 너비
	bars->setPen(Qt::NoPen); // 막대 테두리 없음
	bars->setBrush(QColor(10, 140, 70, 160)); // 막대 색상
	// Finally, refresh the plot with the data
	OrganDose_BarGraph_QCustomPlot->replot();
}
void FunctionPanelRight::Create_Dosimeter_BarGraphWidgets(QVBoxLayout* layoutIn_graph) // called by (1) Calculate_Local, (2) LoadReconsturctionFile_previous
{
	QHBoxLayout* Layout = new QHBoxLayout;
	Layout->setContentsMargins(Window_width * 0.001, Window_height * 0.001, 0, 0);
	Layout->setSpacing(0);
	Layout->setAlignment(Qt::AlignCenter | Qt::AlignTop);
	Dosimeter_BarGraph_QCustomPlot = new QCustomPlot();
	Dosimeter_BarGraph_QCustomPlot->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	int dosimeter_count = 2; // 임시로 도시미터 2개일때
	Dosimeter_BarGraph_QCustomPlot->setFixedSize(Window_width * 0.780 * dosimeter_count / 5, Window_height * 0.244 * SpecialWidgetSizeFactor);
	Layout->addWidget(Dosimeter_BarGraph_QCustomPlot);
	layoutIn_graph->addLayout(Layout);
	// Graph layout added
	QVector<QString> dosimeter_names;
	for (int i = 0; i < dosimeter_count; i++)
	{
		dosimeter_names.push_back("Dosimeter_" + QString::number(i) + "_Hp(10)");
		dosimeter_names.push_back("Dosimeter_" + QString::number(i) + "_Hp(3)");
		dosimeter_names.push_back("Dosimeter_" + QString::number(i) + "_Hp(0.07)");
	}
	QVector<double> doses(dosimeter_names.size(), 0);
	QCPBars *bars = new QCPBars(Dosimeter_BarGraph_QCustomPlot->xAxis, Dosimeter_BarGraph_QCustomPlot->yAxis);
	// Set data
	QVector<double> ticks;
	for (int i = 0; i < dosimeter_names.size() + 1; ++i)
		ticks << i + 1;
	bars->setData(ticks, doses);
	//X 축 정보
	Dosimeter_BarGraph_QCustomPlot->xAxis->setTicker(QSharedPointer<QCPAxisTickerText>(new QCPAxisTickerText));
	QSharedPointer<QCPAxisTickerText> textTicker = qSharedPointerCast<QCPAxisTickerText>(Dosimeter_BarGraph_QCustomPlot->xAxis->ticker());
	textTicker->addTicks(ticks, dosimeter_names);
	Dosimeter_BarGraph_QCustomPlot->xAxis->setTickLabelFont(QFont("sans", FontSizeScaling(8), QFont::Bold)); // X축 장기 폰트크기
	Dosimeter_BarGraph_QCustomPlot->xAxis->setTickLabelRotation(17);
	Dosimeter_BarGraph_QCustomPlot->xAxis->setSubTicks(false);
	Dosimeter_BarGraph_QCustomPlot->xAxis->setTickLength(0, 1); // x 축 바깥으로 나오는 틱 길이
	Dosimeter_BarGraph_QCustomPlot->xAxis->setBasePen(QPen(Qt::black));
	Dosimeter_BarGraph_QCustomPlot->xAxis->setTickPen(QPen(Qt::black));
	Dosimeter_BarGraph_QCustomPlot->xAxis->grid()->setVisible(true);
	Dosimeter_BarGraph_QCustomPlot->xAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));
	Dosimeter_BarGraph_QCustomPlot->xAxis->setRange(0, dosimeter_names.size() + 2); // +2 so there's space(2 ticks) after the last bar

	//Y 축 정보
	Dosimeter_BarGraph_QCustomPlot->yAxis->setScaleType(QCPAxis::stLinear); // Set to linear scale
	QSharedPointer<QCPAxisTicker> linearTicker(new QCPAxisTicker); // Use a standard ticker	
	Dosimeter_BarGraph_QCustomPlot->yAxis->setTicker(linearTicker); // Set the ticker to the standard one for linear scale
	Dosimeter_BarGraph_QCustomPlot->yAxis->setNumberFormat("e");  // "f" for fixed-point notation, typical for linear scales
	Dosimeter_BarGraph_QCustomPlot->yAxis->setNumberPrecision(1); // You can adjust precision as needed
	Dosimeter_BarGraph_QCustomPlot->yAxis->setTickLabelFont(QFont("sans", FontSizeScaling(8))); // Y축 값 폰트크기 (Y-axis font size)
	Dosimeter_BarGraph_QCustomPlot->yAxis->setBasePen(QPen(Qt::black));
	Dosimeter_BarGraph_QCustomPlot->yAxis->setTickPen(QPen(Qt::black));
	Dosimeter_BarGraph_QCustomPlot->yAxis->setSubTickPen(QPen(Qt::black));
	Dosimeter_BarGraph_QCustomPlot->yAxis->grid()->setSubGridVisible(true);
	Dosimeter_BarGraph_QCustomPlot->yAxis->setTickLabelColor(Qt::black);
	Dosimeter_BarGraph_QCustomPlot->yAxis->setLabelColor(Qt::black);
	Dosimeter_BarGraph_QCustomPlot->yAxis->setLabel("Dose (pSv" + DoseUnit_QString + ")");
	Dosimeter_BarGraph_QCustomPlot->yAxis->setLabelFont(QFont("sans", FontSizeScaling(9))); // Y축 label 폰트크기
	Dosimeter_BarGraph_QCustomPlot->yAxis->setLabelColor(Qt::black);
	Dosimeter_BarGraph_QCustomPlot->yAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::SolidLine));
	Dosimeter_BarGraph_QCustomPlot->yAxis->grid()->setSubGridPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));
	for (int i = 0; i < doses.size(); ++i) {
		QCPItemText *valueLabel = new QCPItemText(Dosimeter_BarGraph_QCustomPlot);
		valueLabel->setText(QString::number(doses.at(i), 'e', 1));  // scientific, precision "1"
		valueLabel->position->setType(QCPItemPosition::ptPlotCoords);
		valueLabel->position->setCoords(ticks.at(i), doses.at(i) * 1.35);  // Place it on top of the bar
		valueLabel->setFont(QFont("sans", FontSizeScaling(7), QFont::Bold));  // Optional: set font, size, etc.
	}
	QCPTextElement *title = new QCPTextElement(Dosimeter_BarGraph_QCustomPlot, " Personal dose equivalent ", QFont("sans", FontSizeScaling(12), QFont::Bold)); // Replace 'Your Graph Title' with your desired title
	Dosimeter_BarGraph_QCustomPlot->plotLayout()->insertRow(0);
	Dosimeter_BarGraph_QCustomPlot->plotLayout()->addElement(0, 0, title); // Place the title in the empty row
	title->setTextFlags(Qt::AlignLeft);
	title->setMargins(QMargins(0, 0, 0, 10));
	// Rescale the y-axis to fit the dose data
	bars->rescaleValueAxis();
	double upperScaleFactor = 2;  // e.g., add 1 tick(2곱하기) to the upper bound
	double upperBound = Dosimeter_BarGraph_QCustomPlot->yAxis->range().upper;
	Dosimeter_BarGraph_QCustomPlot->yAxis->setRangeUpper(upperBound * upperScaleFactor);
	theApp.pRt->Dosimeter_BarGraph_QCustomPlot->yAxis->setRangeLower(0);
	// Set background
	Dosimeter_BarGraph_QCustomPlot->setBackground(QColor(240, 240, 240));

	//// Make bars look nice
	bars->setWidth(0.4); // 막대 너비
	bars->setPen(Qt::NoPen); // 막대 테두리 없음
	bars->setBrush(QColor(10, 140, 70, 160)); // 막대 색상
	// Finally, refresh the plot with the data
	Dosimeter_BarGraph_QCustomPlot->replot();
}
void FunctionPanelRight::Create_H_AK_BarGraphWidgets(QVBoxLayout* layoutIn_graph) // called by (1) Calculate_Local, (2) LoadReconsturctionFile_previous
{
	QHBoxLayout* Layout = new QHBoxLayout;
	Layout->setContentsMargins(Window_width * 0.001, Window_height * 0.001, 0, 0);
	Layout->setSpacing(0);
	Layout->setAlignment(Qt::AlignCenter | Qt::AlignTop);
	H_AK_BarGraph_QCustomPlot = new QCustomPlot();
	H_AK_BarGraph_QCustomPlot->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	H_AK_BarGraph_QCustomPlot->setFixedSize(Window_width * 0.780 * m_Phantom_SequenceVector.size() / 7.5, Window_height * 0.244 * SpecialWidgetSizeFactor);
	Layout->addWidget(H_AK_BarGraph_QCustomPlot);
	layoutIn_graph->addLayout(Layout);
	// Graph layout added
	QVector<QString> H_AK_names;
	for (auto itr_phantomID : m_Phantom_SequenceVector)
	{
		H_AK_names.push_back(m_Phantom_ButtonName[itr_phantomID] + "_H*(10)");
		H_AK_names.push_back(m_Phantom_ButtonName[itr_phantomID] + "_AK");
	}
	QVector<double> doses(H_AK_names.size(), 0);
	QCPBars *bars = new QCPBars(H_AK_BarGraph_QCustomPlot->xAxis, H_AK_BarGraph_QCustomPlot->yAxis);
	// Set data
	QVector<double> ticks;
	for (int i = 0; i < H_AK_names.size() + 1; ++i)
		ticks << i + 1;
	bars->setData(ticks, doses);
	//X 축 정보
	H_AK_BarGraph_QCustomPlot->xAxis->setTicker(QSharedPointer<QCPAxisTickerText>(new QCPAxisTickerText));
	QSharedPointer<QCPAxisTickerText> textTicker = qSharedPointerCast<QCPAxisTickerText>(H_AK_BarGraph_QCustomPlot->xAxis->ticker());
	textTicker->addTicks(ticks, H_AK_names);
	H_AK_BarGraph_QCustomPlot->xAxis->setTickLabelFont(QFont("sans", FontSizeScaling(8), QFont::Bold)); // X축 장기 폰트크기
	H_AK_BarGraph_QCustomPlot->xAxis->setTickLabelRotation(17);
	H_AK_BarGraph_QCustomPlot->xAxis->setSubTicks(false);
	H_AK_BarGraph_QCustomPlot->xAxis->setTickLength(0, 1); // x 축 바깥으로 나오는 틱 길이
	H_AK_BarGraph_QCustomPlot->xAxis->setBasePen(QPen(Qt::black));
	H_AK_BarGraph_QCustomPlot->xAxis->setTickPen(QPen(Qt::black));
	H_AK_BarGraph_QCustomPlot->xAxis->grid()->setVisible(true);
	H_AK_BarGraph_QCustomPlot->xAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));
	H_AK_BarGraph_QCustomPlot->xAxis->setRange(0, H_AK_names.size() + 2); // +2 so there's space(2 ticks) after the last bar

	//Y 축 정보
	H_AK_BarGraph_QCustomPlot->yAxis->setScaleType(QCPAxis::stLinear); // Set to linear scale
	QSharedPointer<QCPAxisTicker> linearTicker(new QCPAxisTicker); // Use a standard ticker	
	H_AK_BarGraph_QCustomPlot->yAxis->setTicker(linearTicker); // Set the ticker to the standard one for linear scale
	H_AK_BarGraph_QCustomPlot->yAxis->setNumberFormat("e");  // "f" for fixed-point notation, typical for linear scales
	H_AK_BarGraph_QCustomPlot->yAxis->setNumberPrecision(1); // You can adjust precision as needed
	H_AK_BarGraph_QCustomPlot->yAxis->setTickLabelFont(QFont("sans", FontSizeScaling(8))); // Y축 값 폰트크기 (Y-axis font size)
	H_AK_BarGraph_QCustomPlot->yAxis->setBasePen(QPen(Qt::black));
	H_AK_BarGraph_QCustomPlot->yAxis->setTickPen(QPen(Qt::black));
	H_AK_BarGraph_QCustomPlot->yAxis->setSubTickPen(QPen(Qt::black));
	H_AK_BarGraph_QCustomPlot->yAxis->grid()->setSubGridVisible(true);
	H_AK_BarGraph_QCustomPlot->yAxis->setTickLabelColor(Qt::black);
	H_AK_BarGraph_QCustomPlot->yAxis->setLabelColor(Qt::black);
	H_AK_BarGraph_QCustomPlot->yAxis->setLabel("Dose (pGy" + DoseUnit_QString + " or pSv" + DoseUnit_QString + ")");
	H_AK_BarGraph_QCustomPlot->yAxis->setLabelFont(QFont("sans", FontSizeScaling(9))); // Y축 label 폰트크기
	H_AK_BarGraph_QCustomPlot->yAxis->setLabelColor(Qt::black);
	H_AK_BarGraph_QCustomPlot->yAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::SolidLine));
	H_AK_BarGraph_QCustomPlot->yAxis->grid()->setSubGridPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));
	for (int i = 0; i < doses.size(); ++i) {
		QCPItemText *valueLabel = new QCPItemText(H_AK_BarGraph_QCustomPlot);
		valueLabel->setText(QString::number(doses.at(i), 'e', 1));  // scientific, precision "1"
		valueLabel->position->setType(QCPItemPosition::ptPlotCoords);
		valueLabel->position->setCoords(ticks.at(i), doses.at(i) * 1.35);  // Place it on top of the bar
		valueLabel->setFont(QFont("sans", FontSizeScaling(7), QFont::Bold));  // Optional: set font, size, etc.
	}
	QCPTextElement *title = new QCPTextElement(H_AK_BarGraph_QCustomPlot, " Ambient dose equivalent / Air kerma ", QFont("sans", FontSizeScaling(12), QFont::Bold)); // Replace 'Your Graph Title' with your desired title
	H_AK_BarGraph_QCustomPlot->plotLayout()->insertRow(0);
	H_AK_BarGraph_QCustomPlot->plotLayout()->addElement(0, 0, title); // Place the title in the empty row
	title->setTextFlags(Qt::AlignLeft);
	title->setMargins(QMargins(0, 0, 0, 10));
	// Rescale the y-axis to fit the dose data
	bars->rescaleValueAxis();
	double upperScaleFactor = 2;  // e.g., add 1 tick(2곱하기) to the upper bound
	double upperBound = H_AK_BarGraph_QCustomPlot->yAxis->range().upper;
	H_AK_BarGraph_QCustomPlot->yAxis->setRangeUpper(upperBound * upperScaleFactor);
	theApp.pRt->H_AK_BarGraph_QCustomPlot->yAxis->setRangeLower(0);
	// Set background
	H_AK_BarGraph_QCustomPlot->setBackground(QColor(240, 240, 240));

	//// Make bars look nice
	bars->setWidth(0.4); // 막대 너비
	bars->setPen(Qt::NoPen); // 막대 테두리 없음
	bars->setBrush(QColor(10, 140, 70, 160)); // 막대 색상
	// Finally, refresh the plot with the data
	H_AK_BarGraph_QCustomPlot->replot();
}
void FunctionPanelRight::Create_Skindose_LineGraphWidgets(QVBoxLayout* layoutIn_graph) // called by (1) Calculate_Local, (2) LoadReconsturctionFile_previous
{
	for (int skindose_type = 0; skindose_type < 2; skindose_type++)
	{
		QHBoxLayout* Layout = new QHBoxLayout;
		Layout->setContentsMargins(Window_width * 0.001, Window_height * 0.005, Window_width * 0.001, Window_height * 0.005);
		Layout->setSpacing(0);
		Layout->setAlignment(Qt::AlignCenter | Qt::AlignTop);

		SkinDepthDose_LineGraph_QCustomPlot[skindose_type] = new QCustomPlot();
		SkinDepthDose_LineGraph_QCustomPlot[skindose_type]->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		SkinDepthDose_LineGraph_QCustomPlot[skindose_type]->setFixedSize(Window_width * 0.235 * SpecialWidgetSizeFactor, Window_height * 0.260); // customplot은 한 번 더 줄이기	

		SkinDepthDose_LineGraph_QCustomPlot[skindose_type]->setMouseTracking(true);
		connect(SkinDepthDose_LineGraph_QCustomPlot[skindose_type], SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(show_Skindose_LineGraph_ValueTooltip(QMouseEvent*)));

		Layout->addWidget(SkinDepthDose_LineGraph_QCustomPlot[skindose_type]);
		layoutIn_graph->addLayout(Layout);

		QVector<double> x(0), y(0);
		SkinDepthDose_LineGraph_QCustomPlot[skindose_type]->addGraph();
		SkinDepthDose_LineGraph_QCustomPlot[skindose_type]->graph(0)->setData(x, y);

		QString GraphName;
		if (skindose_type == 0) GraphName = "Local skin dose distribution_1cm2";
		if (skindose_type == 1) GraphName = "Local skin dose distribution_10cm2";

		QCPTextElement *title = new QCPTextElement(SkinDepthDose_LineGraph_QCustomPlot[skindose_type], GraphName, QFont("sans", FontSizeScaling(13), QFont::Bold));
		SkinDepthDose_LineGraph_QCustomPlot[skindose_type]->plotLayout()->insertRow(0); // Insert an empty row in the plot layout
		SkinDepthDose_LineGraph_QCustomPlot[skindose_type]->plotLayout()->addElement(0, 0, title); // Add the title to the left of the new row
		double maximum_dose_location_X = 0;
		double maximum_dose_location_Y = 0;
		double maximum_dose_location_Z = 0;
		QString qstr_location_X = QString("(%1, ").arg(QString::number(maximum_dose_location_X, 'f', 4));
		QString qstr_location_Y = QString("%1, ").arg(QString::number(maximum_dose_location_Y, 'f', 4));
		QString qstr_location_Z = QString("%1)").arg(QString::number(maximum_dose_location_Z, 'f', 4));
		QString qstr_location_XYZ = "Max.pos: " + qstr_location_X + qstr_location_Y + qstr_location_Z;
		double maximum_local_skin_dose = 0;
		QString qstr_maximum_local_skin_dose = "Dose (pGy" + DoseUnit_QString + QString("): %1 ").arg(QString::number(maximum_local_skin_dose, 'e', 4));
		QCPTextElement *dose_info_text = new QCPTextElement(SkinDepthDose_LineGraph_QCustomPlot[skindose_type], qstr_location_XYZ + "             " + qstr_maximum_local_skin_dose, QFont("sans", FontSizeScaling(8), QFont::Bold));
		dose_info_text->setTextFlags(Qt::AlignLeft);
		SkinDepthDose_LineGraph_QCustomPlot[skindose_type]->plotLayout()->insertRow(1); // Insert an empty row in the plot layout
		SkinDepthDose_LineGraph_QCustomPlot[skindose_type]->plotLayout()->addElement(1, 0, dose_info_text); // Add the error to the right of the new row

		SkinDepthDose_LineGraph_QCustomPlot[skindose_type]->xAxis->setLabel("Depth (um)");
		SkinDepthDose_LineGraph_QCustomPlot[skindose_type]->xAxis->setLabelFont(QFont("sans", FontSizeScaling(11))); // x축 라벨 폰트
		SkinDepthDose_LineGraph_QCustomPlot[skindose_type]->xAxis->setTickLabelFont(QFont("sans", FontSizeScaling(8))); // x축 값 폰트

		SkinDepthDose_LineGraph_QCustomPlot[skindose_type]->yAxis->setLabel("Dose (pGy" + DoseUnit_QString + ")");
		SkinDepthDose_LineGraph_QCustomPlot[skindose_type]->yAxis->setLabelFont(QFont("sans", FontSizeScaling(8))); // y축 라벨 폰트
		SkinDepthDose_LineGraph_QCustomPlot[skindose_type]->yAxis->setTickLabelFont(QFont("sans", FontSizeScaling(8))); // y축 값 폰트
		SkinDepthDose_LineGraph_QCustomPlot[skindose_type]->yAxis->setNumberFormat("e"); // "e" for scientific notation and "b" to use beautiful decimal powers
		SkinDepthDose_LineGraph_QCustomPlot[skindose_type]->yAxis->setNumberPrecision(2); // This sets the number to be like "1.4e+03"

		SkinDepthDose_LineGraph_QCustomPlot[skindose_type]->rescaleAxes();
		SkinDepthDose_LineGraph_QCustomPlot[skindose_type]->replot();
	}
}
void FunctionPanelRight::Update_SkinDose_1_10cm2_GraphWidgets() // called by (1) checkProcessInfo_Local
{
	int phantomID = m_Phantom_SequenceVector[0]; // Only for one phantom
	for (int skindose_type = 0; skindose_type < 2; skindose_type++)
	{
		double maximum_dose_location_X, maximum_dose_location_Y, maximum_dose_location_Z, maximum_local_skin_dose;
		double statistical_error; // Statistical error 추가
		if (skindose_type == 0) // 1 cm2
		{
			maximum_dose_location_X = theApp.HP_1cm2_MaximumLocation_GraphInfo[phantomID][0];
			maximum_dose_location_Y = theApp.HP_1cm2_MaximumLocation_GraphInfo[phantomID][1];
			maximum_dose_location_Z = theApp.HP_1cm2_MaximumLocation_GraphInfo[phantomID][2];
			maximum_local_skin_dose = theApp.HP_1cm2_MaximumDose_GraphInfo[phantomID];
			statistical_error = theApp.HP_1cm2_RelativeError_GraphInfo[phantomID] * 100.; // Statistical error (%) 추가 
		}
		if (skindose_type == 1) // 10 cm2
		{
			maximum_dose_location_X = theApp.HP_10cm2_MaximumLocation_GraphInfo[phantomID][0];
			maximum_dose_location_Y = theApp.HP_10cm2_MaximumLocation_GraphInfo[phantomID][1];
			maximum_dose_location_Z = theApp.HP_10cm2_MaximumLocation_GraphInfo[phantomID][2];
			maximum_local_skin_dose = theApp.HP_10cm2_MaximumDose_GraphInfo[phantomID];
			statistical_error = theApp.HP_10cm2_RelativeError_GraphInfo[phantomID] * 100.; // Statistical error 추가 
		}
		QString qstr_location_X = QString("(%1, ").arg(QString::number(maximum_dose_location_X, 'f', 4));
		QString qstr_location_Y = QString("%1, ").arg(QString::number(maximum_dose_location_Y, 'f', 4));
		QString qstr_location_Z = QString("%1)").arg(QString::number(maximum_dose_location_Z, 'f', 4));
		QString qstr_location_XYZ = "Max.pos: " + qstr_location_X + qstr_location_Y + qstr_location_Z;
		QString qstr_maximum_local_skin_dose = "Dose (pGy" + DoseUnit_QString + QString("): %1 ").arg(QString::number(maximum_local_skin_dose, 'e', 4));
		QString qstr_statistical_error = QString("Statistical error: %1%").arg(QString::number(statistical_error, 'f', 2));
		// 두 줄로 표시
		QString full_text = qstr_location_XYZ + "             " + qstr_maximum_local_skin_dose + "\n" + qstr_statistical_error;

		QCPTextElement *dose_info_text = qobject_cast<QCPTextElement*>(SkinDepthDose_LineGraph_QCustomPlot[skindose_type]->plotLayout()->element(1, 0));
		dose_info_text->setText(full_text);
		
		SkinDepthDose_LineGraph_QCustomPlot[skindose_type]->replot();
	}
}
void FunctionPanelRight::Update_SkinDose_HP_LineGraphWidgets() // called by (1) checkProcessInfo_Local
{
	int phantomID = m_Phantom_SequenceVector[0]; // Only for one phantom
	for (int skindose_type = 0; skindose_type < 2; skindose_type++)
	{
		QVector<double> x, y;
		if (skindose_type == 0) // 1 cm2
		{
			x.reserve(theApp.HP_1cm2_Depth_GraphInfo[phantomID].size());
			y.reserve(theApp.HP_1cm2_Depth_GraphInfo[phantomID].size());
			for (const auto& val : theApp.HP_1cm2_Depth_GraphInfo[0])
			{
				if (val.first > theApp.SkinAverageDepth[phantomID] * 10000) continue; // Skip the depth is deeper than reference skin depth
				x.append(val.first);
				y.append(val.second);
			}
		}
		if (skindose_type == 1) // 10 cm2
		{
			x.reserve(theApp.HP_10cm2_Depth_GraphInfo[phantomID].size());
			y.reserve(theApp.HP_10cm2_Depth_GraphInfo[phantomID].size());
			for (const auto& val : theApp.HP_10cm2_Depth_GraphInfo[phantomID])
			{
				if (val.first > theApp.SkinAverageDepth[phantomID] * 10000) continue; // Skip the depth is deeper than reference skin depth
				x.append(val.first);
				y.append(val.second);
			}
		}
		SkinDepthDose_LineGraph_QCustomPlot[skindose_type]->graph(0)->setData(x, y);
		SkinDepthDose_LineGraph_QCustomPlot[skindose_type]->rescaleAxes();
		SkinDepthDose_LineGraph_QCustomPlot[skindose_type]->yAxis->setRangeLower(0);
		SkinDepthDose_LineGraph_QCustomPlot[skindose_type]->replot();
	}
}
/*
std::vector<std::pair<double, double>> FunctionPanelRight::performAdaptiveDownsampling(const std::vector<std::pair<double, double>>& originalData, int targetSize) {

	// 목표 크기보다 작거나 같으면 그대로 반환
	if (originalData.size() <= targetSize) {
		return originalData;
	}

	if (targetSize <= 2) {
		// 목표 크기가 2 이하면 첫 번째와 마지막만 반환
		std::vector<std::pair<double, double>> result;
		result.push_back(originalData.front());
		if (targetSize == 2 && originalData.size() > 1) {
			result.push_back(originalData.back());
		}
		return result;
	}

	// 각 포인트의 중요도 계산
	std::vector<double> importanceScores;
	importanceScores.reserve(originalData.size());

	// 첫 번째와 마지막 포인트는 항상 최고 중요도
	importanceScores.push_back(1000000.0);

	for (size_t i = 1; i < originalData.size() - 1; i++) {
		double importance = 0.0;

		// 이전 구간 변화율 계산
		double prevTimeDiff = originalData[i].first - originalData[i - 1].first;
		double prevValueDiff = std::abs(originalData[i].second - originalData[i - 1].second);
		double prevChangeRate = (prevTimeDiff > 0) ? prevValueDiff / prevTimeDiff : 0;

		// 다음 구간 변화율 계산
		double nextTimeDiff = originalData[i + 1].first - originalData[i].first;
		double nextValueDiff = std::abs(originalData[i + 1].second - originalData[i].second);
		double nextChangeRate = (nextTimeDiff > 0) ? nextValueDiff / nextTimeDiff : 0;

		// 곡률 계산 (방향 변화)
		double curvature = 0.0;
		if (prevTimeDiff > 0 && nextTimeDiff > 0) {
			double prevSlope = (originalData[i].second - originalData[i - 1].second) / prevTimeDiff;
			double nextSlope = (originalData[i + 1].second - originalData[i].second) / nextTimeDiff;
			curvature = std::abs(nextSlope - prevSlope);
		}

		// 종합 중요도 = 변화율(70%) + 곡률(30%)
		importance = (prevChangeRate + nextChangeRate) * 0.7 + curvature * 0.3;
		importanceScores.push_back(importance);
	}

	importanceScores.push_back(1000000.0); // 마지막 포인트

	// 중요도와 인덱스를 함께 정렬
	std::vector<std::pair<double, int>> importanceIndex;
	importanceIndex.reserve(originalData.size());

	for (size_t i = 0; i < originalData.size(); i++) {
		importanceIndex.push_back({ importanceScores[i], static_cast<int>(i) });
	}

	// 중요도 순으로 정렬 (높은 순)
	std::sort(importanceIndex.begin(), importanceIndex.end(),
		[](const std::pair<double, int>& a, const std::pair<double, int>& b) {
			return a.first > b.first;
		});

	// 상위 targetSize개 선택
	std::vector<int> selectedIndices;
	selectedIndices.reserve(targetSize);

	int maxSelections = getmin(targetSize, static_cast<int>(importanceIndex.size()));
	for (int i = 0; i < maxSelections; i++) {
		selectedIndices.push_back(importanceIndex[i].second);
	}

	// 시간 순서로 재정렬
	std::sort(selectedIndices.begin(), selectedIndices.end());

	// 결과 생성
	std::vector<std::pair<double, double>> result;
	result.reserve(selectedIndices.size());

	for (int idx : selectedIndices) {
		result.push_back(originalData[idx]);
	}

	return result;
}
void FunctionPanelRight::handleProcessError(const QString& errorMessage) // Caculator
{
	LOG_ERROR(QString("Handling process error: %1").arg(errorMessage));

	try {
		// 타이머 정지
		if (theApp.managerCalculation->m_timer_LocalCheck && theApp.managerCalculation->m_timer_LocalCheck->isActive()) {
			theApp.managerCalculation->m_timer_LocalCheck->stop();
			LOG_INFO("Timer stopped due to error");
		}

		// UI 상태 초기화
		if (theApp.pInf) {
			theApp.pInf->ProgressLabel->setText("Error occurred");
			theApp.pInf->labelNowTimeRemaining->setText("Error");
			LOG_DEBUG("UI error state set");
		}

		// 에러 카운터 증가
		static int errorCount = 0;
		errorCount++;
		LOG_ERROR(QString("Total error count: %1").arg(errorCount));

		// 연속 에러가 너무 많으면 추가 조치
		if (errorCount > 10) {
			LOG_ERROR("Too many errors detected - consider stopping application");
		}

	}
	catch (...) {
		LOG_ERROR("Exception occurred during error handling cleanup");
	}
}
*/

//// Sub-function (2)
void FunctionPanelRight::show_Skindose_LineGraph_ValueTooltip(QMouseEvent *event) // called by (1) Create_Skindose_LineGraphWidgets
{
	for (int i = 0; i < 2; ++i) {
		if (SkinDepthDose_LineGraph_QCustomPlot[i]->underMouse()) {
			double x = SkinDepthDose_LineGraph_QCustomPlot[i]->xAxis->pixelToCoord(event->pos().x());
			double y = SkinDepthDose_LineGraph_QCustomPlot[i]->yAxis->pixelToCoord(event->pos().y());

			QCPGraph *graph = SkinDepthDose_LineGraph_QCustomPlot[i]->graph(0);
			if (!graph || graph->data()->size() == 0)
				continue;

			// === 변경 포인트: X축 기준으로 가장 가까운 포인트 찾기 ===
			double minDeltaX = DBL_MAX;
			int minIndex = -1;

			for (int dataIndex = 0; dataIndex < graph->data()->size(); ++dataIndex) {
				double key = graph->data()->at(dataIndex)->key;
				// X축 차이만 사용
				double currentDeltaX = qAbs(key - x);

				if (currentDeltaX < minDeltaX) {
					minDeltaX = currentDeltaX;
					minIndex = dataIndex;
				}
			}

			if (minIndex != -1) {
				double dataX = graph->data()->at(minIndex)->key;
				double dataY = graph->data()->at(minIndex)->value;

				QString tooltipText = QString("X: %1, Y: %2").arg(dataX).arg(dataY);
				QToolTip::showText(event->globalPos(), tooltipText, SkinDepthDose_LineGraph_QCustomPlot[i]);
			}
		}
	}
}


////////////////// Calculate_Server ////////////////// 

// Main function
/*
void FunctionPanelRight::uiRunning()
void FunctionPanelRight::ServerInitialization()
void FunctionPanelRight::Calculate_Server()
void FunctionPanelRight::DataInitialization_Server()
void FunctionPanelRight::checkProcessInfo_Server()
*/
//************************************** Save/Load and WriteFile Process **************************************//
void FunctionPanelRight::Write_TextResultFile_SimulationInfo(std::string filepath)
{
	// QFile* outFile = new QFile(QString::fromStdString(filepath));
	// QIODeviceOStream ofp_info(outFile);
	std::ofstream ofp_info(filepath);

	// Save simulation information
	//ofstream ofp_info(filepath);

	std::string McSEE_version = "Under-development";

	ofp_info << "======================================================================" << endl;
	ofp_info << "\t\t    _/      _/              _/_/_/  _/_/_/_/  _/_/_/_/\n" <<
		"\t\t   _/_/  _/_/    _/_/_/  _/        _/        _/       \n" <<
		"\t\t  _/  _/  _/  _/          _/_/    _/_/_/    _/_/_/    \n" <<
		"\t\t _/      _/  _/              _/  _/        _/         \n" <<
		"\t\t_/      _/    _/_/_/  _/_/_/    _/_/_/_/  _/_/_/_/    \n";
	ofp_info << "Monte Carlo Simulation for External Exposure (Ver. " << McSEE_version << ")" << endl;
	ofp_info << "======================================================================" << endl << endl;
	//ofp_info << "Calculation Date: " << timeStamp << endl << endl;
	ofp_info << "Calculation Date: " << theApp.managerCalculation->timeStamp << endl << endl;

	ofp_info << "=========================== Phantom_input ============================" << endl;
	ofp_info << "Phantoms:" << endl;
	for (auto itr_phantomIndex : m_Phantom_SequenceVector)
	{
		ofp_info << "[" << m_Phantom_ButtonName[itr_phantomIndex].toStdString() << "]" << endl;

		if (m_Phantom_MainInfo[itr_phantomIndex][E_PHANTOMMAININFO_CATEGORY] == E_PHANTOMCATEGORY_AIR)
		{
			ofp_info << "\tPosX(cm): " << m_Phantom_MainInfo[itr_phantomIndex][6] << endl; // PosX
			ofp_info << "\tPosY(cm): " << m_Phantom_MainInfo[itr_phantomIndex][7] << endl; // PosY
			ofp_info << "\tPosZ(cm): " << m_Phantom_MainInfo[itr_phantomIndex][8] << endl; // PosZ

			continue; // Air sphre이면 Position 정보만 출력
		}

		// Phantom type: MRCP
		if (m_Phantom_MainInfo[itr_phantomIndex][2] == E_PHANTOMTYPE_ADULTMRCP)
		{
			ofp_info << "\tPhantom_type: Adult_MRCP" << endl;
		}
		else if (m_Phantom_MainInfo[itr_phantomIndex][2] == E_PHANTOMTYPE_PEDIATRICMRCP)
		{
			ofp_info << "\tPhantom_type: Pediatric_MRCP" << endl;
		}
		else if (m_Phantom_MainInfo[itr_phantomIndex][2] == E_PHANTOMTYPE_PFMRCP)
		{
			ofp_info << "\tPhantom_type: Pregnant-female_MRCP" << endl;
		}
		// Phantom type: Tramsformed
		else if (m_Phantom_MainInfo[itr_phantomIndex][2] == E_PHANTOMTYPE_TRANSFORMED)
		{
			ofp_info << "\tPhantom_type: Transformed_phantom" << endl;
		}
		// Phantom type: Imported
		else if (m_Phantom_MainInfo[itr_phantomIndex][2] == E_PHANTOMTYPE_IMPORTED)
		{
			ofp_info << "\tPhantom_type: Imported_phantom" << endl;
		}

		if (m_Phantom_MainInfo[itr_phantomIndex][E_PHANTOMMAININFO_DUMMY] == E_PHANTOMDUMMY_YES) ofp_info << "\tDummy_phantom: Yes" << endl;
		else ofp_info << "\tDummy_phantom: No(Full_phantom)" << endl;

		// Gender
		if (m_Phantom_MainInfo[itr_phantomIndex][1] == 0) ofp_info << "\tGender: Male" << endl;
		else ofp_info << "\tGender: Female" << endl;

		// Age		
		if (m_Phantom_MainInfo[itr_phantomIndex][12] == 0) // Age: Adult 
		{
			ofp_info << "\tAge: Adult" << endl;
		}
		if (m_Phantom_MainInfo[itr_phantomIndex][12] == 1) // Age: 15 
		{
			ofp_info << "\tAge: 15y" << endl;
		}
		if (m_Phantom_MainInfo[itr_phantomIndex][12] == 2) // Age: 10 
		{
			ofp_info << "\tAge: 10y" << endl;
		}
		if (m_Phantom_MainInfo[itr_phantomIndex][12] == 3) // Age: 5 
		{
			ofp_info << "\tAge: 5y" << endl;
		}
		if (m_Phantom_MainInfo[itr_phantomIndex][12] == 4) // Age: 1 
		{
			ofp_info << "\tAge: 1y" << endl;
		}
		if (m_Phantom_MainInfo[itr_phantomIndex][12] == 5) // Age: 0 
		{
			ofp_info << "\tAge: 0y" << endl;
		}

		// Posutre		
		if (m_Phantom_MainInfo[itr_phantomIndex][3] == 0) ofp_info << "\tPosture: Standing" << endl;
		if (m_Phantom_MainInfo[itr_phantomIndex][3] == 1) ofp_info << "\tPosture: Bending" << endl;
		if (m_Phantom_MainInfo[itr_phantomIndex][3] == 2) ofp_info << "\tPosture: Kneeling" << endl;
		if (m_Phantom_MainInfo[itr_phantomIndex][3] == 3) ofp_info << "\tPosture: Sitting" << endl;
		if (m_Phantom_MainInfo[itr_phantomIndex][3] == 4) ofp_info << "\tPosture: Squatting" << endl;
		if (m_Phantom_MainInfo[itr_phantomIndex][3] == 5) ofp_info << "\tPosture: Walking" << endl;

		// Height/Weight
		ofp_info << "\tHeight(cm): " << m_Phantom_MainInfo[itr_phantomIndex][4] << endl; // Height
		ofp_info << "\tWeight(kg): " << m_Phantom_MainInfo[itr_phantomIndex][5] << endl; // Weight

		ofp_info << "\tPosX(cm): " << m_Phantom_MainInfo[itr_phantomIndex][6] << endl; // PosX
		ofp_info << "\tPosY(cm): " << m_Phantom_MainInfo[itr_phantomIndex][7] << endl; // PosY
		ofp_info << "\tPosZ(cm): " << m_Phantom_MainInfo[itr_phantomIndex][8] << endl; // PosZ
		ofp_info << "\tRotX(deg): " << m_Phantom_MainInfo[itr_phantomIndex][9] << endl; // RotX
		ofp_info << "\tRotY(deg): " << m_Phantom_MainInfo[itr_phantomIndex][10] << endl; // RotY
		ofp_info << "\tRotZ(deg): " << m_Phantom_MainInfo[itr_phantomIndex][11] << endl; // RotZ

		//Clothing
		ofp_info << endl << "\tClothing:" << endl;
		for (auto itr_clothingIndex : m_Clothing_SequenceVector[itr_phantomIndex])
		{
			if (m_Clothing_Type[itr_phantomIndex][itr_clothingIndex] == 0)
			{
				ofp_info << "\t[" << m_Clothing_ButtonName[itr_phantomIndex][itr_clothingIndex].toStdString() << "]" << endl;
			}
			if (m_Clothing_Type[itr_phantomIndex][itr_clothingIndex] == 1) ofp_info << "\t[Upper_head]" << endl;
			if (m_Clothing_Type[itr_phantomIndex][itr_clothingIndex] == 2) ofp_info << "\t[Upper_arm]" << endl;
			if (m_Clothing_Type[itr_phantomIndex][itr_clothingIndex] == 3) ofp_info << "\t[Lower_arm]" << endl;
			if (m_Clothing_Type[itr_phantomIndex][itr_clothingIndex] == 4) ofp_info << "\t[Hands]" << endl;
			if (m_Clothing_Type[itr_phantomIndex][itr_clothingIndex] == 5) ofp_info << "\t[Front_trunk]" << endl;
			if (m_Clothing_Type[itr_phantomIndex][itr_clothingIndex] == 6) ofp_info << "\t[Back_trunk]" << endl;
			if (m_Clothing_Type[itr_phantomIndex][itr_clothingIndex] == 7) ofp_info << "\t[Pelvis]" << endl;
			if (m_Clothing_Type[itr_phantomIndex][itr_clothingIndex] == 8) ofp_info << "\t[Upper_leg]" << endl;
			if (m_Clothing_Type[itr_phantomIndex][itr_clothingIndex] == 9) ofp_info << "\t[Lower_leg]" << endl;
			if (m_Clothing_Type[itr_phantomIndex][itr_clothingIndex] == 10) ofp_info << "\t[Feet]" << endl;
			for (auto itr_clothingLayerIndex : m_ClothingLayer_SequenceVector[itr_phantomIndex][itr_clothingIndex]) // 레이어순환
			{
				ofp_info << "\t\t[Layer_" << itr_clothingLayerIndex << "]" << endl; //Layer
				ofp_info << "\t\t\tThickness(cm): " << m_Clothing_MainInfo[itr_phantomIndex][itr_clothingIndex][itr_clothingLayerIndex][0] << endl; // thickness
				ofp_info << "\t\t\tDensity(g/cm3): " << m_Clothing_MainInfo[itr_phantomIndex][itr_clothingIndex][itr_clothingLayerIndex][1] << endl; // Density
				if (m_Clothing_MainInfo[itr_phantomIndex][itr_clothingIndex][itr_clothingLayerIndex][2] == 0) // Material: Cloth
					ofp_info << "\t\t\tMaterial: Cloth" << endl;
				if (m_Clothing_MainInfo[itr_phantomIndex][itr_clothingIndex][itr_clothingLayerIndex][2] == 1) // Material: Lead
					ofp_info << "\t\t\tMaterial: Lead" << endl;
				if (m_Clothing_MainInfo[itr_phantomIndex][itr_clothingIndex][itr_clothingLayerIndex][2] == 2) // Material: Air
					ofp_info << "\t\t\tMaterial: Air" << endl;
				ofp_info << "\t\t^" << endl;
			}
			ofp_info << "\t^" << endl;
		}
		ofp_info << endl;

		// Eyewear
		ofp_info << "\tEyewear:" << endl;
		//if (m_Clothing_MainInfo[itr_phantomIndex][100][0][2] != 0) // thickness가 0 이면 안경 정의 X
		//{
		//	if (m_Clothing_MainInfo[itr_phantomIndex][100][0][4] == 1) // Material: Glasses
		//		ofp_savefile << "\t\tMaterial: Glasses" << endl;
		//	if (m_Clothing_MainInfo[itr_phantomIndex][100][0][4] == 2) // Material: Leaded glasses
		//		ofp_savefile << "\t\tMaterial: Leaded glasses" << endl;

		//	ofp_savefile << "\t\tRadius of glasses(cm): " << m_Clothing_MainInfo[itr_phantomIndex][100][0][1] << endl;
		//	ofp_savefile << "\t\tSeparation distance (cm): " << m_Clothing_MainInfo[itr_phantomIndex][100][0][0] << endl;			
		//	ofp_savefile << "\t\tThickness (cm): " << m_Clothing_MainInfo[itr_phantomIndex][100][0][2] << endl;
		//	ofp_savefile << "\t\tDensity (g/cm3): " << m_Clothing_MainInfo[itr_phantomIndex][100][0][3] << endl;
		//}
		ofp_info << endl;

		//// Dosimeter
		ofp_info << "\tDosimeters:" << endl;

		//for (int DosimeterIdx = 0; DosimeterIdx < m_Clothing_MainInfo[itr_phantomIndex][999][0][0]; DosimeterIdx++) // dosimeter 순환
		//{
		//	ofp_savefile << "\tDosimeter name: Dosimeter_" << DosimeterIdx << endl;
		//	ofp_savefile << "\t\tRelative PosX (cm): " << m_Clothing_MainInfo[itr_phantomIndex][1000][DosimeterIdx][3] << endl; // Relative PosX
		//	ofp_savefile << "\t\tRelative PosY (cm): " << m_Clothing_MainInfo[itr_phantomIndex][1000][DosimeterIdx][4] << endl; // Relative PosY
		//	ofp_savefile << "\t\tRelative PosZ (cm): " << m_Clothing_MainInfo[itr_phantomIndex][1000][DosimeterIdx][5] << endl; // Relative PosZ

		//	ofp_savefile << "\t\tRadius (cm): " << m_Clothing_MainInfo[itr_phantomIndex][1000][DosimeterIdx][1] << endl; // Radius
		//	ofp_savefile << "\t\tDistance (cm): " << m_Clothing_MainInfo[itr_phantomIndex][1000][DosimeterIdx][2] << endl << endl; // Distance
		//  ofp_savefile << "\t^" << endl; //Dosimeter 구분
		//}
		ofp_info << endl;

		ofp_info << "^" << endl; // Phantom 구분
	}

	// Geometry information
	ofp_info << "=========================== Geometry_input ===========================" << endl;
	ofp_info << "Objects:" << endl;
	for (auto itr_objectIndex : m_Object_SequenceVector)
	{
		ofp_info << "[" << m_Object_ButtonName[itr_objectIndex].toStdString() << "]" << endl;
		// Material 각 행 분리
		std::istringstream ss(m_Object_MaterialInfo[itr_objectIndex].toStdString());
		std::string line;
		std::vector<std::string> material_lines;
		while (std::getline(ss, line))
		{
			material_lines.push_back(line);  // 각 줄을 벡터에 추가
		}

		if (m_ObjectType[itr_objectIndex] == 0) // 해당 GeomID가 Box 일때
		{
			ofp_info << "\tType: Box" << endl
				<< "\tHalf_LengthX(cm): " << m_Object_MainInfo[itr_objectIndex][7] << endl
				<< "\tHalf_LengthY(cm): " << m_Object_MainInfo[itr_objectIndex][8] << endl
				<< "\tHalf_LengthZ(cm): " << m_Object_MainInfo[itr_objectIndex][9] << endl
				<< "\tThicknessX(cm): " << m_Object_MainInfo[itr_objectIndex][10] << endl
				<< "\tThicknessY(cm): " << m_Object_MainInfo[itr_objectIndex][11] << endl
				<< "\tThicknessZ(cm): " << m_Object_MainInfo[itr_objectIndex][12] << endl
				<< "\tCenterX(cm): " << m_Object_MainInfo[itr_objectIndex][1] << endl
				<< "\tCenterY(cm): " << m_Object_MainInfo[itr_objectIndex][2] << endl
				<< "\tCenterZ(cm): " << m_Object_MainInfo[itr_objectIndex][3] << endl
				<< "\tRotateX(deg): " << m_Object_MainInfo[itr_objectIndex][4] << endl
				<< "\tRotateY(deg): " << m_Object_MainInfo[itr_objectIndex][5] << endl
				<< "\tRotateZ(deg): " << m_Object_MainInfo[itr_objectIndex][6] << endl
				<< "\tDensity(g/cm3): " << m_Object_MainInfo[itr_objectIndex][13] << endl
				<< "\tMaterial(";
			if (m_Object_MaterialFractionType[itr_objectIndex] == 0) ofp_info << "mass_fraction): " << endl;
			else ofp_info << "atom_fraction): " << endl;
			for (auto itr_material : material_lines)
			{
				ofp_info << "\t" << itr_material << endl;
			}
			ofp_info << "\tCharged_particle_transporation: " << m_Object_ChargedParticleTransport[itr_objectIndex] << endl;

		}
		if (m_ObjectType[itr_objectIndex] == 1) // 해당 GeomID가 sphere 일때
		{
			ofp_info << "\tType: Sphere" << endl
				<< "\tInner_Radius(cm): " << m_Object_MainInfo[itr_objectIndex][14] << endl
				<< "\tOuter_Radius(cm): " << m_Object_MainInfo[itr_objectIndex][15] << endl
				<< "\tStarting_Azimuthal_Angle(deg): " << m_Object_MainInfo[itr_objectIndex][16] << endl
				<< "\tDelta_Azimuthal_Angle(deg): " << m_Object_MainInfo[itr_objectIndex][17] << endl
				<< "\tStarting_Polar_Angle(deg): " << m_Object_MainInfo[itr_objectIndex][18] << endl
				<< "\tDelta_Polar_Angle(deg): " << m_Object_MainInfo[itr_objectIndex][19] << endl
				<< "\tCenterX(cm): " << m_Object_MainInfo[itr_objectIndex][1] << endl
				<< "\tCenterY(cm): " << m_Object_MainInfo[itr_objectIndex][2] << endl
				<< "\tCenterZ(cm): " << m_Object_MainInfo[itr_objectIndex][3] << endl
				<< "\tRotateX(deg): " << m_Object_MainInfo[itr_objectIndex][4] << endl
				<< "\tRotateY(deg): " << m_Object_MainInfo[itr_objectIndex][5] << endl
				<< "\tRotateZ(deg): " << m_Object_MainInfo[itr_objectIndex][6] << endl
				<< "\tDensity(g/cm3): " << m_Object_MainInfo[itr_objectIndex][13] << endl
				<< "\tMaterial(";
			if (m_Object_MaterialFractionType[itr_objectIndex] == 0) ofp_info << "mass_fraction): " << endl;
			else ofp_info << "atom_fraction): " << endl;
			for (auto itr_material : material_lines)
			{
				ofp_info << "\t" << itr_material << endl;
			}
			ofp_info << "\tCharged_particle_transporation: " << m_Object_ChargedParticleTransport[itr_objectIndex] << endl;
		}
		if (m_ObjectType[itr_objectIndex] == 2) // 해당 GeomID가 cylinder 일때
		{
			ofp_info << "\tType: Cylinder" << endl
				<< "\tInner_Radius(cm): " << m_Object_MainInfo[itr_objectIndex][20] << endl
				<< "\tOuter_Radius(cm): " << m_Object_MainInfo[itr_objectIndex][21] << endl
				<< "\tHalf_HeightZ(cm): " << m_Object_MainInfo[itr_objectIndex][22] << endl
				<< "\tStarting_Angle(deg): " << m_Object_MainInfo[itr_objectIndex][23] << endl
				<< "\tDelta_Angle(deg): " << m_Object_MainInfo[itr_objectIndex][24] << endl
				<< "\tCenterX(cm): " << m_Object_MainInfo[itr_objectIndex][1] << endl
				<< "\tCenterY(cm): " << m_Object_MainInfo[itr_objectIndex][2] << endl
				<< "\tCenterZ(cm): " << m_Object_MainInfo[itr_objectIndex][3] << endl
				<< "\tRotateX(deg): " << m_Object_MainInfo[itr_objectIndex][4] << endl
				<< "\tRotateY(deg): " << m_Object_MainInfo[itr_objectIndex][5] << endl
				<< "\tRotateZ(deg): " << m_Object_MainInfo[itr_objectIndex][6] << endl
				<< "\tDensity(g/cm3): " << m_Object_MainInfo[itr_objectIndex][13] << endl
				<< "\tMaterial(";
			if (m_Object_MaterialFractionType[itr_objectIndex] == 0) ofp_info << "mass_fraction): " << endl;
			else ofp_info << "atom_fraction): " << endl;
			for (auto itr_material : material_lines)
			{
				ofp_info << "\t" << itr_material << endl;
			}
			ofp_info << "\tCharged_particle_transporation: " << m_Object_ChargedParticleTransport[itr_objectIndex] << endl;
		}
		ofp_info << "^" << endl;
	}
	// Environment 정보 -> 환경매질 설정
	ofp_info << endl << "Environment: ";
	if (Geometry_EnvironmentType_QComboBox->currentIndex() == 0) ofp_info << "Vacuum " << endl;
	if (Geometry_EnvironmentType_QComboBox->currentIndex() == 1) ofp_info << "Air " << endl;
	if (Geometry_EnvironmentType_QComboBox->currentIndex() == 2) ofp_info << "Water " << endl;

	//Source information
	ofp_info << "============================ Source_input ============================" << endl;

	///
	auto* sourceWidget = getCurrentSourceWidget();
	sourceWidget->WriteSourceInfo(ofp_info);
	/// Write Source info
	
	ofp_info << endl << "===================== Calculation_setting_input ======================" << endl;
	// Target error
	ofp_info << setw(20) << std::left << "\tTarget_organ" << setw(20) << std::left << "Statistical_error(%)" << endl;

	for (int i = 0; i < OrganNameList.size(); i++) {
		if (m_CalculationSetting_TargetError[i] == "") continue;
		else {
			ofp_info << setw(20) << std::left << "\t" + OrganNameList[i].toStdString() << setw(20) <<
				m_CalculationSetting_TargetError[i].toStdString() << endl;
		}
	}
	// Target NPS
	ofp_info << "\tTarget_NPS: ";
	if (m_CalculationSetting_TargetNPS == "")
	{
		ofp_info << "None" << endl;
	}
	else ofp_info << m_CalculationSetting_TargetNPS.toStdString() << endl;
	// Result file name
	ofp_info << "\tResulf_file_name: ";
	if (m_CalculationSetting_ResultFileName.toStdString() != "") ofp_info << m_CalculationSetting_ResultFileName.toStdString() << endl;
	if (m_CalculationSetting_ResultFileName.toStdString() == "") ofp_info << "None" << endl;
	// Number of thread
	ofp_info << "\tNumber_of_thread: ";
	if (m_CalculationSetting_Thread.toStdString() != "") ofp_info << m_CalculationSetting_Thread.toStdString() << endl;
	if (m_CalculationSetting_Thread.toStdString() == "") ofp_info << "None" << endl;
	
	//// Calulation mode
	//ofp_info << "\tCalculation_mode: ";
	//if (SettingCalcModeInfo == 0)  ofp_info << "Local" << endl;
	//if (SettingCalcModeInfo == 1)  ofp_info << "Server" << endl;

	// outFile->close();
}

void FunctionPanelRight::Write_mcsee_File(QString filePath)
{
	// --- 1단계: QJason, 구조체 등 데이터 컨테이너 초기화 ---
	QJsonObject rootObject, manifestObject;
	   
	// 대형 데이터 처리
	QMap<QString, QString> resourcePathMap; // 대형 데이터 resource 매핑	
	QStringList orderedResourceIds; // 대형 데이터 리소스 ID의 처리 순서를 기억할 리스트
	qint64 currentRelativeOffset = 0;// 현재까지 기록된 바이너리 데이터의 총 크기 (상대 offset 계산용)

	// --- 2단계: 각 패널별 리소스 수집 및 sceneInfo 구성 ---
	QJsonArray phantomArray; // Phantom은 각 팬텀별 배열이 최상위 구조
	Gather_PhantomData_mcsee_File(phantomArray, manifestObject, orderedResourceIds, resourcePathMap, currentRelativeOffset);
	rootObject[JsonKeys::PhantomPanel] = phantomArray;	

	QJsonObject geometryPanelObject; // Geometry는 Environment와 각 구조물별 배열로 이루어진 구조
	Gather_GeometryData_mcsee_File(geometryPanelObject);
	rootObject[JsonKeys::GeometryPanel] = geometryPanelObject;
	// ToDo: 다른 패널 정보도 배열에 추가 //
		
	// --- 3단계: 순차적 파일 쓰기 ---
	QFile file(filePath);
	if (!file.open(QIODevice::WriteOnly)) { return; }
	QDataStream out(&file);
	out.setVersion(QDataStream::Qt_5_8);

	QByteArray mainJsonData = QJsonDocument(rootObject).toJson(QJsonDocument::Compact);
	QByteArray manifestJsonData = QJsonDocument(manifestObject).toJson(QJsonDocument::Compact);

	out << (quint64)mainJsonData.size();
	out << (quint64)manifestJsonData.size();
	out.writeRawData(mainJsonData.constData(), mainJsonData.size());
	out.writeRawData(manifestJsonData.constData(), manifestJsonData.size());

	// 핵심: 순서가 보장된 orderedResourceIds 리스트를 사용
	for (const QString& resourceId : orderedResourceIds) {
		QFile resourceFile(resourcePathMap[resourceId]);
		if (resourceFile.open(QIODevice::ReadOnly)) {
			char buffer[65536];
			while (qint64 bytesRead = resourceFile.read(buffer, sizeof(buffer))) {
				out.writeRawData(buffer, bytesRead);
			}
			resourceFile.close();
		}
	}

	file.close();
}
void FunctionPanelRight::Gather_PhantomData_mcsee_File(QJsonArray& outPhantomsArray, QJsonObject& outManifestObject, QStringList& outOrderedResourceIds, QMap<QString, QString>& outResourcePathMap, qint64& inoutCurrentOffset)
{
	for (auto itr_phantomIndex : m_Phantom_SequenceVector)
	{
		// 각 팬텀의 정보를 담을 JSON 객체 생성
		QJsonObject phantomObject;

		phantomObject[JsonKeys::Phantom::Name] = m_Phantom_ButtonName[itr_phantomIndex];

		// Air Sphere인 경우 위치 정보만 저장하고 다음 팬텀으로 넘어감
		if (m_Phantom_MainInfo[itr_phantomIndex][E_PHANTOMMAININFO_CATEGORY] == E_PHANTOMCATEGORY_AIR)
		{
			phantomObject[JsonKeys::Phantom::Category] = "Air"; 
			phantomObject[JsonKeys::Phantom::PosX_cm] = m_Phantom_MainInfo[itr_phantomIndex][6];
			phantomObject[JsonKeys::Phantom::PosY_cm] = m_Phantom_MainInfo[itr_phantomIndex][7];
			phantomObject[JsonKeys::Phantom::PosZ_cm] = m_Phantom_MainInfo[itr_phantomIndex][8];

			outPhantomsArray.append(phantomObject);
			continue;
		}

		// Phantom Type (숫자 대신 의미있는 문자열로 저장)
		int phantomType = m_Phantom_MainInfo[itr_phantomIndex][2];
		if (phantomType == E_PHANTOMTYPE_ADULTMRCP) phantomObject[JsonKeys::Phantom::Type] = "Adult_MRCP"; 
		if (phantomType == E_PHANTOMTYPE_PEDIATRICMRCP) phantomObject[JsonKeys::Phantom::Type] = "Pediatric_MRCP"; 
		if (phantomType == E_PHANTOMTYPE_PFMRCP) phantomObject[JsonKeys::Phantom::Type] = "pf_MRCP"; 
		else if (phantomType == E_PHANTOMTYPE_PEDIATRICMRCP) phantomObject[JsonKeys::Phantom::Type] = "Transformed"; 
		else if (phantomType == E_PHANTOMTYPE_IMPORTED)
		{
			phantomObject[JsonKeys::Phantom::Type] = "Imported";
			QString basePath = theApp.m_ImportedPhantomFilePath_NoExtention[itr_phantomIndex];
			QStringList fileTypes = { "obj", "ele", "node" };

			for (const QString& type : fileTypes) {
				QString resourceFilePath = basePath + "." + type;
				if (QFile::exists(resourceFilePath)) {
					QString resourceId = "res-" + QUuid::createUuid().toString(QUuid::WithoutBraces);
					if (type == "obj")       phantomObject[JsonKeys::Phantom::ObjResourceId] = resourceId;
					else if (type == "ele")  phantomObject[JsonKeys::Phantom::EleResourceId] = resourceId;
					else if (type == "node") phantomObject[JsonKeys::Phantom::NodeResourceId] = resourceId;

					outResourcePathMap[resourceId] = resourceFilePath;

					// 핵심: 정해진 순서대로 ID를 리스트에 추가
					outOrderedResourceIds.append(resourceId);

					QJsonObject resInfo;
					qint64 fileSize = QFileInfo(resourceFilePath).size();
					resInfo[JsonKeys::Manifest::phantom_Type] = type.toUpper(); 
					resInfo[JsonKeys::Manifest::OriginalName] = QFileInfo(resourceFilePath).fileName();
					resInfo[JsonKeys::Manifest::Size] = fileSize; 
					resInfo[JsonKeys::Manifest::Offset] = inoutCurrentOffset; 
					outManifestObject[resourceId] = resInfo;

					inoutCurrentOffset += fileSize;
				}
			}
		}

		phantomObject[JsonKeys::Phantom::IsDummy] = (m_Phantom_MainInfo[itr_phantomIndex][E_PHANTOMMAININFO_DUMMY] == E_PHANTOMDUMMY_YES);
		phantomObject[JsonKeys::Phantom::Gender] = (m_Phantom_MainInfo[itr_phantomIndex][1] == 0) ? "Male" : "Female";

		const QMap<int, QString> ageMap = { {0, "Adult"}, {1, "15y"}, {2, "10y"}, {3, "5y"}, {4, "1y"}, {5, "0y"} };
		phantomObject[JsonKeys::Phantom::Age] = ageMap.value(m_Phantom_MainInfo[itr_phantomIndex][12]);

		const QMap<int, QString> postureMap = { {0, "Standing"}, {1, "Bending"}, {2, "Kneeling"}, {3, "Sitting"}, {4, "Squatting"}, {5, "Walking"} };
		phantomObject[JsonKeys::Phantom::Posture] = postureMap.value(m_Phantom_MainInfo[itr_phantomIndex][3]);

		phantomObject[JsonKeys::Phantom::Height_cm] = m_Phantom_MainInfo[itr_phantomIndex][4];
		phantomObject[JsonKeys::Phantom::Weight_kg] = m_Phantom_MainInfo[itr_phantomIndex][5];
				
		phantomObject[JsonKeys::Phantom::PosX_cm] = m_Phantom_MainInfo[itr_phantomIndex][6];
		phantomObject[JsonKeys::Phantom::PosY_cm] = m_Phantom_MainInfo[itr_phantomIndex][7];
		phantomObject[JsonKeys::Phantom::PosZ_cm] = m_Phantom_MainInfo[itr_phantomIndex][8];

		phantomObject[JsonKeys::Phantom::RotX_deg] = m_Phantom_MainInfo[itr_phantomIndex][9];
		phantomObject[JsonKeys::Phantom::RotY_deg] = m_Phantom_MainInfo[itr_phantomIndex][10];
		phantomObject[JsonKeys::Phantom::RotZ_deg] = m_Phantom_MainInfo[itr_phantomIndex][11];
		
		// Clothing 정보 (배열 안에 객체들을 넣는 구조)
		QJsonArray clothingArray;
		for (auto itr_clothingIndex : m_Clothing_SequenceVector[itr_phantomIndex])
		{
			QJsonObject clothingObject;

			const QMap<int, QString> clothingTypeMap = {
				{0, m_Clothing_ButtonName[itr_phantomIndex][itr_clothingIndex]}, {1, "Upper_head"}, {2, "Upper_arm"},
				{3, "Lower_arm"}, {4, "Hands"}, {5, "Front_trunk"}, {6, "Back_trunk"}, {7, "Pelvis"}, {8, "Upper_leg"},
				{9, "Lower_leg"}, {10, "Feet"}
			};
			clothingObject[JsonKeys::Clothing::Name] = clothingTypeMap.value(m_Clothing_Type[itr_phantomIndex][itr_clothingIndex]);

			QJsonArray layersArray;
			for (auto itr_clothingLayerIndex : m_ClothingLayer_SequenceVector[itr_phantomIndex][itr_clothingIndex])
			{
				QJsonObject layerObject;
				layerObject[JsonKeys::Layer::Thickness_cm] = m_Clothing_MainInfo[itr_phantomIndex][itr_clothingIndex][itr_clothingLayerIndex][0];
				layerObject[JsonKeys::Layer::Density_g_cm3] = m_Clothing_MainInfo[itr_phantomIndex][itr_clothingIndex][itr_clothingLayerIndex][1];

				int materialType = m_Clothing_MainInfo[itr_phantomIndex][itr_clothingIndex][itr_clothingLayerIndex][2];
				if (materialType == 0) layerObject[JsonKeys::Layer::Material] = "Cloth";
				else if (materialType == 1) layerObject[JsonKeys::Layer::Material] = "Lead";
				else if (materialType == 2) layerObject[JsonKeys::Layer::Material] = "Air";

				layersArray.append(layerObject);
			}
			clothingObject[JsonKeys::Clothing::LayerArray] = layersArray;
			clothingArray.append(clothingObject);
		}
		phantomObject[JsonKeys::Phantom::ClothingArray] = clothingArray;

		// 완성된 팬텀 객체를 팬텀 배열에 추가
		outPhantomsArray.append(phantomObject);
	}
}
void FunctionPanelRight::Gather_GeometryData_mcsee_File(QJsonObject& outGeometryPanelObject)
{
	// 1. 모든 지오메트리 객체(Object) 정보 수집
	QJsonArray objectsArray;
	for (auto itr_objectIndex : m_Object_SequenceVector)
	{
		QJsonObject obj; // 현재 객체 정보를 담을 JSON 객체

		// 공통 정보
		obj[JsonKeys::Geometry::Name] = m_Object_ButtonName[itr_objectIndex];
		obj[JsonKeys::Geometry::Density] = m_Object_MainInfo[itr_objectIndex][13];
		obj[JsonKeys::Geometry::MaterialFraction] = m_Object_MaterialFractionType[itr_objectIndex];
		obj[JsonKeys::Geometry::MaterialInfo] = m_Object_MaterialInfo[itr_objectIndex];
		obj[JsonKeys::Geometry::ChargedParticleTransport] = m_Object_ChargedParticleTransport[itr_objectIndex];

		// 타입별 정보
		int objType = m_ObjectType[itr_objectIndex];
		if (objType == 0) // Box
		{
			obj[JsonKeys::Geometry::Type] = "Box";
			obj[JsonKeys::Geometry::BoxHalfLengthX] = m_Object_MainInfo[itr_objectIndex][7];
			obj[JsonKeys::Geometry::BoxHalfLengthY] = m_Object_MainInfo[itr_objectIndex][8];
			obj[JsonKeys::Geometry::BoxHalfLengthZ] = m_Object_MainInfo[itr_objectIndex][9];
			obj[JsonKeys::Geometry::BoxThicknessX] = m_Object_MainInfo[itr_objectIndex][10];
			obj[JsonKeys::Geometry::BoxThicknessY] = m_Object_MainInfo[itr_objectIndex][11];
			obj[JsonKeys::Geometry::BoxThicknessZ] = m_Object_MainInfo[itr_objectIndex][12];
			obj[JsonKeys::Geometry::PosX] = m_Object_MainInfo[itr_objectIndex][1];
			obj[JsonKeys::Geometry::PosY] = m_Object_MainInfo[itr_objectIndex][2];
			obj[JsonKeys::Geometry::PosZ] = m_Object_MainInfo[itr_objectIndex][3];
			obj[JsonKeys::Geometry::RotX] = m_Object_MainInfo[itr_objectIndex][4];
			obj[JsonKeys::Geometry::RotY] = m_Object_MainInfo[itr_objectIndex][5];
			obj[JsonKeys::Geometry::RotZ] = m_Object_MainInfo[itr_objectIndex][6];
		}
		else if (objType == 1) // Sphere
		{
			obj[JsonKeys::Geometry::Type] = "Sphere";
			obj[JsonKeys::Geometry::SphereInnerRadius] = m_Object_MainInfo[itr_objectIndex][14];
			obj[JsonKeys::Geometry::SphereOuterRadius] = m_Object_MainInfo[itr_objectIndex][15];
			obj[JsonKeys::Geometry::SphereStartPhi] = m_Object_MainInfo[itr_objectIndex][16];
			obj[JsonKeys::Geometry::SphereDeltaPhi] = m_Object_MainInfo[itr_objectIndex][17];
			obj[JsonKeys::Geometry::SphereStartTheta] = m_Object_MainInfo[itr_objectIndex][18];
			obj[JsonKeys::Geometry::SphereDeltaTheta] = m_Object_MainInfo[itr_objectIndex][19];
			obj[JsonKeys::Geometry::PosX] = m_Object_MainInfo[itr_objectIndex][1];
			obj[JsonKeys::Geometry::PosY] = m_Object_MainInfo[itr_objectIndex][2];
			obj[JsonKeys::Geometry::PosZ] = m_Object_MainInfo[itr_objectIndex][3];
			obj[JsonKeys::Geometry::RotX] = m_Object_MainInfo[itr_objectIndex][4];
			obj[JsonKeys::Geometry::RotY] = m_Object_MainInfo[itr_objectIndex][5];
			obj[JsonKeys::Geometry::RotZ] = m_Object_MainInfo[itr_objectIndex][6];
		}
		else if (objType == 2) // Cylinder
		{
			obj[JsonKeys::Geometry::Type] = "Cylinder";
			obj[JsonKeys::Geometry::CylinderInnerRadius] = m_Object_MainInfo[itr_objectIndex][20];
			obj[JsonKeys::Geometry::CylinderOuterRadius] = m_Object_MainInfo[itr_objectIndex][21];
			obj[JsonKeys::Geometry::CylinderHalfHeightZ] = m_Object_MainInfo[itr_objectIndex][22];
			obj[JsonKeys::Geometry::CylinderStartAngle] = m_Object_MainInfo[itr_objectIndex][23];
			obj[JsonKeys::Geometry::CylinderDeltaAngle] = m_Object_MainInfo[itr_objectIndex][24];
			obj[JsonKeys::Geometry::PosX] = m_Object_MainInfo[itr_objectIndex][1];
			obj[JsonKeys::Geometry::PosY] = m_Object_MainInfo[itr_objectIndex][2];
			obj[JsonKeys::Geometry::PosZ] = m_Object_MainInfo[itr_objectIndex][3];
			obj[JsonKeys::Geometry::RotX] = m_Object_MainInfo[itr_objectIndex][4];
			obj[JsonKeys::Geometry::RotY] = m_Object_MainInfo[itr_objectIndex][5];
			obj[JsonKeys::Geometry::RotZ] = m_Object_MainInfo[itr_objectIndex][6];
		}
		objectsArray.append(obj);
	}
	outGeometryPanelObject[JsonKeys::GeometryPanelKeys::ObjectArray] = objectsArray;

	// 2. 환경 정보 수집
	QString env = Geometry_EnvironmentType_QComboBox->currentText();
	outGeometryPanelObject[JsonKeys::GeometryPanelKeys::Environment] = env;
}

void FunctionPanelRight::Load_parse_mcsee_FileData(SceneData& outSceneData)
{
	if (!outSceneData.sourceFile || !outSceneData.sourceFile->isOpen()) return;

	QDataStream in(outSceneData.sourceFile);
	in.setVersion(QDataStream::Qt_5_8);

	// (1) 헤더 읽기
	quint64 mainJsonSize, manifestJsonSize;
	in >> mainJsonSize >> manifestJsonSize;

	// (2) 메인 JSON 읽기
	QByteArray mainJsonData = in.device()->read(mainJsonSize);

	// (3) Manifest JSON 읽기
	QByteArray manifestJsonData = in.device()->read(manifestJsonSize);

	// 메인 JSON 파싱
	QJsonDocument mainDoc = QJsonDocument::fromJson(mainJsonData);
	if (!mainDoc.isObject()) return;
	QJsonObject rootObject = mainDoc.object();

	QJsonArray phantomsArray = rootObject[JsonKeys::PhantomPanel].toArray();
	for (const QJsonValue& phantomValue : phantomsArray) {
		QJsonObject pObj = phantomValue.toObject();
		PhantomData phantomData;

		// 필수 값 검사: 이름과 타입은 반드시 있어야 함
		if (!pObj.contains(JsonKeys::Phantom::Name) || !pObj.contains(JsonKeys::Phantom::Type)) {
			theApp.SetMessageBox("[Parse Error] A phantom is missing required fields 'name' or 'phantomType'.");
		}

		phantomData.name = pObj[JsonKeys::Phantom::Name].toString(); 
		phantomData.category = pObj.value(JsonKeys::Phantom::Category).toString();
		phantomData.phantomType = pObj[JsonKeys::Phantom::Type].toString(); 
		phantomData.isDummy = pObj.value(JsonKeys::Phantom::IsDummy).toBool();
		phantomData.gender = pObj[JsonKeys::Phantom::Gender].toString();
		phantomData.age = pObj.value(JsonKeys::Phantom::Age).toString();
		phantomData.posture = pObj.value(JsonKeys::Phantom::Posture).toString();
		phantomData.height_cm = pObj.value(JsonKeys::Phantom::Height_cm).toDouble();
		phantomData.weight_kg = pObj.value(JsonKeys::Phantom::Weight_kg).toDouble();
		phantomData.posX = pObj.value(JsonKeys::Phantom::PosX_cm).toDouble();
		phantomData.posY = pObj.value(JsonKeys::Phantom::PosY_cm).toDouble();
		phantomData.posZ = pObj.value(JsonKeys::Phantom::PosZ_cm).toDouble();
		phantomData.rotX = pObj.value(JsonKeys::Phantom::RotX_deg).toDouble();
		phantomData.rotY = pObj.value(JsonKeys::Phantom::RotY_deg).toDouble();
		phantomData.rotZ = pObj.value(JsonKeys::Phantom::RotZ_deg).toDouble();

		phantomData.objResourceId = pObj[JsonKeys::Phantom::ObjResourceId].toString();
		phantomData.eleResourceId = pObj[JsonKeys::Phantom::EleResourceId].toString();
		phantomData.nodeResourceId = pObj[JsonKeys::Phantom::NodeResourceId].toString();
		
		// ... Clothing 등 다른 데이터 파싱 로직 추가 ...

		outSceneData.phantoms.append(phantomData);
	}
	
	// 지오메트리 패널 데이터 파싱
	if (rootObject.contains(JsonKeys::GeometryPanel))
	{
		QJsonObject geometryPanelObject = rootObject[JsonKeys::GeometryPanel].toObject();

		// 환경 정보 파싱
		outSceneData.environment = geometryPanelObject.value(JsonKeys::GeometryPanelKeys::Environment).toString("Vacuum");

		// 구조물 정보 파싱
		QJsonArray objectsArray = geometryPanelObject[JsonKeys::GeometryPanelKeys::ObjectArray].toArray();
		for (const QJsonValue& objValue : objectsArray)
		{
			QJsonObject obj = objValue.toObject();
			GeometryData geometryData;

			// 공통 정보
			geometryData.name = obj[JsonKeys::Geometry::Name].toString();
			geometryData.type = obj[JsonKeys::Geometry::Type].toString();
			geometryData.density = obj.value(JsonKeys::Geometry::Density).toDouble();
			geometryData.materialFraction = obj.value(JsonKeys::Geometry::MaterialFraction).toDouble();
			geometryData.materialInfo = obj[JsonKeys::Geometry::MaterialInfo].toString();
			geometryData.chargedParticleTransport = obj.value(JsonKeys::Geometry::ChargedParticleTransport).toBool();

			// 타입별 정보
			if (geometryData.type == "Box")
			{
				geometryData.box_halfLengthX = obj.value(JsonKeys::Geometry::BoxHalfLengthX).toDouble();
				geometryData.box_halfLengthY = obj.value(JsonKeys::Geometry::BoxHalfLengthY).toDouble();
				geometryData.box_halfLengthZ = obj.value(JsonKeys::Geometry::BoxHalfLengthZ).toDouble();
				geometryData.box_thicknessX = obj.value(JsonKeys::Geometry::BoxThicknessX).toDouble();
				geometryData.box_thicknessY = obj.value(JsonKeys::Geometry::BoxThicknessY).toDouble();
				geometryData.box_thicknessZ = obj.value(JsonKeys::Geometry::BoxThicknessZ).toDouble();
				geometryData.posX = obj.value(JsonKeys::Geometry::PosX).toDouble();
				geometryData.posY = obj.value(JsonKeys::Geometry::PosY).toDouble();
				geometryData.posZ = obj.value(JsonKeys::Geometry::PosZ).toDouble();
				geometryData.rotX = obj.value(JsonKeys::Geometry::RotX).toDouble();
				geometryData.rotY = obj.value(JsonKeys::Geometry::RotY).toDouble();
				geometryData.rotZ = obj.value(JsonKeys::Geometry::RotZ).toDouble();
			}
			else if (geometryData.type == "Sphere")
			{
				geometryData.sphere_innerRadius = obj.value(JsonKeys::Geometry::SphereInnerRadius).toDouble();
				geometryData.sphere_outerRadius = obj.value(JsonKeys::Geometry::SphereOuterRadius).toDouble();
				geometryData.sphere_startPhi = obj.value(JsonKeys::Geometry::SphereStartPhi).toDouble();
				geometryData.sphere_deltaPhi = obj.value(JsonKeys::Geometry::SphereDeltaPhi).toDouble();
				geometryData.sphere_startTheta = obj.value(JsonKeys::Geometry::SphereStartTheta).toDouble();
				geometryData.sphere_deltaTheta = obj.value(JsonKeys::Geometry::SphereDeltaTheta).toDouble();
				geometryData.posX = obj.value(JsonKeys::Geometry::PosX).toDouble();
				geometryData.posY = obj.value(JsonKeys::Geometry::PosY).toDouble();
				geometryData.posZ = obj.value(JsonKeys::Geometry::PosZ).toDouble();
				geometryData.rotX = obj.value(JsonKeys::Geometry::RotX).toDouble();
				geometryData.rotY = obj.value(JsonKeys::Geometry::RotY).toDouble();
				geometryData.rotZ = obj.value(JsonKeys::Geometry::RotZ).toDouble();
			}
			else if (geometryData.type == "Cylinder")
			{
				geometryData.cylinder_innerRadius = obj.value(JsonKeys::Geometry::CylinderInnerRadius).toDouble();
				geometryData.cylinder_outerRadius = obj.value(JsonKeys::Geometry::CylinderOuterRadius).toDouble();
				geometryData.cylinder_halfHeightZ = obj.value(JsonKeys::Geometry::CylinderHalfHeightZ).toDouble();
				geometryData.cylinder_startAngle = obj.value(JsonKeys::Geometry::CylinderStartAngle).toDouble();
				geometryData.cylinder_deltaAngle = obj.value(JsonKeys::Geometry::CylinderDeltaAngle).toDouble();
				geometryData.posX = obj.value(JsonKeys::Geometry::PosX).toDouble();
				geometryData.posY = obj.value(JsonKeys::Geometry::PosY).toDouble();
				geometryData.posZ = obj.value(JsonKeys::Geometry::PosZ).toDouble();
				geometryData.rotX = obj.value(JsonKeys::Geometry::RotX).toDouble();
				geometryData.rotY = obj.value(JsonKeys::Geometry::RotY).toDouble();
				geometryData.rotZ = obj.value(JsonKeys::Geometry::RotZ).toDouble();
			}
			outSceneData.geometries.append(geometryData);
		}
	}


	// (4) Manifest JSON 파싱 및 최종 offset 계산
	QJsonDocument manifestDoc = QJsonDocument::fromJson(manifestJsonData);
	if (!manifestDoc.isObject()) return;
	QJsonObject manifestObject = manifestDoc.object();

	// 바이너리 데이터가 시작되는 파일 내의 '절대 위치' 계산 -> 두 개의 안내 정보(메인 정보 크기, 목차 정보 크기)가 차지하는 공간 + 메인정보 차지 공간 + 목차 차지 공간
	qint64 binaryDataStartOffset = sizeof(quint64) * 2 + mainJsonSize + manifestJsonSize;

	for (const QString& key : manifestObject.keys()) {
		QJsonObject resObj = manifestObject[key].toObject();
		ResourceInfo resInfo;
		resInfo.id = key;
		resInfo.type = resObj[JsonKeys::Manifest::phantom_Type].toString(); 
		resInfo.originalName = resObj[JsonKeys::Manifest::OriginalName].toString(); 
		resInfo.size = resObj[JsonKeys::Manifest::Size].toVariant().toLongLong();

		// 최종 offset = 바이너리 데이터 시작 위치 + Manifest에 기록된 상대 위치
		resInfo.offset = binaryDataStartOffset + resObj[JsonKeys::Manifest::Offset].toVariant().toLongLong();

		outSceneData.resourceManifest[key] = resInfo;
	}

}
void FunctionPanelRight::Load_commit_mcsee_FileData(const SceneData& sceneData)
{
	setUpdatesEnabled(false); // 업데이트 전 UI 깜빡임 방지

	for (const PhantomData& pData : sceneData.phantoms)
	{
		// Phantom Add 버튼 클릭 -> Adding Setting 창 실행			
		PhantomAddingSetting_Create();

		// Phantom Adding Setting의 인자 입력 및 OK 버튼 클릭
		Commit_PhantomSetting(pData);
		slot_PhantomAddingOK_ButtonClicked();

		// Phantom Add 루틴 실행
		ExecutePhantomAddRoutines();
	}

	// 지오메트리 패널 내의 환경 정보 커밋
	int envIndex = Geometry_EnvironmentType_QComboBox->findText(sceneData.environment);
	if (envIndex != -1) {
		Geometry_EnvironmentType_QComboBox->setCurrentIndex(envIndex);
	}

	for (const GeometryData& gData : sceneData.geometries)
	{
		// Object Add 버튼 클릭 -> Adding Setting 창 실행	
		ObjectAddingSetting_Create();   

		// Object Adding Setting의 인자 입력 및 OK 버튼 클릭		
		Commit_GeometrySetting(gData);
		slot_ObjectAddingOK_ButtonClicked();

		// Object Add 루틴 실행
		ExecuteObjectAddRoutines();
	}	

	// 다른 패널 데이터 커밋 로직이 여기에 추가됩니다.

	setUpdatesEnabled(true); // UI 업데이트 재개
}
void FunctionPanelRight::Commit_PhantomSetting(const PhantomData& pData)
{
	// Phantom Name
	PhantomSetting_Name_QLineEdit->setText(pData.name);

	// Phantom Type
	if (pData.phantomType == "Adult_MRCP") PhantomSetting_Type_QComboBox->setCurrentIndex(E_PHANTOMTYPE_ADULTMRCP);
	else if (pData.phantomType == "Pediatric_MRCP") PhantomSetting_Type_QComboBox->setCurrentIndex(E_PHANTOMTYPE_PEDIATRICMRCP);
	else if (pData.phantomType == "pf_MRCP") PhantomSetting_Type_QComboBox->setCurrentIndex(E_PHANTOMTYPE_PFMRCP);
	else if (pData.phantomType == "Transformed") PhantomSetting_Type_QComboBox->setCurrentIndex(E_PHANTOMTYPE_TRANSFORMED);
	else if (pData.phantomType == "Imported")
	{
		PhantomSetting_Type_QComboBox->setCurrentIndex(E_PHANTOMTYPE_IMPORTED);

		QFileInfo fi(pData.objFilePath); // .obj, .ele, .node 모두 같음!
		QString pathNoExt = fi.path() + "/" + fi.completeBaseName();		
		PhantomSetting_ImportedFileName_QLineEdit->setText(pathNoExt); // .obj 확장자 제거 
	}

	// Dummy
	PhantomSetting_Dummy_QComboBox->setCurrentIndex(pData.isDummy ? 0 : 1);

	// Gender
	PhantomSetting_Gender_QComboBox->setCurrentIndex((pData.gender == "Male") ? 0 : 1);

	// Age & Posture (QMap을 사용해 if문 단순화)
	const QMap<QString, int> ageMap = { {"Adult", 0}, {"15y", 1}, {"10y", 2}, {"5y", 3}, {"1y", 4}, {"0y", 5} };
	PhantomSetting_Age_QComboBox->setCurrentIndex(ageMap.value(pData.age)); // 키가 없으면 기본값 0(Adult)

	const QMap<QString, int> postureMap = { {"Standing", 0}, {"Bending", 1}, {"Kneeling", 2}, {"Sitting", 3}, {"Squatting", 4}, {"Walking", 5} };
	PhantomSetting_Posture_QComboBox->setCurrentIndex(postureMap.value(pData.posture)); // 키가 없으면 기본값 0(Standing)

	// Height, Weight, Position, Rotation
	PhantomSetting_Height_QLineEdit->setText(QString::number(pData.height_cm));
	PhantomSetting_Weight_QLineEdit->setText(QString::number(pData.weight_kg));
	PhantomSetting_PosX_QLineEdit->setText(QString::number(pData.posX));
	PhantomSetting_PosY_QLineEdit->setText(QString::number(pData.posY));
	PhantomSetting_PosZ_QLineEdit->setText(QString::number(pData.posZ));
	PhantomSetting_RotX_QLineEdit->setText(QString::number(pData.rotX));
	PhantomSetting_RotY_QLineEdit->setText(QString::number(pData.rotY));
	PhantomSetting_RotZ_QLineEdit->setText(QString::number(pData.rotZ));
}
void FunctionPanelRight::Commit_GeometrySetting(const GeometryData& gData)
{
	ObjectSetting_Name_QLineEdit->setText(gData.name);
	ObjectSetting_Density_QLineEdit->setText(QString::number(gData.density));
	ObjectSetting_MaterialPanelTextWindow_UserQDialog->setText(gData.materialInfo);
	if (gData.materialFraction == 0) {
		ObjectSetting_MaterialMassFraction_QRadioButton->setChecked(true);
	}
	else {
		ObjectSetting_MaterialAtomFraction_QRadioButton->setChecked(true);
	}
	// ChargedParticleTransport는 UI에 직접 연결된 위젯이 없어 보입니다.
	// 필요시 내부 변수(e.g., m_Object_ChargedParticleTransport)에 직접 할당해야 합니다.

	if (gData.type == "Box") {
		ObjectSetting_Type_QComboBox->setCurrentIndex(0);
		ObjectSetting_BoxLengthX_QLineEdit->setText(QString::number(gData.box_halfLengthX));
		ObjectSetting_BoxLengthY_QLineEdit->setText(QString::number(gData.box_halfLengthY));
		ObjectSetting_BoxLengthZ_QLineEdit->setText(QString::number(gData.box_halfLengthZ));
		ObjectSetting_BoxThicknessX_QLineEdit->setText(QString::number(gData.box_thicknessX));
		ObjectSetting_BoxThicknessY_QLineEdit->setText(QString::number(gData.box_thicknessY));
		ObjectSetting_BoxThicknessZ_QLineEdit->setText(QString::number(gData.box_thicknessZ));
		ObjectSetting_BoxCenterX_QLineEdit->setText(QString::number(gData.posX));
		ObjectSetting_BoxCenterY_QLineEdit->setText(QString::number(gData.posY));
		ObjectSetting_BoxCenterZ_QLineEdit->setText(QString::number(gData.posZ));
		ObjectSetting_BoxRotateX_QLineEdit->setText(QString::number(gData.rotX));
		ObjectSetting_BoxRotateY_QLineEdit->setText(QString::number(gData.rotY));
		ObjectSetting_BoxRotateZ_QLineEdit->setText(QString::number(gData.rotZ));
	}
	else if (gData.type == "Sphere") {
		ObjectSetting_Type_QComboBox->setCurrentIndex(1);
		ObjectSetting_SphereInnerRadius_QLineEdit->setText(QString::number(gData.sphere_innerRadius));
		ObjectSetting_SphereOuterRadius_QLineEdit->setText(QString::number(gData.sphere_outerRadius));
		ObjectSetting_SphereStartingAzimuthalPhi_QLineEdit->setText(QString::number(gData.sphere_startPhi));
		ObjectSetting_SphereDeltaAzimuthalPhi_QLineEdit->setText(QString::number(gData.sphere_deltaPhi));
		ObjectSetting_SphereStartingPolarTheta_QLineEdit->setText(QString::number(gData.sphere_startTheta));
		ObjectSetting_SphereDeltaPolarTheta_QLineEdit->setText(QString::number(gData.sphere_deltaTheta));
		ObjectSetting_SphereCenterX_QLineEdit->setText(QString::number(gData.posX));
		ObjectSetting_SphereCenterY_QLineEdit->setText(QString::number(gData.posY));
		ObjectSetting_SphereCenterZ_QLineEdit->setText(QString::number(gData.posZ));
		ObjectSetting_SphereRotateX_QLineEdit->setText(QString::number(gData.rotX));
		ObjectSetting_SphereRotateY_QLineEdit->setText(QString::number(gData.rotY));
		ObjectSetting_SphereRotateZ_QLineEdit->setText(QString::number(gData.rotZ));
	}
	else if (gData.type == "Cylinder") {
		ObjectSetting_Type_QComboBox->setCurrentIndex(2);
		ObjectSetting_CylinderInnerRadius_QLineEdit->setText(QString::number(gData.cylinder_innerRadius));
		ObjectSetting_CylinderOuterRadius_QLineEdit->setText(QString::number(gData.cylinder_outerRadius));
		ObjectSetting_CylinderHeightZ_QLineEdit->setText(QString::number(gData.cylinder_halfHeightZ));
		ObjectSetting_CylinderStartingAngle_QLineEdit->setText(QString::number(gData.cylinder_startAngle));
		ObjectSetting_CylinderDeltaAngle_QLineEdit->setText(QString::number(gData.cylinder_deltaAngle));
		ObjectSetting_CylinderCenterX_QLineEdit->setText(QString::number(gData.posX));
		ObjectSetting_CylinderCenterY_QLineEdit->setText(QString::number(gData.posY));
		ObjectSetting_CylinderCenterZ_QLineEdit->setText(QString::number(gData.posZ));
		ObjectSetting_CylinderRotateX_QLineEdit->setText(QString::number(gData.rotX));
		ObjectSetting_CylinderRotateY_QLineEdit->setText(QString::number(gData.rotY));
		ObjectSetting_CylinderRotateZ_QLineEdit->setText(QString::number(gData.rotZ));
	}
}

void FunctionPanelRight::ResetCalculation() // Output 패널 관련 정보를 초기화
{
	// global variables
	m_skinDoseVisualizationButton->setEnabled(false);
	theApp.pRslt->flg_GraphUpdate = false;
	theApp.pInf->hide();
	theApp.pRslt->hide();
	RunningIndex = 0; // UI가 돌아가고 있는 지 누적해서 더해가는 변수

	b_IsTargetErrorAbsent = false; // 지정한 Target error가 없으면 true
	b_IsTargetNPSAbsent = false; // 지정한 Target number_of_event가 없으면 true
	b_IsCalculationExecuted = false; // 계산버튼을 눌러서 실행된 상태
	b_IsRealtimeCalculation = false; // 실시간계산중이면 true -> realtime panel show/hide 하기 위함
	b_IsOutputResultLoaded = false; // Output이 최초로 한 번이라도 load 되었으면 true -> 아웃풋패널 클릭 혹은 팬텀버튼 클릭 시 결과 업데이트하기 위함
	ProgressBarValue = 0; // ProgressBar의 진행률 값
	theApp.pInf->bar->setValue(0);
	nEvent = 0; // 현재까지 Geant4에서 수송된 NPS 값

	// Delete phantom list button
	for (auto itr_outputButton : m_Phantom_SequenceVector)
	{
		// OutputPhantomButton 맵에서 버튼을 찾음
		auto button = OutputPhantomButton[itr_outputButton];
		if (button)
		{
			Buttons_FunctionPanelRight.removeOne(button);
			// 레이아웃에서 버튼 제거
			layoutOutputPhantomButtonList->removeWidget(button);
			// 버튼 삭제
			delete button;
		}
	}
	OutputPhantomButton.clear();

	// Delete Organ - Dose bar graph
	if (OrganDose_BarGraph_QVBoxLayout)
	{
		QLayoutItem* item;
		while ((item = OrganDose_BarGraph_QVBoxLayout->takeAt(0)) != nullptr)
		{
			// 만약 위젯이 있다면 삭제
			if (item->widget())
			{
				delete item->widget();
			}
			// 아이템 자체도 삭제
			delete item;
		}
	}
	// OrganDose_BarGraph_QCustomPlot 포인터가 직접 접근 가능한 경우 초기화
	if (OrganDose_BarGraph_QCustomPlot)
	{
		delete OrganDose_BarGraph_QCustomPlot;
		OrganDose_BarGraph_QCustomPlot = nullptr;
	}

	// Delete  Personal dose equivalent bar graph graph
	if (DosimeterDose_BarGraph_QVBoxLayout)
	{
		QLayoutItem* item;
		while ((item = DosimeterDose_BarGraph_QVBoxLayout->takeAt(0)) != nullptr)
		{
			// 만약 위젯이 있다면 삭제
			if (item->widget())
			{
				delete item->widget();
			}
			// 아이템 자체도 삭제
			delete item;
		}
	}
	// Dosimeter_BarGraph_QCustomPlot 포인터가 직접 접근 가능한 경우 초기화
	if (Dosimeter_BarGraph_QCustomPlot)
	{
		delete Dosimeter_BarGraph_QCustomPlot;
		Dosimeter_BarGraph_QCustomPlot = nullptr;
	}

	// Delete  H_AK bar graph
	if (H_AK_BarGraph_QVBoxLayout)
	{
		QLayoutItem* item;
		while ((item = H_AK_BarGraph_QVBoxLayout->takeAt(0)) != nullptr)
		{
			// 만약 위젯이 있다면 삭제
			if (item->widget())
			{
				delete item->widget();
			}
			// 아이템 자체도 삭제
			delete item;
		}
	}
	// H_AK_BarGraph_QCustomPlot 포인터가 직접 접근 가능한 경우 초기화
	if (H_AK_BarGraph_QCustomPlot)
	{
		delete H_AK_BarGraph_QCustomPlot;
		H_AK_BarGraph_QCustomPlot = nullptr;
	}

	// Delete  H_AK bar graph
	if (SkinDepthDose_LineGraph_QVBoxLayout)
	{
		QLayoutItem* item;
		while ((item = SkinDepthDose_LineGraph_QVBoxLayout->takeAt(0)) != nullptr)
		{
			// 만약 위젯이 있다면 삭제
			if (item->widget())
			{
				delete item->widget();
			}
			// 아이템 자체도 삭제
			delete item;
		}
	}
	// SkinDepthDose_LineGraph_QCustomPlot 포인터가 직접 접근 가능한 경우 초기화
	if (SkinDepthDose_LineGraph_QCustomPlot[0])
	{
		delete SkinDepthDose_LineGraph_QCustomPlot[0];
		SkinDepthDose_LineGraph_QCustomPlot[0] = nullptr;
	}
	if (SkinDepthDose_LineGraph_QCustomPlot[1])
	{
		delete SkinDepthDose_LineGraph_QCustomPlot[1];
		SkinDepthDose_LineGraph_QCustomPlot[1] = nullptr;
	}

	// Delete  H_AK bar graph
	if (theApp.pRslt->Layout_OrganDose_RealtimeLineGraph_QVBoxLayout)
	{
		QLayoutItem* item;
		while ((item = theApp.pRslt->Layout_OrganDose_RealtimeLineGraph_QVBoxLayout->takeAt(0)) != nullptr)
		{
			// 만약 위젯이 있다면 삭제
			if (item->widget())
			{
				delete item->widget();
			}
			// 아이템 자체도 삭제
			delete item;
		}
	}
	for (auto itr : theApp.pRslt->OrganDose_RealtimeLineGraph_QCustomPlot)
	{
		delete itr.second;
		itr.second = nullptr;
	}

	// Skin layer generation
	theApp.SkinDenstiy.clear();
	theApp.SkinAverageDepth.clear(); //SkinAverageDepth[phantomID] = average depth from outer skin to inner skin (RST) (cm)
	theApp.SkinLayer_PolyData.clear();
	theApp.FacetInfo.clear(); //FacetInfo[phantomID][FacetID] = 무게중심점의 [centerx(cm), centery(cm), centerz(cm), nx, ny, nz, Distance to inner skin surface(RST)(cm), area of facet(cm2)]
	theApp.VertexToFacetMap.clear(); // VertexToFacetMap[phantomID][vertexID] = {facetID1, facetID2, ...}
	theApp.TinyFacetAdjacentFacetMap.clear(); // TinyFacetAdjacentFacetMap[phantomID][facetID] = {facetID1, facetID2, ...}
	theApp.TinyFacetAdjacentFacetVolumeSum.clear(); // TinyFacetAdjacentFacetVolumeSum[phantomID][facetID] = areaSum
	// For skin dose visulaization 
	theApp.b_IsSkinDoseVisualizationOn = false;
	for (auto itr : theApp.SkinPhantomActor)
	{
		theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(itr.second);
		itr.second = nullptr;
	}
	theApp.SkinPhantomActor.clear();
	theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(theApp.SkinDoseVisualization_ScalarBar);
	theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(theApp.SkinDoseVisualization_Label0percent);
	theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(theApp.SkinDoseVisualization_Label20percent);
	theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(theApp.SkinDoseVisualization_Label40percent);
	theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(theApp.SkinDoseVisualization_Label60percent);
	theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(theApp.SkinDoseVisualization_Label80percent);
	theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(theApp.SkinDoseVisualization_Label100percent);
	theApp.SkinDoseVisualization_ScalarBar = nullptr;
	theApp.SkinDoseVisualization_Label0percent = nullptr;
	theApp.SkinDoseVisualization_Label20percent = nullptr;
	theApp.SkinDoseVisualization_Label40percent = nullptr;
	theApp.SkinDoseVisualization_Label60percent = nullptr;
	theApp.SkinDoseVisualization_Label80percent = nullptr;
	theApp.SkinDoseVisualization_Label100percent = nullptr;

	// Skin dose visulaization calculation
	theApp.Facet_DE_DE2.clear(); //FacetDE[phantomID][FacetID] = 누적 dE
	theApp.VertexDose.clear(); //VertexDose[phantomID][vertexID] = 누적 Dose(Gy, Joule/kg)

	// HP skin dose depth calculation	
	theApp.HP_FacetDE.clear(); //FacetDE[phantomID][FacetID] = 누적 dE
	theApp.HP_FacetDE_Depth.clear(); //HP_FacetDE_Depth[phantomID][FacetID][depth(cm)] = 누적 dE
	// 1, 10cm2 국부 facet의 선량 -> 각각 최대 선량 지점(facetID, 해당좌표) 알기 위함
	theApp.HP_1cm2_MaximumLocation_GraphInfo.clear();
	theApp.HP_1cm2_MaximumDose_GraphInfo.clear();
	theApp.HP_10cm2_MaximumLocation_GraphInfo.clear();
	theApp.HP_10cm2_MaximumDose_GraphInfo.clear();
	// 1, 10cm2 깊이분포 -> 그래프제작용
	theApp.HP_1cm2_Depth_GraphInfo.clear(); // HP_1cm2_Depth_GraphInfo[phantomID] = {depth 간격의 중간, dose}, {depth 간격의 중간, dose}, {depth 간격의 중간, dose}...
	theApp.HP_10cm2_Depth_GraphInfo.clear(); // HP_1cm2_Depth_GraphInfo[phantomID] = {depth 간격의 중간, dose}, {depth 간격의 중간, dose}, {depth 간격의 중간, dose}...
	// 선량결과 업데이트
	m_graphInfo_OrganDose_map.clear();
	m_graphInfo_TimeDose_map.clear();
	m_graphInfo_Error_map.clear();
	TargetOrganError_Map.clear();
	theApp.OutputDose_data.clear();
	// 선량결과 패널 및 organ-dosse 그래프 표시
	for (int i = 0; i < OrganNameList.size(); i++)
	{
		OutputPanel_DoseResult_QLineEdit[OrganNameList[i]]->clear();
	}

	SetGeometryPanelMode();
	Geometry_QPushButton->setChecked(true);
	for (auto itr_phantomindex : m_Phantom_SequenceVector)
	{
		theApp.PhantomPanelActor[itr_phantomindex]->VisibilityOn();
	}
	theApp.UpdatePhantom_ActorHighlighted(const_Phantom_MaximumCount); // 전부 highlight and pickable 끄기
}

void FunctionPanelRight::LoadReconsturctionFile_previous(QString dir) // 무조건 아예 프로그램 처음 킨 상태로 시작해야 함
{	
	// QFile* inputFile = new QFile(dir);
	// QIODeviceIStream ifp(inputFile);

	std::ifstream ifp(dir.toStdString());
	std::string dump;
	while (!ifp.eof())
	{
		ifp >> dump;
		// Phantom_input
		if (dump == "Phantom_input")
		{
			ifp >> dump;
			if (dump == "============================")
			{
				ifp >> dump; // Phantoms:
				while (ifp >> dump) // [phantom_name] or "==========================="
				{
					if (dump == "===========================") break; // To geometry panel
					if (dump != "===========================") // Phantom name
					{
						timer_Close_PhantomUIdialog_Recon->start(100);
						PhantomAddingSetting_Create();
						
						PhantomSetting_Name_QLineEdit->setText(QString::fromStdString(theApp.ExtractInnerString(dump)));

						ifp >> dump >> dump; // Type: MRCP/Transformed/Imported						
						if (dump == "MRCP") PhantomSetting_Type_QComboBox->setCurrentIndex(0);
						if (dump == "Transformed_phantom") PhantomSetting_Type_QComboBox->setCurrentIndex(1);
						if (dump == "Imported_phantom")
						{
							PhantomSetting_Type_QComboBox->setCurrentIndex(2);

							// 1) QFileInfo 로 디렉터리 경로만 추출
							QFileInfo fi(dir);
							QString folderPath = fi.path();

							// 2) 파일명 생성
							QString filename = QStringLiteral("ImportedPhantom_%1.ply")
								.arg(m_Phantom_MakingIndex);

							// 3) 디렉터리 + 파일명 합치기
							QString ply_path = QDir(folderPath).filePath(filename);

							PhantomSetting_ImportedFileName_QLineEdit->setText(ply_path);
						}

						ifp >> dump >> dump; // Dummy yes or no						
						if (dump == "No(Full_phantom)") PhantomSetting_Dummy_QComboBox->setCurrentIndex(1);
						if (dump == "Yes") PhantomSetting_Dummy_QComboBox->setCurrentIndex(0);

						ifp >> dump >> dump; // Gender						
						if (dump == "Male")PhantomSetting_Gender_QComboBox->setCurrentIndex(0);
						if (dump == "Female")PhantomSetting_Gender_QComboBox->setCurrentIndex(1);

						ifp >> dump >> dump; // Age						
						if (dump == "Adult") PhantomSetting_Age_QComboBox->setCurrentIndex(0);
						if (dump == "15y") PhantomSetting_Age_QComboBox->setCurrentIndex(1);
						if (dump == "10y") PhantomSetting_Age_QComboBox->setCurrentIndex(2);
						if (dump == "5y") PhantomSetting_Age_QComboBox->setCurrentIndex(3);
						if (dump == "1y") PhantomSetting_Age_QComboBox->setCurrentIndex(4);
						if (dump == "0y") PhantomSetting_Age_QComboBox->setCurrentIndex(5);

						ifp >> dump >> dump; // Posture						
						if (dump == "Standing") PhantomSetting_Posture_QComboBox->setCurrentIndex(0);
						if (dump == "Bending") PhantomSetting_Posture_QComboBox->setCurrentIndex(1);
						if (dump == "Kneeling") PhantomSetting_Posture_QComboBox->setCurrentIndex(2);
						if (dump == "Sitting") PhantomSetting_Posture_QComboBox->setCurrentIndex(3);
						if (dump == "Squatting") PhantomSetting_Posture_QComboBox->setCurrentIndex(4);
						if (dump == "Walking") PhantomSetting_Posture_QComboBox->setCurrentIndex(5);
						ifp >> dump >> dump; PhantomSetting_Height_QLineEdit->setText(QString::fromStdString(dump)); // Height
						ifp >> dump >> dump; PhantomSetting_Weight_QLineEdit->setText(QString::fromStdString(dump)); // Weight
						ifp >> dump >> dump; PhantomSetting_PosX_QLineEdit->setText(QString::fromStdString(dump)); // PosX
						ifp >> dump >> dump; PhantomSetting_PosY_QLineEdit->setText(QString::fromStdString(dump)); // PosY
						ifp >> dump >> dump; PhantomSetting_PosZ_QLineEdit->setText(QString::fromStdString(dump)); // PosZ
						ifp >> dump >> dump; PhantomSetting_RotX_QLineEdit->setText(QString::fromStdString(dump));// RotX
						ifp >> dump >> dump; PhantomSetting_RotY_QLineEdit->setText(QString::fromStdString(dump)); // RotY
						ifp >> dump >> dump; PhantomSetting_RotZ_QLineEdit->setText(QString::fromStdString(dump)); // RotZ

						slot_PhantomAddingOK_ButtonClicked();

						m_Phantom_SequenceVector.push_back(m_Phantom_MakingIndex);

						// Generate phantom button
						if (PhantomSetting_Name_QLineEdit->text().isEmpty()) PhantomSelect_QPushButton[m_Phantom_MakingIndex] = new QPushButton("Phantom_" + QString::number(m_Phantom_MakingIndex)); // 구조물 이름 입력안했을 때 기본값
						else PhantomSelect_QPushButton[m_Phantom_MakingIndex] = new QPushButton(PhantomSetting_Name_QLineEdit->text()); // 구조물 이름 
						Buttons_FunctionPanelRight.append(PhantomSelect_QPushButton[m_Phantom_MakingIndex]);
						PhantomSelect_QPushButton[m_Phantom_MakingIndex]->setObjectName(QString::number(m_Phantom_MakingIndex));
						PhantomSelect_QPushButton[m_Phantom_MakingIndex]->setFixedSize(Window_width * 0.05, Window_height * 0.040);
						PhantomSelect_QPushButton[m_Phantom_MakingIndex]->setStyleSheet(DialogStyle::MENU_BTN7);
						PhantomSelect_QPushButton[m_Phantom_MakingIndex]->setFont(font_D_BTN14);
						PhantomSelect_QPushButton[m_Phantom_MakingIndex]->setCheckable(true);
						PhantomSelect_QPushButton[m_Phantom_MakingIndex]->setChecked(true);
						connect(PhantomSelect_QPushButton[m_Phantom_MakingIndex], SIGNAL(clicked()), this, SLOT(slot_PhantomSelect_ButtonClicked())); connect(PhantomSelect_QPushButton[m_Phantom_MakingIndex], SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
						PhantomList_QHBoxLayout->addWidget(PhantomSelect_QPushButton[m_Phantom_MakingIndex]);
						for (auto itr : m_Phantom_SequenceVector)
						{
							if (itr != m_Phantom_MakingIndex)
							{
								PhantomSelect_QPushButton[itr]->setChecked(false);
							}
						}
						timer_scroll_multiplePhantom->start(1);
						if (m_Phantom_SequenceVector.size() == const_Phantom_MaximumCount) PhantomAdd_QPushButton->setEnabled(false);

						// Set global variables 
						m_Phantom_SelectedIndex = m_Phantom_MakingIndex;
						m_Phantom_MakingIndex++;

						m_Is_PhantomSetting_Closed = false; // Setting 시 X버튼 눌렀을 때 true
						m_Is_PhantomSetting_OKbutton_Clicked = true; // Setting의 OK 버튼 눌렀을 때 true

						// Set dummy phantom - Dummy on 이면 dummy checkbox 클릭 및 더미로 가시화
						if (PhantomSetting_Dummy_QComboBox->currentIndex() == 0)
						{
							PhantomDummy_QCheckBox->setChecked(true);
							slot_DummyPhantomCheckBoxClicked();
						}
						
						ifp >> dump; // Clothing: 
						//추후 개발
						//int clothing_making_index = 0;
						//while (ifp >> dump)// [clothing_name] or "Eyewear:"
						//{
						//	if (dump == "Eyewear:") break; // End - no clothing
						//	if (dump != "Eyewear:") // [clothing_name]
						//	{
						//		timer_Close_ClothingUIdialog_Recon->start(100);
						//		ClothingAddingSetting();

						//		m_Clothing_ButtonName[m_Phantom_MakingIndex][clothing_making_index] = QString::fromStdString(theApp.ExtractInnerString(dump)); // [clothing_name] 이름 저장
						//		//의복제작//
						//		int layer_making_index = 0;
						//		while (ifp >> dump) // [Layer_X] or "^"
						//		{
						//			if (dump == "^") break; // END signal "^"
						//			if (dump != "^") // [Layer_X]
						//			{
						//				ifp >> dump >> dump; m_Clothing_MainInfo[m_Phantom_MakingIndex][clothing_making_index][layer_making_index][0] = std::stod(dump); // Thickness
						//				ifp >> dump >> dump; m_Clothing_MainInfo[m_Phantom_MakingIndex][clothing_making_index][layer_making_index][1] = std::stod(dump); // Density
						//				ifp >> dump >> dump;
						//				if (dump == "Cloth") m_Clothing_MainInfo[m_Phantom_MakingIndex][clothing_making_index][layer_making_index][2] = 0;
						//				if (dump == "Lead") m_Clothing_MainInfo[m_Phantom_MakingIndex][clothing_making_index][layer_making_index][2] = 1;
						//				if (dump == "Air") m_Clothing_MainInfo[m_Phantom_MakingIndex][clothing_making_index][layer_making_index][2] = 2;
						//				layer_making_index++;
						//				ifp >> dump; // "^" 
						//			}
						//		}

						//		int pre_defined_index = 0;
						//		QVector<QString> clothingNames = { "Invalid", "head", "upper_arm", "lower_arm", "hands", "front_trunk", "back_trunk", "pelvis", "upper_leg", "lower_leg", "feet" };
						//		for (int i = 0; i < clothingNames.size(); ++i)
						//		{
						//			if (clothingNames[i] == m_Clothing_ButtonName[m_Phantom_MakingIndex][clothing_making_index]) {
						//				pre_defined_index = i;
						//			}
						//		}
						//		if (pre_defined_index != 0) // pre-defined
						//		{
						//			theApp.PhantomClothingGenerate(DefinePreWearablePath(pre_defined_index), true);
						//		}
						//		if (pre_defined_index == 0) // user-defined
						//		{

						//		}
						//		clothing_making_index++;
						//	}
						//}

						ifp >> dump; // "Eyewear:"
						ifp >> dump; // "Dosimeters:"
						ifp >> dump; // "^"
					}
				}
			}
		}
		//Geometry_input
		if (dump == "Geometry_input") 
		{
			ifp >> dump;
			if (dump == "===========================")
			{
				ifp >> dump; // Objects:
				while (ifp >> dump) // [object_name] or "Environment:"
				{
					if (dump == "Environment:") break;
					if (dump != "Environment:")
					{
						timer_Close_ObjectUIdialog_Recon->start(100);
						ObjectAddingSetting_Create();
						
						ObjectSetting_Name_QLineEdit->setText(QString::fromStdString(theApp.ExtractInnerString(dump)));						
						ifp >> dump >> dump; // Type: Box or Cylinder or Sphere
						if (dump == "Box")
						{
							ObjectSetting_Type_QComboBox->setCurrentIndex(0);
							ifp >> dump >> dump; ObjectSetting_BoxLengthX_QLineEdit->setText(QString::fromStdString(dump)); // Half_LengthX(cm): X
							ifp >> dump >> dump; ObjectSetting_BoxLengthY_QLineEdit->setText(QString::fromStdString(dump)); // Half_LengthY(cm): X
							ifp >> dump >> dump; ObjectSetting_BoxLengthZ_QLineEdit->setText(QString::fromStdString(dump)); // Half_LengthZ(cm): X
							ifp >> dump >> dump; ObjectSetting_BoxThicknessX_QLineEdit->setText(QString::fromStdString(dump)); // ThicknessX(cm): X
							ifp >> dump >> dump; ObjectSetting_BoxThicknessY_QLineEdit->setText(QString::fromStdString(dump)); // ThicknessY(cm): X
							ifp >> dump >> dump; ObjectSetting_BoxThicknessZ_QLineEdit->setText(QString::fromStdString(dump)); // ThicknessZ(cm): X
							ifp >> dump >> dump; ObjectSetting_BoxCenterX_QLineEdit->setText(QString::fromStdString(dump)); // CenterX(cm): X
							ifp >> dump >> dump; ObjectSetting_BoxCenterY_QLineEdit->setText(QString::fromStdString(dump)); // CenterY(cm): X
							ifp >> dump >> dump; ObjectSetting_BoxCenterZ_QLineEdit->setText(QString::fromStdString(dump)); // CenterZ(cm): X
							ifp >> dump >> dump; ObjectSetting_BoxRotateX_QLineEdit->setText(QString::fromStdString(dump)); // RotateX(deg): X
							ifp >> dump >> dump; ObjectSetting_BoxRotateY_QLineEdit->setText(QString::fromStdString(dump)); // RotateY(deg): X
							ifp >> dump >> dump; ObjectSetting_BoxRotateZ_QLineEdit->setText(QString::fromStdString(dump)); // RotateZ(deg): X
							ifp >> dump >> dump; ObjectSetting_Density_QLineEdit->setText(QString::fromStdString(dump));// Density(g/cm3): X
							ifp >> dump; // Material(mass or atom_fraction);						
							if (dump == "Material(mass_fraction):")  ObjectSetting_MaterialMassFraction_QRadioButton->setChecked(true);
							if (dump == "Material(atom_fraction):")  ObjectSetting_MaterialAtomFraction_QRadioButton->setChecked(true);
							std::string MaterialInfo;
							while (ifp >> dump)
							{
								if (dump == "Charged_particle_transporation:")
								{
									MaterialInfo.pop_back();
									break;
								}
								MaterialInfo += dump + " ";
								ifp >> dump;
								MaterialInfo += dump + '\n';
							}
							ObjectSetting_MaterialPanelTextWindow_UserQDialog->setText(QString::fromStdString(MaterialInfo));
							ifp >> dump; // Charged_particle_transporation: On or Off
							if (dump == "1") m_Object_ChargedParticleTransport[m_Object_MakingIndex] = true;
							if (dump == "0") m_Object_ChargedParticleTransport[m_Object_MakingIndex] = false;
						}
						if (dump == "Sphere")
						{
							ObjectSetting_Type_QComboBox->setCurrentIndex(1);
							ifp >> dump >> dump; ObjectSetting_SphereInnerRadius_QLineEdit->setText(QString::fromStdString(dump)); // Inner_Radius(cm): X
							ifp >> dump >> dump; ObjectSetting_SphereOuterRadius_QLineEdit->setText(QString::fromStdString(dump)); // Outer_Radius(cm): X
							ifp >> dump >> dump; ObjectSetting_SphereStartingAzimuthalPhi_QLineEdit->setText(QString::fromStdString(dump)); // Starting_Azimuthal_Angle(cm): X
							ifp >> dump >> dump; ObjectSetting_SphereDeltaAzimuthalPhi_QLineEdit->setText(QString::fromStdString(dump)); // Delta_Azimuthal_Angle(cm): X
							ifp >> dump >> dump; ObjectSetting_SphereStartingPolarTheta_QLineEdit->setText(QString::fromStdString(dump)); // Starting_Polar_Angle(cm): X
							ifp >> dump >> dump; ObjectSetting_SphereDeltaPolarTheta_QLineEdit->setText(QString::fromStdString(dump)); // Delta_Polar_Angle(cm): X
							ifp >> dump >> dump; ObjectSetting_SphereCenterX_QLineEdit->setText(QString::fromStdString(dump)); // CenterX(cm): X
							ifp >> dump >> dump; ObjectSetting_SphereCenterY_QLineEdit->setText(QString::fromStdString(dump)); // CenterY(cm): X
							ifp >> dump >> dump; ObjectSetting_SphereCenterZ_QLineEdit->setText(QString::fromStdString(dump)); // CenterZ(cm): X
							ifp >> dump >> dump; ObjectSetting_SphereRotateX_QLineEdit->setText(QString::fromStdString(dump)); // RotateX(deg): X
							ifp >> dump >> dump; ObjectSetting_SphereRotateY_QLineEdit->setText(QString::fromStdString(dump)); // RotateY(deg): X
							ifp >> dump >> dump; ObjectSetting_SphereRotateZ_QLineEdit->setText(QString::fromStdString(dump)); // RotateZ(deg): X
							ifp >> dump >> dump; ObjectSetting_Density_QLineEdit->setText(QString::fromStdString(dump));// Density(g/cm3): X
							ifp >> dump; // Material(mass or atom_fraction);						
							if (dump == "Material(mass_fraction):")  ObjectSetting_MaterialMassFraction_QRadioButton->setChecked(true);
							if (dump == "Material(atom_fraction):")  ObjectSetting_MaterialAtomFraction_QRadioButton->setChecked(true);
							std::string MaterialInfo;
							while (ifp >> dump)
							{
								if (dump == "Charged_particle_transporation:")
								{
									MaterialInfo.pop_back();
									break;
								}
								MaterialInfo += dump + " ";
								ifp >> dump;
								MaterialInfo += dump + '\n';
							}
							ObjectSetting_MaterialPanelTextWindow_UserQDialog->setText(QString::fromStdString(MaterialInfo));
							ifp >> dump; // Charged_particle_transporation: On or Off
							if (dump == "1") m_Object_ChargedParticleTransport[m_Object_MakingIndex] = true;
							if (dump == "0") m_Object_ChargedParticleTransport[m_Object_MakingIndex] = false;
						}
						if (dump == "Cylinder")
						{
							ObjectSetting_Type_QComboBox->setCurrentIndex(2);
							ifp >> dump >> dump; ObjectSetting_CylinderInnerRadius_QLineEdit->setText(QString::fromStdString(dump)); // Inner_Radius(cm): X
							ifp >> dump >> dump; ObjectSetting_CylinderOuterRadius_QLineEdit->setText(QString::fromStdString(dump)); // Outer_Radius(cm): X
							ifp >> dump >> dump; ObjectSetting_CylinderHeightZ_QLineEdit->setText(QString::fromStdString(dump)); // Half_HeightZ(cm): X
							ifp >> dump >> dump; ObjectSetting_CylinderStartingAngle_QLineEdit->setText(QString::fromStdString(dump)); // Starting_Angle(cm): X
							ifp >> dump >> dump; ObjectSetting_CylinderDeltaAngle_QLineEdit->setText(QString::fromStdString(dump)); // Delta_Angle(cm): X
							ifp >> dump >> dump; ObjectSetting_CylinderCenterX_QLineEdit->setText(QString::fromStdString(dump)); // CenterX(cm): X
							ifp >> dump >> dump; ObjectSetting_CylinderCenterY_QLineEdit->setText(QString::fromStdString(dump)); // CenterY(cm): X
							ifp >> dump >> dump; ObjectSetting_CylinderCenterZ_QLineEdit->setText(QString::fromStdString(dump)); // CenterZ(cm): X
							ifp >> dump >> dump; ObjectSetting_CylinderRotateX_QLineEdit->setText(QString::fromStdString(dump)); // RotateX(deg): X
							ifp >> dump >> dump; ObjectSetting_CylinderRotateY_QLineEdit->setText(QString::fromStdString(dump)); // RotateY(deg): X
							ifp >> dump >> dump; ObjectSetting_CylinderRotateZ_QLineEdit->setText(QString::fromStdString(dump)); // RotateZ(deg): X
							ifp >> dump >> dump; ObjectSetting_Density_QLineEdit->setText(QString::fromStdString(dump));// Density(g/cm3): X
							ifp >> dump; // Material(mass or atom_fraction);						
							if (dump == "Material(mass_fraction):")  ObjectSetting_MaterialMassFraction_QRadioButton->setChecked(true);
							if (dump == "Material(atom_fraction):")  ObjectSetting_MaterialAtomFraction_QRadioButton->setChecked(true);
							std::string MaterialInfo;
							while (ifp >> dump)
							{
								if (dump == "Charged_particle_transporation:")
								{
									MaterialInfo.pop_back();
									break;
								}
								MaterialInfo += dump + " ";
								ifp >> dump;
								MaterialInfo += dump + '\n';
							}
							ObjectSetting_MaterialPanelTextWindow_UserQDialog->setText(QString::fromStdString(MaterialInfo));
							ifp >> dump; // Charged_particle_transporation: On or Off
							if (dump == "1") m_Object_ChargedParticleTransport[m_Object_MakingIndex] = true;
							if (dump == "0") m_Object_ChargedParticleTransport[m_Object_MakingIndex] = false;
						}

						SaveObject_InfoData_InAddingSetting(m_Object_MakingIndex);
						ObjectSetting_MaterialPanelTextWindow_UserQDialog->setText("");

						// Object panel 버튼 생성
						if (ObjectSetting_Name_QLineEdit->text().isEmpty()) ObjectSelect_QPushButton[m_Object_MakingIndex] = new QPushButton("Object_" + QString::number(m_Object_MakingIndex)); // 구조물 이름 입력안했을 때 기본값
						else ObjectSelect_QPushButton[m_Object_MakingIndex] = new QPushButton(ObjectSetting_Name_QLineEdit->text()); // 구조물 이름 
						Buttons_FunctionPanelRight.append(ObjectSelect_QPushButton[m_Object_MakingIndex]);
						ObjectSelect_QPushButton[m_Object_MakingIndex]->setObjectName(QString::number(m_Object_MakingIndex));
						ObjectSelect_QPushButton[m_Object_MakingIndex]->setFixedSize(Window_width * 0.05, Window_height * 0.040);
						ObjectSelect_QPushButton[m_Object_MakingIndex]->setStyleSheet(DialogStyle::MENU_BTN7);
						ObjectSelect_QPushButton[m_Object_MakingIndex]->setFont(font_D_BTN14);
						ObjectSelect_QPushButton[m_Object_MakingIndex]->setCheckable(true);
						ObjectSelect_QPushButton[m_Object_MakingIndex]->setChecked(true);
						connect(ObjectSelect_QPushButton[m_Object_MakingIndex], SIGNAL(clicked()), this, SLOT(slot_ObjectSelect_ButtonClicked())); connect(ObjectSelect_QPushButton[m_Object_MakingIndex], SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
						ObjectList_QHBoxLayout->addWidget(ObjectSelect_QPushButton[m_Object_MakingIndex]);

						for (auto itr : m_Object_SequenceVector)
						{
							if (itr != m_Object_MakingIndex)
							{
								ObjectSelect_QPushButton[itr]->setChecked(false);
							}
						}
						m_Object_Transparency[m_Object_MakingIndex] = 50;
						// Set object panel
						if (m_ObjectType[m_Object_MakingIndex] == 0) SetBoxObjectMode();
						if (m_ObjectType[m_Object_MakingIndex] == 1) SetSphereObjectMode();
						if (m_ObjectType[m_Object_MakingIndex] == 2) SetCylinderObjectMode();
						UpdateObject_InfoStatus(m_Object_MakingIndex); // Panel 값 업데이트

						// Make object actor
						int id = m_Object_MakingIndex;
						int type = m_ObjectType[id]; // 0:Box, 1:Sphere, 2:Cylinder

						// 1. UI 패널 모드 설정 (이 함수들은 아직 FunctionPanelRight에 있으므로 if문 유지)
						if (type == 0) SetBoxObjectMode();
						else if (type == 1) SetSphereObjectMode();
						else if (type == 2) SetCylinderObjectMode();

						UpdateObject_InfoStatus(id); // Panel 값 업데이트

						// 2. [핵심] Factory를 통해 도형 생성 (theApp.ObjectGenerate_... 대체)
						auto geometry = GeometryFactory::Create(type);
						if (geometry) {
							geometry->Generate(id);
						}

						// 3. 글로벌 변수 업데이트
						m_Object_SequenceVector.push_back(id);
						m_Object_SelectedIndex = id;
						m_Object_MakingIndex++;	
					}		
					ifp >> dump; // "^"
				}

				ifp >> dump; // Enivironment: Vacuum/air/water
				if (dump == "Vacuum") Geometry_EnvironmentType_QComboBox->setCurrentIndex(0);
				if (dump == "Air") Geometry_EnvironmentType_QComboBox->setCurrentIndex(1);
				if (dump == "Water") Geometry_EnvironmentType_QComboBox->setCurrentIndex(2);								
			}
		}
		//Source_input
		if (dump == "Source_input")
		{
			ifp >> dump;
			if (dump == "============================")
			{				
				ifp >> dump >> dump; // Source_geometry: X
				if (dump == "External_point")
				{
					// auto* EPWidget = getSourceWidget<ExternalPointWidget>();
					// EPWidget->ReadSourceInfo(ifp);
					m_comboBoxSourceGeometry->setCurrentIndex(1);
					auto* sourceWidget = getCurrentSourceWidget();
					sourceWidget->ReadSourceInfo(ifp);
					SetSourceGeometryType(m_comboBoxSourceGeometry->currentIndex());
					m_comboBoxSourceGeometry->setDisabled(true);
					m_sourceOKandResetButton->setText("Reset");

					ifp >> dump; // "Calculation_setting_input"
				}
				
				if (dump == "Floor_disk")
				{
					// auto* FDWidget = getSourceWidget<FloorDiskWidget>();
					// FDWidget->ReadSourceInfo(ifp);
					m_comboBoxSourceGeometry->setCurrentIndex(2);
					auto* sourceWidget = getCurrentSourceWidget();
					sourceWidget->ReadSourceInfo(ifp);
					SetSourceGeometryType(m_comboBoxSourceGeometry->currentIndex());
					m_comboBoxSourceGeometry->setDisabled(true);
					m_sourceOKandResetButton->setText("Reset");

					ifp >> dump; // "Calculation_setting_input"
				}

				if (dump == "Object_volume")
				{				
					// auto* OVWidget = getSourceWidget<ObjectVolumeWidget>();
					// OVWidget->ReadSourceInfo(ifp);
					// Initialize selecting source geometry
					m_comboBoxSourceGeometry->setCurrentIndex(3);
					auto* sourceWidget = getCurrentSourceWidget();
					sourceWidget->ReadSourceInfo(ifp);
					SetSourceGeometryType(m_comboBoxSourceGeometry->currentIndex());
					m_comboBoxSourceGeometry->setDisabled(true);
					m_sourceOKandResetButton->setText("Reset");
				}
			}
		}
		//Calculation_setting_input
		if (dump == "Calculation_setting_input")
		{
			ifp >> dump;
			if (dump == "======================")
			{
				ifp >> dump >> dump; // Target_organ       Statistical_error(%)
				while (ifp >> dump)
				{
					if (dump == "Target_NPS:") break;
					if (dump != "Target_NPS:")
					{
						for (int i = 0; i < OrganNameList.size(); i++)
						{
							if (dump == OrganNameList[i].toStdString())
							{
								ifp >> dump;
								m_CalculationSetting_TargetError[i] = QString::fromStdString(dump);
								break;
							}							
						}						
					}
				}
				ifp >> dump;
				if (dump != "None") m_CalculationSetting_TargetNPS = QString::fromStdString(dump); 
				if (dump == "None") m_CalculationSetting_TargetNPS = "";

				ifp >> dump >> dump; 
				if (dump != "None") m_CalculationSetting_ResultFileName = QString::fromStdString(dump); //Result_file_name: X
				if (dump == "None") m_CalculationSetting_ResultFileName = "";

				ifp >> dump >> dump;
				if (dump != "None") m_CalculationSetting_Thread = QString::fromStdString(dump); //Number_of_thread: X
				if (dump == "None") m_CalculationSetting_Thread = "";

				ifp >> dump >> dump;  // Calculation_mode: Local
				if (dump == "Local") SettingVisualizationModeInfo = 0;
				if (dump == "Server") SettingVisualizationModeInfo = 1;

				ifp >> dump; // "========================="
			}
		}
		//Simulation_output
		if (dump == "Simulation_output")
		{
			ifp >> dump;
			if (dump == "==========================")
			{

				ifp >> dump;// =======================================================================
				ifp >> dump >> dump; // Total_Run: 1
				ifp >> dump >> dump >> dump; //  / Number_of_event_processed: 278391164
				ifp >> dump; // =======================================================================
				ifp >> dump >> dump >> dump; // Initialization_time: 102.940057 s
				ifp >> dump >> dump >> dump >> dump;  // / Run_time: 120.029682 s

				// Calculation Start
				b_IsCalculationExecuted = true; // 계산이 수행됨

				SetOutputPanelMode(); // Output 패널로 이동

				theApp.UpdatePhantom_ActorHighlighted(OutputPhantomSelectedIndex); // output 패널에서 선택된 팬텀으로 하이라이트

				// Generate phantom list button
				for (auto itr_phantomID : m_Phantom_SequenceVector)
				{
					OutputPhantomButton[itr_phantomID] = new QPushButton(m_Phantom_ButtonName[itr_phantomID]);
					Buttons_FunctionPanelRight.append(OutputPhantomButton[itr_phantomID]);
					OutputPhantomButton[itr_phantomID]->setObjectName(QString::number(itr_phantomID));
					OutputPhantomButton[itr_phantomID]->setFixedSize(Window_width * 0.05, Window_height * 0.040);
					OutputPhantomButton[itr_phantomID]->setStyleSheet(DialogStyle::MENU_BTN3);
					OutputPhantomButton[itr_phantomID]->setFont(font_D_BTN3);
					OutputPhantomButton[itr_phantomID]->setCheckable(true);
					if (m_Phantom_MainInfo[itr_phantomID][E_PHANTOMMAININFO_DUMMY] == E_PHANTOMDUMMY_YES) OutputPhantomButton[itr_phantomID]->setDisabled(true);

					connect(OutputPhantomButton[itr_phantomID], SIGNAL(clicked()), this, SLOT(slot_OutputPhantomSelect_ButtonClicked())); connect(OutputPhantomButton[itr_phantomID], SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
					layoutOutputPhantomButtonList->addWidget(OutputPhantomButton[itr_phantomID]);
				}
				OutputPhantomButton[OutputPhantomSelectedIndex]->setChecked(true);

				// Create Organ-Dose bar graph - Organ/eff. dose calculation (Real time)
				Create_Dose_BarGraphWidgets(OrganDose_BarGraph_QVBoxLayout);

				// Create Personal dose equivalent bar graph - Personal dose equivalent calculation (Real time)
				Create_Dosimeter_BarGraphWidgets(DosimeterDose_BarGraph_QVBoxLayout);

				// Create H_AK bar graph - Ambient dose equivalent & Air kerma calculation (After completed)
				Create_H_AK_BarGraphWidgets(H_AK_BarGraph_QVBoxLayout); // Bar graph는 선택된 phantom에 무관하게 모든 phantom 및 사용자지정 sphere에 대한 값이 나옴

				// Create HP skin dose depth distribution line graph - HP skin dose depth dstribution calculation (After completed)
				Create_Skindose_LineGraphWidgets(SkinDepthDose_LineGraph_QVBoxLayout); // HP는 단일 팬텀에 대해서만 계산

				theApp.managerCalculation->timer_LocalInit_Stop();		
				// 의복, 피부 layer 생성	
				theApp.phantomObjects->SkinLayerGeneration(); //SKINLAYERGENERATION_skindose

				// 선량결과 업데이트
				std::string tmpStr;
				std::string OrganStr;
				std::string DoseStr;
				std::string errorStr;
				for (auto phantomIdx : m_Phantom_SequenceVector)
				{
					if (m_Phantom_MainInfo[phantomIdx][E_PHANTOMMAININFO_DUMMY] == E_PHANTOMDUMMY_YES) continue;
					while (ifp >> tmpStr)
					{							
						if (tmpStr == "Relative_error")
						{
							// Update time-dose value and calculate progress information factor
							for (int organID = 0; organID < OrganNameList.size(); organID++)
							{
								ifp >> OrganStr; // Organ Name
								ifp >> DoseStr; // Dose
								ifp >> errorStr; // Relative Error	
								m_graphInfo_OrganDose_map[phantomIdx][organID] = (std::make_pair(QString::fromStdString(OrganStr), std::stod(DoseStr)));	
								theApp.OutputDose_data[phantomIdx][organID][0] = QString::fromStdString(OrganStr); // 0 -> organ name
								theApp.OutputDose_data[phantomIdx][organID][1] = QString::fromStdString(theApp.formatToThreeDecimalPlaces(DoseStr)); // 1-> dose
							}
							break;
						}
					}
				}
				// 선량결과 패널값 표시
				for (int i = 0; i < OrganNameList.size(); i++)
				{
					OutputPanel_DoseResult_QLineEdit[theApp.OutputDose_data[OutputPhantomSelectedIndex][i][0]]->setText(theApp.OutputDose_data[OutputPhantomSelectedIndex][i][1]);
				}
				// Organ-dose 그래프 보이기 
				theApp.pRslt->ForcedGraphUpdate_OrganDose();
				theApp.UpdatePhantom_ActorHighlighted(OutputPhantomSelectedIndex);
				theApp.pRt->b_IsOutputResultLoaded = true;

				//// 피부선량분포 가시화
				std::string file_name_SkinDose = "./result/"+ m_CalculationSetting_ResultFileName.toStdString() + "/tmpdata/_UpdatedSkinDoseResult";
				std::ifstream ifs_skinDose(file_name_SkinDose);
				std::stringstream ss_SkinDoseFile;
				ss_SkinDoseFile << ifs_skinDose.rdbuf(); // Read the entire file into the stringstream
				ifs_skinDose.close();
				//theApp.ReadSkinDoseData(ss_SkinDoseFile);
				m_skinDoseVisualizationButton->setEnabled(true);

				// 패널 모드 셋팅
				b_IsRealtimeCalculation = false;
				theApp.pRslt->flg_GraphUpdate = false;
				theApp.pInf->hide();
				theApp.pRslt->hide();
				theApp.m_pVTKWidget->runButton->hide();
				theApp.m_pVTKWidget->stopButton->hide();
				theApp.m_pVTKWidget->resetButton->show();
				m_lineEditResultFileName->setText(m_CalculationSetting_ResultFileName); // Output panel에 띄우기	
			}
		}
	}
	// inputFile->close();
	if (b_IsCalculationExecuted == false) // 결과파일 제외하고 불러왔을 때
	{
		SetGeometryPanelMode();
		Geometry_QPushButton->setChecked(true);
		theApp.UpdatePhantom_ActorHighlighted(const_Phantom_MaximumCount); // 전부 highlight and pickable 끄기
	}
}
void FunctionPanelRight::ClosePhantomUIdialog_Recon()
{
	PhantomSetting_UserQDialog->close();
}
void FunctionPanelRight::CloseClothingUIdialog_Recon()
{
	ClothingSetting_UserQDialog->close();
}
void FunctionPanelRight::CloseObjectUIdialog_Recon()
{
	ObjectAdding_UserQDialog->close();
}

//************************************** Button_slot Process **************************************//
void FunctionPanelRight::slot_allButtonClicked()
{
	QPushButton* clickedButton = qobject_cast<QPushButton*>(sender());

	if (theApp.PanelMode == E_PHANTOM)
	{
		theApp.UpdatePhantom_ActorHighlighted(m_Phantom_SelectedIndex); 
		theApp.UpdateObject_ActorHighlighted(-1);
	}
	if (theApp.PanelMode == E_GEOMETRY)
	{
		theApp.UpdatePhantom_ActorHighlighted(-1);
		theApp.UpdateObject_ActorHighlighted(m_Object_SelectedIndex); // 전부 highlight and pickable 끄기	
	}

	//////////// Phantom Panel /////////////////////	

// == Phantom RAM usage update ==
	int whole_computational_phantom_count = 0;
	int whole_dummy_phantom_count = 0;

	double PhantomBoxXmin = DBL_MAX;
	double PhantomBoxXmax = -DBL_MAX;
	double PhantomBoxYmin = DBL_MAX;
	double PhantomBoxYmax = -DBL_MAX;
	double PhantomBoxZmin = DBL_MAX;
	double PhantomBoxZmax = -DBL_MAX;

	for (auto itr_phantomID : m_Phantom_SequenceVector)
	{
		if (m_Phantom_MainInfo[itr_phantomID][E_PHANTOMMAININFO_DUMMY] == E_PHANTOMDUMMY_YES) whole_dummy_phantom_count++;
		if (m_Phantom_MainInfo[itr_phantomID][E_PHANTOMMAININFO_DUMMY] == E_PHANTOMDUMMY_NO) whole_computational_phantom_count++;

		double* bounds = theApp.PhantomPanelActor[itr_phantomID]->GetBounds();
		if (bounds[0] < PhantomBoxXmin) PhantomBoxXmin = bounds[0];
		if (bounds[1] > PhantomBoxXmax) PhantomBoxXmax = bounds[1];
		if (bounds[2] < PhantomBoxYmin) PhantomBoxYmin = bounds[2];
		if (bounds[3] > PhantomBoxYmax) PhantomBoxYmax = bounds[3];
		if (bounds[4] < PhantomBoxZmin) PhantomBoxZmin = bounds[4];
		if (bounds[5] > PhantomBoxZmax) PhantomBoxZmax = bounds[5];
	}

	maxX_PhantomBox = PhantomBoxXmax;
	maxY_PhantomBox = PhantomBoxYmax;
	maxZ_PhantomBox = PhantomBoxZmax;
	minX_PhantomBox = PhantomBoxXmin;
	minY_PhantomBox = PhantomBoxYmin;
	minZ_PhantomBox = PhantomBoxZmin;

	double xLength = PhantomBoxXmax - PhantomBoxXmin;
	double yLength = PhantomBoxYmax - PhantomBoxYmin;
	double zLength = PhantomBoxZmax - PhantomBoxZmin;

	double box_margin = 5.0; // 5 cm for each end;
	xLength += box_margin;
	yLength += box_margin;
	zLength += box_margin;

	/////// Phantom Panel - Estimate Memory Usage //////

	// Get 'PhantomBox' volume
	double unit_caliber_vol = 1.0E-06; // cm^3 -> m^3
	double unit_caliber_mem = 0.0009765625; // 1 / 1024, MB -> GB
	double PhantomBox_volume = xLength * yLength * zLength * unit_caliber_vol; // Real volume
	// Check if 'empirical threshold'

	double PhantomBox_threshold = 3.330; // EMPIRICAL THRESHOLD
	double memory_usage_one_phantom_empirical = 0.0;//7.8;
	bool LowSpecMode = theApp.isLowSpecMode();

	if (LowSpecMode)
	{
		PhantomBox_threshold = 25.00;
		memory_usage_one_phantom_empirical = 5.50;
	}
	else
	{
		PhantomBox_threshold = 12.00;
		memory_usage_one_phantom_empirical = 6.00;
	}

	if (PhantomBox_volume <= PhantomBox_threshold)
	{
		PhantomBox_volume = PhantomBox_threshold;
	}

	// Calculate memory usage for full phantoms
	double memory_usage_full = 0.0;

	if (LowSpecMode) memory_usage_one_phantom_empirical = 7.26;
	double cor_factor_phantom_type = 1.10;
	double cor_factor_phantom_number = 1.10;

	if (whole_computational_phantom_count == 1) memory_usage_full = memory_usage_one_phantom_empirical;
	if (whole_computational_phantom_count >= 2) memory_usage_full = Volume_to_Memory_full(whole_computational_phantom_count, PhantomBox_volume) * unit_caliber_mem * cor_factor_phantom_type;

	if (whole_computational_phantom_count >= 4) memory_usage_full *= cor_factor_phantom_number;

	// Calculate memory usage for dummy phantoms
	double memory_usage_dummy = 0.0;
	if (whole_dummy_phantom_count >= 1) memory_usage_dummy = Volume_to_Memory_dummy(whole_dummy_phantom_count, PhantomBox_volume) * unit_caliber_mem;

	// sum and return
	double memory_usage = 0.0;
	memory_usage = memory_usage_full + memory_usage_dummy;
	memory_usage = std::round(memory_usage * 10.0) * 0.1; // digit numb

	double memory_space = 1.0;
	memory_space = static_cast<double> (theApp.TotalSystemMemoryGB);
	memory_space = std::round(memory_space * 1000.0) * 0.001; // digit numb

	double memory_occupied = 99.0;
	memory_occupied = memory_usage / memory_space;
	memory_occupied = std::round(memory_occupied * 1000.0) * 0.001; // digit numb
	memory_occupied *= 100.0; // % unit


	double memory_occupied_threshold = 75; // % unit.

	if (memory_occupied > memory_occupied_threshold)
	{
		theApp.setLowMemoryState(true);
	}
	else
	{
		theApp.setLowMemoryState(false);
	}

	////////////////////////////////	
	if (whole_computational_phantom_count == 0) Phantom_RAMusage_QLabel->setText(QString::fromLocal8Bit("0 GB (0 %)"));
	else Phantom_RAMusage_QLabel->setText(QString::number(memory_usage) + QString::fromLocal8Bit(" GB (") + QString::number(memory_occupied) + QString::fromLocal8Bit("%)"));
	
	//////////// Geometry Panel /////////////////////	


	/////////// Output Panel /////////////////////	
	if (m_Phantom_MainInfo[m_Phantom_SelectedIndex][E_PHANTOMMAININFO_DUMMY] == E_PHANTOMDUMMY_YES || m_Phantom_MainInfo[m_Phantom_SelectedIndex][E_PHANTOMMAININFO_TYPE] == E_PHANTOMTYPE_IMPORTED) // Dummy phantom or imported phantom 이면 skin dose 버튼 비활성화
	{
		m_skinDoseVisualizationButton->setDisabled(true);
	}

	// 모든 버튼 다 돌기
	//for (QList<QPushButton*>::iterator it = Buttons_FunctionPanelRight.begin(); it != Buttons_FunctionPanelRight.end(); ++it) 
	//{
	//	if (*it != clickedButton)
	//	{  
	//		(*it)->setEnabled(false);
	//	}
	//}
}

//************************************** Set Mode **************************************//
void FunctionPanelRight::SetGeometryPanelMode()
{
	theApp.PanelMode = E_GEOMETRY;
	theApp.MouseControl_toCamera();
	setFixedWidth(Window_width * 0.209);
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	PhantomPanel_QGroupBox->hide();
	GeometryPanel_QGroupBox->show();
	SourcePanel_QGroupBox->hide();
	OutputPanel_QGroupBox->hide();
	Phantom_QPushButton->setChecked(false);
	Source_QPushButton->setChecked(false);
	Output_QPushButton->setChecked(false);
	theApp.pInf->hide();
	theApp.pRslt->hide();
}
void FunctionPanelRight::SetInitialObjectMode()
{
	ObjectBox_QGroupBox->hide();
	ObjectSphere_QGroupBox->hide();
	ObjectCylinder_QGroupBox->hide();
}
void FunctionPanelRight::SetBoxObjectMode()
{
	ObjectBox_QGroupBox->show();
	ObjectSphere_QGroupBox->hide();
	ObjectCylinder_QGroupBox->hide();	
}
void FunctionPanelRight::SetSphereObjectMode()
{
	ObjectBox_QGroupBox->hide();
	ObjectSphere_QGroupBox->show();
	ObjectCylinder_QGroupBox->hide();
}
void FunctionPanelRight::SetCylinderObjectMode()
{
	ObjectBox_QGroupBox->hide();
	ObjectSphere_QGroupBox->hide();
	ObjectCylinder_QGroupBox->show();	
}
//
void FunctionPanelRight::SetPhantomPanelMode()
{
	theApp.PanelMode = E_PHANTOM;
	theApp.MouseControl_toCamera(); // 다른 패널로 이동할때 camera로
	setFixedWidth(Window_width * 0.209);
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);	
	PhantomPanel_QGroupBox->show();
	GeometryPanel_QGroupBox->hide();
	SourcePanel_QGroupBox->hide();
	OutputPanel_QGroupBox->hide();
	Geometry_QPushButton->setChecked(false);
	Source_QPushButton->setChecked(false);
	Output_QPushButton->setChecked(false);
	theApp.pInf->hide();
	theApp.pRslt->hide();

	if(b_IsCalculationExecuted) m_Phantom_SelectedIndex = OutputPhantomSelectedIndex;
}
//
void FunctionPanelRight::SetSourcePanelMode()
{
	theApp.PanelMode = E_SOURCE;
	theApp.MouseControl_toCamera(); // 다른 패널로 이동할때 camera로
	setFixedWidth(Window_width * 0.209);
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	PhantomPanel_QGroupBox->hide();
	GeometryPanel_QGroupBox->hide();
	SourcePanel_QGroupBox->show();
	OutputPanel_QGroupBox->hide();	
	Phantom_QPushButton->setChecked(false);
	Geometry_QPushButton->setChecked(false);
	Output_QPushButton->setChecked(false);
	theApp.pInf->hide();
	theApp.pRslt->hide();
}
//
void FunctionPanelRight::SetOutputPanelMode()
{
	theApp.PanelMode = E_OUTPUT;
	theApp.MouseControl_toCamera(); // 다른 패널로 이동할때 camera로
	setFixedWidth(Window_width * 0.3);
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	PhantomPanel_QGroupBox->hide();
	GeometryPanel_QGroupBox->hide();
	SourcePanel_QGroupBox->hide();
	OutputPanel_QGroupBox->show();
	Phantom_QPushButton->setChecked(false);
	Geometry_QPushButton->setChecked(false);
	Source_QPushButton->setChecked(false);
	Output_QPushButton->setChecked(true);

	if (m_Phantom_MainInfo[m_Phantom_SelectedIndex][E_PHANTOMMAININFO_DUMMY] == E_PHANTOMDUMMY_YES) // phantom 패널에서 더미팬텀을 선택하고 있을때
	{
		for (auto itr_phantomID : m_Phantom_SequenceVector)
		{
			if (m_Phantom_MainInfo[itr_phantomID][E_PHANTOMMAININFO_DUMMY] == E_PHANTOMDUMMY_NO)
			{
				OutputPhantomSelectedIndex = itr_phantomID;
				break;
			}
		}
	}
	else OutputPhantomSelectedIndex = m_Phantom_SelectedIndex; //팬텀패널에서 풀팬텀 선택하고 있으면 그대로 아웃풋팬텀 인덱스

	if (b_IsRealtimeCalculation)
	{
		theApp.pInf->show();
		theApp.pRslt->show();
	}
	else
	{
		theApp.pInf->hide();
		theApp.pRslt->hide();
	}
}

double FunctionPanelRight::Volume_to_Memory_full(int num_phantom, double volume)
{
	double memory_usage = 0.0;

	if (num_phantom == 1)
	{
		//double memory_usage_2 = memory_usage = 8794.4629 / (volume + 0.298365) + 10738.86;
		//double memory_usage_3 = memory_usage = 13731.5918330256 / (volume + 0.144594843215504) + 16130.3014269161;
		//memory_usage = memory_usage_3 - memory_usage_2;
		memory_usage = 7635;
	}
	if (num_phantom == 2)
	{
		memory_usage = 8794.4629 / (volume + 0.298365) + 10738.86;
	}
	if (num_phantom == 3)
	{
		memory_usage = 13731.5918330256 / (volume + 0.144594843215504) + 16130.3014269161;
	}
	if (num_phantom >= 4)
	{
		double memory_usage_2 = memory_usage = 8794.4629 / (volume + 0.298365) + 10738.86;
		double memory_usage_3 = memory_usage = 13731.5918330256 / (volume + 0.144594843215504) + 16130.3014269161;

		int num_extra_phantom = 3;
		memory_usage = memory_usage_3 + (memory_usage_3 - memory_usage_2) * static_cast<double>(num_phantom - num_extra_phantom);
	}

	return memory_usage;
}

double FunctionPanelRight::Volume_to_Memory_dummy(int num_phantom, double volume)
{
	double memory_usage = 0.0;

	if (num_phantom >= 1) // == 1
	{
		memory_usage += 35.3426 * volume + 230.3512;
	}
	if (num_phantom >= 2) // == 2
	{
		memory_usage += 4.61065544e+03 / (volume + (2.59601429e+00)) + 7.49652765e+01;
	}
	if (num_phantom >= 3) // == 3
	{
		memory_usage += 1.02646212e+03 / (volume + (6.96265010e-01)) + 1.39326085e+02;
	}
	if (num_phantom >= 4) // == 4
	{
		memory_usage += 1.01481411e+03 / (volume + -(4.74861007e-01)) + 1.31503347e+02;
	}
	if (num_phantom > 5) // 
	{
		memory_usage *= static_cast<double>(num_phantom) / 4.0;
	}

	return memory_usage;
}

// 슬롯 구현
void FunctionPanelRight::onShowProgress(QString message)
{
    if (m_progressMsgBox) {
        m_progressMsgBox->setText(message);
        m_progressMsgBox->show();
        
        // [중요] UI가 즉시 갱신되도록 이벤트 강제 처리
        // (단일 스레드 구조에서 무거운 작업 전 UI를 그리기 위해 필수)
        QCoreApplication::processEvents(); 
    }
}

void FunctionPanelRight::onHideProgress()
{
    if (m_progressMsgBox) {
        m_progressMsgBox->hide();
        QCoreApplication::processEvents();
    }
}