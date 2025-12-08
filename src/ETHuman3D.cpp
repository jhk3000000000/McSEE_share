#include "pch.h"
#include "ETHuman3D.h"
#include "ETHuman3DApp.h"
#include "ETQVTKWidget.h"
#include "ProcessInformationPanel.h"
#include "RealTimeResultPanel.h"
#include "FunctionPanelTop.h"
#include "FunctionPanelRight.h"
#include "qtimer.h"
#include "SceneDataTypes.h"
#include "TaskManager.h"
#include <QFileInfo>
#include <QMessageBox>
#include <qdatastream.h>

ETHuman3D::ETHuman3D(QWidget *parent)
	: QWidget(parent)
{}

void ETHuman3D::Initialize()
{
// --- 기존 생성자에 있던 코드를 그대로 가져옵니다 ---

	// 타이틀
	// McSEE_local
	setWindowTitle("Monte Carlo Simulation for External Exposure (McSEE)");

	// Loading Bar 생성 및 표시
	QDialog progressDialog;
	progressDialog.setWindowFlag(Qt::FramelessWindowHint);
	progressDialog.setAttribute(Qt::WA_TranslucentBackground);
	progressDialog.setStyleSheet("background-color: rgb(255, 255, 255);");
    
    // window_width, window_height는 헤더에서 초기화되었으므로 여기서 사용 가능합니다.
	progressDialog.setGeometry(window_width * 0.5 - window_width * 0.112, window_height * 0.5 + window_height * 0.03, window_width * 0.20, window_height * 0.03);
	
    // [중요] Singleton으로 바꿨어도 #define 매크로 덕분에 theApp을 그대로 쓸 수 있습니다.
    theApp.LoadingBar = new QProgressBar();
	QVBoxLayout layout;
	layout.addWidget(theApp.LoadingBar);
	progressDialog.setLayout(&layout);
	theApp.LoadingBar->setStyleSheet(ProcessInformationPanelStyle::PROGRESS_BAR);
	QFont font_bar("Arial", 10, QFont::Bold); 
	theApp.LoadingBar->setFont(font_bar);
	theApp.LoadingBar->setFixedHeight(window_height * 0.02);
	theApp.LoadingBar->setRange(0, 100);
	theApp.LoadingBar->setValue(0);
	theApp.LoadingBar->setTextVisible(true);
	theApp.LoadingBar->show();
	
    // 다이얼로그 표시 (UI 갱신)
	progressDialog.show();
	QApplication::processEvents();
	theApp.LoadingBar->setFormat("Constructing Visualization Window");
	theApp.LoadingBar->setValue(10);

	// ETQVTKWidget 생성 (무거운 작업)
	theApp.m_pVTKWidget = new ETQVTKWidget; 
	theApp.m_pVTKWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	
	// ControlPanel 생성 (무거운 작업)
	theApp.pTop = new FunctionPanelTop;	
	theApp.pRt = new FunctionPanelRight;
	theApp.LoadingBar->setFormat("Finished");
	theApp.LoadingBar->setValue(100);
	theApp.pRslt = new RealTimeResultPanel;
	theApp.pInf = new ProcessInformationPanel;

	// 레이아웃 설정
	QVBoxLayout* mainLayout = new QVBoxLayout();
	mainLayout->setAlignment(Qt::AlignTop);
	mainLayout->setContentsMargins(0.0005 * window_width, 0.0010 * window_height, 0.0005 * window_width, 0.0010 * window_height);
	mainLayout->setSpacing(0.0005 * window_width);

	QVBoxLayout* vLayout0 = new QVBoxLayout;
	vLayout0->setAlignment(Qt::AlignTop);
	vLayout0->setContentsMargins(0, 0, 0, 0);
	vLayout0->setSpacing(0.0005 * window_width);

	{
		QHBoxLayout* subLayout = new QHBoxLayout;
		subLayout->setAlignment(Qt::AlignTop);
		subLayout->setContentsMargins(0, 0, 0, 0);
		subLayout->setSpacing(0);
		subLayout->addWidget(theApp.pTop);

		vLayout0->addLayout(subLayout);
	}

	QHBoxLayout* hLayout1 = new QHBoxLayout;
	hLayout1->setAlignment(Qt::AlignTop);
	hLayout1->setContentsMargins(0, 0, 0, 0);
	hLayout1->setSpacing(0.0005 * window_width);

	{
		QHBoxLayout* subLayout = new QHBoxLayout;
		subLayout->setAlignment(Qt::AlignTop);
		subLayout->setContentsMargins(0, 0, 0, 0);
		subLayout->setSpacing(0);
		subLayout->addWidget(theApp.pRt);

		hLayout1->addLayout(subLayout);
	}

	{
		QHBoxLayout* subLayout = new QHBoxLayout;
		subLayout->setAlignment(Qt::AlignTop);
		subLayout->setContentsMargins(0, 0, 0, 0);
		subLayout->setSpacing(0);
		subLayout->addWidget(theApp.pRslt);

		hLayout1->addLayout(subLayout);
		theApp.pRslt->hide(); 
	}

	{
		QVBoxLayout* subLayout = new QVBoxLayout;
		subLayout->setAlignment(Qt::AlignTop);
		subLayout->setContentsMargins(0, 0, 0, 0);
		subLayout->setSpacing(0.0005 * window_width);
		{
			QHBoxLayout* subsubLayout = new QHBoxLayout;
			subsubLayout->setAlignment(Qt::AlignTop);
			subsubLayout->setContentsMargins(0, 0, 0, 0);
			subsubLayout->setSpacing(0);
			subsubLayout->addWidget(theApp.pInf);

			subLayout->addLayout(subsubLayout);
			theApp.pInf->hide(); 
		}

		{
			QHBoxLayout* subsubLayout = new QHBoxLayout;
			subsubLayout->setAlignment(Qt::AlignTop);
			subsubLayout->setContentsMargins(0, 0, 0, 0);
			subsubLayout->setSpacing(0);
			subsubLayout->addWidget(theApp.m_pVTKWidget);

			subLayout->addLayout(subsubLayout);
		}
		hLayout1->addLayout(subLayout);
	}

	mainLayout->addLayout(vLayout0);
	mainLayout->addLayout(hLayout1);

	// drag && drop 설정
	theApp.m_pVTKWidget->setAcceptDrops(false);
	setAcceptDrops(true);

    // 최종 레이아웃 적용
	setLayout(mainLayout);

	theApp.m_pVTKWidget->GetBaseInteractorStyle()->CallOnLeftButton(); 

	theApp.LoadingBar->hide();
	delete theApp.LoadingBar;
    
    // (팁) 로딩바 포인터가 댕글링 포인터가 되지 않도록 nullptr 처리
    theApp.LoadingBar = nullptr;
}
ETHuman3D::~ETHuman3D()
{
	SAFE_DELETE(theApp.pTop);
	SAFE_DELETE(theApp.pRt);
	SAFE_DELETE(theApp.m_pVTKWidget);
	SAFE_DELETE(theApp.pInf);
}

void ETHuman3D::Load_mcsee_File_autoload_through_maincc(const QString& autoloadPath)
{
	if (autoloadPath.isEmpty()) return;

	QTimer::singleShot(0, this, [autoloadPath]() {
		theApp.pRt->b_Is_Reconstruction = true;

		QFile* file = new QFile(autoloadPath);
		// 텍스트 모드가 아닌 바이너리 읽기 모드로 엽니다.
		if (!file->open(QIODevice::ReadOnly)) {
			QMessageBox::critical(nullptr, "File Error", "Could not open autoload file: " + autoloadPath);
			delete file;
			theApp.pRt->b_Is_Reconstruction = false;
			return;
		}

		// --- 1단계: 파싱 ---
		SceneData loadedSceneData;
		loadedSceneData.sourceFile = file; // .mcsee파일을 sourcefile로 할당
		theApp.pRt->Load_parse_mcsee_FileData(loadedSceneData);

		// --- 2단계: 대용량 데이터 파일 추출 ---
		QFileInfo mcseeFileInfo(autoloadPath);
		QString outputDir = mcseeFileInfo.path(); // .mcsee 파일이 있는 디렉토리 경로

		// Key: 리소스 ID, Value: 새로 생성된 파일의 전체 경로 -> 파싱한 뒤에, .mcsee 파일 경로를 scenedata에 저장
		QMap<QString, QString> extractedFilePaths; // .mcsee 파일이 있는 디렉토리 경로에 저장해야 하므로, scenedata 구조체에 load하는 시점에 저장하게 됨

		for (const auto& resInfo : loadedSceneData.resourceManifest) {
			QString outputFilePath = outputDir + "/" + resInfo.originalName;

			QFile outputFile(outputFilePath);
			if (!outputFile.open(QIODevice::WriteOnly)) {
				qWarning() << "Could not create file for extraction:" << outputFilePath;
				continue;
			}

			// .mcsee 파일에서 데이터 위치로 이동
			file->seek(resInfo.offset);

			// 메모리 사용을 최소화하기 위해 청크 단위로 읽고 쓰기
			qint64 bytesRemaining = resInfo.size;
			char buffer[65536];
			while (bytesRemaining > 0) {
				qint64 bytesToRead = qMin((qint64)sizeof(buffer), bytesRemaining);
				qint64 bytesRead = file->read(buffer, bytesToRead);
				if (bytesRead <= 0) break;
				outputFile.write(buffer, bytesRead);
				bytesRemaining -= bytesRead;
			}
			outputFile.close();
			extractedFilePaths[resInfo.id] = outputFilePath;
			qDebug() << "Extracted file:" << outputFilePath;
		}

		// 추출된 파일 경로를 PhantomData 객체에 연결
		for (PhantomData& pData : loadedSceneData.phantoms) {

			if (!pData.objResourceId.isEmpty()) {
				pData.objFilePath = extractedFilePaths.value(pData.objResourceId);
			}
			if (!pData.eleResourceId.isEmpty()) {
				pData.eleFilePath = extractedFilePaths.value(pData.eleResourceId);
			}
			if (!pData.nodeResourceId.isEmpty()) {
				pData.nodeFilePath = extractedFilePaths.value(pData.nodeResourceId);
			}
		}

		// --- 3단계: 커밋 ---
		theApp.pRt->Load_commit_mcsee_FileData(loadedSceneData);
		QMessageBox::information(nullptr, "Success", "Project loaded successfully.");

		// 로드 완료 후 파일 닫기 및 메모리 해제
		file->close();
		delete file;
		loadedSceneData.sourceFile = nullptr;

		theApp.pRt->b_Is_Reconstruction = false;
		});
}

void ETHuman3D::paintEvent(QPaintEvent *event)
{
	QSize windowSize = this->size();
	int height = windowSize.height();
	int width = windowSize.width();
}

void ETHuman3D::resizeEvent(QResizeEvent *event)
{
	QSize windowSize = this->size();
	int height = windowSize.height();
	int width = windowSize.width();
}

//void ETHuman3D::resizeEvent(QResizeEvent *event) // Fixed ratio resizing(16:9 유지 resizing)
//{
//	// Desired aspect ratio
//	const float aspectRatio = 16.0 / 9.0;
//
//	// Calculate width based on current height
//	int newWidth = aspectRatio * event->size().height();
//
//	// Resize to new width while keeping the same height
//	resize(newWidth, event->size().height());
//
//	QWidget::resizeEvent(event);
//}

// Drag & Drop
void ETHuman3D::dragEnterEvent(QDragEnterEvent* event)
{
	if (event->mimeData()->hasUrls())
	{
		event->acceptProposedAction();
	}
}

// Drag & Drop
void ETHuman3D::dropEvent(QDropEvent *event)
{
	QList<QUrl> urls = event->mimeData()->urls();
	if (urls.isEmpty())
		return;

	QString fileName = urls.first().toLocalFile();
	if (fileName.isEmpty())
		return;

	theApp.RestartProgram();

	if (fileName.contains(".ply"))
	{
		int tmp = 0;
		

	}
	else if (fileName.contains(".stl"))
	{
		

	}
	else if (fileName.contains(".obj"))
	{
		int tmp = 0; // 나중에 오버로딩 할 것
	}
}

void ETHuman3D::closeEvent(QCloseEvent *event)
{
	// 종료 확인 메시지 박스 표시
	QMessageBox::StandardButton reply;
	reply = QMessageBox::question(this, "McSEE", "Do you want to exit the program?",
		QMessageBox::Yes | QMessageBox::No);

	if (reply == QMessageBox::Yes) {
		// "Yes" 클릭 시 정상적으로 종료
		event->accept();
	}
	else {
		// "No" 클릭 시 종료하지 않음
		event->ignore();
	}
}