#include "pch.h"

#include "RealTimeResultPanel.h"
#include "ETHuman3DApp.h"

#include "Util.h"
#include <QRandomGenerator>

#ifndef getmax
#define getmax(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef getmin
#define getmin(a,b)            (((a) < (b)) ? (a) : (b))
#endif

RealTimeResultPanel::RealTimeResultPanel()
{
	widgetSizeRatioFactor = (0.38 * (1 - 0.3 * default_RatioWtoH / RatioWtoH)) / 0.265; // 전체에서 PanelRight output이 늘어난만큼에서 빼고, realtimePanel과 processInformationPanel이 동일한 비율(0.38)로 나눠가짐. 0.265는 origianl width 비율
	window_width *= widgetSizeRatioFactor; // 화면비 calibration

	font_RTP_GBW1.setPointSize(FontSizeScaling(10));
	font_RTP_LBL1.setPointSize(FontSizeScaling(10));

	// size
	setFixedWidth(window_width * 0.265);
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	setStyleSheet(ProcessInformationPanelStyle::BACKGROUND_WIDGET);

	CreateWidgets();

	timer_RealtimeGraphUpdate = new QTimer(this);
	connect(timer_RealtimeGraphUpdate, &QTimer::timeout, this, &RealTimeResultPanel::RealtimeGraphUpdate);
	
}

RealTimeResultPanel::~RealTimeResultPanel()
{

}

void RealTimeResultPanel::CreateWidgets()
{
	QVBoxLayout* layoutMain = new QVBoxLayout;
	assert(layoutMain);
	layoutMain->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

	CreateWidgets_OrganDose();
	// Create a scroll area for the Organ Dose group box
	RealtimeResultScroll_OrganDose_QscrollArea = new QScrollArea();	
	RealtimeResultScroll_OrganDose_QscrollArea->setStyleSheet(ProcessInformationPanelStyle::GROUPBOX_WIDGET);
	RealtimeResultScroll_OrganDose_QscrollArea->setFont(font_RTP_GBW1);
	RealtimeResultScroll_OrganDose_QscrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	RealtimeResultScroll_OrganDose_QscrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	RealtimeResultScroll_OrganDose_QscrollArea->setWidget(RealtimeResultPanel_OrganDose_QGroupBox);
	RealtimeResultScroll_OrganDose_QscrollArea->setWidgetResizable(true);
	
	CreateWidgets_Dosimeter();
	// Create a scroll area for the Organ Dose group box
	RealtimeResultScroll_Dosimeter_QscrollArea = new QScrollArea();
	RealtimeResultScroll_Dosimeter_QscrollArea->hide();
	RealtimeResultScroll_Dosimeter_QscrollArea->setStyleSheet(ProcessInformationPanelStyle::GROUPBOX_WIDGET);
	RealtimeResultScroll_Dosimeter_QscrollArea->setFont(font_RTP_GBW1);
	RealtimeResultScroll_Dosimeter_QscrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	RealtimeResultScroll_Dosimeter_QscrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	RealtimeResultScroll_Dosimeter_QscrollArea->setWidget(RealtimeResultPanel_Dosimeter_QGroupBox);
	RealtimeResultScroll_Dosimeter_QscrollArea->setWidgetResizable(true);

	layoutMain->addWidget(RealtimeResultScroll_OrganDose_QscrollArea);
	layoutMain->addWidget(RealtimeResultScroll_Dosimeter_QscrollArea);

	setLayout(layoutMain);
}
void RealTimeResultPanel::CreateWidgets_OrganDose()
{
	RealtimeResultPanel_OrganDose_QGroupBox = new QGroupBox;
	RealtimeResultPanel_OrganDose_QGroupBox->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	RealtimeResultPanel_OrganDose_QGroupBox->setStyleSheet(ProcessInformationPanelStyle::GROUPBOX_WIDGET);
	RealtimeResultPanel_OrganDose_QGroupBox->setFont(font_RTP_GBW1);
	RealtimeResultPanel_OrganDose_QGroupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	Layout_OrganDose_RealtimeLineGraph_QVBoxLayout = new QVBoxLayout;
	Layout_OrganDose_RealtimeLineGraph_QVBoxLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

	RealtimeResultPanel_OrganDose_QGroupBox->setLayout(Layout_OrganDose_RealtimeLineGraph_QVBoxLayout);	
}

void RealTimeResultPanel::CreateWidgets_Dosimeter()
{
	RealtimeResultPanel_Dosimeter_QGroupBox = new QGroupBox;
	RealtimeResultPanel_Dosimeter_QGroupBox->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	RealtimeResultPanel_Dosimeter_QGroupBox->setStyleSheet(ProcessInformationPanelStyle::GROUPBOX_WIDGET);
	RealtimeResultPanel_Dosimeter_QGroupBox->setFont(font_RTP_GBW1);
	RealtimeResultPanel_Dosimeter_QGroupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	Layout_Dosimeter_RealtimeLineGraph_QVBoxLayout = new QVBoxLayout;
	Layout_Dosimeter_RealtimeLineGraph_QVBoxLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

	RealtimeResultPanel_Dosimeter_QGroupBox->setLayout(Layout_Dosimeter_RealtimeLineGraph_QVBoxLayout);
}


void RealTimeResultPanel::Create_OrganDose_RealtimeLineGraphWidgets(QVBoxLayout* layoutIn_graph, int organORdosimID)
{
	QHBoxLayout* Layout = new QHBoxLayout;
	Layout->setContentsMargins(window_width * 0.005, window_height * 0.025, 0, 0);
	Layout->setSpacing(0);
	Layout->setAlignment(Qt::AlignCenter | Qt::AlignTop);

	OrganDose_RealtimeLineGraph_QCustomPlot[organORdosimID] = new QCustomPlot();
	OrganDose_RealtimeLineGraph_QCustomPlot[organORdosimID]->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	OrganDose_RealtimeLineGraph_QCustomPlot[organORdosimID]->setFixedSize(window_width * 0.225 * specialWidgetSizeFactor, window_height * 0.260); // customplot은 한 번 더 줄이기	

	Layout->addWidget(OrganDose_RealtimeLineGraph_QCustomPlot[organORdosimID]);
	layoutIn_graph->addLayout(Layout);

	QVector<double> x(0), y(0);
	OrganDose_RealtimeLineGraph_QCustomPlot[organORdosimID]->addGraph();
	OrganDose_RealtimeLineGraph_QCustomPlot[organORdosimID]->graph(0)->setData(x, y);

	QString organORdosimName;
	organORdosimName = theApp.pRt->OrganNameList[organORdosimID];

	QCPTextElement *title = new QCPTextElement(OrganDose_RealtimeLineGraph_QCustomPlot[organORdosimID], organORdosimName, QFont("sans", FontSizeScaling(13), QFont::Bold));
	OrganDose_RealtimeLineGraph_QCustomPlot[organORdosimID]->plotLayout()->insertRow(0); // Insert an empty row in the plot layout
	OrganDose_RealtimeLineGraph_QCustomPlot[organORdosimID]->plotLayout()->addElement(0, 0, title); // Add the title to the left of the new row
	double Error = 0;
	QString errorStr = QString("Statistical relative error: %1% ").arg(QString::number(Error, 'f', 2));
	QCPTextElement *ErrorText = new QCPTextElement(OrganDose_RealtimeLineGraph_QCustomPlot[organORdosimID], errorStr, QFont("sans", FontSizeScaling(10), QFont::Bold));
	ErrorText->setTextFlags(Qt::AlignRight);
	OrganDose_RealtimeLineGraph_QCustomPlot[organORdosimID]->plotLayout()->insertRow(1); // Insert an empty row in the plot layout
	OrganDose_RealtimeLineGraph_QCustomPlot[organORdosimID]->plotLayout()->addElement(1, 0, ErrorText); // Add the error to the right of the new row
	
	OrganDose_RealtimeLineGraph_QCustomPlot[organORdosimID]->xAxis->setLabel("Running time (sec)");
	OrganDose_RealtimeLineGraph_QCustomPlot[organORdosimID]->xAxis->setLabelFont(QFont("sans", FontSizeScaling(11))); // x축 라벨 폰트
	OrganDose_RealtimeLineGraph_QCustomPlot[organORdosimID]->xAxis->setTickLabelFont(QFont("sans", FontSizeScaling(8))); // x축 값 폰트

	OrganDose_RealtimeLineGraph_QCustomPlot[organORdosimID]->yAxis->setLabel("Dose (pGy" + theApp.pRt->DoseUnit_QString + " or pSv" + theApp.pRt->DoseUnit_QString + ")");
	OrganDose_RealtimeLineGraph_QCustomPlot[organORdosimID]->yAxis->setLabelFont(QFont("sans", FontSizeScaling(8))); // y축 라벨 폰트
	OrganDose_RealtimeLineGraph_QCustomPlot[organORdosimID]->yAxis->setTickLabelFont(QFont("sans", FontSizeScaling(8))); // y축 값 폰트
	OrganDose_RealtimeLineGraph_QCustomPlot[organORdosimID]->yAxis->setNumberFormat("e"); // "e" for scientific notation and "b" to use beautiful decimal powers
	OrganDose_RealtimeLineGraph_QCustomPlot[organORdosimID]->yAxis->setNumberPrecision(2); // This sets the number to be like "1.4e+03"

	OrganDose_RealtimeLineGraph_QCustomPlot[organORdosimID]->rescaleAxes();
	OrganDose_RealtimeLineGraph_QCustomPlot[organORdosimID]->replot();
}

void RealTimeResultPanel::Create_Dosimeter_RealtimeLineGraphWidgets(QVBoxLayout* layoutIn_graph, int organORdosimID)
{
	for (int dosim_type = 0; dosim_type < 3; dosim_type++)
	{
		QHBoxLayout* Layout = new QHBoxLayout;
		Layout->setContentsMargins(window_width * 0.005, window_height * 0.025, 0, 0);
		Layout->setSpacing(0);
		Layout->setAlignment(Qt::AlignCenter | Qt::AlignTop);

		Dosimeter_RealtimeLineGraph_QCustomPlot[organORdosimID] = new QCustomPlot();
		Dosimeter_RealtimeLineGraph_QCustomPlot[organORdosimID]->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		Dosimeter_RealtimeLineGraph_QCustomPlot[organORdosimID]->setFixedSize(window_width * 0.225 * specialWidgetSizeFactor, window_height * 0.260); // customplot은 한 번 더 줄이기	

		Layout->addWidget(Dosimeter_RealtimeLineGraph_QCustomPlot[organORdosimID]);
		layoutIn_graph->addLayout(Layout);

		QVector<double> x(0), y(0);
		Dosimeter_RealtimeLineGraph_QCustomPlot[organORdosimID]->addGraph();
		Dosimeter_RealtimeLineGraph_QCustomPlot[organORdosimID]->graph(0)->setData(x, y);

		QString organORdosimName;
		if (dosim_type == 0) organORdosimName = "Dosimeter_" + QString::number(organORdosimID) + "_Hp(10)";
		if (dosim_type == 1) organORdosimName = "Dosimeter_" + QString::number(organORdosimID) + "_Hp(3)";
		if (dosim_type == 2) organORdosimName = "Dosimeter_" + QString::number(organORdosimID) + "_Hp(0.07)";

		QCPTextElement *title = new QCPTextElement(Dosimeter_RealtimeLineGraph_QCustomPlot[organORdosimID], organORdosimName, QFont("sans", FontSizeScaling(13), QFont::Bold));
		Dosimeter_RealtimeLineGraph_QCustomPlot[organORdosimID]->plotLayout()->insertRow(0); // Insert an empty row in the plot layout
		Dosimeter_RealtimeLineGraph_QCustomPlot[organORdosimID]->plotLayout()->addElement(0, 0, title); // Add the title to the left of the new row
		double Error = 0;
		QString errorStr = QString("Statistical relative error: %1% ").arg(QString::number(Error, 'f', 2));
		QCPTextElement *ErrorText = new QCPTextElement(Dosimeter_RealtimeLineGraph_QCustomPlot[organORdosimID], errorStr, QFont("sans", FontSizeScaling(10), QFont::Bold));
		ErrorText->setTextFlags(Qt::AlignRight);
		Dosimeter_RealtimeLineGraph_QCustomPlot[organORdosimID]->plotLayout()->insertRow(1); // Insert an empty row in the plot layout
		Dosimeter_RealtimeLineGraph_QCustomPlot[organORdosimID]->plotLayout()->addElement(1, 0, ErrorText); // Add the error to the right of the new row
		
		Dosimeter_RealtimeLineGraph_QCustomPlot[organORdosimID]->xAxis->setLabel("Running time (sec)");
		Dosimeter_RealtimeLineGraph_QCustomPlot[organORdosimID]->xAxis->setLabelFont(QFont("sans", FontSizeScaling(11))); // x축 라벨 폰트
		Dosimeter_RealtimeLineGraph_QCustomPlot[organORdosimID]->xAxis->setTickLabelFont(QFont("sans", FontSizeScaling(8))); // x축 값 폰트

		Dosimeter_RealtimeLineGraph_QCustomPlot[organORdosimID]->yAxis->setLabel("Ambient dose equivalent (pSv" + theApp.pRt->DoseUnit_QString + ")");
		Dosimeter_RealtimeLineGraph_QCustomPlot[organORdosimID]->yAxis->setLabelFont(QFont("sans", FontSizeScaling(8))); // y축 라벨 폰트
		Dosimeter_RealtimeLineGraph_QCustomPlot[organORdosimID]->yAxis->setTickLabelFont(QFont("sans", FontSizeScaling(8))); // y축 값 폰트
		Dosimeter_RealtimeLineGraph_QCustomPlot[organORdosimID]->yAxis->setNumberFormat("e"); // "e" for scientific notation and "b" to use beautiful decimal powers
		Dosimeter_RealtimeLineGraph_QCustomPlot[organORdosimID]->yAxis->setNumberPrecision(2); // This sets the number to be like "1.4e+03"

		Dosimeter_RealtimeLineGraph_QCustomPlot[organORdosimID]->rescaleAxes();
		Dosimeter_RealtimeLineGraph_QCustomPlot[organORdosimID]->replot();
	}
	
}

// 계산 중 실시간 업데이트 관련
void RealTimeResultPanel::RealtimeGraphUpdate()
{
	if (flg_GraphUpdate)
	{
		RealtimeGraphUpdate_TimeDose();
		RealtimeGraphUpdate_OrganDose();
		flg_GraphUpdate = false;
	}
}
void RealTimeResultPanel::RealtimeGraphUpdate_TimeDose()
{
	int phantomIdx = theApp.pRt->OutputPhantomSelectedIndex; // 현재 선택중인 팬텀의 그래프 업데이트
	for (int organID = 0; organID < theApp.pRt->OrganNameList.size(); organID++)
	{
		double Error = (theApp.pRt->m_graphInfo_Error_map[phantomIdx][organID]) * 100;
		QString errorStr = QString("Statistical relative error: %1% ").arg(QString::number(Error, 'f', 2));
		QCPTextElement *ErrorText = qobject_cast<QCPTextElement*>(OrganDose_RealtimeLineGraph_QCustomPlot[organID]->plotLayout()->element(1, 0));
		ErrorText->setText(errorStr);
		
		QVector<double> x, y;
		x.reserve(theApp.pRt->m_graphInfo_TimeDose_map[phantomIdx][organID].size());
		y.reserve(theApp.pRt->m_graphInfo_TimeDose_map[phantomIdx][organID].size());
		for (const auto& val : theApp.pRt->m_graphInfo_TimeDose_map[phantomIdx][organID])
		{
			x.append(val.first);
			y.append(val.second);
		}
		OrganDose_RealtimeLineGraph_QCustomPlot[organID]->graph(0)->setData(x, y);
		OrganDose_RealtimeLineGraph_QCustomPlot[organID]->rescaleAxes();
		OrganDose_RealtimeLineGraph_QCustomPlot[organID]->yAxis->setRangeLower(0);
		OrganDose_RealtimeLineGraph_QCustomPlot[organID]->replot();
	}	
}
void RealTimeResultPanel::RealtimeGraphUpdate_OrganDose()
{
	int phantomIdx = theApp.pRt->OutputPhantomSelectedIndex; // 현재 선택중인 팬텀의 그래프 업데이트
	
	QCPBars *bars = qobject_cast<QCPBars *>(theApp.pRt->OrganDose_BarGraph_QCustomPlot->plottable(0)); // Get bars
	if (!bars)
	{
		qDebug() << "No bars found in the QCustomPlot!";
		return;
	}

	QVector<double> doses;
	doses.reserve(theApp.pRt->OrganNameList.size());
	for (int i = 0; i < theApp.pRt->OrganNameList.size(); i++)
	{
		doses.append(theApp.pRt->m_graphInfo_OrganDose_map[phantomIdx][i].second);
	}
	// Update the bar data
	QVector<double> ticks;
	for (int i = 0; i < doses.size(); ++i)
		ticks << i + 1;
	bars->setData(ticks, doses); // Update data
	// Adjust labels to display new dose values
	for (int i = 0; i < doses.size(); ++i)
	{
		QCPItemText *valueLabel = qobject_cast<QCPItemText *>(theApp.pRt->OrganDose_BarGraph_QCustomPlot->item(i));
		if (valueLabel)
		{
			valueLabel->setText(QString::number(doses.at(i), 'e', 1));
			double yAxisRange = theApp.pRt->OrganDose_BarGraph_QCustomPlot->yAxis->range().size();
			double labelOffset = yAxisRange * 0.05; // 전체 범위의 5%
			valueLabel->position->setCoords(ticks.at(i), doses.at(i) + labelOffset);
			QFont labelFont = valueLabel->font();
			labelFont.setPointSize(FontSizeScaling(6));
			valueLabel->setFont(labelFont);
		}
	}
	// Rescale the y-axis to fit the new dose data
	bars->rescaleValueAxis();
	double upperScaleFactor = 2;
	double upperBound = theApp.pRt->OrganDose_BarGraph_QCustomPlot->yAxis->range().upper;
	theApp.pRt->OrganDose_BarGraph_QCustomPlot->yAxis->setRangeUpper(upperBound * upperScaleFactor);
	theApp.pRt->OrganDose_BarGraph_QCustomPlot->yAxis->setRangeLower(0);

	// Refresh the plot
	theApp.pRt->OrganDose_BarGraph_QCustomPlot->replot();
}

void RealTimeResultPanel::ForcedGraphUpdate() // 다른 팬텀 선택 및 패널 이동 등으로 강제로 업데이트할 때
{
	if (flg_GraphUpdate) std::this_thread::sleep_for(std::chrono::milliseconds(500)); // 지금 실시간 그래프 업데이트 중이면, 처리할때까지 1초만 쉬기
	ForcedGraphUpdate_TimeDose();
	ForcedGraphUpdate_OrganDose();
}
void RealTimeResultPanel::ForcedGraphUpdate_TimeDose()
{
	int phantomIdx = theApp.pRt->OutputPhantomSelectedIndex; // 현재 선택중인 팬텀의 그래프 업데이트
	
	for (int organID = 0; organID < theApp.pRt->OrganNameList.size(); organID++)
	{
		double Error = theApp.pRt->m_graphInfo_Error_map[phantomIdx][organID] * 100;
		QString errorStr = QString("Statistical relative error: %1% ").arg(QString::number(Error, 'f', 2));
		QCPTextElement *ErrorText = qobject_cast<QCPTextElement*>(OrganDose_RealtimeLineGraph_QCustomPlot[organID]->plotLayout()->element(1, 0));
		ErrorText->setText(errorStr);

		QVector<double> x, y;
		x.reserve(theApp.pRt->m_graphInfo_TimeDose_map[phantomIdx][organID].size());
		y.reserve(theApp.pRt->m_graphInfo_TimeDose_map[phantomIdx][organID].size());
		for (const auto& val : theApp.pRt->m_graphInfo_TimeDose_map[phantomIdx][organID])
		{
			x.append(val.first);
			y.append(val.second);
		}
		OrganDose_RealtimeLineGraph_QCustomPlot[organID]->graph(0)->setData(x, y);
		OrganDose_RealtimeLineGraph_QCustomPlot[organID]->rescaleAxes();
		OrganDose_RealtimeLineGraph_QCustomPlot[organID]->yAxis->setRangeLower(0);
		OrganDose_RealtimeLineGraph_QCustomPlot[organID]->replot();
		
	}
	
}
void RealTimeResultPanel::ForcedGraphUpdate_OrganDose()
{
	int phantomIdx = theApp.pRt->OutputPhantomSelectedIndex; // 현재 선택중인 팬텀의 그래프 업데이트	
	QCPBars *bars = qobject_cast<QCPBars *>(theApp.pRt->OrganDose_BarGraph_QCustomPlot->plottable(0)); // Get bars
	if (!bars)
	{
		qDebug() << "No bars found in the QCustomPlot!";
		return;
	}
	
	QVector<double> doses;
	doses.reserve(theApp.pRt->OrganNameList.size());
	for (int i = 0; i < theApp.pRt->OrganNameList.size(); i++)
	{
		doses.append(theApp.pRt->m_graphInfo_OrganDose_map[phantomIdx][i].second);
	}

	
	// Update the bar data
	QVector<double> ticks;
	for (int i = 0; i < doses.size(); ++i)
		ticks << i + 1;
	bars->setData(ticks, doses); // Update data
	// Adjust labels to display new dose values
	for (int i = 0; i < doses.size(); ++i)
	{
		QCPItemText *valueLabel = qobject_cast<QCPItemText *>(theApp.pRt->OrganDose_BarGraph_QCustomPlot->item(i));
		if (valueLabel)
		{
			valueLabel->setText(QString::number(doses.at(i), 'e', 1));
			double yAxisRange = theApp.pRt->OrganDose_BarGraph_QCustomPlot->yAxis->range().size();
			double labelOffset = yAxisRange * 0.05; // 전체 범위의 5%
			valueLabel->position->setCoords(ticks.at(i), doses.at(i) + labelOffset);
			QFont labelFont = valueLabel->font();
			labelFont.setPointSize(FontSizeScaling(6)); // 폰트 크기를 8로 설정 (기본값보다 작게)
			valueLabel->setFont(labelFont);
		}
	}
	
	// Rescale the y-axis to fit the new dose data
	bars->rescaleValueAxis();
	double upperScaleFactor = 2;
	double upperBound = theApp.pRt->OrganDose_BarGraph_QCustomPlot->yAxis->range().upper;
	theApp.pRt->OrganDose_BarGraph_QCustomPlot->yAxis->setRangeUpper(upperBound * upperScaleFactor);
	theApp.pRt->OrganDose_BarGraph_QCustomPlot->yAxis->setRangeLower(0);

	// Refresh the plot
	theApp.pRt->OrganDose_BarGraph_QCustomPlot->replot();
}

void RealTimeResultPanel::ForcedGraphUpdate_H_AK_Dose() // 임시 함수임
{		
	QCPBars *bars = qobject_cast<QCPBars *>(theApp.pRt->H_AK_BarGraph_QCustomPlot->plottable(0)); // Get bars
	if (!bars)
	{
		qDebug() << "No bars found in the QCustomPlot!";
		return;
	}

	QVector<double> doses;
	doses.reserve(theApp.pRt->m_Phantom_SequenceVector.size() * 2); // 팬텀의 개수가 air sphere (전산팬텀 + airsphere) 개수임 -> 각각 AK, H
	for (int i = 0; i < theApp.pRt->m_Phantom_SequenceVector.size(); i++)
	{
		// 랜덤하게 0.5-1 생성 중...
		//int randInt = qrand(); // qrand()는 Qt6에서 사라졌다. 표준 C 라이블러리의 rand()함수를 단순 wrap한 구식이기 때문이다.
		//그래서 상단에 QRandomGenerator를 추가하였다.
		quint32 randInt = QRandomGenerator::global()->generate();
		// 0과 1 사이의 실수로 변환
		double normalized = static_cast<double>(randInt) / static_cast<double>(RAND_MAX);

		// 0.5 ~ 1 사이의 값으로 스케일링
		double randomValue = 0.5 + normalized * 0.5;

		doses.append(randomValue);
		doses.append(randomValue*1.05);
	}


	// Update the bar data
	QVector<double> ticks;
	for (int i = 0; i < doses.size(); ++i)
		ticks << i + 1;
	bars->setData(ticks, doses); // Update data
	// Adjust labels to display new dose values
	for (int i = 0; i < doses.size(); ++i)
	{
		QCPItemText *valueLabel = qobject_cast<QCPItemText *>(theApp.pRt->H_AK_BarGraph_QCustomPlot->item(i));
		if (valueLabel)
		{
			valueLabel->setText(QString::number(doses.at(i), 'e', 1));
			valueLabel->position->setCoords(ticks.at(i), doses.at(i)*1.35);
		}
	}

	// Rescale the y-axis to fit the new dose data
	bars->rescaleValueAxis();
	double upperScaleFactor = 2;
	double upperBound = theApp.pRt->H_AK_BarGraph_QCustomPlot->yAxis->range().upper;
	theApp.pRt->H_AK_BarGraph_QCustomPlot->yAxis->setRangeUpper(upperBound * upperScaleFactor);
	theApp.pRt->H_AK_BarGraph_QCustomPlot->yAxis->setRangeLower(0);

	// Refresh the plot
	theApp.pRt->H_AK_BarGraph_QCustomPlot->replot();
}