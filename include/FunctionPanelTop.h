#pragma once

#include <QtWidgets>
#include "ETHuman3DApp.h"
#include "StyleSheet.h"

using namespace utils::ui;
class FunctionPanelTop : public QWidget
{
	Q_OBJECT

public:
	explicit FunctionPanelTop(QWidget *parent = Q_NULLPTR);
	virtual ~FunctionPanelTop();

protected:
	virtual void paintEvent(QPaintEvent *event) override;

public:
	QPushButton* m_pShadeButton = nullptr;
	QPushButton* m_pFlatButton = nullptr;
	QPushButton* m_pWireFrameButton = nullptr;
	QPushButton* m_pPointsButton = nullptr;

	QPushButton* m_pView3DModelButton = nullptr;
	QPushButton* m_pViewBoundaryBoxButton = nullptr;
	QPushButton* m_pViewAnnotationButton = nullptr;

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

	QFont font_MP_BTN1;
	QFont font_MP_GBW1;

protected:
	void CreateWidgets();

private slots:
	void slot_fpt_buttonLoad_ButtonClicked();
	void slot_fpt_buttonSave_ButtonClicked();

	/*void slot_Shade_ButtonClicked();
	void slot_Flat_ButtonClicked();
	void slot_WireFrame_ButtonClicked();
	void slot_Points_ButtonClicked();*/
	void slot_ViewportTop_ButtonClicked();
	void slot_ViewportBottom_ButtonClicked();
	void slot_ViewportLeft_ButtonClicked();
	void slot_ViewportRight_ButtonClicked();
	void slot_ViewportFront_ButtonClicked();
	void slot_ViewportBack_ButtonClicked();
	//void slot_ViewModel_ButtonClicked();
	//void slot_ViewBoundaryBox_ButtonClicked();
	//void slot_ViewAnnotation_ButtonClicked();
};