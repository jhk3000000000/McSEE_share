#pragma once
#include <QWidget>
#include <QLabel>
#include "FunctionPanelRight.h"
#include "StyleSheet.h"
#include "qcustomplot.h"

using namespace utils::ui;

class RealTimeResultPanel : public QWidget
{

public:
	explicit RealTimeResultPanel();
	virtual ~RealTimeResultPanel();

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

	QFont font_RTP_GBW1;
	QFont font_RTP_LBL1;

	double widgetSizeRatioFactor;
	bool flg_GraphUpdate = false;
	
	QScrollArea* RealtimeResultScroll_OrganDose_QscrollArea = nullptr;
	QScrollArea* RealtimeResultScroll_Dosimeter_QscrollArea = nullptr;
	QGroupBox* RealtimeResultPanel_OrganDose_QGroupBox = nullptr;
	QGroupBox* RealtimeResultPanel_Dosimeter_QGroupBox = nullptr;
	QVBoxLayout* Layout_OrganDose_RealtimeLineGraph_QVBoxLayout = nullptr;
	QVBoxLayout* Layout_Dosimeter_RealtimeLineGraph_QVBoxLayout = nullptr;
	std::map<int, QGroupBox*> groupbox_Graph_map; //groupbox_Graph_map[phantomID] = *QGroupBox
	std::map<int, QVBoxLayout*> layoutIn_Graph_map; //layoutIn_Graph_map[phantomID] = *QVBoxLayout
	std::map<int, QCustomPlot*> OrganDose_RealtimeLineGraph_QCustomPlot; // OrganDose_RealtimeLineGraph_QCustomPlot[Organ or dosimeter ID] = *QcustomPlot
	std::map<int, QCustomPlot*> Dosimeter_RealtimeLineGraph_QCustomPlot; // OrganDose_RealtimeLineGraph_QCustomPlot[Organ or dosimeter ID] = *QcustomPlot
	
public:
	void CreateWidgets();
	void CreateWidgets_OrganDose();	
	void CreateWidgets_Dosimeter();
	void Create_OrganDose_RealtimeLineGraphWidgets(QVBoxLayout* layout, int OrganID);
	void Create_Dosimeter_RealtimeLineGraphWidgets(QVBoxLayout* layout, int OrganID);
	void RealtimeGraphUpdate();
	void RealtimeGraphUpdate_TimeDose();
	void RealtimeGraphUpdate_OrganDose();
	void ForcedGraphUpdate();
	void ForcedGraphUpdate_TimeDose();
	void ForcedGraphUpdate_OrganDose();
	void ForcedGraphUpdate_H_AK_Dose();

	QTimer *timer_RealtimeGraphUpdate;

};