#pragma once

#include <QtWidgets>
#include <QWidget>
#include <QLabel>
#include <ETHuman3D.h>

#include "StyleSheet.h"

using namespace utils::ui;

class ProcessInformationPanel : public QWidget
{
	Q_OBJECT

public:
	explicit ProcessInformationPanel(QWidget *parent = Q_NULLPTR);
	virtual ~ProcessInformationPanel();

public:
	QScreen *screen = QApplication::primaryScreen();
	//Resolution
	double window_width = QGuiApplication::primaryScreen()->geometry().width();
	double window_height = QGuiApplication::primaryScreen()->geometry().height();
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

	double specialWidgetSizeFactor = std::pow(default_logicalDPI / logicalDPI, 0.1) * std::pow(physFactor / default_physFactor, 0.1);

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

	QFont font_PI_GBW1;
	QFont font_PI_LBL1;
	QFont font_PI_BTN1;
	QFont font_PI_BAR1;

	double widgetSizeRatioFactor;

	QLabel* labelMemUsage = nullptr;
	QLabel* labelThreadUsage = nullptr;
	QLabel* labelNowTimeRemaining = nullptr;
	QLabel* labelRunTime = nullptr;
	QLabel* labelTransportedParticles = nullptr;
	QLabel* labelPageFault = nullptr;
	   	
	QProgressBar* bar = nullptr;
	QLabel* ProgressLabel = nullptr;
	QPushButton * StopSaveButton = nullptr;

	
public:
	void CreateWidgets();

private slots:
	void slot_StopSave_ButtonClicked();


};