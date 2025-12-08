#pragma once

#include <QtWidgets>
#include <ETHuman3D.h>
#include <qlist.h>
#include "ETHuman3DApp.h"
#include "ETInteractorStyle.h"
#include "StyleSheet.h"
#include <map>
#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include "qcustomplot.h"

#include <qcompleter.h>
#include <qstringlist.h>
#include <DBServerConnect.h>

#include <QFile>
#include <QTextStream>
#include <QMutex>
#include <QDateTime>
#include <QDebug>
#include <QCoreApplication>
#include <QMutexLocker>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "SceneDataTypes.h"

// Qt6
#include <QEnterEvent>

using namespace utils::ui;


//************************************** Pre-defined Class/Struct **************************************//
class CustomButton : public QPushButton {
public:
	explicit CustomButton(QWidget *parent = nullptr) : QPushButton(parent) {}

protected:
	void mousePressEvent(QMouseEvent *event) override {
		if (event->button() == Qt::RightButton) {
			// Show context menu
			showContextMenu(event->pos());
			return;
		}
		if (event->button() == Qt::LeftButton) {
			dragStartPos = event->pos();
		}
		QPushButton::mousePressEvent(event);
	}

	void mouseMoveEvent(QMouseEvent *event) override {
		if (event->buttons() & Qt::LeftButton) {
			int distance = (event->pos() - dragStartPos).manhattanLength();
			if (distance >= QApplication::startDragDistance()) {
				theApp.b_IsButtonDragging = true;
				move(mapToParent(event->pos() - dragStartPos));
			}
		}
	}

	void showContextMenu(const QPoint &pos) {
		QMenu contextMenu;
		QAction *action1 = contextMenu.addAction("Action 1");
		QAction *action2 = contextMenu.addAction("Action 2");
		// Add more actions as needed

		QAction *selectedAction = contextMenu.exec(mapToGlobal(pos));
		if (selectedAction) {
			// Check which action was selected and call the corresponding function
			if (selectedAction == action1) {
				performAction1();
			}
			else if (selectedAction == action2) {
				performAction2();
			}
			// Add more conditions for additional actions
		}
	}

	void performAction1() {
		theApp.SetMessageBox("action1");
		// Implement the function for Action 1
	}

	void performAction2() {
		theApp.SetMessageBox("action2");
		// Implement the function for Action 2
	}

private:
	QPoint dragStartPos;
};
class LeftButton : public QPushButton {
public:
    LeftButton(QWidget *parent = nullptr) : QPushButton(parent) {
        // Set the default and hover icons
        defaultIcon = QIcon("./data/image/left.png");
        hoverIcon = QIcon("./data/image/left_clicked.png");
        // Set the default icon initially
        setIcon(defaultIcon);
    }
protected:
    // [수정] enterEvent는 Qt 6에서 'QEnterEvent *'를 받습니다.
    void enterEvent(QEnterEvent *event) override {
        // 이 함수는 'Enter' 이벤트일 때만 호출되므로 if문이 필요 없습니다.
        setIcon(hoverIcon);
        QPushButton::enterEvent(event);
    }

    // [수정 없음] leaveEvent는 Qt 6에서도 'QEvent *'가 맞습니다.
    void leaveEvent(QEvent *event) override {
        // 이 함수는 'Leave' 이벤트일 때만 호출되므로 if문이 필요 없습니다.
        setIcon(defaultIcon);
        QPushButton::leaveEvent(event);
    }
private:
    QIcon defaultIcon;
    QIcon hoverIcon;
};
class RightButton : public QPushButton {
public:
    RightButton(QWidget *parent = nullptr) : QPushButton(parent) {
        // Set the default and hover icons
        defaultIcon = QIcon("./data/image/right.png");
        hoverIcon = QIcon("./data/image/right_clicked.png");
        // Set the default icon initially
        setIcon(defaultIcon);
    }
protected:
    // [수정] enterEvent는 Qt 6에서 'QEnterEvent *'를 받습니다.
    void enterEvent(QEnterEvent *event) override {
        // 'Enter' 이벤트일 때만 호출되므로 if문 제거
        setIcon(hoverIcon);
        QPushButton::enterEvent(event);
    }

    // [수정 없음] leaveEvent는 Qt 6에서도 'QEvent *'가 맞습니다.
    void leaveEvent(QEvent *event) override {
        // 'Leave' 이벤트일 때만 호출되므로 if문 제거
        setIcon(defaultIcon);
        QPushButton::leaveEvent(event);
    }
private:
    QIcon defaultIcon;
    QIcon hoverIcon;
};
class MultipleButtonScorllArea : public QScrollArea
{
	Q_OBJECT

public:
	explicit MultipleButtonScorllArea(QWidget *parent = nullptr) : QScrollArea(parent) {}

protected:
	void wheelEvent(QWheelEvent *event) override
	{
		// Check if the wheel event is vertical
		if (event->angleDelta().y() != 0)
		{
			// Ignore the vertical scroll event to lock vertical scrolling
			event->ignore();
		}
		else
		{
			// Otherwise, propagate the event (for horizontal scrolling, if needed)
			QScrollArea::wheelEvent(event);
		}
	}
};
class CalculationSettingDialog : public QDialog {
public:
	CalculationSettingDialog(QWidget *parent = nullptr) : QDialog(parent) {
	}

protected:
	void closeEvent(QCloseEvent *event) override {
		if (theApp.SettingContentsChangeCheck())
		{
			if (QMessageBox::Yes == theApp.SetMessageBox_YesNo("Do you want to exit without saving the settings?"))
			{
				event->accept();
			}
			else
			{
				event->ignore();
			}
		}
	}
public:
};
class MultipleUIDialog : public QDialog {
public:
	MultipleUIDialog(QWidget *parent = nullptr) : QDialog(parent) {
	}

public:
	void closeEvent(QCloseEvent *event) override 
	{
		theApp.MultipleUICloseTrigger();
	}
	void keyPressEvent(QKeyEvent *event) override {
		if (event->key() == Qt::Key_Escape) {
			// Your function to call when Escape is pressed			
			close();
		}
		else {
			// Call the base class implementation for other keys
			QDialog::keyPressEvent(event);
		}
	}

public:
};
class ObjectMaterialTextWindow : public QDialog
{
public:
    ObjectMaterialTextWindow(QWidget *parent = nullptr) : QDialog(parent) {
        setStyleSheet("background-color: white;");

        // [수정] Qt 6 방식으로 기본 스크린의 크기를 가져옵니다.
        QScreen *primaryScreen = QGuiApplication::primaryScreen();
        if (primaryScreen)
        {
            // Qt 5 코드: resize(QApplication::desktop()->screenGeometry().width() * 0.15, ...);
            // Qt 6 코드:
            resize(primaryScreen->geometry().width() * 0.15, primaryScreen->geometry().height() * 0.45);
        }
        else
        {
            // (혹시 모를 예외처리) 스크린을 찾지 못할 경우의 기본 크기
            resize(300, 500); 
        }

        setWindowTitle("Material elemental composition edit");

        QVBoxLayout *layout = new QVBoxLayout(this);
        textEdit = new QTextEdit(this);

        layout->addWidget(textEdit);
        setLayout(layout);
    }

    QString getText() const
    {
        return savedText;
    }
    void setText(const QString &text)
    {
        savedText = text;
        textEdit->setText(savedText);
    }

    QTextEdit *textEdit;

protected:
    void closeEvent(QCloseEvent *event) override
    {
        savedText = textEdit->toPlainText();
        QDialog::closeEvent(event);
    }

private:
    QString savedText = "";
};
class FlexibleValidator : public QValidator
{
	Q_OBJECT
public:
	enum Rule : quint32 {
		AllowPositiveInt = 0x01,   // 1,2,3…
		AllowReal = 0x02,   // 실수(±, e표기 포함)
		ReplaceSpace = 0x04,   // 공백 → '_'
		AllowSignedInt = 0x08,   // 0, +정수, -정수 모두
		AllowPositiveReal = 0x10,  //  0 제외한 양의 실수만
		NoRule = 0x00
	};
	Q_DECLARE_FLAGS(Rules, Rule)

		explicit FlexibleValidator(Rules rules, QObject *parent = nullptr)
		: QValidator(parent), m_rules(rules) {}

	State validate(QString &input, int &) const override
	{
		if (m_rules.testFlag(ReplaceSpace))
			input.replace(' ', '_');

		/* 숫자 검사가 필요 없는 경우 */
		const bool needsNumberCheck = m_rules & (AllowPositiveInt | AllowSignedInt | AllowReal | AllowPositiveReal);
		if (!needsNumberCheck)
			return Acceptable;

		/* ===== Acceptable 판정 ===== */
		if (m_rules.testFlag(AllowPositiveInt) && matchPosIntAccept(input))
			return Acceptable;

		if (m_rules.testFlag(AllowSignedInt) && matchSignedIntAccept(input))
			return Acceptable;

		if (m_rules.testFlag(AllowReal) && matchRealAccept(input))
			return Acceptable;

		if (m_rules.testFlag(AllowPositiveReal) && matchPosRealAccept(input))
			return Acceptable;

		/* ===== Intermediate(편집 중) 판정 ===== */
		if (m_rules.testFlag(AllowPositiveInt) && matchPosIntInter(input))
			return Intermediate;

		if (m_rules.testFlag(AllowSignedInt) && matchSignedIntInter(input))
			return Intermediate;

		if (m_rules.testFlag(AllowReal) && matchRealInter(input))
			return Intermediate;

		if (m_rules.testFlag(AllowPositiveReal) && matchPosRealInter(input))
			return Intermediate;

		return Invalid;   // 나머지는 전부 거부 ‑> 키 자체가 안 찍힘
	}

private:
	/* ---------- 정규식 헬퍼 ---------- */
	static bool matchPosIntAccept(const QString &s)
	{
		static const QRegularExpression re(R"(^[1-9]\d*$)"); return re.match(s).hasMatch();
	}

	static bool matchPosIntInter(const QString &s)
	{
		static const QRegularExpression re(R"(^\d*$)");      return re.match(s).hasMatch();
	}

	static bool matchSignedIntAccept(const QString &s)
	{
		static const QRegularExpression re(R"(^-?(0|[1-9]\d*)$)"); return re.match(s).hasMatch();
	}

	static bool matchSignedIntInter(const QString &s)
	{
		static const QRegularExpression re(R"(^-?\d*$)");          return re.match(s).hasMatch();
	}

	static bool matchRealAccept(const QString &s)
	{
		static const QRegularExpression re(
			R"(^[+-]?((\d+(\.\d*)?)|(\.\d+))([eE][+-]?\d+)?$)"); return re.match(s).hasMatch();
	}

	static bool matchRealInter(const QString &s)
	{
		static const QRegularExpression re(
			R"(^[+-]?(\d*(\.)?\d*)?([eE][+-]?\d*)?$)"); return re.match(s).hasMatch();
	}

	static bool matchPosRealAccept(const QString &s)
	{
		bool ok = false;
		const double v = s.toDouble(&ok);
		return ok && v > 0.0;
	}

	static bool matchPosRealInter(const QString &s)
	{
		static const QRegularExpression re(R"(^\+?(\d*(\.)?\d*)?([eE][+-]?\d*)?$)");
		return re.match(s).hasMatch();
	}

	Rules m_rules;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(FlexibleValidator::Rules)
namespace ValFactory {

	// 1) 0 포함, 양의 정수만
	inline QValidator* makePositiveInt(QObject *p)
	{
		return new FlexibleValidator(FlexibleValidator::AllowPositiveInt, p);
	}

	// 2) 정수 모두 허용
	inline QValidator* makeInt(QObject *p)
	{
		return new FlexibleValidator(FlexibleValidator::AllowSignedInt, p);
	}

	// 3) 실수 모두 허용
	inline QValidator* makeReal(QObject *p)
	{
		return new FlexibleValidator(FlexibleValidator::AllowReal, p);
	}

	// 4) 공백만 '_' 로
	inline QValidator* makeNoSpace(QObject *p)
	{
		return new FlexibleValidator(FlexibleValidator::ReplaceSpace, p);
	}

	// 5) 0 포함, 양의 실수만
	inline QValidator* makePositiveReal(QObject *p)
	{
		return new FlexibleValidator(FlexibleValidator::AllowPositiveReal, p);
	}

} // namespace ValFactory

class FunctionPanelRight : public QWidget
{
	Q_OBJECT
public:
	explicit FunctionPanelRight(QWidget *parent = Q_NULLPTR);
	virtual ~FunctionPanelRight();
	
//************************************** Variables **************************************//
public:
	// UI Resolution
	QScreen *SCREEN = QApplication::primaryScreen();	
	// double Window_width = QApplication::desktop()->screenGeometry().width();
	// double Window_height = QApplication::desktop()->screenGeometry().height();
	double Window_width = SCREEN->geometry().width();
	double Window_height = SCREEN->geometry().height();
	double RatioWtoH = Window_width / Window_height;
	qreal LogicalDPI = SCREEN->logicalDotsPerInch();
	qreal PhysicalDPI = SCREEN->physicalDotsPerInch();
	QSizeF PhysicalSizeMM = SCREEN->physicalSize();
	qreal PhysicalWidthInInches = PhysicalSizeMM.width() / 25.4;
	qreal PhysicalHeightInInches = PhysicalSizeMM.height() / 25.4;
	double PhysicalSizeInches = std::sqrt(PhysicalWidthInInches * PhysicalWidthInInches + PhysicalHeightInInches * PhysicalHeightInInches);
	double PhysFactor = PhysicalSizeInches * PhysicalDPI;
	double Default_window_width = 2560.;
	double Default_window_height = 1440.;
	double Default_RatioWtoH = 2560. / 1440.; // this is just 16:9
	double Default_logicalDPI = 144.; // 150% 배율
	double Default_physFactor = 32. * 93.; 
	/*
	32 Inch * 93 PhysicalDPI (32inch, QHD(2560 * 1440))
	Special calibration for target error scroll bar, time-dose graph
	*/
	double SpecialWidgetSizeFactor = std::pow(PhysFactor / Default_physFactor, 0.1) * std::pow(Default_logicalDPI / LogicalDPI, 0.1);
	//============================================================================================================================//

	// Font
	int FontSizeScaling(int DefaultFontSize)// FontSizeScaling Function
	{		
		double PhysPower = 1.2;
		double logiDPIPower = 0.9;
		double ResRatioPower = 0.6;
		double scale_factor = std::pow(PhysFactor / Default_physFactor, PhysPower) * std::pow(Default_logicalDPI / LogicalDPI, logiDPIPower) * std::pow(Default_RatioWtoH / RatioWtoH, ResRatioPower);
		int calibration = 0;
		if (RatioWtoH <= 1.5) calibration++; // 해상도 세로가 많이 길 때
		if (Window_width <= 1500.) calibration++; // 가로 해상도 자체가 많이 작을 때
		return std::floor(DefaultFontSize * scale_factor) - calibration;
	}
	QFont font_D_BTN1;
	QFont font_D_BTN2;
	QFont font_D_BTN3;
	QFont font_D_BTN4;
	QFont font_D_BTN5;
	QFont font_D_BTN6;
	QFont font_D_BTN7;
	QFont font_D_BTN8;
	QFont font_D_BTN9;
	QFont font_D_BTN10;
	QFont font_D_BTN11;
	QFont font_D_BTN12;
	QFont font_D_BTN13;
	QFont font_D_BTN14;
	QFont font_D_BTN15;
	QFont font_D_BTN16;
	QFont font_D_DE1;
	QFont font_D_RB1;
	QFont font_D_RB2;
	QFont font_D_RB3;
	QFont font_D_GBW1;
	QFont font_D_GBW2;
	QFont font_D_GBW3;
	QFont font_D_GBW4;
	QFont font_D_GBW5;
	QFont font_D_GBW6;
	QFont font_D_GBW7;
	QFont font_D_GBW8;
	QFont font_D_GBW9;
	QFont font_D_GBW10;
	QFont font_D_GBW11;
	QFont font_D_TBW1;
	QFont font_D_LBL1;
	QFont font_D_LBL2;
	QFont font_D_LBL3;
	QFont font_D_LBL4;
	QFont font_D_LBL5;
	QFont font_D_CB1;
	QFont font_D_CB2;
	QFont font_D_CB3;
	QFont font_D_LW1;
	QFont font_D_LW2;
	QFont font_D_LE1;
	QFont font_D_LE2;
	QFont font_D_LE3;
	QFont font_D_LE4;
	QFont font_D_LE5;
	QFont font_D_MAT;
	//============================================================================================================================//
	
	// Common Variables
	QList<QPushButton*> Buttons_FunctionPanelRight; // button 총 집합
	QList<QPushButton*> TempEnabledButtons; // button 총 집합
	QList<QPushButton*> TempDisabledButtons; // button 총 집합
	std::vector<QString> OrganNameList = { "RBM", "BS", "Colon", "Lungs", "Stomach", "Breasts", "Testes/Ovaries",
										"UrinaryBladder", "Oesophagus", "Liver", "Thyroid", "Brain", "SalivaryGlands", "Skin(target)", "Adrenal",
										"ET1", "ET2", "GallBladder", "Heart", "Kidneys", "LymphNodes", "Muscle", "OralMucosa", "Pancreas", "Prostate/Uterus",
										"SmallIntestine", "Spleen", "Thymus", "EyeLens", "Enamel(teeth)", "Eff.dose", "WholeBodyDose" };
	//============================================================================================================================//

	// Main Widget
	QPushButton* Phantom_QPushButton = nullptr;
	QPushButton* Geometry_QPushButton = nullptr;
	QPushButton* Source_QPushButton = nullptr;
	QPushButton* Output_QPushButton = nullptr;
	//============================================================================================================================//

	// Phantom Widget
	QGroupBox* PhantomPanel_QGroupBox = nullptr; /* 전체 팬텀패널 */

		// Phantom Widget - Phantom - Global Variables
		int const_Phantom_MaximumCount = 100; // 최대 팬텀 입력 개수: 100개까지 허용
		int m_Phantom_RAM_Usage = 0; // 팬텀개수에 따른 예상 메모리 사용량
		int m_Phantom_MakingIndex = 0; // delete된것은 고려하지 않고 add 할때마다 늘어나는 생성 인덱스
		int m_Phantom_SelectedIndex = -1; // 현재 선택된 팬텀의 인덱스
		std::vector<int> m_Phantom_SequenceVector; // m_Phantom_SequenceVector = {1,3,2,5 ...} ->현재 UI 상에 위치한 버튼 순서대로 실제 ID가 적힌 것 {1, 3, 2, 5 ...}
		std::map<int, QString> m_Phantom_ButtonName; // Phantom 버튼 이름들
		std::map<int, std::map<int, double>> m_Phantom_MainInfo; // m_Phantom_MainInfo[phantomID][E_PHANTOMPANELINFO]
		enum PhantomMainInfo {
			E_PHANTOMMAININFO_CATEGORY = 0, // Human phantom: 0, Air phantom: 1
			E_PHANTOMMAININFO_GENDER = 1, // Male: 1 , Female: 2
			E_PHANTOMMAININFO_TYPE = 2, // MRCP: 0, Deformable: 1, Imported: 2
			E_PHANTOMMAININFO_POSTURE = 3, // Default: 0, Bending: 1, Kneeling: 2, Sitting: 3, Squatting: 4, Walking: 5
			E_PHANTOMMAININFO_HEIGHT = 4,
			E_PHANTOMMAININFO_WEIGHT = 5,
			E_PHANTOMMAININFO_POS_X = 6,
			E_PHANTOMMAININFO_POS_Y = 7,
			E_PHANTOMMAININFO_POS_Z = 8,
			E_PHANTOMMAININFO_ROT_X = 9,
			E_PHANTOMMAININFO_ROT_Y = 10,
			E_PHANTOMMAININFO_ROT_Z = 11,
			E_PHANTOMMAININFO_AGE = 12, // Adult(35y): 0, 15y: 1, 10y: 2, 5y: 3, 1y: 4, 0y: 5
			E_PHANTOMMAININFO_DUMMY = 13,
			E_PHANTOMMAININFO_FETALAGE = 14 // for PFMRCP phantom only: 0: 8w, 1: 10w, 2:15w, 3:20w, 4:25w, 5:30w, 6:35w, 7:38w  
		};
		enum PhantomCategory {
			E_PHANTOMCATEGORY_HUMAN = 0,
			E_PHANTOMCATEGORY_AIR = 1
		};
		enum PhantomGender {
			E_PHANTOMGENDER_MALE = 0, 
			E_PHANTOMGENDER_FEMALE = 1
		};
		enum PhantomType {
			E_PHANTOMTYPE_ADULTMRCP = 0,
			E_PHANTOMTYPE_PEDIATRICMRCP = 1,
			E_PHANTOMTYPE_PFMRCP = 2,
			E_PHANTOMTYPE_TRANSFORMED = 3,
			E_PHANTOMTYPE_IMPORTED = 4
		};
		enum PhantomPosture {
			E_PHANTOMPOSTURE_STANDING = 0,
			E_PHANTOMPOSTURE_BENDING= 1,
			E_PHANTOMPOSTURE_KNEELING = 2,
			E_PHANTOMPOSTURE_SITTING = 3,
			E_PHANTOMPOSTURE_SQUATTING = 4,
			E_PHANTOMPOSTURE_WALKING = 5,
		};
		enum PhantomAge {
			E_PHANTOMAGE_ADULT = 0,
			E_PHANTOMAGE_15y = 1,
			E_PHANTOMAGE_10y = 2,
			E_PHANTOMAGE_5y = 3,
			E_PHANTOMAGE_1y = 4,
			E_PHANTOMAGE_0y = 5,
		};
		enum PhantomFetalAge {
			E_PHANTOMFETALAGE_8w = 0,
			E_PHANTOMFETALAGE_10w = 1,
			E_PHANTOMFETALAGE_15w = 2,
			E_PHANTOMFETALAGE_20w = 3,
			E_PHANTOMFETALAGE_25w = 4,
			E_PHANTOMFETALAGE_30w = 5,
			E_PHANTOMFETALAGE_35w = 6,
			E_PHANTOMFETALAGE_38w = 7
		};
		enum PhantomDummy {
			E_PHANTOMDUMMY_NO = 0,
			E_PHANTOMDUMMY_YES = 1
		};
		
			// Phantom Widget - Phantom - Global Variables - Setting
			bool m_Is_PhantomSetting_Closed = false; // Setting 시 X버튼 눌렀을 때 true
			bool m_Is_PhantomSetting_OKbutton_Clicked = true; /* Setting의 OK 버튼 눌렀을 때 true */

		// Phantom Widget - Phantom - Qt Variables		
		/**/

			// Phantom Widget - Phantom - Qt Variables - List
			std::map<int, QPushButton*> PhantomSelect_QPushButton;
			QPushButton* PhantomAdd_QPushButton = nullptr;
			QGroupBox* PhantomList_QGroupBox = nullptr;
			QLabel* Phantom_RAMusage_QLabel = nullptr;
			QPushButton* PhantomDelete_QPushButton = nullptr;
			QPushButton* PhantomMoveLeft_UserQPushButton = nullptr;
			QPushButton* PhantomMoveRight_UserQPushButton = nullptr;
			QScrollBar*  PhantomList_Vertical_QScrollBar = nullptr;
			QScrollBar*  PhantomList_Horizontal_QScrollBar = nullptr;
			QHBoxLayout* PhantomList_QHBoxLayout = nullptr;

			// Phantom Widget - Phantom - Qt Variables - Setting
			QLabel* PhantomSetting_Category_QLabel = nullptr;
			QLabel* PhantomSetting_Name_QLabel = nullptr;
			QLabel* PhantomSetting_Type_QLabel = nullptr;
			QLabel* PhantomSetting_Gender_QLabel = nullptr;
			QLabel* PhantomSetting_Age_QLabel = nullptr;
			QLabel* PhantomSetting_Dummy_QLabel = nullptr;
			QLabel* PhantomSetting_Posture_QLabel = nullptr;
			QLabel* PhantomSetting_FetalAge_QLabel = nullptr;
			QLabel* PhantomSetting_Height_QLabel = nullptr;
			QLabel* PhantomSetting_Weight_QLabel = nullptr;
			QLabel* PhantomSetting_Position_QLabel = nullptr;
			QLabel* PhantomSetting_Rotation_QLabel = nullptr;
			QLineEdit* PhantomSetting_Name_QLineEdit = nullptr;
			QLineEdit* PhantomSetting_ImportedFileName_QLineEdit = nullptr;
			QLineEdit* PhantomSetting_Height_QLineEdit = nullptr;
			QLineEdit* PhantomSetting_Weight_QLineEdit = nullptr;
			QLineEdit* PhantomSetting_PosX_QLineEdit = nullptr;
			QLineEdit* PhantomSetting_PosY_QLineEdit = nullptr;
			QLineEdit* PhantomSetting_PosZ_QLineEdit = nullptr;
			QLineEdit* PhantomSetting_RotX_QLineEdit = nullptr;
			QLineEdit* PhantomSetting_RotY_QLineEdit = nullptr;
			QLineEdit* PhantomSetting_RotZ_QLineEdit = nullptr;
			QComboBox* PhantomSetting_Type_QComboBox = nullptr;
			QComboBox* PhantomSetting_Dummy_QComboBox = nullptr;
			QComboBox* PhantomSetting_Gender_QComboBox = nullptr;
			QComboBox* PhantomSetting_Age_QComboBox = nullptr;
			QComboBox* PhantomSetting_Posture_QComboBox = nullptr;
			QComboBox* PhantomSetting_FetalAge_QComboBox = nullptr;
			QPushButton* PhantomSetting_PhantomImport_QPushButton = nullptr;
			QPushButton* PhantomSetting_AddingOK_QPushButton = nullptr;
			QHBoxLayout* PhantomSetting_Type_QHBoxLayout = nullptr;
			QHBoxLayout* PhantomSetting_Dummy_QHBoxLayout = nullptr;
			QHBoxLayout* PhantomSetting_Gender_QHBoxLayout = nullptr;
			QHBoxLayout* PhantomSetting_Age_QHBoxLayout = nullptr;
			QHBoxLayout* PhantomSetting_Posture_QHBoxLayout = nullptr;
			QHBoxLayout* PhantomSetting_FetalAge_QHBoxLayout = nullptr;
			QHBoxLayout* PhantomSetting_Height_QHBoxLayout = nullptr;
			QHBoxLayout* PhantomSetting_Weight_QHBoxLayout = nullptr;
			QHBoxLayout* PhantomSetting_Rot_QHBoxLayout = nullptr;
			MultipleUIDialog* PhantomSetting_UserQDialog = nullptr;
			QRadioButton* PhantomSetting_ComputationlPhantom_QRadioButton = nullptr;
			QRadioButton* PhantomSetting_AirPhantom_QRadioButton = nullptr;

			// Phantom Widget - Phantom - Qt Variables - Status
			QLineEdit* PhantomType_QLineEdit = nullptr;
			QLineEdit* PhantomGender_QLineEdit = nullptr;
			QLineEdit* PhantomAge_QLineEdit = nullptr;
			QLineEdit* PhantomFetalAge_QLineEdit = nullptr;
			QLineEdit* PhantomPosture_QLineEdit = nullptr;
			QLineEdit* PhantomHeight_QLineEdit = nullptr;
			QLineEdit* PhantomWeight_QLineEdit = nullptr;
			QLineEdit* PhantomPosX_QLineEdit = nullptr;
			QLineEdit* PhantomPosY_QLineEdit = nullptr;
			QLineEdit* PhantomPosZ_QLineEdit = nullptr;
			QLineEdit* PhantomRotX_QLineEdit = nullptr;
			QLineEdit* PhantomRotY_QLineEdit = nullptr;
			QLineEdit* PhantomRotZ_QLineEdit = nullptr;
			QPushButton* PhantomUpdate_QPushButton = nullptr;
			QCheckBox* PhantomDummy_QCheckBox = nullptr;	
		//-----------------------------------------------------------------------------------------------------------------//

		// Phantom Widget - Clothing - Global Variables
		int const_Clothing_MaximumCount = 30;
		int m_Clothing_SelectedIndex = -1;
		int m_Clothing_MakingIndex = 0; // delete된것은 고려하지 않고 add 할때마다 늘어나는 생성 인덱스
		int const_ClothingLayer_MaximumCount = 5;
		int m_ClothingLayer_MakingIndex = 0; // delete된것은 고려하지 않고 add 할때마다 늘어나는 생성 인덱스
		int m_ClothingLayer_SelectedIndex = -1;
		std::map<int, std::map<int, int>> m_ClothingLayer_TotalCount; // m_ClothingLayer_TotalCount[phantomID][ClothingID] = 레이어 개수;
		std::map<int, std::vector<int>> m_Clothing_SequenceVector; // m_Clothing_SequenceVector[phantomIdx] = {1,3,2,5 ...} ->현재 UI 상에 위치한 의복 버튼 순서대로 실제 ID가 적힌 것 {1, 3, 2, 5 ...}
		std::map<int, std::map<int, std::vector<int>>> m_ClothingLayer_SequenceVector; // m_Clothing_SequenceVector[phantomIdx][ClothingIdx] = {1,3,2,5 ...} ->현재 UI 상에 위치한 의복 버튼 순서대로 실제 ID가 적힌 것 {1, 3, 2, 5 ...}	
		std::map<int, std::map<int, int>> m_Clothing_Type; // m_Clothing_Type[PhantomIdx][ClothingIndex] = Clothing Type index
		enum ClothingType {
			E_CLOTHINGTYPE_USER = 0,
			E_CLOTHINGTYPE_HEAD = 1,
			E_CLOTHINGTYPE_UPPER_ARM = 2,
			E_CLOTHINGTYPE_LOWER_ARM = 3,
			E_CLOTHINGTYPE_HANDS = 4,
			E_CLOTHINGTYPE_FRONT_TRUNK = 5,
			E_CLOTHINGTYPE_BACK_TRUNK = 6,
			E_CLOTHINGTYPE_PELVIS = 7,
			E_CLOTHINGTYPE_UPPER_LEG = 8,
			E_CLOTHINGTYPE_LOWER_LEG = 9,
			E_CLOTHINGTYPE_FEET = 10
		};
		std::map<int, std::map<int, QString>> m_Clothing_ButtonName; // m_Clothing_ButtonName[PhantomIdx][ClothingIndex] = Clothing Name	
		std::map<int, std::map<int, std::map<int, QString>>> m_ClothingLayer_ButtonName; // m_ClothingLayer_ButtonName[PhantomIdx][ClothingIndex][ClothingLayerIndex] = Layer Name	
		std::map<int, std::map<int, std::map<int, std::map<int, double>>>> m_Clothing_MainInfo;
		enum ClothingMainInfo {
			E_CLOTHINGMAININFO_THICKNESS = 0,
			E_CLOTHINGMAININFO_DENSITY = 1,
			E_CLOTHINGMAININFO_COMPOSITION = 2
		};
		enum ClothingMainInfo_Composition {
			E_CLOTHINGMAININFO_COMPOSITION_CLOTH = 0,
			E_CLOTHINGMAININFO_COMPOSITION_LEAD = 1,
			E_CLOTHINGMAININFO_COMPOSITION_AIR = 2
		};
		/*
		m_Clothing_MainInfo[PhantomIdx][ClothingIndex][LayerIndex][0~2:정보] -1: 사용여부, 0: Thickness, 1: Density, 2: Composition(0: Cloth, 1: Lead, 2: air)		
		m_Clothing_MainInfo[0~99:PhantomIdx][99][0][0] -> UserWearable 총 개수
		m_Clothing_MainInfo[0~99:PhantomIdx][100:glasses][0:one glasses][0~4:정보] 0: distance, 1: radius, 2: thickness, 3: density, 4: composition (1: Glasses, 2: Leaded glasses)
		m_Clothing_MainInfo[0~99:PhantomIdx][999][0][0] -> 해당 팬텀의 dosimeter 총 개수 (~10개)
		m_Clothing_MainInfo[0~99:PhantomIdx][1000:Dosimeter][0~9:dosimetersIdx][0~5:정보] 0: 정보 사용여부 1: radius, 2: distance, 3: rel.PosX, 4: rel.PosY, 5: rel.PosX 
		*/
std::map<int, std::map<int, std::map<int, QString>>> m_PhantomWearableName; // m_PhantomWearableName[0~99:PhantomIdx][1~10 or 21~40:UserWearableIdx][3:Wearable Name]

			// Phantom Widget - Clothing - Global Variables - Setting
			bool m_Is_ClothingSetting_Closed = false;
			bool m_Is_ClothingSetting_OKbutton_Clicked = true;
			bool m_Is_ClothingLayer_Setting_Closed = false;
			bool m_Is_ClothingLayer_Setting_OKbutton_Clicked = true;

		// Phantom Widget - Clothing - Qt Variables
		/**/

			// Phantom Widget - Clothing - Qt Variables - List			
			QLabel* ClothingTitle_QLabel = nullptr;
			QLabel* ClothingLayerTitle_QLabel = nullptr;
			std::map<int, QPushButton*> ClothingSelect_QPushButton;
			std::map<int, QPushButton*> ClothingLayerSelect_QPushButton;
			QPushButton* ClothingAdd_QPushButton;
			QPushButton* ClothingDelete_QPushButton;
			QPushButton* ClothingLayerAdd_QPushButton;
			QPushButton* ClothingLayerDelete_QPushButton;
			QScrollBar*  ClothingList_QScrollBar = nullptr;
			QScrollBar*  ClothingLayerList_QScrollBar = nullptr;
			QHBoxLayout* ClothingList_QHBoxLayout = nullptr;
			QHBoxLayout* ClothingLayerList_QHBoxLayout = nullptr;

			// Phantom Widget - Clothing - Qt Variables - Setting	
			QLineEdit* ClothingSetting_Name_QLineEdit = nullptr;
			QLineEdit* ClothingSetting_Thickness_QLineEdit = nullptr;
			QLineEdit* ClothingSetting_Density_QLineEdit = nullptr;
			QLineEdit* ClothingSetting_Layername_QLineEdit = nullptr;
			QLineEdit* ClothingLayerSetting_Layername_QLineEdit = nullptr;
			QLineEdit* ClothingLayerSetting_Thickness_QLineEdit = nullptr;
			QLineEdit* ClothingLayerSetting_Density_QLineEdit = nullptr;
			QPushButton* ClothingSetting_AddingOK_QPushButton;
			QPushButton* ClothingLayerSetting_AddingOK_QPushButton;
			QComboBox* ClothingSetting_Type_QComboBox = nullptr;
			QComboBox* ClothingSetting_Composition_QComboBox = nullptr;
			QComboBox* ClothingLayerSetting_Composition_QComboBox = nullptr;
			MultipleUIDialog* ClothingSetting_UserQDialog = nullptr;
			MultipleUIDialog* ClothingLayerSetting_UserQDialog = nullptr;

			// Phantom Widget - Clothing - Qt Variables - Status	
			QLineEdit* ClothingThickness_QLineEdit = nullptr;
			QLineEdit* ClothingDensity_QLineEdit = nullptr;
			QComboBox* ClothingComposition_QComboBox = nullptr;
			QPushButton* ClothingUpdate_QPushButton;
			//-----------------------------------------------------------------------------------------------------------------//

		// Phantom Widget - Dosimeter - Global Variables
			int m_Dosimeter_Maximum_Count = 10;
			int m_Dosimeter_Making_Index = 0; // delete된것은 고려하지 않고 add 할때마다 늘어나는 생성 인덱스
			int m_Dosimeter_Selected_Index = 0;
			std::map<int, std::vector<int>> m_DosimeterSequenceVector; // m_DosimeterSequenceVector[phantomIdx] = {1,3,2,5 ...} ->현재 UI 상에 위치한 선량계 버튼 순서대로 실제 ID가 적힌 것 {1, 3, 2, 5 ...}

			// Phantom Widget - Dosimeter - Global Variables - Setting
			/**/

		// Phantom Widget - Dosimeter - Qt Variables
		/**/

			// Phantom Widget - Dosimeter - Qt Variables - List
			std::map<int, QPushButton*> DosimeterSelect_QPushButton;
			QPushButton* DosimeterAdd_QPushButton;
			QPushButton* DosimeterDelete_QPushButton;
			QScrollBar*  DosimeterList_QScrollBar = nullptr;
			QHBoxLayout* DosimeterList_QHBoxLayout = nullptr;

			// Phantom Widget - Dosimeter - Qt Variables - Status
			QLineEdit* DosimeterPosX_QLineEdit;
			QLineEdit* DosimeterPosY_QLineEdit;
			QLineEdit* DosimeterPosZ_QLineEdit;
			QLineEdit* DosimeterRadius_QLineEdit;
			QLineEdit* DosimeterSeparationDistance_QLineEdit;
			QPushButton* DosimeterUpdate_QPushButton;

			// Phantom Widget - Dosimeter - Qt Variables - Setting
			/**/
					   			 
		//-----------------------------------------------------------------------------------------------------------------//

		// Phantom Widget - Eyewear - Global Variables

			// Phantom Widget - Eyewear - Global Variables - Setting
			bool m_Is_EyewearSetting_Closed = false; // 
			bool m_Is_EyewearSetting_OKClicked = true; //

		// Phantom Widget - Eyewear - Qt Variables
		/**/

			// Phantom Widget - Eyewear - Qt Variables - List
			QPushButton* Eyewear_AddDelete_QPushButton = nullptr;
			QPushButton* Eyewear_3DPick_QPushButton = nullptr;

			// Phantom Widget - Eyewear - Qt Variables - Status			
			QLabel* EyewearThickness_QLabel = nullptr;
			QLabel* EyewearThickness_Front_QLabel = nullptr;
			QLabel* EyewearThickness_Side_QLabel = nullptr;
			QLineEdit* EyewearType_QLineEdit = nullptr;
			QLineEdit* EyewearDistance_QLineEdit = nullptr;
			QLineEdit* EyewearRadius_QLineEdit = nullptr;
			QLineEdit* EyewearThickness_QLineEdit = nullptr;
			QLineEdit* EyewearThickness_Front_QLineEdit = nullptr;
			QLineEdit* EyewearThickness_Side_QLineEdit = nullptr;
			QLineEdit* EyewearDensity_QLineEdit = nullptr;
			QPushButton* EyewearUpdate_QPushButton = nullptr;
			QComboBox* EyewearComposition_QComboBox = nullptr;

			// Phantom Widget - Eyewear - Qt Variables - Setting
			QLabel* EyewearSetting_Thickness_QLabel = nullptr;
			QLabel* EyewearSetting_Thickness_Front_QLabel = nullptr;
			QLabel* EyewearSetting_Thickness_Side_QLabel = nullptr;
			QLineEdit* EyewearSetting_Distance_QLineEdit = nullptr;
			QLineEdit* EyewearSetting_Radius_QLineEdit = nullptr;
			QLineEdit* EyewearSetting_Thickness_QLineEdit = nullptr;
			QLineEdit* EyewearSetting_Thickness_Front_QLineEdit = nullptr;
			QLineEdit* EyewearSetting_Thickness_Side_QLineEdit = nullptr;
			QLineEdit* EyewearSetting_Density_QLineEdit = nullptr;
			QPushButton* EyewearSetting_AddingOK_QPushButton = nullptr;
			QComboBox* EyewearSetting_Type_QComboBox = nullptr;
			QComboBox* EyewearSetting_Composition_QComboBox = nullptr;
			MultipleUIDialog* EyewearAdding_UserQDialog = nullptr;	
	
	//============================================================================================================================//
			
	// Geometry Widget
	QGroupBox* GeometryPanel_QGroupBox = nullptr;

		// Geometry Widget - Environment - Global Variables

		// Geometry Widget - Environment - Qt Variables
		QComboBox* Geometry_EnvironmentType_QComboBox = nullptr;
		//-----------------------------------------------------------------------------------------------------------------//

		// Geometry Widget - All Objects - Global Variables
		int const_Object_MaximumCount = 500; // Geometry 패널의 Object 최대 입력 개수
		int m_Object_MakingIndex = 0; // delete된것은 고려하지 않고 add 할때마다 늘어나는 생성 인덱스 -> Group도 포함
		int m_Object_SelectedIndex = -1;
		std::vector<int> m_Object_SequenceVector; // m_Object_SequenceVector = {1,3,2,5 ...} ->현재 UI 상에 위치한 object 버튼 순서대로 실제 ID가 적힌 것 {1, 3, 2, 5 ...}

		std::map<int, int> m_ObjectType; // m_ObjectType[objectMakingIndex] = {0,1,1,0,2,1,1} ->making index 순서대로 object type 정보(0: Box, 1: Sphere, 2: Cylinder)
		enum ObjectType {
			E_OBJECTTYPE_BOX = 0,
			E_OBJECTTYPE_SPHERE = 1,
			E_OBJECTTYPE_CYLINDER = 2
		};
		std::map<int, QString> m_Object_ButtonName; // m_Object_ButtonName[objectMakingIndex] = {"MyBox"}
		std::map<int, std::map<int, double>> m_Object_MainInfo; // m_Object_MainInfo[m_Object_MakingIndex][0~:정보]
		enum ObjectMainInfo {
			E_OBJECTMAININFO_USED = 0, // 정보사용여부
			E_OBJECTMAININFO_POS_X = 1, 
			E_OBJECTMAININFO_POS_Y = 2,
			E_OBJECTMAININFO_POS_Z = 3, 
			E_OBJECTMAININFO_ROT_X = 4,
			E_OBJECTMAININFO_ROT_Y = 5,
			E_OBJECTMAININFO_ROT_Z = 6,
			E_OBJECTMAININFO_HALFLEN_X = 7,
			E_OBJECTMAININFO_HALFLEN_Y = 8,
			E_OBJECTMAININFO_HALFLEN_Z = 9,
			E_OBJECTMAININFO_THICKNESS_X = 10,
			E_OBJECTMAININFO_THICKNESS_Y = 11,
			E_OBJECTMAININFO_THICKNESS_Z = 12, 
			E_OBJECTMAININFO_DENSITY = 13,
			E_OBJECTMAININFO_MINRAD_SPH = 14,
			E_OBJECTMAININFO_MAXRAD_SPH = 15,
			E_OBJECTMAININFO_STARTING_AZI = 16,
			E_OBJECTMAININFO_DELTA_AZI = 17,
			E_OBJECTMAININFO_STARTING_POL = 18,
			E_OBJECTMAININFO_DELTA_POL = 19,
			E_OBJECTMAININFO_MINRAD_CYL = 20,
			E_OBJECTMAININFO_MAXRAD_CYL = 21,
			E_OBJECTMAININFO_HALFHT_X = 22,
			E_OBJECTMAININFO_STARTING_ANG = 23,
			E_OBJECTMAININFO_DELTA_ANG = 24
		};
		std::map<int, QString> m_Object_MaterialInfo; // m_Object_MaterialInfo[m_Object_MakingIndex] = Material text	
		std::map<int, int> m_Object_MaterialFractionType; // m_ObjectMaterialFractionTypeVector[objectMakingIndex] = 0 or 1 (mass fraction / atom fraction)
		std::map<int, bool> m_Object_ChargedParticleTransport; // m_Object_ChargedParticleTransport[m_Object_MakingIndex] = True or false
		std::map<int, double> m_Object_Transparency; // m_Object_Transparency[m_Object_SelectedIndex] = 50;

			// Geometry Widget - All Objects - Global Variables - Setting
			bool b_IsObjectSettingClosed = false; //
			bool b_IsObjectSettingOKClicked = true; // 

		// Geometry Widget - All Objects - Qt Variables
		/**/
			// Geometry Widget - All Objects - Qt Variables - List
			std::map<int, QPushButton*> ObjectSelect_QPushButton;			
			QPushButton* ObjectGroup_QPushButton = nullptr;
			QPushButton* ObjectAdd_QPushButton = nullptr;
			QPushButton* ObjectDelete_QPushButton = nullptr;
			QPushButton* ObjectMoveLeft_UserQPushButton = nullptr;
			QPushButton* ObjectMoveRight_UserQPushButton = nullptr;
			QScrollBar*  ObjectList_QScrollBar = nullptr;
			QHBoxLayout* ObjectList_QHBoxLayout = nullptr;
			
			// Geometry Widget - All Objects - Qt Variables - Setting
			QLabel* ObjectSetting_BoxLength_QLabel = nullptr;
			QLabel* ObjectSetting_BoxThickness_QLabel = nullptr;
			QLabel* ObjectSetting_BoxCenter_QLabel = nullptr;
			QLabel* ObjectSetting_BoxRotate_QLabel = nullptr;
			QLabel* ObjectSetting_SphereRadius_QLabel = nullptr;
			QLabel* ObjectSetting_SphereAzimuthalPhi_QLabel = nullptr;
			QLabel* ObjectSetting_SPolarTheta_QLabel = nullptr;
			QLabel* ObjectSetting_SCenter_QLabel = nullptr;
			QLabel* ObjectSetting_SRotate_QLabel = nullptr;
			QLabel* ObjectSetting_CRadius_QLabel = nullptr;
			QLabel* ObjectSetting_CHeightZ_QLabel = nullptr;
			QLabel* ObjectSetting_CAngle_QLabel = nullptr;
			QLabel* ObjectSetting_CCenter_QLabel = nullptr;
			QLabel* ObjectSetting_CRotate_QLabel = nullptr;
			QLabel* ObjectSetting_Material_QLabel = nullptr;
			QLineEdit* ObjectSetting_Name_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_BoxLengthX_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_BoxLengthY_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_BoxLengthZ_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_BoxThicknessX_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_BoxThicknessY_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_BoxThicknessZ_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_BoxCenterX_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_BoxCenterY_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_BoxCenterZ_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_BoxRotateX_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_BoxRotateY_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_BoxRotateZ_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_SphereInnerRadius_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_SphereOuterRadius_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_SphereStartingAzimuthalPhi_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_SphereDeltaAzimuthalPhi_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_SphereStartingPolarTheta_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_SphereDeltaPolarTheta_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_SphereCenterX_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_SphereCenterY_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_SphereCenterZ_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_SphereRotateX_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_SphereRotateY_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_SphereRotateZ_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_CylinderInnerRadius_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_CylinderOuterRadius_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_CylinderHeightZ_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_CylinderStartingAngle_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_CylinderDeltaAngle_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_CylinderCenterX_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_CylinderCenterY_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_CylinderCenterZ_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_CylinderRotateX_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_CylinderRotateY_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_CylinderRotateZ_QLineEdit = nullptr;
			QLineEdit* ObjectSetting_Density_QLineEdit = nullptr;
			QPushButton* ObjectSetting_MaterialEdit_QPushButton = nullptr;
			QPushButton* ObjectAddingOK_QPushButton = nullptr;
			QComboBox* ObjectSetting_Type_QComboBox = nullptr;
			QRadioButton* ObjectSetting_MaterialMassFraction_QRadioButton = nullptr;
			QRadioButton* ObjectSetting_MaterialAtomFraction_QRadioButton = nullptr;
			QHBoxLayout* ObjectSetting_BoxLength_QHBoxLayout = nullptr;
			QHBoxLayout* ObjectSetting_BoxThickness_QHBoxLayout = nullptr;
			QHBoxLayout* ObjectSetting_BoxCenter_QHBoxLayout = nullptr;
			QHBoxLayout* ObjectSetting_BoxRotate_QHBoxLayout = nullptr;
			QHBoxLayout* ObjectSetting_SphereRadius_QHBoxLayout = nullptr;
			QHBoxLayout* ObjectSetting_SphereAzimuthalPhi_QHBoxLayout = nullptr;
			QHBoxLayout* ObjectSetting_SpherePolarTheta_QHBoxLayout = nullptr;
			QHBoxLayout* ObjectSetting_SphereCenter_QHBoxLayout = nullptr;
			QHBoxLayout* ObjectSetting_SphereRotate_QHBoxLayout = nullptr;
			QHBoxLayout* ObjectSetting_CylinderRadius_QHBoxLayout = nullptr;
			QHBoxLayout* ObjectSetting_CylinderHeightZ_QHBoxLayout = nullptr;
			QHBoxLayout* ObjectSetting_CylinderAngle_QHBoxLayout = nullptr;
			QHBoxLayout* ObjectSetting_CylinderCenter_QHBoxLayout = nullptr;
			QHBoxLayout* ObjectSetting_CylinderRotate_QHBoxLayout = nullptr;
			ObjectMaterialTextWindow* ObjectSetting_MaterialPanelTextWindow_UserQDialog = nullptr;
			MultipleUIDialog* ObjectAdding_UserQDialog = nullptr;

		//-----------------------------------------------------------------------------------------------------------------//
					   
		// Geometry Widget - Box - Global Variables

			// Geometry Widget - Box - Global Variables - Setting
			/**/

		// Geometry Widget - Box - Qt Variables
		/**/
			// Geometry Widget - Box - Qt Variables - Status
			QGroupBox* ObjectBox_QGroupBox = nullptr;
			QLabel* ObjectBox_Type_QLabel = nullptr;
			QLabel* ObjectBox_Length_QLabel = nullptr;
			QLabel* ObjectBox_Thickness_QLabel = nullptr;
			QLabel* ObjectBox_Center_QLabel = nullptr;
			QLabel* ObjectBox_Rotate_QLabel = nullptr;
			QLabel* ObjectBox_Material_QLabel = nullptr;
			QLabel* ObjectBox_Density_QLabel = nullptr;
			QLabel* ObjectBox_Transparency_QLabel = nullptr;
			QLabel* ObjectBox_ChargedParticleTransport_QLabel = nullptr;
			QLineEdit* ObjectBox_Type_QLineEdit = nullptr;
			QLineEdit* ObjectBox_CenterX_QLineEdit = nullptr;
			QLineEdit* ObjectBox_CenterY_QLineEdit = nullptr;
			QLineEdit* ObjectBox_CenterZ_QLineEdit = nullptr;
			QLineEdit* ObjectBox_LengthX_QLineEdit = nullptr;
			QLineEdit* ObjectBox_LengthY_QLineEdit = nullptr;
			QLineEdit* ObjectBox_LengthZ_QLineEdit = nullptr;
			QLineEdit* ObjectBox_RotateX_QLineEdit = nullptr;
			QLineEdit* ObjectBox_RotateY_QLineEdit = nullptr;
			QLineEdit* ObjectBox_RotateZ_QLineEdit = nullptr;
			QLineEdit* ObjectBox_ThicknessX_QLineEdit = nullptr;
			QLineEdit* ObjectBox_ThicknessY_QLineEdit = nullptr;
			QLineEdit* ObjectBox_ThicknessZ_QLineEdit = nullptr;
			QLineEdit* ObjectBox_Density_QLineEdit = nullptr;
			QPushButton* Object_BoxMaterialEdit_QPushButton = nullptr;
			QPushButton* Object_BoxUpdate_QPushButton = nullptr;
			QPushButton* Object_Box_ChargedParticleTransport_QPushButton = nullptr;
			QSlider* Object_BoxTransparency_QSlider = nullptr;
			QRadioButton* ObjectBox_MaterialMassFraction_QRadioButton = nullptr;
			QRadioButton* ObjectBox_MaterialAtomFraction_QRadioButton = nullptr;
			ObjectMaterialTextWindow* ObjectBox_MaterialPanelTextWindow_UserQDialog = nullptr;

		//-----------------------------------------------------------------------------------------------------------------//

		// Geometry Widget - Sphere - Global Variables

			// Geometry Widget - Sphere - Global Variables - Setting
			/**/

		// Geometry Widget - Sphere - Qt Variables
		/**/
			// Geometry Widget - Sphere - Qt Variables - Status
			QGroupBox* ObjectCylinder_QGroupBox = nullptr;
			QLabel* ObjectSphere_Type_QLabel = nullptr;
			QLabel* ObjectSphere_Radius_QLabel = nullptr;
			QLabel* ObjectSphere_AzimuthalPhi_QLabel = nullptr;
			QLabel* ObjectSphere_PolarTheta_QLabel = nullptr;
			QLabel* ObjectSphere_Center_QLabel = nullptr;
			QLabel* ObjectSphere_Rotate_QLabel = nullptr;
			QLabel* ObjectSphere_Material_QLabel = nullptr;
			QLabel* ObjectSphere_Density_QLabel = nullptr;
			QLabel* ObjectSphere_Transparency_QLabel = nullptr;
			QLabel* ObjectSphere_ChargedParticleTransport_QLabel = nullptr;
			QLineEdit* ObjectSphere_Type_QLineEdit = nullptr;
			QLineEdit* ObjectSphere_CenterX_QLineEdit = nullptr;
			QLineEdit* ObjectSphere_CenterY_QLineEdit = nullptr;
			QLineEdit* ObjectSphere_CenterZ_QLineEdit = nullptr;
			QLineEdit* ObjectSphere_OuterRadius_QLineEdit = nullptr;
			QLineEdit* ObjectSphere_InnerRadius_QLineEdit = nullptr;
			QLineEdit* ObjectSphere_StartingAzimuthalPhi_QLineEdit = nullptr;
			QLineEdit* ObjectSphere_DeltaAzimuthalPhi_QLineEdit = nullptr;
			QLineEdit* ObjectSphere_StartingPolarTheta_QLineEdit = nullptr;
			QLineEdit* ObjectSphere_DeltaPolarTheta_QLineEdit = nullptr;
			QLineEdit* ObjectSphere_RotateX_QLineEdit = nullptr;
			QLineEdit* ObjectSphere_RotateY_QLineEdit = nullptr;
			QLineEdit* ObjectSphere_RotateZ_QLineEdit = nullptr;
			QLineEdit* ObjectSphere_Density_QLineEdit = nullptr;
			QPushButton* Object_SphereMaterialEdit_QPushButton = nullptr;
			QPushButton* Object_SphereUpdate_QPushButton = nullptr;
			QPushButton* Object_Sphere_ChargedParticleTransport_QPushButton = nullptr;
			QSlider* Object_SphereTransparency_QSlider = nullptr;
			QRadioButton* ObjectSphere_MaterialMassFraction_QRadioButton = nullptr;
			QRadioButton* ObjectSphere_MaterialAtomFraction_QRadioButton = nullptr;
			ObjectMaterialTextWindow* ObjectSphere_MaterialPanelTextWindow_UserQDialog = nullptr;

		//-----------------------------------------------------------------------------------------------------------------//

		// Geometry Widget - Cylinder - Global Variables

			// Geometry Widget - Cylinder - Global Variables - Setting
			/**/

		// Geometry Widget - Cylinder - Qt Variables
		/**/
			// Geometry Widget - Cylinder - Qt Variables - Status
			QGroupBox* ObjectSphere_QGroupBox = nullptr;
			QLabel* ObjectCylinder_Type_QLabel = nullptr;
			QLabel* ObjectCylinder_Radius_QLabel = nullptr;
			QLabel* ObjectCylinder_HeightZ_QLabel = nullptr;
			QLabel* ObjectCylinder_Angle_QLabel = nullptr;
			QLabel* ObjectCylinder_Center_QLabel = nullptr;
			QLabel* ObjectCylinder_Rotate_QLabel = nullptr;
			QLabel* ObjectCylinder_Material_QLabel = nullptr;
			QLabel* ObjectCylinder_Density_QLabel = nullptr;
			QLabel* ObjectCylinder_Transparency_QLabel = nullptr;
			QLabel* ObjectCylinder_ChargedParticleTransport_QLabel = nullptr;
			QLineEdit* ObjectCylinder_Type_QLineEdit = nullptr;
			QLineEdit* ObjectCylinder_CenterX_QLineEdit = nullptr;
			QLineEdit* ObjectCylinder_CenterY_QLineEdit = nullptr;
			QLineEdit* ObjectCylinder_CenterZ_QLineEdit = nullptr;
			QLineEdit* ObjectCylinder_RotateX_QLineEdit = nullptr;
			QLineEdit* ObjectCylinder_RotateY_QLineEdit = nullptr;
			QLineEdit* ObjectCylinder_RotateZ_QLineEdit = nullptr;
			QLineEdit* ObjectCylinder_OuterRadius_QLineEdit = nullptr;
			QLineEdit* ObjectCylinder_InnerRadius_QLineEdit = nullptr;
			QLineEdit* ObjectCylinder_HeightZ_QLineEdit = nullptr;
			QLineEdit* ObjectCylinder_StartingAngle_QLineEdit = nullptr;
			QLineEdit* ObjectCylinder_DeltaAngle_QLineEdit = nullptr;
			QLineEdit* ObjectCylinder_Density_QLineEdit = nullptr;
			QPushButton* Object_CylinderMaterialEdit_QPushButton = nullptr;
			QPushButton* Object_CylinderUpdate_QPushButton = nullptr;
			QPushButton* Object_Cylinder_ChargedParticleTransport_QPushButton = nullptr;
			QSlider* Object_CylinderTransparency_QSlider = nullptr;
			QRadioButton* ObjectCylinder_MaterialMassFraction_QRadioButton = nullptr;
			QRadioButton* ObjectCylinder_MaterialAtomFraction_QRadioButton = nullptr;
			ObjectMaterialTextWindow* ObjectCylinder_MaterialPanelTextWindow_UserQDialog = nullptr;

		//============================================================================================================================//

	//-----------------------------------------------------------------------------------------------------------------//

////////////////// Source Widget ////////////////// 

// Global int	
	int CurrentSourceType = -1;	
	int List_sourceEP_MakingIndex = 0; // radionulcide에서 현재 입력한 핵종 개수	
	int List_sourceFD_MakingIndex = 0; // radionulcide에서 현재 입력한 핵종 개수	
	int List_sourceOV_MakingIndex = 0; // radionulcide에서 현재 입력한 핵종 개수
	int Object_sourceOV_SelectedIndex = -1;	
	/*변수이동필요*/int PhaseSpaceFileRow = 0; // Phase Space File의 입자 수(line 수)
	int RIList_sourceHP_MakingIndex = 0; // radionulcide에서 현재 입력한 핵종 개수
	int RIList_sourceCB_MakingIndex = 0; // radionulcide에서 현재 입력한 핵종 개수
	int RIList_sourceRC_MakingIndex = 0; // radionulcide에서 현재 입력한 핵종 개수		
		
	int EnergyspectrumListTotalCount_CB = 0; // radionulcide에서 현재 입력한 핵종 개수

// Global double
	/*변수이동필요*/double SelectedPointNormalVector_sourceHP[3] = { 0., 0., 0. }; // Hot particle 선택한 지점의 노말벡터
	/*변수이동필요*/double PSF_FileSize; // PSF의 파일 크기
	

// Global bool
	bool b_IsSourceBiasingListGenerating_sourceEP = false;
	std::map<int, bool> SourceBiasingCheckedPhantomList_sourceEP;		
	bool b_IsSourceLocationVisualized_sourceOV = true;
	bool b_IsSourceOV_AddingSettingClosed = false;
	bool b_IsSourceOV_AddingSettingOKClicked = true; 
	bool b_IsSourceLocationVisualized_sourceCB = true;
	bool b_IsSourceLocationVisualized_sourcePB = true;
	bool b_sourceOV_InObjectSelect = false;
	
// QGroupBox
	QGroupBox* SourcePanel_QGroupBox = nullptr;
	// Broad Beam (BB)
	QGroupBox* m_sourceGeometryBroadBeamGroupBox = nullptr;
	QGroupBox* m_AziPolGroupBox = nullptr;
	// External Point (EP)
	QGroupBox* m_sourceGeometryExternalPointGroupBox = nullptr;	
	QGroupBox* m_sourceExternal_AddManuallyGroupBox = nullptr;
	QGroupBox* m_sourceExternal_ExternalFileGroupBox = nullptr;
	// Floor Disk (FD)
	QGroupBox* m_sourceGeometryFloorDiskGroupBox = nullptr;
	// Object Volume (OV)
	QGroupBox* m_sourceGeometryObjectVolumeGroupBox = nullptr;
	// Phase Space (PS)
	QGroupBox* m_sourceGeometryPhaseSpaceGroupBox = nullptr;
	QGroupBox* m_sourceIAEAphspGroupBox = nullptr;
	QGroupBox* m_sourceUSERphspGroupBox = nullptr;
	// Hot Particle (HP)
	QGroupBox* m_sourceGeometryHotParticlePointGroupBox = nullptr;
	QGroupBox* m_sourceHPSingleGroupBox = nullptr;
	QGroupBox* m_sourceHPmultipleGroupBox = nullptr;
	// Cone Beam (CB)
	QGroupBox* m_sourceGeometryConeBeamGroupBox = nullptr;
	// Room Air Contamination (RC)
	QGroupBox* m_sourceGeometryRoomContaminationGroupBox = nullptr;
	// Environmental Source (ES)
	QGroupBox* m_sourceGeometryEnvironmentalSourceGroupBox = nullptr;
	// Parallel Beam (PB)
	QGroupBox* m_sourceGeometryParallelBeamGroupBox = nullptr;

// QLabel

	// External Point (EP)
	QLabel* RI_Title_sourceEP_QLabel = nullptr;
	QLabel* RI_Activity_sourceEP_QLabel = nullptr;
	QLabel* RI_List_sourceEP_QLabel = nullptr;
	QLabel* ES_List_sourceEP_QLabel = nullptr;
	QLabel* ES_Title_sourceEP_QLabel = nullptr;
	QLabel* ES_Intensity_sourceEP_QLabel = nullptr;
	QLabel* ES_FileName_sourceEP_QLabel = nullptr;
	// Floor Disk (FD)
	QLabel* RI_Title_sourceFD_QLabel = nullptr;
	QLabel* RI_Activity_sourceFD_QLabel = nullptr;
	QLabel* RI_List_sourceFD_QLabel = nullptr;	
	QLabel* ES_List_sourceFD_QLabel = nullptr;
	QLabel* ES_Title_sourceFD_QLabel = nullptr;
	QLabel* ES_Intensity_sourceFD_QLabel = nullptr;
	QLabel* ES_FileName_sourceFD_QLabel = nullptr;
	// Hot Particle (HP)
	QLabel* RI_Title_sourceHP_QLabel = nullptr;
	QLabel* RI_Activity_sourceHP_QLabel = nullptr;
	QLabel* RI_List_sourceHP_QLabel = nullptr;
	QLabel* ES_List_sourceHP_QLabel = nullptr;
	QLabel* ES_Title_sourceHP_QLabel = nullptr;
	QLabel* ES_Intensity_sourceHP_QLabel = nullptr;
	QLabel* ES_FileName_sourceHP_QLabel = nullptr;
	// Object Volume (OV)
	QLabel* RI_Title_sourceOV_QLabel = nullptr;
	QLabel* RI_Activity_sourceOV_QLabel = nullptr;
	QLabel* RI_List_sourceOV_QLabel = nullptr;
	QLabel* ES_List_sourceOV_QLabel = nullptr;
	QLabel* ES_Title_sourceOV_QLabel = nullptr;
	QLabel* ES_FileName_sourceOV_QLabel = nullptr;
	QLabel* ES_Intensity_sourceOV_QLabel = nullptr;	
	// Cone Beam (CB)
	QLabel* labelConeBeamRadionuclideTitle = nullptr;
	QLabel* labelConeBeamRadionuclideActivity = nullptr;
	QLabel* labelConeBeamRadionuclideList = nullptr;
	QLabel* labelConeBeamEnergyspectrumTitle = nullptr;
	QLabel* labelConeBeamEnergyspectrumIntensity = nullptr;
	QLabel* labelConeBeamEnergyspectrumFileName = nullptr;
	QLabel* labelConeBeamEnergyspectrumList = nullptr;
	// Room Air Contamination (RC)
	QLabel* label_sourceRC_BoxLength = nullptr;
	QLabel* label_sourceRC_BoxCenter = nullptr;
	QLabel* RadionuclideList_sourceRC_QLabel = nullptr;
	// Environmental Source (ES)
	QLabel* labelEnvironmentalRadionuclideTitle = nullptr;
	QLabel* labelEnvironmentalRadionuclideActivity = nullptr;
	QLabel* labelEnvironmentalRadionuclideList = nullptr;
	QLabel* labelEnvironmentalSoilDepth = nullptr;
	// Parallel Beam (PB)
	QLabel* labelParallelBeamParticleTypeTitle = nullptr;		
	QLabel* labelParallelBeamEnergyTitle = nullptr;
	QLabel* labelParallelBeamIntensityTitle = nullptr;
	QLabel* labelParallelBeamEnergyspectrumTitle = nullptr;
	QLabel* labelParallelBeamEnergyspectrumIntensity = nullptr;
	QLabel* labelParallelBeamEnergyspectrumFileName = nullptr;
	QLabel* labelParallelBeamEnergyspectrumList = nullptr;

// QLineEdit
	// B
	QLineEdit* m_lineEditAzimuthalAngle = nullptr;
	QLineEdit* m_lineEditPolarAngle = nullptr;
	QLineEdit* m_lineBroadBeamEnergy = nullptr;

	//EP
	QLineEdit* RI_Radionuclide_sourceEP_QLineEdit = nullptr;
	QLineEdit* RI_Activity_sourceEP_QLineEdit = nullptr;
	QLineEdit* PosX_sourceEP_QLineEdit = nullptr;
	QLineEdit* PosY_sourceEP_QLineEdit = nullptr;
	QLineEdit* PosZ_sourceEP_QLineEdit = nullptr;
	QLineEdit* ES_Intensity_sourceEP_QLineEdit = nullptr;	

	// FD
	QLineEdit* PosX_SourceFD_QLineEdit = nullptr;
	QLineEdit* PosY_SourceFD_QLineEdit = nullptr;
	QLineEdit* PosZ_SourceFD_QLineEdit = nullptr;
	QLineEdit* Radius_sourceFD_QLineEdit = nullptr;
	QLineEdit* RI_Radionuclide_sourceFD_QLineEdit = nullptr;
	QLineEdit* RI_Activity_sourceFD_QLineEdit = nullptr;
	QLineEdit* ES_Intensity_sourceFD_QLineEdit = nullptr;

	// OV
	QLineEdit* RI_Radionuclide_sourceOV_QLineEdit = nullptr;
	QLineEdit* RI_Activity_sourceOV_QLineEdit = nullptr;
	QLineEdit* ES_Intensity_sourceOV_QLineEdit = nullptr;

	// PS

	// HP
	QLineEdit* PosX_SourceHP_QLineEdit = nullptr;
	QLineEdit* PosY_SourceHP_QLineEdit = nullptr;
	QLineEdit* PosZ_SourceHP_QLineEdit = nullptr;
	QLineEdit* SeparationDist_SourceHP_QLineEdit = nullptr;
	QLineEdit* RI_Radionuclide_sourceHP_QLineEdit = nullptr;
	QLineEdit* RI_Activity_sourceHP_QLineEdit = nullptr;
	QLineEdit* ES_Intensity_sourceHP_QLineEdit = nullptr;

	// CB
	QLineEdit* m_lineEditConeBeamPointX = nullptr;
	QLineEdit* m_lineEditConeBeamPointY = nullptr;
	QLineEdit* m_lineEditConeBeamPointZ = nullptr;
	QLineEdit* m_lineEditConeBeamDirectionX = nullptr;
	QLineEdit* m_lineEditConeBeamDirectionY = nullptr;
	QLineEdit* m_lineEditConeBeamDirectionZ = nullptr;
	QLineEdit* m_lineEditConeBeamDirectionSolidAngle = nullptr;
	QLineEdit* m_lineEditConeBeamRadionuclide = nullptr;
	QLineEdit* m_lineEditConeBeamActivity = nullptr;
	QLineEdit* m_lineEditConeBeamIntensity = nullptr;

	// RC
	QLineEdit* Box_HalfLengthX_sourceRC_QLineEdit = nullptr;
	QLineEdit* Box_HalfLengthY_sourceRC_QLineEdit = nullptr;
	QLineEdit* Box_HalfLengthZ_sourceRC_QLineEdit = nullptr;
	QLineEdit* Box_CenterX_sourceRC_QLineEdit = nullptr;
	QLineEdit* Box_CenterY_sourceRC_QLineEdit = nullptr;
	QLineEdit* Box_CenterZ_sourceRC_QLineEdit = nullptr;
	QLineEdit* m_lineEditRCHemiRadius = nullptr;
	QLineEdit* m_lineEditRCHemiCenterX = nullptr;
	QLineEdit* m_lineEditRCHemiCenterY = nullptr;
	QLineEdit* m_lineEditRCHemiCenterZ = nullptr;
	QLineEdit* RI_sourceRC_QLineEdit = nullptr;
	QLineEdit* Activity_sourceRC_QLineEdit = nullptr;

	// ES
	QLineEdit* m_lineEditEnvironmentalSoilDepth = nullptr;
	QLineEdit* m_lineEditEnvironmentalRadionuclide = nullptr;
	QLineEdit* m_lineEditEnvironmentalActivity = nullptr;

	// PB
	QLineEdit* m_lineEditParallelBeamPointX = nullptr;
	QLineEdit* m_lineEditParallelBeamPointY = nullptr;
	QLineEdit* m_lineEditParallelBeamPointZ = nullptr;
	QLineEdit* m_lineEditParallelBeamRadius = nullptr;
	QLineEdit* m_lineEditParallelBeamDirectionTheta = nullptr;
	QLineEdit* m_lineEditParallelBeamDirectionPhi = nullptr;		
	QLineEdit* m_lineEditParallelBeamEnergy = nullptr;
	QLineEdit* m_lineEditParallelBeamIntensity_MonoEnergy = nullptr;
	QLineEdit* m_lineEditParallelBeamIntensity_EnergySpectrum = nullptr;

// QPushButton
	QPushButton* m_sourceOKandResetButton = nullptr;
	//
	QPushButton* m_sourceBresetButton = nullptr;
	QPushButton* m_BroadBeamSourceVisualizationButton = nullptr;

	//EP
	QPushButton* m_sourceEPresetButton = nullptr;
	QPushButton* SourceEP_SphereSize_Minus_QPushButton = nullptr;
	QPushButton* SourceEP_SphereSize_Plus_QPushButton = nullptr;
	QPushButton* m_ExternalPointSourceVisualizationButton = nullptr;
	QPushButton* m_sourceExternal_AddManuallyButton = nullptr;
	QPushButton* ExternalSingleEnergyspectrumFileLoadButton = nullptr;	
	QPushButton* sourceEP_UpdatePositionButton = nullptr;
	QPushButton* pickExternal3DButton = nullptr;	
	QPushButton* ExternalSingleRadionuclideButton = nullptr;
	QPushButton* ExternalSingleRadionuclideAddButton = nullptr;
	QPushButton* ExternalSingleEnergySpectrumAddButton = nullptr;	
	QPushButton* m_sourceExternal_ExternalFileButton = nullptr;
	QPushButton* ExternalFileEP_PositionTranslateButton = nullptr;
	QPushButton* ExternalSingleSourceBiasingOnOffButton = nullptr;
	QPushButton* loadFileButton_ExternalFileEP = nullptr;
	std::vector<QPushButton*> RI_ListDelete_sourceEP_QButton;
	std::vector<QPushButton*> RI_ListInfo_sourceEP_QButton;
	std::vector<QPushButton*> ES_ListDelete_sourceEP_QButton;
	std::vector<QPushButton*> ES_ListInfo_sourceEP_QButton;

	//FD
	QPushButton* m_sourceFDresetButton = nullptr;
	QPushButton* m_FloorSourceVisualizationButton = nullptr;
	QPushButton* m_FloorSetDefaultGeom = nullptr;
	QPushButton* m_FloorZpos3DPickButton = nullptr;
	QPushButton* m_FloorUpdateButton = nullptr;	
	QPushButton* sourceFD_RadionuclideAddButton = nullptr;
	QPushButton* sourceFD_EnergyspectrumFileLoadButton = nullptr;
	QPushButton* sourceFD_EnergySpectrumAddButton = nullptr;
	std::vector<QPushButton*> RI_ListDelete_sourceFD_QButton;
	std::vector<QPushButton*> RI_ListInfo_sourceFD_QButton;
	std::vector<QPushButton*> ES_ListDelete_sourceFD_QButton;
	std::vector<QPushButton*> ES_ListInfo_sourceFD_QButton;

	//	OV
	std::map<int, QPushButton*> sourceOV_ObjectButton;
	QPushButton* m_sourceOV_SettingOKButton = nullptr;
	QPushButton* m_sourceOVresetButton = nullptr;
	QPushButton* m_ObjectVolumeSourceVisualizationButton = nullptr;
	QPushButton* sourceOV_ObjectAddButton = nullptr;
	QPushButton* sourceOV_ObjectDeleteButton = nullptr;
	QPushButton* sourceOV_EnergyspectrumFileLoadButton = nullptr;
	QPushButton* sourceOV_EnergyspectrumAddButton = nullptr;
	QPushButton* sourceOV_RadionuclideAddButton = nullptr;
	std::vector<QPushButton*> RI_ListDelete_sourceOV_QButton;
	std::vector<QPushButton*> RI_ListInfo_sourceOV_QButton;
	std::vector<QPushButton*> ES_ListDelete_sourceOV_QButton;
	std::vector<QPushButton*> ES_ListInfo_sourceOV_QButton;

	//PS
	QPushButton* m_sourcePSresetButton = nullptr;
	QPushButton* SourceVisualziation_sourePS_QButton = nullptr;	
	QPushButton* sourcePS_MCNP_button = nullptr;
	QPushButton* sourcePS_PHITS_button = nullptr;
	QPushButton* sourcePS_FLUKA_button = nullptr;
	QPushButton* sourcePS_IAEA_button = nullptr;
	QPushButton* sourcePS_USER_Button = nullptr;

	//	HP
	QPushButton* SourceHP_SphereSize_Plus_QPushButton = nullptr;
	QPushButton* SourceHP_SphereSize_Minus_QPushButton = nullptr;
	QPushButton* m_HotParticleSourceVisualizationButton = nullptr;
	QPushButton* sourceHP_EnergyspectrumFileLoadButton = nullptr;	
	QPushButton* Pick3D_HP_QButton = nullptr;
	QPushButton* UpdatePosition_sourceHP_QButton = nullptr;
	QPushButton* RIList_Add_sourceHP_QButton = nullptr;
	std::vector<QPushButton*> RIList_Delete_sourceHP_QButton;
	std::vector<QPushButton*> RIList_Info_sourceHP_QButton;
	QPushButton* ESList_Add_sourceHP_QButton = nullptr;
	std::vector<QPushButton*> ESList_Delete_sourceHP_QButton;
	std::vector<QPushButton*> ESList_Info_sourceHP_QButton;

	//CB
	QPushButton* m_sourceCBresetButton = nullptr;
	QPushButton* m_ConeBeamSourceVisualizationButton = nullptr;
	QPushButton* m_ConeBeamDirectionVisualizationButton = nullptr;
	QPushButton* pickConeBeam3DButton = nullptr;
	QPushButton* sourceCB_UpdateButton = nullptr;
	QPushButton* ConeBeamDirectionUpdateButton = nullptr;
	QPushButton* ConeBeamRadionuclideButton = nullptr;
	QPushButton* ConeBeamRadionuclideAddButton = nullptr;
	QPushButton* ConeBeamEnergyspectrumButton = nullptr;
	QPushButton* ConeBeamEnergyspectrumFileLoadButton = nullptr;
	QPushButton* ConeBeamEnergyspectrumAddButton = nullptr;
	std::vector<QPushButton*> m_sourceCB_listDeleteButton;
	std::vector<QPushButton*> m_sourceCB_listInfoButton;

	//PS
	QPushButton* loadFileButton_IAEAphsp = nullptr;
	QPushButton* sourcePS_loadPSFButton = nullptr;

	// RC
	QPushButton* UpdateRCposButton = nullptr;
	QPushButton* m_sourceRCresetButton = nullptr;
	QPushButton* m_RoomContaminationSourceVisualizationButton = nullptr;
	QPushButton* RIList_Add_sourceRC_QButton = nullptr;
	std::vector<QPushButton*> RIList_Delete_sourceRC_QButton;
	std::vector<QPushButton*> RIList_Info_sourceRC_QButton;

	// E
	QPushButton* m_sourceEresetButton = nullptr;
	QPushButton* m_EnvironmentalSourceVisualizationButton = nullptr;	
	QPushButton* EnvironmentalRadionuclideAddButton = nullptr;

	//PB
	QPushButton* m_ParallelBeamSourceVisualizationButton = nullptr;
	QPushButton* pickParallelBeam3DButton = nullptr;
	QPushButton* sourcePB_UpdateButton = nullptr;
	QPushButton* ParallelBeamRadionuclideAddButton = nullptr;
	QPushButton* ParallelBeamEnergyspectrumFileLoadButton = nullptr;
	QPushButton* ParallelBeamEnergyspectrumAddButton = nullptr;

// QComboBox
	QComboBox* m_comboBoxSourceGeometry = nullptr;
	QComboBox* m_comboBoxBeamdirection = nullptr;
	QComboBox* m_comboBoxBroadBeamParticleType = nullptr;
	QComboBox* m_comoboBoxHPphantoms = nullptr;
	QComboBox* m_comboBoxMultipleHP = nullptr;
	QComboBox* m_comboBoxEnvironmentalSourceType = nullptr;
	QComboBox* m_comboBoxSetting_sourceOV_objectList = nullptr;
	QComboBox* m_comboBoxParallelBeamParticleType = nullptr;
// QTextEdit
	QTextEdit* sourceEP_EnergyspectrumFileNameText = nullptr;
	//
	QTextEdit* sourceFD_EnergyspectrumFileNameText = nullptr;
	//
	QTextEdit* sourceOV_EnergyspectrumFileNameText = nullptr;
	//
	QTextEdit* sourcePS_PSFname = nullptr;
	//
	QTextEdit* sourceHP_EnergyspectrumFileNameText = nullptr;
	//
	QTextEdit* sourceCB_EnergyspectrumFileNameText = nullptr;
	//
	QTextEdit* sourcePB_EnergyspectrumFileNameText = nullptr;
// QRadioButton
	QRadioButton* RI_Select_sourceEP_QRadioButton = nullptr;
	QRadioButton* ES_Select_sourceEP_QRadioButton = nullptr;
	QRadioButton* RI_Select_sourceFD_QRadioButton = nullptr;
	QRadioButton* ES_Select_sourceFD_QRadioButton = nullptr;
	QRadioButton* RI_Select_sourceRC_QRadioButton = nullptr;
	QRadioButton* ES_Select_sourceRC_QRadioButton = nullptr;
	QRadioButton* RI_Select_sourceHP_QRadioButton = nullptr;
	QRadioButton* ES_Select_sourceHP_QRadioButton = nullptr;
	QRadioButton* sourceRC_Box_radioButton = nullptr;
	QRadioButton* sourceRC_Hemisphere_radioButton = nullptr;
	QRadioButton* sourceOV_Radionuclide_radioButton = nullptr;
	QRadioButton* sourceOV_Energyspectrum_radioButton = nullptr;
	QRadioButton* sourceCB_Radionuclide_radioButton = nullptr;
	QRadioButton* sourceCB_Energyspectrum_radioButton = nullptr;	
	QRadioButton* sourcePB_MonoEnergy_radioButton = nullptr;
	QRadioButton* sourcePB_EnergySpectrum_radioButton = nullptr;
	
// QScrollArea
	QScrollArea* scrollAreaTargetERR_EP = new QScrollArea();
	QScrollArea* scrollAreaTargetERR_HP = new QScrollArea();
// QListWidget
	QListWidget *RI_List_sourceEP_QListWidget = nullptr;
	QListWidget *ES_List_sourceEP_QListWidget = nullptr;

	QListWidget *RI_List_sourceFD_QListWidget = nullptr;
	QListWidget *ES_List_sourceFD_QListWidget = nullptr;
	
	QListWidget *RI_List_sourceHP_QListWidget = nullptr;
	QListWidget *ES_List_sourceHP_QListWidget = nullptr;

	QListWidget *RI_List_sourceRC_QListWidget = nullptr;
	
	QListWidget *RI_List_sourceOV_QListWidget = nullptr;	
	QListWidget *ES_List_sourceOV_QListWidget = nullptr;

	QListWidget *RI_List_sourceCB_QListWidget = nullptr;
	QListWidget *ES_List_sourceCB_QListWidget = nullptr;

	QListWidget *RI_List_sourcePB_QListWidget = nullptr;
	QListWidget *ES_List_sourcePB_QListWidget = nullptr;

	QListWidget *listWidget_E_Radionuclide = nullptr;

// QCheckBox	
	QCheckBox* DirectionCheck_sourcePS_QCheckBox = nullptr;
	QCheckBox* sourceCB_dirCheckBox = nullptr;
// QHBoxLayout
	QHBoxLayout* layout_sourceOV_ObjectList = nullptr;
// QScrollBar
	QScrollBar*  sourceOV_ObjectScrollBar = nullptr;
// 
	MultipleUIDialog* sourceOV_ObjectAddingDialog = nullptr;
// QStringList
	QStringList RIsourceList; // Completer(자동완성)
//////////////////////////////////////////////////////

////////////////// Output Widget ////////////////// 

// Global int	
	int OutputPhantomSelectedIndex = 0; // Output 패널 상에서 선택된 팬텀 인덱스

// QGroupBox
	QGroupBox* OutputPanel_QGroupBox = nullptr;
	QGroupBox* OutputPanel_OutputPhantomButtons_QGroupBox = nullptr;
	QGroupBox* OutputPanel_Results_QGroupBox = nullptr;
	QGroupBox* InnerResultPanel_Dose_QGroupBox = nullptr;
	QGroupBox* InnerResultPanel_Dosimeter_QGroupBox = nullptr;
	QGroupBox* InnerResultPanel_H_AK_QGroupBox = nullptr;
	QGroupBox* InnerResultPanel_LocalSkin_QGroupBox = nullptr;
// QLabel	
	std::map<int, QLabel*> m_outputPersonalLabelEP;	
// QLineEdit
	std::map<QString, QLineEdit*> OutputPanel_DoseResult_QLineEdit;
	std::map<QString, QLineEdit*> m_outputFluenceB_tmp;
	std::map<QString, QLineEdit*> m_outputAirKermaB;
	std::map<int, QLineEdit*> OutputPanel_DosimeterResult_QLineEdit;
	QLineEdit* m_outputEffDoseforAK_B = nullptr;
	//
	std::map<QString, QLineEdit*> m_outputDoseRateEP;
	std::map<QString, QLineEdit*> m_outputAirKermaEP;
	std::map<int, QLineEdit*> m_outputPersonalEP;
	QLineEdit* m_outputEffDoseforAK_EP = nullptr;
	//	
	std::map<QString, QLineEdit*> m_outputDoseRateFD;
	std::map<QString, QLineEdit*> m_outputAirKermaFD;
	std::map<int, QLineEdit*> m_outputPersonalFD;
	QLineEdit* m_outputEffDoseforAK_FD = nullptr;
	//
	std::map<QString, QLineEdit*> m_outputDoseRateOV;
	std::map<QString, QLineEdit*> m_outputAirKermaOV;
	std::map<int, QLineEdit*> m_outputPersonalOV;
	QLineEdit* m_outputEffDoseforAK_OV = nullptr;
	//
	std::map<QString, QLineEdit*> m_outputDosePS;
	std::map<QString, QLineEdit*> m_outputAirKermaPS;
	std::map<int, QLineEdit*> m_outputPersonalPS;
	QLineEdit* m_outputEffDoseforAK_PS = nullptr;
	//
	std::map<QString, QLineEdit*> m_outputDoseRateHP;
	std::map<QString, QLineEdit*> m_outputAirKermaHP;
	std::map<int, QLineEdit*> m_outputPersonalHP;
	QLineEdit* m_outputEffDoseforAK_HP = nullptr;
	//
	std::map<QString, QLineEdit*> m_outputDoseRC;	
	std::map<QString, QLineEdit*> m_outputAirKermaRC;
	std::map<int, QLineEdit*> m_outputPersonalRC;
	QLineEdit* m_outputEffDoseforAK_RC = nullptr;
// QPushButton
	std::map<int, QPushButton*> OutputPhantomButton;
	QPushButton* m_skinDoseVisualizationButton = nullptr;
// QComboBox	
	QComboBox* m_comboBox_resultType = nullptr;
	QComboBox* m_comboOutputBoxExternalPoint = nullptr;
	QComboBox* m_comboOutputBoxHotParticle = nullptr;
	QComboBox* m_comboOutputBoxFloorDisk = nullptr;
	QComboBox* m_comboOutputBoxObjectVolume = nullptr;
	QComboBox* m_comboOutputBoxPhaseSpace = nullptr;
// QHBoxLayout
	QHBoxLayout* layoutOutputPhantomButtonList = nullptr;
//////////////////////////////////////////////////////

////////////////// Calculation Setting Dialog ////////////////// 
	std::map<QString, QLineEdit*> CalculationSetting_TargetError_QLineEdit;
	QLineEdit* CalculationSetting_TargetNPS_QLineEdit;
	QLineEdit* CalculationSetting_ResultFileName_QLineEdit;
	QLineEdit* CalculationSetting_Thread_QLineEdit;
	QRadioButton* m_RadioButton_SkinDepthDoseOff = nullptr;
	QRadioButton* m_RadioButton_SkinDepthDoseOn = nullptr;
	QRadioButton* m_radioVisualizationModeOff = nullptr;
	QRadioButton* m_radioVisualizationModeOn = nullptr;
	QPushButton* m_settingSaveButton = nullptr;
	std::map<int, QString> m_CalculationSetting_TargetError;
	QString m_CalculationSetting_TargetNPS = "";
	QString m_CalculationSetting_ResultFileName = "";
	QString m_CalculationSetting_Thread = "";
	int SettingVisualizationModeInfo = 0; // Default: Off
	int SettingSkinDepthModeInfo = 0; // Default: Off

//////////////////////////////////////////////////////

////////////////// Calculation Widget ////////////////// 

// Global int	
	int Time_Interval = 4.9; // Calculation 시 UI update 주기 (s)
	int RunningIndex = 0; // UI가 돌아가고 있는 지 누적해서 더해가는 변수
	/*변수이동필요*/FILETIME OrgandoseFile_PRE_ModifiedTime = { 0,0 };
	/*변수이동필요*/FILETIME SkindoseFile_PRE_ModifiedTime = { 0,0 };

// Global Qstring
	QString DoseUnit_QString; // 피부선량 환산 unit
	/*변수이동필요*/QString resultDirectoryQstr; // DB에서 받아오는 result 폴더명

// Global bool	
	bool b_IsTargetErrorAbsent = false; // 지정한 Target error가 없으면 true
	bool b_IsTargetNPSAbsent = false; // 지정한 Target nps가 없으면 true
	bool b_IsCalculationExecuted = false; // 계산버튼을 눌러서 실행된 상태
	bool b_IsRealtimeCalculation = false; // 실시간계산중이면 true -> realtime panel show/hide 하기 위함
	bool b_IsOutputResultLoaded = false; // Output이 최초로 한 번이라도 load 되었으면 true -> 아웃풋패널 클릭 혹은 팬텀버튼 클릭 시 결과 업데이트하기 위함

// Global double
	double ProgressBarValue = 0; // ProgressBar의 진행률 값
	double ProgressFactor; // 지정한 업데이트 간격당 진행률 증가정도
	std::uint64_t nEvent = 0; // 현재까지 Geant4에서 수송된 NPS 값	

// Global string
	/*변수이동필요*/std::string timeStamp;

	// For server
	std::string PreCumulative = "NULL"; // 이전의 Cumulative 선량결과 텍스트
	std::string ResultFileName_forServer; // 현재 시간이 기록된 result 텍스트 파일(Simul_info 뒤에 덧붙여짐)	
	std::string ResultFileName_notused; // McSEE 결과파일 이름
			
// QLineEdit	
	QLineEdit* m_Output_H_AK_AmbientDoseEquivaelnt = nullptr;
	QLineEdit* m_Output_H_AK_AirKerma = nullptr;
	QLineEdit* m_Output_H_AK_posX = nullptr;
	QLineEdit* m_Output_H_AK_posY= nullptr;
	QLineEdit* m_Output_H_AK_posZ = nullptr;
	QLineEdit* m_Output_H_AK_radius = nullptr;
	QLineEdit* m_lineEditResultFileName = nullptr;
// QPushButton
	QPushButton* m_CalculateButton = nullptr;
// QVBoxLayout
	QVBoxLayout* OrganDose_BarGraph_QVBoxLayout = nullptr;
	QVBoxLayout* DosimeterDose_BarGraph_QVBoxLayout = nullptr;
	QVBoxLayout* H_AK_BarGraph_QVBoxLayout = nullptr;
	QVBoxLayout* SkinDepthDose_LineGraph_QVBoxLayout = nullptr;
	//
	QVBoxLayout* layoutInnerIn_graphEP = nullptr;
	QVBoxLayout* layoutInnerIn_graphHP = nullptr;
	QVBoxLayout* layoutInnerIn_graphFD = nullptr;
	QVBoxLayout* layoutInnerIn_graphOV = nullptr;
	QVBoxLayout* layoutInnerIn_graphPS = nullptr;
	QVBoxLayout* layoutInnerIn_graphRC = nullptr;
//
	DBServerConnect* serverConnect = nullptr;
	QCustomPlot* OrganDose_BarGraph_QCustomPlot = nullptr;
	QCustomPlot* Dosimeter_BarGraph_QCustomPlot = nullptr;
	QCustomPlot* H_AK_BarGraph_QCustomPlot = nullptr;
	std::map<int, QCustomPlot*> SkinDepthDose_LineGraph_QCustomPlot;
	
//////////////////////////////////////////////////////
	
////////////////// Information Container ////////////////// 
	
// Source
	std::vector<int> m_sourceOV_objectSequenceVector; // m_sourceOV_objectSequenceVector[ID] = Object ID
	std::map<int, std::map<int, double>> sourceOV_objectBound; // sourceOV_objectBound[ID][0~5] = 0: Xmin, 1: Xmax, 2: Ymin, 3: Ymax, 4: Zmin, 5: Zmax
	std::vector<std::map<int, QString>> ListInfo_sourceEP; // ListInfo_sourceEP[pointID][0~5] = 0: 사용여부(True or ""), 1: posX, 2: posY, 3: posZ, 4: RI, 5: activity(Bq)		
	std::vector<std::map<int, QString>> sourceCB_info; // ListInfo_sourceEP[pointID][0~5] = 0: 사용여부(True or ""), 1: posX, 2: posY, 3: posZ, 4: RI, 5: activity(Bq)
	std::vector<std::map<int, QString>> ListInfo_sourceFD; // RI_Info_sourceRC[pointID][0~2] = 0: 사용여부(True or ""), 1: RI, 2: activity(Bq/cm3)
	std::vector<std::map<int, QString>> RI_Info_sourceRC; // RI_Info_sourceRC[pointID][0~2] = 0: 사용여부(True or ""), 1: RI, 2: activity(Bq/cm3)
	std::vector<std::map<int, QString>> ListInfo_sourceHP; // RI_Info_sourceRC[pointID][0~2] = 0: 사용여부(True or ""), 1: RI, 2: activity(Bq/cm3)
	std::map<int, std::vector<std::map<int, QString>>> ListInfo_sourceOV; // RI_Info_sourceOV[objectID][RIindex][1~2] = 0: 사용여부, 1: RI, 2: activity(Bq/cm3)
	std::vector<std::map<int, QString>> MultipleHPinfoVector; // MultipleHPinfoVector[point#] = 0: posX, 1: posY, 2: posZ, 3: sepaDist, 4: RI, 5: activity(Bq)	
//////////////////////////////////////////////////////

// Calculation	
	std::map<int, std::map<int, std::pair<QString, double>>> m_graphInfo_OrganDose_map; //m_graphInfo_time_dose_map[phantomID][organ/dosimeter ID] = {OrganNameList, Now dose}
	std::map<int, std::map<int, std::vector<std::pair<double, double>>>> m_graphInfo_TimeDose_map; //m_graphInfo_time_dose_map[phantomID][organ/dosimeter ID][vectorIdx] = {time, dose}
	std::map<int, std::map<int, double>> m_graphInfo_Error_map; //m_graphInfo_Error_map[phantomID][organ/dosimeter ID] = Now error	
	std::map<std::string, double> TargetOrganError_Map;
	QString SkinDose_BinaryFilePath;

//Reconstruction
	bool b_Is_Reconstruction = false;
	void ClosePhantomUIdialog_Recon();
	void CloseClothingUIdialog_Recon();
	void CloseObjectUIdialog_Recon();

//************************************** Common Util **************************************//
public:
	void ExecuteAllButttonClicked();

//************************************** Create MainWidget **************************************//
protected:
	void CreateWidgets();

//************************************** CreateWidget **************************************//
public:
	void CreatePhantomWidgets();

	void Create_Phantom_ListWidgets(QVBoxLayout* layout);	

	QString DefinePreWearablePath(int wearableIndex); // Pre-defined 의복을 입력하는 과정에서 사전제작 의복 obj 파일 경로를 받아오는 함수
	
	void CreateGeometryWidgets();
	void CreateObjectBoxWidgets(QVBoxLayout* layout);
	void CreateObjectSphereWidgets(QVBoxLayout* layout);
	void CreateObjectCylinderWidgets(QVBoxLayout* layout);	
	
	void CreateSourceWidgets();

	void CreateSourceGeometryBroadBeamWidgets(QVBoxLayout* layout);
	void CreateAziPol(int idx, QVBoxLayout* layout);
	void CreateSourceGeometryExternalPointWidgets(QVBoxLayout* layout);
	void CreateSourceGeometryHotParticlePointWidgets(QVBoxLayout* layout);
	void CreateSourceGeometryConeBeamWidgets(QVBoxLayout* layout);
	void CreateSourceGeometryFloorDiskWidgets(QVBoxLayout* layout);
	void CreateSourceGeometryObjectVolumeWidgets(QVBoxLayout* layout);
	void CreateSourceGeometryPhaseSpaceWidgets(QVBoxLayout* layout);
	void CreateSourceGeometryRoomContaminationWidgets(QVBoxLayout* layout);
	void CreateSourceGeometryEnviromentalSourceWidgets(QVBoxLayout* layout);
	void CreateSourceGeometryParallelBeamWidgets(QVBoxLayout* layout);
	void ExecuteSourceOVAddRoutines();

	void CreateOutputWidgets();
	void CreateOutputPhantomButtonWidget(QVBoxLayout* layout);
	void CreateOutputResultWidget(QVBoxLayout* layout);

	void CreateOutput_Dose(QVBoxLayout* layout);
	void CreateOutput_Dosimeter(QVBoxLayout* layout);
	void CreateOutput_H_AK(QVBoxLayout* layout);
	void CreateOutput_LocalSkin(QVBoxLayout* layout);
	void Create_Dose_BarGraphWidgets(QVBoxLayout* layoutIn_graph); // Output 패널 상에 장기별 선량 막대그래프 초기화
	void Create_Dosimeter_BarGraphWidgets(QVBoxLayout* layoutIn_graph); // Output 패널 상에 선량계 막대그래프 초기화
	void Create_H_AK_BarGraphWidgets(QVBoxLayout* layoutIn_graph); // Output 패널 상에 에어커마/주위선량당량 막대그래프 초기화
	void Create_Skindose_LineGraphWidgets(QVBoxLayout* layout); // Output 패널 상에 Hot particle 피부깊이선량분포 꺾은선그래프 초기화
	

//************************************** Functions **************************************//
public:
	void AllButtonEnabled();
	void AllButtonDisabled(); // 가시화창과 마우스 상호작용 시, 해당 작업이 끝날 때 까지 UI 내의 버튼들 비활성화

	void SetGeometryPanelMode();
	void SetPhantomPanelMode();
	void SetOutputPanelMode();
	void SetSourcePanelMode();	

	void SavePhantom_InfoData_InAddingSetting(int phantomIndex); // AddingSetting창의 데이터를 InfoData 컨테이너에 업데이트
	void SavePhantom_InfoData(int phantomIndex); // InfoStatus의 데이터를 InfoData 컨테이너에 업데이트
	void SavePhantom_InfoData_InMouseControl(int phantomIndex);
	void UpdatePhantom_ListButton(int phantomIndex); // 현재 선택 객체 인덱스를 기반으로 List 버튼 업데이트
	void UpdatePhantom_InfoStatus(int phantomIndex); // InfoData의 데이터를 Info Status에 업데이트
	void PhantomAddingSetting_Create();
	void ExecutePhantomAddRoutines();

	void InitializeClothingPanel_InClothingAdd(); // 최초 의복 입력 시, 의복 제작할 때 clothing panel의 기본구조를 초기화
	void SaveClothingInformation_InClothingGenerate(); // 의복 제작 단계에서, 의복정보 컨테이너에 clothing setting창에서 입력한 정보를 저장
	void SaveClothingInformation_InLayerGenerate(); // 의복 레이어를 추가하는 단계에서, 의복정보 컨테이너에 clothing layer setting창에서 입력한 정보를 저장	
	void SetClothingPanelInfo(int clothingIndex, int clothingLayerIndex); // 팬텀, 의복 추가, 제거, 선택하는 과정 전반에서 clothing 패널 정보를 업데이트

	void RemoveClothingPanel();
	void SetClothingPanel_inPhantomDelete();
	void SetClothingPanel_inPhantomSelect();	

	void EyewearAddingSetting();

	void SaveObject_InfoData_InAddingSetting(int objectIndex); // Adding Setting의 데이터를 InfoData로 저장	
	void SaveObject_InfoData(int objectIndex); // Info Status의 데이터를 InfoData로 저장
	void SaveObject_InfoData_InMouseControl(int objectIndex); // 움직인 actor의 데이터를 InfoData로 저장
	void UpdateObject_InfoStatus(int objectIndex); // InfoData의 데이터를 Info Status에 업데이트
	void SetInitialObjectMode();
	void SetBoxObjectMode();
	void SetSphereObjectMode();
	void SetCylinderObjectMode();
	void ObjectAddingSetting_Create();
	void ExecuteObjectAddRoutines();
		
	void UpdateObject_Mode(int objectIndex);
	void UpdateObject_ListButton(int objectIndex);
	
	void ClothingAddingSetting(); // Phantom 패널 내 Clothing Add 버튼 클릭 시 Clothing Setting 창을 띄우는 함수
	void ClothingLayerAddingSetting(); // Phantom 패널 내 Clothing Layer Add 버튼 클릭 시 Clothing Layer Setting 창을 띄우는 함수

	void SetSourceGeometryType(int index);
	void SetBroadBeamAziPol(int user);	

	void SetResultType(int user);
	
	void sourceOV_ObjectAddingSetting_Create();

	void DoseCalculation_StopAndSave(); // 가시화창 우측 하단 Stop 버튼 클릭 시 실행
	void triggerCalculate(); // 가시화창 우측 하단 실행 버튼 클릭 시 실행
	void triggerSetting(); // 가시화창 우측 하단 설정 버튼 클릭 시 실행	
	void Update_SkinDose_1_10cm2_GraphWidgets();
	void Update_SkinDose_HP_LineGraphWidgets(); // checkProcessInfo_Local로 선량계산결과 주기적으로 업데이트 중, Hot particle 선원 피부선량깊이분포 꺾은선그래프 결과 업데이트

	void handleProcessError(const QString& errorMessage);
	std::vector<std::pair<double, double>> performAdaptiveDownsampling(const std::vector<std::pair<double, double>>& originalData, int targetSize);

//************************************** Slots **************************************//
public slots:	
	void slot_allButtonClicked();

	void slot_Phantom_ButtonClicked();
	void slot_Geometry_ButtonClicked();
	void slot_Source_ButtonClicked();
	void slot_Output_ButtonClicked();
	void slot_PhantomUpdate_ButtonClicked();
	
	void slot_PhantomSetting_ComputationlPhantom_RadioButtonClicked();
	void slot_PhantomSetting_AirPhantom_RadioButtonClicked();
	void slot_PhantomSetting_Import_ButtonClicked();
	void slot_ChangePhantomTypeCombo_Setting(int idx);
	void slot_ChangePhantomDummyCombo_Setting(int idx);
	void slot_ChangePhantomGenderCombo_Setting(int idx);
	void slot_ChangePhantomAgeCombo_Setting(int idx);
	void slot_ChangePhantomPostureCombo_Setting(int idx);
	void slot_ChangePhantomFetalAgeCombo_Setting(int idx);
	void slot_PhantomAddingOK_ButtonClicked();

	void slot_Glasses3DPick_ButtonClicked();
	void slot_GlassesUpdate_ButtonClicked();
	void slot_DummyPhantomCheckBoxClicked();
	void slot_PhantomSelect_ButtonClicked();
	void slot_PhantomAdd_ButtonClicked();
	void slot_PhantomMoveLeft_ButtonClicked();
	void slot_PhantomMoveRight_ButtonClicked();
	void slot_PhantomDelete_ButtonClicked();
	void moveMaximumScrollbar_phantom();

	void slot_ClothingAdd_ButtonClicked(); // Phantom 패널 내 Clothing Add 버튼 클릭 시 실행
	void slot_ClothingDelete_ButtonClicked(); // Phantom 패널 내 Clothing Delete 버튼 클릭 시 실행
	void slot_ClothingSelect_ButtonClicked(); // Phantom 패널 내 Clothing 리스트에서 각 clothing 선택 시 실행
	void slot_ClothingLayerAdd_ButtonClicked(); // Phantom 패널 내 Clothing Layer Add 버튼 클릭 시 실행
	void slot_ClothingLayerSelect_ButtonClicked(); // Phantom 패널 내 Clothing Layer 리스트에서 각 clothing layer 선택 시 실행
	void slot_ClothingLayerDelete_ButtonClicked(); // Phantom 패널 내 Clothing Layer Delete 버튼 클릭 시 실행
	void slot_ClothingLayerUpdate_ButtonClicked(); // Phantom 패널 내 Clothing Layer Update 버튼 클릭 시 실행
	
	void slot_EyewearSetting_Type_ComboChanged(int idx);
	void slot_EyewearSetting_Composition_ComboChanged(int idx);
	void slot_EyewearSetting_AddingOK_ButtonClicked();
	
	void slot_ChangeClothingSettingTypeCombo(int idx); // Clothing Layer Setting 창에서 clothing type 콤보박스 변경 시 실행
		
	void slot_ChangeGeometeryEnvironment(int idx);
	void slot_ObjectTransparencySlider(int value);
	void slot_ObjectChargedParticleTransport_ButtonClicked();
	void slot_ChangeObjectCombo(int idx);
	void slot_ObjectAddingOK_ButtonClicked();
	
	void slot_ObjectPanel_MaterialEdit_ButtonClicked();
	void slot_ObjectSetting_MaterialEdit_ButtonClicked();	
	void moveMaximumScrollbar_object();
	void slot_OutputPhantomSelect_ButtonClicked();
	void slot_ChangeSourceGeometryCombo(int idx);
	void slot_ClothingAddingOK_ButtonClicked(); // Clothing Setting 창 내에서 OK 버튼 클릭 시 실행
	void slot_ClothingLayerAddingOK_ButtonClicked(); // Clothing Layer Setting 창 내에서 OK 버튼 클릭 시 실행
	void moveMaximumScrollbar_clothing(); // 의복 처음 생성 시 의복패널 초기화할 때 스크롤바 오른쪽으로 최대 이동
	void moveMaximumScrollbar_clothingLayer(); // 의복 처음 생성 시 의복패널 초기화할 때 + 의복레이어 추가할 때 스크롤바 오른쪽으로 최대 이동
	void moveMaximumScrollbar_dosimeter();

	void slot_Add_Delete_Eyewear_ButtonClicked();
	void slot_DosimeterPick3D_ButtonClicked();
	void slot_DosimeterLocationUpdate_ButtonClicked();
	void slot_DosimeterUpdate_ButtonClicked();
	void slot_BroadBeamUserDefined(int user);
	void slot_sourceEP_Pick3D_ButtonClicked();
	
	
	void slot_HotParticle_Pick3D_ButtonClicked();
	void slot_FloorSetDefaultGeom_ButtonClicked();	
	void slot_FloorUpdate_ButtonClicked();
	void slot_FloorSourceVisualization_ButtonClicked();
	void slot_ObjectVolumeSourceVisualization_ButtonClicked();
	void slot_RoomContaminationSourceVisualization_ButtonClicked();	
	void slot_SourcePS_MCNP_ButtonClicked();
	void slot_SourcePS_PHITS_ButtonClicked();
	void slot_SourcePS_FLUKA_ButtonClicked();
	void slot_SourcePS_IAEA_ButtonClicked();
	void slot_SourcePS_USER_ButtonClicked();	
	void slot_sourcePS_loadPSF_ButtonClicked();
	void slot_PhaseSpaceSourceVisualization_ButtonClicked();
	void slot_DirectionCheck_sourcePS_QCheckBox_ButtonClicked();
	void slot_ConeBeam_Pick3D_ButtonClicked();

	void slot_ParallelBeam_Pick3D_ButtonClicked();
	void slot_ParallelBeamUpdate_ButtonClicked();

	void slot_SourceOKandReset_ButtonClicked();
	void slot_BUpdate_ButtonClicked();
			
	void slot_CBreset_ButtonClicked();	
	void slot_Ereset_ButtonClicked();

	void slot_UpdateRCpos_ButtonClicked();

	void slot_BroadBeamSourceVisualization_ButtonClicked();

	void slot_sourceEP_UpdatePosition_ButtonClicked();	
	void slot_sourceEP_RadionuclideSelect_RadioButtonClicked();
	void slot_sourceEP_EnergySpectrumSelect_RadioButtonClicked();
	void slot_RIList_Add_sourceEP_ButtonClicked();
	void slot_RIList_Delete_sourceEP_ButtonClicked();
	void slot_RIList_Info_sourceEP_ButtonClicked();
	void slot_ESList_Add_sourceEP_ButtonClicked();
	void slot_ESList_Delete_sourceEP_ButtonClicked();
	void slot_ESList_Info_sourceEP_ButtonClicked();
	
	void slot_SourceEP_SphereSize_Minus_ButtonClicked();
	void slot_SourceEP_SphereSize_Plus_ButtonClicked();
	void slot_ExternalPointSourceVisualization_ButtonClicked();
	void slot_EnergyspectrumFileLoad_sourceEP_ButtonClicked();
	void slot_sourceFD_EnergyspectrumFileLoad_ButtonClicked();
	void slot_sourceFD_RadionuclideSelect_RadioButtonClicked();
	void slot_sourceFD_EnergySpectrumSelect_RadioButtonClicked();
	void slot_RIList_Add_sourceFD_ButtonClicked();
	void slot_RIList_Delete_sourceFD_ButtonClicked();
	void slot_RIList_Info_sourceFD_ButtonClicked();

	void slot_ESList_Add_sourceFD_ButtonClicked();
	void slot_ESList_Delete_sourceFD_ButtonClicked();
	void slot_ESList_Info_sourceFD_ButtonClicked();

	void slot_RIList_Add_sourceRC_ButtonClicked();
	void slot_RIList_Delete_sourceRC_ButtonClicked();
	void slot_RIList_Info_sourceRC_ButtonClicked();

	void slot_sourceOV_RadionuclideSelect_RadioButtonClicked();
	void slot_sourceOV_EnergySpectrumSelect_RadioButtonClicked();
	void slot_sourceOV_ObjectAdd_ButtonClicked();
	void slot_sourceOV_ObjectSelect_ButtonClicked();
	void slot_sourceOV_ObjectDelete_ButtonClicked();
	void slot_sourceOV_ObjectSettingChangeCombo(int index);
	void slot_sourceOV_SettingOK_ButtonClicked();
	void slot_sourceOV_RadionuclideAdd_ButtonClicked();
	void slot_sourceOV_RadionuclideDelete_ButtonClicked();
	void slot_sourceOV_RadionuclideInfo_ButtonClicked();
	void slot_sourceOV_EnergyspectrumFileLoad_ButtonClicked();
	void slot_ESList_Add_sourceOV_ButtonClicked();
	void slot_ESList_Delete_sourceOV_ButtonClicked();
	void slot_ESList_Info_sourceOV_ButtonClicked();
	
	void slot_RIList_Add_sourceHP_ButtonClicked();
	void slot_Update_Position_HP_ButtonClicked();
	void slot_SourceHP_SphereSize_Minus_ButtonClicked();
	void slot_SourceHP_SphereSize_Plus_ButtonClicked();
	void slot_HotParticleSourceVisualization_ButtonClicked();
	void slot_RI_Select_sourceHP_RadioButtonClicked();	
	void slot_RIList_Delete_sourceHP_ButtonClicked();
	void slot_RIList_Info_sourceHP_ButtonClicked();

	void slot_ES_Select_sourceHP_RadioButtonClicked();
	void slot_ESList_Add_sourceHP_ButtonClicked();
	void slot_ESList_Delete_sourceHP_ButtonClicked();
	void slot_ESList_Info_sourceHP_ButtonClicked();
	void slot_EnergyspectrumFileLoad_sourceHP_ButtonClicked();



	void slot_ConeBeamSourceVisualization_ButtonClicked();
	void slot_ConeBeamUpdate_ButtonClicked();
	void slot_ConeBeamRadionuclide_ButtonClicked();
	void slot_ConeBeamRadionuclideAdd_ButtonClicked();
	void slot_ConeBeamRadionuclideDelete_ButtonClicked();
	void slot_ConeBeamRadionuclideInfo_ButtonClicked();
	void slot_ConeBeamEnergyspectrum_ButtonClicked();
	void slot_ConeBeamEnergyspectrumFileLoad_ButtonClicked();
	void slot_ConeBeamEnergyspectrumAdd_ButtonClicked();
	void slot_ConeBeamEnergyspectrumInfo_ButtonClicked();
	void slot_EnvironmentalSourceVisualization_ButtonClicked();
	void slot_EnvironmentalRadionuclideAdd_ButtonClicked();
	void slot_ChangeEnvironmentalSourceType(int idx);
	void slot_RI_Select_sourceRC_RadioButtonClicked();
	void slot_ES_Select_sourceRC_RadioButtonClicked();
	void slot_ParallelBeamSourceVisualization_ButtonClicked();
	void slot_ParallelBeamRadionuclideAdd_ButtonClicked();
	void slot_ParallelBeamEnergyspectrumFileLoad_ButtonClicked();
	void slot_ParallelBeamEnergyspectrumAdd_ButtonClicked();

	void slot_DosimeterSelect_ButtonClicked();
	void slot_DosimeterAdd_ButtonClicked();
	void slot_DosimeterDelete_ButtonClicked();
	
	void slot_ObjectSelect_ButtonClicked();
	void slot_ObjectAdd_ButtonClicked();
	void slot_ObjectDelete_ButtonClicked();
	void slot_ObjectMoveLeft_ButtonClicked();
	void slot_ObjectMoveRight_ButtonClicked();
	void slot_ObjectUpdate_ButtonClicked();

	void slot_SkinDoseVisualization_ButtonClicked();
	void slot_Output_resultType(int user);

	void slot_settingSave_ButtonClicked();
	void slot_Calculate_ButtonClicked();
	
	void uiRunning();
	void ServerInitialization();
	void checkProcessInfo_Local(); // 선량계산 중 주기적 결과 업데이트(Local 버전)
	void checkProcessInfo_Server();
	void DataInitialization_Local(); // 선량계산 실행을 위한 데이터 초기화(Local 버전)
	void DataInitialization_Server();
	void Calculate_Local(); // 선량계산 실행(Local 버전)
	void Calculate_Server();

	void Write_TextResultFile_SimulationInfo(std::string filepath);
	void Write_mcsee_File(QString filePath);	
	void Gather_PhantomData_mcsee_File(
		QJsonArray& outPhantomsArray,
		QJsonObject& outManifestObject,
		QStringList& outOrderedResourceIds,
		QMap<QString, QString>& outResourcePathMap,
		qint64& inoutCurrentOffset
	);
	void Gather_GeometryData_mcsee_File(QJsonObject& outGeometryPanelObject);
	void Load_parse_mcsee_FileData(SceneData& outSceneData);
	void Load_commit_mcsee_FileData(const SceneData& sceneData);
	void Commit_PhantomSetting(const PhantomData& pData);
	void Commit_GeometrySetting(const GeometryData& gData);
	void LoadReconsturctionFile_previous(QString dir); // 피폭상황 및 선량결과 모두 초기화(프로그램 재시작)하고 Recon 파일을 통해 불러오기
	void ResetCalculation(); // 계산 후, Reset 버튼을 통해 입력한 피폭상황을 놔두고, 선량결과만 초기화

	void show_Skindose_LineGraph_ValueTooltip(QMouseEvent *event); // Hot particle 피부깊이선량분포 꺾은선그래프에서 마우스 갖다 대면 피부선량 표시
	void setElidedButtonText(QPushButton* btn); // 버튼 이름 길 때 자동으로 "…" 처럼 생략되고 마우스 오버로 확인 가능
	
//************************************** Timer **************************************//
public:
	QTimer *timer_DataInitialization_Local;
	QTimer *timer_DataInitialization_Server;	
	QTimer *timer_uiRunning;
	QTimer *timer_checkProcessInfo_Local;
	QTimer *timer_checkProcessInfo_Server;
	QTimer *timer_scroll_multiplePhantom;
	QTimer *timer_scroll_dosimeter;
	QTimer *timer_scroll_clothing;
	QTimer *timer_scroll_clothingLayer;
	QTimer *timer_scroll_object;
	QTimer *timer_Close_PhantomUIdialog_Recon;
	QTimer *timer_Close_ClothingUIdialog_Recon;
	QTimer *timer_Close_ObjectUIdialog_Recon;

//*************************************Memory Usage Expectation**************************************//
	double Volume_to_Memory_full(int num_phantom, double volume);
	double Volume_to_Memory_dummy(int num_phantom, double volume);

//*************************************PhantomBox Bound Data**************************************//
	double maxX_PhantomBox;
	double maxY_PhantomBox;
	double maxZ_PhantomBox;
	double minX_PhantomBox;
	double minY_PhantomBox;
	double minZ_PhantomBox;
};