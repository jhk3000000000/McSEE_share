#include "pch.h"
#include "ETHuman3DApp.h"

#include "ProcessInformationPanel.h"
#include "FunctionPanelRight.h"
#include "Manager_Calculation.h"


#include "Util.h"

ProcessInformationPanel::ProcessInformationPanel(QWidget *parent)
	: QWidget(parent)
{
	widgetSizeRatioFactor = (0.62 * (1 - 0.3 * default_RatioWtoH / RatioWtoH)) / 0.432; // 전체에서 PanelRight output이 늘어난만큼에서 빼고, realtimePanel과 processInformationPanel이 동일한 비율(0.62)로 나눠가짐. 0.432는 origianl width 비율
	window_width *= widgetSizeRatioFactor; // 화면비 calibration

	font_PI_GBW1.setPointSize(FontSizeScaling(11));
	font_PI_LBL1.setPointSize(FontSizeScaling(11));
	font_PI_BTN1.setPointSize(FontSizeScaling(11));
	font_PI_BAR1.setPointSize(FontSizeScaling(11));

	int width = window_width * 0.432;
	int height = window_height * 0.14;
	setFixedSize(width, height);
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	setStyleSheet(ProcessInformationPanelStyle::BACKGROUND_WIDGET);

	CreateWidgets();
}

ProcessInformationPanel::~ProcessInformationPanel() 
{

}

void ProcessInformationPanel::CreateWidgets()
{
	QVBoxLayout* layoutMain = new QVBoxLayout;
	assert(layoutMain);
	layoutMain->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

	QVBoxLayout* labelLayout = new QVBoxLayout;
	labelLayout->setContentsMargins(0, 0, 0, 0);
	labelLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

	{
		auto* grid = new QGridLayout;
		grid->setContentsMargins(0, 0, 0, 0);
		grid->setHorizontalSpacing(window_width * 0.01);
		grid->setVerticalSpacing(window_height * 0.005);
		for (int c = 0; c < 3; ++c) grid->setColumnStretch(c, 1);

		// 한 쌍(키:값) 생성 헬퍼: 멤버 포인터를 new로 채우고 공통 스타일/폰트 적용
		auto makePair = [&](const QString& keyText, QLabel*& valueLbl)->QWidget* {
			auto* w = new QWidget;
			w->setAutoFillBackground(false);
			w->setAttribute(Qt::WA_StyledBackground, false);
			w->setStyleSheet("background: transparent;"); // 스타일 투명화
			auto* h = new QHBoxLayout(w);
			h->setContentsMargins(0, 0, 0, 0);

			auto* key = new QLabel(keyText, w);
			key->setStyleSheet(ProcessInformationPanelStyle::DATA_LABEL); // 배경 그대로 유지
			key->setFont(font_PI_LBL1);

			valueLbl = new QLabel("—", w); // 멤버 포인터 new
			valueLbl->setStyleSheet(ProcessInformationPanelStyle::DATA_LABEL);
			valueLbl->setFont(font_PI_LBL1);

			key->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
			valueLbl->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

			h->addWidget(key);
			h->addSpacing(window_width * 0.004);
			h->addWidget(valueLbl);
			h->addStretch();
			return w;
		};

		// 1행: Memory / Thread / Time Remaining
		grid->addWidget(makePair("Memory Usage:", labelMemUsage), 0, 0);
		grid->addWidget(makePair("Thread Usage:", labelThreadUsage), 0, 1);
		grid->addWidget(makePair("Time Remaining:", labelNowTimeRemaining), 0, 2);

		// 2행: PageFault(단독) / Particles / Runtime
		labelPageFault = new QLabel("", nullptr);
		labelPageFault->setStyleSheet(QString(ProcessInformationPanelStyle::DATA_LABEL2));
		labelPageFault->setFont(font_PI_LBL1);
		labelPageFault->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
		grid->addWidget(labelPageFault, 1, 0);

		grid->addWidget(makePair("Particles:", labelTransportedParticles), 1, 1);
		grid->addWidget(makePair("Runtime:", labelRunTime), 1, 2);

		// 부모 레이아웃에 부착
		labelLayout->addLayout(grid);

		// 초기 표시값
		labelMemUsage->setText("0 GB");
		labelThreadUsage->setText("0");
		labelNowTimeRemaining->setText("0d 00:00:00");
		labelTransportedParticles->setText("0");
		labelRunTime->setText("0d 00:00:00");
	}
	

	//{// 첫번째 라인
	//	QHBoxLayout* subLayout = new QHBoxLayout;
	//	subLayout->setContentsMargins(0, 0, 0, 0);
	//	subLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

	//	QLabel* labelMemUse = new QLabel;
	//	labelMemUse->setFixedSize(window_width * 0.072, window_height * 0.028);
	//	labelMemUse->setStyleSheet(ProcessInformationPanelStyle::DATA_LABEL);
	//	labelMemUse->setFont(font_PI_LBL1);
	//	labelMemUse->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	//	labelMemUse->setText(QString::fromLocal8Bit("Memory Usage:"));

	//	labelMemUsage = new QLabel;
	//	labelMemUsage->setFixedSize(window_width * 0.045, window_height * 0.018);
	//	labelMemUsage->setStyleSheet(ProcessInformationPanelStyle::DATA_LABEL);
	//	labelMemUsage->setFont(font_PI_LBL1);
	//	labelMemUsage->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	//	labelMemUsage->setText(QString::fromLocal8Bit("0 GB"));

	//	QSpacerItem *spacer1 = new QSpacerItem(window_width * 0.002, window_height * 0.018); // Adjust spacing
	//	
	//	QLabel* labelThreadUse = new QLabel;
	//	labelThreadUse->setFixedSize(window_width * 0.068, window_height * 0.028);
	//	labelThreadUse->setStyleSheet(ProcessInformationPanelStyle::DATA_LABEL);
	//	labelThreadUse->setFont(font_PI_LBL1);
	//	labelThreadUse->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	//	labelThreadUse->setText(QString::fromLocal8Bit("Thread Usage:"));

	//	labelThreadUsage = new QLabel;
	//	labelThreadUsage->setFixedSize(window_width * 0.068, window_height * 0.018);
	//	labelThreadUsage->setStyleSheet(ProcessInformationPanelStyle::DATA_LABEL);
	//	labelThreadUsage->setFont(font_PI_LBL1);
	//	labelThreadUsage->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	//	labelThreadUsage->setText(QString::fromLocal8Bit("0"));
	//	
	//	QLabel* labelTimeRemaining = new QLabel;
	//	labelTimeRemaining->setFixedSize(window_width * 0.125, window_height * 0.028);
	//	labelTimeRemaining->setStyleSheet(ProcessInformationPanelStyle::DATA_LABEL);
	//	labelTimeRemaining->setFont(font_PI_LBL1);
	//	labelTimeRemaining->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	//	labelTimeRemaining->setText(QString::fromLocal8Bit("Time Remaining:"));

	//	//QSpacerItem *spacer2 = new QSpacerItem(Window_width * 0.010, Window_height * 0.018); // Adjust spacing

	//	labelNowTimeRemaining = new QLabel;
	//	labelNowTimeRemaining->setFixedSize(window_width * 0.070, window_height * 0.018);
	//	labelNowTimeRemaining->setStyleSheet(ProcessInformationPanelStyle::DATA_LABEL);
	//	labelNowTimeRemaining->setFont(font_PI_LBL1);
	//	labelNowTimeRemaining->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	//	labelNowTimeRemaining->setText(QString::fromLocal8Bit("0d 00:00:00"));

	//	subLayout->addWidget(labelMemUse);
	//	subLayout->addWidget(labelMemUsage);
	//	subLayout->addSpacerItem(spacer1);
	//	subLayout->addWidget(labelThreadUse);
	//	subLayout->addWidget(labelThreadUsage);		
	//	subLayout->addWidget(labelTimeRemaining);
	//	//subLayout->addSpacerItem(spacer2);
	//	subLayout->addWidget(labelNowTimeRemaining);

	//	labelLayout->addLayout(subLayout);
	//}

	//{
	//	QHBoxLayout* subLayout = new QHBoxLayout;
	//	subLayout->setContentsMargins(0, 0, 0, 0);
	//	subLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);	

	//	//Paging Fault
	//	labelPageFault = new QLabel;
	//	labelPageFault->setFixedSize(window_width * 0.072, window_height * 0.028);
	//	labelPageFault->setStyleSheet(ProcessInformationPanelStyle::DATA_LABEL);
	//	labelPageFault->setFont(font_PI_LBL1);
	//	labelPageFault->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	//	labelPageFault->setText(QString::fromLocal8Bit("None"));
	//	//Paging Fault

	//	QLabel* labelParticles = new QLabel;
	//	labelParticles->setFixedSize(window_width * 0.0475, window_height * 0.028);
	//	labelParticles->setStyleSheet(ProcessInformationPanelStyle::DATA_LABEL);
	//	labelParticles->setFont(font_PI_LBL1);
	//	labelParticles->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	//	labelParticles->setText(QString::fromLocal8Bit("Particles:"));

	//	labelTransportedParticles = new QLabel;
	//	labelTransportedParticles->setFixedSize(window_width * 0.098, window_height * 0.018);
	//	labelTransportedParticles->setStyleSheet(ProcessInformationPanelStyle::DATA_LABEL);
	//	labelTransportedParticles->setFont(font_PI_LBL1);
	//	labelTransportedParticles->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	//	labelTransportedParticles->setText(QString::fromLocal8Bit("0"));

	//	QLabel* labelRunningTime = new QLabel;
	//	labelRunningTime->setFixedSize(window_width * 0.062, window_height * 0.028);
	//	labelRunningTime->setStyleSheet(ProcessInformationPanelStyle::DATA_LABEL);
	//	labelRunningTime->setFont(font_PI_LBL1);
	//	labelRunningTime->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	//	labelRunningTime->setText(QString::fromLocal8Bit("Runtime:"));

	//	labelRunTime = new QLabel;
	//	labelRunTime->setFixedSize(window_width * 0.070, window_height * 0.018);
	//	labelRunTime->setStyleSheet(ProcessInformationPanelStyle::DATA_LABEL);
	//	labelRunTime->setFont(font_PI_LBL1);
	//	labelRunTime->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	//	labelRunTime->setText(QString::fromLocal8Bit("0d 00:00:00"));
	//			
	//	subLayout->addWidget(labelPageFault);
	//	subLayout->addWidget(labelParticles);
	//	subLayout->addWidget(labelTransportedParticles);
	//	subLayout->addWidget(labelRunningTime);
	//	subLayout->addWidget(labelRunTime);

	//	labelLayout->addLayout(subLayout);
	//}

	{
		QHBoxLayout* subLayout = new QHBoxLayout;
		subLayout->setContentsMargins(0, 0, 0, 0);
		subLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

		bar = new QProgressBar(this);
		bar->setStyleSheet(ProcessInformationPanelStyle::PROGRESS_BAR);
		bar->setFixedHeight(window_height * 0.025);
		font_PI_BAR1.setBold(true);
		bar->setFont(font_PI_BAR1);
		bar->setRange(0, 100);
		bar->setValue(0);
		bar->setTextVisible(true);
		bar->show();

		subLayout->addWidget(bar);

		labelLayout->addLayout(subLayout);
	}

	{
		QHBoxLayout* subLayout = new QHBoxLayout;
		subLayout->setContentsMargins(0, window_height * 0.005, 0, 0);
		subLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

		ProgressLabel = new QLabel;
		ProgressLabel->setFixedSize(window_width * 0.400, window_height * 0.03);
		ProgressLabel->setStyleSheet(ProcessInformationPanelStyle::DATA_LABEL);
		ProgressLabel->setFont(font_PI_LBL1);
		ProgressLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		ProgressLabel->setText(QString::fromLocal8Bit(""));		

		subLayout->addWidget(ProgressLabel);

		labelLayout->addLayout(subLayout);
	}

	layoutMain->addLayout(labelLayout);

	setLayout(layoutMain);
}

void ProcessInformationPanel::slot_StopSave_ButtonClicked()
{
	if (QMessageBox::Yes == theApp.SetMessageBox_YesNo("The stop process is irreversible. Do you want to stop the simulation?"))
	{
		theApp.managerCalculation->DoseCalculation_StopAndSave();
		StopSaveButton->setDisabled(true);
	}
}


