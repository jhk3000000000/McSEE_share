#include "PhantomWidget.h"
#include "FunctionPanelRight.h" // 부모 패널의 헤더 필수 (panel 포인터 사용)
#include "ETHuman3DApp.h"       // theApp 접근
#include "GeometryObjects.h"
#include "PhantomObjects.h"     // 팬텀 로직
#include "Util.h"               // 유틸리티

// 생성자
PhantomWidget::PhantomWidget(QWidget* parent)
    : QGroupBox(parent)
{   
    // 타이머 초기화 (기존 코드에 있던 것들)
    timer_scroll_multiplePhantom = new QTimer(this);
    connect(timer_scroll_multiplePhantom, SIGNAL(timeout()), this, SLOT(moveMaximumScrollbar_phantom()));
    
    timer_scroll_clothing = new QTimer(this);
    connect(timer_scroll_clothing, SIGNAL(timeout()), this, SLOT(moveMaximumScrollbar_clothing()));
    
    timer_scroll_clothingLayer = new QTimer(this);
    connect(timer_scroll_clothingLayer, SIGNAL(timeout()), this, SLOT(moveMaximumScrollbar_clothingLayer()));
    
    timer_scroll_dosimeter = new QTimer(this);
    connect(timer_scroll_dosimeter, SIGNAL(timeout()), this, SLOT(moveMaximumScrollbar_dosimeter()));

	// Phantom Setting Dialog Closer
    timer_Close_PhantomUIdialog_Recon = new QTimer(this);
    timer_Close_PhantomUIdialog_Recon->setSingleShot(true); // [중요] 한 번만 실행되도록 설정
    connect(timer_Close_PhantomUIdialog_Recon, SIGNAL(timeout()), this, SLOT(ClosePhantomUIdialog_Recon()));

    // Clothing Setting Dialog Closer
    timer_Close_ClothingUIdialog_Recon = new QTimer(this);
    timer_Close_ClothingUIdialog_Recon->setSingleShot(true); // [중요] 한 번만 실행되도록 설정
    connect(timer_Close_ClothingUIdialog_Recon, SIGNAL(timeout()), this, SLOT(CloseClothingUIdialog_Recon()));
}

PhantomWidget::~PhantomWidget()
{
    // 동적 할당된 리소스 정리 (필요 시)
    if (PhantomSetting_UserQDialog) delete PhantomSetting_UserQDialog;
    if (ClothingSetting_UserQDialog) delete ClothingSetting_UserQDialog;
    // ... 나머지 다이얼로그들
}

// 초기화 함수 (기존 CreatePhantomWidgets의 리팩토링 버전)
bool PhantomWidget::initialize()
{
    // 1. 기본 위젯 스타일 설정
    this->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    this->setStyleSheet(DialogStyle::GROUPBOX_WIDGET);
    this->setFont(panel->font_D_GBW1); // 부모 패널의 폰트 사용
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // 2. 메인 레이아웃 생성
    QVBoxLayout* layout = new QVBoxLayout;
    layout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.008, panel->Window_width * 0.003, 0);
    layout->setSpacing(panel->Window_width * 0.003);
    layout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    this->setLayout(layout);

    	{
		// Calculate physicalRAM in Windows PC
		MEMORYSTATUSEX statex;
		statex.dwLength = sizeof(statex);
		GlobalMemoryStatusEx(&statex);
		double totalPhysMemGB = (double)statex.ullTotalPhys / (1024 * 1024 * 1024);
		m_model.m_Phantom_RAM_Usage = static_cast<int>(std::ceil(totalPhysMemGB));

		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(panel->Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(0, panel->Window_height * 0.001, panel->Window_width * 0.005, panel->Window_height * 0.001);
		
		QLabel* labelRAMtitle = new QLabel;
		labelRAMtitle->setFixedSize(panel->Window_width * 0.115, panel->Window_height * 0.018);
		labelRAMtitle->setStyleSheet(DialogStyle::DATA_LABEL);
		labelRAMtitle->setFont(panel->font_D_LBL1);
		labelRAMtitle->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		labelRAMtitle->setText(QString::fromLocal8Bit("Memory Usage: "));

		Phantom_RAMusage_QLabel = new QLabel;
		Phantom_RAMusage_QLabel->setFixedSize(panel->Window_width * 0.085, panel->Window_height * 0.018);
		Phantom_RAMusage_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		Phantom_RAMusage_QLabel->setFont(panel->font_D_LBL1);
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
		hLayout->setSpacing(panel->Window_width * 0.005); // 지금 vertical line에서 생성된 버튼들 사이의 horizontal spacing
		hLayout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		hLayout->setContentsMargins(panel->Window_width * 0.005, panel->Window_height * 0.005, panel->Window_width * 0.005, panel->Window_height * 0.005); // 최초 버튼 생성하는 마진
		
		// 버튼생성
		PhantomAdd_QPushButton = new QPushButton(QString::fromLocal8Bit("Add"));
		panel->Buttons_FunctionPanelRight.append(PhantomAdd_QPushButton);
		PhantomAdd_QPushButton->setFixedSize(panel->Window_width * 0.030, panel->Window_height * 0.025);
		PhantomAdd_QPushButton->setCheckable(false);
		PhantomAdd_QPushButton->setStyleSheet(DialogStyle::MENU_BTN3);
		PhantomAdd_QPushButton->setFont(panel->font_D_BTN3);

		// 버튼생성
		PhantomDelete_QPushButton = new QPushButton(QString::fromLocal8Bit("Delete"));
		panel->Buttons_FunctionPanelRight.append(PhantomDelete_QPushButton);
		PhantomDelete_QPushButton->setFixedSize(panel->Window_width * 0.030, panel->Window_height * 0.025);
		PhantomDelete_QPushButton->setCheckable(false);
		PhantomDelete_QPushButton->setStyleSheet(DialogStyle::MENU_BTN3);
		PhantomDelete_QPushButton->setFont(panel->font_D_BTN3);
		
		// 버튼생성
		PhantomMoveLeft_UserQPushButton = new LeftButton(this);
		panel->Buttons_FunctionPanelRight.append(PhantomMoveLeft_UserQPushButton);
		PhantomMoveLeft_UserQPushButton->setFixedSize(panel->Window_width * 0.012, panel->Window_height * 0.025);
		// Apply stylesheet for color and shape
		PhantomMoveLeft_UserQPushButton->setStyleSheet("QPushButton {"
			"background-color: white;"
			"border: 0px;" // Half of the width/height for circular shape
			"}");

		// 버튼생성
		PhantomMoveRight_UserQPushButton = new RightButton(this);
		panel->Buttons_FunctionPanelRight.append(PhantomMoveRight_UserQPushButton);
		PhantomMoveRight_UserQPushButton->setFixedSize(panel->Window_width * 0.012, panel->Window_height * 0.025);
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
	GroupBoxIn->setFont(panel->font_D_GBW1);
	GroupBoxIn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	QGroupBox* GroupBoxOut = new QGroupBox;
	GroupBoxOut->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	GroupBoxOut->setStyleSheet(DialogStyle::GROUPBOX_WIDGET);
	GroupBoxOut->setFont(panel->font_D_GBW1);
	GroupBoxOut->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	QVBoxLayout* layoutIn = new QVBoxLayout();
	layoutIn->setContentsMargins(panel->Window_width * 0.003, 0, panel->Window_width * 0.003, 0);
	layoutIn->setSpacing(panel->Window_width * 0.003);
	layoutIn->setAlignment(Qt::AlignLeft | Qt::AlignTop);

	QVBoxLayout* layoutOut = new QVBoxLayout();
	layoutOut->setContentsMargins(panel->Window_width * 0.003, 0, panel->Window_width * 0.003, 0);
	layoutOut->setSpacing(panel->Window_width * 0.003);
	layoutOut->setAlignment(Qt::AlignLeft | Qt::AlignTop);

	QScrollArea* scrollArea = new QScrollArea();
	scrollArea->setStyleSheet(DialogStyle::GROUPBOX_WIDGET);
	scrollArea->setFont(panel->font_D_GBW1);
	scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	PhantomList_Vertical_QScrollBar = scrollArea->verticalScrollBar();

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(panel->Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(0, panel->Window_height * 0.005, 0, 0);

		QLabel* labelType = new QLabel;
		labelType->setFixedSize(panel->Window_width * 0.022, panel->Window_height * 0.028);
		labelType->setStyleSheet(DialogStyle::DATA_LABEL);
		labelType->setFont(panel->font_D_LBL5);
		labelType->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		labelType->setText(QString::fromLocal8Bit("Type:"));

		PhantomType_QLineEdit = new QLineEdit;
		PhantomType_QLineEdit->setFixedSize(panel->Window_width * 0.084, panel->Window_height * 0.028);
		PhantomType_QLineEdit->setAlignment(Qt::AlignCenter);
		PhantomType_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		PhantomType_QLineEdit->setFont(panel->font_D_LE4);
		PhantomType_QLineEdit->setText("");
		PhantomType_QLineEdit->setReadOnly(true);		

		QLabel* labelSpacer = new QLabel;
		labelSpacer->setFixedSize(panel->Window_width * 0.01, panel->Window_height * 0.028);
		labelSpacer->setStyleSheet(DialogStyle::DATA_LABEL);
		labelSpacer->setFont(panel->font_D_LBL5);
		labelSpacer->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		labelSpacer->setText(QString::fromLocal8Bit(""));

		PhantomDummy_QCheckBox = new QCheckBox;
		PhantomDummy_QCheckBox->setEnabled(true);
		PhantomDummy_QCheckBox->setChecked(false);
		PhantomDummy_QCheckBox->setFixedSize(panel->Window_width * 0.006, panel->Window_height * 0.02);
		PhantomDummy_QCheckBox->setStyleSheet("QCheckBox { background-color: transparent; }");

		QLabel* labelDummy = new QLabel;
		labelDummy->setFixedSize(panel->Window_width * 0.03, panel->Window_height * 0.028);
		labelDummy->setStyleSheet(DialogStyle::DATA_LABEL);
		labelDummy->setFont(panel->font_D_LBL4);
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
		hLayout->setSpacing(panel->Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(0, panel->Window_height * 0.005, 0, 0);

		QLabel* labelGender = new QLabel;
		labelGender->setFixedSize(panel->Window_width * 0.028, panel->Window_height * 0.028);
		labelGender->setStyleSheet(DialogStyle::DATA_LABEL);
		labelGender->setFont(panel->font_D_LBL5);
		labelGender->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		labelGender->setText(QString::fromLocal8Bit("Gender:"));

		PhantomGender_QLineEdit = new QLineEdit;
		PhantomGender_QLineEdit->setFixedSize(panel->Window_width * 0.048, panel->Window_height * 0.028);
		PhantomGender_QLineEdit->setAlignment(Qt::AlignCenter);
		PhantomGender_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		PhantomGender_QLineEdit->setFont(panel->font_D_LE4);
		PhantomGender_QLineEdit->setText("");
		PhantomGender_QLineEdit->setReadOnly(true);

		QLabel* labelSpacer = new QLabel;
		labelSpacer->setFixedSize(panel->Window_width * 0.01, panel->Window_height * 0.028);
		labelSpacer->setStyleSheet(DialogStyle::DATA_LABEL);
		labelSpacer->setFont(panel->font_D_LBL5);
		labelSpacer->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		labelSpacer->setText(QString::fromLocal8Bit(""));

		QLabel* labelAge = new QLabel;
		labelAge->setFixedSize(panel->Window_width * 0.02, panel->Window_height * 0.028);
		labelAge->setStyleSheet(DialogStyle::DATA_LABEL);
		labelAge->setFont(panel->font_D_LBL5);
		labelAge->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		labelAge->setText(QString::fromLocal8Bit("Age:"));

		PhantomAge_QLineEdit = new QLineEdit;
		PhantomAge_QLineEdit->setFixedSize(panel->Window_width * 0.048, panel->Window_height * 0.028);
		PhantomAge_QLineEdit->setAlignment(Qt::AlignCenter);
		PhantomAge_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		PhantomAge_QLineEdit->setFont(panel->font_D_LE4);
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
		hLayout->setSpacing(panel->Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(0, panel->Window_height * 0.005, 0, 0);

		QLabel* labelPosture = new QLabel;
		labelPosture->setFixedSize(panel->Window_width * 0.028, panel->Window_height * 0.028);
		labelPosture->setStyleSheet(DialogStyle::DATA_LABEL);
		labelPosture->setFont(panel->font_D_LBL5);
		labelPosture->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		labelPosture->setText(QString::fromLocal8Bit("Posture:"));
		
		PhantomPosture_QLineEdit = new QLineEdit;
		PhantomPosture_QLineEdit->setFixedSize(panel->Window_width * 0.048, panel->Window_height * 0.028);
		PhantomPosture_QLineEdit->setAlignment(Qt::AlignCenter);
		PhantomPosture_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		PhantomPosture_QLineEdit->setFont(panel->font_D_LE4);
		PhantomPosture_QLineEdit->setText("");
		PhantomPosture_QLineEdit->setReadOnly(true);
				
		QLabel* labelSpacer = new QLabel;
		labelSpacer->setFixedSize(panel->Window_width * 0.0001, panel->Window_height * 0.028);
		labelSpacer->setStyleSheet(DialogStyle::DATA_LABEL);
		labelSpacer->setFont(panel->font_D_LBL5);
		labelSpacer->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		labelSpacer->setText(QString::fromLocal8Bit(""));

		QLabel* labelFetalAge = new QLabel;
		labelFetalAge->setFixedSize(panel->Window_width * 0.03, panel->Window_height * 0.028);
		labelFetalAge->setStyleSheet(DialogStyle::DATA_LABEL);
		labelFetalAge->setFont(panel->font_D_LBL5);
		labelFetalAge->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		labelFetalAge->setText(QString::fromLocal8Bit("Fetal Age:"));

		PhantomFetalAge_QLineEdit = new QLineEdit;
		PhantomFetalAge_QLineEdit->setFixedSize(panel->Window_width * 0.048, panel->Window_height * 0.028);
		PhantomFetalAge_QLineEdit->setAlignment(Qt::AlignCenter);
		PhantomFetalAge_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		PhantomFetalAge_QLineEdit->setFont(panel->font_D_LE4);
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
		hLayout->setSpacing(panel->Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(0, panel->Window_height * 0.005, 0, 0);

		QLabel* labelHeight = new QLabel;
		labelHeight->setFixedSize(panel->Window_width * 0.028, panel->Window_height * 0.028);
		labelHeight->setStyleSheet(DialogStyle::DATA_LABEL);
		labelHeight->setFont(panel->font_D_LBL5);
		labelHeight->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		labelHeight->setText(QString::fromLocal8Bit("Height:"));

		PhantomHeight_QLineEdit = new QLineEdit;
		PhantomHeight_QLineEdit->setFixedSize(panel->Window_width * 0.048, panel->Window_height * 0.028);
		PhantomHeight_QLineEdit->setAlignment(Qt::AlignCenter);
		PhantomHeight_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		PhantomHeight_QLineEdit->setFont(panel->font_D_LE4);
		PhantomHeight_QLineEdit->setText("");
		PhantomHeight_QLineEdit->setReadOnly(true);

		QLabel* labelSpacer = new QLabel;
		labelSpacer->setFixedSize(panel->Window_width * 0.002, panel->Window_height * 0.028);
		labelSpacer->setStyleSheet(DialogStyle::DATA_LABEL);
		labelSpacer->setFont(panel->font_D_LBL5);
		labelSpacer->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		labelSpacer->setText(QString::fromLocal8Bit(""));

		QLabel* labelWeight = new QLabel;
		labelWeight->setFixedSize(panel->Window_width * 0.028, panel->Window_height * 0.028);
		labelWeight->setStyleSheet(DialogStyle::DATA_LABEL);
		labelWeight->setFont(panel->font_D_LBL5);
		labelWeight->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		labelWeight->setText(QString::fromLocal8Bit("Weight:"));
		
		PhantomWeight_QLineEdit = new QLineEdit;				
		PhantomWeight_QLineEdit->setFixedSize(panel->Window_width * 0.048, panel->Window_height * 0.028);
		PhantomWeight_QLineEdit->setAlignment(Qt::AlignCenter);
		PhantomWeight_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		PhantomWeight_QLineEdit->setFont(panel->font_D_LE4);
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
		hLayout->setSpacing(panel->Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(0, panel->Window_height * 0.005, 0, 0);

		QLabel* labelPhantomPosRot = new QLabel;
		labelPhantomPosRot->setFixedSize(panel->Window_width * 0.104, panel->Window_height * 0.028);
		labelPhantomPosRot->setStyleSheet(DialogStyle::DATA_LABEL);
		labelPhantomPosRot->setFont(panel->font_D_LBL1);
		labelPhantomPosRot->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		labelPhantomPosRot->setText(QString::fromLocal8Bit("Phantom Pos/Rot"));

		hLayout->addWidget(labelPhantomPosRot);

		layoutIn->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(panel->Window_width * 0.002);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(panel->Window_width * 0.015, panel->Window_height * 0.005, 0, 0);

		QLabel* labelPhantomPosX = new QLabel;
		labelPhantomPosX->setFixedSize(panel->Window_width * 0.0478, panel->Window_height * 0.028);
		labelPhantomPosX->setStyleSheet(DialogStyle::DATA_LABEL);
		labelPhantomPosX->setFont(panel->font_D_LBL1);
		labelPhantomPosX->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		labelPhantomPosX->setText(QString::fromLocal8Bit("Pos. XYZ:"));

		PhantomPosX_QLineEdit = new QLineEdit;
		PhantomPosX_QLineEdit->setFixedSize(panel->Window_width * 0.035, panel->Window_height * 0.028);
		PhantomPosX_QLineEdit->setAlignment(Qt::AlignCenter);
		PhantomPosX_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME4);
		PhantomPosX_QLineEdit->setFont(panel->font_D_LE4);
		PhantomPosX_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(cm)"));

		PhantomPosY_QLineEdit = new QLineEdit;
		PhantomPosY_QLineEdit->setFixedSize(panel->Window_width * 0.035, panel->Window_height * 0.028);
		PhantomPosY_QLineEdit->setAlignment(Qt::AlignCenter);
		PhantomPosY_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME4);
		PhantomPosY_QLineEdit->setFont(panel->font_D_LE4);
		PhantomPosY_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(cm)"));

		PhantomPosZ_QLineEdit = new QLineEdit;
		PhantomPosZ_QLineEdit->setFixedSize(panel->Window_width * 0.035, panel->Window_height * 0.028);
		PhantomPosZ_QLineEdit->setAlignment(Qt::AlignCenter);
		PhantomPosZ_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME4);
		PhantomPosZ_QLineEdit->setFont(panel->font_D_LE4);
		PhantomPosZ_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(cm)"));

		hLayout->addWidget(labelPhantomPosX);
		hLayout->addWidget(PhantomPosX_QLineEdit);
		hLayout->addWidget(PhantomPosY_QLineEdit);
		hLayout->addWidget(PhantomPosZ_QLineEdit);

		layoutIn->addLayout(hLayout);
	}
	
	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(panel->Window_width * 0.002);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(panel->Window_width * 0.015, panel->Window_height * 0.005, 0, 0);

		QLabel* labelPhantomRotateX = new QLabel;
		labelPhantomRotateX->setFixedSize(panel->Window_width * 0.0478, panel->Window_height * 0.028);
		labelPhantomRotateX->setStyleSheet(DialogStyle::DATA_LABEL);
		labelPhantomRotateX->setFont(panel->font_D_LBL1);
		labelPhantomRotateX->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		labelPhantomRotateX->setText(QString::fromLocal8Bit("Rot. XYZ:"));

		PhantomRotX_QLineEdit = new QLineEdit;
		PhantomRotX_QLineEdit->setFixedSize(panel->Window_width * 0.035, panel->Window_height * 0.028);
		PhantomRotX_QLineEdit->setAlignment(Qt::AlignCenter);
		PhantomRotX_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME4);
		PhantomRotX_QLineEdit->setFont(panel->font_D_LE4);
		PhantomRotX_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(deg)"));

		PhantomRotY_QLineEdit = new QLineEdit;
		PhantomRotY_QLineEdit->setFixedSize(panel->Window_width * 0.035, panel->Window_height * 0.028);
		PhantomRotY_QLineEdit->setAlignment(Qt::AlignCenter);
		PhantomRotY_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME4);
		PhantomRotY_QLineEdit->setFont(panel->font_D_LE4);
		PhantomRotY_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(deg)"));

		PhantomRotZ_QLineEdit = new QLineEdit;
		PhantomRotZ_QLineEdit->setFixedSize(panel->Window_width * 0.035, panel->Window_height * 0.028);
		PhantomRotZ_QLineEdit->setAlignment(Qt::AlignCenter);
		PhantomRotZ_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME4);
		PhantomRotZ_QLineEdit->setFont(panel->font_D_LE4);
		PhantomRotZ_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(deg)"));

		hLayout->addWidget(labelPhantomRotateX);
		hLayout->addWidget(PhantomRotX_QLineEdit);
		hLayout->addWidget(PhantomRotY_QLineEdit);
		hLayout->addWidget(PhantomRotZ_QLineEdit);
		
		layoutIn->addLayout(hLayout);
	}
	   

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(panel->Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		hLayout->setContentsMargins(0, panel->Window_height * 0.009, panel->Window_width * 0.007, panel->Window_height * 0.009);

		PhantomUpdate_QPushButton = new QPushButton(QString::fromLocal8Bit("Update"));		
		panel->Buttons_FunctionPanelRight.append(PhantomUpdate_QPushButton);
		PhantomUpdate_QPushButton->setFixedSize(panel->Window_width * 0.042, panel->Window_height * 0.037);
		PhantomUpdate_QPushButton->setCheckable(false);
		PhantomUpdate_QPushButton->setStyleSheet(DialogStyle::MENU_BTN2);
		PhantomUpdate_QPushButton->setFont(panel->font_D_BTN2);
		PhantomUpdate_QPushButton->setChecked(false);

		connect(PhantomUpdate_QPushButton, SIGNAL(clicked()), this, SLOT(slot_PhantomUpdate_ButtonClicked())); connect(PhantomUpdate_QPushButton, SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));

		hLayout->addWidget(PhantomUpdate_QPushButton);

		layoutIn->addLayout(hLayout);
	}

	////////////// Clothing panel ////////////////
	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(panel->Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(0, panel->Window_height * 0.005, 0, 0);

		ClothingTitle_QLabel = new QLabel;
		ClothingTitle_QLabel->setFixedSize(panel->Window_width * 0.104, panel->Window_height * 0.028);
		ClothingTitle_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		ClothingTitle_QLabel->setFont(panel->font_D_LBL1);
		ClothingTitle_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		ClothingTitle_QLabel->setText(QString::fromLocal8Bit("Clothing"));

		hLayout->addWidget(ClothingTitle_QLabel);

		layoutIn->addLayout(hLayout);
	}
	{
		QGroupBox* GroupBoxClothingList = new QGroupBox;
		GroupBoxClothingList->setAlignment(Qt::AlignCenter | Qt::AlignLeft);
		GroupBoxClothingList->setStyleSheet(DialogStyle::GROUPBOX_WIDGET10);
		GroupBoxClothingList->setFont(panel->font_D_GBW10);
		GroupBoxClothingList->setFixedHeight(panel->Window_height * 0.060);
		GroupBoxClothingList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed); // 가로, 세로 방향 크기
		GroupBoxClothingList->setContentsMargins(panel->Window_width * 0.001, 0, panel->Window_width * 0.001, 0);

		layoutIn->addWidget(GroupBoxClothingList);

		QVBoxLayout* layoutOut = new QVBoxLayout();
		layoutOut->setContentsMargins(panel->Window_width * 0.001, 0, panel->Window_width * 0.005, panel->Window_height * 0.005);
		layoutOut->setSpacing(panel->Window_width * 0.005);
		layoutOut->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QGroupBox* GroupBoxIn = new QGroupBox;
		GroupBoxIn->setAlignment(Qt::AlignCenter | Qt::AlignLeft);
		GroupBoxIn->setStyleSheet(DialogStyle::GROUPBOX_WIDGET12);
		GroupBoxIn->setFont(panel->font_D_GBW10);
		GroupBoxIn->setFixedHeight(0.060 * panel->Window_height);
		GroupBoxIn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

		ClothingList_QHBoxLayout = new QHBoxLayout();
		ClothingList_QHBoxLayout->setContentsMargins(panel->Window_width * 0.001, panel->Window_height * 0.005, panel->Window_width * 0.001, panel->Window_height * 0.005); // 왼쪽, 위, 오른쪽, 아래의 여유공간(해당 layout 안에서의)
		ClothingList_QHBoxLayout->setSpacing(panel->Window_width * 0.003);
		ClothingList_QHBoxLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		MultipleButtonScorllArea* scrollArea = new MultipleButtonScorllArea();
		scrollArea->setStyleSheet(DialogStyle::GROUPBOX_WIDGET10);
		scrollArea->setFont(panel->font_D_GBW10);
		scrollArea->setFixedHeight(panel->Window_height * 0.060);
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
		hLayout->setSpacing(panel->Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		hLayout->setContentsMargins(0, panel->Window_height * 0.005, panel->Window_width * 0.010, 0);

		hLayout->addStretch();

		ClothingAdd_QPushButton = new QPushButton(QString::fromLocal8Bit("Add"));
		panel->Buttons_FunctionPanelRight.append(ClothingAdd_QPushButton);
		ClothingAdd_QPushButton->setFixedSize(panel->Window_width * 0.030, panel->Window_height * 0.025);
		ClothingAdd_QPushButton->setStyleSheet(DialogStyle::MENU_BTN2);
		ClothingAdd_QPushButton->setFont(panel->font_D_BTN2);
		ClothingAdd_QPushButton->setCheckable(false);

		ClothingDelete_QPushButton = new QPushButton(QString::fromLocal8Bit("Delete"));
		panel->Buttons_FunctionPanelRight.append(ClothingDelete_QPushButton);
		ClothingDelete_QPushButton->setFixedSize(panel->Window_width * 0.030, panel->Window_height * 0.025);
		ClothingDelete_QPushButton->setStyleSheet(DialogStyle::MENU_BTN2);
		ClothingDelete_QPushButton->setFont(panel->font_D_BTN2);
		ClothingDelete_QPushButton->setCheckable(false);

		connect(ClothingAdd_QPushButton, SIGNAL(clicked()), this, SLOT(slot_ClothingAdd_ButtonClicked())); connect(ClothingAdd_QPushButton, SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
		connect(ClothingDelete_QPushButton, SIGNAL(clicked()), this, SLOT(slot_ClothingDelete_ButtonClicked())); connect(ClothingDelete_QPushButton, SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));

		hLayout->addWidget(ClothingAdd_QPushButton);
		hLayout->addWidget(ClothingDelete_QPushButton);

		layoutIn->addLayout(hLayout);
	}
	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(panel->Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(panel->Window_width * 0.025, panel->Window_height * 0.005, 0, 0);

		ClothingLayerTitle_QLabel = new QLabel;
		ClothingLayerTitle_QLabel->setFixedSize(panel->Window_width * 0.104, panel->Window_height * 0.028);
		ClothingLayerTitle_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		ClothingLayerTitle_QLabel->setFont(panel->font_D_LBL1);
		ClothingLayerTitle_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		ClothingLayerTitle_QLabel->setText(QString::fromLocal8Bit("Clothing Layer"));

		hLayout->addWidget(ClothingLayerTitle_QLabel);

		layoutIn->addLayout(hLayout);
	}
	{
		QGroupBox* GroupBoxClothingLayerList = new QGroupBox;
		GroupBoxClothingLayerList->setAlignment(Qt::AlignCenter | Qt::AlignLeft);
		GroupBoxClothingLayerList->setStyleSheet(DialogStyle::GROUPBOX_WIDGET10);
		GroupBoxClothingLayerList->setFont(panel->font_D_GBW10);
		GroupBoxClothingLayerList->setFixedHeight(0.060 * panel->Window_height);
		GroupBoxClothingLayerList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed); // 가로, 세로 방향 크기
		GroupBoxClothingLayerList->setContentsMargins(panel->Window_width * 0.001, 0, panel->Window_width * 0.001, 0);

		layoutIn->addWidget(GroupBoxClothingLayerList);

		QVBoxLayout* layoutOut = new QVBoxLayout();
		layoutOut->setContentsMargins(panel->Window_width * 0.02, 0, panel->Window_width * 0.005, panel->Window_height * 0.005); // List groupbox의 좌상우하 여백
		layoutOut->setSpacing(panel->Window_width * 0.005);
		layoutOut->setAlignment(Qt::AlignRight | Qt::AlignTop);

		QGroupBox* GroupBoxIn = new QGroupBox;
		GroupBoxIn->setAlignment(Qt::AlignCenter | Qt::AlignLeft);
		GroupBoxIn->setStyleSheet(DialogStyle::GROUPBOX_WIDGET12);
		GroupBoxIn->setFont(panel->font_D_GBW10);
		GroupBoxIn->setFixedHeight(0.060 * panel->Window_height);
		GroupBoxIn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

		ClothingLayerList_QHBoxLayout = new QHBoxLayout();
		ClothingLayerList_QHBoxLayout->setContentsMargins(panel->Window_width * 0.001, panel->Window_height * 0.005, panel->Window_width * 0.001, panel->Window_height * 0.005); // 왼쪽, 위, 오른쪽, 아래의 여유공간(해당 layout 안에서의)
		ClothingLayerList_QHBoxLayout->setSpacing(panel->Window_width * 0.003);
		ClothingLayerList_QHBoxLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		MultipleButtonScorllArea* scrollArea = new MultipleButtonScorllArea();
		scrollArea->setStyleSheet(DialogStyle::GROUPBOX_WIDGET10);
		scrollArea->setFont(panel->font_D_GBW10);
		scrollArea->setFixedHeight(panel->Window_height * 0.060);
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
		hLayout->setSpacing(panel->Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		hLayout->setContentsMargins(0, panel->Window_height * 0.005, panel->Window_width * 0.010, panel->Window_height * 0.005);

		hLayout->addStretch();

		ClothingLayerAdd_QPushButton = new QPushButton(QString::fromLocal8Bit("Add"));
		panel->Buttons_FunctionPanelRight.append(ClothingLayerAdd_QPushButton);
		ClothingLayerAdd_QPushButton->setFixedSize(panel->Window_width * 0.030, panel->Window_height * 0.025);
		ClothingLayerAdd_QPushButton->setStyleSheet(DialogStyle::MENU_BTN2);
		ClothingLayerAdd_QPushButton->setFont(panel->font_D_BTN2);
		ClothingLayerAdd_QPushButton->setCheckable(false);

		ClothingLayerDelete_QPushButton = new QPushButton(QString::fromLocal8Bit("Delete"));
		panel->Buttons_FunctionPanelRight.append(ClothingLayerDelete_QPushButton);
		ClothingLayerDelete_QPushButton->setFixedSize(panel->Window_width * 0.030, panel->Window_height * 0.025);
		ClothingLayerDelete_QPushButton->setStyleSheet(DialogStyle::MENU_BTN2);
		ClothingLayerDelete_QPushButton->setFont(panel->font_D_BTN2);
		ClothingLayerDelete_QPushButton->setCheckable(false);

		connect(ClothingLayerAdd_QPushButton, SIGNAL(clicked()), this, SLOT(slot_ClothingLayerAdd_ButtonClicked())); connect(ClothingLayerAdd_QPushButton, SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
		connect(ClothingLayerDelete_QPushButton, SIGNAL(clicked()), this, SLOT(slot_ClothingLayerDelete_ButtonClicked())); connect(ClothingLayerDelete_QPushButton, SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));

		hLayout->addWidget(ClothingLayerAdd_QPushButton);
		hLayout->addWidget(ClothingLayerDelete_QPushButton);

		layoutIn->addLayout(hLayout);
	}
	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(panel->Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(panel->Window_width * 0.018, panel->Window_height * 0.005, 0, panel->Window_height * 0.005);

		QLabel* labelThickness = new QLabel;
		labelThickness->setFixedSize(panel->Window_width * 0.05, panel->Window_height * 0.028);
		labelThickness->setStyleSheet(DialogStyle::DATA_LABEL);
		labelThickness->setFont(panel->font_D_LBL1);
		labelThickness->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		labelThickness->setText(QString::fromLocal8Bit("Thickness:"));

		ClothingThickness_QLineEdit = new QLineEdit;
		ClothingThickness_QLineEdit->setFixedSize(panel->Window_width * 0.094, panel->Window_height * 0.028);
		ClothingThickness_QLineEdit->setAlignment(Qt::AlignCenter);
		ClothingThickness_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		ClothingThickness_QLineEdit->setFont(panel->font_D_LE1);
		ClothingThickness_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("cm"));

		hLayout->addWidget(labelThickness);
		hLayout->addWidget(ClothingThickness_QLineEdit);

		layoutIn->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(panel->Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(panel->Window_width * 0.018, panel->Window_height * 0.005, 0, panel->Window_height * 0.005);

		QLabel* labelDensity = new QLabel;
		labelDensity->setFixedSize(panel->Window_width * 0.05, panel->Window_height * 0.028);
		labelDensity->setStyleSheet(DialogStyle::DATA_LABEL);
		labelDensity->setFont(panel->font_D_LBL1);
		labelDensity->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		labelDensity->setText(QString::fromLocal8Bit("Density:"));

		ClothingDensity_QLineEdit = new QLineEdit;
		ClothingDensity_QLineEdit->setFixedSize(panel->Window_width * 0.094, panel->Window_height * 0.028);
		ClothingDensity_QLineEdit->setAlignment(Qt::AlignCenter);
		ClothingDensity_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		ClothingDensity_QLineEdit->setFont(panel->font_D_LE1);
		ClothingDensity_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("g/cm3"));

		hLayout->addWidget(labelDensity);
		hLayout->addWidget(ClothingDensity_QLineEdit);

		layoutIn->addLayout(hLayout);
	}
	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(panel->Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(panel->Window_width * 0.018, panel->Window_height * 0.005, 0, 0);

		QLabel* labelLayer = new QLabel;
		labelLayer->setFixedSize(panel->Window_width * 0.05, panel->Window_height * 0.028);
		labelLayer->setStyleSheet(DialogStyle::DATA_LABEL);
		labelLayer->setFont(panel->font_D_LBL1);
		labelLayer->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		labelLayer->setText(QString::fromLocal8Bit("Composition:"));

		ClothingComposition_QComboBox = new QComboBox;
		ClothingComposition_QComboBox->setFixedSize(panel->Window_width * 0.095, panel->Window_height * 0.028);
		ClothingComposition_QComboBox->setStyleSheet(DialogStyle::COMBOBOX);
		ClothingComposition_QComboBox->setFont(panel->font_D_CB1);

		ClothingComposition_QComboBox->setEditable(true);
		ClothingComposition_QComboBox->lineEdit()->setReadOnly(true);
		ClothingComposition_QComboBox->lineEdit()->setAlignment(Qt::AlignCenter);
		ClothingComposition_QComboBox->lineEdit()->setFont(panel->font_D_CB1);

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
		hLayout->setSpacing(panel->Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(panel->Window_width * 0.123, panel->Window_height * 0.009, 0, panel->Window_height * 0.027);

		// 버튼생성
		ClothingUpdate_QPushButton = new QPushButton(QString::fromLocal8Bit("Update"));
		panel->Buttons_FunctionPanelRight.append(ClothingUpdate_QPushButton);
		ClothingUpdate_QPushButton->setFixedSize(panel->Window_width * 0.042, panel->Window_height * 0.037);
		ClothingUpdate_QPushButton->setCheckable(false);
		ClothingUpdate_QPushButton->setStyleSheet(DialogStyle::MENU_BTN2);
		ClothingUpdate_QPushButton->setFont(panel->font_D_BTN2);
		ClothingUpdate_QPushButton->setChecked(false);

		connect(ClothingUpdate_QPushButton, SIGNAL(clicked()), this, SLOT(slot_ClothingLayerUpdate_ButtonClicked())); connect(ClothingUpdate_QPushButton, SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
		
		hLayout->addWidget(ClothingUpdate_QPushButton);

		layoutIn->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(panel->Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(0, panel->Window_height * 0.005, 0, 0);

		QLabel* labelGlasses = new QLabel;
		labelGlasses->setFixedSize(panel->Window_width * 0.052, panel->Window_height * 0.028);
		labelGlasses->setStyleSheet(DialogStyle::DATA_LABEL);
		labelGlasses->setFont(panel->font_D_LBL1);
		labelGlasses->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		labelGlasses->setText(QString::fromLocal8Bit("Eyewear"));

		Eyewear_AddDelete_QPushButton = new QPushButton(QString::fromLocal8Bit("Add"));
		panel->Buttons_FunctionPanelRight.append(Eyewear_AddDelete_QPushButton);
		Eyewear_AddDelete_QPushButton->setFixedSize(panel->Window_width * 0.030, panel->Window_height * 0.025);
		Eyewear_AddDelete_QPushButton->setStyleSheet(DialogStyle::MENU_BTN2);
		Eyewear_AddDelete_QPushButton->setFont(panel->font_D_BTN2);
		Eyewear_AddDelete_QPushButton->setCheckable(false);

		connect(Eyewear_AddDelete_QPushButton, SIGNAL(clicked()), this, SLOT(slot_Add_Delete_Eyewear_ButtonClicked())); connect(Eyewear_AddDelete_QPushButton, SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));

		hLayout->addWidget(labelGlasses);
		QSpacerItem *spacer1 = new QSpacerItem(panel->Window_width * 0.081, panel->Window_height * 0.028); // Adjust spacing
		hLayout->addSpacerItem(spacer1);
		hLayout->addWidget(Eyewear_AddDelete_QPushButton);

		layoutIn->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(panel->Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(panel->Window_width * 0.015, panel->Window_height * 0.005, 0, 0);

		QLabel* labelLayer = new QLabel;
		labelLayer->setFixedSize(panel->Window_width * 0.052, panel->Window_height * 0.028);
		labelLayer->setStyleSheet(DialogStyle::DATA_LABEL);
		labelLayer->setFont(panel->font_D_LBL1);
		labelLayer->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		labelLayer->setText(QString::fromLocal8Bit("Type:"));

		EyewearType_QLineEdit = new QLineEdit;
		EyewearType_QLineEdit->setFixedSize(panel->Window_width * 0.094, panel->Window_height * 0.028);
		EyewearType_QLineEdit->setAlignment(Qt::AlignCenter);
		EyewearType_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		EyewearType_QLineEdit->setFont(panel->font_D_LE1);
		EyewearType_QLineEdit->setText("Glasses");

		hLayout->addWidget(labelLayer);
		hLayout->addWidget(EyewearType_QLineEdit);

		layoutIn->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(panel->Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(panel->Window_width * 0.015, panel->Window_height * 0.005, 0, 0);

		QLabel* labelGlassesDistance = new QLabel;
		labelGlassesDistance->setFixedSize(panel->Window_width * 0.052, panel->Window_height * 0.028);
		labelGlassesDistance->setStyleSheet(DialogStyle::DATA_LABEL);
		labelGlassesDistance->setFont(panel->font_D_LBL1);
		labelGlassesDistance->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		labelGlassesDistance->setText(QString::fromLocal8Bit("Distance:"));

		EyewearDistance_QLineEdit = new QLineEdit;
		EyewearDistance_QLineEdit->setFixedSize(panel->Window_width * 0.094, panel->Window_height * 0.028);
		EyewearDistance_QLineEdit->setAlignment(Qt::AlignCenter);
		EyewearDistance_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		EyewearDistance_QLineEdit->setFont(panel->font_D_LE1);
		EyewearDistance_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("cm"));

		hLayout->addWidget(labelGlassesDistance);
		hLayout->addWidget(EyewearDistance_QLineEdit);

		layoutIn->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(panel->Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(panel->Window_width * 0.015, panel->Window_height * 0.005, 0, 0);

		QLabel* labelGlassesRadius = new QLabel;
		labelGlassesRadius->setFixedSize(panel->Window_width * 0.052, panel->Window_height * 0.028);
		labelGlassesRadius->setStyleSheet(DialogStyle::DATA_LABEL);
		labelGlassesRadius->setFont(panel->font_D_LBL1);
		labelGlassesRadius->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		labelGlassesRadius->setText(QString::fromLocal8Bit("Radius:"));

		EyewearRadius_QLineEdit = new QLineEdit;
		EyewearRadius_QLineEdit->setFixedSize(panel->Window_width * 0.094, panel->Window_height * 0.028);
		EyewearRadius_QLineEdit->setAlignment(Qt::AlignCenter);
		EyewearRadius_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		EyewearRadius_QLineEdit->setFont(panel->font_D_LE1);
		EyewearRadius_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("cm"));

		hLayout->addWidget(labelGlassesRadius);
		hLayout->addWidget(EyewearRadius_QLineEdit);

		layoutIn->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(panel->Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(panel->Window_width * 0.015, panel->Window_height * 0.005, 0, 0);

		EyewearThickness_QLabel = new QLabel;
		EyewearThickness_QLabel->setFixedSize(panel->Window_width * 0.052, panel->Window_height * 0.028);
		EyewearThickness_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		EyewearThickness_QLabel->setFont(panel->font_D_LBL1);
		EyewearThickness_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		EyewearThickness_QLabel->setText(QString::fromLocal8Bit("Thickness:"));

		EyewearThickness_QLineEdit = new QLineEdit;
		EyewearThickness_QLineEdit->setFixedSize(panel->Window_width * 0.094, panel->Window_height * 0.028);
		EyewearThickness_QLineEdit->setAlignment(Qt::AlignCenter);
		EyewearThickness_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		EyewearThickness_QLineEdit->setFont(panel->font_D_LE1);
		EyewearThickness_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("cm"));

		hLayout->addWidget(EyewearThickness_QLabel);
		hLayout->addWidget(EyewearThickness_QLineEdit);

		layoutIn->addLayout(hLayout);
	}


	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(panel->Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(panel->Window_width * 0.015, panel->Window_height * 0.005, 0, 0);

		EyewearThickness_Front_QLabel = new QLabel;
		EyewearThickness_Front_QLabel->setFixedSize(panel->Window_width * 0.052, panel->Window_height * 0.028); // hard coding 된 거 
		EyewearThickness_Front_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		EyewearThickness_Front_QLabel->setFont(panel->font_D_LBL1);
		EyewearThickness_Front_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		EyewearThickness_Front_QLabel->setText(QString::fromLocal8Bit("Thick. Front:"));

		EyewearThickness_Front_QLineEdit = new QLineEdit;
		EyewearThickness_Front_QLineEdit->setFixedSize(panel->Window_width * 0.094, panel->Window_height * 0.028);
		EyewearThickness_Front_QLineEdit->setAlignment(Qt::AlignCenter);
		EyewearThickness_Front_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		EyewearThickness_Front_QLineEdit->setFont(panel->font_D_LE1);
		EyewearThickness_Front_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("cm"));

		hLayout->addWidget(EyewearThickness_Front_QLabel);
		hLayout->addWidget(EyewearThickness_Front_QLineEdit);

		EyewearThickness_Front_QLabel->hide();
		EyewearThickness_Front_QLineEdit->hide();

		layoutIn->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(panel->Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(panel->Window_width * 0.015, panel->Window_height * 0.005, 0, 0);

		EyewearThickness_Side_QLabel = new QLabel;
		EyewearThickness_Side_QLabel->setFixedSize(panel->Window_width * 0.052, panel->Window_height * 0.028);
		EyewearThickness_Side_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		EyewearThickness_Side_QLabel->setFont(panel->font_D_LBL1);
		EyewearThickness_Side_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		EyewearThickness_Side_QLabel->setText(QString::fromLocal8Bit("Thick. Side:"));

		EyewearThickness_Side_QLineEdit = new QLineEdit;
		EyewearThickness_Side_QLineEdit->setFixedSize(panel->Window_width * 0.094, panel->Window_height * 0.028);
		EyewearThickness_Side_QLineEdit->setAlignment(Qt::AlignCenter);
		EyewearThickness_Side_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		EyewearThickness_Side_QLineEdit->setFont(panel->font_D_LE1);
		EyewearThickness_Side_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("cm"));

		hLayout->addWidget(EyewearThickness_Side_QLabel);
		hLayout->addWidget(EyewearThickness_Side_QLineEdit);

		EyewearThickness_Side_QLabel->hide();
		EyewearThickness_Side_QLineEdit->hide();

		layoutIn->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(panel->Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(panel->Window_width * 0.015, panel->Window_height * 0.005, 0, 0);

		QLabel* labelGlassesDensity = new QLabel;
		labelGlassesDensity->setFixedSize(panel->Window_width * 0.052, panel->Window_height * 0.028);
		labelGlassesDensity->setStyleSheet(DialogStyle::DATA_LABEL);
		labelGlassesDensity->setFont(panel->font_D_LBL1);
		labelGlassesDensity->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		labelGlassesDensity->setText(QString::fromLocal8Bit("Density:"));

		EyewearDensity_QLineEdit = new QLineEdit;
		EyewearDensity_QLineEdit->setFixedSize(panel->Window_width * 0.094, panel->Window_height * 0.028);
		EyewearDensity_QLineEdit->setAlignment(Qt::AlignCenter);
		EyewearDensity_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		EyewearDensity_QLineEdit->setFont(panel->font_D_LE1);
		EyewearDensity_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("g/cm3"));

		hLayout->addWidget(labelGlassesDensity);
		hLayout->addWidget(EyewearDensity_QLineEdit);

		layoutIn->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(panel->Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(panel->Window_width * 0.015, panel->Window_height * 0.005, 0, 0);

		QLabel* labelLayer = new QLabel;
		labelLayer->setFixedSize(panel->Window_width * 0.052, panel->Window_height * 0.028);
		labelLayer->setStyleSheet(DialogStyle::DATA_LABEL);
		labelLayer->setFont(panel->font_D_LBL1);
		labelLayer->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		labelLayer->setText(QString::fromLocal8Bit("Composition:"));

		EyewearComposition_QComboBox = new QComboBox;
		EyewearComposition_QComboBox->setFixedSize(panel->Window_width * 0.095, panel->Window_height * 0.028);
		EyewearComposition_QComboBox->setStyleSheet(DialogStyle::COMBOBOX);
		EyewearComposition_QComboBox->setFont(panel->font_D_CB1);

		EyewearComposition_QComboBox->setEditable(true);
		EyewearComposition_QComboBox->lineEdit()->setReadOnly(true);
		EyewearComposition_QComboBox->lineEdit()->setAlignment(Qt::AlignCenter);
		EyewearComposition_QComboBox->lineEdit()->setFont(panel->font_D_CB1);
				
		EyewearComposition_QComboBox->addItem("Lead");
		EyewearComposition_QComboBox->addItem("Glass");
		EyewearComposition_QComboBox->setCurrentIndex(0);

		hLayout->addWidget(labelLayer);
		hLayout->addWidget(EyewearComposition_QComboBox);

		layoutIn->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(panel->Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(panel->Window_width * 0.123, panel->Window_height * 0.005, 0, 0);

		EyewearUpdate_QPushButton = new QPushButton(QString::fromLocal8Bit("Update"));
		panel->Buttons_FunctionPanelRight.append(EyewearUpdate_QPushButton);
		EyewearUpdate_QPushButton->setFixedSize(panel->Window_width * 0.042, panel->Window_height * 0.037);
		EyewearUpdate_QPushButton->setCheckable(false);
		EyewearUpdate_QPushButton->setStyleSheet(DialogStyle::MENU_BTN2);
		EyewearUpdate_QPushButton->setFont(panel->font_D_BTN2);
		EyewearUpdate_QPushButton->setChecked(false);
		
		connect(EyewearUpdate_QPushButton, SIGNAL(clicked()), this, SLOT(slot_GlassesUpdate_ButtonClicked())); connect(EyewearUpdate_QPushButton, SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
				
		hLayout->addWidget(EyewearUpdate_QPushButton);

		layoutIn->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setContentsMargins(0, panel->Window_width * 0.005, 0, 0);
		hLayout->setSpacing(panel->Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

		QLabel* labelDosimetry = new QLabel;
		labelDosimetry->setFixedSize(panel->Window_width * 0.056, panel->Window_height * 0.028);
		labelDosimetry->setStyleSheet(DialogStyle::DATA_LABEL);
		labelDosimetry->setFont(panel->font_D_LBL1);
		labelDosimetry->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		labelDosimetry->setText(QString::fromLocal8Bit("Dosimeter"));

		hLayout->addWidget(labelDosimetry);

		layoutIn->addLayout(hLayout);
	}

	{
		QGroupBox* GroupBoxDosimeterList = new QGroupBox;
		GroupBoxDosimeterList->setAlignment(Qt::AlignCenter | Qt::AlignLeft);
		GroupBoxDosimeterList->setStyleSheet(DialogStyle::GROUPBOX_WIDGET10);
		GroupBoxDosimeterList->setFont(panel->font_D_GBW10);
		GroupBoxDosimeterList->setFixedHeight(0.060 * panel->Window_height);
		GroupBoxDosimeterList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed); // 가로, 세로 방향 크기
		GroupBoxDosimeterList->setContentsMargins(panel->Window_width * 0.001, 0, panel->Window_width * 0.001, 0);

		layoutIn->addWidget(GroupBoxDosimeterList);

		QVBoxLayout* layoutOut = new QVBoxLayout();
		layoutOut->setContentsMargins(panel->Window_width * 0.001, 0, panel->Window_width * 0.005, panel->Window_height * 0.005);
		layoutOut->setSpacing(panel->Window_width * 0.005);
		layoutOut->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QGroupBox* GroupBoxIn = new QGroupBox;
		GroupBoxIn->setAlignment(Qt::AlignCenter | Qt::AlignLeft);
		GroupBoxIn->setStyleSheet(DialogStyle::GROUPBOX_WIDGET12);
		GroupBoxIn->setFont(panel->font_D_GBW10);
		GroupBoxIn->setFixedHeight(0.060 * panel->Window_height);
		GroupBoxIn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

		DosimeterList_QHBoxLayout = new QHBoxLayout();
		DosimeterList_QHBoxLayout->setContentsMargins(panel->Window_width * 0.001, panel->Window_height * 0.005, panel->Window_width * 0.001, panel->Window_height * 0.005); // 왼쪽, 위, 오른쪽, 아래의 여유공간(해당 layout 안에서의)
		DosimeterList_QHBoxLayout->setSpacing(panel->Window_width * 0.003);
		DosimeterList_QHBoxLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		MultipleButtonScorllArea* scrollArea = new MultipleButtonScorllArea();
		scrollArea->setStyleSheet(DialogStyle::GROUPBOX_WIDGET10);
		scrollArea->setFont(panel->font_D_GBW10);
		scrollArea->setFixedHeight(panel->Window_height * 0.060);
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
		hLayout->setSpacing(panel->Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		hLayout->setContentsMargins(0, panel->Window_height * 0.005, panel->Window_width * 0.010, panel->Window_height * 0.005);
	
		DosimeterAdd_QPushButton = new QPushButton(QString::fromLocal8Bit("Add"));
		panel->Buttons_FunctionPanelRight.append(DosimeterAdd_QPushButton);
		DosimeterAdd_QPushButton->setFixedSize(panel->Window_width * 0.030, panel->Window_height * 0.025);
		DosimeterAdd_QPushButton->setStyleSheet(DialogStyle::MENU_BTN2);
		DosimeterAdd_QPushButton->setFont(panel->font_D_BTN2);	
		DosimeterAdd_QPushButton->setCheckable(false);

		DosimeterDelete_QPushButton = new QPushButton(QString::fromLocal8Bit("Delete"));
		panel->Buttons_FunctionPanelRight.append(DosimeterDelete_QPushButton);
		DosimeterDelete_QPushButton->setFixedSize(panel->Window_width * 0.030, panel->Window_height * 0.025);
		DosimeterDelete_QPushButton->setStyleSheet(DialogStyle::MENU_BTN2);
		DosimeterDelete_QPushButton->setFont(panel->font_D_BTN2);
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
		hLayout->setContentsMargins(0, panel->Window_height * 0.005, panel->Window_width * 0.010, 0);
		hLayout->setSpacing(panel->Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QLabel* label0 = new QLabel;
		label0->setFixedSize(panel->Window_width * 0.038, panel->Window_height * 0.028);
		label0->setStyleSheet(DialogStyle::DATA_LABEL);
		label0->setFont(panel->font_D_LBL1);
		label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		label0->setText(QString::fromLocal8Bit("Pos. XYZ "));

		DosimeterPosX_QLineEdit = new QLineEdit;
		DosimeterPosX_QLineEdit->setFixedSize(panel->Window_width * 0.038, panel->Window_height * 0.028);
		DosimeterPosX_QLineEdit->setAlignment(Qt::AlignCenter);
		DosimeterPosX_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME2);
		DosimeterPosX_QLineEdit->setFont(panel->font_D_LE2);
		DosimeterPosX_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(cm)"));

		DosimeterPosY_QLineEdit = new QLineEdit;
		DosimeterPosY_QLineEdit->setFixedSize(panel->Window_width * 0.038, panel->Window_height * 0.028);
		DosimeterPosY_QLineEdit->setAlignment(Qt::AlignCenter);
		DosimeterPosY_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME2);
		DosimeterPosY_QLineEdit->setFont(panel->font_D_LE2);
		DosimeterPosY_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(cm)"));

		DosimeterPosZ_QLineEdit = new QLineEdit;
		DosimeterPosZ_QLineEdit->setFixedSize(panel->Window_width * 0.038, panel->Window_height * 0.028);
		DosimeterPosZ_QLineEdit->setAlignment(Qt::AlignCenter);
		DosimeterPosZ_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME2);
		DosimeterPosZ_QLineEdit->setFont(panel->font_D_LE2);
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
		hLayout->setContentsMargins(0, panel->Window_height * 0.005, panel->Window_width * 0.010, 0);
		hLayout->setSpacing(panel->Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QLabel* label0 = new QLabel;
		label0->setFixedSize(panel->Window_width * 0.038, panel->Window_height * 0.028);
		label0->setStyleSheet(DialogStyle::DATA_LABEL);
		label0->setFont(panel->font_D_LBL1);
		label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		label0->setText(QString::fromLocal8Bit("Radius "));

		DosimeterRadius_QLineEdit = new QLineEdit;
		DosimeterRadius_QLineEdit->setFixedSize(panel->Window_width * 0.104, panel->Window_height * 0.028);
		DosimeterRadius_QLineEdit->setAlignment(Qt::AlignCenter);
		DosimeterRadius_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME2);
		DosimeterRadius_QLineEdit->setFont(panel->font_D_LE2);
		DosimeterRadius_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("Dosimeter radius (cm)"));

		hLayout->addWidget(label0);
		hLayout->addStretch();
		hLayout->addWidget(DosimeterRadius_QLineEdit);

		layoutIn->addLayout(hLayout);
	}

	{// Separation Distance
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setContentsMargins(0, panel->Window_height * 0.005, panel->Window_width * 0.010, 0);
		hLayout->setSpacing(panel->Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QLabel* label0 = new QLabel;
		label0->setFixedSize(panel->Window_width * 0.038, panel->Window_height * 0.028);
		label0->setStyleSheet(DialogStyle::DATA_LABEL);
		label0->setFont(panel->font_D_LBL1);
		label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		label0->setText(QString::fromLocal8Bit("Distance"));

		DosimeterSeparationDistance_QLineEdit = new QLineEdit;
		DosimeterSeparationDistance_QLineEdit->setFixedSize(panel->Window_width * 0.104, panel->Window_height * 0.028);
		DosimeterSeparationDistance_QLineEdit->setAlignment(Qt::AlignCenter);
		DosimeterSeparationDistance_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME2);
		DosimeterSeparationDistance_QLineEdit->setFont(panel->font_D_LE2);
		DosimeterSeparationDistance_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("separation dist. (cm)"));

		hLayout->addWidget(label0);
		hLayout->addStretch();
		hLayout->addWidget(DosimeterSeparationDistance_QLineEdit);

		layoutIn->addLayout(hLayout);
	}

	{// 3D Pick and update
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setContentsMargins(0, panel->Window_height * 0.005, panel->Window_width * 0.010, 0);
		hLayout->setSpacing(panel->Window_width * 0.005);
		hLayout->setAlignment(Qt::AlignRight | Qt::AlignTop);

		DosimeterUpdate_QPushButton = new QPushButton(QString::fromLocal8Bit("Update"));
		panel->Buttons_FunctionPanelRight.append(DosimeterUpdate_QPushButton);
		DosimeterUpdate_QPushButton->setFixedSize(panel->Window_width * 0.042, panel->Window_height * 0.037);
		DosimeterUpdate_QPushButton->setStyleSheet(DialogStyle::MENU_BTN2);
		DosimeterUpdate_QPushButton->setFont(panel->font_D_BTN2);

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

    return true;
}


void PhantomWidget::Commit_PhantomSetting(const PhantomData& pData)
{
    // [UI 업데이트 로직]
    // 이제 변수명 앞에 아무것도 안 붙여도 됩니다!
    
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

        QFileInfo fi(pData.objFilePath); 
        QString pathNoExt = fi.path() + "/" + fi.completeBaseName();        
        PhantomSetting_ImportedFileName_QLineEdit->setText(pathNoExt); 
    }

    // Dummy
    PhantomSetting_Dummy_QComboBox->setCurrentIndex(pData.isDummy ? 0 : 1);

    // Gender
    PhantomSetting_Gender_QComboBox->setCurrentIndex((pData.gender == "Male") ? 0 : 1);

    // Age & Posture
    const QMap<QString, int> ageMap = { {"Adult", 0}, {"15y", 1}, {"10y", 2}, {"5y", 3}, {"1y", 4}, {"0y", 5} };
    PhantomSetting_Age_QComboBox->setCurrentIndex(ageMap.value(pData.age)); 

    const QMap<QString, int> postureMap = { {"Standing", 0}, {"Bending", 1}, {"Kneeling", 2}, {"Sitting", 3}, {"Squatting", 4}, {"Walking", 5} };
    PhantomSetting_Posture_QComboBox->setCurrentIndex(postureMap.value(pData.posture)); 

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

void PhantomWidget::updateModelFromUI()
{
    // 1. 현재 선택된 팬텀이 없으면 저장할 대상이 없으므로 리턴
    int pIdx = m_model.m_Phantom_SelectedIndex;
    if (pIdx < 0) return;

    // =========================================================
    // [1] Phantom Position & Rotation & Info Update
    // =========================================================
    // QLineEdit의 텍스트를 숫자로 변환하여 모델에 저장
    m_model.m_Phantom_MainInfo[pIdx][E_PHANTOMMAININFO_POS_X] = PhantomPosX_QLineEdit->text().toDouble();
    m_model.m_Phantom_MainInfo[pIdx][E_PHANTOMMAININFO_POS_Y] = PhantomPosY_QLineEdit->text().toDouble();
    m_model.m_Phantom_MainInfo[pIdx][E_PHANTOMMAININFO_POS_Z] = PhantomPosZ_QLineEdit->text().toDouble();

    m_model.m_Phantom_MainInfo[pIdx][E_PHANTOMMAININFO_ROT_X] = PhantomRotX_QLineEdit->text().toDouble();
    m_model.m_Phantom_MainInfo[pIdx][E_PHANTOMMAININFO_ROT_Y] = PhantomRotY_QLineEdit->text().toDouble();
    m_model.m_Phantom_MainInfo[pIdx][E_PHANTOMMAININFO_ROT_Z] = PhantomRotZ_QLineEdit->text().toDouble();

    // (필요 시) Height, Weight 등이 메인 화면에서 수정 가능하다면 여기서 업데이트
    // m_model.m_Phantom_MainInfo[pIdx][E_PHANTOMMAININFO_HEIGHT] = PhantomHeight_QLineEdit->text().toDouble();
    // m_model.m_Phantom_MainInfo[pIdx][E_PHANTOMMAININFO_WEIGHT] = PhantomWeight_QLineEdit->text().toDouble();


    // =========================================================
    // [2] Clothing Layer Update (의복이 선택된 상태라면)
    // =========================================================
    int cIdx = m_model.m_Clothing_SelectedIndex;
    int lIdx = m_model.m_ClothingLayer_SelectedIndex;

    if (cIdx >= 0 && lIdx >= 0)
    {
        // [PhantomIdx][ClothingIdx][LayerIdx][InfoEnum] 구조
        m_model.m_Clothing_MainInfo[pIdx][cIdx][lIdx][E_CLOTHINGMAININFO_THICKNESS] 
            = ClothingThickness_QLineEdit->text().toDouble();
        
        m_model.m_Clothing_MainInfo[pIdx][cIdx][lIdx][E_CLOTHINGMAININFO_DENSITY] 
            = ClothingDensity_QLineEdit->text().toDouble();
        
        m_model.m_Clothing_MainInfo[pIdx][cIdx][lIdx][E_CLOTHINGMAININFO_COMPOSITION] 
            = ClothingComposition_QComboBox->currentIndex();
    }


    // =========================================================
    // [3] Dosimeter Update (선량계가 선택된 상태라면)
    // =========================================================
    int dIdx = m_model.m_Dosimeter_Selected_Index;
    
    // *중요: 기존 코드에서 선량계 정보를 m_model.m_Clothing_MainInfo의 [1000]번 인덱스에 저장하던 로직 유지
    if (dIdx >= 0)
    {
        // 1: Radius, 2: Separation, 3~5: XYZ
        m_model.m_Clothing_MainInfo[pIdx][1000][dIdx][1] = DosimeterRadius_QLineEdit->text().toDouble();
        m_model.m_Clothing_MainInfo[pIdx][1000][dIdx][2] = DosimeterSeparationDistance_QLineEdit->text().toDouble();
        m_model.m_Clothing_MainInfo[pIdx][1000][dIdx][3] = DosimeterPosX_QLineEdit->text().toDouble();
        m_model.m_Clothing_MainInfo[pIdx][1000][dIdx][4] = DosimeterPosY_QLineEdit->text().toDouble();
        m_model.m_Clothing_MainInfo[pIdx][1000][dIdx][5] = DosimeterPosZ_QLineEdit->text().toDouble();
    }

	m_model.m_Dosimeter_Radius = DosimeterRadius_QLineEdit->text().toDouble();
    // =========================================================
    // [4] Eyewear Update (안경)
    // =========================================================
    // Eyewear는 기존 코드 구조상 별도의 저장 변수가 있거나, 
    // 즉시 함수 인자로 넘어가는 경우가 많아 모델 변수가 명확지 않다면 생략 가능합니다.
    // 만약 모델에 추가했다면 여기서 업데이트하세요.

	m_model.m_Eyewear_Thickness = EyewearThickness_QLineEdit->text().toDouble();
    m_model.m_Eyewear_Radius = EyewearRadius_QLineEdit->text().toDouble();
    m_model.m_Eyewear_Distance = EyewearDistance_QLineEdit->text().toDouble();
    m_model.m_Eyewear_Density = EyewearDensity_QLineEdit->text().toDouble();
    m_model.m_Eyewear_composition = EyewearComposition_QComboBox->currentIndex();
}

std::map<int, QPushButton*>& PhantomWidget::getQPushButtonMap(ButtonType type)
{
    switch (type) {
        case ButtonType::Phantom:
            return PhantomSelect_QPushButton;
        
        case ButtonType::Clothing:
            return ClothingSelect_QPushButton;
        
        case ButtonType::ClothingLayer:
            return ClothingLayerSelect_QPushButton;
        
        case ButtonType::Dosimeter:
            return DosimeterSelect_QPushButton;
    }
    
    // (예외 처리) 혹시 모를 상황을 대비해 기본값으로 Phantom 반환
    // 실제로는 위 case 중 하나에 반드시 걸리게 짜야 합니다.
    return PhantomSelect_QPushButton; 
}

QLabel* PhantomWidget::getPhantomRAMUsageLabel() const
{
	return Phantom_RAMusage_QLabel;
}

void PhantomWidget::WritePhantomMacro(std::ofstream& ofpPhantom)
{
	updateModelFromUI();
}
void PhantomWidget::WritePhantomInfo(std::ofstream& ofp_info)
{
	updateModelFromUI();
	for (auto itr_phantomIndex : m_model.m_Phantom_SequenceVector)
	{
		ofp_info << "[" << m_model.m_Phantom_ButtonName[itr_phantomIndex].toStdString() << "]" << endl;

		if (m_model.m_Phantom_MainInfo[itr_phantomIndex][E_PHANTOMMAININFO_CATEGORY] == E_PHANTOMCATEGORY_AIR)
		{
			ofp_info << "\tPosX(cm): " << m_model.m_Phantom_MainInfo[itr_phantomIndex][6] << endl; // PosX
			ofp_info << "\tPosY(cm): " << m_model.m_Phantom_MainInfo[itr_phantomIndex][7] << endl; // PosY
			ofp_info << "\tPosZ(cm): " << m_model.m_Phantom_MainInfo[itr_phantomIndex][8] << endl; // PosZ

			continue; // Air sphre이면 Position 정보만 출력
		}

		// Phantom type: MRCP
		if (m_model.m_Phantom_MainInfo[itr_phantomIndex][2] == E_PHANTOMTYPE_ADULTMRCP)
		{
			ofp_info << "\tPhantom_type: Adult_MRCP" << endl;
		}
		else if (m_model.m_Phantom_MainInfo[itr_phantomIndex][2] == E_PHANTOMTYPE_PEDIATRICMRCP)
		{
			ofp_info << "\tPhantom_type: Pediatric_MRCP" << endl;
		}
		else if (m_model.m_Phantom_MainInfo[itr_phantomIndex][2] == E_PHANTOMTYPE_PFMRCP)
		{
			ofp_info << "\tPhantom_type: Pregnant-female_MRCP" << endl;
		}
		// Phantom type: Tramsformed
		else if (m_model.m_Phantom_MainInfo[itr_phantomIndex][2] == E_PHANTOMTYPE_TRANSFORMED)
		{
			ofp_info << "\tPhantom_type: Transformed_phantom" << endl;
		}
		// Phantom type: Imported
		else if (m_model.m_Phantom_MainInfo[itr_phantomIndex][2] == E_PHANTOMTYPE_IMPORTED)
		{
			ofp_info << "\tPhantom_type: Imported_phantom" << endl;
		}

		if (m_model.m_Phantom_MainInfo[itr_phantomIndex][E_PHANTOMMAININFO_DUMMY] == E_PHANTOMDUMMY_YES) ofp_info << "\tDummy_phantom: Yes" << endl;
		else ofp_info << "\tDummy_phantom: No(Full_phantom)" << endl;

		// Gender
		if (m_model.m_Phantom_MainInfo[itr_phantomIndex][1] == 0) ofp_info << "\tGender: Male" << endl;
		else ofp_info << "\tGender: Female" << endl;

		// Age		
		if (m_model.m_Phantom_MainInfo[itr_phantomIndex][12] == 0) // Age: Adult 
		{
			ofp_info << "\tAge: Adult" << endl;
		}
		if (m_model.m_Phantom_MainInfo[itr_phantomIndex][12] == 1) // Age: 15 
		{
			ofp_info << "\tAge: 15y" << endl;
		}
		if (m_model.m_Phantom_MainInfo[itr_phantomIndex][12] == 2) // Age: 10 
		{
			ofp_info << "\tAge: 10y" << endl;
		}
		if (m_model.m_Phantom_MainInfo[itr_phantomIndex][12] == 3) // Age: 5 
		{
			ofp_info << "\tAge: 5y" << endl;
		}
		if (m_model.m_Phantom_MainInfo[itr_phantomIndex][12] == 4) // Age: 1 
		{
			ofp_info << "\tAge: 1y" << endl;
		}
		if (m_model.m_Phantom_MainInfo[itr_phantomIndex][12] == 5) // Age: 0 
		{
			ofp_info << "\tAge: 0y" << endl;
		}

		// Posutre		
		if (m_model.m_Phantom_MainInfo[itr_phantomIndex][3] == 0) ofp_info << "\tPosture: Standing" << endl;
		if (m_model.m_Phantom_MainInfo[itr_phantomIndex][3] == 1) ofp_info << "\tPosture: Bending" << endl;
		if (m_model.m_Phantom_MainInfo[itr_phantomIndex][3] == 2) ofp_info << "\tPosture: Kneeling" << endl;
		if (m_model.m_Phantom_MainInfo[itr_phantomIndex][3] == 3) ofp_info << "\tPosture: Sitting" << endl;
		if (m_model.m_Phantom_MainInfo[itr_phantomIndex][3] == 4) ofp_info << "\tPosture: Squatting" << endl;
		if (m_model.m_Phantom_MainInfo[itr_phantomIndex][3] == 5) ofp_info << "\tPosture: Walking" << endl;

		// Height/Weight
		ofp_info << "\tHeight(cm): " << m_model.m_Phantom_MainInfo[itr_phantomIndex][4] << endl; // Height
		ofp_info << "\tWeight(kg): " << m_model.m_Phantom_MainInfo[itr_phantomIndex][5] << endl; // Weight

		ofp_info << "\tPosX(cm): " << m_model.m_Phantom_MainInfo[itr_phantomIndex][6] << endl; // PosX
		ofp_info << "\tPosY(cm): " << m_model.m_Phantom_MainInfo[itr_phantomIndex][7] << endl; // PosY
		ofp_info << "\tPosZ(cm): " << m_model.m_Phantom_MainInfo[itr_phantomIndex][8] << endl; // PosZ
		ofp_info << "\tRotX(deg): " << m_model.m_Phantom_MainInfo[itr_phantomIndex][9] << endl; // RotX
		ofp_info << "\tRotY(deg): " << m_model.m_Phantom_MainInfo[itr_phantomIndex][10] << endl; // RotY
		ofp_info << "\tRotZ(deg): " << m_model.m_Phantom_MainInfo[itr_phantomIndex][11] << endl; // RotZ

		//Clothing
		ofp_info << endl << "\tClothing:" << endl;
		for (auto itr_clothingIndex : m_model.m_Clothing_SequenceVector[itr_phantomIndex])
		{
			if (m_model.m_Clothing_Type[itr_phantomIndex][itr_clothingIndex] == 0)
			{
				ofp_info << "\t[" << m_model.m_Clothing_ButtonName[itr_phantomIndex][itr_clothingIndex].toStdString() << "]" << endl;
			}
			if (m_model.m_Clothing_Type[itr_phantomIndex][itr_clothingIndex] == 1) ofp_info << "\t[Upper_head]" << endl;
			if (m_model.m_Clothing_Type[itr_phantomIndex][itr_clothingIndex] == 2) ofp_info << "\t[Upper_arm]" << endl;
			if (m_model.m_Clothing_Type[itr_phantomIndex][itr_clothingIndex] == 3) ofp_info << "\t[Lower_arm]" << endl;
			if (m_model.m_Clothing_Type[itr_phantomIndex][itr_clothingIndex] == 4) ofp_info << "\t[Hands]" << endl;
			if (m_model.m_Clothing_Type[itr_phantomIndex][itr_clothingIndex] == 5) ofp_info << "\t[Front_trunk]" << endl;
			if (m_model.m_Clothing_Type[itr_phantomIndex][itr_clothingIndex] == 6) ofp_info << "\t[Back_trunk]" << endl;
			if (m_model.m_Clothing_Type[itr_phantomIndex][itr_clothingIndex] == 7) ofp_info << "\t[Pelvis]" << endl;
			if (m_model.m_Clothing_Type[itr_phantomIndex][itr_clothingIndex] == 8) ofp_info << "\t[Upper_leg]" << endl;
			if (m_model.m_Clothing_Type[itr_phantomIndex][itr_clothingIndex] == 9) ofp_info << "\t[Lower_leg]" << endl;
			if (m_model.m_Clothing_Type[itr_phantomIndex][itr_clothingIndex] == 10) ofp_info << "\t[Feet]" << endl;
			for (auto itr_clothingLayerIndex : m_model.m_ClothingLayer_SequenceVector[itr_phantomIndex][itr_clothingIndex]) // 레이어순환
			{
				ofp_info << "\t\t[Layer_" << itr_clothingLayerIndex << "]" << endl; //Layer
				ofp_info << "\t\t\tThickness(cm): " << m_model.m_Clothing_MainInfo[itr_phantomIndex][itr_clothingIndex][itr_clothingLayerIndex][0] << endl; // thickness
				ofp_info << "\t\t\tDensity(g/cm3): " << m_model.m_Clothing_MainInfo[itr_phantomIndex][itr_clothingIndex][itr_clothingLayerIndex][1] << endl; // Density
				if (m_model.m_Clothing_MainInfo[itr_phantomIndex][itr_clothingIndex][itr_clothingLayerIndex][2] == 0) // Material: Cloth
					ofp_info << "\t\t\tMaterial: Cloth" << endl;
				if (m_model.m_Clothing_MainInfo[itr_phantomIndex][itr_clothingIndex][itr_clothingLayerIndex][2] == 1) // Material: Lead
					ofp_info << "\t\t\tMaterial: Lead" << endl;
				if (m_model.m_Clothing_MainInfo[itr_phantomIndex][itr_clothingIndex][itr_clothingLayerIndex][2] == 2) // Material: Air
					ofp_info << "\t\t\tMaterial: Air" << endl;
				ofp_info << "\t\t^" << endl;
			}
			ofp_info << "\t^" << endl;
		}
		ofp_info << endl;

		// Eyewear
		ofp_info << "\tEyewear:" << endl;
		//if (m_model.m_Clothing_MainInfo[itr_phantomIndex][100][0][2] != 0) // thickness가 0 이면 안경 정의 X
		//{
		//	if (m_model.m_Clothing_MainInfo[itr_phantomIndex][100][0][4] == 1) // Material: Glasses
		//		ofp_savefile << "\t\tMaterial: Glasses" << endl;
		//	if (m_model.m_Clothing_MainInfo[itr_phantomIndex][100][0][4] == 2) // Material: Leaded glasses
		//		ofp_savefile << "\t\tMaterial: Leaded glasses" << endl;

		//	ofp_savefile << "\t\tRadius of glasses(cm): " << m_model.m_Clothing_MainInfo[itr_phantomIndex][100][0][1] << endl;
		//	ofp_savefile << "\t\tSeparation distance (cm): " << m_model.m_Clothing_MainInfo[itr_phantomIndex][100][0][0] << endl;			
		//	ofp_savefile << "\t\tThickness (cm): " << m_model.m_Clothing_MainInfo[itr_phantomIndex][100][0][2] << endl;
		//	ofp_savefile << "\t\tDensity (g/cm3): " << m_model.m_Clothing_MainInfo[itr_phantomIndex][100][0][3] << endl;
		//}
		ofp_info << endl;

		//// Dosimeter
		ofp_info << "\tDosimeters:" << endl;

		//for (int DosimeterIdx = 0; DosimeterIdx < m_model.m_Clothing_MainInfo[itr_phantomIndex][999][0][0]; DosimeterIdx++) // dosimeter 순환
		//{
		//	ofp_savefile << "\tDosimeter name: Dosimeter_" << DosimeterIdx << endl;
		//	ofp_savefile << "\t\tRelative PosX (cm): " << m_model.m_Clothing_MainInfo[itr_phantomIndex][1000][DosimeterIdx][3] << endl; // Relative PosX
		//	ofp_savefile << "\t\tRelative PosY (cm): " << m_model.m_Clothing_MainInfo[itr_phantomIndex][1000][DosimeterIdx][4] << endl; // Relative PosY
		//	ofp_savefile << "\t\tRelative PosZ (cm): " << m_model.m_Clothing_MainInfo[itr_phantomIndex][1000][DosimeterIdx][5] << endl; // Relative PosZ

		//	ofp_savefile << "\t\tRadius (cm): " << m_model.m_Clothing_MainInfo[itr_phantomIndex][1000][DosimeterIdx][1] << endl; // Radius
		//	ofp_savefile << "\t\tDistance (cm): " << m_model.m_Clothing_MainInfo[itr_phantomIndex][1000][DosimeterIdx][2] << endl << endl; // Distance
		//  ofp_savefile << "\t^" << endl; //Dosimeter 구분
		//}
		ofp_info << endl;

		ofp_info << "^" << endl; // Phantom 구분
	}

}

void PhantomWidget::ReadPhantomInfo(std::ifstream& ifp)
{
	std::string dump;
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
					QString folderPath = m_CurrentLoadingDir;
					// 2) 파일명 생성
					QString filename = QStringLiteral("ImportedPhantom_%1.ply")
						.arg(m_model.m_Phantom_MakingIndex);
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
				m_model.m_Phantom_SequenceVector.push_back(m_model.m_Phantom_MakingIndex);
				// Generate phantom button
				if (PhantomSetting_Name_QLineEdit->text().isEmpty()) PhantomSelect_QPushButton[m_model.m_Phantom_MakingIndex] = new QPushButton("Phantom_" + QString::number(m_model.m_Phantom_MakingIndex)); // 구조물 이름 입력안했을 때 기본값
				else PhantomSelect_QPushButton[m_model.m_Phantom_MakingIndex] = new QPushButton(PhantomSetting_Name_QLineEdit->text()); // 구조물 이름 
				panel->Buttons_FunctionPanelRight.append(PhantomSelect_QPushButton[m_model.m_Phantom_MakingIndex]);
				PhantomSelect_QPushButton[m_model.m_Phantom_MakingIndex]->setObjectName(QString::number(m_model.m_Phantom_MakingIndex));
				PhantomSelect_QPushButton[m_model.m_Phantom_MakingIndex]->setFixedSize(panel->Window_width * 0.05, panel->Window_height * 0.040);
				PhantomSelect_QPushButton[m_model.m_Phantom_MakingIndex]->setStyleSheet(DialogStyle::MENU_BTN7);
				PhantomSelect_QPushButton[m_model.m_Phantom_MakingIndex]->setFont(panel->font_D_BTN14);
				PhantomSelect_QPushButton[m_model.m_Phantom_MakingIndex]->setCheckable(true);
				PhantomSelect_QPushButton[m_model.m_Phantom_MakingIndex]->setChecked(true);
				connect(PhantomSelect_QPushButton[m_model.m_Phantom_MakingIndex], SIGNAL(clicked()), this, SLOT(slot_PhantomSelect_ButtonClicked())); connect(PhantomSelect_QPushButton[m_model.m_Phantom_MakingIndex], SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
				PhantomList_QHBoxLayout->addWidget(PhantomSelect_QPushButton[m_model.m_Phantom_MakingIndex]);
				for (auto itr : m_model.m_Phantom_SequenceVector)
				{
					if (itr != m_model.m_Phantom_MakingIndex)
					{
						PhantomSelect_QPushButton[itr]->setChecked(false);
					}
				}
				timer_scroll_multiplePhantom->start(1);
				if (m_model.m_Phantom_SequenceVector.size() == PhantomConstants::const_Phantom_MaximumCount) PhantomAdd_QPushButton->setEnabled(false);
				// Set global variables 
				m_model.m_Phantom_SelectedIndex = m_model.m_Phantom_MakingIndex;
				m_model.m_Phantom_MakingIndex++;
				m_model.m_Is_PhantomSetting_Closed = false; // Setting 시 X버튼 눌렀을 때 true
				m_model.m_Is_PhantomSetting_OKbutton_Clicked = true; // Setting의 OK 버튼 눌렀을 때 true
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
				//		m_Clothing_ButtonName[m_model.m_Phantom_MakingIndex][clothing_making_index] = QString::fromStdString(theApp.ExtractInnerString(dump)); // [clothing_name] 이름 저장
				//		//의복제작//
				//		int layer_making_index = 0;
				//		while (ifp >> dump) // [Layer_X] or "^"
				//		{
				//			if (dump == "^") break; // END signal "^"
				//			if (dump != "^") // [Layer_X]
				//			{
				//				ifp >> dump >> dump; m_Clothing_MainInfo[m_model.m_Phantom_MakingIndex][clothing_making_index][layer_making_index][0] = std::stod(dump); // Thickness
				//				ifp >> dump >> dump; m_Clothing_MainInfo[m_model.m_Phantom_MakingIndex][clothing_making_index][layer_making_index][1] = std::stod(dump); // Density
				//				ifp >> dump >> dump;
				//				if (dump == "Cloth") m_Clothing_MainInfo[m_model.m_Phantom_MakingIndex][clothing_making_index][layer_making_index][2] = 0;
				//				if (dump == "Lead") m_Clothing_MainInfo[m_model.m_Phantom_MakingIndex][clothing_making_index][layer_making_index][2] = 1;
				//				if (dump == "Air") m_Clothing_MainInfo[m_model.m_Phantom_MakingIndex][clothing_making_index][layer_making_index][2] = 2;
				//				layer_making_index++;
				//				ifp >> dump; // "^" 
				//			}
				//		}
				//		int pre_defined_index = 0;
				//		QVector<QString> clothingNames = { "Invalid", "head", "upper_arm", "lower_arm", "hands", "front_trunk", "back_trunk", "pelvis", "upper_leg", "lower_leg", "feet" };
				//		for (int i = 0; i < clothingNames.size(); ++i)
				//		{
				//			if (clothingNames[i] == m_Clothing_ButtonName[m_model.m_Phantom_MakingIndex][clothing_making_index]) {
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

void PhantomWidget::SetLoadingBaseDir(const QString& dir)
{
    // 파일 전체 경로가 들어오면, 미리 폴더 경로만 추출해서 저장해 둡니다.
    // 예: "C:/Data/test.txt" -> "C:/Data"
    QFileInfo fi(dir);
    m_CurrentLoadingDir = fi.path();
}
void PhantomWidget::Eyewear_Generate(int PhantomIdx)
{
	updateModelFromUI();
	m_model.m_Clothing_MainInfo[PhantomIdx][100][0][0] = EyewearDistance_QLineEdit->text().toDouble(); // distance
	m_model.m_Clothing_MainInfo[PhantomIdx][100][0][1] = EyewearRadius_QLineEdit->text().toDouble(); // radius
	m_model.m_Clothing_MainInfo[PhantomIdx][100][0][2] = EyewearThickness_QLineEdit->text().toDouble(); // radius
	m_model.m_Clothing_MainInfo[PhantomIdx][100][0][3] = EyewearDensity_QLineEdit->text().toDouble(); // density
	// composition(1: Glasses, 2: Leaded glasses)
	m_model.m_Clothing_MainInfo[PhantomIdx][100][0][4] = EyewearComposition_QComboBox->currentIndex();
}
void PhantomWidget::Dosimeter_Generate(int PhantomIdx, int DosimeterIdx)
{
	updateModelFromUI();
	m_model.m_Clothing_MainInfo[PhantomIdx][999][0][0] = m_model.m_DosimeterSequenceVector[PhantomIdx].size();
	m_model.m_Clothing_MainInfo[PhantomIdx][1000][DosimeterIdx][3] = DosimeterPosX_QLineEdit->text().toDouble(); // X
	m_model.m_Clothing_MainInfo[PhantomIdx][1000][DosimeterIdx][4] = DosimeterPosY_QLineEdit->text().toDouble(); // Y
	m_model.m_Clothing_MainInfo[PhantomIdx][1000][DosimeterIdx][5] = DosimeterPosZ_QLineEdit->text().toDouble(); // Z
	m_model.m_Clothing_MainInfo[PhantomIdx][1000][DosimeterIdx][1] = DosimeterRadius_QLineEdit->text().toDouble(); // Radius
	m_model.m_Clothing_MainInfo[PhantomIdx][1000][DosimeterIdx][2] = DosimeterSeparationDistance_QLineEdit->text().toDouble(); // Distance
}

void PhantomWidget::temporal_Dosimeter_SetText(double x, double y, double z)
{
	updateModelFromUI();
    DosimeterPosX_QLineEdit->setText(QString::number(x));
    DosimeterPosY_QLineEdit->setText(QString::number(y));
    DosimeterPosZ_QLineEdit->setText(QString::number(z));
}
// ----------------------------------------------------------------------------
// [Helper Functions] - initialize()를 돕는 함수들
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// [Slot Functions]
// ----------------------------------------------------------------------------

void PhantomWidget::Create_Phantom_ListWidgets(QVBoxLayout* layout) // MultiplePhantom button 생성 (scroll area)
{
	PhantomList_QGroupBox = new QGroupBox;
	PhantomList_QGroupBox->setAlignment(Qt::AlignCenter | Qt::AlignLeft);
	PhantomList_QGroupBox->setStyleSheet(DialogStyle::GROUPBOX_WIDGET10);
	PhantomList_QGroupBox->setFont(panel->font_D_GBW10);
	PhantomList_QGroupBox->setFixedHeight(panel->Window_height * 0.060);
	PhantomList_QGroupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed); // 가로, 세로 방향 크기
	PhantomList_QGroupBox->setContentsMargins(panel->Window_width * 0.001, 0, panel->Window_width * 0.001, 0);

	layout->addWidget(PhantomList_QGroupBox);

	QVBoxLayout* layoutOut = new QVBoxLayout();
	layoutOut->setContentsMargins(panel->Window_width * 0.001, 0, panel->Window_width * 0.005, panel->Window_height * 0.005);
	layoutOut->setSpacing(panel->Window_width * 0.005);
	layoutOut->setAlignment(Qt::AlignLeft | Qt::AlignTop);

	QGroupBox* GroupBoxIn = new QGroupBox;	
	GroupBoxIn->setAlignment(Qt::AlignCenter | Qt::AlignLeft);
	GroupBoxIn->setStyleSheet(DialogStyle::GROUPBOX_WIDGET12);
	GroupBoxIn->setFont(panel->font_D_GBW10);
	GroupBoxIn->setFixedHeight(0.060 * panel->Window_height);
	GroupBoxIn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	PhantomList_QHBoxLayout = new QHBoxLayout();
	PhantomList_QHBoxLayout->setContentsMargins(panel->Window_width * 0.001, panel->Window_height * 0.005, panel->Window_width * 0.001, panel->Window_height * 0.005); // 왼쪽, 위, 오른쪽, 아래의 여유공간(해당 layout 안에서의)
	PhantomList_QHBoxLayout->setSpacing(panel->Window_width * 0.003);
	PhantomList_QHBoxLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

	MultipleButtonScorllArea* scrollArea = new MultipleButtonScorllArea();
	scrollArea->setStyleSheet(DialogStyle::GROUPBOX_WIDGET10);
	scrollArea->setFont(panel->font_D_GBW10);
	scrollArea->setFixedHeight(panel->Window_height * 0.060);
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
void PhantomWidget::slot_PhantomAdd_ButtonClicked()
{
	updateModelFromUI();
	// == Adding setting dialog 실행 == 
	PhantomAddingSetting_Create(); 
	PhantomSetting_UserQDialog->exec();
	// == Adding setting 완료 == 

	// == Add 루틴들 실행 ==
	ExecutePhantomAddRoutines();	
}
void PhantomWidget::ExecutePhantomAddRoutines()
{
	updateModelFromUI();
	{
		if (m_model.m_Is_PhantomSetting_Closed == true) // X버튼으로 나갔을때
		{
			m_model.m_Is_PhantomSetting_Closed = false;
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
				m_model.m_Phantom_MakingIndex,
				BodySizeInfo_for_ThisPhantom);

			theApp.phantomObjects->PhantomPolydataActor_Generate(phantom_path, m_model.m_Phantom_MakingIndex, BodySizeInfo_for_ThisPhantom);
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
			theApp.PhantomPanelActor[m_model.m_Phantom_MakingIndex] = actor;
			theApp.PhantomPanelActor[m_model.m_Phantom_MakingIndex]->SetPosition(m_model.m_Phantom_MainInfo[m_model.m_Phantom_MakingIndex][6], m_model.m_Phantom_MainInfo[m_model.m_Phantom_MakingIndex][7], m_model.m_Phantom_MainInfo[m_model.m_Phantom_MakingIndex][8]);
			theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(theApp.PhantomPanelActor[m_model.m_Phantom_MakingIndex]);
			theApp.m_pVTKWidget->renderWindow()->Render();
		}
	}

	// == List 버튼 생성 ==
	{
		if (m_model.m_Phantom_MainInfo[m_model.m_Phantom_MakingIndex][E_PHANTOMMAININFO_CATEGORY] == E_PHANTOMCATEGORY_AIR) // 에어 팬텀이면 
		{
			if (PhantomSetting_Name_QLineEdit->text().isEmpty()) PhantomSelect_QPushButton[m_model.m_Phantom_MakingIndex] = new QPushButton("AirSphere_" + QString::number(m_model.m_Phantom_MakingIndex));
			else PhantomSelect_QPushButton[m_model.m_Phantom_MakingIndex] = new QPushButton(PhantomSetting_Name_QLineEdit->text());
		}
		else // 전산팬텀이면
		{
			if (PhantomSetting_Name_QLineEdit->text().isEmpty()) PhantomSelect_QPushButton[m_model.m_Phantom_MakingIndex] = new QPushButton("Phantom_" + QString::number(m_model.m_Phantom_MakingIndex));
			else PhantomSelect_QPushButton[m_model.m_Phantom_MakingIndex] = new QPushButton(PhantomSetting_Name_QLineEdit->text());
		}
		// Generate button
		panel->Buttons_FunctionPanelRight.append(PhantomSelect_QPushButton[m_model.m_Phantom_MakingIndex]);
		PhantomSelect_QPushButton[m_model.m_Phantom_MakingIndex]->setObjectName(QString::number(m_model.m_Phantom_MakingIndex));
		PhantomSelect_QPushButton[m_model.m_Phantom_MakingIndex]->setFixedSize(panel->Window_width * 0.05, panel->Window_height * 0.040);
		PhantomSelect_QPushButton[m_model.m_Phantom_MakingIndex]->setStyleSheet(DialogStyle::MENU_BTN7);
		PhantomSelect_QPushButton[m_model.m_Phantom_MakingIndex]->setFont(panel->font_D_BTN14);
		panel->setElidedButtonText(PhantomSelect_QPushButton[m_model.m_Phantom_MakingIndex]); // setFont 뒤에 해야 함!
		PhantomSelect_QPushButton[m_model.m_Phantom_MakingIndex]->setCheckable(true);
		PhantomSelect_QPushButton[m_model.m_Phantom_MakingIndex]->setChecked(true);
		connect(PhantomSelect_QPushButton[m_model.m_Phantom_MakingIndex], SIGNAL(clicked()), this, SLOT(slot_PhantomSelect_ButtonClicked())); connect(PhantomSelect_QPushButton[m_model.m_Phantom_MakingIndex], SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
		PhantomList_QHBoxLayout->addWidget(PhantomSelect_QPushButton[m_model.m_Phantom_MakingIndex]);
		timer_scroll_multiplePhantom->start(1); // List 스크롤
		if (m_model.m_Phantom_SequenceVector.size() == PhantomConstants::const_Phantom_MaximumCount) PhantomAdd_QPushButton->setEnabled(false); // 최대치 도달 시 Add 불가능하게
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
		m_model.m_Phantom_SelectedIndex = m_model.m_Phantom_MakingIndex;
		m_model.m_Phantom_SequenceVector.push_back(m_model.m_Phantom_SelectedIndex);
		m_model.m_Phantom_MakingIndex++;
	}
	// 객체 업데이트
	{
		// 팬텀 list 버튼 업데이트
		UpdatePhantom_ListButton(m_model.m_Phantom_SelectedIndex);

		// 팬텀 actor 업데이트
		theApp.UpdatePhantom_ActorHighlighted(m_model.m_Phantom_SelectedIndex); // 현재 선택된 팬텀만 pickable 및 highlight

		// 팬텀 info status 업데이트
		UpdatePhantom_InfoStatus(m_model.m_Phantom_SelectedIndex);
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
void PhantomWidget::slot_PhantomDelete_ButtonClicked()
{
	updateModelFromUI();
	if (m_model.m_Phantom_SequenceVector.size() < 1) // 팬텀이 1개도 없을 때는 delete declined
	{
		theApp.SetMessageBox("There is no phantom to delete!");
		return;
	}
	
	// 기존 버튼 지우기	
	int originalposition = theApp.removeElement(m_model.m_Phantom_SequenceVector, m_model.m_Phantom_SelectedIndex); //{0 1 2} -> {0 2}, origianl index는 지워진 1의 자리	
	PhantomList_QHBoxLayout->removeWidget(PhantomSelect_QPushButton[m_model.m_Phantom_SelectedIndex]);
	panel->Buttons_FunctionPanelRight.removeOne(PhantomSelect_QPushButton[m_model.m_Phantom_SelectedIndex]);
	delete PhantomSelect_QPushButton[m_model.m_Phantom_SelectedIndex];
	PhantomSelect_QPushButton.erase(m_model.m_Phantom_SelectedIndex);

	// Remove actor
	{
		theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(theApp.PhantomPanelActor[m_model.m_Phantom_SelectedIndex]);
		theApp.m_pVTKWidget->renderWindow()->Render();
		theApp.PhantomPanelActor[m_model.m_Phantom_SelectedIndex] = nullptr;
	}
	
	// Remove polyata
	{
		theApp.PhantomPanelPolyData_Collection[m_model.m_Phantom_SelectedIndex][0][0] = nullptr;
		for (auto itr_PolyData : theApp.PhantomPanelPolyData_Collection[m_model.m_Phantom_SelectedIndex][1]) // Clothing
		{
			theApp.PhantomPanelPolyData_Collection[m_model.m_Phantom_SelectedIndex][1][itr_PolyData.first] = nullptr;
		}
		for (auto itr_PolyData : theApp.PhantomPanelPolyData_Collection[m_model.m_Phantom_SelectedIndex][2]) // Dosimeter
		{
			theApp.PhantomPanelPolyData_Collection[m_model.m_Phantom_SelectedIndex][2][itr_PolyData.first] = nullptr;
		}
		for (auto itr_PolyData : theApp.PhantomPanelPolyData_Collection[m_model.m_Phantom_SelectedIndex][3]) // Glasses
		{
			theApp.PhantomPanelPolyData_Collection[m_model.m_Phantom_SelectedIndex][3][itr_PolyData.first] = nullptr;
		}
	}

	// Phantom 패널
	{
		m_model.m_Phantom_MainInfo[m_model.m_Phantom_SelectedIndex].clear(); // phantom info data 지우기
	}
	

	// Clothing 패널
	{
		// Clothing
		RemoveClothingPanel(); // Clothing 패널 버튼 제거
		m_model.m_Clothing_SequenceVector[m_model.m_Phantom_SelectedIndex].clear(); // Clothing 벡터 초기화 -> 벡터 초기화로 모두 초기화됨
	}


	///////////////////////////// remove 완료, 새로 만들기///////////////////

	// SelectedIndex 설정
	{
		if (m_model.m_Phantom_SequenceVector.size() == 0) m_model.m_Phantom_SelectedIndex = -1; // Phantom 개수 0일때 현재 선택된 index = -1(Invalid)로 설정
		else m_model.m_Phantom_SelectedIndex = m_model.m_Phantom_SequenceVector[originalposition]; // 새로운 인덱스로 설정
	}
	
	// 팬텀 list 버튼 업데이트
	{
		UpdatePhantom_ListButton(m_model.m_Phantom_SelectedIndex);
	}

	// InfoStatus 업데이트
	{
		UpdatePhantom_InfoStatus(m_model.m_Phantom_SelectedIndex); // Info Status창 업데이트
	}
	
	// 팬텀 actor 업데이트
	{
		theApp.UpdatePhantom_ActorHighlighted(m_model.m_Phantom_SelectedIndex); // 현재 선택된 팬텀만 highlight
	}
	
	// 의복 업데이트
	{
		SetClothingPanel_inPhantomDelete();
	}
}
void PhantomWidget::slot_PhantomSelect_ButtonClicked() // 다른 팬텀 선택 버튼
{
	updateModelFromUI();
	QPushButton* buttonSender = qobject_cast<QPushButton*>(sender());

	// 원래 버튼 클릭 시 다시 check하고 돌아가기
	if (buttonSender->objectName().toInt() == m_model.m_Phantom_SelectedIndex)
	{
		PhantomSelect_QPushButton[m_model.m_Phantom_SelectedIndex]->setChecked(true);
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
	m_model.m_Phantom_SelectedIndex = buttonSender->objectName().toInt();
	int id = m_model.m_Phantom_SelectedIndex;

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
void PhantomWidget::slot_PhantomMoveLeft_ButtonClicked()
{
	updateModelFromUI();
	if (m_model.m_Phantom_SequenceVector.size() < 2) return; // 객체가 2개 미만이면 함수를 즉시 종료합니다.

	int swappedIndex = theApp.transposeElement(m_model.m_Phantom_SequenceVector, m_model.m_Phantom_SelectedIndex, "left");
	bool removeButton = false;
	for (auto itr_phantomSequence : m_model.m_Phantom_SequenceVector)
	{
		if (itr_phantomSequence == swappedIndex) removeButton = true;
		if (removeButton)
		{
			PhantomList_QHBoxLayout->removeWidget(PhantomSelect_QPushButton[itr_phantomSequence]);
			panel->Buttons_FunctionPanelRight.removeOne(PhantomSelect_QPushButton[itr_phantomSequence]);
			delete PhantomSelect_QPushButton[itr_phantomSequence];
			PhantomSelect_QPushButton.erase(itr_phantomSequence);
		}
	}
	bool generateButton = false;
	for (auto itr_phantomSequence : m_model.m_Phantom_SequenceVector)
	{
		if (itr_phantomSequence == swappedIndex) generateButton = true;
		if (generateButton)
		{
			PhantomSelect_QPushButton[itr_phantomSequence] = new QPushButton(m_model.m_Phantom_ButtonName[itr_phantomSequence]);
			panel->Buttons_FunctionPanelRight.append(PhantomSelect_QPushButton[itr_phantomSequence]);
			PhantomSelect_QPushButton[itr_phantomSequence]->setObjectName(QString::number(itr_phantomSequence));
			PhantomSelect_QPushButton[itr_phantomSequence]->setFixedSize(panel->Window_width * 0.05, panel->Window_height * 0.040);
			PhantomSelect_QPushButton[itr_phantomSequence]->setStyleSheet(DialogStyle::MENU_BTN7);
			PhantomSelect_QPushButton[itr_phantomSequence]->setFont(panel->font_D_BTN14);
			panel->setElidedButtonText(PhantomSelect_QPushButton[itr_phantomSequence]); // setFont 뒤에 해야 함!
			PhantomSelect_QPushButton[itr_phantomSequence]->setCheckable(true);
			connect(PhantomSelect_QPushButton[itr_phantomSequence], SIGNAL(clicked()), this, SLOT(slot_PhantomSelect_ButtonClicked())); connect(PhantomSelect_QPushButton[itr_phantomSequence], SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
			PhantomList_QHBoxLayout->addWidget(PhantomSelect_QPushButton[itr_phantomSequence]);
		}
	}
	PhantomSelect_QPushButton[m_model.m_Phantom_SelectedIndex]->setChecked(true);
}
void PhantomWidget::slot_PhantomMoveRight_ButtonClicked()
{
	updateModelFromUI();
	if (m_model.m_Phantom_SequenceVector.size() < 2) return; // 객체가 2개 미만이면 함수를 즉시 종료합니다.

	int swappedIndex = theApp.transposeElement(m_model.m_Phantom_SequenceVector, m_model.m_Phantom_SelectedIndex, "right");
	bool removeButton = false;
	for (auto itr_phantomSequence : m_model.m_Phantom_SequenceVector)
	{
		if (itr_phantomSequence == m_model.m_Phantom_SelectedIndex) removeButton = true;
		if (removeButton)
		{
			PhantomList_QHBoxLayout->removeWidget(PhantomSelect_QPushButton[itr_phantomSequence]);
			panel->Buttons_FunctionPanelRight.removeOne(PhantomSelect_QPushButton[itr_phantomSequence]);
			delete PhantomSelect_QPushButton[itr_phantomSequence];
			PhantomSelect_QPushButton.erase(itr_phantomSequence);
		}
	}
	bool generateButton = false;
	for (auto itr_phantomSequence : m_model.m_Phantom_SequenceVector)
	{
		if (itr_phantomSequence == m_model.m_Phantom_SelectedIndex) generateButton = true;
		if (generateButton)
		{
			PhantomSelect_QPushButton[itr_phantomSequence] = new QPushButton(m_model.m_Phantom_ButtonName[itr_phantomSequence]);
			panel->Buttons_FunctionPanelRight.append(PhantomSelect_QPushButton[itr_phantomSequence]);
			PhantomSelect_QPushButton[itr_phantomSequence]->setObjectName(QString::number(itr_phantomSequence));
			PhantomSelect_QPushButton[itr_phantomSequence]->setFixedSize(panel->Window_width * 0.05, panel->Window_height * 0.040);
			PhantomSelect_QPushButton[itr_phantomSequence]->setStyleSheet(DialogStyle::MENU_BTN7);
			PhantomSelect_QPushButton[itr_phantomSequence]->setFont(panel->font_D_BTN14);
			panel->setElidedButtonText(PhantomSelect_QPushButton[itr_phantomSequence]); // setFont 뒤에 해야 함!
			PhantomSelect_QPushButton[itr_phantomSequence]->setCheckable(true);
			connect(PhantomSelect_QPushButton[itr_phantomSequence], SIGNAL(clicked()), this, SLOT(slot_PhantomSelect_ButtonClicked())); connect(PhantomSelect_QPushButton[itr_phantomSequence], SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
			PhantomList_QHBoxLayout->addWidget(PhantomSelect_QPushButton[itr_phantomSequence]);
		}
	}
	PhantomSelect_QPushButton[m_model.m_Phantom_SelectedIndex]->setChecked(true);
}

// Main function - Adding Setting
void PhantomWidget::PhantomAddingSetting_Create()
{
	updateModelFromUI();
	m_model.m_Is_PhantomSetting_OKbutton_Clicked = false;
	// Create a new dialog
	PhantomSetting_UserQDialog = new MultipleUIDialog(this);
	PhantomSetting_UserQDialog->setStyleSheet("background-color: rgb(205, 222, 243);");
	PhantomSetting_UserQDialog->setFixedWidth(panel->Window_width * 0.3);
	PhantomSetting_UserQDialog->setFixedHeight(panel->Window_height * 0.72);
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
		hLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.015, panel->Window_width * 0.003, 0);
		hLayout->setSpacing(panel->Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		PhantomSetting_Category_QLabel = new QLabel;
		PhantomSetting_Category_QLabel->setFixedSize(panel->Window_width * 0.090, panel->Window_height * 0.028);
		PhantomSetting_Category_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		PhantomSetting_Category_QLabel->setFont(panel->font_D_LBL1);
		PhantomSetting_Category_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		PhantomSetting_Category_QLabel->setText(QString::fromLocal8Bit("Category:"));

		// 버튼생성
		PhantomSetting_ComputationlPhantom_QRadioButton = new QRadioButton(QString::fromLocal8Bit("Computational phantom"));
		PhantomSetting_ComputationlPhantom_QRadioButton->setStyleSheet(DialogStyle::RADIOBUTTON_WIDGET);
		PhantomSetting_ComputationlPhantom_QRadioButton->setFont(panel->font_D_RB3);
		PhantomSetting_ComputationlPhantom_QRadioButton->setFixedSize(panel->Window_width * 0.099, panel->Window_height * 0.028);
		PhantomSetting_ComputationlPhantom_QRadioButton->setChecked(true);

		// 버튼생성
		PhantomSetting_AirPhantom_QRadioButton = new QRadioButton(QString::fromLocal8Bit("Air sphere phantom"));
		PhantomSetting_AirPhantom_QRadioButton->setStyleSheet(DialogStyle::RADIOBUTTON_WIDGET);
		PhantomSetting_AirPhantom_QRadioButton->setFont(panel->font_D_RB3);
		PhantomSetting_AirPhantom_QRadioButton->setFixedSize(panel->Window_width * 0.099, panel->Window_height * 0.028);
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
		hLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.015, panel->Window_width * 0.003, 0);
		hLayout->setSpacing(panel->Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		PhantomSetting_Name_QLabel = new QLabel;
		PhantomSetting_Name_QLabel->setFixedSize(panel->Window_width * 0.090, panel->Window_height * 0.028);
		PhantomSetting_Name_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		PhantomSetting_Name_QLabel->setFont(panel->font_D_LBL1);
		PhantomSetting_Name_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		PhantomSetting_Name_QLabel->setText(QString::fromLocal8Bit("Name:"));

		PhantomSetting_Name_QLineEdit = new QLineEdit;
		PhantomSetting_Name_QLineEdit->setFixedSize(panel->Window_width * 0.170, panel->Window_height * 0.028);
		PhantomSetting_Name_QLineEdit->setAlignment(Qt::AlignCenter);
		PhantomSetting_Name_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		PhantomSetting_Name_QLineEdit->setFont(panel->font_D_LE1);
		PhantomSetting_Name_QLineEdit->setPlaceholderText("Phantom_" + QString::number(m_model.m_Phantom_MakingIndex));

		hLayout->addWidget(PhantomSetting_Name_QLabel);
		hLayout->addWidget(PhantomSetting_Name_QLineEdit);

		layout->addLayout(hLayout);
	}

	{// Type
		PhantomSetting_Type_QHBoxLayout = new QHBoxLayout;
		PhantomSetting_Type_QHBoxLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.015, panel->Window_width * 0.003, 0);
		PhantomSetting_Type_QHBoxLayout->setSpacing(panel->Window_width * 0.003);
		PhantomSetting_Type_QHBoxLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		PhantomSetting_Type_QLabel = new QLabel;
		PhantomSetting_Type_QLabel->setFixedSize(panel->Window_width * 0.090, panel->Window_height * 0.028);
		PhantomSetting_Type_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		PhantomSetting_Type_QLabel->setFont(panel->font_D_LBL1);
		PhantomSetting_Type_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		PhantomSetting_Type_QLabel->setText(QString::fromLocal8Bit("Type:"));

		PhantomSetting_Type_QComboBox = new QComboBox;
		PhantomSetting_Type_QComboBox->setFixedSize(panel->Window_width * 0.170, panel->Window_height * 0.028);
		PhantomSetting_Type_QComboBox->setStyleSheet(DialogStyle::COMBOBOX);
		PhantomSetting_Type_QComboBox->setFont(panel->font_D_CB1);

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
		hLayout->setSpacing(panel->Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		hLayout->setContentsMargins(panel->Window_width * 0.003, 0, panel->Window_width * 0.003, 0);

		PhantomSetting_ImportedFileName_QLineEdit = new QLineEdit;
		PhantomSetting_ImportedFileName_QLineEdit->setFixedSize(panel->Window_width * 0.195, panel->Window_height * 0.028);
		PhantomSetting_ImportedFileName_QLineEdit->setAlignment(Qt::AlignCenter);
		PhantomSetting_ImportedFileName_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		PhantomSetting_ImportedFileName_QLineEdit->setFont(panel->font_D_LE5);
		PhantomSetting_ImportedFileName_QLineEdit->setPlaceholderText("None");
		PhantomSetting_ImportedFileName_QLineEdit->setReadOnly(true);
		PhantomSetting_ImportedFileName_QLineEdit->hide();

		PhantomSetting_PhantomImport_QPushButton = new QPushButton(QString::fromLocal8Bit("Import"));
		panel->Buttons_FunctionPanelRight.append(PhantomSetting_PhantomImport_QPushButton);
		PhantomSetting_PhantomImport_QPushButton->setFixedSize(panel->Window_width * 0.065, panel->Window_height * 0.028);
		PhantomSetting_PhantomImport_QPushButton->setCheckable(false);
		PhantomSetting_PhantomImport_QPushButton->setStyleSheet(DialogStyle::MENU_BTN3);
		PhantomSetting_PhantomImport_QPushButton->setFont(panel->font_D_BTN3);
		PhantomSetting_PhantomImport_QPushButton->hide();

		connect(PhantomSetting_PhantomImport_QPushButton, SIGNAL(clicked()), this, SLOT(slot_PhantomSetting_Import_ButtonClicked())); connect(PhantomSetting_PhantomImport_QPushButton, SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));

		hLayout->addWidget(PhantomSetting_ImportedFileName_QLineEdit);
		hLayout->addWidget(PhantomSetting_PhantomImport_QPushButton);

		layout->addLayout(hLayout);
	}

	{// Dummy
		PhantomSetting_Dummy_QHBoxLayout = new QHBoxLayout;
		PhantomSetting_Dummy_QHBoxLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.015, panel->Window_width * 0.003, 0);
		PhantomSetting_Dummy_QHBoxLayout->setSpacing(panel->Window_width * 0.003);
		PhantomSetting_Dummy_QHBoxLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		PhantomSetting_Dummy_QLabel = new QLabel;
		PhantomSetting_Dummy_QLabel->setFixedSize(panel->Window_width * 0.090, panel->Window_height * 0.028);
		PhantomSetting_Dummy_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		PhantomSetting_Dummy_QLabel->setFont(panel->font_D_LBL1);
		PhantomSetting_Dummy_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		PhantomSetting_Dummy_QLabel->setText(QString::fromLocal8Bit("Dummy phantom:"));

		PhantomSetting_Dummy_QComboBox = new QComboBox;
		PhantomSetting_Dummy_QComboBox->setFixedSize(panel->Window_width * 0.170, panel->Window_height * 0.028);
		PhantomSetting_Dummy_QComboBox->setStyleSheet(DialogStyle::COMBOBOX);
		PhantomSetting_Dummy_QComboBox->setFont(panel->font_D_CB1);

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
		PhantomSetting_Gender_QHBoxLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.015, panel->Window_width * 0.003, 0);
		PhantomSetting_Gender_QHBoxLayout->setSpacing(panel->Window_width * 0.003);
		PhantomSetting_Gender_QHBoxLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		PhantomSetting_Gender_QLabel = new QLabel;
		PhantomSetting_Gender_QLabel->setFixedSize(panel->Window_width * 0.090, panel->Window_height * 0.028);
		PhantomSetting_Gender_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		PhantomSetting_Gender_QLabel->setFont(panel->font_D_LBL1);
		PhantomSetting_Gender_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		PhantomSetting_Gender_QLabel->setText(QString::fromLocal8Bit("Gender:"));

		PhantomSetting_Gender_QComboBox = new QComboBox;
		PhantomSetting_Gender_QComboBox->setFixedSize(panel->Window_width * 0.170, panel->Window_height * 0.028);
		PhantomSetting_Gender_QComboBox->setStyleSheet(DialogStyle::COMBOBOX);
		PhantomSetting_Gender_QComboBox->setFont(panel->font_D_CB1);

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
		PhantomSetting_Age_QHBoxLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.015, panel->Window_width * 0.003, 0);
		PhantomSetting_Age_QHBoxLayout->setSpacing(panel->Window_width * 0.003);
		PhantomSetting_Age_QHBoxLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		PhantomSetting_Age_QLabel = new QLabel;
		PhantomSetting_Age_QLabel->setFixedSize(panel->Window_width * 0.090, panel->Window_height * 0.028);
		PhantomSetting_Age_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		PhantomSetting_Age_QLabel->setFont(panel->font_D_LBL1);
		PhantomSetting_Age_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		PhantomSetting_Age_QLabel->setText(QString::fromLocal8Bit("Age:"));

		PhantomSetting_Age_QComboBox = new QComboBox;
		PhantomSetting_Age_QComboBox->setFixedSize(panel->Window_width * 0.170, panel->Window_height * 0.028);
		PhantomSetting_Age_QComboBox->setStyleSheet(DialogStyle::COMBOBOX);
		PhantomSetting_Age_QComboBox->setFont(panel->font_D_CB1);

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
		PhantomSetting_Posture_QHBoxLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.015, panel->Window_width * 0.003, 0);
		PhantomSetting_Posture_QHBoxLayout->setSpacing(panel->Window_width * 0.003);
		PhantomSetting_Posture_QHBoxLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		PhantomSetting_Posture_QLabel = new QLabel;
		PhantomSetting_Posture_QLabel->setFixedSize(panel->Window_width * 0.090, panel->Window_height * 0.028);
		PhantomSetting_Posture_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		PhantomSetting_Posture_QLabel->setFont(panel->font_D_LBL1);
		PhantomSetting_Posture_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		PhantomSetting_Posture_QLabel->setText(QString::fromLocal8Bit("Posture:"));

		PhantomSetting_Posture_QComboBox = new QComboBox;
		PhantomSetting_Posture_QComboBox->setFixedSize(panel->Window_width * 0.170, panel->Window_height * 0.028);
		PhantomSetting_Posture_QComboBox->setStyleSheet(DialogStyle::COMBOBOX);
		PhantomSetting_Posture_QComboBox->setFont(panel->font_D_CB1);

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
		PhantomSetting_FetalAge_QHBoxLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.015, panel->Window_width * 0.003, 0);
		PhantomSetting_FetalAge_QHBoxLayout->setSpacing(panel->Window_width * 0.003);
		PhantomSetting_FetalAge_QHBoxLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		PhantomSetting_FetalAge_QLabel = new QLabel;
		PhantomSetting_FetalAge_QLabel->setFixedSize(panel->Window_width * 0.090, panel->Window_height * 0.028);
		PhantomSetting_FetalAge_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		PhantomSetting_FetalAge_QLabel->setFont(panel->font_D_LBL1);
		PhantomSetting_FetalAge_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		PhantomSetting_FetalAge_QLabel->setText(QString::fromLocal8Bit("Fetal Age:"));

		PhantomSetting_FetalAge_QComboBox = new QComboBox;
		PhantomSetting_FetalAge_QComboBox->setFixedSize(panel->Window_width * 0.170, panel->Window_height * 0.028);
		PhantomSetting_FetalAge_QComboBox->setStyleSheet(DialogStyle::COMBOBOX);
		PhantomSetting_FetalAge_QComboBox->setFont(panel->font_D_CB1);

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
		PhantomSetting_Height_QHBoxLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.015, panel->Window_width * 0.003, 0);
		PhantomSetting_Height_QHBoxLayout->setSpacing(panel->Window_width * 0.003);
		PhantomSetting_Height_QHBoxLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		PhantomSetting_Height_QLabel = new QLabel;
		PhantomSetting_Height_QLabel->setFixedSize(panel->Window_width * 0.090, panel->Window_height * 0.028);
		PhantomSetting_Height_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		PhantomSetting_Height_QLabel->setFont(panel->font_D_LBL1);
		PhantomSetting_Height_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		PhantomSetting_Height_QLabel->setText(QString::fromLocal8Bit("Height:"));

		PhantomSetting_Height_QLineEdit = new QLineEdit;
		PhantomSetting_Height_QLineEdit->setFixedSize(panel->Window_width * 0.170, panel->Window_height * 0.028);
		PhantomSetting_Height_QLineEdit->setAlignment(Qt::AlignCenter);
		PhantomSetting_Height_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		PhantomSetting_Height_QLineEdit->setFont(panel->font_D_LE1);
		PhantomSetting_Height_QLineEdit->setPlaceholderText("cm");

		PhantomSetting_Height_QHBoxLayout->addWidget(PhantomSetting_Height_QLabel);
		PhantomSetting_Height_QHBoxLayout->addWidget(PhantomSetting_Height_QLineEdit);

		PhantomSetting_Height_QLineEdit->setDisabled(true);

		layout->addLayout(PhantomSetting_Height_QHBoxLayout);
	}

	{// Weight
		PhantomSetting_Weight_QHBoxLayout = new QHBoxLayout;
		PhantomSetting_Weight_QHBoxLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.015, panel->Window_width * 0.003, 0);
		PhantomSetting_Weight_QHBoxLayout->setSpacing(panel->Window_width * 0.003);
		PhantomSetting_Weight_QHBoxLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		PhantomSetting_Weight_QLabel = new QLabel;
		PhantomSetting_Weight_QLabel->setFixedSize(panel->Window_width * 0.090, panel->Window_height * 0.028);
		PhantomSetting_Weight_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		PhantomSetting_Weight_QLabel->setFont(panel->font_D_LBL1);
		PhantomSetting_Weight_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		PhantomSetting_Weight_QLabel->setText(QString::fromLocal8Bit("Weight:"));

		PhantomSetting_Weight_QLineEdit = new QLineEdit;
		PhantomSetting_Weight_QLineEdit->setFixedSize(panel->Window_width * 0.170, panel->Window_height * 0.028);
		PhantomSetting_Weight_QLineEdit->setAlignment(Qt::AlignCenter);
		PhantomSetting_Weight_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		PhantomSetting_Weight_QLineEdit->setFont(panel->font_D_LE1);
		PhantomSetting_Weight_QLineEdit->setPlaceholderText("kg");

		PhantomSetting_Weight_QHBoxLayout->addWidget(PhantomSetting_Weight_QLabel);
		PhantomSetting_Weight_QHBoxLayout->addWidget(PhantomSetting_Weight_QLineEdit);

		PhantomSetting_Weight_QLineEdit->setDisabled(true);

		layout->addLayout(PhantomSetting_Weight_QHBoxLayout);
	}

	{// Center
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.015, panel->Window_width * 0.003, 0);
		hLayout->setSpacing(panel->Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		PhantomSetting_Position_QLabel = new QLabel;
		PhantomSetting_Position_QLabel->setFixedSize(panel->Window_width * 0.090, panel->Window_height * 0.028);
		PhantomSetting_Position_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		PhantomSetting_Position_QLabel->setFont(panel->font_D_LBL1);
		PhantomSetting_Position_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		PhantomSetting_Position_QLabel->setText(QString::fromLocal8Bit("Center XYZ:"));

		PhantomSetting_PosX_QLineEdit = new QLineEdit;
		PhantomSetting_PosX_QLineEdit->setFixedSize(panel->Window_width * 0.0545, panel->Window_height * 0.028);
		PhantomSetting_PosX_QLineEdit->setAlignment(Qt::AlignCenter);
		PhantomSetting_PosX_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		PhantomSetting_PosX_QLineEdit->setFont(panel->font_D_LE1);
		PhantomSetting_PosX_QLineEdit->setPlaceholderText("cm");

		PhantomSetting_PosY_QLineEdit = new QLineEdit;
		PhantomSetting_PosY_QLineEdit->setFixedSize(panel->Window_width * 0.0545, panel->Window_height * 0.028);
		PhantomSetting_PosY_QLineEdit->setAlignment(Qt::AlignCenter);
		PhantomSetting_PosY_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		PhantomSetting_PosY_QLineEdit->setFont(panel->font_D_LE1);
		PhantomSetting_PosY_QLineEdit->setPlaceholderText("cm");

		PhantomSetting_PosZ_QLineEdit = new QLineEdit;
		PhantomSetting_PosZ_QLineEdit->setFixedSize(panel->Window_width * 0.0545, panel->Window_height * 0.028);
		PhantomSetting_PosZ_QLineEdit->setAlignment(Qt::AlignCenter);
		PhantomSetting_PosZ_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		PhantomSetting_PosZ_QLineEdit->setFont(panel->font_D_LE1);
		PhantomSetting_PosZ_QLineEdit->setPlaceholderText("cm");

		hLayout->addWidget(PhantomSetting_Position_QLabel);
		hLayout->addWidget(PhantomSetting_PosX_QLineEdit);
		hLayout->addWidget(PhantomSetting_PosY_QLineEdit);
		hLayout->addWidget(PhantomSetting_PosZ_QLineEdit);

		layout->addLayout(hLayout);
	}

	{// Rotation 
		PhantomSetting_Rot_QHBoxLayout = new QHBoxLayout;
		PhantomSetting_Rot_QHBoxLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.015, panel->Window_width * 0.003, 0);
		PhantomSetting_Rot_QHBoxLayout->setSpacing(panel->Window_width * 0.003);
		PhantomSetting_Rot_QHBoxLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		PhantomSetting_Rotation_QLabel = new QLabel;
		PhantomSetting_Rotation_QLabel->setFixedSize(panel->Window_width * 0.090, panel->Window_height * 0.028);
		PhantomSetting_Rotation_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		PhantomSetting_Rotation_QLabel->setFont(panel->font_D_LBL1);
		PhantomSetting_Rotation_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		PhantomSetting_Rotation_QLabel->setText(QString::fromLocal8Bit("Rotate XYZ:"));

		PhantomSetting_RotX_QLineEdit = new QLineEdit;
		PhantomSetting_RotX_QLineEdit->setFixedSize(panel->Window_width * 0.0545, panel->Window_height * 0.028);
		PhantomSetting_RotX_QLineEdit->setAlignment(Qt::AlignCenter);
		PhantomSetting_RotX_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		PhantomSetting_RotX_QLineEdit->setFont(panel->font_D_LE1);
		PhantomSetting_RotX_QLineEdit->setPlaceholderText("deg");

		PhantomSetting_RotY_QLineEdit = new QLineEdit;
		PhantomSetting_RotY_QLineEdit->setFixedSize(panel->Window_width * 0.0545, panel->Window_height * 0.028);
		PhantomSetting_RotY_QLineEdit->setAlignment(Qt::AlignCenter);
		PhantomSetting_RotY_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		PhantomSetting_RotY_QLineEdit->setFont(panel->font_D_LE1);
		PhantomSetting_RotY_QLineEdit->setPlaceholderText("deg");

		PhantomSetting_RotZ_QLineEdit = new QLineEdit;
		PhantomSetting_RotZ_QLineEdit->setFixedSize(panel->Window_width * 0.0545, panel->Window_height * 0.028);
		PhantomSetting_RotZ_QLineEdit->setAlignment(Qt::AlignCenter);
		PhantomSetting_RotZ_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		PhantomSetting_RotZ_QLineEdit->setFont(panel->font_D_LE1);
		PhantomSetting_RotZ_QLineEdit->setPlaceholderText("deg");

		PhantomSetting_Rot_QHBoxLayout->addWidget(PhantomSetting_Rotation_QLabel);
		PhantomSetting_Rot_QHBoxLayout->addWidget(PhantomSetting_RotX_QLineEdit);
		PhantomSetting_Rot_QHBoxLayout->addWidget(PhantomSetting_RotY_QLineEdit);
		PhantomSetting_Rot_QHBoxLayout->addWidget(PhantomSetting_RotZ_QLineEdit);

		layout->addLayout(PhantomSetting_Rot_QHBoxLayout);
	}

	{// OK Button
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(panel->Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		hLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.015, panel->Window_width * 0.018, 0);

		PhantomSetting_AddingOK_QPushButton = new QPushButton(QString::fromLocal8Bit("OK"));
		panel->Buttons_FunctionPanelRight.append(PhantomSetting_AddingOK_QPushButton);
		PhantomSetting_AddingOK_QPushButton->setFixedSize(panel->Window_width * 0.104, panel->Window_height * 0.028);
		PhantomSetting_AddingOK_QPushButton->setCheckable(false);
		PhantomSetting_AddingOK_QPushButton->setStyleSheet(DialogStyle::MENU_BTN3);
		PhantomSetting_AddingOK_QPushButton->setFont(panel->font_D_BTN3);

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
void PhantomWidget::slot_PhantomAddingOK_ButtonClicked()
{
	SavePhantom_InfoData_InAddingSetting(m_model.m_Phantom_MakingIndex); // Phantom info 컨테이너에 정보 저장	

	// AddingOK에 Setting widget 정상 종료를 위해 반드시 필요함
	m_model.m_Is_PhantomSetting_OKbutton_Clicked = true;	
	PhantomSetting_UserQDialog->close();
}
void PhantomWidget::slot_PhantomSetting_ComputationlPhantom_RadioButtonClicked()
{
	updateModelFromUI();
	PhantomSetting_ComputationlPhantom_QRadioButton->setChecked(true);
	PhantomSetting_AirPhantom_QRadioButton->setChecked(false);

	// Label, buttons show
	PhantomSetting_Name_QLineEdit->setPlaceholderText("Phantom_" + QString::number(m_model.m_Phantom_MakingIndex));

	PhantomSetting_Type_QHBoxLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.015, panel->Window_width * 0.003, 0);
	PhantomSetting_Type_QHBoxLayout->setSpacing(panel->Window_width * 0.003);
	PhantomSetting_Type_QLabel->show();
	PhantomSetting_Type_QComboBox->show();

	PhantomSetting_Dummy_QHBoxLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.015, panel->Window_width * 0.003, 0);
	PhantomSetting_Dummy_QHBoxLayout->setSpacing(panel->Window_width * 0.003);
	PhantomSetting_Dummy_QLabel->show();
	PhantomSetting_Dummy_QComboBox->show();

	PhantomSetting_Gender_QHBoxLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.015, panel->Window_width * 0.003, 0);
	PhantomSetting_Gender_QHBoxLayout->setSpacing(panel->Window_width * 0.003);
	PhantomSetting_Gender_QLabel->show();
	PhantomSetting_Gender_QComboBox->show();

	PhantomSetting_Age_QHBoxLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.015, panel->Window_width * 0.003, 0);
	PhantomSetting_Age_QHBoxLayout->setSpacing(panel->Window_width * 0.003);
	PhantomSetting_Age_QLabel->show();
	PhantomSetting_Age_QComboBox->show();

	PhantomSetting_Posture_QHBoxLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.015, panel->Window_width * 0.003, 0);
	PhantomSetting_Posture_QHBoxLayout->setSpacing(panel->Window_width * 0.003);
	PhantomSetting_Posture_QLabel->show();
	PhantomSetting_Posture_QComboBox->show();

	PhantomSetting_Height_QHBoxLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.015, panel->Window_width * 0.003, 0);
	PhantomSetting_Height_QHBoxLayout->setSpacing(panel->Window_width * 0.003);
	PhantomSetting_Height_QLabel->show();
	PhantomSetting_Height_QLineEdit->show();

	PhantomSetting_Weight_QHBoxLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.015, panel->Window_width * 0.003, 0);
	PhantomSetting_Weight_QHBoxLayout->setSpacing(panel->Window_width * 0.003);
	PhantomSetting_Weight_QLabel->show();
	PhantomSetting_Weight_QLineEdit->show();

	PhantomSetting_Rot_QHBoxLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.015, panel->Window_width * 0.003, 0);
	PhantomSetting_Rot_QHBoxLayout->setSpacing(panel->Window_width * 0.003);
	PhantomSetting_Rotation_QLabel->show();
	PhantomSetting_RotX_QLineEdit->show();
	PhantomSetting_RotY_QLineEdit->show();
	PhantomSetting_RotZ_QLineEdit->show();

	// Phantom setting 창크기 조절
	PhantomSetting_UserQDialog->setFixedWidth(panel->Window_width * 0.3);
	PhantomSetting_UserQDialog->setFixedHeight(panel->Window_height * 0.7);
}
void PhantomWidget::slot_PhantomSetting_AirPhantom_RadioButtonClicked()
{
	PhantomSetting_ComputationlPhantom_QRadioButton->setChecked(false);
	PhantomSetting_AirPhantom_QRadioButton->setChecked(true);

	// Label, buttons show	
	PhantomSetting_Name_QLineEdit->setPlaceholderText("AirSphere_" + QString::number(m_model.m_Phantom_MakingIndex));

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
	PhantomSetting_UserQDialog->setFixedWidth(panel->Window_width * 0.3);
	PhantomSetting_UserQDialog->setFixedHeight(panel->Window_height * 0.250);
}
void PhantomWidget::slot_PhantomSetting_Import_ButtonClicked()
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
void PhantomWidget::slot_ChangePhantomTypeCombo_Setting(int idx)
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
void PhantomWidget::slot_ChangePhantomDummyCombo_Setting(int idx)
{
	PhantomSetting_Type_QComboBox->hide();	PhantomSetting_Type_QComboBox->show(); // This is for focusing (Do not show white blank)
	PhantomSetting_Dummy_QComboBox->hide();	PhantomSetting_Dummy_QComboBox->show(); // This is for focusing (Do not show white blank)
	PhantomSetting_Gender_QComboBox->hide(); PhantomSetting_Gender_QComboBox->show(); // This is for focusing (Do not show white blank)
	PhantomSetting_Age_QComboBox->hide();	PhantomSetting_Age_QComboBox->show(); // This is for focusing (Do not show white blank)
	PhantomSetting_Posture_QComboBox->hide();	PhantomSetting_Posture_QComboBox->show(); // This is for focusing (Do not show white blank)
	PhantomSetting_FetalAge_QComboBox->hide();	PhantomSetting_FetalAge_QComboBox->show(); // This is for focusing (Do not show white blank)
}
void PhantomWidget::slot_ChangePhantomGenderCombo_Setting(int idx)
{
	PhantomSetting_Type_QComboBox->hide();	PhantomSetting_Type_QComboBox->show(); // This is for focusing (Do not show white blank)
	PhantomSetting_Dummy_QComboBox->hide();	PhantomSetting_Dummy_QComboBox->show(); // This is for focusing (Do not show white blank)
	PhantomSetting_Gender_QComboBox->hide(); PhantomSetting_Gender_QComboBox->show(); // This is for focusing (Do not show white blank)
	PhantomSetting_Age_QComboBox->hide();	PhantomSetting_Age_QComboBox->show(); // This is for focusing (Do not show white blank)
	PhantomSetting_Posture_QComboBox->hide();	PhantomSetting_Posture_QComboBox->show(); // This is for focusing (Do not show white blank)
	PhantomSetting_FetalAge_QComboBox->hide();	PhantomSetting_FetalAge_QComboBox->show(); // This is for focusing (Do not show white blank)
}
void PhantomWidget::slot_ChangePhantomAgeCombo_Setting(int idx)
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
void PhantomWidget::slot_ChangePhantomPostureCombo_Setting(int idx)
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
void PhantomWidget::slot_ChangePhantomFetalAgeCombo_Setting(int idx)
{
	PhantomSetting_Type_QComboBox->hide();	PhantomSetting_Type_QComboBox->show(); // This is for focusing (Do not show white blank)
	PhantomSetting_Dummy_QComboBox->hide();	PhantomSetting_Dummy_QComboBox->show(); // This is for focusing (Do not show white blank)
	PhantomSetting_Gender_QComboBox->hide(); PhantomSetting_Gender_QComboBox->show(); // This is for focusing (Do not show white blank)
	PhantomSetting_Age_QComboBox->hide();	PhantomSetting_Age_QComboBox->show(); // This is for focusing (Do not show white blank)
	PhantomSetting_Posture_QComboBox->hide();	PhantomSetting_Posture_QComboBox->show(); // This is for focusing (Do not show white blank)
	PhantomSetting_FetalAge_QComboBox->hide();	PhantomSetting_FetalAge_QComboBox->show(); // This is for focusing (Do not show white blank)
}

// Main function - Info Status
void PhantomWidget::slot_DummyPhantomCheckBoxClicked()
{
	updateModelFromUI();
	int id = m_model.m_Phantom_SelectedIndex;

	// 현재 선택 index가 invalid면 false로 돌리고 return
	if (id < 0)
	{
		PhantomDummy_QCheckBox->setChecked(false);
		return;
	}
	else if (m_model.m_Phantom_MainInfo[id][E_PHANTOMMAININFO_CATEGORY] == E_PHANTOMCATEGORY_AIR
		|| m_model.m_Phantom_MainInfo[id][E_PHANTOMMAININFO_TYPE] == E_PHANTOMTYPE_IMPORTED) // 에어팬텀이거나 imported 팬텀이면 false로 돌리고 return
	{
		PhantomDummy_QCheckBox->setChecked(false);
		return;
	}

	if (PhantomDummy_QCheckBox->isChecked() == true)
	{
		m_model.m_Phantom_MainInfo[id][E_PHANTOMMAININFO_DUMMY] = E_PHANTOMDUMMY_YES;

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
		m_model.m_Phantom_MainInfo[id][E_PHANTOMMAININFO_DUMMY] = E_PHANTOMDUMMY_NO;

		theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(theApp.PhantomPanelActor[id]);
		theApp.PhantomPanelActor[id] = theApp.FullPhantomActor[id];
		// Translate actor
		theApp.PhantomPanelActor[id]->SetPosition(PhantomPosX_QLineEdit->text().toDouble(), PhantomPosY_QLineEdit->text().toDouble(), PhantomPosZ_QLineEdit->text().toDouble());
		theApp.PhantomPanelActor[id]->SetOrientation(PhantomRotX_QLineEdit->text().toDouble(), PhantomRotY_QLineEdit->text().toDouble(), PhantomRotZ_QLineEdit->text().toDouble());
		theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(theApp.PhantomPanelActor[id]);
		theApp.m_pVTKWidget->renderWindow()->Render();
	}
}
void PhantomWidget::slot_PhantomUpdate_ButtonClicked()
{
	updateModelFromUI();
	if (m_model.m_Phantom_SequenceVector.size() == 0)
	{
		theApp.SetMessageBox("Phantom should be added first.");
		return;
	}
	// Phantom Pos/Rot update
	SavePhantom_InfoData(m_model.m_Phantom_SelectedIndex);

	theApp.PhantomPanelActor[m_model.m_Phantom_SelectedIndex]->SetPosition(m_model.m_Phantom_MainInfo[m_model.m_Phantom_SelectedIndex][6], m_model.m_Phantom_MainInfo[m_model.m_Phantom_SelectedIndex][7], m_model.m_Phantom_MainInfo[m_model.m_Phantom_SelectedIndex][8]);
	theApp.PhantomPanelActor[m_model.m_Phantom_SelectedIndex]->SetOrientation(m_model.m_Phantom_MainInfo[m_model.m_Phantom_SelectedIndex][9], m_model.m_Phantom_MainInfo[m_model.m_Phantom_SelectedIndex][10], m_model.m_Phantom_MainInfo[m_model.m_Phantom_SelectedIndex][11]);
	
	theApp.m_pVTKWidget->renderWindow()->Render();
}

// Sub-function
void PhantomWidget::moveMaximumScrollbar_phantom() // called by (1) slot_PhantomAdd_ButtonClicked
{
	updateModelFromUI();
	timer_scroll_multiplePhantom->stop();
	PhantomList_Horizontal_QScrollBar->setValue(PhantomList_Horizontal_QScrollBar->maximum());
	PhantomList_Vertical_QScrollBar->setValue(0);
}
void PhantomWidget::SavePhantom_InfoData_InAddingSetting(int phantomIndex)
{
	updateModelFromUI();
	// Phantom category
	if (PhantomSetting_AirPhantom_QRadioButton->isChecked() == false) m_model.m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_CATEGORY] = E_PHANTOMCATEGORY_HUMAN;
	else if (PhantomSetting_AirPhantom_QRadioButton->isChecked() == true)
	{
		if (PhantomSetting_Name_QLineEdit->text().isEmpty()) m_model.m_Phantom_ButtonName[phantomIndex] = "AirSphere_" + QString::number(phantomIndex);
		else m_model.m_Phantom_ButtonName[phantomIndex] = PhantomSetting_Name_QLineEdit->text();

		m_model.m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_CATEGORY] = E_PHANTOMCATEGORY_AIR;
		m_model.m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_POS_X] = PhantomSetting_PosX_QLineEdit->text().toDouble(); // posX
		m_model.m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_POS_X] = PhantomSetting_PosY_QLineEdit->text().toDouble(); // posY
		m_model.m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_POS_X] = PhantomSetting_PosZ_QLineEdit->text().toDouble(); // posZ
		m_model.m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_DUMMY] = E_PHANTOMDUMMY_NO;
	}

	// Phantom name
	if (PhantomSetting_Name_QLineEdit->text().isEmpty()) m_model.m_Phantom_ButtonName[phantomIndex] = "Phantom_" + QString::number(phantomIndex);
	else m_model.m_Phantom_ButtonName[phantomIndex] = PhantomSetting_Name_QLineEdit->text();
	
	m_model.m_Phantom_MainInfo[phantomIndex][1] = PhantomSetting_Gender_QComboBox->currentIndex(); // Gender
	m_model.m_Phantom_MainInfo[phantomIndex][2] = PhantomSetting_Type_QComboBox->currentIndex(); // Type	
	if (PhantomSetting_Type_QComboBox->currentIndex() == E_PHANTOMTYPE_IMPORTED) theApp.m_ImportedPhantomFilePath_NoExtention[phantomIndex] = PhantomSetting_ImportedFileName_QLineEdit->text(); // Imported phantom file name
	m_model.m_Phantom_MainInfo[phantomIndex][3] = PhantomSetting_Posture_QComboBox->currentIndex(); // Posture

	// Height/Weight
	if (PhantomSetting_Type_QComboBox->currentIndex() == E_PHANTOMTYPE_ADULTMRCP) // Adult MRCP or Adult Posture-dependent-phantom
	{
		if (PhantomSetting_Gender_QComboBox->currentIndex() == E_PHANTOMGENDER_MALE) { m_model.m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_HEIGHT] = 176; m_model.m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_WEIGHT] = 73; } // adult male
		if (PhantomSetting_Gender_QComboBox->currentIndex() == E_PHANTOMGENDER_FEMALE) { m_model.m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_HEIGHT] = 163; m_model.m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_WEIGHT] = 60; } // adult female
	}
	else if (PhantomSetting_Type_QComboBox->currentIndex() == E_PHANTOMTYPE_PEDIATRICMRCP) // Pediatric MRCP
	{
		if (PhantomSetting_Gender_QComboBox->currentIndex() == E_PHANTOMGENDER_MALE && PhantomSetting_Age_QComboBox->currentIndex() == E_PHANTOMAGE_15y) { m_model.m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_HEIGHT] = 167; m_model.m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_WEIGHT] = 56; } // 15y male
		if (PhantomSetting_Gender_QComboBox->currentIndex() == E_PHANTOMGENDER_MALE && PhantomSetting_Age_QComboBox->currentIndex() == E_PHANTOMAGE_10y) { m_model.m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_HEIGHT] = 138; m_model.m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_WEIGHT] = 32; } // 10y male
		if (PhantomSetting_Gender_QComboBox->currentIndex() == E_PHANTOMGENDER_MALE && PhantomSetting_Age_QComboBox->currentIndex() == E_PHANTOMAGE_5y) { m_model.m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_HEIGHT] = 109; m_model.m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_WEIGHT] = 19; } // 05y male
		if (PhantomSetting_Gender_QComboBox->currentIndex() == E_PHANTOMGENDER_MALE && PhantomSetting_Age_QComboBox->currentIndex() == E_PHANTOMAGE_1y) { m_model.m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_HEIGHT] = 76; m_model.m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_WEIGHT] = 10; } // 01y male
		if (PhantomSetting_Gender_QComboBox->currentIndex() == E_PHANTOMGENDER_MALE && PhantomSetting_Age_QComboBox->currentIndex() == E_PHANTOMAGE_0y) { m_model.m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_HEIGHT] = 51; m_model.m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_WEIGHT] = 3.5; } // 00y male

		if (PhantomSetting_Gender_QComboBox->currentIndex() == E_PHANTOMGENDER_FEMALE && PhantomSetting_Age_QComboBox->currentIndex() == E_PHANTOMAGE_15y) { m_model.m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_HEIGHT] = 161; m_model.m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_WEIGHT] = 53; } // 15y female
		if (PhantomSetting_Gender_QComboBox->currentIndex() == E_PHANTOMGENDER_FEMALE && PhantomSetting_Age_QComboBox->currentIndex() == E_PHANTOMAGE_10y) { m_model.m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_HEIGHT] = 138; m_model.m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_WEIGHT] = 32; } // 10y female
		if (PhantomSetting_Gender_QComboBox->currentIndex() == E_PHANTOMGENDER_FEMALE && PhantomSetting_Age_QComboBox->currentIndex() == E_PHANTOMAGE_5y)  { m_model.m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_HEIGHT] = 109; m_model.m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_WEIGHT] = 19; } // 05y female
		if (PhantomSetting_Gender_QComboBox->currentIndex() == E_PHANTOMGENDER_FEMALE && PhantomSetting_Age_QComboBox->currentIndex() == E_PHANTOMAGE_1y)  { m_model.m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_HEIGHT] = 76; m_model.m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_WEIGHT] = 10; } // 01y female
		if (PhantomSetting_Gender_QComboBox->currentIndex() == E_PHANTOMGENDER_FEMALE && PhantomSetting_Age_QComboBox->currentIndex() == E_PHANTOMAGE_0y)  { m_model.m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_HEIGHT] = 51; m_model.m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_WEIGHT] = 3.5; } // 00y female
	}
	else if (PhantomSetting_Type_QComboBox->currentIndex() == E_PHANTOMTYPE_PFMRCP) // PFMRCP
	{
		m_model.m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_HEIGHT] = 163; m_model.m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_WEIGHT] = 60;
	}
	else if (PhantomSetting_Type_QComboBox->currentIndex() == E_PHANTOMTYPE_TRANSFORMED && PhantomSetting_Posture_QComboBox->currentIndex() != E_PHANTOMPOSTURE_STANDING) // Adult posture-dependent-phantom
	{
		if (PhantomSetting_Gender_QComboBox->currentIndex() == E_PHANTOMGENDER_MALE && PhantomSetting_Age_QComboBox->currentIndex() == E_PHANTOMAGE_ADULT) { m_model.m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_HEIGHT] = 176; m_model.m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_WEIGHT] = 73; } // adult male
		if (PhantomSetting_Gender_QComboBox->currentIndex() == E_PHANTOMGENDER_FEMALE && PhantomSetting_Age_QComboBox->currentIndex() == E_PHANTOMAGE_ADULT) { m_model.m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_HEIGHT] = 163; m_model.m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_WEIGHT] = 60; } // adult female
	}
	else if (PhantomSetting_Type_QComboBox->currentIndex() == E_PHANTOMTYPE_TRANSFORMED && PhantomSetting_Posture_QComboBox->currentIndex() == E_PHANTOMPOSTURE_STANDING) //Body-size-dependent phantom
	{
		m_model.m_Phantom_MainInfo[phantomIndex][4] = PhantomSetting_Height_QLineEdit->text().toDouble(); // Height
		m_model.m_Phantom_MainInfo[phantomIndex][5] = PhantomSetting_Weight_QLineEdit->text().toDouble(); // Weight
	}
	if (PhantomSetting_Type_QComboBox->currentIndex() == E_PHANTOMTYPE_IMPORTED) //Imported phantom
	{
		m_model.m_Phantom_MainInfo[phantomIndex][4] = -1; // Invalid Height
		m_model.m_Phantom_MainInfo[phantomIndex][5] = -1; // Invalid Weight
	}

	m_model.m_Phantom_MainInfo[phantomIndex][6] = PhantomSetting_PosX_QLineEdit->text().toDouble(); // posX
	m_model.m_Phantom_MainInfo[phantomIndex][7] = PhantomSetting_PosY_QLineEdit->text().toDouble(); // posY
	m_model.m_Phantom_MainInfo[phantomIndex][8] = PhantomSetting_PosZ_QLineEdit->text().toDouble(); // posZ
	m_model.m_Phantom_MainInfo[phantomIndex][9] = PhantomSetting_RotX_QLineEdit->text().toDouble(); // rotX
	m_model.m_Phantom_MainInfo[phantomIndex][10] = PhantomSetting_RotY_QLineEdit->text().toDouble(); // rotY
	m_model.m_Phantom_MainInfo[phantomIndex][11] = PhantomSetting_RotZ_QLineEdit->text().toDouble(); // rotZ

	m_model.m_Phantom_MainInfo[phantomIndex][12] = PhantomSetting_Age_QComboBox->currentIndex(); // Age
	m_model.m_Phantom_MainInfo[phantomIndex][14] = PhantomSetting_FetalAge_QComboBox->currentIndex(); // Fetal Age

	// Dummy phantom
	if (PhantomSetting_Dummy_QComboBox->currentIndex() == 0) m_model.m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_DUMMY] = E_PHANTOMDUMMY_YES;
	if (PhantomSetting_Dummy_QComboBox->currentIndex() == 1) m_model.m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_DUMMY] = E_PHANTOMDUMMY_NO;
}
void PhantomWidget::SavePhantom_InfoData(int phantomIndex)
{
	updateModelFromUI();
	m_model.m_Phantom_MainInfo[phantomIndex][6] = PhantomPosX_QLineEdit->text().toDouble(); // posX
	m_model.m_Phantom_MainInfo[phantomIndex][7] = PhantomPosY_QLineEdit->text().toDouble(); // posY
	m_model.m_Phantom_MainInfo[phantomIndex][8] = PhantomPosZ_QLineEdit->text().toDouble(); // posZ
	m_model.m_Phantom_MainInfo[phantomIndex][9] = PhantomRotX_QLineEdit->text().toDouble(); // rotX
	m_model.m_Phantom_MainInfo[phantomIndex][10] = PhantomRotY_QLineEdit->text().toDouble(); // rotY
	m_model.m_Phantom_MainInfo[phantomIndex][11] = PhantomRotZ_QLineEdit->text().toDouble(); // rotZ
}
void PhantomWidget::SavePhantom_InfoData_InMouseControl(int phantomIndex)
{
	updateModelFromUI();
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

	m_model.m_Phantom_MainInfo[phantomIndex][6] = TransformPos[0];
	m_model.m_Phantom_MainInfo[phantomIndex][7] = TransformPos[1];
	m_model.m_Phantom_MainInfo[phantomIndex][8] = TransformPos[2];
	m_model.m_Phantom_MainInfo[phantomIndex][9] = TransformRot[0];
	m_model.m_Phantom_MainInfo[phantomIndex][10] = TransformRot[1];
	m_model.m_Phantom_MainInfo[phantomIndex][11] = TransformRot[2];
}
void PhantomWidget::UpdatePhantom_ListButton(int phantomIndex)
{
	updateModelFromUI();
	for (auto itr_phantom : m_model.m_Phantom_SequenceVector) // 최대 팬텀 개수
	{
		if (itr_phantom != m_model.m_Phantom_SelectedIndex)
		{
			PhantomSelect_QPushButton[itr_phantom]->setChecked(false);
		}
	}
	if (phantomIndex >= 0) PhantomSelect_QPushButton[phantomIndex]->setChecked(true);
}
void PhantomWidget::UpdatePhantom_InfoStatus(int phantomIndex)
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

	if (m_model.m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_CATEGORY] == E_PHANTOMCATEGORY_AIR) // Air phantom 일 때
	{
		PhantomGender_QLineEdit->clear();
		PhantomType_QLineEdit->clear();
		PhantomPosture_QLineEdit->clear();
		PhantomAge_QLineEdit->clear();
		PhantomFetalAge_QLineEdit->clear();
		PhantomHeight_QLineEdit->clear();
		PhantomWeight_QLineEdit->clear();

		// Pos, Rot
		PhantomPosX_QLineEdit->setText(QString::number(m_model.m_Phantom_MainInfo[phantomIndex][6]));
		PhantomPosY_QLineEdit->setText(QString::number(m_model.m_Phantom_MainInfo[phantomIndex][7]));
		PhantomPosZ_QLineEdit->setText(QString::number(m_model.m_Phantom_MainInfo[phantomIndex][8]));
		PhantomRotX_QLineEdit->setText(QString::number(m_model.m_Phantom_MainInfo[phantomIndex][9]));
		PhantomRotY_QLineEdit->setText(QString::number(m_model.m_Phantom_MainInfo[phantomIndex][10]));
		PhantomRotZ_QLineEdit->setText(QString::number(m_model.m_Phantom_MainInfo[phantomIndex][11]));

		return;
	}

	// Gender
	if (m_model.m_Phantom_MainInfo[phantomIndex][1] == 0) { PhantomGender_QLineEdit->setText("Male"); }
	if (m_model.m_Phantom_MainInfo[phantomIndex][1] == 1) { PhantomGender_QLineEdit->setText("Female"); }

	// Phantom Type
	if (m_model.m_Phantom_MainInfo[phantomIndex][2] == E_PHANTOMTYPE_ADULTMRCP) { PhantomType_QLineEdit->setText("Adult MRCP"); }
	if (m_model.m_Phantom_MainInfo[phantomIndex][2] == E_PHANTOMTYPE_PEDIATRICMRCP) { PhantomType_QLineEdit->setText("Pediatric MRCP"); }
	if (m_model.m_Phantom_MainInfo[phantomIndex][2] == E_PHANTOMTYPE_PFMRCP) { PhantomType_QLineEdit->setText("pregnant-female MRCP"); }
	if (m_model.m_Phantom_MainInfo[phantomIndex][2] == E_PHANTOMTYPE_TRANSFORMED) { PhantomType_QLineEdit->setText("Transformed phantom"); }
	if (m_model.m_Phantom_MainInfo[phantomIndex][2] == E_PHANTOMTYPE_IMPORTED) { PhantomType_QLineEdit->setText("Imported phantom"); }

	// Posture
	if (m_model.m_Phantom_MainInfo[phantomIndex][3] == 0) { PhantomPosture_QLineEdit->setText("Standing"); }
	if (m_model.m_Phantom_MainInfo[phantomIndex][3] == 1) { PhantomPosture_QLineEdit->setText("Bending"); }
	if (m_model.m_Phantom_MainInfo[phantomIndex][3] == 2) { PhantomPosture_QLineEdit->setText("Kneeling"); }
	if (m_model.m_Phantom_MainInfo[phantomIndex][3] == 3) { PhantomPosture_QLineEdit->setText("Sitting"); }
	if (m_model.m_Phantom_MainInfo[phantomIndex][3] == 4) { PhantomPosture_QLineEdit->setText("Squatting"); }
	if (m_model.m_Phantom_MainInfo[phantomIndex][3] == 5) { PhantomPosture_QLineEdit->setText("Walking"); }
	if (m_model.m_Phantom_MainInfo[phantomIndex][2] == E_PHANTOMTYPE_IMPORTED) { PhantomPosture_QLineEdit->setText(" "); }

	// Height
	PhantomHeight_QLineEdit->setText(QString::number(m_model.m_Phantom_MainInfo[phantomIndex][4]));
	if (m_model.m_Phantom_MainInfo[phantomIndex][2] == E_PHANTOMTYPE_IMPORTED) PhantomHeight_QLineEdit->setText(" ");

	// Weight
	PhantomWeight_QLineEdit->setText(QString::number(m_model.m_Phantom_MainInfo[phantomIndex][5]));
	if (m_model.m_Phantom_MainInfo[phantomIndex][2] == E_PHANTOMTYPE_IMPORTED) PhantomWeight_QLineEdit->setText(" ");

	// Pos, Rot
	PhantomPosX_QLineEdit->setText(QString::number(m_model.m_Phantom_MainInfo[phantomIndex][6]));
	PhantomPosY_QLineEdit->setText(QString::number(m_model.m_Phantom_MainInfo[phantomIndex][7]));
	PhantomPosZ_QLineEdit->setText(QString::number(m_model.m_Phantom_MainInfo[phantomIndex][8]));
	PhantomRotX_QLineEdit->setText(QString::number(m_model.m_Phantom_MainInfo[phantomIndex][9]));
	PhantomRotY_QLineEdit->setText(QString::number(m_model.m_Phantom_MainInfo[phantomIndex][10]));
	PhantomRotZ_QLineEdit->setText(QString::number(m_model.m_Phantom_MainInfo[phantomIndex][11]));

	// Age
	if (m_model.m_Phantom_MainInfo[phantomIndex][12] == 0) { PhantomAge_QLineEdit->setText("Adult"); }
	if (m_model.m_Phantom_MainInfo[phantomIndex][12] == 1) { PhantomAge_QLineEdit->setText("15y"); }
	if (m_model.m_Phantom_MainInfo[phantomIndex][12] == 2) { PhantomAge_QLineEdit->setText("10y"); }
	if (m_model.m_Phantom_MainInfo[phantomIndex][12] == 3) { PhantomAge_QLineEdit->setText("5y"); }
	if (m_model.m_Phantom_MainInfo[phantomIndex][12] == 4) { PhantomAge_QLineEdit->setText("1y"); }
	if (m_model.m_Phantom_MainInfo[phantomIndex][12] == 5) { PhantomAge_QLineEdit->setText("0y"); }

	// Fetal Age
	if (m_model.m_Phantom_MainInfo[phantomIndex][14] == 0) { PhantomFetalAge_QLineEdit->setText("08w"); }
	if (m_model.m_Phantom_MainInfo[phantomIndex][14] == 1) { PhantomFetalAge_QLineEdit->setText("10w"); }
	if (m_model.m_Phantom_MainInfo[phantomIndex][14] == 2) { PhantomFetalAge_QLineEdit->setText("15w"); }
	if (m_model.m_Phantom_MainInfo[phantomIndex][14] == 3) { PhantomFetalAge_QLineEdit->setText("20w"); }
	if (m_model.m_Phantom_MainInfo[phantomIndex][14] == 4) { PhantomFetalAge_QLineEdit->setText("25w"); }
	if (m_model.m_Phantom_MainInfo[phantomIndex][14] == 5) { PhantomFetalAge_QLineEdit->setText("30w"); }
	if (m_model.m_Phantom_MainInfo[phantomIndex][14] == 6) { PhantomFetalAge_QLineEdit->setText("35w"); }
	if (m_model.m_Phantom_MainInfo[phantomIndex][14] == 7) { PhantomFetalAge_QLineEdit->setText("38w"); }

	// Dummy phantom
	if (m_model.m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_DUMMY] == E_PHANTOMDUMMY_YES) PhantomDummy_QCheckBox->setChecked(true);
	if (m_model.m_Phantom_MainInfo[phantomIndex][E_PHANTOMMAININFO_DUMMY] == E_PHANTOMDUMMY_NO) PhantomDummy_QCheckBox->setChecked(false);

}

void PhantomWidget::RemoveClothingPanel() // called by (1) slot_PhantomSelect_ButtonClicked, (2) slot_PhantomAdd_ButtonClicked, (3) slot_PhantomDelete_ButtonClicked
{
	updateModelFromUI();
	// Remove clothing list
	for (auto itr_clothing : m_model.m_Clothing_SequenceVector[m_model.m_Phantom_SelectedIndex])
	{
		ClothingList_QHBoxLayout->removeWidget(ClothingSelect_QPushButton[itr_clothing]);
		panel->Buttons_FunctionPanelRight.removeOne(ClothingSelect_QPushButton[itr_clothing]);
		delete ClothingSelect_QPushButton[itr_clothing];
		ClothingSelect_QPushButton.erase(itr_clothing);
	}
	// Remove clothing layer list
	for (auto itr_layer : m_model.m_ClothingLayer_SequenceVector[m_model.m_Phantom_SelectedIndex][m_model.m_Clothing_SelectedIndex])
	{
		ClothingLayerList_QHBoxLayout->removeWidget(ClothingLayerSelect_QPushButton[itr_layer]);
		panel->Buttons_FunctionPanelRight.removeOne(ClothingLayerSelect_QPushButton[itr_layer]);
		delete ClothingLayerSelect_QPushButton[itr_layer];
		ClothingLayerSelect_QPushButton.erase(itr_layer);
	}
	// Set panel info
	if (m_model.m_Clothing_SelectedIndex >= 0) SetClothingPanelInfo(m_model.m_Clothing_SelectedIndex, m_model.m_ClothingLayer_SelectedIndex); // Clothing 존재할 때
	else
	{
		ClothingThickness_QLineEdit->setText(""); // thickness
		ClothingDensity_QLineEdit->setText(""); // density		
		ClothingComposition_QComboBox->setCurrentIndex(0);// Composition
	}

	m_model.m_Clothing_SelectedIndex = -1;
	m_model.m_Clothing_MakingIndex = 0;
	m_model.m_ClothingLayer_SelectedIndex = -1;
	m_model.m_ClothingLayer_MakingIndex = 0;
}
void PhantomWidget::SetClothingPanel_inPhantomDelete() // called by (1) slot_PhantomDelete_ButtonClicked
{
	// Find clothing index
	if (m_model.m_Clothing_SequenceVector[m_model.m_Phantom_SelectedIndex].size() == 0) m_model.m_Clothing_SelectedIndex = -1;
	else m_model.m_Clothing_SelectedIndex = m_model.m_Clothing_SequenceVector[m_model.m_Phantom_SelectedIndex][0];// First index
	m_model.m_Clothing_MakingIndex = -1; // Biggest is last index, find the biggest element in SequenceVector
	for (auto itr : m_model.m_Clothing_SequenceVector[m_model.m_Phantom_SelectedIndex])
	{
		if (itr > m_model.m_Clothing_MakingIndex) m_model.m_Clothing_MakingIndex = itr;
	}
	m_model.m_Clothing_MakingIndex++; // 가장 큰 원소보다 1 더 큰 값

	// Generate clothing list
	for (auto itr : m_model.m_Clothing_SequenceVector[m_model.m_Phantom_SelectedIndex])
	{
		ClothingSelect_QPushButton[itr] = new QPushButton(m_model.m_Clothing_ButtonName[m_model.m_Phantom_SelectedIndex][itr]); // 이름 	
		panel->Buttons_FunctionPanelRight.append(ClothingSelect_QPushButton[itr]);
		ClothingSelect_QPushButton[itr]->setObjectName(QString::number(itr));
		ClothingSelect_QPushButton[itr]->setFixedSize(panel->Window_width * 0.05, panel->Window_height * 0.040);
		ClothingSelect_QPushButton[itr]->setStyleSheet(DialogStyle::MENU_BTN7);
		ClothingSelect_QPushButton[itr]->setFont(panel->font_D_BTN14);
		ClothingSelect_QPushButton[itr]->setCheckable(true);
		ClothingSelect_QPushButton[itr]->setChecked(true);
		connect(ClothingSelect_QPushButton[itr], SIGNAL(clicked()), this, SLOT(slot_ClothingSelect_ButtonClicked())); connect(ClothingSelect_QPushButton[itr], SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
		ClothingList_QHBoxLayout->addWidget(ClothingSelect_QPushButton[itr]);
	}
	// Set checked clothing button
	for (auto itr : m_model.m_Clothing_SequenceVector[m_model.m_Phantom_SelectedIndex])
	{
		if (itr != m_model.m_Clothing_SelectedIndex)
		{
			ClothingSelect_QPushButton[itr]->setChecked(false);
		}
		else ClothingSelect_QPushButton[itr]->setChecked(true);
	}

	// Clothing layer

	// Find clothing layer index
	if (m_model.m_ClothingLayer_SequenceVector[m_model.m_Phantom_SelectedIndex][m_model.m_Clothing_SelectedIndex].size() == 0) m_model.m_ClothingLayer_SelectedIndex = -1;
	else m_model.m_ClothingLayer_SelectedIndex = m_model.m_ClothingLayer_SequenceVector[m_model.m_Phantom_SelectedIndex][m_model.m_Clothing_SelectedIndex][0];// First index
	m_model.m_ClothingLayer_MakingIndex = -1; // Biggest is last index, find the biggest element in m_model.m_ClothingLayer_SequenceVector
	for (auto itr : m_model.m_ClothingLayer_SequenceVector[m_model.m_Phantom_SelectedIndex][m_model.m_Clothing_SelectedIndex])
	{
		if (itr > m_model.m_ClothingLayer_MakingIndex) m_model.m_ClothingLayer_MakingIndex = itr;
	}
	m_model.m_ClothingLayer_MakingIndex++; // 가장 큰 원소보다 1 더 큰 값


	// Generate clothing layer list
	for (auto itr : m_model.m_ClothingLayer_SequenceVector[m_model.m_Phantom_SelectedIndex][m_model.m_Clothing_SelectedIndex])
	{		
		ClothingLayerSelect_QPushButton[itr] = new QPushButton(m_model.m_ClothingLayer_ButtonName[m_model.m_Phantom_SelectedIndex][m_model.m_Clothing_SelectedIndex][itr]); // 이름 		
		panel->Buttons_FunctionPanelRight.append(ClothingLayerSelect_QPushButton[itr]);
		ClothingLayerSelect_QPushButton[itr]->setObjectName(QString::number(itr));
		ClothingLayerSelect_QPushButton[itr]->setFixedSize(panel->Window_width * 0.04, panel->Window_height * 0.025);
		ClothingLayerSelect_QPushButton[itr]->setStyleSheet(DialogStyle::MENU_BTN7);
		ClothingLayerSelect_QPushButton[itr]->setFont(panel->font_D_BTN14);
		ClothingLayerSelect_QPushButton[itr]->setCheckable(true);
		ClothingLayerSelect_QPushButton[itr]->setChecked(true);
		connect(ClothingLayerSelect_QPushButton[itr], SIGNAL(clicked()), this, SLOT(slot_ClothingLayerSelect_ButtonClicked())); connect(ClothingLayerSelect_QPushButton[itr], SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
		ClothingLayerList_QHBoxLayout->addWidget(ClothingLayerSelect_QPushButton[itr]);
	}
	// Set checked clothing layer button
	for (auto itr : m_model.m_ClothingLayer_SequenceVector[m_model.m_Phantom_SelectedIndex][m_model.m_Clothing_SelectedIndex])
	{
		if (itr != m_model.m_ClothingLayer_SelectedIndex)
		{
			ClothingLayerSelect_QPushButton[itr]->setChecked(false);
		}
		else ClothingLayerSelect_QPushButton[itr]->setChecked(true);
	}

	// Set panel info
	if (m_model.m_Clothing_SelectedIndex >= 0) SetClothingPanelInfo(m_model.m_Clothing_SelectedIndex, m_model.m_ClothingLayer_SelectedIndex); // Clothing 존재할 때
	else
	{
		ClothingThickness_QLineEdit->setText(""); // thickness
		ClothingDensity_QLineEdit->setText(""); // density		
		ClothingComposition_QComboBox->setCurrentIndex(0);// Composition
	}
}
void PhantomWidget::SetClothingPanel_inPhantomSelect() // called by (1) slot_PhantomSelect_ButtonClicked
{
	updateModelFromUI();
	// Find clothing index
	if (m_model.m_Clothing_SequenceVector[m_model.m_Phantom_SelectedIndex].size() == 0) m_model.m_Clothing_SelectedIndex = -1;
	else m_model.m_Clothing_SelectedIndex = m_model.m_Clothing_SequenceVector[m_model.m_Phantom_SelectedIndex][0];// First index
	m_model.m_Clothing_MakingIndex = -1; // Biggest is last index, find the biggest element in SequenceVector
	for (auto itr : m_model.m_Clothing_SequenceVector[m_model.m_Phantom_SelectedIndex])
	{
		if (itr > m_model.m_Clothing_MakingIndex) m_model.m_Clothing_MakingIndex = itr;
	}
	m_model.m_Clothing_MakingIndex++; // 가장 큰 원소보다 1 더 큰 값	

	// Generate clothing list
	for (auto itr : m_model.m_Clothing_SequenceVector[m_model.m_Phantom_SelectedIndex])
	{
		ClothingSelect_QPushButton[itr] = new QPushButton(m_model.m_Clothing_ButtonName[m_model.m_Phantom_SelectedIndex][itr]); // 이름 	
		panel->Buttons_FunctionPanelRight.append(ClothingSelect_QPushButton[itr]);
		ClothingSelect_QPushButton[itr]->setObjectName(QString::number(itr));
		ClothingSelect_QPushButton[itr]->setFixedSize(panel->Window_width * 0.05, panel->Window_height * 0.040);
		ClothingSelect_QPushButton[itr]->setStyleSheet(DialogStyle::MENU_BTN7);
		ClothingSelect_QPushButton[itr]->setFont(panel->font_D_BTN14);
		ClothingSelect_QPushButton[itr]->setCheckable(true);
		ClothingSelect_QPushButton[itr]->setChecked(true);
		connect(ClothingSelect_QPushButton[itr], SIGNAL(clicked()), this, SLOT(slot_ClothingSelect_ButtonClicked())); connect(ClothingSelect_QPushButton[itr], SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
		ClothingList_QHBoxLayout->addWidget(ClothingSelect_QPushButton[itr]);
	}
	// Set checked clothing button
	for (auto itr : m_model.m_Clothing_SequenceVector[m_model.m_Phantom_SelectedIndex])
	{
		if (itr != m_model.m_Clothing_SelectedIndex)
		{
			ClothingSelect_QPushButton[itr]->setChecked(false);
		}
		else ClothingSelect_QPushButton[itr]->setChecked(true);
	}

	// Clothing layer

	// Find clothing layer index
	if (m_model.m_ClothingLayer_SequenceVector[m_model.m_Phantom_SelectedIndex][m_model.m_Clothing_SelectedIndex].size() == 0) m_model.m_ClothingLayer_SelectedIndex = -1;
	else m_model.m_ClothingLayer_SelectedIndex = m_model.m_ClothingLayer_SequenceVector[m_model.m_Phantom_SelectedIndex][m_model.m_Clothing_SelectedIndex][0];// First index
	m_model.m_ClothingLayer_MakingIndex = -1; // Biggest is last index, find the biggest element in m_model.m_ClothingLayer_SequenceVector
	for (auto itr : m_model.m_ClothingLayer_SequenceVector[m_model.m_Phantom_SelectedIndex][m_model.m_Clothing_SelectedIndex])
	{
		if (itr > m_model.m_ClothingLayer_MakingIndex) m_model.m_ClothingLayer_MakingIndex = itr;
	}
	m_model.m_ClothingLayer_MakingIndex++; // 가장 큰 원소보다 1 더 큰 값


	// Generate clothing layer list
	for (auto itr : m_model.m_ClothingLayer_SequenceVector[m_model.m_Phantom_SelectedIndex][m_model.m_Clothing_SelectedIndex])
	{		
		ClothingLayerSelect_QPushButton[itr] = new QPushButton(m_model.m_ClothingLayer_ButtonName[m_model.m_Phantom_SelectedIndex][m_model.m_Clothing_SelectedIndex][itr]); // 이름 		
		panel->Buttons_FunctionPanelRight.append(ClothingLayerSelect_QPushButton[itr]);
		ClothingLayerSelect_QPushButton[itr]->setObjectName(QString::number(itr));
		ClothingLayerSelect_QPushButton[itr]->setFixedSize(panel->Window_width * 0.04, panel->Window_height * 0.025);
		ClothingLayerSelect_QPushButton[itr]->setStyleSheet(DialogStyle::MENU_BTN7);
		ClothingLayerSelect_QPushButton[itr]->setFont(panel->font_D_BTN14);
		ClothingLayerSelect_QPushButton[itr]->setCheckable(true);
		ClothingLayerSelect_QPushButton[itr]->setChecked(true);
		connect(ClothingLayerSelect_QPushButton[itr], SIGNAL(clicked()), this, SLOT(slot_ClothingLayerSelect_ButtonClicked())); connect(ClothingLayerSelect_QPushButton[itr], SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
		ClothingLayerList_QHBoxLayout->addWidget(ClothingLayerSelect_QPushButton[itr]);
	}
	// Set checked clothing layer button
	for (auto itr : m_model.m_ClothingLayer_SequenceVector[m_model.m_Phantom_SelectedIndex][m_model.m_Clothing_SelectedIndex])
	{
		if (itr != m_model.m_ClothingLayer_SelectedIndex)
		{
			ClothingLayerSelect_QPushButton[itr]->setChecked(false);
		}
		else ClothingLayerSelect_QPushButton[itr]->setChecked(true);
	}

	// Set panel info
	if (m_model.m_Clothing_SelectedIndex >= 0) SetClothingPanelInfo(m_model.m_Clothing_SelectedIndex, m_model.m_ClothingLayer_SelectedIndex); // Clothing 존재할 때
	else
	{
		ClothingThickness_QLineEdit->setText(""); // thickness
		ClothingDensity_QLineEdit->setText(""); // density		
		ClothingComposition_QComboBox->setCurrentIndex(0);// Composition
	}
}


////////////////////////// Clothing ////////////////////////// 

// Main function - List
void PhantomWidget::slot_ClothingAdd_ButtonClicked()
{
	updateModelFromUI();
	if (m_model.m_Clothing_SequenceVector.size() == PhantomConstants::const_Clothing_MaximumCount)
	{
		theApp.SetMessageBox("The maximum number of clothing has been reached.");
		return;
	}
	ClothingAddingSetting();
	if (m_model.m_Is_ClothingSetting_Closed == true) // X버튼으로 나갔을때
	{
		m_model.m_Is_ClothingSetting_Closed = false;
		return;
	}
}
void PhantomWidget::slot_ClothingDelete_ButtonClicked()
{
	if (m_model.m_Clothing_SequenceVector[m_model.m_Phantom_SelectedIndex].size() < 1)
	{
		theApp.SetMessageBox("There is no clothing to delete!");
		return;
	}
	// Clothing 버튼 지우기	
	int originalposition = theApp.removeElement(m_model.m_Clothing_SequenceVector[m_model.m_Phantom_SelectedIndex], m_model.m_Clothing_SelectedIndex); // SequenceVector의 원소 지우기: {0 1 2} -> {0 2}, origianl index는 지워진 1의 자리	
	ClothingList_QHBoxLayout->removeWidget(ClothingSelect_QPushButton[m_model.m_Clothing_SelectedIndex]);
	panel->Buttons_FunctionPanelRight.removeOne(ClothingSelect_QPushButton[m_model.m_Clothing_SelectedIndex]);
	delete ClothingSelect_QPushButton[m_model.m_Clothing_SelectedIndex];
	ClothingSelect_QPushButton.erase(m_model.m_Clothing_SelectedIndex);

	// Remove actor
	theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(theApp.PhantomPanelActor[m_model.m_Phantom_SelectedIndex]);
	theApp.m_pVTKWidget->renderWindow()->Render();
	theApp.PhantomPanelActor[m_model.m_Phantom_SelectedIndex] = nullptr;

	// Remove polydata
	theApp.PhantomPanelPolyData_Collection[m_model.m_Phantom_SelectedIndex][1][m_model.m_Clothing_SelectedIndex] = nullptr;

	// Appending and generate actor

	theApp.phantomObjects->AppendPhantomPolyData_GeneratePhantomActor(m_model.m_Phantom_SelectedIndex);

	// Layer 버튼 모두 지우기			
	for (auto itr_layer : m_model.m_ClothingLayer_SequenceVector[m_model.m_Phantom_SelectedIndex][m_model.m_Clothing_SelectedIndex])
	{
		ClothingLayerList_QHBoxLayout->removeWidget(ClothingLayerSelect_QPushButton[itr_layer]);
		panel->Buttons_FunctionPanelRight.removeOne(ClothingLayerSelect_QPushButton[itr_layer]);
		delete ClothingLayerSelect_QPushButton[itr_layer];
		ClothingLayerSelect_QPushButton.erase(itr_layer);
	}

	// clothing panel 업데이트	
	if (m_model.m_Clothing_SequenceVector[m_model.m_Phantom_SelectedIndex].size() == 0) // clothing 개수 0일때 현재 선택된 clothing index = -1(Invalid)
	{
		ClothingThickness_QLineEdit->setText(""); // thickness
		ClothingDensity_QLineEdit->setText(""); // density
		ClothingComposition_QComboBox->setCurrentIndex(0); // Composition

		m_model.m_Clothing_SelectedIndex = -1;
		m_model.m_ClothingLayer_SelectedIndex = -1;
		m_model.m_ClothingLayer_MakingIndex = 0;
	}
	else // Clothing 개수 남아있을때 
	{
		m_model.m_Clothing_SelectedIndex = m_model.m_Clothing_SequenceVector[m_model.m_Phantom_SelectedIndex][originalposition]; // 1번 index 자리(2번째 자리)가 지워짐 -> 새로 바뀐 벡터의 1번 index의 원소가 새로운 id
		// Find clothing layer index
		if (m_model.m_ClothingLayer_SequenceVector[m_model.m_Phantom_SelectedIndex][m_model.m_Clothing_SelectedIndex].size() == 0) m_model.m_ClothingLayer_SelectedIndex = -1;
		else m_model.m_ClothingLayer_SelectedIndex = m_model.m_ClothingLayer_SequenceVector[m_model.m_Phantom_SelectedIndex][m_model.m_Clothing_SelectedIndex][0];// First index		
		for (auto itr_layer : m_model.m_ClothingLayer_SequenceVector[m_model.m_Phantom_SelectedIndex][m_model.m_Clothing_SelectedIndex]) //Generate clothing layer button
		{
			// Generate clothing layer button
			ClothingLayerSelect_QPushButton[itr_layer] = new QPushButton(m_model.m_ClothingLayer_ButtonName[m_model.m_Phantom_SelectedIndex][m_model.m_Clothing_SelectedIndex][itr_layer]);
			panel->Buttons_FunctionPanelRight.append(ClothingLayerSelect_QPushButton[itr_layer]);
			ClothingLayerSelect_QPushButton[itr_layer]->setObjectName(QString::number(itr_layer));
			ClothingLayerSelect_QPushButton[itr_layer]->setFixedSize(panel->Window_width * 0.04, panel->Window_height * 0.025);
			ClothingLayerSelect_QPushButton[itr_layer]->setStyleSheet(DialogStyle::MENU_BTN7);
			ClothingLayerSelect_QPushButton[itr_layer]->setFont(panel->font_D_BTN14);
			ClothingLayerSelect_QPushButton[itr_layer]->setCheckable(true);
			ClothingLayerSelect_QPushButton[itr_layer]->setChecked(false);
			connect(ClothingLayerSelect_QPushButton[itr_layer], SIGNAL(clicked()), this, SLOT(slot_ClothingLayerSelect_ButtonClicked())); connect(ClothingLayerSelect_QPushButton[itr_layer], SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
			ClothingLayerList_QHBoxLayout->addWidget(ClothingLayerSelect_QPushButton[itr_layer]);
		}
		ClothingLayerSelect_QPushButton[m_model.m_ClothingLayer_SelectedIndex]->setChecked(true);
		SetClothingPanelInfo(m_model.m_Clothing_SelectedIndex, m_model.m_ClothingLayer_SelectedIndex); // First index로 panel info 업데이트		
		m_model.m_ClothingLayer_MakingIndex = -1; // Biggest is last index, find the biggest element in m_model.m_ClothingLayer_SequenceVector
		for (auto itr : m_model.m_ClothingLayer_SequenceVector[m_model.m_Phantom_SelectedIndex][m_model.m_Clothing_SelectedIndex])
		{
			if (itr > m_model.m_ClothingLayer_MakingIndex) m_model.m_ClothingLayer_MakingIndex = itr;
		}
		m_model.m_ClothingLayer_MakingIndex++; // 가장 큰 원소보다 1 더 큰 값
	}
	//  button check
	for (auto itr_clothing : m_model.m_Clothing_SequenceVector[m_model.m_Phantom_SelectedIndex])
	{
		if (itr_clothing != m_model.m_Clothing_SelectedIndex)
		{
			ClothingSelect_QPushButton[itr_clothing]->setChecked(false);
		}
	}
	if (m_model.m_Clothing_SelectedIndex >= 0) ClothingSelect_QPushButton[m_model.m_Clothing_SelectedIndex]->setChecked(true);
}
void PhantomWidget::slot_ClothingSelect_ButtonClicked()
{
	updateModelFromUI();
	// Get global variables 	
	QPushButton* buttonSender = qobject_cast<QPushButton*>(sender());

	// 원래 버튼 클릭 시 다시 check하고 돌아가기
	if (buttonSender->objectName().toInt() == m_model.m_Clothing_SelectedIndex)
	{
		ClothingSelect_QPushButton[m_model.m_Clothing_SelectedIndex]->setChecked(true);
		return;
	}

	// Remove clothing layer list
	for (auto itr_layer : m_model.m_ClothingLayer_SequenceVector[m_model.m_Phantom_SelectedIndex][m_model.m_Clothing_SelectedIndex])
	{
		ClothingLayerList_QHBoxLayout->removeWidget(ClothingLayerSelect_QPushButton[itr_layer]);
		panel->Buttons_FunctionPanelRight.removeOne(ClothingLayerSelect_QPushButton[itr_layer]);
		delete ClothingLayerSelect_QPushButton[itr_layer];
		ClothingLayerSelect_QPushButton.erase(itr_layer);
	}

	/////////////////////// Clothing index 변경///////////////////////////
	m_model.m_Clothing_SelectedIndex = buttonSender->objectName().toInt();
	int ClothingLayerTotalCount = m_model.m_ClothingLayer_SequenceVector[m_model.m_Phantom_SelectedIndex][m_model.m_Clothing_SelectedIndex].size();

	// Set clicked ClothingSelect_QPushButton
	for (auto itr : m_model.m_Clothing_SequenceVector[m_model.m_Phantom_SelectedIndex])
	{
		if (itr != m_model.m_Clothing_SelectedIndex)
		{
			ClothingSelect_QPushButton[itr]->setChecked(false);
		}
	}

	// Generate clothing layer list
	for (auto itr : m_model.m_ClothingLayer_SequenceVector[m_model.m_Phantom_SelectedIndex][m_model.m_Clothing_SelectedIndex])
	{
		ClothingLayerSelect_QPushButton[itr] = new QPushButton(m_model.m_ClothingLayer_ButtonName[m_model.m_Phantom_SelectedIndex][m_model.m_Clothing_SelectedIndex][itr]);
		panel->Buttons_FunctionPanelRight.append(ClothingLayerSelect_QPushButton[itr]);
		ClothingLayerSelect_QPushButton[itr]->setObjectName(QString::number(itr));
		ClothingLayerSelect_QPushButton[itr]->setFixedSize(panel->Window_width * 0.04, panel->Window_height * 0.025);
		ClothingLayerSelect_QPushButton[itr]->setStyleSheet(DialogStyle::MENU_BTN7);
		ClothingLayerSelect_QPushButton[itr]->setFont(panel->font_D_BTN14);
		ClothingLayerSelect_QPushButton[itr]->setCheckable(true);
		ClothingLayerSelect_QPushButton[itr]->setChecked(true);
		connect(ClothingLayerSelect_QPushButton[itr], SIGNAL(clicked()), this, SLOT(slot_ClothingLayerSelect_ButtonClicked())); connect(ClothingLayerSelect_QPushButton[itr], SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
		ClothingLayerList_QHBoxLayout->addWidget(ClothingLayerSelect_QPushButton[itr]);
	}

	// Find clothing layer index
	if (m_model.m_ClothingLayer_SequenceVector[m_model.m_Phantom_SelectedIndex][m_model.m_Clothing_SelectedIndex].size() == 0) m_model.m_ClothingLayer_SelectedIndex = -1;
	else m_model.m_ClothingLayer_SelectedIndex = m_model.m_ClothingLayer_SequenceVector[m_model.m_Phantom_SelectedIndex][m_model.m_Clothing_SelectedIndex][0];// First index
	m_model.m_ClothingLayer_MakingIndex = -1; // Biggest is last index, find the biggest element in m_model.m_ClothingLayer_SequenceVector
	for (auto itr : m_model.m_ClothingLayer_SequenceVector[m_model.m_Phantom_SelectedIndex][m_model.m_Clothing_SelectedIndex])
	{
		if (itr > m_model.m_ClothingLayer_MakingIndex) m_model.m_ClothingLayer_MakingIndex = itr;
	}
	m_model.m_ClothingLayer_MakingIndex++; // 가장 큰 원소보다 1 더 큰 값

	// Set clicked ClothingLayerSelect_QPushButton
	for (auto itr : m_model.m_ClothingLayer_SequenceVector[m_model.m_Phantom_SelectedIndex][m_model.m_Clothing_SelectedIndex])
	{
		if (itr == m_model.m_ClothingLayer_SequenceVector[m_model.m_Phantom_SelectedIndex][m_model.m_Clothing_SelectedIndex][m_model.m_ClothingLayer_SelectedIndex]) // 다른 clothing 버튼 클릭 시 layer는 첫번째로 고정
		{
			ClothingLayerSelect_QPushButton[itr]->setChecked(true);
		}
		else ClothingLayerSelect_QPushButton[itr]->setChecked(false);
	}
	SetClothingPanelInfo(m_model.m_Clothing_SelectedIndex, m_model.m_ClothingLayer_SelectedIndex); // Set clothing panel info -> 다른 clothing 버튼 클릭 시 layer는 0번으로 고정됨
}
void PhantomWidget::slot_ClothingLayerAdd_ButtonClicked()
{
	updateModelFromUI();
	if (m_model.m_ClothingLayer_SequenceVector[m_model.m_Phantom_SelectedIndex][m_model.m_Clothing_SelectedIndex].size() == PhantomConstants::const_ClothingLayer_MaximumCount)
	{
		theApp.SetMessageBox("The maximum number of clothing layer has been reached.");
		return;
	}
	ClothingLayerAddingSetting();
	if (m_model.m_Is_ClothingLayer_Setting_Closed == true) // X버튼으로 나갔을때
	{
		m_model.m_Is_ClothingLayer_Setting_Closed = false;
		return;
	}

	// Get global variables 
	m_model.m_ClothingLayer_SequenceVector[m_model.m_Phantom_SelectedIndex][m_model.m_Clothing_SelectedIndex].push_back(m_model.m_ClothingLayer_MakingIndex);
	//
	ClothingLayerSelect_QPushButton[m_model.m_ClothingLayer_MakingIndex] = new QPushButton(m_model.m_ClothingLayer_ButtonName[m_model.m_Phantom_SelectedIndex][m_model.m_Clothing_SelectedIndex][m_model.m_ClothingLayer_MakingIndex]); // 이름 		
	panel->Buttons_FunctionPanelRight.append(ClothingLayerSelect_QPushButton[m_model.m_ClothingLayer_MakingIndex]);
	ClothingLayerSelect_QPushButton[m_model.m_ClothingLayer_MakingIndex]->setObjectName(QString::number(m_model.m_ClothingLayer_MakingIndex));
	ClothingLayerSelect_QPushButton[m_model.m_ClothingLayer_MakingIndex]->setFixedSize(panel->Window_width * 0.04, panel->Window_height * 0.025);
	ClothingLayerSelect_QPushButton[m_model.m_ClothingLayer_MakingIndex]->setStyleSheet(DialogStyle::MENU_BTN7);
	ClothingLayerSelect_QPushButton[m_model.m_ClothingLayer_MakingIndex]->setFont(panel->font_D_BTN14);
	ClothingLayerSelect_QPushButton[m_model.m_ClothingLayer_MakingIndex]->setCheckable(true);
	ClothingLayerSelect_QPushButton[m_model.m_ClothingLayer_MakingIndex]->setChecked(true);
	connect(ClothingLayerSelect_QPushButton[m_model.m_ClothingLayer_MakingIndex], SIGNAL(clicked()), this, SLOT(slot_ClothingLayerSelect_ButtonClicked())); connect(ClothingLayerSelect_QPushButton[m_model.m_ClothingLayer_MakingIndex], SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
	ClothingLayerList_QHBoxLayout->addWidget(ClothingLayerSelect_QPushButton[m_model.m_ClothingLayer_MakingIndex]);

	for (auto itr : m_model.m_ClothingLayer_SequenceVector[m_model.m_Phantom_SelectedIndex][m_model.m_Clothing_SelectedIndex])
	{
		if (itr != m_model.m_ClothingLayer_MakingIndex)
		{
			ClothingLayerSelect_QPushButton[itr]->setChecked(false);
		}
	}
	//Dosimeter scorll bar max 이동
	timer_scroll_clothingLayer->start(1); // 1ms 이후 이동 -> 그냥 함수에 넣으면 multiplephantombutton 추가가 더 늦어서 최대로 안 감

	// Set global variables 
	m_model.m_ClothingLayer_SelectedIndex = m_model.m_ClothingLayer_MakingIndex;
	m_model.m_ClothingLayer_MakingIndex++;
}
void PhantomWidget::slot_ClothingLayerDelete_ButtonClicked()
{
	if (m_model.m_ClothingLayer_SequenceVector[m_model.m_Phantom_SelectedIndex][m_model.m_Clothing_SelectedIndex].size() <= 1) //
	{
		theApp.SetMessageBox("At least one layer of clothing needed. To remove clothing, click Delete button of clothing.");
		return;
	}

	// 버튼 지우기	
	int originalposition = theApp.removeElement(m_model.m_ClothingLayer_SequenceVector[m_model.m_Phantom_SelectedIndex][m_model.m_Clothing_SelectedIndex], m_model.m_ClothingLayer_SelectedIndex); //{0 1 2} -> {0 2}, origianl index는 지워진 1의 자리	
	ClothingLayerList_QHBoxLayout->removeWidget(ClothingLayerSelect_QPushButton[m_model.m_ClothingLayer_SelectedIndex]);
	panel->Buttons_FunctionPanelRight.removeOne(ClothingLayerSelect_QPushButton[m_model.m_ClothingLayer_SelectedIndex]);
	delete ClothingLayerSelect_QPushButton[m_model.m_ClothingLayer_SelectedIndex];
	ClothingLayerSelect_QPushButton.erase(m_model.m_ClothingLayer_SelectedIndex);

	// clothingPanel 업데이트
	m_model.m_ClothingLayer_SelectedIndex = m_model.m_ClothingLayer_SequenceVector[m_model.m_Phantom_SelectedIndex][m_model.m_Clothing_SelectedIndex][originalposition]; //1번 index 자리(2번째 자리)가 지워짐 -> 새로 바뀐 벡터의 1번 index의 원소가 새로운 id
	SetClothingPanelInfo(m_model.m_Clothing_SelectedIndex, m_model.m_ClothingLayer_SelectedIndex);

	// button check
	for (auto itr_clothing : m_model.m_ClothingLayer_SequenceVector[m_model.m_Phantom_SelectedIndex][m_model.m_Clothing_SelectedIndex])
	{
		if (itr_clothing != m_model.m_ClothingLayer_SelectedIndex)
		{
			ClothingLayerSelect_QPushButton[itr_clothing]->setChecked(false);
		}
	}
	if (m_model.m_ClothingLayer_SelectedIndex >= 0) ClothingLayerSelect_QPushButton[m_model.m_ClothingLayer_SelectedIndex]->setChecked(true);
}
void PhantomWidget::slot_ClothingLayerSelect_ButtonClicked()
{
	updateModelFromUI();
	QPushButton* buttonSender = qobject_cast<QPushButton*>(sender());
	// 원래 버튼 클릭 시 다시 check하고 돌아가기
	if (buttonSender->objectName().toInt() == m_model.m_ClothingLayer_SelectedIndex)
	{
		ClothingLayerSelect_QPushButton[m_model.m_ClothingLayer_SelectedIndex]->setChecked(true);
		return;
	}

	m_model.m_ClothingLayer_SelectedIndex = buttonSender->objectName().toInt();
	for (auto itr : m_model.m_ClothingLayer_SequenceVector[m_model.m_Phantom_SelectedIndex][m_model.m_Clothing_SelectedIndex])
	{
		if (itr != m_model.m_ClothingLayer_SelectedIndex)
		{
			ClothingLayerSelect_QPushButton[itr]->setChecked(false);
		}
		else ClothingLayerSelect_QPushButton[itr]->setChecked(true);
	}

	SetClothingPanelInfo(m_model.m_Clothing_SelectedIndex, m_model.m_ClothingLayer_SelectedIndex); // Set clothing panel info
}

// Main function - Setting
void PhantomWidget::ClothingAddingSetting()
{
	updateModelFromUI();
	m_model.m_Is_ClothingSetting_OKbutton_Clicked = false;

	// Create a new dialog
	ClothingSetting_UserQDialog = new MultipleUIDialog(this);
	ClothingSetting_UserQDialog->setStyleSheet("background-color: rgb(205, 222, 243);");
	ClothingSetting_UserQDialog->setFixedWidth(panel->Window_width * 0.3);
	ClothingSetting_UserQDialog->setFixedHeight(panel->Window_height * 0.35);
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
		hLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.015, panel->Window_width * 0.003, 0);
		hLayout->setSpacing(panel->Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QLabel* label0 = new QLabel;
		label0->setFixedSize(panel->Window_width * 0.090, panel->Window_height * 0.028);
		label0->setStyleSheet(DialogStyle::DATA_LABEL);
		label0->setFont(panel->font_D_LBL1);
		label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		label0->setText(QString::fromLocal8Bit("Type:"));

		ClothingSetting_Type_QComboBox = new QComboBox;
		ClothingSetting_Type_QComboBox->setFixedSize(panel->Window_width * 0.105, panel->Window_height * 0.028);
		ClothingSetting_Type_QComboBox->setStyleSheet(DialogStyle::COMBOBOX);
		ClothingSetting_Type_QComboBox->setFont(panel->font_D_CB1);

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
		hLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.015, panel->Window_width * 0.003, 0);
		hLayout->setSpacing(panel->Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QLabel* label0 = new QLabel;
		label0->setFixedSize(panel->Window_width * 0.090, panel->Window_height * 0.028);
		label0->setStyleSheet(DialogStyle::DATA_LABEL);
		label0->setFont(panel->font_D_LBL1);
		label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		label0->setText(QString::fromLocal8Bit("Clothing name:"));

		ClothingSetting_Name_QLineEdit = new QLineEdit;
		ClothingSetting_Name_QLineEdit->setFixedSize(panel->Window_width * 0.170, panel->Window_height * 0.028);
		ClothingSetting_Name_QLineEdit->setAlignment(Qt::AlignCenter);
		ClothingSetting_Name_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		ClothingSetting_Name_QLineEdit->setFont(panel->font_D_LE1);
		ClothingSetting_Name_QLineEdit->setPlaceholderText("Clothing_" + QString::number(m_model.m_Clothing_MakingIndex));

		hLayout->addWidget(label0);
		hLayout->addWidget(ClothingSetting_Name_QLineEdit);

		layout->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.015, panel->Window_width * 0.003, 0);
		hLayout->setSpacing(panel->Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QLabel* label0 = new QLabel;
		label0->setFixedSize(panel->Window_width * 0.090, panel->Window_height * 0.028);
		label0->setStyleSheet(DialogStyle::DATA_LABEL);
		label0->setFont(panel->font_D_LBL1);
		label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		label0->setText(QString::fromLocal8Bit("Layer"));

		hLayout->addWidget(label0);

		layout->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setContentsMargins(panel->Window_width * 0.053, panel->Window_height * 0.005, panel->Window_width * 0.003, 0);
		hLayout->setSpacing(panel->Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QLabel* label0 = new QLabel;
		label0->setFixedSize(panel->Window_width * 0.080, panel->Window_height * 0.028);
		label0->setStyleSheet(DialogStyle::DATA_LABEL);
		label0->setFont(panel->font_D_LBL1);
		label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		label0->setText(QString::fromLocal8Bit("Layer name:"));

		ClothingSetting_Layername_QLineEdit = new QLineEdit;
		ClothingSetting_Layername_QLineEdit->setFixedSize(panel->Window_width * 0.120, panel->Window_height * 0.028);
		ClothingSetting_Layername_QLineEdit->setAlignment(Qt::AlignCenter);
		ClothingSetting_Layername_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		ClothingSetting_Layername_QLineEdit->setFont(panel->font_D_LE1);
		ClothingSetting_Layername_QLineEdit->setPlaceholderText("Layer_0"); // Clothing을 새로 제작할 때는 항상 0번 레이어		

		hLayout->addWidget(label0);
		hLayout->addWidget(ClothingSetting_Layername_QLineEdit);

		layout->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setContentsMargins(panel->Window_width * 0.053, panel->Window_height * 0.005, panel->Window_width * 0.003, 0);
		hLayout->setSpacing(panel->Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QLabel* label0 = new QLabel;
		label0->setFixedSize(panel->Window_width * 0.080, panel->Window_height * 0.028);
		label0->setStyleSheet(DialogStyle::DATA_LABEL);
		label0->setFont(panel->font_D_LBL1);
		label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		label0->setText(QString::fromLocal8Bit("Thickness:"));

		ClothingSetting_Thickness_QLineEdit = new QLineEdit;
		ClothingSetting_Thickness_QLineEdit->setFixedSize(panel->Window_width * 0.120, panel->Window_height * 0.028);
		ClothingSetting_Thickness_QLineEdit->setAlignment(Qt::AlignCenter);
		ClothingSetting_Thickness_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		ClothingSetting_Thickness_QLineEdit->setFont(panel->font_D_LE1);
		ClothingSetting_Thickness_QLineEdit->setPlaceholderText("cm");

		hLayout->addWidget(label0);
		hLayout->addWidget(ClothingSetting_Thickness_QLineEdit);

		layout->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setContentsMargins(panel->Window_width * 0.053, panel->Window_height * 0.005, panel->Window_width * 0.003, 0);
		hLayout->setSpacing(panel->Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QLabel* label0 = new QLabel;
		label0->setFixedSize(panel->Window_width * 0.080, panel->Window_height * 0.028);
		label0->setStyleSheet(DialogStyle::DATA_LABEL);
		label0->setFont(panel->font_D_LBL1);
		label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		label0->setText(QString::fromLocal8Bit("Density:"));

		ClothingSetting_Density_QLineEdit = new QLineEdit;
		ClothingSetting_Density_QLineEdit->setFixedSize(panel->Window_width * 0.120, panel->Window_height * 0.028);
		ClothingSetting_Density_QLineEdit->setAlignment(Qt::AlignCenter);
		ClothingSetting_Density_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		ClothingSetting_Density_QLineEdit->setFont(panel->font_D_LE1);
		ClothingSetting_Density_QLineEdit->setPlaceholderText("g/cm3");

		hLayout->addWidget(label0);
		hLayout->addWidget(ClothingSetting_Density_QLineEdit);

		layout->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setContentsMargins(panel->Window_width * 0.053, panel->Window_height * 0.005, panel->Window_width * 0.003, 0);
		hLayout->setSpacing(panel->Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QLabel* label0 = new QLabel;
		label0->setFixedSize(panel->Window_width * 0.080, panel->Window_height * 0.028);
		label0->setStyleSheet(DialogStyle::DATA_LABEL);
		label0->setFont(panel->font_D_LBL1);
		label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		label0->setText(QString::fromLocal8Bit("Composition:"));

		ClothingSetting_Composition_QComboBox = new QComboBox;
		ClothingSetting_Composition_QComboBox->setFixedSize(panel->Window_width * 0.120, panel->Window_height * 0.028);
		ClothingSetting_Composition_QComboBox->setStyleSheet(DialogStyle::COMBOBOX);
		ClothingSetting_Composition_QComboBox->setFont(panel->font_D_CB1);

		ClothingSetting_Composition_QComboBox->setEditable(true);
		ClothingSetting_Composition_QComboBox->lineEdit()->setReadOnly(true);
		ClothingSetting_Composition_QComboBox->lineEdit()->setAlignment(Qt::AlignCenter);
		ClothingSetting_Composition_QComboBox->lineEdit()->setFont(panel->font_D_CB1);

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
		hLayout->setSpacing(panel->Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		hLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.015, panel->Window_width * 0.003, 0);

		ClothingSetting_AddingOK_QPushButton = new QPushButton(QString::fromLocal8Bit("OK"));
		panel->Buttons_FunctionPanelRight.append(ClothingSetting_AddingOK_QPushButton);
		ClothingSetting_AddingOK_QPushButton->setFixedSize(panel->Window_width * 0.104, panel->Window_height * 0.028);
		ClothingSetting_AddingOK_QPushButton->setCheckable(false);
		ClothingSetting_AddingOK_QPushButton->setStyleSheet(DialogStyle::MENU_BTN3);
		ClothingSetting_AddingOK_QPushButton->setFont(panel->font_D_BTN3);

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
void PhantomWidget::slot_ChangeClothingSettingTypeCombo(int idx) // called by (1) ClothingAddingSetting
{
	updateModelFromUI();
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
void PhantomWidget::slot_ClothingAddingOK_ButtonClicked()
{
	updateModelFromUI();
	if (m_model.m_Phantom_SelectedIndex == -1)
	{
		theApp.SetMessageBox("Phantom is not generated!");
		return;
	}

	int SelectedClothingTypeIndex = ClothingSetting_Type_QComboBox->currentIndex();

	if (SelectedClothingTypeIndex != 0) // Pre-defined 의복을 입력할 때
	{
		for (auto itr_clothingIndex : m_model.m_Clothing_SequenceVector[m_model.m_Phantom_SelectedIndex])
		{
			if (SelectedClothingTypeIndex == m_model.m_Clothing_Type[m_model.m_Phantom_SelectedIndex][itr_clothingIndex])
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
		panel->AllButtonDisabled();
		// 이후, ETInteractorStyleRubberBand가 활성화되어, 완료 후 PhantomClothingGenerate 실행됨.
	}

	m_model.m_Is_ClothingSetting_OKbutton_Clicked = true; // OK 버튼이 클릭됨
	ClothingSetting_UserQDialog->close();
}
void PhantomWidget::ClothingLayerAddingSetting()
{
	updateModelFromUI();
	m_model.m_Is_ClothingLayer_Setting_OKbutton_Clicked = false;

	// Create a new dialog
	ClothingLayerSetting_UserQDialog = new MultipleUIDialog(this);
	ClothingLayerSetting_UserQDialog->setStyleSheet("background-color: rgb(205, 222, 243);");
	ClothingLayerSetting_UserQDialog->setFixedWidth(panel->Window_width * 0.3);
	ClothingLayerSetting_UserQDialog->setFixedHeight(panel->Window_height * 0.25);
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
		hLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.015, panel->Window_width * 0.003, 0);
		hLayout->setSpacing(panel->Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QLabel* label0 = new QLabel;
		label0->setFixedSize(panel->Window_width * 0.090, panel->Window_height * 0.028);
		label0->setStyleSheet(DialogStyle::DATA_LABEL);
		label0->setFont(panel->font_D_LBL1);
		label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		label0->setText(QString::fromLocal8Bit("Layer name:"));

		ClothingLayerSetting_Layername_QLineEdit = new QLineEdit;
		ClothingLayerSetting_Layername_QLineEdit->setFixedSize(panel->Window_width * 0.120, panel->Window_height * 0.028);
		ClothingLayerSetting_Layername_QLineEdit->setAlignment(Qt::AlignCenter);
		ClothingLayerSetting_Layername_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		ClothingLayerSetting_Layername_QLineEdit->setFont(panel->font_D_LE1);
		ClothingLayerSetting_Layername_QLineEdit->setPlaceholderText("Layer_" + QString::number(m_model.m_ClothingLayer_MakingIndex));		

		hLayout->addWidget(label0);
		hLayout->addWidget(ClothingLayerSetting_Layername_QLineEdit);

		layout->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.015, panel->Window_width * 0.003, 0);
		hLayout->setSpacing(panel->Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QLabel* label0 = new QLabel;
		label0->setFixedSize(panel->Window_width * 0.090, panel->Window_height * 0.028);
		label0->setStyleSheet(DialogStyle::DATA_LABEL);
		label0->setFont(panel->font_D_LBL1);
		label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		label0->setText(QString::fromLocal8Bit("Thickness:"));

		ClothingLayerSetting_Thickness_QLineEdit = new QLineEdit;
		ClothingLayerSetting_Thickness_QLineEdit->setFixedSize(panel->Window_width * 0.120, panel->Window_height * 0.028);
		ClothingLayerSetting_Thickness_QLineEdit->setAlignment(Qt::AlignCenter);
		ClothingLayerSetting_Thickness_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		ClothingLayerSetting_Thickness_QLineEdit->setFont(panel->font_D_LE1);
		ClothingLayerSetting_Thickness_QLineEdit->setPlaceholderText("cm");

		hLayout->addWidget(label0);
		hLayout->addWidget(ClothingLayerSetting_Thickness_QLineEdit);

		layout->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.015, panel->Window_width * 0.003, 0);
		hLayout->setSpacing(panel->Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QLabel* label0 = new QLabel;
		label0->setFixedSize(panel->Window_width * 0.090, panel->Window_height * 0.028);
		label0->setStyleSheet(DialogStyle::DATA_LABEL);
		label0->setFont(panel->font_D_LBL1);
		label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		label0->setText(QString::fromLocal8Bit("Density:"));

		ClothingLayerSetting_Density_QLineEdit = new QLineEdit;
		ClothingLayerSetting_Density_QLineEdit->setFixedSize(panel->Window_width * 0.120, panel->Window_height * 0.028);
		ClothingLayerSetting_Density_QLineEdit->setAlignment(Qt::AlignCenter);
		ClothingLayerSetting_Density_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		ClothingLayerSetting_Density_QLineEdit->setFont(panel->font_D_LE1);
		ClothingLayerSetting_Density_QLineEdit->setPlaceholderText("g/cm3");

		hLayout->addWidget(label0);
		hLayout->addWidget(ClothingLayerSetting_Density_QLineEdit);

		layout->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.015, panel->Window_width * 0.003, 0);
		hLayout->setSpacing(panel->Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QLabel* label0 = new QLabel;
		label0->setFixedSize(panel->Window_width * 0.090, panel->Window_height * 0.028);
		label0->setStyleSheet(DialogStyle::DATA_LABEL);
		label0->setFont(panel->font_D_LBL1);
		label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		label0->setText(QString::fromLocal8Bit("Composition:"));

		ClothingLayerSetting_Composition_QComboBox = new QComboBox;
		ClothingLayerSetting_Composition_QComboBox->setFixedSize(panel->Window_width * 0.120, panel->Window_height * 0.028);
		ClothingLayerSetting_Composition_QComboBox->setStyleSheet(DialogStyle::COMBOBOX);
		ClothingLayerSetting_Composition_QComboBox->setFont(panel->font_D_CB1);

		ClothingLayerSetting_Composition_QComboBox->setEditable(true);
		ClothingLayerSetting_Composition_QComboBox->lineEdit()->setReadOnly(true);
		ClothingLayerSetting_Composition_QComboBox->lineEdit()->setAlignment(Qt::AlignCenter);
		ClothingLayerSetting_Composition_QComboBox->lineEdit()->setFont(panel->font_D_CB1);

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
		hLayout->setSpacing(panel->Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		hLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.015, panel->Window_width * 0.003, 0);

		ClothingLayerSetting_AddingOK_QPushButton = new QPushButton(QString::fromLocal8Bit("OK"));
		panel->Buttons_FunctionPanelRight.append(ClothingLayerSetting_AddingOK_QPushButton);
		ClothingLayerSetting_AddingOK_QPushButton->setFixedSize(panel->Window_width * 0.104, panel->Window_height * 0.028);
		ClothingLayerSetting_AddingOK_QPushButton->setCheckable(false);
		ClothingLayerSetting_AddingOK_QPushButton->setStyleSheet(DialogStyle::MENU_BTN3);
		ClothingLayerSetting_AddingOK_QPushButton->setFont(panel->font_D_BTN3);

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
void PhantomWidget::slot_ClothingLayerAddingOK_ButtonClicked()
{
	updateModelFromUI();
	SaveClothingInformation_InLayerGenerate();
	SetClothingPanelInfo(m_model.m_Clothing_SelectedIndex, m_model.m_ClothingLayer_MakingIndex);

	m_model.m_Is_ClothingLayer_Setting_OKbutton_Clicked = true; // OK 버튼이 클릭됨
	ClothingLayerSetting_UserQDialog->close();
}

// Main function - Status
void PhantomWidget::slot_ClothingLayerUpdate_ButtonClicked()
{
	updateModelFromUI();
	m_model.m_Clothing_MainInfo[m_model.m_Phantom_SelectedIndex][m_model.m_Clothing_SelectedIndex][m_model.m_ClothingLayer_SelectedIndex][0] = ClothingThickness_QLineEdit->text().toDouble();
	m_model.m_Clothing_MainInfo[m_model.m_Phantom_SelectedIndex][m_model.m_Clothing_SelectedIndex][m_model.m_ClothingLayer_SelectedIndex][1] = ClothingDensity_QLineEdit->text().toDouble();
	m_model.m_Clothing_MainInfo[m_model.m_Phantom_SelectedIndex][m_model.m_Clothing_SelectedIndex][m_model.m_ClothingLayer_SelectedIndex][2] = ClothingComposition_QComboBox->currentIndex();
}

// Sub-function (1)
void PhantomWidget::SaveClothingInformation_InClothingGenerate() // called by (1) ETHuman3DApp::PhantomClothingGenerate
{
	// Clothing name
	if (ClothingSetting_Name_QLineEdit->text().isEmpty()) m_model.m_Clothing_ButtonName[m_model.m_Phantom_SelectedIndex][m_model.m_Clothing_MakingIndex] = "Clothing_" + QString::number(m_model.m_Clothing_MakingIndex);
	else m_model.m_Clothing_ButtonName[m_model.m_Phantom_SelectedIndex][m_model.m_Clothing_MakingIndex] = ClothingSetting_Name_QLineEdit->text();

	// Clothing Layer name
	if (ClothingSetting_Layername_QLineEdit->text().isEmpty()) m_model.m_ClothingLayer_ButtonName[m_model.m_Phantom_SelectedIndex][m_model.m_Clothing_MakingIndex][0] = "Layer_0";
	else m_model.m_ClothingLayer_ButtonName[m_model.m_Phantom_SelectedIndex][m_model.m_Clothing_MakingIndex][0] = ClothingSetting_Layername_QLineEdit->text();

	m_model.m_Clothing_Type[m_model.m_Phantom_SelectedIndex][m_model.m_Clothing_MakingIndex] = ClothingSetting_Type_QComboBox->currentIndex(); // Type

	// ClothingLayerIndex is always 0 (First layer of clothing)
	m_model.m_Clothing_MainInfo[m_model.m_Phantom_SelectedIndex][m_model.m_Clothing_MakingIndex][0][-1] = 1; // 사용여부
	m_model.m_Clothing_MainInfo[m_model.m_Phantom_SelectedIndex][m_model.m_Clothing_MakingIndex][0][0] = ClothingSetting_Thickness_QLineEdit->text().toDouble();  // thickness
	m_model.m_Clothing_MainInfo[m_model.m_Phantom_SelectedIndex][m_model.m_Clothing_MakingIndex][0][1] = ClothingSetting_Density_QLineEdit->text().toDouble(); // density
	m_model.m_Clothing_MainInfo[m_model.m_Phantom_SelectedIndex][m_model.m_Clothing_MakingIndex][0][2] = ClothingSetting_Composition_QComboBox->currentIndex(); // composition	
}
void PhantomWidget::InitializeClothingPanel_InClothingAdd() // called by (1) ETHuman3DApp::PhantomClothingGenerate
{
	updateModelFromUI();
	m_model.m_Clothing_SequenceVector[m_model.m_Phantom_SelectedIndex].push_back(m_model.m_Clothing_MakingIndex);
	int ClothingTotalCount = m_model.m_Clothing_SequenceVector[m_model.m_Phantom_SelectedIndex].size();
		
	ClothingSelect_QPushButton[m_model.m_Clothing_MakingIndex] = new QPushButton(m_model.m_Clothing_ButtonName[m_model.m_Phantom_SelectedIndex][m_model.m_Clothing_MakingIndex]); // 이름 	
	panel->Buttons_FunctionPanelRight.append(ClothingSelect_QPushButton[m_model.m_Clothing_MakingIndex]);
	ClothingSelect_QPushButton[m_model.m_Clothing_MakingIndex]->setObjectName(QString::number(m_model.m_Clothing_MakingIndex));
	ClothingSelect_QPushButton[m_model.m_Clothing_MakingIndex]->setFixedSize(panel->Window_width * 0.05, panel->Window_height * 0.040);
	ClothingSelect_QPushButton[m_model.m_Clothing_MakingIndex]->setStyleSheet(DialogStyle::MENU_BTN7);
	ClothingSelect_QPushButton[m_model.m_Clothing_MakingIndex]->setFont(panel->font_D_BTN14);
	ClothingSelect_QPushButton[m_model.m_Clothing_MakingIndex]->setCheckable(true);
	ClothingSelect_QPushButton[m_model.m_Clothing_MakingIndex]->setChecked(true);
	connect(ClothingSelect_QPushButton[m_model.m_Clothing_MakingIndex], SIGNAL(clicked()), this, SLOT(slot_ClothingSelect_ButtonClicked())); connect(ClothingSelect_QPushButton[m_model.m_Clothing_MakingIndex], SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
	ClothingList_QHBoxLayout->addWidget(ClothingSelect_QPushButton[m_model.m_Clothing_MakingIndex]);
	for (auto itr : m_model.m_Clothing_SequenceVector[m_model.m_Phantom_SelectedIndex])
	{
		if (itr != m_model.m_Clothing_MakingIndex)
		{
			ClothingSelect_QPushButton[itr]->setChecked(false);
		}
	}
	// scorll bar max 이동
	timer_scroll_clothing->start(1); // 1ms 이후 이동 -> 그냥 함수에 넣으면 multiplephantombutton 추가가 더 늦어서 최대로 안 감

	// Clothing layer 초기화
	for (auto itr_layer : m_model.m_ClothingLayer_SequenceVector[m_model.m_Phantom_SelectedIndex][m_model.m_Clothing_SelectedIndex])
	{
		ClothingLayerList_QHBoxLayout->removeWidget(ClothingLayerSelect_QPushButton[itr_layer]);
		panel->Buttons_FunctionPanelRight.removeOne(ClothingLayerSelect_QPushButton[itr_layer]);
		delete ClothingLayerSelect_QPushButton[itr_layer];
		ClothingLayerSelect_QPushButton.erase(itr_layer);
	}

	// Get global variables 
	m_model.m_ClothingLayer_SequenceVector[m_model.m_Phantom_SelectedIndex][m_model.m_Clothing_MakingIndex].clear();
	m_model.m_ClothingLayer_SequenceVector[m_model.m_Phantom_SelectedIndex][m_model.m_Clothing_MakingIndex].push_back(0);
	//	
	ClothingLayerSelect_QPushButton[0] = new QPushButton(m_model.m_ClothingLayer_ButtonName[m_model.m_Phantom_SelectedIndex][m_model.m_Clothing_MakingIndex][0]); // 이름 	
	panel->Buttons_FunctionPanelRight.append(ClothingLayerSelect_QPushButton[0]);
	ClothingLayerSelect_QPushButton[0]->setObjectName(QString::number(0));
	ClothingLayerSelect_QPushButton[0]->setFixedSize(panel->Window_width * 0.04, panel->Window_height * 0.025);
	ClothingLayerSelect_QPushButton[0]->setStyleSheet(DialogStyle::MENU_BTN7);
	ClothingLayerSelect_QPushButton[0]->setFont(panel->font_D_BTN14);
	ClothingLayerSelect_QPushButton[0]->setCheckable(true);
	ClothingLayerSelect_QPushButton[0]->setChecked(true);
	connect(ClothingLayerSelect_QPushButton[0], SIGNAL(clicked()), this, SLOT(slot_ClothingLayerSelect_ButtonClicked())); connect(ClothingLayerSelect_QPushButton[0], SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
	ClothingLayerList_QHBoxLayout->addWidget(ClothingLayerSelect_QPushButton[0]);

	//Dosimeter scorll bar max 이동
	timer_scroll_clothingLayer->start(1); // 1ms 이후 이동 -> 그냥 함수에 넣으면 multiplephantombutton 추가가 더 늦어서 최대로 안 감

	// Set global variables 
	m_model.m_Clothing_SelectedIndex = m_model.m_Clothing_MakingIndex; // Set m_model.m_Clothing_SelectedIndex to be m_model.m_Clothing_MakingIndex
	m_model.m_Clothing_MakingIndex++;
	m_model.m_ClothingLayer_MakingIndex = 1; // 0번은 기본으로 만듦
	m_model.m_ClothingLayer_SelectedIndex = 0;
}
void PhantomWidget::SaveClothingInformation_InLayerGenerate() // called by (1) slot_ClothingLayerAddingOK_ButtonClicked
{
	// Clothing Layer name
	if (ClothingLayerSetting_Layername_QLineEdit->text().isEmpty()) m_model.m_ClothingLayer_ButtonName[m_model.m_Phantom_SelectedIndex][m_model.m_Clothing_SelectedIndex][m_model.m_ClothingLayer_MakingIndex] = "Layer_" + QString::number(m_model.m_ClothingLayer_MakingIndex);
	else m_model.m_ClothingLayer_ButtonName[m_model.m_Phantom_SelectedIndex][m_model.m_Clothing_SelectedIndex][m_model.m_ClothingLayer_MakingIndex] = ClothingLayerSetting_Layername_QLineEdit->text();
		
	m_model.m_Clothing_MainInfo[m_model.m_Phantom_SelectedIndex][m_model.m_Clothing_SelectedIndex][m_model.m_ClothingLayer_MakingIndex][0] = ClothingLayerSetting_Thickness_QLineEdit->text().toDouble();  // thickness
	m_model.m_Clothing_MainInfo[m_model.m_Phantom_SelectedIndex][m_model.m_Clothing_SelectedIndex][m_model.m_ClothingLayer_MakingIndex][1] = ClothingLayerSetting_Density_QLineEdit->text().toDouble(); // density
	m_model.m_Clothing_MainInfo[m_model.m_Phantom_SelectedIndex][m_model.m_Clothing_SelectedIndex][m_model.m_ClothingLayer_MakingIndex][2] = ClothingLayerSetting_Composition_QComboBox->currentIndex(); // composition	
}
void PhantomWidget::SetClothingPanelInfo(int clothingIndex, int clothingLayerIndex) 
/* called by (1) ETHuman3DApp::PhantomClothingGenerate, (2) RemoveClothingPanel, (3) SetClothingPanel_inPhantomDelete, (4) SetClothingPanel_inPhantomSelect, (5) slot_ClothingDelete_ButtonClicked,
(6) slot_ClothingSelect_ButtonClicked, (7) slot_ClothingLayerAddingOK_ButtonClicked, (8) slot_ClothingLayerDelete_ButtonClicked, (9) slot_ClothingLayerSelect_ButtonClicked */
{
	ClothingThickness_QLineEdit->setText(QString::number(m_model.m_Clothing_MainInfo[m_model.m_Phantom_SelectedIndex][clothingIndex][clothingLayerIndex][0])); // thickness
	ClothingDensity_QLineEdit->setText(QString::number(m_model.m_Clothing_MainInfo[m_model.m_Phantom_SelectedIndex][clothingIndex][clothingLayerIndex][1])); // density

	// Composition
	int CompositionIndex = m_model.m_Clothing_MainInfo[m_model.m_Phantom_SelectedIndex][clothingIndex][clothingLayerIndex][2];
	ClothingComposition_QComboBox->setCurrentIndex(CompositionIndex);
}
QString PhantomWidget::DefinePreWearablePath(int clothingSelectedType_Index) // called by (1) slot_ClothingAddingOK_ButtonClicked, (2) LoadReconsturctionFile_previous
{
	QString strPath = "";
	QString baseDir = "./data/wearable/";
	QString gender, age, posture;
	QString parts;
	QString candidates[] = { "Invalid", "head", "upper_arm", "lower_arm", "hands", "front_trunk", "back_trunk", "pelvis", "upper_leg", "lower_leg", "feet" };

	parts = (clothingSelectedType_Index >= 1 && clothingSelectedType_Index <= 10) ? candidates[clothingSelectedType_Index] : "Invalid";

	if (m_model.m_Phantom_MainInfo[m_model.m_Phantom_SelectedIndex][1] == 0) // Male
		gender = "M";
	else if (m_model.m_Phantom_MainInfo[m_model.m_Phantom_SelectedIndex][1] == 1) // Female
		gender = "F";

	if (m_model.m_Phantom_MainInfo[m_model.m_Phantom_SelectedIndex][12] == 0) // Adult
	{
		age = "A";
	}
	else if (m_model.m_Phantom_MainInfo[m_model.m_Phantom_SelectedIndex][12] == 1) // 15
	{
		age = "15";
	}
	else if (m_model.m_Phantom_MainInfo[m_model.m_Phantom_SelectedIndex][12] == 2) // 10
	{
		age = "10";
	}
	else if (m_model.m_Phantom_MainInfo[m_model.m_Phantom_SelectedIndex][12] == 3) // 5
	{
		age = "05";
	}
	else if (m_model.m_Phantom_MainInfo[m_model.m_Phantom_SelectedIndex][12] == 4) // 1
	{
		age = "01";
	}
	else if (m_model.m_Phantom_MainInfo[m_model.m_Phantom_SelectedIndex][12] == 5) // 0
	{
		age = "00";
	}

	if (m_model.m_Phantom_MainInfo[m_model.m_Phantom_SelectedIndex][2] == E_PHANTOMTYPE_ADULTMRCP || m_model.m_Phantom_MainInfo[m_model.m_Phantom_SelectedIndex][2] == E_PHANTOMTYPE_PEDIATRICMRCP) // MRCP
	{
		strPath = baseDir + "MRCP_" + age + gender + "/MRCP_" + age + gender + "_" + parts + ".obj";
	}
	if (m_model.m_Phantom_MainInfo[m_model.m_Phantom_SelectedIndex][2] == E_PHANTOMTYPE_TRANSFORMED) // Posture
	{
		if (m_model.m_Phantom_MainInfo[m_model.m_Phantom_SelectedIndex][3] == 1) posture = "bending";
		else if (m_model.m_Phantom_MainInfo[m_model.m_Phantom_SelectedIndex][3] == 2) posture = "kneeling";
		else if (m_model.m_Phantom_MainInfo[m_model.m_Phantom_SelectedIndex][3] == 3) posture = "sitting";
		else if (m_model.m_Phantom_MainInfo[m_model.m_Phantom_SelectedIndex][3] == 4) posture = "squatting";
		else if (m_model.m_Phantom_MainInfo[m_model.m_Phantom_SelectedIndex][3] == 5) posture = "walking";

		strPath = baseDir + "MRCP_" + age + gender + "_" + posture + "/MRCP_" + age + gender + "_" + posture + "_" + parts + ".obj";
	}
	if (m_model.m_Phantom_MainInfo[m_model.m_Phantom_SelectedIndex][2] == E_PHANTOMTYPE_TRANSFORMED) // Body-size
	{
		strPath = baseDir + theApp.PhantomFileTitle[m_model.m_Phantom_SelectedIndex] + "/" + theApp.PhantomFileTitle[m_model.m_Phantom_SelectedIndex] + "_" + parts + ".obj";
	}

	if (parts == "Invalid")
	{
		strPath = "Invalid";
	}
	return strPath;
}

// Sub-function (2)
void PhantomWidget::moveMaximumScrollbar_clothing() // called by (1) InitializeClothingPanel_InClothingAdd
{
	timer_scroll_clothing->stop();
	ClothingList_QScrollBar->setValue(ClothingList_QScrollBar->maximum());
}
void PhantomWidget::moveMaximumScrollbar_clothingLayer() // called by (1) InitializeClothingPanel_InClothingAdd, (2) slot_ClothingLayerAdd_ButtonClicked
{
	timer_scroll_clothingLayer->stop();
	ClothingLayerList_QScrollBar->setValue(ClothingLayerList_QScrollBar->maximum());
}

/////////////////////// Eyewear /////////////////////// 
void PhantomWidget::slot_Add_Delete_Eyewear_ButtonClicked()
{
	// Add process
	if (Eyewear_AddDelete_QPushButton->text() == "Add")
	{
		updateModelFromUI();
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
void PhantomWidget::EyewearAddingSetting()
{
	updateModelFromUI();
	m_model.m_Is_EyewearSetting_OKClicked = false;

	// Create a new dialog
	EyewearAdding_UserQDialog = new MultipleUIDialog(this);
	EyewearAdding_UserQDialog->setStyleSheet("background-color: rgb(205, 222, 243);");
	EyewearAdding_UserQDialog->setFixedWidth(panel->Window_width * 0.3);
	EyewearAdding_UserQDialog->setFixedHeight(panel->Window_height * 0.45);
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
		hLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.015, panel->Window_width * 0.003, 0);
		hLayout->setSpacing(panel->Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QLabel* label0 = new QLabel;
		label0->setFixedSize(panel->Window_width * 0.090, panel->Window_height * 0.028);
		label0->setStyleSheet(DialogStyle::DATA_LABEL);
		label0->setFont(panel->font_D_LBL1);
		label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		label0->setText(QString::fromLocal8Bit("Type:"));

		EyewearSetting_Type_QComboBox = new QComboBox;
		EyewearSetting_Type_QComboBox->setFixedSize(panel->Window_width * 0.170, panel->Window_height * 0.028);
		EyewearSetting_Type_QComboBox->setStyleSheet(DialogStyle::COMBOBOX);
		EyewearSetting_Type_QComboBox->setFont(panel->font_D_CB1);
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
		hLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.015, panel->Window_width * 0.003, 0);
		hLayout->setSpacing(panel->Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QLabel* label0 = new QLabel;
		label0->setFixedSize(panel->Window_width * 0.090, panel->Window_height * 0.028);
		label0->setStyleSheet(DialogStyle::DATA_LABEL);
		label0->setFont(panel->font_D_LBL1);
		label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		label0->setText(QString::fromLocal8Bit("Distance:"));

		EyewearSetting_Distance_QLineEdit = new QLineEdit;
		EyewearSetting_Distance_QLineEdit->setFixedSize(panel->Window_width * 0.170, panel->Window_height * 0.028);
		EyewearSetting_Distance_QLineEdit->setAlignment(Qt::AlignCenter);
		EyewearSetting_Distance_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		EyewearSetting_Distance_QLineEdit->setFont(panel->font_D_LE1);
		EyewearSetting_Distance_QLineEdit->setPlaceholderText("cm");

		hLayout->addWidget(label0);
		hLayout->addWidget(EyewearSetting_Distance_QLineEdit);

		layout->addLayout(hLayout);
	}
	{// Radius 
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.015, panel->Window_width * 0.003, 0);
		hLayout->setSpacing(panel->Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QLabel* label0 = new QLabel;
		label0->setFixedSize(panel->Window_width * 0.090, panel->Window_height * 0.028);
		label0->setStyleSheet(DialogStyle::DATA_LABEL);
		label0->setFont(panel->font_D_LBL1);
		label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		label0->setText(QString::fromLocal8Bit("Radius:"));

		EyewearSetting_Radius_QLineEdit = new QLineEdit;
		EyewearSetting_Radius_QLineEdit->setFixedSize(panel->Window_width * 0.170, panel->Window_height * 0.028);
		EyewearSetting_Radius_QLineEdit->setAlignment(Qt::AlignCenter);
		EyewearSetting_Radius_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		EyewearSetting_Radius_QLineEdit->setFont(panel->font_D_LE1);
		EyewearSetting_Radius_QLineEdit->setPlaceholderText("cm");

		hLayout->addWidget(label0);
		hLayout->addWidget(EyewearSetting_Radius_QLineEdit);

		layout->addLayout(hLayout);
	}
	{// Thickness
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.015, panel->Window_width * 0.003, 0);
		hLayout->setSpacing(panel->Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		EyewearSetting_Thickness_QLabel = new QLabel;
		EyewearSetting_Thickness_QLabel->setFixedSize(panel->Window_width * 0.090, panel->Window_height * 0.028);
		EyewearSetting_Thickness_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		EyewearSetting_Thickness_QLabel->setFont(panel->font_D_LBL1);
		EyewearSetting_Thickness_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		EyewearSetting_Thickness_QLabel->setText(QString::fromLocal8Bit("Thickness:"));

		EyewearSetting_Thickness_QLineEdit = new QLineEdit;
		EyewearSetting_Thickness_QLineEdit->setFixedSize(panel->Window_width * 0.170, panel->Window_height * 0.028);
		EyewearSetting_Thickness_QLineEdit->setAlignment(Qt::AlignCenter);
		EyewearSetting_Thickness_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		EyewearSetting_Thickness_QLineEdit->setFont(panel->font_D_LE1);
		EyewearSetting_Thickness_QLineEdit->setPlaceholderText("cm");

		hLayout->addWidget(EyewearSetting_Thickness_QLabel);
		hLayout->addWidget(EyewearSetting_Thickness_QLineEdit);

		layout->addLayout(hLayout);
	}
	{// Thickness_Front
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.015, panel->Window_width * 0.003, 0);
		hLayout->setSpacing(panel->Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		EyewearSetting_Thickness_Front_QLabel = new QLabel;
		EyewearSetting_Thickness_Front_QLabel->setFixedSize(panel->Window_width * 0.090, panel->Window_height * 0.028);
		EyewearSetting_Thickness_Front_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		EyewearSetting_Thickness_Front_QLabel->setFont(panel->font_D_LBL1);
		EyewearSetting_Thickness_Front_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		EyewearSetting_Thickness_Front_QLabel->setText(QString::fromLocal8Bit("Front Thickness:"));
		EyewearSetting_Thickness_Front_QLabel->hide();

		EyewearSetting_Thickness_Front_QLineEdit = new QLineEdit;
		EyewearSetting_Thickness_Front_QLineEdit->setFixedSize(panel->Window_width * 0.170, panel->Window_height * 0.028);
		EyewearSetting_Thickness_Front_QLineEdit->setAlignment(Qt::AlignCenter);
		EyewearSetting_Thickness_Front_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		EyewearSetting_Thickness_Front_QLineEdit->setFont(panel->font_D_LE1);
		EyewearSetting_Thickness_Front_QLineEdit->setPlaceholderText("cm");
		EyewearSetting_Thickness_Front_QLineEdit->hide();

		hLayout->addWidget(EyewearSetting_Thickness_Front_QLabel);
		hLayout->addWidget(EyewearSetting_Thickness_Front_QLineEdit);

		layout->addLayout(hLayout);
	}
	{// Thickness_Side
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.015, panel->Window_width * 0.003, 0);
		hLayout->setSpacing(panel->Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		EyewearSetting_Thickness_Side_QLabel = new QLabel;
		EyewearSetting_Thickness_Side_QLabel->setFixedSize(panel->Window_width * 0.090, panel->Window_height * 0.028);
		EyewearSetting_Thickness_Side_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
		EyewearSetting_Thickness_Side_QLabel->setFont(panel->font_D_LBL1);
		EyewearSetting_Thickness_Side_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		EyewearSetting_Thickness_Side_QLabel->setText(QString::fromLocal8Bit("Side Thickness:"));
		EyewearSetting_Thickness_Side_QLabel->hide();

		EyewearSetting_Thickness_Side_QLineEdit = new QLineEdit;
		EyewearSetting_Thickness_Side_QLineEdit->setFixedSize(panel->Window_width * 0.170, panel->Window_height * 0.028);
		EyewearSetting_Thickness_Side_QLineEdit->setAlignment(Qt::AlignCenter);
		EyewearSetting_Thickness_Side_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		EyewearSetting_Thickness_Side_QLineEdit->setFont(panel->font_D_LE1);
		EyewearSetting_Thickness_Side_QLineEdit->setPlaceholderText("cm");
		EyewearSetting_Thickness_Side_QLineEdit->hide();

		hLayout->addWidget(EyewearSetting_Thickness_Side_QLabel);
		hLayout->addWidget(EyewearSetting_Thickness_Side_QLineEdit);

		layout->addLayout(hLayout);
	}
	{// Density
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.015, panel->Window_width * 0.003, 0);
		hLayout->setSpacing(panel->Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QLabel* label0 = new QLabel;
		label0->setFixedSize(panel->Window_width * 0.090, panel->Window_height * 0.028);
		label0->setStyleSheet(DialogStyle::DATA_LABEL);
		label0->setFont(panel->font_D_LBL1);
		label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		label0->setText(QString::fromLocal8Bit("Density:"));

		EyewearSetting_Density_QLineEdit = new QLineEdit;
		EyewearSetting_Density_QLineEdit->setFixedSize(panel->Window_width * 0.170, panel->Window_height * 0.028);
		EyewearSetting_Density_QLineEdit->setAlignment(Qt::AlignCenter);
		EyewearSetting_Density_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
		EyewearSetting_Density_QLineEdit->setFont(panel->font_D_LE1);
		EyewearSetting_Density_QLineEdit->setPlaceholderText("cm");

		hLayout->addWidget(label0);
		hLayout->addWidget(EyewearSetting_Density_QLineEdit);
		 
		layout->addLayout(hLayout);
	}
	{// Composition
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.015, panel->Window_width * 0.003, 0);
		hLayout->setSpacing(panel->Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QLabel* label0 = new QLabel;
		label0->setFixedSize(panel->Window_width * 0.090, panel->Window_height * 0.028);
		label0->setStyleSheet(DialogStyle::DATA_LABEL);
		label0->setFont(panel->font_D_LBL1);
		label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		label0->setText(QString::fromLocal8Bit("Composition:"));

		EyewearSetting_Composition_QComboBox = new QComboBox;
		EyewearSetting_Composition_QComboBox->setFixedSize(panel->Window_width * 0.170, panel->Window_height * 0.028);
		EyewearSetting_Composition_QComboBox->setStyleSheet(DialogStyle::COMBOBOX);
		EyewearSetting_Composition_QComboBox->setFont(panel->font_D_CB1);
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
		hLayout->setSpacing(panel->Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		hLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.015, panel->Window_width * 0.003, 0);

		EyewearSetting_AddingOK_QPushButton = new QPushButton(QString::fromLocal8Bit("OK"));
		panel->Buttons_FunctionPanelRight.append(EyewearSetting_AddingOK_QPushButton);
		EyewearSetting_AddingOK_QPushButton->setFixedSize(panel->Window_width * 0.104, panel->Window_height * 0.028);
		EyewearSetting_AddingOK_QPushButton->setCheckable(false);
		EyewearSetting_AddingOK_QPushButton->setStyleSheet(DialogStyle::MENU_BTN3);
		EyewearSetting_AddingOK_QPushButton->setFont(panel->font_D_BTN3);

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
void PhantomWidget::slot_Glasses3DPick_ButtonClicked()
{
	updateModelFromUI();
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
void PhantomWidget::slot_GlassesUpdate_ButtonClicked()
{
	updateModelFromUI();
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
void PhantomWidget::slot_EyewearSetting_Type_ComboChanged(int idx)
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
void PhantomWidget::slot_EyewearSetting_Composition_ComboChanged(int idx)
{
	EyewearSetting_Composition_QComboBox->hide(); EyewearSetting_Composition_QComboBox->show();
}
void PhantomWidget::slot_EyewearSetting_AddingOK_ButtonClicked()
{

}

////////////////////////// Dosimeter ////////////////////////// 

// Main function - List
void PhantomWidget::slot_DosimeterAdd_ButtonClicked()
{
	updateModelFromUI();
	m_model.m_DosimeterSequenceVector[m_model.m_Phantom_SelectedIndex].push_back(m_model.m_Dosimeter_Making_Index);
	int DosimeterTotalCount = m_model.m_DosimeterSequenceVector[m_model.m_Phantom_SelectedIndex].size();
	m_model.m_Dosimeter_Selected_Index = m_model.m_Dosimeter_Making_Index;

	DosimeterSelect_QPushButton[m_model.m_Dosimeter_Selected_Index] = new QPushButton(QString::fromLocal8Bit("Dosimeter_") + QString::number(m_model.m_Dosimeter_Selected_Index));
	panel->Buttons_FunctionPanelRight.append(DosimeterSelect_QPushButton[m_model.m_Dosimeter_Selected_Index]);
	DosimeterSelect_QPushButton[m_model.m_Dosimeter_Selected_Index]->setObjectName(QString::number(m_model.m_Dosimeter_Selected_Index));
	DosimeterSelect_QPushButton[m_model.m_Dosimeter_Selected_Index]->setFixedSize(panel->Window_width * 0.05, panel->Window_height * 0.040);
	DosimeterSelect_QPushButton[m_model.m_Dosimeter_Selected_Index]->setStyleSheet(DialogStyle::MENU_BTN7);
	DosimeterSelect_QPushButton[m_model.m_Dosimeter_Selected_Index]->setFont(panel->font_D_BTN14);
	DosimeterSelect_QPushButton[m_model.m_Dosimeter_Selected_Index]->setCheckable(true);
	DosimeterSelect_QPushButton[m_model.m_Dosimeter_Selected_Index]->setChecked(true);
	connect(DosimeterSelect_QPushButton[m_model.m_Dosimeter_Selected_Index], SIGNAL(clicked()), this, SLOT(slot_DosimeterSelect_ButtonClicked())); connect(DosimeterSelect_QPushButton[m_model.m_Dosimeter_Selected_Index], SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
	DosimeterList_QHBoxLayout->addWidget(DosimeterSelect_QPushButton[m_model.m_Dosimeter_Selected_Index]);

	for (auto itr : m_model.m_DosimeterSequenceVector[m_model.m_Phantom_SelectedIndex])
	{
		if (itr != m_model.m_Dosimeter_Selected_Index)
		{
			DosimeterSelect_QPushButton[itr]->setChecked(false);
		}
	}

	//Dosimeter scorll bar max 이동
	timer_scroll_dosimeter->start(1); // 1ms 이후 이동 -> 그냥 함수에 넣으면 multiplephantombutton 추가가 더 늦어서 최대로 안 감

	if (DosimeterTotalCount == PhantomConstants::m_Dosimeter_Maximum_Count - 1) DosimeterAdd_QPushButton->setEnabled(false);

	m_model.m_Dosimeter_Making_Index++;
}
void PhantomWidget::slot_DosimeterDelete_ButtonClicked()
{
	int originalposition = theApp.removeElement(m_model.m_DosimeterSequenceVector[m_model.m_Phantom_SelectedIndex], m_model.m_Dosimeter_Selected_Index); //{0 1 2} -> {0 2}, origianl index는 지워진 1의 자리
	int DosimeterTotalCount = m_model.m_DosimeterSequenceVector[m_model.m_Phantom_SelectedIndex].size();

	DosimeterAdd_QPushButton->setEnabled(true);

	// Remove dosimeter button
	DosimeterList_QHBoxLayout->removeWidget(DosimeterSelect_QPushButton[m_model.m_Dosimeter_Selected_Index]);
	panel->Buttons_FunctionPanelRight.removeOne(DosimeterSelect_QPushButton[m_model.m_Dosimeter_Selected_Index]);
	delete DosimeterSelect_QPushButton[m_model.m_Dosimeter_Selected_Index];
	DosimeterSelect_QPushButton.erase(m_model.m_Dosimeter_Selected_Index);

	// dosimeterinfo 업데이트	
	m_model.m_Clothing_MainInfo[m_model.m_Phantom_SelectedIndex][1000][m_model.m_Dosimeter_Selected_Index][0] = 0; // 해당 dosimeterSelectedIndex의 dosimInfo map의 데이터는 사용 X
	// dosimeter UI 값 업데이트
	if (DosimeterTotalCount == 0) // Dosimeter 개수 0일때 현재 선택된 dosimeterIndex = -1(Invalid)
	{
		m_model.m_Dosimeter_Selected_Index = -1;
		DosimeterRadius_QLineEdit->clear();
		DosimeterSeparationDistance_QLineEdit->clear();
		DosimeterPosX_QLineEdit->clear();
		DosimeterPosY_QLineEdit->clear();
		DosimeterPosZ_QLineEdit->clear();
	}
	else
	{
		m_model.m_Dosimeter_Selected_Index = m_model.m_DosimeterSequenceVector[m_model.m_Phantom_SelectedIndex][originalposition]; //1번 index 자리(2번째 자리)가 지워짐 -> 새로 바뀐 벡터의 1번 index의 원소가 새로운 id
		DosimeterRadius_QLineEdit->setText(QString::number(m_model.m_Clothing_MainInfo[m_model.m_Phantom_SelectedIndex][1000][m_model.m_Dosimeter_Selected_Index][1]));
		DosimeterSeparationDistance_QLineEdit->setText(QString::number(m_model.m_Clothing_MainInfo[m_model.m_Phantom_SelectedIndex][1000][m_model.m_Dosimeter_Selected_Index][2]));
		DosimeterPosX_QLineEdit->setText(QString::number(m_model.m_Clothing_MainInfo[m_model.m_Phantom_SelectedIndex][1000][m_model.m_Dosimeter_Selected_Index][3]));
		DosimeterPosY_QLineEdit->setText(QString::number(m_model.m_Clothing_MainInfo[m_model.m_Phantom_SelectedIndex][1000][m_model.m_Dosimeter_Selected_Index][4]));
		DosimeterPosZ_QLineEdit->setText(QString::number(m_model.m_Clothing_MainInfo[m_model.m_Phantom_SelectedIndex][1000][m_model.m_Dosimeter_Selected_Index][5]));
	}
}
void PhantomWidget::slot_DosimeterSelect_ButtonClicked()
{
	updateModelFromUI();
	QPushButton* buttonSender = qobject_cast<QPushButton*>(sender());
	m_model.m_Dosimeter_Selected_Index = buttonSender->objectName().toInt();

	for (auto itr : m_model.m_DosimeterSequenceVector[m_model.m_Phantom_SelectedIndex])
	{
		if (itr != m_model.m_Dosimeter_Selected_Index)
		{
			DosimeterSelect_QPushButton[itr]->setChecked(false);
		}
	}
	DosimeterRadius_QLineEdit->setText(QString::number(m_model.m_Clothing_MainInfo[m_model.m_Phantom_SelectedIndex][1000][m_model.m_Dosimeter_Selected_Index][1]));
	DosimeterSeparationDistance_QLineEdit->setText(QString::number(m_model.m_Clothing_MainInfo[m_model.m_Phantom_SelectedIndex][1000][m_model.m_Dosimeter_Selected_Index][2]));
	DosimeterPosX_QLineEdit->setText(QString::number(m_model.m_Clothing_MainInfo[m_model.m_Phantom_SelectedIndex][1000][m_model.m_Dosimeter_Selected_Index][3]));
	DosimeterPosY_QLineEdit->setText(QString::number(m_model.m_Clothing_MainInfo[m_model.m_Phantom_SelectedIndex][1000][m_model.m_Dosimeter_Selected_Index][4]));
	DosimeterPosZ_QLineEdit->setText(QString::number(m_model.m_Clothing_MainInfo[m_model.m_Phantom_SelectedIndex][1000][m_model.m_Dosimeter_Selected_Index][5]));
}
// Main function - Setting

// Main function - Status
void PhantomWidget::slot_DosimeterPick3D_ButtonClicked()
{
	updateModelFromUI();
	theApp.m_DosimeterPick3D = !theApp.m_DosimeterPick3D;
}
void PhantomWidget::slot_DosimeterLocationUpdate_ButtonClicked()
{
	updateModelFromUI();
	double pickedpos[3];
	pickedpos[0] = DosimeterPosX_QLineEdit->text().toDouble();
	pickedpos[1] = DosimeterPosY_QLineEdit->text().toDouble();
	pickedpos[2] = DosimeterPosZ_QLineEdit->text().toDouble();

	double PosX = m_model.m_Phantom_MainInfo[m_model.m_Phantom_SelectedIndex][5];
	double PosY = m_model.m_Phantom_MainInfo[m_model.m_Phantom_SelectedIndex][6];
	double PosZ = m_model.m_Phantom_MainInfo[m_model.m_Phantom_SelectedIndex][7];

	double RotX = m_model.m_Phantom_MainInfo[m_model.m_Phantom_SelectedIndex][8];
	double RotY = m_model.m_Phantom_MainInfo[m_model.m_Phantom_SelectedIndex][9];
	double RotZ = m_model.m_Phantom_MainInfo[m_model.m_Phantom_SelectedIndex][10];
	const double PI = 3.141592 / 180; // 라디안 변환 
	double RadianX = RotX * PI;
	double RadianY = RotY * PI;
	double RadianZ = RotZ * PI;

	// 입력한 picked 좌표(실제 위치)는 pCenter 이동시킨 polydata_base에 맞게끔 되어 있으므로 이 상태에서 pointID만 얻기

	vtkSmartPointer<vtkKdTree> kdTree = vtkSmartPointer<vtkKdTree>::New();
	kdTree->BuildLocatorFromPoints(theApp.m_3DHumanData_MultiplePhantom[m_model.m_Phantom_SelectedIndex].polydata_base); // picked 좌표에서 polydata_base의 존재하는 가장 가까운 점을 찾을거임
	double closestPointDist;
	int closestPointID = kdTree->FindClosestPoint(pickedpos[0], pickedpos[1], pickedpos[2], closestPointDist); // 이 ID가 polydata_base 존재하는 점 중 가장 가까운 점의 ID
	double closestPoint[3]; // 이 점이 polydata_base에 존재하는 점 중 picked된 점을 이동/회전을 역적용한뒤의 점과 가장 가까운 점을 찾음
	theApp.m_3DHumanData_MultiplePhantom[m_model.m_Phantom_SelectedIndex].polydata_base->GetPoint(closestPointID, closestPoint);

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
	theApp.phantomObjects->RefreshDosimeter3DShpere(ReversedTransformedPicekdPos, m_model.m_Dosimeter_Selected_Index);
}
void PhantomWidget::slot_DosimeterUpdate_ButtonClicked()
{
	updateModelFromUI();
	theApp.phantomObjects->RemoveDosimeter3DShpere();

	theApp.m_DosimeterPick3D = !theApp.m_DosimeterPick3D;

	theApp.phantomObjects->DosimeterGenerate(m_model.m_Dosimeter_Selected_Index);
}

void PhantomWidget::moveMaximumScrollbar_dosimeter() // called by (1) slot_DosimeterAdd_ButtonClicked
{
	timer_scroll_dosimeter->stop();
	DosimeterList_QScrollBar->setValue(DosimeterList_QScrollBar->maximum());
}

void PhantomWidget::ClosePhantomUIdialog_Recon()
{
	PhantomSetting_UserQDialog->close();
}
void PhantomWidget::CloseClothingUIdialog_Recon()
{
	ClothingSetting_UserQDialog->close();
}