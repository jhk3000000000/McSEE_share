#include "pch.h"
#include "ETHuman3D.h"
#include "ETHuman3DApp.h"

#include <QtWidgets/QApplication>
#include <QSplashScreen>
#include <QPixmap>
//#include <QDesktopWidget> // Qt 5에서 화면 정보 - Qt 6에서는 deprecated 됨.
#include <QVTKOpenGLNativeWidget.h> // VTK

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/McSEE.ico"));

    // [재시작-로드] 인자 파싱
    QCommandLineParser parser;
    parser.setApplicationDescription("McSEE");
    parser.addHelpOption();
    QCommandLineOption autoloadOpt("autoload", "Autoload a result file on startup", "file");
    parser.addOption(autoloadOpt);
    parser.process(a);

    QPixmap pixmap("./data/image/McSEE.png");
    QSplashScreen splash(pixmap);
    splash.show();
    QApplication::processEvents();

    // 1. 객체 생성 (이제 생성자는 텅 비어있어서 아주 가볍고 빠릅니다)
    ETHuman3D w; 

    // 2. [추가됨] 초기화 함수 호출 (가장 중요!)
    // 이 함수 안에서 theApp.pTop, theApp.pRt, VTKWidget 등이 모두 new로 생성됩니다.
    // 이 줄이 없거나 순서가 뒤로 밀리면, 프로그램이 바로 죽습니다(Crash).
    w.Initialize(); 

    // 3. 윈도우 크기 설정 및 표시
    // Initialize()가 UI를 다 만들었으므로, 이제 안심하고 보여줄 수 있습니다.
    w.setMinimumSize(QGuiApplication::primaryScreen()->geometry().width() * 0.9975,
                     QGuiApplication::primaryScreen()->geometry().height() * 0.980);
    w.showMaximized();

    // 4. [재시작-로드] 수행
    // 위에서 Initialize()를 했기 때문에 theApp.pRt가 존재합니다. 따라서 안전하게 로드 가능합니다.
    w.Load_mcsee_File_autoload_through_maincc(parser.value(autoloadOpt));

    splash.finish(&w);
    return a.exec();
}

//아래는 원본입니다
//int main(int argc, char *argv[])
//{
//	//QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
//	QApplication a(argc, argv);
//		
//	QPixmap pixmap("./data/image/McSEE.png");
//	QSplashScreen splash(pixmap);
//	splash.show();
//	QApplication::processEvents(); // Splash가 실행되게끔 함
//
//	ETHuman3D w;
//
//	w.setMinimumSize(QApplication::desktop()->screenGeometry().width() * 0.9975, QApplication::desktop()->screenGeometry().height() * 0.980);
//	w.showMaximized();		
//
//	splash.finish(&w);	
//
//	return a.exec();
//}
