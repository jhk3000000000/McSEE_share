#include "pch.h"
#include "FunctionPanelTop.h"
#include "FunctionPanelRight.h"
#include "ProcessInformationPanel.h"
#include "ETQVTKWidget.h"
#include "ETHuman3DApp.h"
#include "Util.h"

FunctionPanelTop::FunctionPanelTop(QWidget *parent)
	: QWidget(parent)
{
	window_width *= default_RatioWtoH / RatioWtoH; // 화면비 calibration

	font_MP_BTN1.setPointSize(FontSizeScaling(12));
	font_MP_GBW1.setPointSize(FontSizeScaling(12));

	setParent(parent);
	setStyleSheet(MenuPanelSTyle::BACKGROUND_WIDGET);
	this->setContextMenuPolicy(Qt::PreventContextMenu);

	// size
	int height = 0.076 * window_height;
	setFixedHeight(height);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	// 생성
	CreateWidgets();
}

FunctionPanelTop::~FunctionPanelTop()
{
	SAFE_DELETE(m_pShadeButton);
	SAFE_DELETE(m_pFlatButton);
	SAFE_DELETE(m_pWireFrameButton);
	SAFE_DELETE(m_pPointsButton);

	SAFE_DELETE(m_pView3DModelButton);
	SAFE_DELETE(m_pViewBoundaryBoxButton);
	SAFE_DELETE(m_pViewAnnotationButton);
}

void FunctionPanelTop::CreateWidgets()
{
	QHBoxLayout* mainLayout = new QHBoxLayout;
	mainLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	mainLayout->setSpacing(window_width * 0.0015);
	mainLayout->setContentsMargins(0, window_height * 0.003, window_width * 0.0015, 0);

	QHBoxLayout* hLayout0 = new QHBoxLayout;
	hLayout0->setSpacing(window_width * 0.001);
	hLayout0->setContentsMargins(0, 0, 0, 0);
	hLayout0->setAlignment(Qt::AlignLeft | Qt::AlignTop);

	{
		QGroupBox *groupBox = new QGroupBox("Data");
		groupBox->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
		groupBox->setStyleSheet(MenuPanelSTyle::GROUPBOX_WIDGET);
		groupBox->setFont(font_MP_GBW1);
		groupBox->setFixedSize(window_width * 0.081, window_height * 0.0713);
		groupBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

		QHBoxLayout* subLayout = new QHBoxLayout;
		subLayout->setSpacing(window_width * 0.0026);
		subLayout->setContentsMargins(window_width * 0.0026, window_height * 0.0046, window_width * 0.0026, window_height * 0.0046);
		subLayout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);

		QPushButton* fpt_buttonLoad = new QPushButton(QString::fromLocal8Bit("Load"));
		fpt_buttonLoad->setFixedSize(window_width * 0.035, window_height *0.0463);
		fpt_buttonLoad->setStyleSheet(MenuPanelSTyle::MENU_BTN);
		fpt_buttonLoad->setFont(font_MP_BTN1);
		connect(fpt_buttonLoad, SIGNAL(clicked()), this, SLOT(slot_fpt_buttonLoad_ButtonClicked()));

		QPushButton* fpt_buttonSave = new QPushButton(QString::fromLocal8Bit("Save"));
		fpt_buttonSave->setFixedSize(window_width * 0.035, window_height *0.0463);
		fpt_buttonSave->setStyleSheet(MenuPanelSTyle::MENU_BTN);
		fpt_buttonSave->setFont(font_MP_BTN1);
		connect(fpt_buttonSave, SIGNAL(clicked()), this, SLOT(slot_fpt_buttonSave_ButtonClicked()));

		subLayout->addWidget(fpt_buttonLoad);
		subLayout->addWidget(fpt_buttonSave);

		groupBox->setLayout(subLayout);

		hLayout0->addWidget(groupBox);
	}

	QHBoxLayout* hLayout2 = new QHBoxLayout;
	hLayout2->setSpacing(window_width * 0.001);
	hLayout2->setContentsMargins(0, 0, 0, 0);
	hLayout2->setAlignment(Qt::AlignLeft | Qt::AlignTop);

	{
		QGroupBox *groupBox = new QGroupBox("Viewport");
		groupBox->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
		groupBox->setStyleSheet(MenuPanelSTyle::GROUPBOX_WIDGET);
		groupBox->setFont(font_MP_GBW1);
		groupBox->setFixedSize(window_width *0.1807, window_height * 0.0713);
		groupBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

		QHBoxLayout* subLayout = new QHBoxLayout;
		subLayout->setSpacing(window_width * 0.0026);
		subLayout->setContentsMargins(window_width * 0.0026, window_height * 0.0046, window_width * 0.0026, window_height * 0.0046);
		subLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QPushButton* pTopButton = new QPushButton(QString::fromLocal8Bit("Top"));
		pTopButton->setFixedSize(window_width * 0.0276, window_height * 0.0463);
		pTopButton->setStyleSheet(MenuPanelSTyle::MENU_BTN);
		pTopButton->setFont(font_MP_BTN1);
		connect(pTopButton, SIGNAL(clicked()), this, SLOT(slot_ViewportTop_ButtonClicked()));

		QPushButton* pBottomButton = new QPushButton(QString::fromLocal8Bit("Bot\ntom"));
		pBottomButton->setFixedSize(window_width * 0.0276, window_height * 0.0463);
		pBottomButton->setStyleSheet(MenuPanelSTyle::MENU_BTN);
		pBottomButton->setFont(font_MP_BTN1);
		connect(pBottomButton, SIGNAL(clicked()), this, SLOT(slot_ViewportBottom_ButtonClicked()));


		QPushButton* pLeftButton = new QPushButton(QString::fromLocal8Bit("Left"));
		pLeftButton->setFixedSize(window_width * 0.0276, window_height * 0.0463);
		pLeftButton->setStyleSheet(MenuPanelSTyle::MENU_BTN);
		pLeftButton->setFont(font_MP_BTN1);
		connect(pLeftButton, SIGNAL(clicked()), this, SLOT(slot_ViewportLeft_ButtonClicked()));


		QPushButton* pRightButton = new QPushButton(QString::fromLocal8Bit("Right"));
		pRightButton->setFixedSize(window_width * 0.0276, window_height * 0.0463);
		pRightButton->setStyleSheet(MenuPanelSTyle::MENU_BTN);
		pRightButton->setFont(font_MP_BTN1);
		connect(pRightButton, SIGNAL(clicked()), this, SLOT(slot_ViewportRight_ButtonClicked()));


		QPushButton* pFrontButton = new QPushButton(QString::fromLocal8Bit("Front"));
		pFrontButton->setFixedSize(window_width * 0.0276, window_height * 0.0463);
		pFrontButton->setStyleSheet(MenuPanelSTyle::MENU_BTN);
		pFrontButton->setFont(font_MP_BTN1);
		connect(pFrontButton, SIGNAL(clicked()), this, SLOT(slot_ViewportFront_ButtonClicked()));
		

		QPushButton* pBackButton = new QPushButton(QString::fromLocal8Bit("Back"));
		pBackButton->setFixedSize(window_width * 0.0276, window_height * 0.0463);
		pBackButton->setStyleSheet(MenuPanelSTyle::MENU_BTN);
		pBackButton->setFont(font_MP_BTN1);
		connect(pBackButton, SIGNAL(clicked()), this, SLOT(slot_ViewportBack_ButtonClicked()));
		
		subLayout->addWidget(pTopButton);
		subLayout->addWidget(pBottomButton);
		subLayout->addWidget(pLeftButton);
		subLayout->addWidget(pRightButton);
		subLayout->addWidget(pFrontButton);
		subLayout->addWidget(pBackButton);


		groupBox->setLayout(subLayout);

		hLayout2->addWidget(groupBox);
	}
	   

	QHBoxLayout* hLayout4 = new QHBoxLayout;
	hLayout4->setSpacing(2);
	hLayout4->setContentsMargins(5, 5, 10, 5);
	hLayout4->setAlignment(Qt::AlignLeft | Qt::AlignTop);

	{
		QLabel* labelLogo = new QLabel;
		labelLogo->setStyleSheet(MenuPanelSTyle::IMAGE_LABEL);
		labelLogo->setAlignment(Qt::AlignCenter);
		labelLogo->setFixedSize(120 * 1.2, 55 * 1.2);

		hLayout4->addWidget(labelLogo);
	}

	mainLayout->addLayout(hLayout0);
	mainLayout->addLayout(hLayout2);

	mainLayout->addStretch();
	mainLayout->addLayout(hLayout4);

	setLayout(mainLayout);

	slot_ViewportFront_ButtonClicked(); // 기본 카메라뷰를 Front view로 설정
}

void FunctionPanelTop::paintEvent(QPaintEvent *event)
{
	QStyleOption option;
	//option.init(this);
	option.initFrom(this);
	QPainter painter(this);
	style()->drawPrimitive(QStyle::PE_Widget, &option, &painter, this);

	__super::paintEvent(event);
}

void FunctionPanelTop::slot_fpt_buttonSave_ButtonClicked()
{
	QString filter = tr("MCSEE Project File (*.mcsee);;All Files (*)");
	QString filePath = QFileDialog::getSaveFileName(
		this,
		tr("Save MCSEE Project"), // 대화상자 제목
		QDir::homePath(),        // 기본 경로 (예: 내 문서)
		filter                   // 파일 형식 필터
	);

	if (filePath.isEmpty()) {
		return;
	}

	theApp.pRt->Write_mcsee_File(filePath);



	//// QFile 객체 생성
	//QFile file(filePath);

	//// 파일을 쓰기 모드로 열기
	//if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
	//{
	//	QMessageBox::warning(this, tr("Unable to open file"), file.errorString());
	//	return;
	//}

	//// 파일 닫기
	//file.close();		

	//theApp.WriteSaveFile(filePath.toStdString());
	//theApp.WriteExtdataFile(filePath.toStdString() + ".extdata");

	theApp.SetMessageBox("File has saved sucessfully.");
}

void FunctionPanelTop::slot_ViewportTop_ButtonClicked()
{
	// Viewport reset 전, object/phantom 있으면 축 제외하고 카메라뷰 클리핑하기 위해 축들은 잠깐 가시화 끄기
	if (theApp.pRt) // pRt 생성 되었는지 확인 
	{
		if (theApp.pRt->m_Phantom_SequenceVector.size() > 0 ||
			theApp.pRt->m_sourceOV_objectSequenceVector.size() > 0) theApp.m_pVTKWidget->SetVisibilityOff_AxesObjects();
	}	

	auto camera = theApp.m_pVTKWidget->GetSceneRenderer()->GetActiveCamera();
	if (camera)
	{
		camera->SetPosition(0, 0, 1);
		camera->SetFocalPoint(0, 0, 0);
		camera->SetViewUp(0, 1, 0);
	}

	theApp.m_pVTKWidget->GetSceneRenderer()->ResetCamera();	

	theApp.m_pVTKWidget->SetVisibilityOn_AxesObjects(); // 다시 축들을 가시화 켜기

	theApp.m_pVTKWidget->renderWindow()->Render();
}

void FunctionPanelTop::slot_ViewportBottom_ButtonClicked()
{
	// Viewport reset 전, object/phantom 있으면 축 제외하고 카메라뷰 클리핑하기 위해 축들은 잠깐 가시화 끄기
	if (theApp.pRt) // pRt 생성 되었는지 확인 
	{
		if (theApp.pRt->m_Phantom_SequenceVector.size() > 0 ||
			theApp.pRt->m_sourceOV_objectSequenceVector.size() > 0) theApp.m_pVTKWidget->SetVisibilityOff_AxesObjects();
	}

	auto camera = theApp.m_pVTKWidget->GetSceneRenderer()->GetActiveCamera();
	if (camera)
	{
		camera->SetPosition(0, 0, -1);
		camera->SetFocalPoint(0, 0, 0);
		camera->SetViewUp(0, -1, 0);
	}

	theApp.m_pVTKWidget->GetSceneRenderer()->ResetCamera();	

	theApp.m_pVTKWidget->SetVisibilityOn_AxesObjects(); // 다시 축들을 가시화 켜기

	theApp.m_pVTKWidget->renderWindow()->Render();
}

void FunctionPanelTop::slot_ViewportLeft_ButtonClicked()
{
	// Viewport reset 전, object/phantom 있으면 축 제외하고 카메라뷰 클리핑하기 위해 축들은 잠깐 가시화 끄기
	if (theApp.pRt) // pRt 생성 되었는지 확인 
	{
		if (theApp.pRt->m_Phantom_SequenceVector.size() > 0 ||
			theApp.pRt->m_sourceOV_objectSequenceVector.size() > 0) theApp.m_pVTKWidget->SetVisibilityOff_AxesObjects();
	}

	auto camera = theApp.m_pVTKWidget->GetSceneRenderer()->GetActiveCamera();
	if (camera)
	{
		camera->SetPosition(1, 0, 0);
		camera->SetFocalPoint(0, 0, 0);
		camera->SetViewUp(0, 0, 1);
	}

	theApp.m_pVTKWidget->GetSceneRenderer()->ResetCamera();

	theApp.m_pVTKWidget->SetVisibilityOn_AxesObjects(); // 다시 축들을 가시화 켜기

	theApp.m_pVTKWidget->renderWindow()->Render();

}

void FunctionPanelTop::slot_ViewportRight_ButtonClicked()
{
	// Viewport reset 전, object/phantom 있으면 축 제외하고 카메라뷰 클리핑하기 위해 축들은 잠깐 가시화 끄기
	if (theApp.pRt) // pRt 생성 되었는지 확인 
	{
		if (theApp.pRt->m_Phantom_SequenceVector.size() > 0 ||
			theApp.pRt->m_sourceOV_objectSequenceVector.size() > 0) theApp.m_pVTKWidget->SetVisibilityOff_AxesObjects();
	}

	auto camera = theApp.m_pVTKWidget->GetSceneRenderer()->GetActiveCamera();
	if (camera)
	{
		camera->SetPosition(-1, 0, 0);
		camera->SetFocalPoint(0, 0, 0);
		camera->SetViewUp(0, 0, 1);
	}

	theApp.m_pVTKWidget->GetSceneRenderer()->ResetCamera();

	theApp.m_pVTKWidget->SetVisibilityOn_AxesObjects(); // 다시 축들을 가시화 켜기

	theApp.m_pVTKWidget->renderWindow()->Render();

}

void FunctionPanelTop::slot_ViewportFront_ButtonClicked()
{
	// Viewport reset 전, object/phantom 있으면 축 제외하고 카메라뷰 클리핑하기 위해 축들은 잠깐 가시화 끄기
	if (theApp.pRt) // pRt 생성 되었는지 확인 
	{
		if (theApp.pRt->m_Phantom_SequenceVector.size() > 0 ||
			theApp.pRt->m_sourceOV_objectSequenceVector.size() > 0) theApp.m_pVTKWidget->SetVisibilityOff_AxesObjects();
	}

	auto camera = theApp.m_pVTKWidget->GetSceneRenderer()->GetActiveCamera();
	if (camera)
	{
		camera->SetPosition(0, -1, 0);
		camera->SetFocalPoint(0, 0, 0);
		camera->SetViewUp(0, 0, 1);
	}

	theApp.m_pVTKWidget->GetSceneRenderer()->ResetCamera();

	theApp.m_pVTKWidget->SetVisibilityOn_AxesObjects(); // 다시 축들을 가시화 켜기

	theApp.m_pVTKWidget->renderWindow()->Render();

}

void FunctionPanelTop::slot_ViewportBack_ButtonClicked()
{
	// Viewport reset 전, object/phantom 있으면 축 제외하고 카메라뷰 클리핑하기 위해 축들은 잠깐 가시화 끄기
	if (theApp.pRt) // pRt 생성 되었는지 확인 
	{
		if (theApp.pRt->m_Phantom_SequenceVector.size() > 0 ||
			theApp.pRt->m_sourceOV_objectSequenceVector.size() > 0) theApp.m_pVTKWidget->SetVisibilityOff_AxesObjects();
	}

	auto camera = theApp.m_pVTKWidget->GetSceneRenderer()->GetActiveCamera();
	if (camera)
	{
		camera->SetPosition(0, 1, 0);
		camera->SetFocalPoint(0, 0, 0);
		camera->SetViewUp(0, 0, 1);
	}

	theApp.m_pVTKWidget->GetSceneRenderer()->ResetCamera();

	theApp.m_pVTKWidget->SetVisibilityOn_AxesObjects(); // 다시 축들을 가시화 켜기

	theApp.m_pVTKWidget->renderWindow()->Render();

}

void FunctionPanelTop::slot_fpt_buttonLoad_ButtonClicked()
{
	const QString dir = QFileDialog::getOpenFileName(
		this, tr("Select a result file to open"), QString(), tr("Scan Files (*.*)"));
	if (dir.isEmpty()) return;

	const QString strTitle = QString::fromLocal8Bit(
		"All input parameter will be reset. Do you want to load the file?");
	if (QMessageBox::Yes != theApp.SetMessageBox_YesNo(strTitle)) return;

	// [재시작-로드] 1) 현재 프로세스에서 즉시 로드하지 않고,
	//               새 인스턴스를 --autoload <선택한 경로>로 "재실행"한다.
	//   - startDetached: 부모-자식 프로세스 연결 없이 바로 독립 실행
	//   - QDir::toNativeSeparators: Windows 경로(역슬래시) 안전화
	//   - applicationDirPath: 작업 디렉터리를 실행파일 위치로 고정(상대경로 리소스 안전)
	const QString exe = QCoreApplication::applicationFilePath();                   // [재시작-로드]
	QStringList args;  args << "--autoload" << QDir::toNativeSeparators(dir);      // [재시작-로드]
	if (!QProcess::startDetached(exe, args, QCoreApplication::applicationDirPath())) { // [재시작-로드]
		QMessageBox::warning(this, tr("Error"), tr("Failed to restart application."));
		return;
	}

	// [재시작-로드] 2) 사용자 경험(UX) 개선을 위해,
	//   - 새 프로세스를 띄운 뒤, 기존 창을 "즉시 숨기고"
	//   - 종료 신호는 "다음 이벤트 틱"에 큐잉하여 프리즈/응답없음 방지
	for (QWidget *w : QApplication::topLevelWidgets()) { // [재시작-로드]
		if (w) w->hide();                                // [재시작-로드]
	}
	QApplication::processEvents(QEventLoop::AllEvents, 50);   // [재시작-로드]

	// [재시작-로드] 3) 즉시 quit() 호출 대신 QueuedConnection으로 안전하게 종료
	QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection); // [재시작-로드]
}

// 아래는 원본입니다
//void FunctionPanelTop::slot_fpt_buttonLoad_ButtonClicked()
//{
//	QString dir = QFileDialog::getOpenFileName(
//			this,
//			tr("Select a result file to open"),
//			"",
//			tr("Scan Files (*.*)"));
//	if (!dir.isEmpty())
//	{
//		QString strTitle = QString::fromLocal8Bit("All input parameter will be reset. Do you want to load the file?");
//
//		if (QMessageBox::Yes == theApp.SetMessageBox_YesNo(strTitle))
//		{
//			theApp.pRt->b_Is_Reconstruction = true;
//			QString path = dir;
//			if (path.endsWith(".txt"))  // 만약 결과 파일이면 ".txt" 부분 제거, savedata파일이면 확장자가 없을 것임
//			{
//				path.chop(4);
//			}
//			theApp.Read_mcsee_File_previous(path + ".extdata");
//			theApp.pRt->LoadReconsturctionFile_previous(dir);						
//			theApp.pRt->b_Is_Reconstruction = false;
//		}		
//	}
//
//	theApp.pRt->ExecuteAllButttonClicked();
//}


