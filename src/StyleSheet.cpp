#include "pch.h"

#include <QtWidgets>
#include "StyleSheet.h"


namespace utils {
	namespace ui {
		// DataLoadPaneStyle is not used!
			const QString DataLoadPaneStyle::BACKGROUND_WIDGET = "QWidget{ background:transparent; background-color:#A0B1C3; border: none; outline: none;}";

			const QString DataLoadPaneStyle::GROUPBOX_WIDGET = "QGroupBox{ background-color : rgba(0, 0, 0, 20%); height: 60px; border: 0px; border-color:rgba(44, 43, 55, 20% ); font-size:15px; border-radius: 3px; \
																			font-family:\"Noto Sans\"; font: bold;}";

		
			const QString DataLoadPaneStyle::RADIOBUTTON_WIDGET = "QRadioButton{ color: white; font: 15px; font-family:\"Noto Sans\"; border: 0px; \
																				 background-color: transparent; text-align : left; padding-top:20px; padding-left:13px; }\
																				QRadioButton::indicator:unchecked{ image: url('./data/image/radio_nor.png'); }\
																				QRadioButton::indicator:checked{ image: url('./data/image/radio_pre.png'); }";

			const QString DataLoadPaneStyle::RADIOBUTTON_WIDGET2 = "QRadioButton{ color: white; font: 15px; font-family:\"Noto Sans\"; border: 0px; \
																				 background-color: transparent; text-align : left; padding-top:0px; padding-left:13px; }\
																				QRadioButton::indicator:unchecked{ image: url('./data/image/radio_nor.png'); }\
																				QRadioButton::indicator:checked{ image: url('./data/image/radio_pre.png'); }";

			const QString DataLoadPaneStyle::IMPORT_BTN = "QPushButton { background-color:transparent; background-color:#546880; font-size:15px; color: #ffffff; font-family:\"Noto Sans\"; padding: 1px; border-radius: 4px; }\
																	QPushButton:hover { background-color:#5d6372; }\
																	QPushButton:checked { background-color: #40424f;}\
																	QPushButton:pressed { background-color:#40424f; }";


			const QString DataLoadPaneStyle::BODY_PART_CHART = "QWidget {background-color: white; border-style:hidden; font-size:12px; font-family:\"Noto Sans\";color:black;}\
															 QTableWidget { gridline-color: rgb(80, 80, 80); font-size: 12px; selection-color: black; selection-background-color: #E4E4E4;  }\
															 QTableWidget QTableCornerButton::section { border: 1px solid #AEAEAE; background-color: #E4E4E4;  margin-right: 1px; margin-bottom: 1px;}\
															 QTableWidget:disabled { background-color: #DCDCDC;}\
															 QHeaderView::section { background-color: #D2D2D2; padding: 0px; border: 1px solid #AEAEAE; font-size: 12px;	 height:20px; \
															 margin-left: -1px;  margin-right: 1px;  margin-top: -1px; margin-bottom: 1px; }\
															 QHeaderView::section:horizontal { }\
															 QHeaderView::section:vertical { }\
															 QLineEdit { background-color:white; Text-align:center;\
															 border: 1px; border-color:gray; border-style:inset; font-size:15px; font-family:\"Noto Sans\"; color:black;}";

			const QString DataLoadPaneStyle::DATA_LABEL = "QLabel{background-color : rgba(0, 0, 0, 0%);font-size:15px; font-family:\"Noto Sans\"; \
															color:black; border: 0px; font-weight:1200;}";

			const QString DataLoadPaneStyle::LINEEDIT_NAME = "QLineEdit { background-color:#FFFFFF; Text-align:center;\
															 border: 1px; border-color:gray; border-style:inset; font-size:15px; font: bold; border: 0px; font-weight:800;}";
		// DataLoadPaneStyle is not used!

		// DataInfomationPaneStyle is not used!
			const QString DataInfomationPaneStyle::BACKGROUND_WIDGET = "QWidget{ background:transparent; background-color:rgba(158,0,0,15%); border-radius: 3px;}";

			const QString DataInfomationPaneStyle::DATA_LABEL = "QLabel{background-color : rgba(0, 0, 0, 0%);font-size:15px; font-family:\"Noto Sans\"; \
															color:white; border: 0px; font-weight:100;}";

			const QString DataInfomationPaneStyle::DATA_LABEL2 = "QLabel{background-color : rgba(0, 0, 0, 0%);font: bold; font-size:15px; font-family:\"Noto Sans\"; \
															color:black; border: 0px; }";
		// DataInfomationPaneStyle is not used!

		// ThreadInfomationPaneStyle is not used!
			const QString ThreadInfomationPaneStyle::BACKGROUND_WIDGET = "QWidget{ background:transparent; background-color:rgba(0,158,0,15%); border-radius: 3px;}";

			const QString ThreadInfomationPaneStyle::DATA_LABEL = "QLabel{background-color : rgba(0, 0, 0, 0%);font-size:17px; font-family:\"Noto Sans\"; \
															color:white; border: 0px; font-weight:250;}";

			const QString ThreadInfomationPaneStyle::DATA_LABEL2 = "QLabel{background-color : rgba(0, 0, 0, 0%);font-size:12px; font-family:\"Noto Sans\"; \
															color:black; border: 0px; font-weight:250;}";
		// ThreadInfomationPaneStyle is not used!

		// RightPanelStyle is not used!
			const QString RightPanelStyle::BACKGROUND_WIDGET = "QWidget{ background:transparent; background-color:#A0B1C3; border-radius: 3px;}";

			const QString RightPanelStyle::TYPE0_BTN = "QPushButton { background-color:transparent; background-color:#546880; font-size:15px; font-weight:800; color: #ffffff; font-family:\"Noto Sans\"; padding: 1px; border-radius: 0px; }\
														QPushButton:hover { background-color:#5d6372; }\
														QPushButton:checked { background-color: #40424f;}\
														QPushButton:pressed { background-color:#40424f; }";

			const QString RightPanelStyle::TYPE1_BTN = "QPushButton { background-color:transparent; background-color:#546880; font-size:15px; font: bold; color: #ffffff; font-family:\"Noto Sans\"; padding: 1px; border-radius: 0px; }\
														QPushButton:hover { background-color:#5d6372; }\
														QPushButton:checked { background-color: #40424f;}\
														QPushButton:pressed { background-color:#40424f; }";

			const QString RightPanelStyle::DATA_LABEL_PHOTO = "QLabel{background-color : rgba(255, 255, 0, 20%);font-size:20px; \
															color:#444444; font: bold; border-style: solid; border-width: 1px; border-color: rgba(50, 50, 50, 50% ); font-weight:800;}";

			const QString RightPanelStyle::GROUPBOX_WIDGET = "QGroupBox{ background-color : rgba(80, 80, 80, 0%); height: 60px; border: 0px; border-color:rgba(44, 43, 55, 20% ); font-size:15px; border-radius: 3px; \
																			font-family:\"Noto Sans\"; font: bold; margin-top: 6px; margin-left: 12px;margin-right: 12px;}";

			const QString RightPanelStyle::TOOTH_BTN = "QPushButton { background-color:transparent; background-color:#546880; font-size:15px; color: #ffffff; font-family:\"Noto Sans\"; padding: 1px; border-radius: 17px; }\
														QPushButton:hover { background-color:#5d6372; }\
														QPushButton:checked { background-color: #40424f;}\
														QPushButton:disabled { background-color: #919191;}\
														QPushButton:pressed { background-color:#40424f; }";

			const QString RightPanelStyle::TOOTH_LABEL = "QLabel{background-color: #919191;font-size:14px; font-family:\"Noto Sans\"; \
															color:white; border: 0px; font-weight:500; font-size:30px;}";
		// RightPanelStyle is not used!

		// DataTreeStyle is not used!
			const QString DataTreeStyle::BACKGROUND_WIDGET = "QWidget{ background:transparent; background-color:#A0B1C3; border: none; outline: none;}";
			const QString DataTreeStyle::DATATREE_SLIDER_BUTTON = "QSlider{border: none; outline: none;}\
																		QSlider::groove:horizontal {\
																		border: 1px solid #bbb;\
																		background: white;\
																		height: 15px;\
																		border-radius: 4px;\
																		}\
																		\
																		QSlider::sub-page:horizontal {\
																		background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\
																		    stop: 0 #66e, stop: 1 #bbf);\
																		background: qlineargradient(x1: 0, y1: 0.2, x2: 1, y2: 1,\
																		    stop: 0 #bbf, stop: 1 #55f);\
																		border: 1px solid #777;\
																		height: 10px;\
																		border-radius: 4px;\
																		}\
																		\
																		QSlider::add-page:horizontal {\
																		background: #fff;\
																		border: 1px solid #777;\
																		height: 10px;\
																		border-radius: 4px;\
																		}\
																		\
																		QSlider::handle:horizontal {\
																		background: qlineargradient(x1:0, y1:0, x2:1, y2:1,\
																		    stop:0 #eee, stop:1 #ccc);\
																		border: 1px solid #777;\
																		width: 13px;\
																		margin-top:-2px;\
																		margin-bottom: -2px;\
																		border-radius: 4px;\
																		}\
																		\
																		QSlider::handle:horizontal:hover {\
																		background: qlineargradient(x1:0, y1:0, x2:1, y2:1,\
																		    stop:0 #fff, stop:1 #ddd);\
																		border: 1px solid #444;\
																		border-radius: 4px;\
																		}\
																		\
																		QSlider::sub-page:horizontal:disabled {\
																		background: #bbb;\
																		border-color: #999;\
																		}\
																		\
																		QSlider::add-page:horizontal:disabled {\
																		background: #eee;\
																		border-color: #999;\
																		}\
																		\
																		QSlider::handle:horizontal:disabled {\
																		background: #eee;\
																		border: 1px solid #aaa;\
																		border-radius: 4px;\
																		}\
																		\
																QSlider::handle:vertical:disabled { background: #DCDCDC;}";
		// DataTreeStyle is not used!


		// CameraOptionViewSTyle is not used!
		const QString CameraOptionViewSTyle::CAM_VIEWPORT_BTN = "QPushButton { background-color:transparent; background-color:#546880; font-size:15px; color: #ffffff; font-family:\"Noto Sans\"; padding: 1px; border-radius: 3px; }\
													QPushButton:hover { background-color:#5d6372; }\
													QPushButton:checked { background-color: #40424f;}\
													QPushButton:pressed { background-color:#40424f; }";
		// CameraOptionViewSTyle is not used!

	
		const QString ProcessInformationPanelStyle::BACKGROUND_WIDGET = "QWidget{ background:transparent; background-color:rgba(10,10,10,15%); border-radius: 3px;}";

		// 후보: #63A4DF; 588CE0; 
		const QString ProcessInformationPanelStyle::GROUPBOX_WIDGET = "QGroupBox{ background-color : rgba(0, 0, 0, 20%); height: 60px; border: 0px; background-color:#EDEDED   ; border-radius: 0px; \
																		 color:#000000; font: bold; margin-top: 0px; margin-left: 3px;margin-right: 0px;}\
															 QGroupBox::title{margin-left: 6px; margin-bottom: 2px;}";

		const QString ProcessInformationPanelStyle::DATA_LABEL =  "QLabel{background-color : rgba(0, 0, 0, 0%);font-family:\"Noto Sans\"; \
														color:black; border: 0px; font-weight:1600;}\
														QLabel:disabled{ color:#9D9D9D; }";
		const QString ProcessInformationPanelStyle::DATA_LABEL2 = "QLabel{background-color : rgba(0, 0, 0, 0%);font-family:\"Noto Sans\"; \
														color:red; border: 0px; font-weight:1600;}\
														QLabel:disabled{ color:#9D9D9D; }";
		const QString ProcessInformationPanelStyle::PROGRESS_BAR = "QProgressBar{ border: 2px solid grey; border-radius: 5px; text-align: center; }\
																	QProgressBar::chunk { background-color: #00B050; width: 10px; margin: 1px;}";
		const QString ProcessInformationPanelStyle::MENU_BTN = "QPushButton { background-color:transparent; background-color:#FF5D5D; color: #000000; font: bold; padding: 0px; border: 1px solid #AEAEAE; border-radius: 7px; }\
													QPushButton:hover { background-color:#700000; }\
													QPushButton:checked { background-color: #94CAFC;}\
													QPushButton:disabled { background-color:#FFC5C5; color:#9D9D9D; }";

		const QString ControlPanelSTyle::BACKGROUND_WIDGET = "QWidget{ background:transparent; background-color:#C9DCE7; border: none; outline: none;}"; // Origianl
		//const QString ControlPanelSTyle::BACKGROUND_WIDGET = "QWidget{ background:transparent; background-color:#B9D7C4; border: none; outline: none;}"; // Geometry

		const QString ControlPanelSTyle::CONTROL_CHECKBOX = "QCheckBox { background:transparent;\
														  Text-align:left; border-style:hidden;font-size:17px;\
														  font-family:\"Noto Sans\";color:black; border: none; outline: none;}\
														  QCheckBox::indicator { width: 27px; height: 25px;}";

		const QString MenuPanelSTyle::BACKGROUND_WIDGET = "QWidget{ background:transparent; background-color:#D7ECFD; border: none; outline: none;}";

		const QString MenuPanelSTyle::MENU_BTN = "QPushButton { background-color:transparent; background-color:#3399FF; color: #444444; font: bold; padding: 0px; border-radius: 0px; }\
													QPushButton:hover { background-color:#198CFF; }\
													QPushButton:checked { background-color: #0058B0;}\
													QPushButton:pressed { background-color:#0058B0; }";

		const QString MenuPanelSTyle::IMAGE_LABEL = "QLabel{background-color : rgba(0, 0, 0, 0%); image: url(:/ETCad/res/image/HUREL.png);}";

		const QString MenuPanelSTyle::GROUPBOX_WIDGET = "QGroupBox{ background-color : rgba(0, 0, 0, 20%); height: 60px; border: 0px; background-color:#AFCCF0; border-radius: 0px; \
																		color:#444444; font: bold; margin-top: 0px; margin-left: 3px;margin-right: 0px;}\
															 QGroupBox::title{margin-left: 6px; margin-bottom: 2px;}";

		const QString DialogStyle::BACKGROUND_WIDGET = "QWidget{ background:transparent; background-color:#A0B1C3; border: none; outline: none;}";

		const QString DialogStyle::BACKGROUND_WIDGET2 = "QWidget{ background:transparent; background-color:#AACDDC; border: none; outline: none;}";

		const QString DialogStyle::MENU_BTN = "QPushButton { background-color:transparent; background-color:#87C2FD; color: #000000; font: bold; padding: 0px; border-radius: 0px; }\
													QPushButton:hover { background-color:#5CABFB; }\
													QPushButton:checked { background-color: #007CF7;}\
													QPushButton:pressed { background-color:#007CF7; }";

		const QString DialogStyle::MENU_BTN2 = "QPushButton { background-color:transparent; background-color:#FFFFFF; color: #000000; font: bold; padding: 0px; border-radius: 7px; }\
													    QPushButton:hover { background-color:#D5D5D5; }\
													    QPushButton:checked { background-color: #94CAFC;}\
													    QPushButton:pressed { background-color: #94CAFC; }\
														QPushButton:disabled { background-color: #DADADA; }";


		const QString DialogStyle::MENU_BTN3 = "QPushButton { background-color:transparent; background-color:#FFFFFF; color: #000000; font: bold; padding: 0px; border: 1px solid #AEAEAE; border-radius: 7px; }\
													QPushButton:hover { background-color:#ADD2E8; }\
													QPushButton:checked { background-color: #94CAFC;}\
													QPushButton:disabled { background-color:#DADADA; color:#9D9D9D; }";

		const QString DialogStyle::MENU_BTN4 = "QPushButton { background-color:transparent; background-color:#EFEFEF; color: #000000; font: bold; padding: 0px; border: 1px solid #AEAEAE; border-radius: 7px;  }\
													QPushButton:hover { background-color:#75A3D3; color: #000000; font: bold; padding: 0px; border: 1px solid #AEAEAE; border-radius: 7px;}\
													QPushButton:checked { background-color: #6288C9; color: #FFFFFF; font: bold; padding: 0px; border: 1px solid #AEAEAE; border-radius: 7px;}\
													QPushButton:disabled { background-color:#DADADA; color:#9D9D9D; color: #FFFFFF; font: bold; padding: 0px; border: 1px solid #AEAEAE; border-radius: 7px;}";

		const QString DialogStyle::MENU_BTN5 = "QPushButton { background-color:transparent; background-color:#FFFFFF; color: #000000; font: bold; padding: 0px; border-radius: 0px; }\
													QPushButton:hover { background-color:#5CABFB; }\
													QPushButton:checked { background-color: #374A68; color: #FFFFFF; font: bold; padding: 0px; border-radius: 0px;}\
													QPushButton:pressed { background-color:#374A68; color: #FFFFFF; font: bold; padding: 0px; border-radius: 0px;}";

		const QString DialogStyle::MENU_BTN6 = "QPushButton { background-color:transparent; background-color:#FFFFFF; color: #000000; font: bold; padding: 0px; border-radius: 0px; }\
													QPushButton:hover { background-color:#F4C2BD; }\
													QPushButton:checked { background-color: #EBD7D5; color: #000000; font: bold; padding: 0px; border-radius: 0px;}\
													QPushButton:pressed { background-color:#EBD7D5; color: #000000; font: bold; padding: 0px; border-radius: 0px;}\
                                                    QPushButton:disabled{ background - color:#DADADA; color:#9D9D9D; }";

		const QString DialogStyle::MENU_BTN7 = "QPushButton { background-color:transparent; background-color:#FFFFFF; color: #000000; font: bold; padding: 0px; border: 1px solid #AEAEAE; border-radius: 7px; }\
													QPushButton:hover { background-color:#ADD2E8; }\
													QPushButton:checked { background-color: #94CAFC;}\
													QPushButton:disabled { background-color:#94CAFC; color:#000000; }";

		const QString DialogStyle::MENU_BTN8 = "QPushButton { background-color:transparent; background-color:#FFFFFF; color: #000000; font: bold; padding: 0px; border: 1px solid #AEAEAE; border-radius: 7px; }\
													QPushButton:hover { background-color:#ADD2E8; }\
													QPushButton:checked { background-color: #94CAFC;}\
													QPushButton:disabled { background-color:#FFFFFF; color:#9D9D9D; }";

		const QString DialogStyle::MENU_BTN9 = "QPushButton { background-color:transparent; background-color:#FFFFFF; color: #000000; font: bold; padding: 0px; border: 1px solid #AEAEAE; border-radius: 7px; }\
													QPushButton:hover { background-color:#ADD2E8; }\
													QPushButton:checked { background-color: #94CAFC;}\
													QPushButton:disabled { background-color::#FFFFFF; color:#9D9D9D; }";

		const QString DialogStyle::MENU_BTN10 = "QPushButton { background-color:transparent; background-color:#FFFFFF; color: #000000; font: bold; padding: 0px; border: 1px solid #AEAEAE; border-radius: 0px; }\
													QPushButton:hover { background-color:#ADD2E8; }\
													QPushButton:checked { background-color: #94CAFC;}\
													QPushButton:disabled { background-color:#94CAFC; color:#9D9D9D; }";

		const QString DialogStyle::MENU_BTN11 = "QPushButton { background-color:transparent; background-color:#87C2FD; color: #000000; font: bold; padding: 0px; border-radius: 7px; }\
													QPushButton:hover { background-color:#5CABFB; }\
													QPushButton:checked { background-color: #007CF7;}\
													QPushButton:pressed { background-color:#007CF7; }";

		const QString DialogStyle::MENU_BTN12 = "QPushButton { background-color:transparent; background-color:#FFFFFF; color: #9D9D9D; font: bold; padding: 0px; border: 1px solid #AEAEAE; border-radius: 7px; }\
													QPushButton:hover { background-color:#ADD2E8; }\
													QPushButton:checked { background-color: #94CAFC; color: #000000; }\
													QPushButton:disabled { background-color:#FFFFFF; color:#9D9D9D; }";

		const QString DialogStyle::MENU_BTN13 = "QPushButton { background-color:transparent; background-color:#FFFFFF; color: #9D9D9D; font: bold; padding: 0px; border: 1px solid #AEAEAE; border-radius: 7px; }\
													QPushButton:hover { background-color:#ADD2E8; }\
													QPushButton:checked { background-color: #94CAFC; color: #000000; }\
													QPushButton:disabled { background-color:#FFFFFF; color:#9D9D9D; }";

		const QString DialogStyle::MENU_BTN14 = "QPushButton { background-color: transparent; background-image: url('./data/run.png'); color: #000000; font: bold; padding: 0px; border-radius: 7px; }\
													QPushButton:hover { background-color: #5CABFB; background-image: url('./data/run_clicked.png'); }";
													

		const QString DialogStyle::DATE_EDIT = "QDateEdit { background-color:transparent; background-color:#EFEFEF; color: #000000; font: bold; padding: 0px; border: 1px solid #AEAEAE; border-radius: 0px; }\
													QDateEdit:hover { background-color:#EFEFEF; }\
													QDateEdit:checked { background-color: #46719F;}\
													QDateEdit:disabled { background-color:#DADADA; color:#9D9D9D; }\
                                                    QAbstractItemView:enabled {background-color:#EFEFEF; }";

		const QString DialogStyle::IMAGE_LABEL = "QLabel{background-color : rgba(0, 0, 0, 0%); image: url(:/ETCad/res/image/logo.png);}";

		const QString DialogStyle::RADIOBUTTON_WIDGET = "QRadioButton{ color:#000000; font: bold; margin-top: 0px; margin-left: 3px;margin-bottom: 3px;\
																background-color: transparent; text-align : top; padding-top:0px; }\
																QRadioButton::indicator:unchecked{ image: url('./data/image/radio_nor.png'); }\
																QRadioButton::indicator:checked{ image: url('./data/image/radio_pre.png'); }";
		
		// 후보: #63A4DF; #588CE0; 
		const QString DialogStyle::GROUPBOX_WIDGET = "QGroupBox{ background-color : rgba(0, 0, 0, 20%); height: 60px; border: 0px; background-color:#CDDEF3   ; border-radius: 0px; \
																		 color:#000000; font: bold; margin-top: 0px; margin-left: 3px;margin-right: 0px;}\
															 QGroupBox::title{margin-left: 6px; margin-bottom: 2px;}";
		// FED2DC	
		const QString DialogStyle::GROUPBOX_WIDGET2 = "QGroupBox{ background-color : rgba(0, 0, 0, 20%); height: 60px; border: 0px; background-color:#FACCCF; border-radius: 0px; \
																		 color:#000000; font: bold; margin-top: 0px; margin-left: 3px;margin-right: 0px;}\
															 QGroupBox::title{margin-left: 6px; margin-bottom: 2px;}";
		// FFD5D5
		const QString DialogStyle::GROUPBOX_WIDGET3 = "QGroupBox{ background-color : rgba(0, 0, 0, 20%); height: 60px; border: 0px; background-color:#FBD7DA; border-radius: 0px; \
																		 color:#000000; font: bold; margin-top: 0px; margin-left: 3px;margin-right: 0px;}\
															 QGroupBox::title{margin-left: 6px; margin-bottom: 2px;}";

		const QString DialogStyle::GROUPBOX_WIDGET4 = "QGroupBox{ background-color : rgba(0, 0, 0, 20%); height: 60px; border: 0px; background-color:#DCDCDC; border-radius: 1px; \
																		 color:#000000; font: bold; margin-top: 0px; margin-left: 3px;margin-right: 0px;}\
															 QGroupBox::title{margin-left: 6px; margin-bottom: 2px;}";

		const QString DialogStyle::GROUPBOX_WIDGET5 = "QGroupBox{ background-color : rgba(0, 0, 0, 20%); height: 60px; border: 0px; background-color:#EDEDED; border-radius: 1px; \
																		 color:#000000; font: bold; margin-top: 0px; margin-left: 3px;margin-right: 0px;}\
															 QGroupBox::title{margin-left: 6px; margin-bottom: 2px;}";

		const QString DialogStyle::GROUPBOX_WIDGET6 = "QGroupBox{ background-color : rgba(0, 0, 0, 20%); height: 60px; border: 0px; background-color:#58879b; border-radius: 1px; \
																		 color:#000000; font: bold; margin-top: 0px; margin-left: 3px;margin-right: 0px;}\
															 QGroupBox::title{margin-left: 6px; margin-bottom: 2px;}";
		const QString DialogStyle::GROUPBOX_WIDGET7 = "QGroupBox{ background-color : rgba(0, 0, 0, 0%); height: 60px; border: 0px; background-color:#B9D7C4; border-radius: 1px; \
																		 color:#000000; font: bold; margin-top: 0px; margin-left: 3px;margin-right: 0px;}\
															 QGroupBox::title{margin-left: 6px; margin-bottom: 2px;}";
		const QString DialogStyle::GROUPBOX_WIDGET8 = "QGroupBox{ background-color : rgba(0, 0, 0, 20%); height: 60px; border: 0px; background-color:#A8C6B3; border-radius: 1px; \
																		 color:#000000; font: bold; margin-top: 0px; margin-left: 3px;margin-right: 0px;}\
															 QGroupBox::title{margin-left: 6px; margin-bottom: 2px;}";
		const QString DialogStyle::GROUPBOX_WIDGET9 = "QGroupBox{ background-color : rgba(0, 0, 0, 20%); height: 60px; border: 0px; background-color:#A8C6EA; border-radius: 1px; \
																		 color:#000000; font: bold; margin-top: 0px; margin-left: 3px;margin-right: 0px;}\
															 QGroupBox::title{margin-left: 6px; margin-bottom: 2px;}";		
		const QString DialogStyle::GROUPBOX_WIDGET10 = "QGroupBox{ background-color : rgba(0, 0, 0, 20%); border: 0px; background-color:#CDDEF3; border-radius: 5px; \
																		 color:#000000; font: bold; margin-top: 0px; margin-left: 3px;margin-right: 0px;}\
														 QGroupBox::title{margin-left: 6px; margin-bottom: 2px;}";		
		const QString DialogStyle::GROUPBOX_WIDGET11 = "QGroupBox{ background-color : rgba(0, 0, 0, 20%); border: 0px; background-color:#EDEDED; border-radius: 5px; \
																		 color:#000000; font: bold; margin-top: 0px; margin-left: 3px;margin-right: 0px;}\
														 QGroupBox::title{margin-left: 6px; margin-bottom: 2px;}";
		const QString DialogStyle::GROUPBOX_WIDGET12 = "QGroupBox{ background-color:#CDDEF3; border: 1px solid #808080; border-radius: 5px; \
                                                             color:#000000; font-weight: bold; margin-top: 0px; margin-left: 3px; margin-right: 0px; padding: 5px;}\
                                             QGroupBox::title{subcontrol-origin: margin; subcontrol-position: top left; margin-left: 6px; margin-bottom: 2px;}";	
		const QString DialogStyle::GROUPBOX_WIDGET13 = "QGroupBox{ background-color:#B9D7C4; border: 1px solid #808080; border-radius: 5px; \
                                                            color:#000000; font-weight: bold; margin-top: 0px; margin-left: 3px; margin-right: 0px; padding: 5px;}\
                                            QGroupBox::title{subcontrol-origin: margin; subcontrol-position: top left; margin-left: 6px; margin-bottom: 2px;}";		
		const QString DialogStyle::GROUPBOX_WIDGET14 = "QGroupBox{ background-color:#FBD7DA; border: 1px solid #808080; border-radius: 5px; \
                                                            color:#000000; font-weight: bold; margin-top: 0px; margin-left: 3px; margin-right: 0px; padding: 5px;}\
                                            QGroupBox::title{subcontrol-origin: margin; subcontrol-position: top left; margin-left: 6px; margin-bottom: 2px;}";
		const QString DialogStyle::GROUPBOX_WIDGET15 = "QGroupBox{ background-color:#EDEDED; border: 1px solid #808080; border-radius: 5px; \
                                                             color:#000000; font-weight: bold; margin-top: 0px; margin-left: 3px; margin-right: 0px; padding: 5px;}\
                                             QGroupBox::title{subcontrol-origin: margin; subcontrol-position: top left; margin-left: 6px; margin-bottom: 2px;}";
	

		const QString DialogStyle::THUMBNAIL_TABLE = "QTableWidget { background:transparent; background-color: #dee9f3; margin-top: 0px; margin-left: 0px; margin-bottom: 43px; border-radius: 0px; }";

		const QString DialogStyle::TABLE_WIDGET = "QWidget {background-color: #EFEFEF; border-style:hidden; color:black;}\
														 QTableWidget { gridline-color: white; selection-color: white; border-color:#a5a5b9; border-style:solid; }\
														 QTableWidget::item:selected { background-color: #3399FF; }\
														 QHeaderView::section { background-color: #E2E2E2; padding: 0px; border: 1px solid #AEAEAE; font-size: 13px; color:#000000; font: bold; height:35px; \
														 margin-left: -1px;  margin-right: 1px;  margin-top: -1px; margin-bottom: 1px; }\
														 QHeaderView::section:horizontal { }\
														 QHeaderView::section:vertical { }";

		const QString DialogStyle::DATA_LABEL = "QLabel{background-color : rgba(0, 0, 0, 0%); font-family:\"Noto Sans\"; \
														color:black; border: 0px; font-weight:800;}\
														QLabel:disabled{ color:#9D9D9D; }";

		const QString DialogStyle::DATA_LABEL2 = "QLabel{background-color : rgba(0, 0, 0, 0%);font-family:\"Noto Sans\"; \
														color:white; border: 0px; font-weight:800;}\
														QLabel:disabled{ color:#9D9D9D; }";

		const QString DialogStyle::DATA_LABEL3 = "QLabel{background-color : rgba(0, 0, 0, 0%);font-family:\"Noto Sans\"; \
														color:black; border: 0px; font-weight:1600;}\
														QLabel:disabled{ color:#9D9D9D; }";
		const QString DialogStyle::DATA_LABEL4 = "QLabel{background-color : rgba(0, 0, 0, 0%);font-family:\"Noto Sans\"; \
														color:black; border: 0px; font-weight:800;}\
														QLabel:disabled{ color:#9D9D9D; }";

		const QString DialogStyle::COMBOBOX = "QComboBox {background:transparent; Text-align:left; background-color:#FFFFFF; border:0px; border-color:black;\
													border-style:inset; font: bold; border: 0px; font-weight:800;}\
													QComboBox QAbstractItemView { padding-top: 1px;border-style:inset; border-color:white; background-color:white; color:black; }\
													QComboBox QAbstractItemView::item {min-height: 20px;}\
													QComboBox::disabled {background:transparent; background-color:#FFFFFF; Text-align:left; border:0px; border-color:gray;}\
													QComboBox::drop-down {width: 22px;}";


		const QString DialogStyle::COMBOBOX2 = "QComboBox {background:transparent; Text-align:left; background-color:#FFFFFF; border:0px; border-color:black;\
													border-style:inset; font: bold; border: 0px; font-weight:800;}\
													QComboBox QAbstractItemView { padding-top: 1px;border-style:inset; border-color:white; background-color:white; color:black; }\
													QComboBox QAbstractItemView::item {min-height: 20px;}\
													QComboBox::disabled {background:transparent; background-color:#FFFFFF; Text-align:left; border:0px; border-color:gray;}\
													QComboBox::drop-down {width: 22px;}";

		const QString DialogStyle::COMBOBOX3 = "QComboBox {background:transparent; Text-align:left; background-color:#FFFFFF; border:0px; border-color:black;\
													border-style:inset; font: bold; border: 0px; font-weight:800;}\
													QComboBox QAbstractItemView { padding-top: 1px;border-style:inset; border-color:white; background-color:white; color:black; }\
													QComboBox QAbstractItemView::item {min-height: 20px;}\
													QComboBox::disabled {background:transparent; background-color:#FFFFFF; Text-align:left; border:0px; border-color:gray;}\
													QComboBox::drop-down {width: 22px;}";


		const QString DialogStyle::LINEEDIT_NAME = "QLineEdit { background-color:#FFFFFF; Text-align:center;\
														 border: 1px; border-color:gray; border-style:inset; font: bold; border: 0px; font-weight:800;}\
													QLineEdit::disabled{ background-color:#B3B2B2; border: 1px; border-color:gray; border-style:inset; font: bold; border: 0px; font-weight:800; }";
		const QString DialogStyle::LINEEDIT_NAME2 = "QLineEdit { background-color:#FFFFFF; Text-align:center;\
														 border: 1px; border-color:gray; border-style:inset; font: bold; border: 0px; font-weight:800;}";
		const QString DialogStyle::LINEEDIT_NAME3 = "QLineEdit { background-color:#FFFFFF; Text-align:center;\
														 border: 1px; border-color:gray; border-style:inset; font: bold; border: 0px; font-weight:800;}";
		const QString DialogStyle::LINEEDIT_NAME4 = "QLineEdit { background-color:#FFFFFF; Text-align:center;\
														 border: 1px; border-color:gray; border-style:inset; font: bold; border: 0px; font-weight:800;}\
													QLineEdit::disabled{ background-color:#B3B2B2; border: 1px; border-color:gray; border-style:inset; font: bold; border: 0px; font-weight:800; }";
		const QString DialogStyle::LINEEDIT_NAME5 = "QLineEdit { background-color:#FFFFFF; Text-align:center;\
														 border: 1px; border-color:gray; border-style:inset; font: bold; border: 0px; font-weight:800;}\
													QLineEdit::disabled{ background-color:#B3B2B2; border: 1px; border-color:gray; border-style:inset; font: bold; border: 0px; font-weight:800; }";
		




		const QString DialogStyle::SLIDER_BUTTON = "QSlider{background: #AFCCF0; border: none; outline: none; margin-top:0px;}\
																		QSlider::groove:horizontal {\
																		border: 1px solid #bbb;\
																		background: white;\
																		height: 20px;\
																		border-radius: 1px;\
																		}\
																		\
																		QSlider::sub-page:horizontal {\
																		background: #C7EF4D;\
																		border: 1px solid #777;\
																		height: 10px;\
																		border-radius: 4px;\
																		}\
																		\
																		QSlider::add-page:horizontal {\
																		background: #E2EFBA;\
																		border: 1px solid #777;\
																		height: 10px;\
																		border-radius: 4px;\
																		}\
																		\
																		QSlider::handle:horizontal {\
																		background: #B9B9FF;\
																		border: 1px solid #777;\
																		height: 25px;\
																		width: 12px;\
																		margin-top:-2px;\
																		margin-bottom: -2px;\
																		border-radius: 1px;\
																		}\
																		\
																		QSlider::handle:horizontal:hover {\
																		background: #6E6EFF;\
																		height: 25px;\
																		width: 12px;\
																		border: 1px solid #444;\
																		border-radius: 1px;\
																		}\
																		QSlider::handle:vertical:disabled { background: #DCDCDC;}";

		const QString DialogStyle::SCROLLBAR_HORIZONTAL_THIN = R"(
																	QScrollBar:horizontal {
																		height: 8px;
																		margin: 0px 0px 0px 0px;
																	}
																	QScrollBar::handle:horizontal {
																		background: gray;
																		min-width: 20px;
																		height: 8px;
																		border-radius: 4px;
																	}
																	QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
																		width: 0px;
																		height: 0px;
																	}
																)";

	

	}
}