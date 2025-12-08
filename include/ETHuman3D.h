#pragma once

#include <QtWidgets/QMainWindow>
#include <QScreen> // for Qt6
#include <QGuiApplication>
class ETHuman3D : public QWidget
{
	Q_OBJECT

public:
	explicit ETHuman3D(QWidget *parent = Q_NULLPTR);
	virtual ~ETHuman3D();

	// [추가] 초기화 함수 선언
    void Initialize();
	
public:
	void Load_mcsee_File_autoload_through_maincc(const QString& autoloadPath); // Load 용
	QScreen *screen = QApplication::primaryScreen();
	//Resolution
	double window_width = screen->geometry().width();
	double window_height = screen->geometry().height();
	//double window_width = QApplication::desktop()->screenGeometry().width();
	//double window_height = QApplication::desktop()->screenGeometry().height();
	double RatioWtoH = window_width / window_height;
	//DPI
	qreal logicalDPI = screen->logicalDotsPerInch();
	qreal physicalDPI = screen->physicalDotsPerInch();
	//Physical Inch
	QSizeF physicalSizeMM = screen->physicalSize();
	qreal physicalWidthInInches = physicalSizeMM.width() / 25.4;
	qreal physicalHeightInInches = physicalSizeMM.height() / 25.4;
	double physicalSizeInches = std::sqrt(physicalWidthInInches * physicalWidthInInches + physicalHeightInInches * physicalHeightInInches);
	double physFactor = physicalSizeInches * physicalDPI;

	double default_window_width = 2560.;
	double default_window_height = 1440.;
	double default_RatioWtoH = 2560. / 1440.; // this is just 16:9
	double default_logicalDPI = 144.; // 150% 배율
	double default_physFactor = 32. * 93.; // 32 Inch * 93 PhysicalDPI (32inch, QHD(2560 * 1440))

	int FontSizeScaling(int DefaultFontSize)
	{
		double PhysPower = 1.2;
		double logiDPIPower = 0.9;
		double ResRatioPower = 0.6;
		//theApp.SetMessageBox("PhysFactor: " + QString::number(PhysFactor));
		//theApp.SetMessageBox("Default_physFactor: " + QString::number(Default_physFactor));
		//theApp.SetMessageBox("Default_logicalDPI: " + QString::number(Default_logicalDPI)  );
		//theApp.SetMessageBox("LogicalDPI: " + QString::number(LogicalDPI));
		//theApp.SetMessageBox("Default_RatioWtoH: " + QString::number(Default_RatioWtoH));
		//theApp.SetMessageBox("RatioWtoH: " + QString::number(RatioWtoH));
		double scale_factor = std::pow(physFactor / default_physFactor, PhysPower) * std::pow(default_logicalDPI / logicalDPI, logiDPIPower) * std::pow(default_RatioWtoH / RatioWtoH, ResRatioPower);
		int calibration = 0;
		if (RatioWtoH <= 1.5) calibration++; // 해상도 세로가 많이 길 때
		if (window_width <= 1500.) calibration++; // 가로 해상도 자체가 많이 작을 때
		//theApp.SetMessageBox("calibration: " + QString::number(calibration));
		return std::floor(DefaultFontSize * scale_factor) - calibration;
	}

protected:
	void paintEvent(QPaintEvent *event);
	//virtual void resizeEvent(QResizeEvent *event) override; // Fixed ratio resizing
	void resizeEvent(QResizeEvent *event);
	void dragEnterEvent(QDragEnterEvent* event);
	void dropEvent(QDropEvent *event);
	void closeEvent(QCloseEvent *event) override;
};
