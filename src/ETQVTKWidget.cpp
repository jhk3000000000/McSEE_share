#include "ETHuman3DApp.h"
#include "pch.h"
#include "FunctionPanelRight.h"
#include "ETQVTKWidget.h"
#include "ETInteractorStyle.h"
#include "ETInteractorStyleTrackballActor.h"
#include "ETInteractorStyleRubberBand.h"
#include "vtkAreaPicker.h"
#include "vtkPlanes.h"
#include "vtkContourWidget.h"
#include "vtkCubeAxesActor.h"
#include "Util.h"

class RunButton : public QPushButton {
public:
    RunButton(QWidget *parent = nullptr) : QPushButton(parent) {
        // Set the default and hover icons
        defaultIcon = QIcon("./data/image/execute.png");
        hoverIcon = QIcon("./data/image/execute_clicked.png");

        // [수정] 스크린 크기를 생성자에서 '한 번만' 계산합니다.
        QScreen *screen = QGuiApplication::primaryScreen();
        if (screen)
        {
            double screenWidth = screen->geometry().width();
            m_iconSize = QSize(screenWidth * 0.090, screenWidth * 0.030);
        }
        else
        {
            m_iconSize = QSize(172, 57); // (예: 1920*0.09, 1920*0.03) 기본값
        }

        // Set the default icon initially
        setIcon(defaultIcon);
        // [수정] 계산된 멤버 변수 사용
        setIconSize(m_iconSize);
    }
protected:
    // [수정] 매개변수를 'QEnterEvent *'로 변경
    void enterEvent(QEnterEvent *event) override {
        // [수정] 불필요한 if문 제거
        setIcon(hoverIcon);
        // [수정] 멤버 변수 사용 (QApplication::desktop() 호출 제거)
        setIconSize(m_iconSize);
        QPushButton::enterEvent(event);
    }

    // [수정 없음] leaveEvent 시그니처는 'QEvent *' 그대로입니다.
    void leaveEvent(QEvent *event) override {
        // [수정] 불필요한 if문 제거
        setIcon(defaultIcon);
        // [수정] 멤버 변수 사용 (QApplication::desktop() 호출 제거)
        setIconSize(m_iconSize);
        QPushButton::leaveEvent(event);
    }
private:
    QIcon defaultIcon;
    QIcon hoverIcon;
    QSize m_iconSize; // [추가] 계산된 아이콘 크기를 저장할 멤버 변수
};

class StopButton : public QPushButton {
public:
    StopButton(QWidget *parent = nullptr) : QPushButton(parent) {
        // Set the default and hover icons
        defaultIcon = QIcon("./data/image/stop.png");
        hoverIcon = QIcon("./data/image/stop_clicked.png");
        disabledIcon = QIcon("./data/image/stop_disabled.png");

        // [수정] 스크린 크기를 생성자에서 '한 번만' 계산합니다.
        QScreen *screen = QGuiApplication::primaryScreen();
        if (screen)
        {
            double screenWidth = screen->geometry().width();
            m_iconSize = QSize(screenWidth * 0.090, screenWidth * 0.030);
        }
        else
        {
            m_iconSize = QSize(172, 57); // (예: 1920*0.09, 1920*0.03) 기본값
        }

        // Set the default icon initially
        setIcon(defaultIcon);
        // [수정] 계산된 멤버 변수 사용
        setIconSize(m_iconSize);
    }
protected:
    // [수정] 매개변수를 'QEnterEvent *'로 변경
    void enterEvent(QEnterEvent *event) override {
        // [수정] 불필요한 if문 제거
        setIcon(hoverIcon);
        // [수정] 멤버 변수 사용 (QApplication::desktop() 호출 제거)
        setIconSize(m_iconSize);
        QPushButton::enterEvent(event);
    }

    // [수정 없음] leaveEvent 시그니처는 'QEvent *' 그대로입니다.
    void leaveEvent(QEvent *event) override {
        // [수정] 불필요한 if문 제거
        setIcon(defaultIcon);
        // [수정] 멤버 변수 사용 (QApplication::desktop() 호출 제거)
        setIconSize(m_iconSize);
        QPushButton::leaveEvent(event);
    }

    // [수정] 'override' 키워드 X
    void setEnabled(bool enabled) {
        QPushButton::setEnabled(enabled);
        if (!enabled) {
            setIcon(disabledIcon);
        }
        else {
            // [수정] 활성화될 때, 마우스가 이미 위에 있는지 확인
            if (underMouse()) {
                setIcon(hoverIcon);
            } else {
                setIcon(defaultIcon);
            }
        }
    }
private:
    QIcon defaultIcon;
    QIcon hoverIcon;
    QIcon disabledIcon;
    QSize m_iconSize; // [추가] 계산된 아이콘 크기를 저장할 멤버 변수
};

class ResetButton : public QPushButton {
public:
    ResetButton(QWidget *parent = nullptr) : QPushButton(parent) {
        // Set the default and hover icons
        defaultIcon = QIcon("./data/image/reset.png");
        hoverIcon = QIcon("./data/image/reset.png");
        disabledIcon = QIcon("./data/image/reset.png");

        // [수정] 스크린 크기를 생성자에서 '한 번만' 계산합니다.
        QScreen *screen = QGuiApplication::primaryScreen();
        if (screen)
        {
            double screenWidth = screen->geometry().width();
            m_iconSize = QSize(screenWidth * 0.090, screenWidth * 0.030);
        }
        else
        {
            m_iconSize = QSize(172, 57); // (예: 1920*0.09, 1920*0.03) 기본값
        }

        // Set the default icon initially
        setIcon(defaultIcon);
        // [수정] 계산된 멤버 변수 사용
        setIconSize(m_iconSize);
    }
protected:
    // [수정] 매개변수를 'QEnterEvent *'로 변경
    void enterEvent(QEnterEvent *event) override {
        // [수정] 불필요한 if문 제거
        setIcon(hoverIcon);
        // [수정] 멤버 변수 사용 (QApplication::desktop() 호출 제거)
        setIconSize(m_iconSize);
        QPushButton::enterEvent(event);
    }

    // [수정 없음] leaveEvent 시그니처는 'QEvent *' 그대로입니다.
    void leaveEvent(QEvent *event) override {
        // [수정] 불필요한 if문 제거
        setIcon(defaultIcon);
        // [수정] 멤버 변수 사용 (QApplication::desktop() 호출 제거)
        setIconSize(m_iconSize);
        QPushButton::leaveEvent(event);
    }

    // [수정] 'override' 키워드 X
    void setEnabled(bool enabled) {
        QPushButton::setEnabled(enabled);
        if (!enabled) {
            setIcon(disabledIcon);
        }
        else {
            // (Reset 버튼은 hover 상태가 따로 없으므로 defaultIcon으로 설정)
            setIcon(defaultIcon);
        }
    }
private:
    QIcon defaultIcon;
    QIcon hoverIcon;
    QIcon disabledIcon;
    QSize m_iconSize; // [추가] 계산된 아이콘 크기를 저장할 멤버 변수
};

class SettingButton : public QPushButton {
public:
    SettingButton(QWidget *parent = nullptr) : QPushButton(parent) {
        // Set the default and hover icons
        defaultIcon = QIcon("./data/image/setting.png");
        hoverIcon = QIcon("./data/image/setting_clicked.png");

        // [수정] 스크린 크기를 생성자에서 '한 번만' 계산합니다.
        QScreen *screen = QGuiApplication::primaryScreen();
        if (screen)
        {
            double screenWidth = screen->geometry().width();
            // (너비와 높이에 모두 screenWidth * 0.015를 사용하셨네요)
            m_iconSize = QSize(screenWidth * 0.015, screenWidth * 0.015);
        }
        else
        {
            m_iconSize = QSize(28, 28); // (예: 1920*0.015) 기본값
        }

        // Set the default icon initially
        setIcon(defaultIcon);
        // [수정] 계산된 멤버 변수 사용
        setIconSize(m_iconSize);
    }
protected:
    // [수정] 매개변수를 'QEnterEvent *'로 변경
    void enterEvent(QEnterEvent *event) override {
        // [수정] 불필요한 if문 제거
        setIcon(hoverIcon);
        // [수정] 멤버 변수 사용 (QApplication::desktop() 호출 제거)
        setIconSize(m_iconSize);
        QPushButton::enterEvent(event);
    }

    // [수정 없음] leaveEvent 시그니처는 'QEvent *' 그대로입니다.
    void leaveEvent(QEvent *event) override {
        // [수정] 불필요한 if문 제거
        setIcon(defaultIcon);
        // [수정] 멤버 변수 사용 (QApplication::desktop() 호출 제거)
        setIconSize(m_iconSize);
        QPushButton::leaveEvent(event);
    }
private:
    QIcon defaultIcon;
    QIcon hoverIcon;
    QSize m_iconSize; // [추가] 계산된 아이콘 크기를 저장할 멤버 변수
};

ETQVTKWidget::ETQVTKWidget()
{
	initialize();
}

namespace {
	//----------------------------------------------------------------------------
	class vtkPositionCallback : public vtkCommand
	{
	public:
		static vtkPositionCallback* New()
		{
			return new vtkPositionCallback;
		}

		void Execute(vtkObject* vtkNotUsed(caller), unsigned long vtkNotUsed(event),
			void* vtkNotUsed(callData))
		{
			this->Axes->InitPathTraversal();
			
			vtkAssemblyPath* path = 0;
			int count = 0;
			vtkFollower* followers[3] = { this->XLabel, this->YLabel, this->ZLabel };
			int followerId = 0;
			while ((path = this->Axes->GetNextPath()) != NULL)
			{
				if (count++ > 2)
				{
					vtkProp3D* prop3D =
						static_cast<vtkProp3D*>(path->GetLastNode()->GetViewProp());
					if (prop3D)
					{
						prop3D->PokeMatrix(path->GetLastNode()->GetMatrix());
						followers[followerId]->SetPosition(prop3D->GetCenter());
						followerId++;
						prop3D->PokeMatrix(NULL);
					}
				}
			}
		}

		vtkPositionCallback() : XLabel(0), YLabel(0), ZLabel(0), Axes(0)
		{			
		}

		vtkFollower* XLabel;
		vtkFollower* YLabel;
		vtkFollower* ZLabel;
		vtkAssembly* Axes;
	};
}

ETQVTKWidget::~ETQVTKWidget()
{
	if (m_movableaxesWidget)
		m_movableaxesWidget->Delete();
	if (collection)
		collection->Delete();
	if (movableAxes)
		movableAxes->Delete();
//	if (m_axesWidget)
//		m_axesWidget->Delete();
}

void ETQVTKWidget::initialize()
{
	// Default 랜더러
	m_pSceneRenderer = vtkSmartPointer<vtkRenderer>::New();

	// 배경색 설정
	SetBackGroundColor();

	// 랜더러 추가
	m_pSceneRenderer->InteractiveOn();
	m_pSceneRenderer->GetActiveCamera()->SetParallelProjection(1);
	this->renderWindow()->AddRenderer(m_pSceneRenderer);

	//기본 interactor(Camera) 생성
	m_pInteractorCamera = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	m_pBaseInteractorStyle = vtkSmartPointer<ETInteractorStyle>::New();
	m_pInteractorCamera->SetInteractorStyle(m_pBaseInteractorStyle);

	// Actor interactor 생성
	m_pInteractorActor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	m_pBaseInteractorStyleTrackballActor = vtkSmartPointer<ETInteractorStyleTrackballActor>::New();
	m_pInteractorActor->SetInteractorStyle(m_pBaseInteractorStyleTrackballActor);

	// Rubberband interactor 생성
	m_pInteractorRubberBand = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	m_pBaseInteractorStyleRubberBand = vtkSmartPointer<ETInteractorStyleRubberBand>::New();
	m_pInteractorRubberBand->SetInteractorStyle(m_pBaseInteractorStyleRubberBand);
	
	// 기본적으로는 Camera로 시작
	this->renderWindow()->GetInteractor()->SetInteractorStyle(m_pBaseInteractorStyle);
	 
/*	// AxisWidget 생성
	m_axesWidget = vtkOrientationMarkerWidget::New();
	vtkSmartPointer<vtkAxesActor> axes = vtkSmartPointer<vtkAxesActor>::New();
	m_axesWidget->SetOrientationMarker(axes);
	m_axesWidget->SetCurrentRenderer(m_pSceneRenderer);
	m_axesWidget->SetInteractor(this->renderWindow()->GetInteractor());
	m_axesWidget->SetEnabled(1);
	m_axesWidget->SetInteractive(0);
	m_axesWidget->SetViewport(0., 0., 0.15, 0.1);

	this->renderWindow()->Render();
*/

	// MovableAxis 생성
	m_movableaxesWidget->SetTotalLength(60., 60., 120.);
	m_movableaxesWidget->SetNormalizedTipLength(0.05, 0.05, 0.025);
	m_movableaxesWidget->SetNormalizedShaftLength(1, 1, 1);
	m_movableaxesWidget->GetActors(collection);	
	
	collection->InitTraversal();
	
	for (int i = 0; i < collection->GetNumberOfItems(); ++i)
	{
		vtkProp3D * prop = (vtkProp3D*)collection->GetNextProp();
		prop->PickableOff(); // X,Y,Z 축 움직이지 않게 고정		
		movableAxes->AddPart(prop);

		//movableAxes->AddPart((vtkProp3D*)collection->GetNextProp()); // 원본 -> 움직이는 X,Y,Z 축
		
	}	
	m_pSceneRenderer->AddActor(movableAxes);	

	// create our own labels that will follow and face the camera
	double color[3];
	color[0] = 0.f;
	color[1] = 0.f;
	color[2] = 0.f;

	vtkNew<vtkVectorText> xText;
	vtkNew<vtkPolyDataMapper> xTextMapper;
	xText->SetText("X");	
	xTextMapper->SetInputConnection(xText->GetOutputPort());
	xLabel->GetProperty()->SetColor(1.f, 0.f, 0.f);
	xLabel->SetMapper(xTextMapper);
	xLabel->SetScale(5);
	xLabel->SetCamera(m_pSceneRenderer->GetActiveCamera());
	xLabel->SetPosition(((vtkProp3D*)collection->GetItemAsObject(3))->GetCenter()); // XAxisTip
	xLabel->PickableOff();
	m_pSceneRenderer->AddActor(xLabel);

	vtkNew<vtkVectorText> yText;
	vtkNew<vtkPolyDataMapper> yTextMapper;
	yText->SetText("Y");
	yTextMapper->SetInputConnection(yText->GetOutputPort());
	yLabel->GetProperty()->SetColor(0.f, 1.f, 0.f);
	yLabel->SetMapper(yTextMapper);
	yLabel->SetScale(5);
	yLabel->SetCamera(m_pSceneRenderer->GetActiveCamera());
	yLabel->SetPosition(((vtkProp3D*)collection->GetItemAsObject(4))->GetCenter()); // YAxisTip
	yLabel->PickableOff();
	m_pSceneRenderer->AddActor(yLabel);

	vtkNew<vtkVectorText> zText;
	vtkNew<vtkPolyDataMapper> zTextMapper;
	zText->SetText("Z");
	zTextMapper->SetInputConnection(zText->GetOutputPort());
	zLabel->GetProperty()->SetColor(0.f, 0.f, 1.f);
	zLabel->SetMapper(zTextMapper);
	zLabel->SetScale(5);
	zLabel->SetCamera(m_pSceneRenderer->GetActiveCamera());
	zLabel->SetPosition(((vtkProp3D*)collection->GetItemAsObject(5))->GetCenter()); // ZAxisTip
	zLabel->PickableOff();
	m_pSceneRenderer->AddActor(zLabel);

	// custom callback to set the positions of the labels
	vtkNew<vtkPositionCallback> callback;
	callback->XLabel = xLabel;
	callback->YLabel = yLabel;
	callback->ZLabel = zLabel;
	callback->Axes = movableAxes;

	// create axsi in minus direction
	vtkNew<vtkPolyData> linesPolyData;

	double origin[3] = { 0., 0., 0. };
	double minus_x[3] = { -60., 0., 0. };
	double minus_y[3] = { 0., -60., 0. };
	double minus_z[3] = { 0., 0., -120. };

	vtkNew<vtkLineSource> lineSourceX;
	lineSourceX->SetResolution(30);
	lineSourceX->SetPoint1(origin);
	lineSourceX->SetPoint2(minus_x);
	lineSourceX->Update();

	vtkNew<vtkLineSource> lineSourceY;
	lineSourceY->SetResolution(30);
	lineSourceY->SetPoint1(origin);
	lineSourceY->SetPoint2(minus_y);
	lineSourceY->Update();

	vtkNew<vtkLineSource> lineSourceZ;
	lineSourceZ->SetResolution(60);
	lineSourceZ->SetPoint1(origin);
	lineSourceZ->SetPoint2(minus_z);
	lineSourceZ->Update();

	vtkNew<vtkPolyDataMapper> LineXMapper;
	LineXMapper->SetInputConnection(lineSourceX->GetOutputPort());
	LineXActor->SetMapper(LineXMapper);
	LineXActor->GetProperty()->SetLineWidth(1.);
	LineXActor->GetProperty()->SetColor(1.f, 0.f, 0.f);
	StippledLine(LineXActor, 0xA1A1, 2);

	vtkNew<vtkPolyDataMapper> LineYMapper;
	LineYMapper->SetInputConnection(lineSourceY->GetOutputPort());
	LineYActor->SetMapper(LineYMapper);
	LineYActor->GetProperty()->SetLineWidth(1.);
	LineYActor->GetProperty()->SetColor(0.f, 1.f, 0.f);
	StippledLine(LineYActor, 0xA1A1, 2);

	vtkNew<vtkPolyDataMapper> LineZMapper;
	LineZMapper->SetInputConnection(lineSourceZ->GetOutputPort());
	LineZActor->SetMapper(LineZMapper);
	LineZActor->GetProperty()->SetLineWidth(1.);
	LineZActor->GetProperty()->SetColor(0.f, 0.f, 1.f);
	StippledLine(LineZActor, 0xA1A1, 2);

	LineXActor->PickableOff(); // X점선
	LineYActor->PickableOff(); // Y점선
	LineZActor->PickableOff(); // Z점선
	m_pSceneRenderer->AddActor(LineXActor);
	m_pSceneRenderer->AddActor(LineYActor);
	m_pSceneRenderer->AddActor(LineZActor);

	m_pSceneRenderer->ResetCamera();
	this->renderWindow()->Render();

/*	vtkNew<vtkInteractorStyleTrackballActor> style;

	// an interactor
	vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
	renderWindowInteractor->SetRenderWindow(this->renderWindow());

	renderWindowInteractor->SetInteractorStyle(style);
	style->AddObserver(vtkCommand::InteractionEvent, callback);

	// begin mouse interaction
	renderWindowInteractor->Start();*/		

	runButton = new RunButton(this);
	Buttons_ETQVTKWidget.append(runButton);
	runButton->setFixedSize(QGuiApplication::primaryScreen()->geometry().width()*0.090, QGuiApplication::primaryScreen()->geometry().width()*0.030); // Adjust size as needed
	// Apply stylesheet for color and shape
	runButton->setStyleSheet("QPushButton {"
		"background-color: white;"
		"border: 0px;" // Half of the width/height for circular shape
		"}");
	connect(runButton, &QPushButton::clicked, this, &ETQVTKWidget::runButtonClicked);

	stopButton = new StopButton(this);
	Buttons_ETQVTKWidget.append(stopButton);
	stopButton->setFixedSize(QGuiApplication::primaryScreen()->geometry().width()*0.090, QGuiApplication::primaryScreen()->geometry().width()*0.030); // Adjust size as needed
	// Apply stylesheet for color and shape
	stopButton->setStyleSheet("QPushButton {"
		"background-color: white;"
		"border: 0px;" // Half of the width/height for circular shape
		"}");
	connect(stopButton, &QPushButton::clicked, this, &ETQVTKWidget::stopButtonClicked);
	stopButton->hide();

	resetButton = new ResetButton(this);
	Buttons_ETQVTKWidget.append(resetButton);
	resetButton->setFixedSize(QGuiApplication::primaryScreen()->geometry().width()*0.090, QGuiApplication::primaryScreen()->geometry().width()*0.030); // Adjust size as needed
	// Apply stylesheet for color and shape
	resetButton->setStyleSheet("QPushButton {"
		"background-color: white;"
		"border: 0px;" // Half of the width/height for circular shape
		"}");
	connect(resetButton, &QPushButton::clicked, this, &ETQVTKWidget::resetButtonClicked);
	resetButton->hide();

	settingButton = new SettingButton(this);
	Buttons_ETQVTKWidget.append(settingButton);
	// Apply stylesheet for color and shape
	settingButton->setStyleSheet("QPushButton {"
		"background-color: white;"

		"border: 0px;" // Half of the width/height for circular shape
		"}");
	connect(settingButton, &QPushButton::clicked, this, &ETQVTKWidget::settingButtonClicked);

	QHBoxLayout *buttonLayout = new QHBoxLayout();
	buttonLayout->addWidget(settingButton);
	buttonLayout->addSpacing(QGuiApplication::primaryScreen()->geometry().width() * 0.003); // Control the spacing between buttons
	buttonLayout->addWidget(runButton);
	buttonLayout->addWidget(stopButton);
	buttonLayout->addWidget(resetButton);

	QGridLayout *layout = new QGridLayout(this);
	layout->addLayout(buttonLayout, 0, 0, Qt::AlignBottom | Qt::AlignRight);
	this->setLayout(layout);
}

void ETQVTKWidget::ForceNonPickableForFixedObjects()
{
	collection->InitTraversal();
	for (int i = 0; i < collection->GetNumberOfItems(); ++i)
	{
		vtkProp3D * prop = (vtkProp3D*)collection->GetNextProp();
		prop->PickableOff(); // X,Y,Z 축 움직이지 않게 고정		
		movableAxes->AddPart(prop);
	}

	xLabel->PickableOff();
	yLabel->PickableOff();
	zLabel->PickableOff();
	LineXActor->PickableOff();
	LineYActor->PickableOff();
	LineZActor->PickableOff();
}

void ETQVTKWidget::SetVisibilityOn_AxesObjects()
{
	collection->InitTraversal();
	for (int i = 0; i < collection->GetNumberOfItems(); ++i)
	{
		vtkProp3D * prop = (vtkProp3D*)collection->GetNextProp();
		prop->SetVisibility(true); // X,Y,Z 축 움직이지 않게 고정		
		movableAxes->AddPart(prop);
	}

	xLabel->SetVisibility(true);
	yLabel->SetVisibility(true);
	zLabel->SetVisibility(true);
	LineXActor->SetVisibility(true);
	LineYActor->SetVisibility(true);
	LineZActor->SetVisibility(true);
}

void ETQVTKWidget::SetVisibilityOff_AxesObjects()
{
	collection->InitTraversal();
	for (int i = 0; i < collection->GetNumberOfItems(); ++i)
	{
		vtkProp3D * prop = (vtkProp3D*)collection->GetNextProp();
		prop->SetVisibility(false); // X,Y,Z 축 움직이지 않게 고정		
		movableAxes->AddPart(prop);
	}

	xLabel->SetVisibility(false);
	yLabel->SetVisibility(false);
	zLabel->SetVisibility(false);
	LineXActor->SetVisibility(false);
	LineYActor->SetVisibility(false);
	LineZActor->SetVisibility(false);
}

void ETQVTKWidget::runButtonClicked() {
	// 1. 사용자에게 시뮬레이션 실행 여부를 먼저 확인
	if (QMessageBox::No == theApp.SetMessageBox_YesNo("Do you want to run the simulation?")) {
		return; // '아니오'를 선택하면 함수를 즉시 종료
	}

	bool shouldRunCalculation = true; // 계산 실행 여부를 저장할 변수

	// 2. 저메모리 상태인지 확인하고, 그렇다면 사용자에게 경고
	if (theApp.isLowMemoryState()) {
		if (QMessageBox::No == theApp.SetMessageBox_YesNo("Warning: High Memory pressure detected;\nPotential performance degradation is possible.\nDo you want to continue?")) {
			shouldRunCalculation = false; // '아니오'를 선택하면 계산 실행 변수를 false로
		}
	}

	// 3. 최종 결정에 따라 계산 실행
	if (shouldRunCalculation) {
		theApp.pRt->triggerCalculate();

		// 4. 계산이 성공적으로 실행되면 UI 상태 변경
		if (theApp.pRt->b_IsCalculationExecuted) {
			runButton->hide();
			stopButton->show();
			resetButton->hide();
		}
	}
}
void ETQVTKWidget::stopButtonClicked() {
	if (QMessageBox::Yes == theApp.SetMessageBox_YesNo("Do you want to stop the simulation and save the results so far?"))
	{		
		theApp.pRt->DoseCalculation_StopAndSave();
		runButton->hide();
		stopButton->hide();
		resetButton->show();
	}
}
void ETQVTKWidget::resetButtonClicked() {
	theApp.pRt->ResetCalculation();
	runButton->show();
	stopButton->hide();
	resetButton->hide();

}
void ETQVTKWidget::settingButtonClicked() {
	theApp.pRt->triggerSetting();
}
void ETQVTKWidget::SetSceneRenderer(vtkSmartPointer<vtkRenderer> renderer)
{
	m_pSceneRenderer = renderer;
}

vtkSmartPointer<vtkRenderer> ETQVTKWidget::GetSceneRenderer()
{
	return m_pSceneRenderer;
}

void ETQVTKWidget::SetBackGroundColor()
{
	if (m_pSceneRenderer == nullptr)
		return;

	m_pSceneRenderer->SetGradientBackground(false);
	//m_pSceneRenderer->SetBackground2(110.0 / 255.0, 109.0 / 255.0, 227.0 / 255.0); // BOTTOM: Dark-Violet
	//m_pSceneRenderer->SetBackground(15.0 / 255.0, 15.0 / 255.0, 90.0 / 255.0); /// TOP	

	//m_pSceneRenderer->SetBackground2(168.0 / 255.0, 201.0 / 255.0, 229.0 / 255.0); /// TOP	
	//m_pSceneRenderer->SetBackground(95.0 / 255.0, 118.0 / 255.0, 137 / 255.0); // BOTTOM: Dark-Violet

	m_pSceneRenderer->SetBackground(1.f, 1.f, 1.f); /// TOP	

}

void ETQVTKWidget::StippledLine(vtkActor* actor, int lineStipplePattern, int lineStippleRepeat)
{
	vtkNew<vtkDoubleArray> tcoords;
	vtkNew<vtkImageData> image;
	vtkNew<vtkTexture> texture;

	// Create texture
	int dimension = 16 * lineStippleRepeat;

	image->SetDimensions(dimension, 1, 1);
	image->AllocateScalars(VTK_UNSIGNED_CHAR, 4);
	image->SetExtent(0, dimension - 1, 0, 0, 0, 0);
	unsigned char* pixel;
	pixel = static_cast<unsigned char*>(image->GetScalarPointer());
	unsigned char on = 255;
	unsigned char off = 0;
	for (int i = 0; i < 16; ++i)
	{
		unsigned int mask = (1 << i);
		unsigned int bit = (lineStipplePattern & mask) >> i;
		unsigned char value = static_cast<unsigned char>(bit);
		if (value == 0)	{
			for (int j = 0; j < lineStippleRepeat; j++)	{
				*pixel = on;
				*(pixel + 1) = on;
				*(pixel + 2) = on;
				*(pixel + 3) = off;
				pixel += 4;
			}
		}
		else {
			for (int j = 0; j < lineStippleRepeat; j++)	{
				*pixel = on;
				*(pixel + 1) = on;
				*(pixel + 2) = on;
				*(pixel + 3) = on;
				pixel += 4;
			}
		}
	}

	vtkPolyData* polyData =	dynamic_cast<vtkPolyData*>(actor->GetMapper()->GetInput());

	// Create texture coordnates
	tcoords->SetNumberOfComponents(1);
	tcoords->SetNumberOfTuples(polyData->GetNumberOfPoints());
	for (int i = 0; i < polyData->GetNumberOfPoints(); ++i)
	{
		double value = static_cast<double>(i) * .5;
		tcoords->SetTypedTuple(i, &value);
	}

	polyData->GetPointData()->SetTCoords(tcoords);
	texture->SetInputData(image);
	texture->InterpolateOff();
	texture->RepeatOn();

	actor->SetTexture(texture);
}