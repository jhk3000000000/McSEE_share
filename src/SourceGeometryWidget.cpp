#include <windows.h>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>


#include "SourceGeometryWidget.h"
#include "ETHuman3DApp.h" // theApp 접근용도
#include "ETQVTKWidget.h" // render 함수 접근 용도
#include "StyleSheet.h"
#include "pch.h"

#include "FunctionPanelRight.h" // *** 부모 패널 ***

using namespace utils::ui;


SourceGeometryWidget::SourceGeometryWidget(QWidget* parent)
	: QGroupBox(parent)
{
}

SourceGeometryWidget::~SourceGeometryWidget()
{
}

bool SourceGeometryWidget::initialize()
{
	this->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	this->setStyleSheet(DialogStyle::GROUPBOX_WIDGET3);
	this->setFont(panel->font_D_GBW3);
	this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	return true;
}


///////////////////////////////////////////////////////////////////////////
// BroadBeamWidget Implementation
///////////////////////////////////////////////////////////////////////////


BroadBeamWidget::BroadBeamWidget(QWidget* parent)
	: ParentT(parent)
{
}

BroadBeamWidget::~BroadBeamWidget()
{
}

bool BroadBeamWidget::initialize()
{
	if (!ParentT::initialize()) {
		return false;
	}

	QVBoxLayout* subLayout = new QVBoxLayout;
	subLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.005, panel->Window_width * 0.003, panel->Window_height * 0.005);
	subLayout->setSpacing(panel->Window_width * 0.003);
	subLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

	{
		QVBoxLayout* vLayout = new QVBoxLayout;
		vLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.005, panel->Window_width * 0.003, panel->Window_height * 0.005);
		vLayout->setSpacing(panel->Window_width * 0.003);
		vLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		{
			QHBoxLayout* hLayout = new QHBoxLayout;
			hLayout->setSpacing(panel->Window_width * 0.005);
			hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

			QLabel* labelTitle = new QLabel;
			labelTitle->setFixedSize(panel->Window_width * 0.104, panel->Window_height * 0.028);
			labelTitle->setStyleSheet(DialogStyle::DATA_LABEL);
			labelTitle->setFont(panel->font_D_LBL1);
			labelTitle->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
			labelTitle->setText(QString::fromLocal8Bit("Visualization"));

			m_BroadBeamSourceVisualizationButton = new QPushButton(QString::fromLocal8Bit("On"));
			panel->Buttons_FunctionPanelRight.append(m_BroadBeamSourceVisualizationButton);
			m_BroadBeamSourceVisualizationButton->setFixedSize(panel->Window_width * 0.05, panel->Window_height * 0.028);
			m_BroadBeamSourceVisualizationButton->setCheckable(true);
			m_BroadBeamSourceVisualizationButton->setChecked(true);
			m_BroadBeamSourceVisualizationButton->setStyleSheet(DialogStyle::MENU_BTN3);
			m_BroadBeamSourceVisualizationButton->setFont(panel->font_D_BTN3);

			//connect(m_BroadBeamSourceVisualizationButton, SIGNAL(clicked()), panel, SLOT(slot_BroadBeamSourceVisualization_ButtonClicked()));
			connect(m_BroadBeamSourceVisualizationButton, SIGNAL(clicked()), this, SLOT(slot_BroadBeamSourceVisualization_ButtonClicked()));
			connect(m_BroadBeamSourceVisualizationButton, SIGNAL(clicked()), panel, SLOT(slot_allButtonClicked()));

			hLayout->addWidget(labelTitle);
			hLayout->addStretch();
			hLayout->addWidget(m_BroadBeamSourceVisualizationButton);

			vLayout->addLayout(hLayout);
		}

		{
			QHBoxLayout* hLayout = new QHBoxLayout;
			hLayout->setContentsMargins(panel->Window_width * 0.003, 0, panel->Window_width * 0.003, 0);
			hLayout->setSpacing(panel->Window_width * 0.003);
			hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

			QLabel* label0 = new QLabel;
			label0->setFixedSize(panel->Window_width * 0.068, panel->Window_height * 0.028);
			label0->setStyleSheet(DialogStyle::DATA_LABEL);
			label0->setFont(panel->font_D_LBL1);
			label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
			label0->setText(QString::fromLocal8Bit("Geometry:"));

			m_comboBoxBeamdirection = new QComboBox;
			m_comboBoxBeamdirection->setFixedSize(panel->Window_width * 0.104, panel->Window_height * 0.028);
			m_comboBoxBeamdirection->setStyleSheet(DialogStyle::COMBOBOX);
			m_comboBoxBeamdirection->setFont(panel->font_D_CB1);

			m_comboBoxBeamdirection->setEditable(true);
			m_comboBoxBeamdirection->lineEdit()->setReadOnly(true);
			m_comboBoxBeamdirection->lineEdit()->setAlignment(Qt::AlignCenter);
			m_comboBoxBeamdirection->lineEdit()->setFont(panel->font_D_CB1);

			m_comboBoxBeamdirection->addItem("AP");
			m_comboBoxBeamdirection->addItem("PA");
			m_comboBoxBeamdirection->addItem("LLAT");
			m_comboBoxBeamdirection->addItem("RLAT");
			m_comboBoxBeamdirection->addItem("ROT");
			m_comboBoxBeamdirection->addItem("ISO");
			//m_comboBoxBeamdirection->addItem("User defined");
			m_comboBoxBeamdirection->setCurrentIndex(0);

			connect(m_comboBoxBeamdirection, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_BroadBeamUserDefined(int)));

			hLayout->addWidget(label0);
			hLayout->addStretch();
			hLayout->addWidget(m_comboBoxBeamdirection);

			vLayout->addLayout(hLayout);

			CreateAziPol(0, vLayout);
		}

		{// lineedit widgets
			QHBoxLayout* hLayout = new QHBoxLayout;
			hLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.003, panel->Window_width * 0.003, panel->Window_height * 0.003);
			hLayout->setSpacing(panel->Window_width * 0.003);
			hLayout->setAlignment(Qt::AlignRight | Qt::AlignTop);

			QPushButton* sourceB_UpdatePositionButton = new QPushButton(QString::fromLocal8Bit("Update"));
			panel->Buttons_FunctionPanelRight.append(sourceB_UpdatePositionButton);
			sourceB_UpdatePositionButton->setFixedSize(panel->Window_width * 0.050, panel->Window_height * 0.036);
			sourceB_UpdatePositionButton->setStyleSheet(DialogStyle::MENU_BTN2);
			sourceB_UpdatePositionButton->setFont(panel->font_D_BTN2);

			connect(sourceB_UpdatePositionButton, SIGNAL(clicked()), panel, SLOT(slot_BUpdate_ButtonClicked()));
			connect(sourceB_UpdatePositionButton, SIGNAL(clicked()), panel, SLOT(slot_allButtonClicked()));

			hLayout->addWidget(sourceB_UpdatePositionButton);

			vLayout->addLayout(hLayout);
		}

		{// Radioisotope
			QHBoxLayout* hLayout = new QHBoxLayout;
			hLayout->setContentsMargins(panel->Window_width * 0.003, 0, panel->Window_width * 0.003, 0);
			hLayout->setSpacing(panel->Window_width * 0.003);
			hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

			QLabel* label0 = new QLabel;
			label0->setFixedSize(panel->Window_width * 0.068, panel->Window_height * 0.028);
			label0->setStyleSheet(DialogStyle::DATA_LABEL);
			label0->setFont(panel->font_D_LBL1);
			label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
			label0->setText(QString::fromLocal8Bit("Particle type:"));

			m_comboBoxBroadBeamParticleType = new QComboBox;
			m_comboBoxBroadBeamParticleType->setFixedSize(panel->Window_width * 0.104, panel->Window_height * 0.028);
			m_comboBoxBroadBeamParticleType->setStyleSheet(DialogStyle::COMBOBOX);
			m_comboBoxBroadBeamParticleType->setFont(panel->font_D_CB1);

			m_comboBoxBroadBeamParticleType->setEditable(true);
			m_comboBoxBroadBeamParticleType->lineEdit()->setReadOnly(true);
			m_comboBoxBroadBeamParticleType->lineEdit()->setAlignment(Qt::AlignCenter);
			m_comboBoxBroadBeamParticleType->lineEdit()->setFont(panel->font_D_CB1);

			m_comboBoxBroadBeamParticleType->addItem("Photon");
			m_comboBoxBroadBeamParticleType->addItem("Electron");
			m_comboBoxBroadBeamParticleType->addItem("Neutron");
			m_comboBoxBroadBeamParticleType->addItem("Proton");
			m_comboBoxBroadBeamParticleType->addItem("Alpha");
			m_comboBoxBroadBeamParticleType->setCurrentIndex(0);

			hLayout->addWidget(label0);
			hLayout->addStretch();
			hLayout->addWidget(m_comboBoxBroadBeamParticleType);

			vLayout->addLayout(hLayout);
		}

		{// Energy
			QHBoxLayout* hLayout = new QHBoxLayout;
			hLayout->setContentsMargins(panel->Window_width * 0.003, 0, panel->Window_width * 0.003, 0);
			hLayout->setSpacing(panel->Window_width * 0.003);
			hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);


			QLabel* label0 = new QLabel;
			label0->setFixedSize(panel->Window_width * 0.068, panel->Window_height * 0.028);
			label0->setStyleSheet(DialogStyle::DATA_LABEL);
			label0->setFont(panel->font_D_LBL1);
			label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
			label0->setText(QString::fromLocal8Bit("Energy:"));

			m_lineBroadBeamEnergy = new QLineEdit;
			m_lineBroadBeamEnergy->setFixedSize(panel->Window_width * 0.104, panel->Window_height * 0.028);
			m_lineBroadBeamEnergy->setAlignment(Qt::AlignCenter);
			m_lineBroadBeamEnergy->setStyleSheet(DialogStyle::LINEEDIT_NAME);
			m_lineBroadBeamEnergy->setFont(panel->font_D_LE1);
			m_lineBroadBeamEnergy->setPlaceholderText(QString::fromLocal8Bit("MeV"));

			hLayout->addWidget(label0);
			hLayout->addStretch();
			hLayout->addWidget(m_lineBroadBeamEnergy);

			vLayout->addLayout(hLayout);
		}
		subLayout->addLayout(vLayout);
	}

	this->setLayout(subLayout);
	return true;
}

void BroadBeamWidget::CreateAziPol(int idx, QVBoxLayout* layout)
{
	m_AziPolGroupBox = new QGroupBox();
	m_AziPolGroupBox->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	m_AziPolGroupBox->setStyleSheet(DialogStyle::GROUPBOX_WIDGET3);
	m_AziPolGroupBox->setFont(panel->font_D_GBW3);
	m_AziPolGroupBox->setFixedHeight(panel->Window_height * 0.08);
	m_AziPolGroupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	QVBoxLayout* subLayout = new QVBoxLayout;
	subLayout->setContentsMargins(0, panel->Window_height * 0.005, panel->Window_width * 0.003, 0);
	subLayout->setSpacing(panel->Window_width * 0.003);
	subLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

	m_AziPolGroupBox->setLayout(subLayout);
	layout->addWidget(m_AziPolGroupBox);
	{
		QVBoxLayout* vLayout = new QVBoxLayout;
		vLayout->setContentsMargins(0, panel->Window_height * 0.005, panel->Window_width * 0.003, 0);
		vLayout->setSpacing(panel->Window_width * 0.003);
		vLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		{
			QHBoxLayout* hLayout = new QHBoxLayout;
			hLayout->setContentsMargins(panel->Window_width * 0.01, panel->Window_height * 0.005, 0, 0);
			hLayout->setSpacing(panel->Window_width * 0.003);
			hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);


			QLabel* label0 = new QLabel;
			label0->setFixedSize(panel->Window_width * 0.069, panel->Window_height * 0.028);
			label0->setStyleSheet(DialogStyle::DATA_LABEL);
			label0->setFont(panel->font_D_LBL1);
			label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
			label0->setText(QString::fromLocal8Bit("Azimuthal ang.:"));

			m_lineEditAzimuthalAngle = new QLineEdit;
			m_lineEditAzimuthalAngle->setFixedSize(panel->Window_width * 0.104, panel->Window_height * 0.028);
			m_lineEditAzimuthalAngle->setAlignment(Qt::AlignCenter);
			m_lineEditAzimuthalAngle->setStyleSheet(DialogStyle::LINEEDIT_NAME);
			m_lineEditAzimuthalAngle->setFont(panel->font_D_LE1);
			m_lineEditAzimuthalAngle->setPlaceholderText(QString::fromLocal8Bit("(deg)"));

			hLayout->addWidget(label0);
			hLayout->addWidget(m_lineEditAzimuthalAngle);

			vLayout->addLayout(hLayout);
		}

		{
			QHBoxLayout* hLayout = new QHBoxLayout;
			hLayout->setContentsMargins(panel->Window_width * 0.01, panel->Window_height * 0.005, 0, 0);
			hLayout->setSpacing(panel->Window_width * 0.003);
			hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);


			QLabel* label0 = new QLabel;
			label0->setFixedSize(panel->Window_width * 0.069, panel->Window_height * 0.028);
			label0->setStyleSheet(DialogStyle::DATA_LABEL);
			label0->setFont(panel->font_D_LBL1);
			label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
			label0->setText(QString::fromLocal8Bit("Polar angle:"));

			m_lineEditPolarAngle = new QLineEdit;
			m_lineEditPolarAngle->setFixedSize(panel->Window_width * 0.104, panel->Window_height * 0.028);
			m_lineEditPolarAngle->setAlignment(Qt::AlignCenter);
			m_lineEditPolarAngle->setStyleSheet(DialogStyle::LINEEDIT_NAME);
			m_lineEditPolarAngle->setFont(panel->font_D_LE1);
			m_lineEditPolarAngle->setPlaceholderText(QString::fromLocal8Bit("(deg)"));

			hLayout->addWidget(label0);
			hLayout->addWidget(m_lineEditPolarAngle);

			vLayout->addLayout(hLayout);
		}

		subLayout->addLayout(vLayout);

		SetBroadBeamAziPol(idx);
	}
}

void BroadBeamWidget::slot_BroadBeamUserDefined(int user)
{
	SetBroadBeamAziPol(user);
}

void BroadBeamWidget::SetBroadBeamAziPol(int user)
{
	if (6 == user)
	{
		m_AziPolGroupBox->show();
	}
	else
	{
		m_AziPolGroupBox->hide();
	}
}

void BroadBeamWidget::slot_BroadBeamSourceVisualization_ButtonClicked()
{
	if (m_BroadBeamSourceVisualizationButton->isChecked())
	{
		m_BroadBeamSourceVisualizationButton->setText("On");
		if (m_comboBoxBeamdirection->currentIndex() == 0 || m_comboBoxBeamdirection->currentIndex() == 1 || m_comboBoxBeamdirection->currentIndex() == 2 || m_comboBoxBeamdirection->currentIndex() == 3) //AP, PA, LLAT, RLAT
		{
			for (auto itr : theApp.actors_cylinderB) theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(itr);
			for (auto itr : theApp.actors_coneB) theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(itr);
			theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(theApp.actor_circleB);
		}
		if (m_comboBoxBeamdirection->currentIndex() == 4) //ROT
		{
			for (auto itr : theApp.actors_cylinderB) theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(itr);
			for (auto itr : theApp.actors_coneB) theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(itr);
			theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(theApp.actor_circleB);
			theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(theApp.actor_single_cylinderB);
			for (auto itr : theApp.actors_arrowB) theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(itr);
		}
		if (m_comboBoxBeamdirection->currentIndex() == 5) //ISO
		{
			for (auto itr : theApp.actors_cylinderB) theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(itr);
			for (auto itr : theApp.actors_coneB) theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(itr);
			theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(theApp.actor_circleB);
			theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(theApp.actor_sphereB);
			for (auto itr : theApp.actors_arrowB) theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(itr);
		}		
	}
	else
	{
		m_BroadBeamSourceVisualizationButton->setText("Off");
		if (m_comboBoxBeamdirection->currentIndex() == 0 || m_comboBoxBeamdirection->currentIndex() == 1 || m_comboBoxBeamdirection->currentIndex() == 2 || m_comboBoxBeamdirection->currentIndex() == 3) //AP, PA, LLAT, RLAT
		{
			for (auto itr : theApp.actors_cylinderB) theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(itr);
			for (auto itr : theApp.actors_coneB) theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(itr);
			theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(theApp.actor_circleB);
		}
		if (m_comboBoxBeamdirection->currentIndex() == 4) //ROT
		{
			for (auto itr : theApp.actors_cylinderB) theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(itr);
			for (auto itr : theApp.actors_coneB) theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(itr);
			theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(theApp.actor_circleB);
			theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(theApp.actor_single_cylinderB);
			for (auto itr : theApp.actors_arrowB) theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(itr);
		}
		if (m_comboBoxBeamdirection->currentIndex() == 5) //ISO
		{
			for (auto itr : theApp.actors_cylinderB) theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(itr);
			for (auto itr : theApp.actors_coneB) theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(itr);
			theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(theApp.actor_circleB);
			theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(theApp.actor_sphereB);
			for (auto itr : theApp.actors_arrowB) theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(itr);
		}
	}
	theApp.m_pVTKWidget->renderWindow()->Render();
}
///////////////////////////////////////////////////////////////////////////
// ExternalPointWidget Implementation
///////////////////////////////////////////////////////////////////////////

ExternalPointWidget::ExternalPointWidget(QWidget* parent)
    : ParentT(parent)
{
}

ExternalPointWidget::~ExternalPointWidget()
{
}

bool ExternalPointWidget::initialize()
{
    if (!ParentT::initialize()) return false;

    // panel->Window_width 처럼 panel 포인터를 통해 부모의 자원에 접근합니다.
    QVBoxLayout* subLayout = new QVBoxLayout;
    subLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.005, panel->Window_width * 0.003, panel->Window_height * 0.005);
    subLayout->setSpacing(panel->Window_width * 0.003);
    subLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    {
        QVBoxLayout* vLayout = new QVBoxLayout;
        vLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.005, panel->Window_width * 0.003, panel->Window_height * 0.005);
        vLayout->setSpacing(panel->Window_width * 0.003);
        vLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

        // [Visualization Section]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setSpacing(panel->Window_width * 0.005);
            hLayout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

            QLabel* labelTitle = new QLabel;
            labelTitle->setFixedSize(panel->Window_width * 0.104, panel->Window_height * 0.028);
            labelTitle->setStyleSheet(DialogStyle::DATA_LABEL);
            labelTitle->setFont(panel->font_D_LBL1);
            labelTitle->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            labelTitle->setText(QString::fromLocal8Bit("Visualization"));

            QSpacerItem *spacer1 = new QSpacerItem(panel->Window_width * 0.06, panel->Window_height * 0.028);

            SourceEP_SphereSize_Minus_QPushButton = new QPushButton(QString::fromLocal8Bit("-"));
            panel->Buttons_FunctionPanelRight.append(SourceEP_SphereSize_Minus_QPushButton); // 부모 패널의 버튼 관리 리스트에 추가
            SourceEP_SphereSize_Minus_QPushButton->setFixedSize(panel->Window_width * 0.015, panel->Window_height * 0.028);
            SourceEP_SphereSize_Minus_QPushButton->setStyleSheet(DialogStyle::MENU_BTN3);
            SourceEP_SphereSize_Minus_QPushButton->setFont(panel->font_D_BTN3);

            SourceEP_SphereSize_Plus_QPushButton = new QPushButton(QString::fromLocal8Bit("+"));
            panel->Buttons_FunctionPanelRight.append(SourceEP_SphereSize_Plus_QPushButton);
            SourceEP_SphereSize_Plus_QPushButton->setFixedSize(panel->Window_width * 0.015, panel->Window_height * 0.028);
            SourceEP_SphereSize_Plus_QPushButton->setStyleSheet(DialogStyle::MENU_BTN3);
            SourceEP_SphereSize_Plus_QPushButton->setFont(panel->font_D_BTN3);

            m_ExternalPointSourceVisualizationButton = new QPushButton(QString::fromLocal8Bit("On"));
            panel->Buttons_FunctionPanelRight.append(m_ExternalPointSourceVisualizationButton);
            m_ExternalPointSourceVisualizationButton->setFixedSize(panel->Window_width * 0.05, panel->Window_height * 0.028);
            m_ExternalPointSourceVisualizationButton->setCheckable(true);
            m_ExternalPointSourceVisualizationButton->setChecked(true);
            m_ExternalPointSourceVisualizationButton->setStyleSheet(DialogStyle::MENU_BTN3);
            m_ExternalPointSourceVisualizationButton->setFont(panel->font_D_BTN3);

            // [중요] connect의 수신자를 this(현재 위젯)로 변경
            connect(SourceEP_SphereSize_Minus_QPushButton, SIGNAL(clicked()), this, SLOT(slot_SourceEP_SphereSize_Minus_ButtonClicked())); 
            connect(SourceEP_SphereSize_Minus_QPushButton, SIGNAL(clicked()), panel, SLOT(slot_allButtonClicked())); // 공통 슬롯은 panel로
            
            connect(SourceEP_SphereSize_Plus_QPushButton, SIGNAL(clicked()), this, SLOT(slot_SourceEP_SphereSize_Plus_ButtonClicked())); 
            connect(SourceEP_SphereSize_Plus_QPushButton, SIGNAL(clicked()), panel, SLOT(slot_allButtonClicked()));

            connect(m_ExternalPointSourceVisualizationButton, SIGNAL(clicked()), this, SLOT(slot_ExternalPointSourceVisualization_ButtonClicked())); 
            connect(m_ExternalPointSourceVisualizationButton, SIGNAL(clicked()), panel, SLOT(slot_allButtonClicked()));
                        
            hLayout->addWidget(labelTitle);
            hLayout->addSpacerItem(spacer1);
            hLayout->addStretch();
            hLayout->addWidget(SourceEP_SphereSize_Minus_QPushButton);
            hLayout->addWidget(SourceEP_SphereSize_Plus_QPushButton);
            hLayout->addWidget(m_ExternalPointSourceVisualizationButton);

            vLayout->addLayout(hLayout);
        }

        // ... (중략: Position XYZ, 3D Pick 등 나머지 UI 코드도 위와 같은 방식으로 를 붙여서 복사) ...
        // ... Radionuclide, Activity, ListWidget 등 모든 UI 코드를 여기에 넣으세요.
        // ... connect 문에서 this, SLOT(...) 부분만 주의하시면 됩니다.
        
        // [예시: Radionuclide Completer 부분]
        // panel->RIsourceList가 panel에 있다고 가정합니다.
        /*
        QCompleter *completer = new QCompleter(panel->RIsourceList, this); 
        completer->setCaseSensitivity(Qt::CaseInsensitive);
        RI_Radionuclide_sourceEP_QLineEdit->setCompleter(completer);
        */

        subLayout->addLayout(vLayout);
    }

    this->setLayout(subLayout);
    return true;
}

void ExternalPointWidget::slot_SourceEP_SphereSize_Minus_ButtonClicked()
{
    // theApp은 전역 변수(Singleton)라고 가정합니다.
    theApp.m_Point_Source_Scale--;
    for (auto itr_source_actor : theApp.SourcePanelActor_Position)
    {
        itr_source_actor->SetScale(pow(theApp.const_PointSource_ScaleFactorPerClick, theApp.m_Point_Source_Scale));
    }
    theApp.m_pVTKWidget->renderWindow()->Render();
}
void ExternalPointWidget::slot_SourceEP_SphereSize_Plus_ButtonClicked()
{
	theApp.m_Point_Source_Scale++;
	for (auto itr_source_actor : theApp.SourcePanelActor_Position)
	{
		itr_source_actor->SetScale(pow(theApp.const_PointSource_ScaleFactorPerClick, theApp.m_Point_Source_Scale));
	}
	theApp.m_pVTKWidget->renderWindow()->Render();
}
void ExternalPointWidget::slot_ExternalPointSourceVisualization_ButtonClicked()
{
    if (m_ExternalPointSourceVisualizationButton->isChecked())
    {
        m_ExternalPointSourceVisualizationButton->setText("On");
        for (auto itr: theApp.SourcePanelActor_Position) theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(itr);
    }
    else
    {
        m_ExternalPointSourceVisualizationButton->setText("Off");
        for (auto itr : theApp.SourcePanelActor_Position) theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(itr);
    }
    theApp.m_pVTKWidget->renderWindow()->Render();
}
void ExternalPointWidget::slot_sourceEP_Pick3D_ButtonClicked()
{   
	theApp.sourceObjects->Selecting3DShpere_Delete();
	PosX_sourceEP_QLineEdit->clear();
	PosY_sourceEP_QLineEdit->clear();
	PosZ_sourceEP_QLineEdit->clear();

    // ... 초기화 코드 ...
    QPushButton* clickedButton = qobject_cast<QPushButton*>(sender());

    if (pickExternal3DButton->isChecked() == true) 
    {
        theApp.st_Pick3D_sourceEP = true;
        
        // panel의 버튼 리스트 순회
        for (QList<QPushButton*>::iterator it = panel->Buttons_FunctionPanelRight.begin(); it != panel->Buttons_FunctionPanelRight.end(); ++it) 
        {
            if (*it != clickedButton && *it != sourceEP_UpdatePositionButton)
            {  
                if ((*it)->isEnabled() == true) panel->TempEnabledButtons.append((*it)); // panel의 Temp 리스트 사용
                if ((*it)->isEnabled() == false) panel->TempDisabledButtons.append((*it));
                (*it)->setDisabled(true);
            }
        }
    }
    else 
    {
		theApp.st_Pick3D_sourceEP = false;

		for (QList<QPushButton*>::iterator it = panel->TempEnabledButtons.begin(); it != panel->TempEnabledButtons.end(); ++it)
		{
			(*it)->setEnabled(true);
		}
		panel->TempEnabledButtons.clear();

		for (QList<QPushButton*>::iterator it = panel->TempDisabledButtons.begin(); it != panel->TempDisabledButtons.end(); ++it)
		{
			(*it)->setDisabled(true);
		}
		panel->TempDisabledButtons.clear();
    }   
}
void ExternalPointWidget::slot_sourceEP_UpdatePosition_ButtonClicked()
{
	double center[3] = { 0.0, };
	center[0] = PosX_sourceEP_QLineEdit->text().toFloat();
	center[1] = PosY_sourceEP_QLineEdit->text().toFloat();
	center[2] = PosZ_sourceEP_QLineEdit->text().toFloat();

	theApp.sourceObjects->Selecting3DShpere(center); // 임시 가리키는 임시 sphere 생성

	if (theApp.st_Pick3D_sourceEP == true)
	{
		// Pick 완료로 버튼들 원복하기
		pickExternal3DButton->setChecked(false);
		theApp.st_Pick3D_sourceEP = false;

		for (QList<QPushButton*>::iterator it = panel->TempEnabledButtons.begin(); it != panel->TempEnabledButtons.end(); ++it)
		{
			(*it)->setEnabled(true);
		}
		panel->TempEnabledButtons.clear();

		for (QList<QPushButton*>::iterator it = panel->TempDisabledButtons.begin(); it != panel->TempDisabledButtons.end(); ++it)
		{
			(*it)->setDisabled(true);
		}
		panel->TempDisabledButtons.clear();
	}


}
void ExternalPointWidget::slot_sourceEP_RadionuclideSelect_RadioButtonClicked()
{
	RI_Select_sourceEP_QRadioButton->setChecked(true);
	ES_Select_sourceEP_QRadioButton->setChecked(false);

	RI_Title_sourceEP_QLabel->show();
	RI_Radionuclide_sourceEP_QLineEdit->show();
	RI_Activity_sourceEP_QLabel->show();
	RI_Activity_sourceEP_QLineEdit->show();
	RI_List_sourceEP_QLabel->show();
	ExternalSingleRadionuclideAddButton->show();
	RI_List_sourceEP_QListWidget->show();

	ES_Title_sourceEP_QLabel->hide();
	ExternalSingleEnergyspectrumFileLoadButton->hide();
	ES_Intensity_sourceEP_QLabel->hide();
	ES_Intensity_sourceEP_QLineEdit->hide();
	ES_FileName_sourceEP_QLabel->hide();
	sourceEP_EnergyspectrumFileNameText->hide();
	ES_List_sourceEP_QLabel->hide();
	ExternalSingleEnergySpectrumAddButton->hide();
	ES_List_sourceEP_QListWidget->hide();

	// === ES 관련 초기화 ===

	// 1. VTK 렌더러에서 모든 관련 Actor 제거
	// a. 먼저 루프를 돌면서 각 Actor를 렌더러에서 제거합니다.
	for (vtkActor* actor : theApp.SourcePanelActor_Position)
	{
		if (actor) // 포인터가 유효한지 확인
		{
			theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(actor);
		}
	}
	// b. VTK 렌더 윈도우를 업데이트하여 변경 사항을 적용합니다.
	theApp.m_pVTKWidget->renderWindow()->Render();

	// 2. QListWidget의 모든 아이템 제거
	// clear() 함수는 위젯의 모든 아이템을 삭제하고 메모리에서 해제합니다.
	ES_List_sourceEP_QListWidget->clear();

	// 3. 관리하던 모든 vector의 내용 비우기
	// 각 vector의 clear() 멤버 함수를 호출하여 모든 요소를 제거합니다.
	ListInfo_sourceEP.clear();
	ES_ListDelete_sourceEP_QButton.clear();
	ES_ListInfo_sourceEP_QButton.clear();
	theApp.SourcePanelActor_Position.clear();

	// 4. 인덱스 카운터 초기화
	// 리스트 아이템 개수를 추적하는 변수를 초기값(보통 0)으로 리셋합니다.
	List_sourceEP_MakingIndex = 0;
}
void ExternalPointWidget::slot_sourceEP_EnergySpectrumSelect_RadioButtonClicked()
{
	RI_Select_sourceEP_QRadioButton->setChecked(false);
	ES_Select_sourceEP_QRadioButton->setChecked(true);

	RI_Title_sourceEP_QLabel->hide();
	RI_Radionuclide_sourceEP_QLineEdit->hide();
	RI_Activity_sourceEP_QLabel->hide();
	RI_Activity_sourceEP_QLineEdit->hide();
	RI_List_sourceEP_QLabel->hide();
	ExternalSingleRadionuclideAddButton->hide();
	RI_List_sourceEP_QListWidget->hide();

	ES_Title_sourceEP_QLabel->show();
	ExternalSingleEnergyspectrumFileLoadButton->show();
	ES_Intensity_sourceEP_QLabel->show();
	ES_Intensity_sourceEP_QLineEdit->show();
	ES_FileName_sourceEP_QLabel->show();
	sourceEP_EnergyspectrumFileNameText->show();
	ES_List_sourceEP_QLabel->show();
	ExternalSingleEnergySpectrumAddButton->show();
	ES_List_sourceEP_QListWidget->show();

	// === RI 관련 초기화 ===

	// 1. VTK 렌더러에서 모든 관련 Actor 제거
	// a. 먼저 루프를 돌면서 각 Actor를 렌더러에서 제거합니다.
	for (vtkActor* actor : theApp.SourcePanelActor_Position)
	{
		if (actor) // 포인터가 유효한지 확인
		{
			theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(actor);
		}
	}
	// b. VTK 렌더 윈도우를 업데이트하여 변경 사항을 적용합니다.
	theApp.m_pVTKWidget->renderWindow()->Render();

	// 2. QListWidget의 모든 아이템 제거
	// clear() 함수는 위젯의 모든 아이템을 삭제하고 메모리에서 해제합니다.
	RI_List_sourceEP_QListWidget->clear();

	// 3. 관리하던 모든 vector의 내용 비우기
	// 각 vector의 clear() 멤버 함수를 호출하여 모든 요소를 제거합니다.
	ListInfo_sourceEP.clear();
	RI_ListDelete_sourceEP_QButton.clear();
	RI_ListInfo_sourceEP_QButton.clear();
	theApp.SourcePanelActor_Position.clear();

	// 4. 인덱스 카운터 초기화
	// 리스트 아이템 개수를 추적하는 변수를 초기값(보통 0)으로 리셋합니다.
	List_sourceEP_MakingIndex = 0;
}

void ExternalPointWidget::slot_RIList_Add_sourceEP_ButtonClicked()
{
	// 지원하는 RI 리스트에 있는지 판별
	QString inputText = RI_Radionuclide_sourceEP_QLineEdit->text().trimmed();
	if (inputText.isEmpty()) {
		theApp.SetMessageBox("Radionuclide name is empty!");
		return;
	}

	// 1) 대소문자 구분하여 리스트에 있는지 검사
	if (!panel->RIsourceList.contains(inputText, Qt::CaseSensitive)) {
		// 2) 자동 보정: 첫 글자만 대문자, 나머지는 소문자
		QString corrected = inputText.toLower();
		if (!corrected.isEmpty()) {
			corrected[0] = corrected[0].toUpper();
		}
		// 보정된 문자열이 리스트에 있으면 QLineEdit에 반영
		if (panel->RIsourceList.contains(corrected, Qt::CaseSensitive)) {
			RI_Radionuclide_sourceEP_QLineEdit->setText(corrected);
			inputText = corrected;
		}
		else {
			// 보정해도 없으면 에러
			theApp.SetMessageBox(inputText + ": Radionuclide name error!");
			return;
		}
	}

	QListWidgetItem* item = new QListWidgetItem(RI_List_sourceEP_QListWidget);
	QWidget* widget = new QWidget;
	QHBoxLayout* hLayout = new QHBoxLayout;
	widget->setStyleSheet("background-color: white;");
		
	QString arg1 = RI_Radionuclide_sourceEP_QLineEdit->text();
	QLabel* label1 = new QLabel(arg1);
	label1->setFixedWidth(panel->Window_width * 0.025);
	label1->setFont(panel->font_D_LW2); // Set font for the label

	QString arg2 = QString::number(RI_Activity_sourceEP_QLineEdit->text().toDouble(), 'e', 3);
	QLabel* label2 = new QLabel(arg2);
	label2->setFixedWidth(panel->Window_width * 0.025);	
	label2->setFont(panel->font_D_LW2); // Set font for the label

	QString arg3 = PosX_sourceEP_QLineEdit->text().left(6);
	if (arg3 == "") arg3 = "0";
	QLabel* labelPosX = new QLabel(arg3);
	labelPosX->setFixedWidth(panel->Window_width * 0.025);
	labelPosX->setFont(panel->font_D_LW2); // Set font for the label

	QString arg4 = PosY_sourceEP_QLineEdit->text().left(6);
	if (arg4 == "") arg4 = "0";
	QLabel* labelPosY = new QLabel(arg4);	
	labelPosY->setFixedWidth(panel->Window_width * 0.025);
	labelPosY->setFont(panel->font_D_LW2); // Set font for the label

	QString arg5 = PosZ_sourceEP_QLineEdit->text().left(6);
	if (arg5 == "") arg5 = "0";
	QLabel* labelPosZ = new QLabel(arg5);
	labelPosZ->setFixedWidth(panel->Window_width * 0.025);
	labelPosZ->setFont(panel->font_D_LW2); // Set font for the label
		
	RI_ListDelete_sourceEP_QButton.push_back(new QPushButton());
	if (List_sourceEP_MakingIndex >= RI_ListDelete_sourceEP_QButton.size()) theApp.SetMessageBox("m_sourceEP_listDeleteButton Vector index error");
	RI_ListDelete_sourceEP_QButton[List_sourceEP_MakingIndex]->setCheckable(false);
	RI_ListDelete_sourceEP_QButton[List_sourceEP_MakingIndex]->setStyleSheet(DialogStyle::MENU_BTN13);
	RI_ListDelete_sourceEP_QButton[List_sourceEP_MakingIndex]->setFont(panel->font_D_BTN13);
	RI_ListDelete_sourceEP_QButton[List_sourceEP_MakingIndex]->setFixedSize(panel->Window_width * 0.008, panel->Window_height * 0.012);
	RI_ListDelete_sourceEP_QButton[List_sourceEP_MakingIndex]->setIcon(QIcon("./data/image/delete.png"));
	RI_ListDelete_sourceEP_QButton[List_sourceEP_MakingIndex]->setObjectName(QString::number(List_sourceEP_MakingIndex));
		
	RI_ListInfo_sourceEP_QButton.push_back(new QPushButton());
	if (List_sourceEP_MakingIndex >= RI_ListDelete_sourceEP_QButton.size()) theApp.SetMessageBox("m_sourceEP_listInfoButton Vector index error");
	RI_ListInfo_sourceEP_QButton[List_sourceEP_MakingIndex]->setCheckable(false);
	RI_ListInfo_sourceEP_QButton[List_sourceEP_MakingIndex]->setStyleSheet(DialogStyle::MENU_BTN13);
	RI_ListInfo_sourceEP_QButton[List_sourceEP_MakingIndex]->setFont(panel->font_D_BTN13);
	RI_ListInfo_sourceEP_QButton[List_sourceEP_MakingIndex]->setFixedSize(panel->Window_width * 0.008, panel->Window_height * 0.012);
	RI_ListInfo_sourceEP_QButton[List_sourceEP_MakingIndex]->setIcon(QIcon("./data/image/info.png"));	
	RI_ListInfo_sourceEP_QButton[List_sourceEP_MakingIndex]->setObjectName(QString::number(List_sourceEP_MakingIndex));

	hLayout->addWidget(label1);
	hLayout->addWidget(label2);
	hLayout->addWidget(labelPosX);
	hLayout->addWidget(labelPosY);
	hLayout->addWidget(labelPosZ);
	hLayout->addWidget(RI_ListDelete_sourceEP_QButton[List_sourceEP_MakingIndex]);
	hLayout->addWidget(RI_ListInfo_sourceEP_QButton[List_sourceEP_MakingIndex]);
	widget->setLayout(hLayout);

	RI_List_sourceEP_QListWidget->setItemWidget(item, widget);
	item->setSizeHint(widget->sizeHint());

	// Connect signals of the new buttons
	connect(RI_ListDelete_sourceEP_QButton[List_sourceEP_MakingIndex], SIGNAL(clicked()), this, SLOT(slot_RIList_Delete_sourceEP_ButtonClicked())); connect(RI_ListDelete_sourceEP_QButton[List_sourceEP_MakingIndex], SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
	connect(RI_ListInfo_sourceEP_QButton[List_sourceEP_MakingIndex], SIGNAL(clicked()), this, SLOT(slot_RIList_Info_sourceEP_ButtonClicked())); connect(RI_ListInfo_sourceEP_QButton[List_sourceEP_MakingIndex], SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));

	// Generate actor
	double center[3] = { 0.0, };
	center[0] = PosX_sourceEP_QLineEdit->text().toFloat();
	center[1] = PosY_sourceEP_QLineEdit->text().toFloat();
	center[2] = PosZ_sourceEP_QLineEdit->text().toFloat();

	theApp.sourceObjects->GenerateSourceActor_sourcePS(center);

	// Set Global Variables
	std::map<int, QString> tmp_map;
	tmp_map[0] = "True";
	tmp_map[1] = PosX_sourceEP_QLineEdit->text(); if (tmp_map[1] == "") tmp_map[1] = "0";
	tmp_map[2] = PosY_sourceEP_QLineEdit->text(); if (tmp_map[2] == "") tmp_map[2] = "0";
	tmp_map[3] = PosZ_sourceEP_QLineEdit->text(); if (tmp_map[3] == "") tmp_map[3] = "0";
	tmp_map[4] = RI_Radionuclide_sourceEP_QLineEdit->text();
	tmp_map[5] = RI_Activity_sourceEP_QLineEdit->text(); if (tmp_map[5] == "") tmp_map[5] = "0";
	ListInfo_sourceEP.push_back(tmp_map);
	List_sourceEP_MakingIndex++;

	// Set panel info 
	PosX_sourceEP_QLineEdit->clear();
	PosY_sourceEP_QLineEdit->clear();
	PosZ_sourceEP_QLineEdit->clear();
	RI_Activity_sourceEP_QLineEdit->clear();
	RI_Radionuclide_sourceEP_QLineEdit->clear();

	if(!panel->b_Is_Reconstruction) theApp.SetMessageBox_RadionuclideWarning("The radionuclide data provided in this code are based on the data provided in the ICRP 107 publication.\nTHIS DATA DO NOT INCLUDE ANY DAUGHTER NUCLIDES!!!");
}
void ExternalPointWidget::slot_RIList_Delete_sourceEP_ButtonClicked()
{
	// Button의 Index 찾기
	QPushButton* buttonSender = qobject_cast<QPushButton*>(sender());
	QString buttonText = buttonSender->objectName();

	// The first part is the ID
	int RadonuclideIndex = buttonText.toDouble();
	int row = RI_List_sourceEP_QListWidget->row(RI_List_sourceEP_QListWidget->item(RadonuclideIndex));
	QListWidgetItem* toRemove = RI_List_sourceEP_QListWidget->takeItem(row);
	delete toRemove; // Delete the item to free memory
	
	if (RadonuclideIndex >= 0 && RadonuclideIndex < ListInfo_sourceEP.size()) 
	{
		ListInfo_sourceEP.erase(ListInfo_sourceEP.begin() + RadonuclideIndex);
		RI_ListDelete_sourceEP_QButton.erase(RI_ListDelete_sourceEP_QButton.begin() + RadonuclideIndex);
		RI_ListInfo_sourceEP_QButton.erase(RI_ListInfo_sourceEP_QButton.begin() + RadonuclideIndex);
		// Actor delete
		theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(theApp.SourcePanelActor_Position[RadonuclideIndex]);
		theApp.m_pVTKWidget->renderWindow()->Render();
		theApp.SourcePanelActor_Position.erase(theApp.SourcePanelActor_Position.begin() + RadonuclideIndex);
	}
	else {
		theApp.SetMessageBox("sourceEP_info Vecetor index error");
	}
	
	for (int index = 0; index < RI_ListDelete_sourceEP_QButton.size(); index++)
	{
		RI_ListDelete_sourceEP_QButton[index]->setObjectName(QString::number(index));
		RI_ListInfo_sourceEP_QButton[index]->setObjectName(QString::number(index));
	}
	List_sourceEP_MakingIndex--; // listwidget은 vector 형태로 동작해야 함(row의 item이 실제 몇 행인지로 결정됨)	
}
void ExternalPointWidget::slot_RIList_Info_sourceEP_ButtonClicked()
{
	// Button의 Index 찾기
	QPushButton* buttonSender = qobject_cast<QPushButton*>(sender());
	QString buttonText = buttonSender->objectName();

	QStringList list_button = buttonText.split("_");

	// The first part is the ID
	int RadonuclideIndex = list_button[0].toInt();
	
	QListWidgetItem* item = RI_List_sourceEP_QListWidget->item(RadonuclideIndex);
	if (!item) {
		// Handle error: item not found
		return;
	}

	QWidget* widget = RI_List_sourceEP_QListWidget->itemWidget(item);
	QHBoxLayout* hLayout = qobject_cast<QHBoxLayout*>(widget->layout());
	if (!hLayout) {
		// Handle error: layout not found
		return;
	}

	// Assuming the order of labels in the layout is known
	QLabel* labelRadionuclide = qobject_cast<QLabel*>(hLayout->itemAt(0)->widget());
	QLabel* labelActivity = qobject_cast<QLabel*>(hLayout->itemAt(1)->widget());
	QLabel* labelX = qobject_cast<QLabel*>(hLayout->itemAt(2)->widget());
	QLabel* labelY = qobject_cast<QLabel*>(hLayout->itemAt(3)->widget());
	QLabel* labelZ = qobject_cast<QLabel*>(hLayout->itemAt(4)->widget());

	// Create a new dialog
	QDialog* infoDialog = new QDialog(this);
	infoDialog->setStyleSheet("background-color: white;");
	infoDialog->resize(panel->Window_width * 0.3, panel->Window_height * 0.55);
	infoDialog->setWindowTitle("Radionuclide Information");

	// Create a layout and a label for the dialog
	QVBoxLayout* layout = new QVBoxLayout(infoDialog);
	// Create a QTextEdit for the dialog
	QTextEdit* textEdit = new QTextEdit(infoDialog);
	textEdit->setReadOnly(true); // Make it read-only if editing is not required

	// Create a QTextCursor for inserting formatted text
	QTextCursor cursor = textEdit->textCursor();

	// Define different text formats
	QTextCharFormat boldFormat;
	boldFormat.setFontFamily("Courier New"); // This is monospaced font
	boldFormat.setFontWeight(QFont::Bold);
	boldFormat.setFontPointSize(14);

	QTextCharFormat boldMediumFormat;
	boldMediumFormat.setFontFamily("Courier New");
	boldMediumFormat.setFontWeight(QFont::Bold);
	boldMediumFormat.setFontPointSize(12);

	QTextCharFormat boldSmallFormat;
	boldSmallFormat.setFontFamily("Courier New");
	boldSmallFormat.setFontWeight(QFont::Bold);
	boldSmallFormat.setFontPointSize(10);

	QTextCharFormat normalFormat;
	normalFormat.setFontFamily("Courier New");
	normalFormat.setFontPointSize(10);

	QTextCharFormat smallFormat;
	smallFormat.setFontFamily("Courier New");
	smallFormat.setFontPointSize(8);

	QTextCharFormat verySmallFormat;
	verySmallFormat.setFontFamily("Courier New");
	verySmallFormat.setFontPointSize(6);

	// Insert formatted text into QTextEdit
	cursor.insertText("*This radionuclide information is based on ICRP 107 Publication\n\n", smallFormat);
	cursor.insertText("Radionuclide Information\n", boldFormat);
	cursor.insertText("Radionuclide: " + labelRadionuclide->text() + "\n", normalFormat);
	cursor.insertText("Activity (Bq): " + labelActivity->text() + "\n", normalFormat);
	cursor.insertText("PosX (cm): " + labelX->text() + "\n", normalFormat);
	cursor.insertText("PosY (cm): " + labelY->text() + "\n", normalFormat);
	cursor.insertText("PosZ (cm): " + labelZ->text() + "\n", normalFormat);

	// Fine RN information from ICRP07.NDX file (ICRP 107 Pub.)
	QFile file("./data/radioisotopes/_ICRP-07.NDX");
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		// Handle error: Unable to open file
		return;
	}
	QTextStream in(&file);
	QString line;
	bool isDataFound = false;
	QString halfLife, daughterNucleus1, branchingFraction1, daughterNucleus2, branchingFraction2, daughterNucleus3, branchingFraction3, daughterNucleus4, branchingFraction4;
	int daughterNucleus1_index = 7; // 이상적으로 8열(Index: 7)에서 딸핵종1 시작

	while (!in.atEnd()) {
		line = in.readLine();
		if (line.trimmed().isEmpty()) continue; // Skip empty lines
		QStringList columns = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
		if (columns[0] == labelRadionuclide->text()) {
			isDataFound = true;
			halfLife = columns[1]; // Extract HalfLife
			bool isNumber; double tmpNum;
			tmpNum = columns[3].toDouble(&isNumber); // 이상적으로는 숫자, 하지만 DecayMode 알파벳이 할당되어 있을수도 있음
			if (!isNumber) daughterNucleus1_index++; // Decay Mode has assgined to two columns
			daughterNucleus1 = columns[daughterNucleus1_index];
			branchingFraction1 = columns[daughterNucleus1_index + 2];
			tmpNum = columns[daughterNucleus1_index + 3].toDouble(&isNumber);
			if (isNumber) break;// 이상적으로는 딸핵종, 하지만 딸핵종이 더 이상 없어 0이 할당되어 있을수도 있음

			daughterNucleus2 = columns[daughterNucleus1_index + 3];
			branchingFraction2 = columns[daughterNucleus1_index + 5];
			tmpNum = columns[daughterNucleus1_index + 6].toDouble(&isNumber);
			if (isNumber) break;// 이상적으로는 딸핵종, 하지만 딸핵종이 더 이상 없어 0이 할당되어 있을수도 있음

			daughterNucleus3 = columns[daughterNucleus1_index + 6];
			branchingFraction3 = columns[daughterNucleus1_index + 8];
			tmpNum = columns[daughterNucleus1_index + 9].toDouble(&isNumber);
			if (isNumber) break;// 이상적으로는 딸핵종, 하지만 딸핵종이 더 이상 없어 0이 할당되어 있을수도 있음

			daughterNucleus4 = columns[daughterNucleus1_index + 9];
			branchingFraction4 = columns[daughterNucleus1_index + 11];
			break; // Stop reading further as the data is found
		}
	}
	file.close();

	cursor.insertText("HalfLife: " + halfLife + "\n\n", normalFormat);
	cursor.insertText("Daughter Nuclides Information\n", boldFormat);
	cursor.insertText("Radionuclide   Fraction\n", boldSmallFormat);

	// Insert text into QTextEdit with padding
	if (!daughterNucleus1.isEmpty()) {
		QString padding(15 - daughterNucleus1.length(), ' ');
		cursor.insertText(daughterNucleus1 + padding + branchingFraction1 + "\n", normalFormat);
	}
	if (!daughterNucleus2.isEmpty()) {
		QString padding(15 - daughterNucleus2.length(), ' ');
		cursor.insertText(daughterNucleus2 + padding + branchingFraction2 + "\n", normalFormat);
	}
	if (!daughterNucleus3.isEmpty()) {
		QString padding(15 - daughterNucleus3.length(), ' ');
		cursor.insertText(daughterNucleus3 + padding + branchingFraction3 + "\n", normalFormat);
	}
	if (!daughterNucleus4.isEmpty()) {
		QString padding(15 - daughterNucleus4.length(), ' ');
		cursor.insertText(daughterNucleus4 + padding + branchingFraction4 + "\n", normalFormat);
	}

	cursor.insertText("\nRadiation Spectrum of the Entered Radionuclide\n", boldFormat);	
	cursor.insertText("Radiation      Fraction       Energy (MeV)\n", boldSmallFormat);
		
	QString RNspectrumFileName = ("./data/radioisotopes/" + labelRadionuclide->text() + ".txt");
	QFile RNspectrumFile(RNspectrumFileName);
	if (!RNspectrumFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		// Handle error: Unable to open file
		return;
	}

	QTextStream RNspectrumInput(&RNspectrumFile);
	QString RNspectrumLine;

	// Skip the first line
	if (!RNspectrumInput.atEnd()) {
		RNspectrumLine = RNspectrumInput.readLine();
	}
	
	while (!RNspectrumInput.atEnd()) {
		RNspectrumLine = RNspectrumInput.readLine();
		QStringList columns = RNspectrumLine.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);

		if (columns.size() >= 4) {
			QString radiation = columns[3];
			QString fraction = columns[1];
			QString energy = columns[2];

			QString padding1(15 - radiation.length(), ' ');
			QString padding2(15 - fraction.length(), ' ');
			cursor.insertText(radiation + padding1 + fraction + padding2 + energy + "\n", normalFormat);
		}
	}
	RNspectrumFile.close();

	cursor.insertText("\n*Alpha recoil nuclei and fission fragments are excluded.\n*PG, DG, and DB are radiations of spontaneous fission.\n", smallFormat);
	cursor.insertText("G: Gamma rays\n", verySmallFormat);
	cursor.insertText("PG: Prompt gamma rays\n", verySmallFormat);
	cursor.insertText("DG: Delayed gamma rays\n", verySmallFormat);
	cursor.insertText("X: X rays\n", verySmallFormat);
	cursor.insertText("AQ: Annihilation photons\n", verySmallFormat);
	cursor.insertText("B+: Beta-plus particles\n", verySmallFormat);
	cursor.insertText("B-: Beta-minus particles\n", verySmallFormat);
	cursor.insertText("DB: Delayed beta particles\n", verySmallFormat);
	cursor.insertText("IE: Internal conversion electrons\n", verySmallFormat);
	cursor.insertText("AE: Auger electrons\n", verySmallFormat);
	cursor.insertText("A: Alpha particles\n", verySmallFormat);
	cursor.insertText("N: Neutrons\n", verySmallFormat);

	cursor.movePosition(QTextCursor::Start);
	textEdit->setTextCursor(cursor);

	// Add the QTextEdit to the layout
	layout->addWidget(textEdit);

	// Set the layout to the dialog
	infoDialog->setLayout(layout);

	// Show the dialog
	infoDialog->exec(); // Use exec() for a modal dialog or show() for a non-modal dialog
}

void ExternalPointWidget::slot_EnergyspectrumFileLoad_sourceEP_ButtonClicked()
{
	QString filter;
	filter = tr("(*.*)");
	QString dir = QFileDialog::getOpenFileName(
		this,
		tr("Select a file to open"),
		"",
		filter);
		
	if (!dir.isEmpty())
	{
		sourceEP_EnergyspectrumFileNameText->setText(dir);
		sourceEP_EnergyspectrumFileNameText->setReadOnly(true);
		QFont font;
		font.setFamily("Arial");    // Font family
		font.setPointSize(panel->FontSizeScaling(7));      // Font size
		sourceEP_EnergyspectrumFileNameText->setFont(font);
	}
}
void ExternalPointWidget::slot_ESList_Add_sourceEP_ButtonClicked()
{
	// 지원하는 RI 리스트에 있는지 판별
	QString inputText = sourceEP_EnergyspectrumFileNameText->toPlainText();
	if (inputText.isEmpty()) {
		theApp.SetMessageBox("Energy spectrum file is empty!");
		return;
	}
	
	QListWidgetItem* item = new QListWidgetItem(ES_List_sourceEP_QListWidget);
	QWidget* widget = new QWidget;
	QHBoxLayout* hLayout = new QHBoxLayout;
	widget->setStyleSheet("background-color: white;");

	QString arg1 = QFileInfo(inputText).fileName();
	QLabel* label1 = new QLabel(arg1);
	label1->setFixedWidth(panel->Window_width * 0.025);
	label1->setFont(panel->font_D_LW2); // Set font for the label
	label1->setToolTip(arg1); // 전체 파일명 툴팁
	label1->setScaledContents(false);
	label1->setTextInteractionFlags(Qt::TextSelectableByMouse); // 선택 가능 (옵션)

	QString arg2 = QString::number(ES_Intensity_sourceEP_QLineEdit->text().toDouble(), 'e', 3);
	QLabel* label2 = new QLabel(arg2);
	label2->setFixedWidth(panel->Window_width * 0.025);
	label2->setFont(panel->font_D_LW2); // Set font for the label

	QString arg3 = PosX_sourceEP_QLineEdit->text().left(6);
	if (arg3 == "") arg3 = "0";
	QLabel* labelPosX = new QLabel(arg3);
	labelPosX->setFixedWidth(panel->Window_width * 0.025);
	labelPosX->setFont(panel->font_D_LW2); // Set font for the label

	QString arg4 = PosY_sourceEP_QLineEdit->text().left(6);
	if (arg4 == "") arg4 = "0";
	QLabel* labelPosY = new QLabel(arg4);
	labelPosY->setFixedWidth(panel->Window_width * 0.025);
	labelPosY->setFont(panel->font_D_LW2); // Set font for the label

	QString arg5 = PosZ_sourceEP_QLineEdit->text().left(6);
	if (arg5 == "") arg5 = "0";
	QLabel* labelPosZ = new QLabel(arg5);
	labelPosZ->setFixedWidth(panel->Window_width * 0.025);
	labelPosZ->setFont(panel->font_D_LW2); // Set font for the label

	ES_ListDelete_sourceEP_QButton.push_back(new QPushButton());
	if (List_sourceEP_MakingIndex >= ES_ListDelete_sourceEP_QButton.size()) theApp.SetMessageBox("m_sourceEP_ES_listDeleteButton Vector index error");
	ES_ListDelete_sourceEP_QButton[List_sourceEP_MakingIndex]->setCheckable(false);
	ES_ListDelete_sourceEP_QButton[List_sourceEP_MakingIndex]->setStyleSheet(DialogStyle::MENU_BTN13);
	ES_ListDelete_sourceEP_QButton[List_sourceEP_MakingIndex]->setFont(panel->font_D_BTN13);
	ES_ListDelete_sourceEP_QButton[List_sourceEP_MakingIndex]->setFixedSize(panel->Window_width * 0.008, panel->Window_height * 0.012);
	ES_ListDelete_sourceEP_QButton[List_sourceEP_MakingIndex]->setIcon(QIcon("./data/image/delete.png"));
	ES_ListDelete_sourceEP_QButton[List_sourceEP_MakingIndex]->setObjectName(QString::number(List_sourceEP_MakingIndex));

	ES_ListInfo_sourceEP_QButton.push_back(new QPushButton());
	if (List_sourceEP_MakingIndex >= ES_ListInfo_sourceEP_QButton.size()) theApp.SetMessageBox("m_sourceEP_ES_listInfoButton Vector index error");
	ES_ListInfo_sourceEP_QButton[List_sourceEP_MakingIndex]->setCheckable(false);
	ES_ListInfo_sourceEP_QButton[List_sourceEP_MakingIndex]->setStyleSheet(DialogStyle::MENU_BTN13);
	ES_ListInfo_sourceEP_QButton[List_sourceEP_MakingIndex]->setFont(panel->font_D_BTN13);
	ES_ListInfo_sourceEP_QButton[List_sourceEP_MakingIndex]->setFixedSize(panel->Window_width * 0.008, panel->Window_height * 0.012);
	ES_ListInfo_sourceEP_QButton[List_sourceEP_MakingIndex]->setIcon(QIcon("./data/image/info.png"));
	ES_ListInfo_sourceEP_QButton[List_sourceEP_MakingIndex]->setObjectName(QString::number(List_sourceEP_MakingIndex));

	hLayout->addWidget(label1);
	hLayout->addWidget(label2);
	hLayout->addWidget(labelPosX);
	hLayout->addWidget(labelPosY);
	hLayout->addWidget(labelPosZ);
	hLayout->addWidget(ES_ListDelete_sourceEP_QButton[List_sourceEP_MakingIndex]);
	hLayout->addWidget(ES_ListInfo_sourceEP_QButton[List_sourceEP_MakingIndex]);
	widget->setLayout(hLayout);

	ES_List_sourceEP_QListWidget->setItemWidget(item, widget);
	item->setSizeHint(widget->sizeHint());

	// Connect signals of the new buttons
	connect(ES_ListDelete_sourceEP_QButton[List_sourceEP_MakingIndex], SIGNAL(clicked()), this, SLOT(slot_ESList_Delete_sourceEP_ButtonClicked())); connect(ES_ListDelete_sourceEP_QButton[List_sourceEP_MakingIndex], SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
	connect(ES_ListInfo_sourceEP_QButton[List_sourceEP_MakingIndex], SIGNAL(clicked()), this, SLOT(slot_ESList_Info_sourceEP_ButtonClicked())); connect(ES_ListInfo_sourceEP_QButton[List_sourceEP_MakingIndex], SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));

	// Generate actor
	double center[3] = { 0.0, };
	center[0] = PosX_sourceEP_QLineEdit->text().toFloat();
	center[1] = PosY_sourceEP_QLineEdit->text().toFloat();
	center[2] = PosZ_sourceEP_QLineEdit->text().toFloat();

	theApp.sourceObjects->GenerateSourceActor_sourcePS(center);

	// Set Global Variables
	std::map<int, QString> tmp_map;
	tmp_map[0] = "True";
	tmp_map[1] = PosX_sourceEP_QLineEdit->text(); if (tmp_map[1] == "") tmp_map[1] = "0";
	tmp_map[2] = PosY_sourceEP_QLineEdit->text(); if (tmp_map[2] == "") tmp_map[2] = "0";
	tmp_map[3] = PosZ_sourceEP_QLineEdit->text(); if (tmp_map[3] == "") tmp_map[3] = "0";
	tmp_map[4] = inputText;	
	tmp_map[5] = ES_Intensity_sourceEP_QLineEdit->text(); if (tmp_map[5] == "") tmp_map[5] = "0";
	ListInfo_sourceEP.push_back(tmp_map);
	List_sourceEP_MakingIndex++;

	// Set panel info 
	PosX_sourceEP_QLineEdit->clear();
	PosY_sourceEP_QLineEdit->clear();
	PosZ_sourceEP_QLineEdit->clear();
	ES_Intensity_sourceEP_QLineEdit->clear();
	sourceEP_EnergyspectrumFileNameText->clear();	
}
void ExternalPointWidget::slot_ESList_Delete_sourceEP_ButtonClicked()
{
	// Button의 Index 찾기
	QPushButton* buttonSender = qobject_cast<QPushButton*>(sender());
	QString buttonText = buttonSender->objectName();

	// The first part is the ID
	int EnersySpectrumIndex = buttonText.toDouble();
	int row = ES_List_sourceEP_QListWidget->row(ES_List_sourceEP_QListWidget->item(EnersySpectrumIndex));
	QListWidgetItem* toRemove = ES_List_sourceEP_QListWidget->takeItem(row);
	delete toRemove; // Delete the item to free memory

	if (EnersySpectrumIndex >= 0 && EnersySpectrumIndex < ListInfo_sourceEP.size())
	{
		ListInfo_sourceEP.erase(ListInfo_sourceEP.begin() + EnersySpectrumIndex);
		ES_ListDelete_sourceEP_QButton.erase(ES_ListDelete_sourceEP_QButton.begin() + EnersySpectrumIndex);
		ES_ListInfo_sourceEP_QButton.erase(ES_ListInfo_sourceEP_QButton.begin() + EnersySpectrumIndex);
		// Actor delete
		theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(theApp.SourcePanelActor_Position[EnersySpectrumIndex]);
		theApp.m_pVTKWidget->renderWindow()->Render();
		theApp.SourcePanelActor_Position.erase(theApp.SourcePanelActor_Position.begin() + EnersySpectrumIndex);
	}
	else {
		theApp.SetMessageBox("sourceEP_info Vecetor index error");
	}

	for (int index = 0; index < ES_ListDelete_sourceEP_QButton.size(); index++)
	{
		ES_ListDelete_sourceEP_QButton[index]->setObjectName(QString::number(index));
		ES_ListInfo_sourceEP_QButton[index]->setObjectName(QString::number(index));
	}
	List_sourceEP_MakingIndex--; // listwidget은 vector 형태로 동작해야 함(row의 item이 실제 몇 행인지로 결정됨)	
}
void ExternalPointWidget::slot_ESList_Info_sourceEP_ButtonClicked()
{
	// Button의 Index 찾기
	QPushButton* buttonSender = qobject_cast<QPushButton*>(sender());
	QString buttonText = buttonSender->objectName();

	QStringList list_button = buttonText.split("_");

	// The first part is the ID
	int EnergySpectrumIndex = list_button[0].toInt();

	QListWidgetItem* item = ES_List_sourceEP_QListWidget->item(EnergySpectrumIndex);
	if (!item) {
		// Handle error: item not found
		return;
	}

	QWidget* widget = ES_List_sourceEP_QListWidget->itemWidget(item);
	QHBoxLayout* hLayout = qobject_cast<QHBoxLayout*>(widget->layout());
	if (!hLayout) {
		// Handle error: layout not found
		return;
	}

	// Assuming the order of labels in the layout is known
	QLabel* labelEnergySpectrumFile = qobject_cast<QLabel*>(hLayout->itemAt(0)->widget());
	QLabel* labelIntensity = qobject_cast<QLabel*>(hLayout->itemAt(1)->widget());
	QLabel* labelX = qobject_cast<QLabel*>(hLayout->itemAt(2)->widget());
	QLabel* labelY = qobject_cast<QLabel*>(hLayout->itemAt(3)->widget());
	QLabel* labelZ = qobject_cast<QLabel*>(hLayout->itemAt(4)->widget());

	// Create a new dialog
	QDialog* infoDialog = new QDialog(this);
	infoDialog->setStyleSheet("background-color: white;");
	infoDialog->resize(panel->Window_width * 0.3, panel->Window_height * 0.55);
	infoDialog->setWindowTitle("Radionuclide Information");

	// Create a layout and a label for the dialog
	QVBoxLayout* layout = new QVBoxLayout(infoDialog);
	// Create a QTextEdit for the dialog
	QTextEdit* textEdit = new QTextEdit(infoDialog);
	textEdit->setReadOnly(true); // Make it read-only if editing is not required

	// Create a QTextCursor for inserting formatted text
	QTextCursor cursor = textEdit->textCursor();

	// Define different text formats
	QTextCharFormat boldFormat;
	boldFormat.setFontFamily("Courier New"); // This is monospaced font
	boldFormat.setFontWeight(QFont::Bold);
	boldFormat.setFontPointSize(14);

	QTextCharFormat boldMediumFormat;
	boldMediumFormat.setFontFamily("Courier New");
	boldMediumFormat.setFontWeight(QFont::Bold);
	boldMediumFormat.setFontPointSize(12);

	QTextCharFormat boldSmallFormat;
	boldSmallFormat.setFontFamily("Courier New");
	boldSmallFormat.setFontWeight(QFont::Bold);
	boldSmallFormat.setFontPointSize(10);

	QTextCharFormat normalFormat;
	normalFormat.setFontFamily("Courier New");
	normalFormat.setFontPointSize(10);

	QTextCharFormat smallFormat;
	smallFormat.setFontFamily("Courier New");
	smallFormat.setFontPointSize(8);

	QTextCharFormat verySmallFormat;
	verySmallFormat.setFontFamily("Courier New");
	verySmallFormat.setFontPointSize(6);

	// Insert formatted text into QTextEdit	
	cursor.insertText("EnergySpectrum Information\n", boldFormat);
	cursor.insertText("FileName: " + labelEnergySpectrumFile->text() + "\n", normalFormat);
	cursor.insertText("Intensity (#/s): " + labelIntensity->text() + "\n", normalFormat);
	cursor.insertText("PosX (cm): " + labelX->text() + "\n", normalFormat);
	cursor.insertText("PosY (cm): " + labelY->text() + "\n", normalFormat);
	cursor.insertText("PosZ (cm): " + labelZ->text() + "\n", normalFormat);

	cursor.insertText("\nRadiation Spectrum of the Entered File\n", boldFormat);
	cursor.insertText("Radiation      Fraction       Energy (MeV)\n", boldSmallFormat);

	QString ESFileName = ListInfo_sourceEP[EnergySpectrumIndex][4];
	QFile ESFile(ESFileName);
	if (!ESFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		// Handle error: Unable to open file
		return;
	}

	QTextStream ESInput(&ESFile);
	while (!ESInput.atEnd()) {
		QString ESLine = ESInput.readLine();
		QStringList columns = ESLine.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);

		if (columns.size() >= 3) {
			QString radiation = columns[0];
			QString fraction = columns[1];
			QString energy = columns[2];					

			QString padding1(15 - radiation.length(), ' ');
			QString padding2(15 - fraction.length(), ' ');
			cursor.insertText(radiation + padding1 + fraction + padding2 + energy + "\n", normalFormat);
		}
	}
	ESFile.close();

	cursor.movePosition(QTextCursor::Start);
	textEdit->setTextCursor(cursor);

	// Add the QTextEdit to the layout
	layout->addWidget(textEdit);

	// Set the layout to the dialog
	infoDialog->setLayout(layout);

	// Show the dialog
	infoDialog->exec(); // Use exec() for a modal dialog or show() for a non-modal dialog
}


///////////////////////////////////////////////////////////////////////////
// FloorDiskWidget Implementation
///////////////////////////////////////////////////////////////////////////

FloorDiskWidget::FloorDiskWidget(QWidget* parent)
    : ParentT(parent)
{
}

FloorDiskWidget::~FloorDiskWidget()
{
}

bool FloorDiskWidget::initialize()
{
	if (!ParentT::initialize()) return false;

	// panel->Window_width 등 부모 자원에 접근하기 위해 panel 포인터 사용
	QVBoxLayout* subLayout = new QVBoxLayout;
	subLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.005, panel->Window_width * 0.003, panel->Window_height * 0.005);
	subLayout->setSpacing(panel->Window_width * 0.003);
	subLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

	{
		QVBoxLayout* vLayout = new QVBoxLayout;
		vLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.005, panel->Window_width * 0.003, panel->Window_height * 0.005);
		vLayout->setSpacing(panel->Window_width * 0.003);
		vLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		// [Visualization]
		{
			QHBoxLayout* hLayout = new QHBoxLayout;
			hLayout->setSpacing(panel->Window_width * 0.005);
			hLayout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

			QLabel* labelTitle = new QLabel;
			labelTitle->setFixedSize(panel->Window_width * 0.104, panel->Window_height * 0.028);
			labelTitle->setStyleSheet(DialogStyle::DATA_LABEL);
			labelTitle->setFont(panel->font_D_LBL1);
			labelTitle->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
			labelTitle->setText(QString::fromLocal8Bit("Visualization"));

			m_FloorSourceVisualizationButton = new QPushButton(QString::fromLocal8Bit("On"));
			panel->Buttons_FunctionPanelRight.append(m_FloorSourceVisualizationButton);
			m_FloorSourceVisualizationButton->setFixedSize(panel->Window_width * 0.05, panel->Window_height * 0.028);
			m_FloorSourceVisualizationButton->setCheckable(true);
			m_FloorSourceVisualizationButton->setChecked(true);
			m_FloorSourceVisualizationButton->setStyleSheet(DialogStyle::MENU_BTN3);
			m_FloorSourceVisualizationButton->setFont(panel->font_D_BTN3);

			connect(m_FloorSourceVisualizationButton, SIGNAL(clicked()), this, SLOT(slot_FloorSourceVisualization_ButtonClicked())); 
			connect(m_FloorSourceVisualizationButton, SIGNAL(clicked()), panel, SLOT(slot_allButtonClicked()));

			hLayout->addWidget(labelTitle);
			hLayout->addStretch();
			hLayout->addWidget(m_FloorSourceVisualizationButton);

			vLayout->addLayout(hLayout);
		}

		// [Position XYZ]
		{// x position label, lineedit widgets
			QHBoxLayout* hLayout = new QHBoxLayout;
			hLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.005, panel->Window_width * 0.003, 0);
			hLayout->setSpacing(panel->Window_width * 0.003);
			hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

			QLabel* label0 = new QLabel;
			label0->setFixedSize(panel->Window_width * 0.058, panel->Window_height * 0.028);
			label0->setStyleSheet(DialogStyle::DATA_LABEL);
			label0->setFont(panel->font_D_LBL1);
			label0->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
			label0->setText(QString::fromLocal8Bit("Position XYZ:"));

			PosX_SourceFD_QLineEdit = new QLineEdit;
			PosX_SourceFD_QLineEdit->setFixedSize(panel->Window_width * 0.033, panel->Window_height * 0.028);
			PosX_SourceFD_QLineEdit->setAlignment(Qt::AlignCenter);
			PosX_SourceFD_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME5);
			PosX_SourceFD_QLineEdit->setFont(panel->font_D_LE5);
			PosX_SourceFD_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(cm)"));

			PosY_SourceFD_QLineEdit = new QLineEdit;
			PosY_SourceFD_QLineEdit->setFixedSize(panel->Window_width * 0.033, panel->Window_height * 0.028);
			PosY_SourceFD_QLineEdit->setAlignment(Qt::AlignCenter);
			PosY_SourceFD_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME5);
			PosY_SourceFD_QLineEdit->setFont(panel->font_D_LE5);
			PosY_SourceFD_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(cm)"));

			PosZ_SourceFD_QLineEdit = new QLineEdit;
			PosZ_SourceFD_QLineEdit->setFixedSize(panel->Window_width * 0.033, panel->Window_height * 0.028);
			PosZ_SourceFD_QLineEdit->setAlignment(Qt::AlignCenter);
			PosZ_SourceFD_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME5);
			PosZ_SourceFD_QLineEdit->setFont(panel->font_D_LE5);
			PosZ_SourceFD_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(cm)"));

			hLayout->addWidget(label0);
			hLayout->addStretch();
			hLayout->addWidget(PosX_SourceFD_QLineEdit);
			hLayout->addWidget(PosY_SourceFD_QLineEdit);
			hLayout->addWidget(PosZ_SourceFD_QLineEdit);

			vLayout->addLayout(hLayout);
		}

		// [Radius]
		{// z position label, lineedit widgets
			QHBoxLayout* hLayout = new QHBoxLayout;
			hLayout->setContentsMargins(panel->Window_width * 0.013, 0, panel->Window_width * 0.003, 0);
			hLayout->setSpacing(panel->Window_width * 0.003);
			hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

			QLabel* label0 = new QLabel;
			label0->setFixedSize(panel->Window_width * 0.042, panel->Window_height * 0.028);
			label0->setStyleSheet(DialogStyle::DATA_LABEL);
			label0->setFont(panel->font_D_LBL1);
			label0->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
			label0->setText(QString::fromLocal8Bit("Radius "));

			Radius_sourceFD_QLineEdit = new QLineEdit;
			Radius_sourceFD_QLineEdit->setFixedSize(panel->Window_width * 0.104, panel->Window_height * 0.028);
			Radius_sourceFD_QLineEdit->setAlignment(Qt::AlignCenter);
			Radius_sourceFD_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
			Radius_sourceFD_QLineEdit->setFont(panel->font_D_LE1);
			Radius_sourceFD_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("Disk radius (cm)"));

			hLayout->addWidget(label0);
			hLayout->addStretch();
			hLayout->addWidget(Radius_sourceFD_QLineEdit);

			vLayout->addLayout(hLayout);
		}

		// [Buttons: Default & Update]
		{//
			QHBoxLayout* hLayout = new QHBoxLayout;
			hLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.009, panel->Window_width * 0.003, panel->Window_height * 0.005);
			hLayout->setSpacing(panel->Window_width * 0.003);
			hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

			m_FloorSetDefaultGeom = new QPushButton(QString::fromLocal8Bit("Default\nposition"));
			panel->Buttons_FunctionPanelRight.append(m_FloorSetDefaultGeom);
			m_FloorSetDefaultGeom->setFixedSize(panel->Window_width * 0.050, panel->Window_height * 0.036);
			m_FloorSetDefaultGeom->setCheckable(false);
			m_FloorSetDefaultGeom->setStyleSheet(DialogStyle::MENU_BTN2);
			m_FloorSetDefaultGeom->setFont(panel->font_D_BTN2);
			m_FloorSetDefaultGeom->setChecked(false);

			m_FloorUpdateButton = new QPushButton(QString::fromLocal8Bit("Update"));
			panel->Buttons_FunctionPanelRight.append(m_FloorUpdateButton);
			m_FloorUpdateButton->setFixedSize(panel->Window_width * 0.050, panel->Window_height * 0.036);
			m_FloorUpdateButton->setCheckable(false);
			m_FloorUpdateButton->setStyleSheet(DialogStyle::MENU_BTN2);
			m_FloorUpdateButton->setFont(panel->font_D_BTN2);
			m_FloorUpdateButton->setChecked(false);

			connect(m_FloorSetDefaultGeom, SIGNAL(clicked()), this, SLOT(slot_FloorSetDefaultGeom_ButtonClicked())); 
			connect(m_FloorSetDefaultGeom, SIGNAL(clicked()), panel, SLOT(slot_allButtonClicked()));
			
			connect(m_FloorUpdateButton, SIGNAL(clicked()), this, SLOT(slot_FloorUpdate_ButtonClicked())); 
			connect(m_FloorUpdateButton, SIGNAL(clicked()), panel, SLOT(slot_allButtonClicked()));

			hLayout->addStretch();
			hLayout->addWidget(m_FloorSetDefaultGeom);
			hLayout->addWidget(m_FloorUpdateButton);

			vLayout->addLayout(hLayout);
		}

		// [Radio Buttons: RI vs ES]
		{
			QHBoxLayout* hLayout = new QHBoxLayout;
			hLayout->setSpacing(panel->Window_width * 0.010);
			hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
			hLayout->setContentsMargins(panel->Window_width * 0.005, panel->Window_height * 0.009, 0, 0);
			
			// 버튼생성
			RI_Select_sourceFD_QRadioButton = new QRadioButton(QString::fromLocal8Bit("Radionuclide"));
			RI_Select_sourceFD_QRadioButton->setStyleSheet(DialogStyle::RADIOBUTTON_WIDGET);
			RI_Select_sourceFD_QRadioButton->setFont(panel->font_D_RB2);
			RI_Select_sourceFD_QRadioButton->setFixedSize(panel->Window_width * 0.081, panel->Window_height * 0.028);
			RI_Select_sourceFD_QRadioButton->setChecked(true);
			// 버튼생성
			ES_Select_sourceFD_QRadioButton = new QRadioButton(QString::fromLocal8Bit("Energy spectrum"));
			ES_Select_sourceFD_QRadioButton->setStyleSheet(DialogStyle::RADIOBUTTON_WIDGET);
			ES_Select_sourceFD_QRadioButton->setFont(panel->font_D_RB2);
			ES_Select_sourceFD_QRadioButton->setFixedSize(panel->Window_width * 0.081, panel->Window_height * 0.028);
			ES_Select_sourceFD_QRadioButton->setChecked(false);

			connect(RI_Select_sourceFD_QRadioButton, SIGNAL(clicked()), this, SLOT(slot_sourceFD_RadionuclideSelect_RadioButtonClicked())); 
			connect(RI_Select_sourceFD_QRadioButton, SIGNAL(clicked()), panel, SLOT(slot_allButtonClicked()));
			
			connect(ES_Select_sourceFD_QRadioButton, SIGNAL(clicked()), this, SLOT(slot_sourceFD_EnergySpectrumSelect_RadioButtonClicked())); 
			connect(ES_Select_sourceFD_QRadioButton, SIGNAL(clicked()), panel, SLOT(slot_allButtonClicked()));

			hLayout->addWidget(RI_Select_sourceFD_QRadioButton);
			hLayout->addWidget(ES_Select_sourceFD_QRadioButton);

			vLayout->addLayout(hLayout);
		}

		// [Radionuclide UI]
		{// Radioisotope
			QHBoxLayout* hLayout = new QHBoxLayout;
			hLayout->setContentsMargins(panel->Window_width * 0.003, 0, panel->Window_width * 0.003, 0);
			hLayout->setSpacing(panel->Window_width * 0.003);
			hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

			RI_Title_sourceFD_QLabel = new QLabel;
			RI_Title_sourceFD_QLabel->setFixedSize(panel->Window_width * 0.068, panel->Window_height * 0.028);
			RI_Title_sourceFD_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
			RI_Title_sourceFD_QLabel->setFont(panel->font_D_LBL1);
			RI_Title_sourceFD_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
			RI_Title_sourceFD_QLabel->setText(QString::fromLocal8Bit("Radionuclide"));

			RI_Radionuclide_sourceFD_QLineEdit = new QLineEdit;
			RI_Radionuclide_sourceFD_QLineEdit->setFixedSize(panel->Window_width * 0.104, panel->Window_height * 0.028);
			RI_Radionuclide_sourceFD_QLineEdit->setAlignment(Qt::AlignCenter);
			RI_Radionuclide_sourceFD_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
			RI_Radionuclide_sourceFD_QLineEdit->setFont(panel->font_D_LE1);
			RI_Radionuclide_sourceFD_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("Enter radionuclide"));

			// panel->RIsourceList는 Panel에 있다고 가정
			QCompleter *completer = new QCompleter(panel->RIsourceList, this);
			completer->setCaseSensitivity(Qt::CaseInsensitive);
			RI_Radionuclide_sourceFD_QLineEdit->setCompleter(completer);

			hLayout->addWidget(RI_Title_sourceFD_QLabel);
			hLayout->addStretch();
			hLayout->addWidget(RI_Radionuclide_sourceFD_QLineEdit);

			vLayout->addLayout(hLayout);
		}

		{// Activity
			QHBoxLayout* hLayout = new QHBoxLayout;
			hLayout->setContentsMargins(panel->Window_width * 0.003, 0, panel->Window_width * 0.003, 0);
			hLayout->setSpacing(panel->Window_width * 0.003);
			hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

			RI_Activity_sourceFD_QLabel = new QLabel;
			RI_Activity_sourceFD_QLabel->setFixedSize(panel->Window_width * 0.068, panel->Window_height * 0.028);
			RI_Activity_sourceFD_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
			RI_Activity_sourceFD_QLabel->setFont(panel->font_D_LBL1);
			RI_Activity_sourceFD_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
			RI_Activity_sourceFD_QLabel->setText(QString::fromLocal8Bit("Activity"));

			RI_Activity_sourceFD_QLineEdit = new QLineEdit;
			RI_Activity_sourceFD_QLineEdit->setFixedSize(panel->Window_width * 0.104, panel->Window_height * 0.028);
			RI_Activity_sourceFD_QLineEdit->setAlignment(Qt::AlignCenter);
			RI_Activity_sourceFD_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
			RI_Activity_sourceFD_QLineEdit->setFont(panel->font_D_LE1);
			RI_Activity_sourceFD_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("Bq/cm2"));

			hLayout->addWidget(RI_Activity_sourceFD_QLabel);
			hLayout->addStretch();
			hLayout->addWidget(RI_Activity_sourceFD_QLineEdit);

			vLayout->addLayout(hLayout);
		}

		{// Radionuclide Add
			QHBoxLayout* hLayout = new QHBoxLayout;
			hLayout->setContentsMargins(panel->Window_width * 0.003, 0, panel->Window_width * 0.003, 0);
			hLayout->setSpacing(panel->Window_width * 0.003);
			hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

			RI_List_sourceFD_QLabel = new QLabel;
			RI_List_sourceFD_QLabel->setFixedSize(panel->Window_width * 0.088, panel->Window_height * 0.028);
			RI_List_sourceFD_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
			RI_List_sourceFD_QLabel->setFont(panel->font_D_LBL1);
			RI_List_sourceFD_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
			RI_List_sourceFD_QLabel->setText(QString::fromLocal8Bit("Entered sources"));

			sourceFD_RadionuclideAddButton = new QPushButton(QString::fromLocal8Bit("Add"));
			panel->Buttons_FunctionPanelRight.append(sourceFD_RadionuclideAddButton);
			sourceFD_RadionuclideAddButton->setFixedSize(panel->Window_width * 0.042, panel->Window_height * 0.037);
			sourceFD_RadionuclideAddButton->setCheckable(false);
			sourceFD_RadionuclideAddButton->setStyleSheet(DialogStyle::MENU_BTN2);
			sourceFD_RadionuclideAddButton->setFont(panel->font_D_BTN2);
			sourceFD_RadionuclideAddButton->setChecked(false);

			connect(sourceFD_RadionuclideAddButton, SIGNAL(clicked()), this, SLOT(slot_RIList_Add_sourceFD_ButtonClicked())); 
			connect(sourceFD_RadionuclideAddButton, SIGNAL(clicked()), panel, SLOT(slot_allButtonClicked()));

			hLayout->addWidget(RI_List_sourceFD_QLabel);
			hLayout->addStretch();
			hLayout->addWidget(sourceFD_RadionuclideAddButton);

			vLayout->addLayout(hLayout);
		}

		{// Radionulcide list
			QHBoxLayout* hLayout = new QHBoxLayout;
			hLayout->setContentsMargins(panel->Window_width * 0.003, 0, panel->Window_width * 0.003, 0);
			hLayout->setSpacing(panel->Window_width * 0.003);
			hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

			RI_List_sourceFD_QListWidget = new QListWidget;
			
			RI_List_sourceFD_QListWidget->setStyleSheet("QListWidget { border-radius: 15px; background-color: white; }");
			RI_List_sourceFD_QListWidget->setFixedSize(panel->Window_width * 0.175, panel->Window_height * 0.155);
			RI_List_sourceFD_QListWidget->setFont(panel->font_D_LW1);

			hLayout->addWidget(RI_List_sourceFD_QListWidget);

			vLayout->addLayout(hLayout);
		}

// [User-specific && Load file]
		{
			QHBoxLayout* hLayout = new QHBoxLayout;
			hLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.010, panel->Window_width * 0.003, 0);
			hLayout->setSpacing(panel->Window_width * 0.003);
			hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

			ES_Title_sourceFD_QLabel = new QLabel;
			ES_Title_sourceFD_QLabel->setFixedSize(panel->Window_width * 0.095, panel->Window_height * 0.028);
			ES_Title_sourceFD_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
			ES_Title_sourceFD_QLabel->setFont(panel->font_D_LBL1);
			ES_Title_sourceFD_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
			ES_Title_sourceFD_QLabel->setText(QString::fromLocal8Bit("Energy spectrum file"));
			ES_Title_sourceFD_QLabel->hide();

			sourceFD_EnergyspectrumFileLoadButton = new QPushButton(QString::fromLocal8Bit("Load file"));
			panel->Buttons_FunctionPanelRight.append(sourceFD_EnergyspectrumFileLoadButton);
			sourceFD_EnergyspectrumFileLoadButton->setFixedSize(panel->Window_width * 0.067, panel->Window_height * 0.028);
			sourceFD_EnergyspectrumFileLoadButton->setStyleSheet(DialogStyle::MENU_BTN11);
			sourceFD_EnergyspectrumFileLoadButton->setFont(panel->font_D_BTN11);
			sourceFD_EnergyspectrumFileLoadButton->hide();

			// 기능은 this, 전체 갱신은 panel
			connect(sourceFD_EnergyspectrumFileLoadButton, SIGNAL(clicked()), this, SLOT(slot_sourceFD_EnergyspectrumFileLoad_ButtonClicked())); 
			connect(sourceFD_EnergyspectrumFileLoadButton, SIGNAL(clicked()), panel, SLOT(slot_allButtonClicked()));

			hLayout->addWidget(ES_Title_sourceFD_QLabel);
			hLayout->addWidget(sourceFD_EnergyspectrumFileLoadButton);

			vLayout->addLayout(hLayout);
		}

		// [File Name Display]
		{
			QHBoxLayout* hLayout = new QHBoxLayout;
			hLayout->setContentsMargins(panel->Window_width * 0.003, 0, panel->Window_width * 0.003, 0);
			hLayout->setSpacing(panel->Window_width * 0.005);
			hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

			ES_FileName_sourceFD_QLabel = new QLabel;
			ES_FileName_sourceFD_QLabel->setFixedSize(panel->Window_width * 0.068, panel->Window_height * 0.028);
			ES_FileName_sourceFD_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
			ES_FileName_sourceFD_QLabel->setFont(panel->font_D_LBL1);
			ES_FileName_sourceFD_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
			ES_FileName_sourceFD_QLabel->setText(QString::fromLocal8Bit("File name"));
			ES_FileName_sourceFD_QLabel->hide();

			sourceFD_EnergyspectrumFileNameText = new QTextEdit;
			sourceFD_EnergyspectrumFileNameText->setFixedSize(panel->Window_width * 0.104, panel->Window_height * 0.056);
			sourceFD_EnergyspectrumFileNameText->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
			sourceFD_EnergyspectrumFileNameText->setStyleSheet("QTextEdit { background-color: rgb(255, 255, 255) }");
			sourceFD_EnergyspectrumFileNameText->setFont(panel->font_D_MAT);
			sourceFD_EnergyspectrumFileNameText->setPlaceholderText(QString::fromLocal8Bit("empty"));
			sourceFD_EnergyspectrumFileNameText->hide();

			hLayout->addWidget(ES_FileName_sourceFD_QLabel);
			hLayout->addStretch();
			hLayout->addWidget(sourceFD_EnergyspectrumFileNameText);

			vLayout->addLayout(hLayout);
		}

		// [Intensity]
		{
			QHBoxLayout* hLayout = new QHBoxLayout;
			hLayout->setContentsMargins(panel->Window_width * 0.003, 0, panel->Window_width * 0.003, 0);
			hLayout->setSpacing(panel->Window_width * 0.003);
			hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

			ES_Intensity_sourceFD_QLabel = new QLabel;
			ES_Intensity_sourceFD_QLabel->setFixedSize(panel->Window_width * 0.068, panel->Window_height * 0.028);
			ES_Intensity_sourceFD_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
			ES_Intensity_sourceFD_QLabel->setFont(panel->font_D_LBL1);
			ES_Intensity_sourceFD_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
			ES_Intensity_sourceFD_QLabel->setText(QString::fromLocal8Bit("Intensity"));
			ES_Intensity_sourceFD_QLabel->hide();

			ES_Intensity_sourceFD_QLineEdit = new QLineEdit;
			ES_Intensity_sourceFD_QLineEdit->setFixedSize(panel->Window_width * 0.104, panel->Window_height * 0.028);
			ES_Intensity_sourceFD_QLineEdit->setAlignment(Qt::AlignCenter);
			ES_Intensity_sourceFD_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
			ES_Intensity_sourceFD_QLineEdit->setFont(panel->font_D_LE1);
			ES_Intensity_sourceFD_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("particles/s/cm2"));
			ES_Intensity_sourceFD_QLineEdit->hide();

			hLayout->addWidget(ES_Intensity_sourceFD_QLabel);
			hLayout->addStretch();
			hLayout->addWidget(ES_Intensity_sourceFD_QLineEdit);

			vLayout->addLayout(hLayout);
		}

		// [Energy Spectrum Add Button]
		{
			QHBoxLayout* hLayout = new QHBoxLayout;
			hLayout->setContentsMargins(panel->Window_width * 0.003, 0, panel->Window_width * 0.003, 0);
			hLayout->setSpacing(panel->Window_width * 0.003);
			hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

			ES_List_sourceFD_QLabel = new QLabel;
			ES_List_sourceFD_QLabel->setFixedSize(panel->Window_width * 0.088, panel->Window_height * 0.028);
			ES_List_sourceFD_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
			ES_List_sourceFD_QLabel->setFont(panel->font_D_LBL1);
			ES_List_sourceFD_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
			ES_List_sourceFD_QLabel->setText(QString::fromLocal8Bit("Entered sources"));
			ES_List_sourceFD_QLabel->hide();

			// 버튼생성
			sourceFD_EnergySpectrumAddButton = new QPushButton(QString::fromLocal8Bit("Add"));
			panel->Buttons_FunctionPanelRight.append(sourceFD_EnergySpectrumAddButton);
			sourceFD_EnergySpectrumAddButton->setFixedSize(panel->Window_width * 0.042, panel->Window_height * 0.037);
			sourceFD_EnergySpectrumAddButton->setCheckable(false);
			sourceFD_EnergySpectrumAddButton->setStyleSheet(DialogStyle::MENU_BTN2);
			sourceFD_EnergySpectrumAddButton->setFont(panel->font_D_BTN2);
			sourceFD_EnergySpectrumAddButton->setChecked(false);
			sourceFD_EnergySpectrumAddButton->hide();

			// 기능은 this, 전체 갱신은 panel
			connect(sourceFD_EnergySpectrumAddButton, SIGNAL(clicked()), this, SLOT(slot_ESList_Add_sourceFD_ButtonClicked())); 
			connect(sourceFD_EnergySpectrumAddButton, SIGNAL(clicked()), panel, SLOT(slot_allButtonClicked()));

			hLayout->addWidget(ES_List_sourceFD_QLabel);
			hLayout->addStretch();
			hLayout->addWidget(sourceFD_EnergySpectrumAddButton);

			vLayout->addLayout(hLayout);
		}

		// [Energy Spectrum list]
		{
			QHBoxLayout* hLayout = new QHBoxLayout;
			hLayout->setContentsMargins(panel->Window_width * 0.003, 0, panel->Window_width * 0.003, 0);
			hLayout->setSpacing(panel->Window_width * 0.003);
			hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

			ES_List_sourceFD_QListWidget = new QListWidget;

			ES_List_sourceFD_QListWidget->setStyleSheet("QListWidget { border-radius: 15px; background-color: white; }");
			ES_List_sourceFD_QListWidget->setFixedSize(panel->Window_width * 0.175, panel->Window_height * 0.155); 
			ES_List_sourceFD_QListWidget->setFont(panel->font_D_LW1);
			ES_List_sourceFD_QListWidget->hide();

			hLayout->addWidget(ES_List_sourceFD_QListWidget);

			vLayout->addLayout(hLayout);
		}
		
		subLayout->addLayout(vLayout);
	}
	
	this->setLayout(subLayout);
	return true;
}

void FloorDiskWidget::slot_FloorSourceVisualization_ButtonClicked()
{
	if (m_FloorSourceVisualizationButton->isChecked())
	{
		m_FloorSourceVisualizationButton->setText("On");
		for (auto itr : theApp.SourcePanelActor_Position) theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(itr);
	}
	else
	{
		m_FloorSourceVisualizationButton->setText("Off");
		for (auto itr : theApp.SourcePanelActor_Position) theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(itr);
	}
	theApp.m_pVTKWidget->renderWindow()->Render();
}
void FloorDiskWidget::slot_FloorSetDefaultGeom_ButtonClicked()
{
	if (panel->m_Phantom_SequenceVector.size() == 0)
	{
		theApp.SetMessageBox("At least one phantom is required!");
		return;
	}
	// 현재까지 설치된 팬텀의 bound box 계산
	int phantomID_zpos_min = 0;
	std::vector<double> phantombox_bounds = { DBL_MAX, -DBL_MAX, DBL_MAX, -DBL_MAX, DBL_MAX, -DBL_MAX };
	for (auto itr_phantomID : panel->m_Phantom_SequenceVector)
	{
		double* bounds = theApp.PhantomPanelActor[itr_phantomID]->GetBounds();
		if (bounds[0] < phantombox_bounds[0]) phantombox_bounds[0] = bounds[0];
		if (bounds[1] > phantombox_bounds[1]) phantombox_bounds[1] = bounds[1];
		if (bounds[2] < phantombox_bounds[2]) phantombox_bounds[2] = bounds[2];
		if (bounds[3] > phantombox_bounds[3]) phantombox_bounds[3] = bounds[3];
		if (bounds[4] < phantombox_bounds[4]) phantombox_bounds[4] = bounds[4];
		if (bounds[5] > phantombox_bounds[5]) phantombox_bounds[5] = bounds[5];
	}

	double centerX = (phantombox_bounds[0] + phantombox_bounds[1]) / 2;
	double centerY = (phantombox_bounds[2] + phantombox_bounds[3]) / 2;
	double radius;
	if ((phantombox_bounds[1] - phantombox_bounds[0]) > (phantombox_bounds[3] - phantombox_bounds[2])) //X길이가 더 클 때
	{
		radius = (phantombox_bounds[1] - phantombox_bounds[0]) / 2;
	}
	else //Y길이가 더 클 때
	{
		radius = (phantombox_bounds[3] - phantombox_bounds[2]) / 2;
	}
	PosX_SourceFD_QLineEdit->setText(QString::number(centerX));
	PosY_SourceFD_QLineEdit->setText(QString::number(centerY));
	Radius_sourceFD_QLineEdit->setText(QString::number(radius));

	double zLocation = phantombox_bounds[4] - 0.001; // 0.001 cm 만큼 밑으로 마진
	PosZ_SourceFD_QLineEdit->setText(QString::number(zLocation));
}
void FloorDiskWidget::slot_FloorUpdate_ButtonClicked()
{
	double center_radius[4] = { 0.0, };
	center_radius[0] = PosX_SourceFD_QLineEdit->text().toFloat();
	center_radius[1] = PosY_SourceFD_QLineEdit->text().toFloat();
	center_radius[2] = PosZ_SourceFD_QLineEdit->text().toFloat();
	center_radius[3] = Radius_sourceFD_QLineEdit->text().toFloat();

	theApp.sourceObjects->GenerateSourceActor_sourceFD(center_radius);;
}
void FloorDiskWidget::slot_sourceFD_RadionuclideSelect_RadioButtonClicked()
{
	RI_Select_sourceFD_QRadioButton->setChecked(true);
	ES_Select_sourceFD_QRadioButton->setChecked(false);

	RI_Title_sourceFD_QLabel->show();
	RI_Radionuclide_sourceFD_QLineEdit->show();
	RI_Activity_sourceFD_QLabel->show();
	RI_Activity_sourceFD_QLineEdit->show();
	sourceFD_RadionuclideAddButton->show();
	RI_List_sourceFD_QListWidget->show();
	RI_List_sourceFD_QLabel->show();

	ES_Title_sourceFD_QLabel->hide();
	sourceFD_EnergyspectrumFileLoadButton->hide();
	ES_Intensity_sourceFD_QLabel->hide();
	ES_Intensity_sourceFD_QLineEdit->hide();
	ES_FileName_sourceFD_QLabel->hide();
	sourceFD_EnergyspectrumFileNameText->hide();
	ES_List_sourceFD_QLabel->hide();
	sourceFD_EnergySpectrumAddButton->hide();
	ES_List_sourceFD_QListWidget->hide();

	// === ES 관련 초기화 ===
	   
	// 2. QListWidget의 모든 아이템 제거
	// clear() 함수는 위젯의 모든 아이템을 삭제하고 메모리에서 해제합니다.
	ES_List_sourceFD_QListWidget->clear();

	// 3. 관리하던 모든 vector의 내용 비우기
	// 각 vector의 clear() 멤버 함수를 호출하여 모든 요소를 제거합니다.
	ListInfo_sourceFD.clear();
	ES_ListDelete_sourceFD_QButton.clear();
	ES_ListInfo_sourceFD_QButton.clear();
	theApp.SourcePanelActor_Position.clear();

	// 4. 인덱스 카운터 초기화
	// 리스트 아이템 개수를 추적하는 변수를 초기값(보통 0)으로 리셋합니다.
	List_sourceFD_MakingIndex = 0;

}
void FloorDiskWidget::slot_RIList_Add_sourceFD_ButtonClicked()
{
	// 지원하는 RI 리스트에 있는지 판별
	QString inputText = RI_Radionuclide_sourceFD_QLineEdit->text().trimmed();
	if (inputText.isEmpty()) {
		theApp.SetMessageBox("Radionuclide name is empty!");
		return;
	}

	// 1) 대소문자 구분하여 리스트에 있는지 검사
	if (!panel->RIsourceList.contains(inputText, Qt::CaseSensitive)) {
		// 2) 자동 보정: 첫 글자만 대문자, 나머지는 소문자
		QString corrected = inputText.toLower();
		if (!corrected.isEmpty()) {
			corrected[0] = corrected[0].toUpper();
		}
		// 보정된 문자열이 리스트에 있으면 QLineEdit에 반영
		if (panel->RIsourceList.contains(corrected, Qt::CaseSensitive)) {
			RI_Radionuclide_sourceFD_QLineEdit->setText(corrected);
			inputText = corrected;
		}
		else {
			// 보정해도 없으면 에러
			theApp.SetMessageBox(inputText + ": Radionuclide name error!");
			return;
		}
	}

	QListWidgetItem* item = new QListWidgetItem(RI_List_sourceFD_QListWidget);
	QWidget* widget = new QWidget;
	QHBoxLayout* hLayout = new QHBoxLayout;
	widget->setStyleSheet("background-color: white;");

	QString arg1 = RI_Radionuclide_sourceFD_QLineEdit->text();
	QLabel* label1 = new QLabel(arg1);
	label1->setFixedWidth(panel->Window_width * 0.025);
	label1->setFont(panel->font_D_LW2); // Set font for the label

	QString arg2 = QString::number(RI_Activity_sourceFD_QLineEdit->text().toDouble(), 'e', 3);
	QLabel* label2 = new QLabel(arg2);
	label2->setFixedWidth(panel->Window_width * 0.025);
	label2->setFont(panel->font_D_LW2); // Set font for the label

	QString arg3 = "";
	QLabel* labelPosX = new QLabel(arg3);
	labelPosX->setFixedWidth(panel->Window_width * 0.025);
	labelPosX->setFont(panel->font_D_LW2); // Set font for the label

	QString arg4 = "";
	QLabel* labelPosY = new QLabel(arg4);
	labelPosY->setFixedWidth(panel->Window_width * 0.025);
	labelPosY->setFont(panel->font_D_LW2); // Set font for the label

	QString arg5 = "";
	QLabel* labelPosZ = new QLabel(arg5);
	labelPosZ->setFixedWidth(panel->Window_width * 0.025);
	labelPosZ->setFont(panel->font_D_LW2); // Set font for the label

	RI_ListDelete_sourceFD_QButton.push_back(new QPushButton());
	if (List_sourceFD_MakingIndex >= RI_ListDelete_sourceFD_QButton.size()) theApp.SetMessageBox("RI_Delete_sourceFD_QButton Vector index error");
	RI_ListDelete_sourceFD_QButton[List_sourceFD_MakingIndex]->setCheckable(false);
	RI_ListDelete_sourceFD_QButton[List_sourceFD_MakingIndex]->setStyleSheet(DialogStyle::MENU_BTN13);
	RI_ListDelete_sourceFD_QButton[List_sourceFD_MakingIndex]->setFont(panel->font_D_BTN13);
	RI_ListDelete_sourceFD_QButton[List_sourceFD_MakingIndex]->setFixedSize(panel->Window_width * 0.008, panel->Window_height * 0.012);
	RI_ListDelete_sourceFD_QButton[List_sourceFD_MakingIndex]->setIcon(QIcon("./data/image/delete.png"));
	RI_ListDelete_sourceFD_QButton[List_sourceFD_MakingIndex]->setObjectName(QString::number(List_sourceFD_MakingIndex));

	RI_ListInfo_sourceFD_QButton.push_back(new QPushButton());
	if (List_sourceFD_MakingIndex >= RI_ListDelete_sourceFD_QButton.size()) theApp.SetMessageBox("RI_Info_sourceFD_QButton Vector index error");
	RI_ListInfo_sourceFD_QButton[List_sourceFD_MakingIndex]->setCheckable(false);
	RI_ListInfo_sourceFD_QButton[List_sourceFD_MakingIndex]->setStyleSheet(DialogStyle::MENU_BTN13);
	RI_ListInfo_sourceFD_QButton[List_sourceFD_MakingIndex]->setFont(panel->font_D_BTN13);
	RI_ListInfo_sourceFD_QButton[List_sourceFD_MakingIndex]->setFixedSize(panel->Window_width * 0.008, panel->Window_height * 0.012);
	RI_ListInfo_sourceFD_QButton[List_sourceFD_MakingIndex]->setIcon(QIcon("./data/image/info.png"));
	RI_ListInfo_sourceFD_QButton[List_sourceFD_MakingIndex]->setObjectName(QString::number(List_sourceFD_MakingIndex));

	hLayout->addWidget(label1);
	hLayout->addWidget(label2);
	hLayout->addWidget(labelPosX);
	hLayout->addWidget(labelPosY);
	hLayout->addWidget(labelPosZ);
	hLayout->addWidget(RI_ListDelete_sourceFD_QButton[List_sourceFD_MakingIndex]);
	hLayout->addWidget(RI_ListInfo_sourceFD_QButton[List_sourceFD_MakingIndex]);
	widget->setLayout(hLayout);

	RI_List_sourceFD_QListWidget->setItemWidget(item, widget);
	item->setSizeHint(widget->sizeHint());

	// Connect signals of the new buttons
	connect(RI_ListDelete_sourceFD_QButton[List_sourceFD_MakingIndex], SIGNAL(clicked()), this, SLOT(slot_RIList_Delete_sourceFD_ButtonClicked())); connect(RI_ListDelete_sourceFD_QButton[List_sourceFD_MakingIndex], SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
	connect(RI_ListInfo_sourceFD_QButton[List_sourceFD_MakingIndex], SIGNAL(clicked()), this, SLOT(slot_RIList_Info_sourceFD_ButtonClicked())); connect(RI_ListInfo_sourceFD_QButton[List_sourceFD_MakingIndex], SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));

	// Set Global Variables
	std::map<int, QString> tmp_map;
	tmp_map[0] = "True";
	tmp_map[1] = RI_Radionuclide_sourceFD_QLineEdit->text();
	tmp_map[2] = RI_Activity_sourceFD_QLineEdit->text(); if (tmp_map[2] == "") tmp_map[2] = "0";
	ListInfo_sourceFD.push_back(tmp_map);
	List_sourceFD_MakingIndex++;

	theApp.SetMessageBox_RadionuclideWarning("The radionuclide data provided in this code are based on the data provided in the ICRP 107 publication.\nTHIS DATA DO NOT INCLUDE ANY DAUGHTER NUCLIDES!!!");
}
void FloorDiskWidget::slot_RIList_Delete_sourceFD_ButtonClicked()
{
	// Button의 Index 찾기
	QPushButton* buttonSender = qobject_cast<QPushButton*>(sender());
	QString buttonText = buttonSender->objectName();

	// The first part is the ID
	int RadonuclideIndex = buttonText.toDouble();
	int row = RI_List_sourceFD_QListWidget->row(RI_List_sourceFD_QListWidget->item(RadonuclideIndex));
	QListWidgetItem* toRemove = RI_List_sourceFD_QListWidget->takeItem(row);
	delete toRemove; // Delete the item to free memory

	if (RadonuclideIndex >= 0 && RadonuclideIndex < ListInfo_sourceFD.size()) {
		ListInfo_sourceFD.erase(ListInfo_sourceFD.begin() + RadonuclideIndex);
		RI_ListDelete_sourceFD_QButton.erase(RI_ListDelete_sourceFD_QButton.begin() + RadonuclideIndex);
		RI_ListInfo_sourceFD_QButton.erase(RI_ListInfo_sourceFD_QButton.begin() + RadonuclideIndex);
	}
	else {
		theApp.SetMessageBox("RI_list_Info_sourceFD Vecetor index error");
	}

	for (int index = 0; index < RI_ListDelete_sourceFD_QButton.size(); index++)
	{
		RI_ListDelete_sourceFD_QButton[index]->setObjectName(QString::number(index));
		RI_ListInfo_sourceFD_QButton[index]->setObjectName(QString::number(index));
	}
	List_sourceFD_MakingIndex--; // listwidget은 vector 형태로 동작해야 함(row의 item이 실제 몇 행인지로 결정됨)	
}
void FloorDiskWidget::slot_RIList_Info_sourceFD_ButtonClicked()
{
	// Button의 Index 찾기
	QPushButton* buttonSender = qobject_cast<QPushButton*>(sender());
	QString buttonText = buttonSender->objectName();

	QStringList list_button = buttonText.split("_");

	// The first part is the ID
	int RadonuclideIndex = list_button[0].toInt();
	QListWidgetItem* item = RI_List_sourceFD_QListWidget->item(RadonuclideIndex);
	if (!item) {
		// Handle error: item not found
		return;
	}

	QWidget* widget = RI_List_sourceFD_QListWidget->itemWidget(item);
	QHBoxLayout* hLayout = qobject_cast<QHBoxLayout*>(widget->layout());
	if (!hLayout) {
		// Handle error: layout not found
		return;
	}

	// Assuming the order of labels in the layout is known
	QLabel* labelRadionuclide = qobject_cast<QLabel*>(hLayout->itemAt(0)->widget());
	QLabel* labelActivity = qobject_cast<QLabel*>(hLayout->itemAt(1)->widget());

	// Create a new dialog
	QDialog* infoDialog = new QDialog(this);
	infoDialog->setStyleSheet("background-color: white;");
	infoDialog->resize(panel->Window_width * 0.3, panel->Window_height * 0.55);
	infoDialog->setWindowTitle("Radionuclide Information");

	// Create a layout and a label for the dialog
	QVBoxLayout* layout = new QVBoxLayout(infoDialog);
	// Create a QTextEdit for the dialog
	QTextEdit* textEdit = new QTextEdit(infoDialog);
	textEdit->setReadOnly(true); // Make it read-only if editing is not required

	// Create a QTextCursor for inserting formatted text
	QTextCursor cursor = textEdit->textCursor();

	// Define different text formats
	QTextCharFormat boldFormat;
	boldFormat.setFontFamily("Courier New"); // This is monospaced font
	boldFormat.setFontWeight(QFont::Bold);
	boldFormat.setFontPointSize(14);

	QTextCharFormat boldMediumFormat;
	boldMediumFormat.setFontFamily("Courier New");
	boldMediumFormat.setFontWeight(QFont::Bold);
	boldMediumFormat.setFontPointSize(12);

	QTextCharFormat boldSmallFormat;
	boldSmallFormat.setFontFamily("Courier New");
	boldSmallFormat.setFontWeight(QFont::Bold);
	boldSmallFormat.setFontPointSize(10);

	QTextCharFormat normalFormat;
	normalFormat.setFontFamily("Courier New");
	normalFormat.setFontPointSize(10);

	QTextCharFormat smallFormat;
	smallFormat.setFontFamily("Courier New");
	smallFormat.setFontPointSize(8);

	QTextCharFormat verySmallFormat;
	verySmallFormat.setFontFamily("Courier New");
	verySmallFormat.setFontPointSize(6);

	// Insert formatted text into QTextEdit
	cursor.insertText("*This radionuclide information is based on ICRP 107 Publication\n\n", smallFormat);
	cursor.insertText("Radionuclide Information\n", boldFormat);
	cursor.insertText("Radionuclide: " + labelRadionuclide->text() + "\n", normalFormat);
	cursor.insertText("Activity (Bq): " + labelActivity->text() + "\n", normalFormat);

	// Fine RN information from ICRP07.NDX file (ICRP 107 Pub.)
	QFile file("./data/radioisotopes/_ICRP-07.NDX");
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		// Handle error: Unable to open file
		return;
	}
	QTextStream in(&file);
	QString line;
	bool isDataFound = false;
	QString halfLife, daughterNucleus1, branchingFraction1, daughterNucleus2, branchingFraction2, daughterNucleus3, branchingFraction3, daughterNucleus4, branchingFraction4;
	int daughterNucleus1_index = 7; // 이상적으로 8열(Index: 7)에서 딸핵종1 시작

	while (!in.atEnd()) {
		line = in.readLine();
		if (line.trimmed().isEmpty()) continue; // Skip empty lines
		QStringList columns = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
		if (columns[0] == labelRadionuclide->text()) {
			isDataFound = true;
			halfLife = columns[1]; // Extract HalfLife
			bool isNumber; double tmpNum;
			tmpNum = columns[3].toDouble(&isNumber); // 이상적으로는 숫자, 하지만 DecayMode 알파벳이 할당되어 있을수도 있음
			if (!isNumber) daughterNucleus1_index++; // Decay Mode has assgined to two columns
			daughterNucleus1 = columns[daughterNucleus1_index];
			branchingFraction1 = columns[daughterNucleus1_index + 2];
			tmpNum = columns[daughterNucleus1_index + 3].toDouble(&isNumber);
			if (isNumber) break;// 이상적으로는 딸핵종, 하지만 딸핵종이 더 이상 없어 0이 할당되어 있을수도 있음

			daughterNucleus2 = columns[daughterNucleus1_index + 3];
			branchingFraction2 = columns[daughterNucleus1_index + 5];
			tmpNum = columns[daughterNucleus1_index + 6].toDouble(&isNumber);
			if (isNumber) break;// 이상적으로는 딸핵종, 하지만 딸핵종이 더 이상 없어 0이 할당되어 있을수도 있음

			daughterNucleus3 = columns[daughterNucleus1_index + 6];
			branchingFraction3 = columns[daughterNucleus1_index + 8];
			tmpNum = columns[daughterNucleus1_index + 9].toDouble(&isNumber);
			if (isNumber) break;// 이상적으로는 딸핵종, 하지만 딸핵종이 더 이상 없어 0이 할당되어 있을수도 있음

			daughterNucleus4 = columns[daughterNucleus1_index + 9];
			branchingFraction4 = columns[daughterNucleus1_index + 11];
			break; // Stop reading further as the data is found
		}
	}
	file.close();

	cursor.insertText("HalfLife: " + halfLife + "\n\n", normalFormat);
	cursor.insertText("Daughter Nuclides Information\n", boldFormat);
	cursor.insertText("Radionuclide   Fraction\n", boldSmallFormat);

	// Insert text into QTextEdit with padding
	if (!daughterNucleus1.isEmpty()) {
		QString padding(15 - daughterNucleus1.length(), ' ');
		cursor.insertText(daughterNucleus1 + padding + branchingFraction1 + "\n", normalFormat);
	}
	if (!daughterNucleus2.isEmpty()) {
		QString padding(15 - daughterNucleus2.length(), ' ');
		cursor.insertText(daughterNucleus2 + padding + branchingFraction2 + "\n", normalFormat);
	}
	if (!daughterNucleus3.isEmpty()) {
		QString padding(15 - daughterNucleus3.length(), ' ');
		cursor.insertText(daughterNucleus3 + padding + branchingFraction3 + "\n", normalFormat);
	}
	if (!daughterNucleus4.isEmpty()) {
		QString padding(15 - daughterNucleus4.length(), ' ');
		cursor.insertText(daughterNucleus4 + padding + branchingFraction4 + "\n", normalFormat);
	}

	cursor.insertText("\nRadiation Spectrum of the Entered Radionuclide\n", boldFormat);
	cursor.insertText("Radiation      Fraction       Energy (MeV)\n", boldSmallFormat);

	QString RNspectrumFileName = ("./data/radioisotopes/" + labelRadionuclide->text() + ".txt");
	QFile RNspectrumFile(RNspectrumFileName);
	if (!RNspectrumFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		// Handle error: Unable to open file
		return;
	}

	QTextStream RNspectrumInput(&RNspectrumFile);
	QString RNspectrumLine;

	// Skip the first line
	if (!RNspectrumInput.atEnd()) {
		RNspectrumLine = RNspectrumInput.readLine();
	}


	while (!RNspectrumInput.atEnd()) {
		RNspectrumLine = RNspectrumInput.readLine();
		QStringList columns = RNspectrumLine.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);

		if (columns.size() >= 4) {
			QString radiation = columns[3];
			QString fraction = columns[1];
			QString energy = columns[2];
			
			QString padding1(15 - radiation.length(), ' ');
			QString padding2(15 - fraction.length(), ' ');
			cursor.insertText(radiation + padding1 + fraction + padding2 + energy + "\n", normalFormat);
		}
	}
	RNspectrumFile.close();

	cursor.insertText("\n*Alpha recoil nuclei and fission fragments are excluded.\n*PG, DG, and DB are radiations of spontaneous fission.\n", smallFormat);
	cursor.insertText("G: Gamma rays\n", verySmallFormat);
	cursor.insertText("PG: Prompt gamma rays\n", verySmallFormat);
	cursor.insertText("DG: Delayed gamma rays\n", verySmallFormat);
	cursor.insertText("X: X rays\n", verySmallFormat);
	cursor.insertText("AQ: Annihilation photons\n", verySmallFormat);
	cursor.insertText("B+: Beta-plus particles\n", verySmallFormat);
	cursor.insertText("B-: Beta-minus particles\n", verySmallFormat);
	cursor.insertText("DB: Delayed beta particles\n", verySmallFormat);
	cursor.insertText("IE: Internal conversion electrons\n", verySmallFormat);
	cursor.insertText("AE: Auger electrons\n", verySmallFormat);
	cursor.insertText("A: Alpha particles\n", verySmallFormat);
	cursor.insertText("N: Neutrons\n", verySmallFormat);

	cursor.movePosition(QTextCursor::Start);
	textEdit->setTextCursor(cursor);

	// Add the QTextEdit to the layout
	layout->addWidget(textEdit);

	// Set the layout to the dialog
	infoDialog->setLayout(layout);

	// Show the dialog
	infoDialog->exec(); // Use exec() for a modal dialog or show() for a non-modal dialog
}
void FloorDiskWidget::slot_sourceFD_EnergySpectrumSelect_RadioButtonClicked()
{
	RI_Select_sourceFD_QRadioButton->setChecked(false);
	ES_Select_sourceFD_QRadioButton->setChecked(true);

	RI_Title_sourceFD_QLabel->hide();
	RI_Radionuclide_sourceFD_QLineEdit->hide();
	RI_Activity_sourceFD_QLabel->hide();
	RI_Activity_sourceFD_QLineEdit->hide();
	sourceFD_RadionuclideAddButton->hide();
	RI_List_sourceFD_QListWidget->hide();
	RI_List_sourceFD_QLabel->hide();

	ES_Title_sourceFD_QLabel->show();
	sourceFD_EnergyspectrumFileLoadButton->show();
	ES_Intensity_sourceFD_QLabel->show();
	ES_Intensity_sourceFD_QLineEdit->show();
	ES_FileName_sourceFD_QLabel->show();
	sourceFD_EnergyspectrumFileNameText->show();
	ES_List_sourceFD_QLabel->show();
	sourceFD_EnergySpectrumAddButton->show();
	ES_List_sourceFD_QListWidget->show();

	// === RI 관련 초기화 ===
	
	// 2. QListWidget의 모든 아이템 제거
	// clear() 함수는 위젯의 모든 아이템을 삭제하고 메모리에서 해제합니다.
	RI_List_sourceFD_QListWidget->clear();

	// 3. 관리하던 모든 vector의 내용 비우기
	// 각 vector의 clear() 멤버 함수를 호출하여 모든 요소를 제거합니다.
	ListInfo_sourceFD.clear();
	RI_ListDelete_sourceFD_QButton.clear();
	RI_ListInfo_sourceFD_QButton.clear();
	theApp.SourcePanelActor_Position.clear();

	// 4. 인덱스 카운터 초기화
	// 리스트 아이템 개수를 추적하는 변수를 초기값(보통 0)으로 리셋합니다.
	List_sourceFD_MakingIndex = 0;
}
void FloorDiskWidget::slot_ESList_Add_sourceFD_ButtonClicked()
{
	// 지원하는 RI 리스트에 있는지 판별
	QString inputText = sourceFD_EnergyspectrumFileNameText->toPlainText();
	if (inputText.isEmpty()) {
		theApp.SetMessageBox("Energy spectrum file is empty!");
		return;
	}

	QListWidgetItem* item = new QListWidgetItem(ES_List_sourceFD_QListWidget);
	QWidget* widget = new QWidget;
	QHBoxLayout* hLayout = new QHBoxLayout;
	widget->setStyleSheet("background-color: white;");

	QString arg1 = QFileInfo(inputText).fileName();
	QLabel* label1 = new QLabel(arg1);
	label1->setFixedWidth(panel->Window_width * 0.025);
	label1->setFont(panel->font_D_LW2); // Set font for the label
	label1->setToolTip(arg1); // 전체 파일명 툴팁
	label1->setScaledContents(false);
	label1->setTextInteractionFlags(Qt::TextSelectableByMouse); // 선택 가능 (옵션)

	QString arg2 = QString::number(ES_Intensity_sourceFD_QLineEdit->text().toDouble(), 'e', 3);
	QLabel* label2 = new QLabel(arg2);
	label2->setFixedWidth(panel->Window_width * 0.025);
	label2->setFont(panel->font_D_LW2); // Set font for the label

	QString arg3 = "";
	QLabel* labelPosX = new QLabel(arg3);
	labelPosX->setFixedWidth(panel->Window_width * 0.025);
	labelPosX->setFont(panel->font_D_LW2); // Set font for the label

	QString arg4 = "";
	QLabel* labelPosY = new QLabel(arg4);
	labelPosY->setFixedWidth(panel->Window_width * 0.025);
	labelPosY->setFont(panel->font_D_LW2); // Set font for the label

	QString arg5 = "";
	QLabel* labelPosZ = new QLabel(arg5);
	labelPosZ->setFixedWidth(panel->Window_width * 0.025);
	labelPosZ->setFont(panel->font_D_LW2); // Set font for the label

	ES_ListDelete_sourceFD_QButton.push_back(new QPushButton());
	if (List_sourceFD_MakingIndex >= ES_ListDelete_sourceFD_QButton.size()) theApp.SetMessageBox("m_sourceFD_ES_listDeleteButton Vector index error");
	ES_ListDelete_sourceFD_QButton[List_sourceFD_MakingIndex]->setCheckable(false);
	ES_ListDelete_sourceFD_QButton[List_sourceFD_MakingIndex]->setStyleSheet(DialogStyle::MENU_BTN13);
	ES_ListDelete_sourceFD_QButton[List_sourceFD_MakingIndex]->setFont(panel->font_D_BTN13);
	ES_ListDelete_sourceFD_QButton[List_sourceFD_MakingIndex]->setFixedSize(panel->Window_width * 0.008, panel->Window_height * 0.012);
	ES_ListDelete_sourceFD_QButton[List_sourceFD_MakingIndex]->setIcon(QIcon("./data/image/delete.png"));
	ES_ListDelete_sourceFD_QButton[List_sourceFD_MakingIndex]->setObjectName(QString::number(List_sourceFD_MakingIndex));

	ES_ListInfo_sourceFD_QButton.push_back(new QPushButton());
	if (List_sourceFD_MakingIndex >= ES_ListInfo_sourceFD_QButton.size()) theApp.SetMessageBox("m_sourceFD_ES_listInfoButton Vector index error");
	ES_ListInfo_sourceFD_QButton[List_sourceFD_MakingIndex]->setCheckable(false);
	ES_ListInfo_sourceFD_QButton[List_sourceFD_MakingIndex]->setStyleSheet(DialogStyle::MENU_BTN13);
	ES_ListInfo_sourceFD_QButton[List_sourceFD_MakingIndex]->setFont(panel->font_D_BTN13);
	ES_ListInfo_sourceFD_QButton[List_sourceFD_MakingIndex]->setFixedSize(panel->Window_width * 0.008, panel->Window_height * 0.012);
	ES_ListInfo_sourceFD_QButton[List_sourceFD_MakingIndex]->setIcon(QIcon("./data/image/info.png"));
	ES_ListInfo_sourceFD_QButton[List_sourceFD_MakingIndex]->setObjectName(QString::number(List_sourceFD_MakingIndex));

	hLayout->addWidget(label1);
	hLayout->addWidget(label2);
	hLayout->addWidget(labelPosX);
	hLayout->addWidget(labelPosY);
	hLayout->addWidget(labelPosZ);
	hLayout->addWidget(ES_ListDelete_sourceFD_QButton[List_sourceFD_MakingIndex]);
	hLayout->addWidget(ES_ListInfo_sourceFD_QButton[List_sourceFD_MakingIndex]);
	widget->setLayout(hLayout);

	ES_List_sourceFD_QListWidget->setItemWidget(item, widget);
	item->setSizeHint(widget->sizeHint());

	// Connect signals of the new buttons
	connect(ES_ListDelete_sourceFD_QButton[List_sourceFD_MakingIndex], SIGNAL(clicked()), this, SLOT(slot_ESList_Delete_sourceFD_ButtonClicked())); connect(ES_ListDelete_sourceFD_QButton[List_sourceFD_MakingIndex], SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
	connect(ES_ListInfo_sourceFD_QButton[List_sourceFD_MakingIndex], SIGNAL(clicked()), this, SLOT(slot_ESList_Info_sourceFD_ButtonClicked())); connect(ES_ListInfo_sourceFD_QButton[List_sourceFD_MakingIndex], SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
	
	// Set Global Variables
	std::map<int, QString> tmp_map;
	tmp_map[0] = "True";
	tmp_map[1] = inputText;
	tmp_map[2] = ES_Intensity_sourceFD_QLineEdit->text(); if (tmp_map[2] == "") tmp_map[2] = "0";
	ListInfo_sourceFD.push_back(tmp_map);
	List_sourceFD_MakingIndex++;

	// Set panel info 
	ES_Intensity_sourceFD_QLineEdit->clear();
	sourceFD_EnergyspectrumFileNameText->clear();
}
void FloorDiskWidget::slot_ESList_Delete_sourceFD_ButtonClicked()
{
	// Button의 Index 찾기
	QPushButton* buttonSender = qobject_cast<QPushButton*>(sender());
	QString buttonText = buttonSender->objectName();

	// The first part is the ID
	int EnersySpectrumIndex = buttonText.toDouble();
	int row = ES_List_sourceFD_QListWidget->row(ES_List_sourceFD_QListWidget->item(EnersySpectrumIndex));
	QListWidgetItem* toRemove = ES_List_sourceFD_QListWidget->takeItem(row);
	delete toRemove; // Delete the item to free memory

	if (EnersySpectrumIndex >= 0 && EnersySpectrumIndex < ListInfo_sourceFD.size())
	{
		ListInfo_sourceFD.erase(ListInfo_sourceFD.begin() + EnersySpectrumIndex);
		ES_ListDelete_sourceFD_QButton.erase(ES_ListDelete_sourceFD_QButton.begin() + EnersySpectrumIndex);
		ES_ListInfo_sourceFD_QButton.erase(ES_ListInfo_sourceFD_QButton.begin() + EnersySpectrumIndex);
		
	}
	else {
		theApp.SetMessageBox("sourceFD_info Vecetor index error");
	}

	for (int index = 0; index < ES_ListDelete_sourceFD_QButton.size(); index++)
	{
		ES_ListDelete_sourceFD_QButton[index]->setObjectName(QString::number(index));
		ES_ListInfo_sourceFD_QButton[index]->setObjectName(QString::number(index));
	}
	List_sourceFD_MakingIndex--; // listwidget은 vector 형태로 동작해야 함(row의 item이 실제 몇 행인지로 결정됨)	
}
void FloorDiskWidget::slot_ESList_Info_sourceFD_ButtonClicked()
{
	// Button의 Index 찾기
	QPushButton* buttonSender = qobject_cast<QPushButton*>(sender());
	QString buttonText = buttonSender->objectName();

	QStringList list_button = buttonText.split("_");

	// The first part is the ID
	int EnergySpectrumIndex = list_button[0].toInt();

	QListWidgetItem* item = ES_List_sourceFD_QListWidget->item(EnergySpectrumIndex);
	if (!item) {
		// Handle error: item not found
		return;
	}

	QWidget* widget = ES_List_sourceFD_QListWidget->itemWidget(item);
	QHBoxLayout* hLayout = qobject_cast<QHBoxLayout*>(widget->layout());
	if (!hLayout) {
		// Handle error: layout not found
		return;
	}

	// Assuming the order of labels in the layout is known
	QLabel* labelEnergySpectrumFile = qobject_cast<QLabel*>(hLayout->itemAt(0)->widget());
	QLabel* labelIntensity = qobject_cast<QLabel*>(hLayout->itemAt(1)->widget());

	// Create a new dialog
	QDialog* infoDialog = new QDialog(this);
	infoDialog->setStyleSheet("background-color: white;");
	infoDialog->resize(panel->Window_width * 0.3, panel->Window_height * 0.55);
	infoDialog->setWindowTitle("Radionuclide Information");

	// Create a layout and a label for the dialog
	QVBoxLayout* layout = new QVBoxLayout(infoDialog);
	// Create a QTextEdit for the dialog
	QTextEdit* textEdit = new QTextEdit(infoDialog);
	textEdit->setReadOnly(true); // Make it read-only if editing is not required

	// Create a QTextCursor for inserting formatted text
	QTextCursor cursor = textEdit->textCursor();

	// Define different text formats
	QTextCharFormat boldFormat;
	boldFormat.setFontFamily("Courier New"); // This is monospaced font
	boldFormat.setFontWeight(QFont::Bold);
	boldFormat.setFontPointSize(14);

	QTextCharFormat boldMediumFormat;
	boldMediumFormat.setFontFamily("Courier New");
	boldMediumFormat.setFontWeight(QFont::Bold);
	boldMediumFormat.setFontPointSize(12);

	QTextCharFormat boldSmallFormat;
	boldSmallFormat.setFontFamily("Courier New");
	boldSmallFormat.setFontWeight(QFont::Bold);
	boldSmallFormat.setFontPointSize(10);

	QTextCharFormat normalFormat;
	normalFormat.setFontFamily("Courier New");
	normalFormat.setFontPointSize(10);

	QTextCharFormat smallFormat;
	smallFormat.setFontFamily("Courier New");
	smallFormat.setFontPointSize(8);

	QTextCharFormat verySmallFormat;
	verySmallFormat.setFontFamily("Courier New");
	verySmallFormat.setFontPointSize(6);

	// Insert formatted text into QTextEdit	
	cursor.insertText("EnergySpectrum Information\n", boldFormat);
	cursor.insertText("FileName: " + labelEnergySpectrumFile->text() + "\n", normalFormat);
	cursor.insertText("Intensity (#/s): " + labelIntensity->text() + "\n", normalFormat);

	cursor.insertText("\nRadiation Spectrum of the Entered File\n", boldFormat);
	cursor.insertText("Radiation      Fraction       Energy (MeV)\n", boldSmallFormat);

	QString ESFileName = ListInfo_sourceFD[EnergySpectrumIndex][1];
	QFile ESFile(ESFileName);
	if (!ESFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		// Handle error: Unable to open file
		return;
	}

	QTextStream ESInput(&ESFile);
	while (!ESInput.atEnd()) {
		QString ESLine = ESInput.readLine();
		QStringList columns = ESLine.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);

		if (columns.size() >= 3) {
			QString radiation = columns[0];
			QString fraction = columns[1];
			QString energy = columns[2];			

			QString padding1(15 - radiation.length(), ' ');
			QString padding2(15 - fraction.length(), ' ');
			cursor.insertText(radiation + padding1 + fraction + padding2 + energy + "\n", normalFormat);
		}
	}
	ESFile.close();

	cursor.movePosition(QTextCursor::Start);
	textEdit->setTextCursor(cursor);

	// Add the QTextEdit to the layout
	layout->addWidget(textEdit);

	// Set the layout to the dialog
	infoDialog->setLayout(layout);

	// Show the dialog
	infoDialog->exec(); // Use exec() for a modal dialog or show() for a non-modal dialog
}
void FloorDiskWidget::slot_sourceFD_EnergyspectrumFileLoad_ButtonClicked()
{
	QString filter;
	filter = tr("(*.*)");
	QString dir = QFileDialog::getOpenFileName(
		this,
		tr("Select a file to open"),
		"",
		filter);

	if (!dir.isEmpty())
	{
		sourceFD_EnergyspectrumFileNameText->setText(dir);
		sourceFD_EnergyspectrumFileNameText->setReadOnly(true);
		QFont font;
		font.setFamily("Arial");    // Font family
		font.setPointSize(panel->FontSizeScaling(7));      // Font size
		sourceFD_EnergyspectrumFileNameText->setFont(font);
	}
}


///////////////////////////////////////////////////////////////////////////
// ObjectVolumeWidget Implementation
///////////////////////////////////////////////////////////////////////////
// --- Helper Functions (Private Logic) ---
ObjectVolumeWidget::ObjectVolumeWidget(QWidget* parent)
    : ParentT(parent)
{
}

ObjectVolumeWidget::~ObjectVolumeWidget()
{
    // 다이얼로그 등 동적 할당된 객체 중 부모가 없는 경우 여기서 삭제 필요
    if (sourceOV_ObjectAddingDialog) {
        delete sourceOV_ObjectAddingDialog;
        sourceOV_ObjectAddingDialog = Q_NULLPTR;
    }
}

bool ObjectVolumeWidget::initialize()
{
    if (!ParentT::initialize()) return false;

    // panel->Window_width 등 부모 자원에 접근하기 위해 panel 포인터 사용
    QVBoxLayout* subLayout = new QVBoxLayout;
    subLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.005, panel->Window_width * 0.003, panel->Window_height * 0.005);
    subLayout->setSpacing(panel->Window_width * 0.003);
    subLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    {
        QVBoxLayout* vLayout = new QVBoxLayout;
        vLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.005, panel->Window_width * 0.003, panel->Window_height * 0.005);
        vLayout->setSpacing(panel->Window_width * 0.003);
        vLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

        // [Visualization]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setSpacing(panel->Window_width * 0.005);
            hLayout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

            QLabel* labelTitle = new QLabel;
            labelTitle->setFixedSize(panel->Window_width * 0.104, panel->Window_height * 0.028);
            labelTitle->setStyleSheet(DialogStyle::DATA_LABEL);
            labelTitle->setFont(panel->font_D_LBL1);
            labelTitle->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            labelTitle->setText(QString::fromLocal8Bit("Visualization"));

            m_ObjectVolumeSourceVisualizationButton = new QPushButton(QString::fromLocal8Bit("On"));
            panel->Buttons_FunctionPanelRight.append(m_ObjectVolumeSourceVisualizationButton);
            m_ObjectVolumeSourceVisualizationButton->setFixedSize(panel->Window_width * 0.05, panel->Window_height * 0.028);
            m_ObjectVolumeSourceVisualizationButton->setCheckable(true);
            m_ObjectVolumeSourceVisualizationButton->setChecked(true);
            m_ObjectVolumeSourceVisualizationButton->setStyleSheet(DialogStyle::MENU_BTN3);
            m_ObjectVolumeSourceVisualizationButton->setFont(panel->font_D_BTN3);

            // 로직은 this, 전체 갱신은 panel
            connect(m_ObjectVolumeSourceVisualizationButton, SIGNAL(clicked()), this, SLOT(slot_ObjectVolumeSourceVisualization_ButtonClicked())); 
            connect(m_ObjectVolumeSourceVisualizationButton, SIGNAL(clicked()), panel, SLOT(slot_allButtonClicked()));

            hLayout->addWidget(labelTitle);
            hLayout->addStretch();
            hLayout->addWidget(m_ObjectVolumeSourceVisualizationButton);

            vLayout->addLayout(hLayout);
        }

        // [Object List Label]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setContentsMargins(panel->Window_width * 0.003, 0, panel->Window_width * 0.003, 0);
            hLayout->setSpacing(panel->Window_width * 0.003);
            hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

            QLabel* label0 = new QLabel;
            label0->setFixedSize(panel->Window_width * 0.068, panel->Window_height * 0.028);
            label0->setStyleSheet(DialogStyle::DATA_LABEL);
            label0->setFont(panel->font_D_LBL1);
            label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            label0->setText(QString::fromLocal8Bit("Object list"));

            hLayout->addWidget(label0);

            vLayout->addLayout(hLayout);
        }

        // [Object List Scroll Area] (Complex UI)
        {
            QGroupBox* GroupBox_sourceOV_ObjectList = new QGroupBox;
            GroupBox_sourceOV_ObjectList->setAlignment(Qt::AlignCenter | Qt::AlignLeft);
            GroupBox_sourceOV_ObjectList->setStyleSheet(DialogStyle::GROUPBOX_WIDGET3);
            GroupBox_sourceOV_ObjectList->setFont(panel->font_D_GBW3);
            GroupBox_sourceOV_ObjectList->setFixedHeight(0.070 * panel->Window_height);
            GroupBox_sourceOV_ObjectList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed); 
            GroupBox_sourceOV_ObjectList->setContentsMargins(panel->Window_width * 0.001, 0, panel->Window_width * 0.001, 0);

            vLayout->addWidget(GroupBox_sourceOV_ObjectList);

            QVBoxLayout* layoutOut = new QVBoxLayout();
            layoutOut->setContentsMargins(panel->Window_width * 0.001, 0, panel->Window_width * 0.005, panel->Window_height * 0.005);
            layoutOut->setSpacing(panel->Window_width * 0.005);
            layoutOut->setAlignment(Qt::AlignLeft | Qt::AlignTop);

            QGroupBox* GroupBoxIn = new QGroupBox;
            GroupBoxIn->setAlignment(Qt::AlignCenter | Qt::AlignLeft);
            GroupBoxIn->setStyleSheet(DialogStyle::GROUPBOX_WIDGET14);
            GroupBoxIn->setFont(panel->font_D_GBW3);
            GroupBoxIn->setFixedHeight(0.060 * panel->Window_height);
            GroupBoxIn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

            // 멤버 변수 layout_sourceOV_ObjectList 초기화
            layout_sourceOV_ObjectList = new QHBoxLayout();
            layout_sourceOV_ObjectList->setContentsMargins(panel->Window_width * 0.001, panel->Window_height * 0.005, panel->Window_width * 0.001, panel->Window_height * 0.005); 
            layout_sourceOV_ObjectList->setSpacing(panel->Window_width * 0.003);
            layout_sourceOV_ObjectList->setAlignment(Qt::AlignLeft | Qt::AlignTop);

            // 커스텀 스크롤 영역 사용 (헤더 include 필수)
            MultipleButtonScorllArea* scrollArea = new MultipleButtonScorllArea();
            scrollArea->setStyleSheet(DialogStyle::GROUPBOX_WIDGET3);
            scrollArea->setFont(panel->font_D_GBW3);
            scrollArea->setMinimumHeight(0.060 * panel->Window_height);
            scrollArea->setMaximumHeight(0.060 * panel->Window_height);
            scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff); 
            
            // 멤버 변수 sourceOV_ObjectScrollBar 초기화
            sourceOV_ObjectScrollBar = scrollArea->horizontalScrollBar();

            GroupBoxIn->setLayout(layout_sourceOV_ObjectList);
            scrollArea->setWidget(GroupBoxIn);
            scrollArea->setWidgetResizable(true);
            layoutOut->addWidget(scrollArea);
            GroupBox_sourceOV_ObjectList->setLayout(layoutOut);
            
            // UI 리프레시 트릭
            GroupBox_sourceOV_ObjectList->hide();
            GroupBox_sourceOV_ObjectList->show();
        }

        // [Add / Delete Buttons]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setSpacing(panel->Window_width * 0.005);
            hLayout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
            hLayout->setContentsMargins(0, panel->Window_height * 0.005, panel->Window_width * 0.010, panel->Window_height * 0.005);

            sourceOV_ObjectAddButton = new QPushButton(QString::fromLocal8Bit("Add"));
            panel->Buttons_FunctionPanelRight.append(sourceOV_ObjectAddButton);
            sourceOV_ObjectAddButton->setFixedSize(panel->Window_width * 0.030, panel->Window_height * 0.025);
            sourceOV_ObjectAddButton->setStyleSheet(DialogStyle::MENU_BTN2);
            sourceOV_ObjectAddButton->setFont(panel->font_D_BTN2);
            sourceOV_ObjectAddButton->setCheckable(false);

            sourceOV_ObjectDeleteButton = new QPushButton(QString::fromLocal8Bit("Delete"));
            panel->Buttons_FunctionPanelRight.append(sourceOV_ObjectDeleteButton);
            sourceOV_ObjectDeleteButton->setFixedSize(panel->Window_width * 0.030, panel->Window_height * 0.025);
            sourceOV_ObjectDeleteButton->setStyleSheet(DialogStyle::MENU_BTN2);
            sourceOV_ObjectDeleteButton->setFont(panel->font_D_BTN2);
            sourceOV_ObjectDeleteButton->setCheckable(false);

            connect(sourceOV_ObjectAddButton, SIGNAL(clicked()), this, SLOT(slot_sourceOV_ObjectAdd_ButtonClicked())); 
            connect(sourceOV_ObjectAddButton, SIGNAL(clicked()), panel, SLOT(slot_allButtonClicked()));
            
            connect(sourceOV_ObjectDeleteButton, SIGNAL(clicked()), this, SLOT(slot_sourceOV_ObjectDelete_ButtonClicked())); 
            connect(sourceOV_ObjectDeleteButton, SIGNAL(clicked()), panel, SLOT(slot_allButtonClicked()));

            hLayout->addWidget(sourceOV_ObjectAddButton);
            hLayout->addWidget(sourceOV_ObjectDeleteButton);

            vLayout->addLayout(hLayout);
        }

        // [Radio Buttons: RI vs ES]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setSpacing(panel->Window_width * 0.010);
            hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            hLayout->setContentsMargins(panel->Window_width * 0.005, panel->Window_height * 0.009, 0, 0);

            sourceOV_Radionuclide_radioButton = new QRadioButton(QString::fromLocal8Bit("Radionuclide"));
            sourceOV_Radionuclide_radioButton->setStyleSheet(DialogStyle::RADIOBUTTON_WIDGET);
            sourceOV_Radionuclide_radioButton->setFont(panel->font_D_RB2);
            sourceOV_Radionuclide_radioButton->setFixedSize(panel->Window_width * 0.081, panel->Window_height * 0.028);
            sourceOV_Radionuclide_radioButton->setChecked(true);

            sourceOV_Energyspectrum_radioButton = new QRadioButton(QString::fromLocal8Bit("Energy spectrum"));
            sourceOV_Energyspectrum_radioButton->setStyleSheet(DialogStyle::RADIOBUTTON_WIDGET);
            sourceOV_Energyspectrum_radioButton->setFont(panel->font_D_RB2);
            sourceOV_Energyspectrum_radioButton->setFixedSize(panel->Window_width * 0.081, panel->Window_height * 0.028);
            sourceOV_Energyspectrum_radioButton->setChecked(false);

            connect(sourceOV_Radionuclide_radioButton, SIGNAL(clicked()), this, SLOT(slot_sourceOV_RadionuclideSelect_RadioButtonClicked())); 
            connect(sourceOV_Radionuclide_radioButton, SIGNAL(clicked()), panel, SLOT(slot_allButtonClicked()));
            
            connect(sourceOV_Energyspectrum_radioButton, SIGNAL(clicked()), this, SLOT(slot_sourceOV_EnergySpectrumSelect_RadioButtonClicked())); 
            connect(sourceOV_Energyspectrum_radioButton, SIGNAL(clicked()), panel, SLOT(slot_allButtonClicked()));

            hLayout->addWidget(sourceOV_Radionuclide_radioButton);
            hLayout->addWidget(sourceOV_Energyspectrum_radioButton);

            vLayout->addLayout(hLayout);
        }

        // [Radionuclide UI]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setContentsMargins(panel->Window_width * 0.003, 0, panel->Window_width * 0.003, 0);
            hLayout->setSpacing(panel->Window_width * 0.003);
            hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

            RI_Title_sourceOV_QLabel = new QLabel;
            RI_Title_sourceOV_QLabel->setFixedSize(panel->Window_width * 0.068, panel->Window_height * 0.028);
            RI_Title_sourceOV_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
            RI_Title_sourceOV_QLabel->setFont(panel->font_D_LBL1);
            RI_Title_sourceOV_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            RI_Title_sourceOV_QLabel->setText(QString::fromLocal8Bit("Radionuclide"));

            RI_Radionuclide_sourceOV_QLineEdit = new QLineEdit;
            RI_Radionuclide_sourceOV_QLineEdit->setFixedSize(panel->Window_width * 0.104, panel->Window_height * 0.028);
            RI_Radionuclide_sourceOV_QLineEdit->setAlignment(Qt::AlignCenter);
            RI_Radionuclide_sourceOV_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
            RI_Radionuclide_sourceOV_QLineEdit->setFont(panel->font_D_LE1);
            RI_Radionuclide_sourceOV_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("Enter radionuclide"));

            // panel->RIsourceList 접근
            QCompleter *completer = new QCompleter(panel->RIsourceList, this);
            completer->setCaseSensitivity(Qt::CaseInsensitive);
            RI_Radionuclide_sourceOV_QLineEdit->setCompleter(completer);

            hLayout->addWidget(RI_Title_sourceOV_QLabel);
            hLayout->addStretch();
            hLayout->addWidget(RI_Radionuclide_sourceOV_QLineEdit);

            vLayout->addLayout(hLayout);
        }

        // [Activity UI]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setContentsMargins(panel->Window_width * 0.003, 0, panel->Window_width * 0.003, 0);
            hLayout->setSpacing(panel->Window_width * 0.003);
            hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

            RI_Activity_sourceOV_QLabel = new QLabel;
            RI_Activity_sourceOV_QLabel->setFixedSize(panel->Window_width * 0.068, panel->Window_height * 0.028);
            RI_Activity_sourceOV_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
            RI_Activity_sourceOV_QLabel->setFont(panel->font_D_LBL1);
            RI_Activity_sourceOV_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            RI_Activity_sourceOV_QLabel->setText(QString::fromLocal8Bit("Activity"));

            RI_Activity_sourceOV_QLineEdit = new QLineEdit;
            RI_Activity_sourceOV_QLineEdit->setFixedSize(panel->Window_width * 0.104, panel->Window_height * 0.028);
            RI_Activity_sourceOV_QLineEdit->setAlignment(Qt::AlignCenter);
            RI_Activity_sourceOV_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
            RI_Activity_sourceOV_QLineEdit->setFont(panel->font_D_LE1);
            RI_Activity_sourceOV_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("Bq/cm3"));

            hLayout->addWidget(RI_Activity_sourceOV_QLabel);
            hLayout->addStretch();
            hLayout->addWidget(RI_Activity_sourceOV_QLineEdit);

            vLayout->addLayout(hLayout);
        }

        // [Radionuclide Add Button]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setContentsMargins(panel->Window_width * 0.003, 0, panel->Window_width * 0.003, 0);
            hLayout->setSpacing(panel->Window_width * 0.003);
            hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

            RI_List_sourceOV_QLabel = new QLabel;
            RI_List_sourceOV_QLabel->setFixedSize(panel->Window_width * 0.088, panel->Window_height * 0.028);
            RI_List_sourceOV_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
            RI_List_sourceOV_QLabel->setFont(panel->font_D_LBL1);
            RI_List_sourceOV_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
            RI_List_sourceOV_QLabel->setText(QString::fromLocal8Bit("Entered sources"));

            sourceOV_RadionuclideAddButton = new QPushButton(QString::fromLocal8Bit("Add"));
            panel->Buttons_FunctionPanelRight.append(sourceOV_RadionuclideAddButton);
            sourceOV_RadionuclideAddButton->setFixedSize(panel->Window_width * 0.042, panel->Window_height * 0.037);
            sourceOV_RadionuclideAddButton->setCheckable(false);
            sourceOV_RadionuclideAddButton->setStyleSheet(DialogStyle::MENU_BTN2);
            sourceOV_RadionuclideAddButton->setFont(panel->font_D_BTN2);
            sourceOV_RadionuclideAddButton->setChecked(false);

            connect(sourceOV_RadionuclideAddButton, SIGNAL(clicked()), this, SLOT(slot_sourceOV_RadionuclideAdd_ButtonClicked())); 
            connect(sourceOV_RadionuclideAddButton, SIGNAL(clicked()), panel, SLOT(slot_allButtonClicked()));

            hLayout->addWidget(RI_List_sourceOV_QLabel);
            hLayout->addStretch();
            hLayout->addWidget(sourceOV_RadionuclideAddButton);

            vLayout->addLayout(hLayout);
        }

        // [Radionuclide List Widget]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setContentsMargins(panel->Window_width * 0.003, 0, panel->Window_width * 0.003, 0);
            hLayout->setSpacing(panel->Window_width * 0.003);
            hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

            RI_List_sourceOV_QListWidget = new QListWidget;
            RI_List_sourceOV_QListWidget->setStyleSheet("QListWidget { border-radius: 15px; background-color: white; }");
            RI_List_sourceOV_QListWidget->setFixedSize(panel->Window_width * 0.175, panel->Window_height * 0.155);
            RI_List_sourceOV_QListWidget->setFont(panel->font_D_LW1);

            hLayout->addWidget(RI_List_sourceOV_QListWidget);

            vLayout->addLayout(hLayout);
        }

        // [Energy Spectrum - Load File]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.010, panel->Window_width * 0.003, 0);
            hLayout->setSpacing(panel->Window_width * 0.003);
            hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

            ES_Title_sourceOV_QLabel = new QLabel;
            ES_Title_sourceOV_QLabel->setFixedSize(panel->Window_width * 0.095, panel->Window_height * 0.028);
            ES_Title_sourceOV_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
            ES_Title_sourceOV_QLabel->setFont(panel->font_D_LBL1);
            ES_Title_sourceOV_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            ES_Title_sourceOV_QLabel->setText(QString::fromLocal8Bit("Energy spectrum file"));
            ES_Title_sourceOV_QLabel->hide();

            sourceOV_EnergyspectrumFileLoadButton = new QPushButton(QString::fromLocal8Bit("Load file"));
            panel->Buttons_FunctionPanelRight.append(sourceOV_EnergyspectrumFileLoadButton);
            sourceOV_EnergyspectrumFileLoadButton->setFixedSize(panel->Window_width * 0.067, panel->Window_height * 0.028);
            sourceOV_EnergyspectrumFileLoadButton->setStyleSheet(DialogStyle::MENU_BTN11);
            sourceOV_EnergyspectrumFileLoadButton->setFont(panel->font_D_BTN11);
            sourceOV_EnergyspectrumFileLoadButton->hide();

            connect(sourceOV_EnergyspectrumFileLoadButton, SIGNAL(clicked()), this, SLOT(slot_sourceOV_EnergyspectrumFileLoad_ButtonClicked())); 
            connect(sourceOV_EnergyspectrumFileLoadButton, SIGNAL(clicked()), panel, SLOT(slot_allButtonClicked()));

            hLayout->addWidget(ES_Title_sourceOV_QLabel);
            hLayout->addWidget(sourceOV_EnergyspectrumFileLoadButton);

            vLayout->addLayout(hLayout);
        }

        // [Energy Spectrum - File Name]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setContentsMargins(panel->Window_width * 0.003, 0, panel->Window_width * 0.003, 0);
            hLayout->setSpacing(panel->Window_width * 0.005);
            hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

            ES_FileName_sourceOV_QLabel = new QLabel;
            ES_FileName_sourceOV_QLabel->setFixedSize(panel->Window_width * 0.068, panel->Window_height * 0.028);
            ES_FileName_sourceOV_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
            ES_FileName_sourceOV_QLabel->setFont(panel->font_D_LBL1);
            ES_FileName_sourceOV_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            ES_FileName_sourceOV_QLabel->setText(QString::fromLocal8Bit("File name"));
            ES_FileName_sourceOV_QLabel->hide();

            sourceOV_EnergyspectrumFileNameText = new QTextEdit;
            sourceOV_EnergyspectrumFileNameText->setFixedSize(panel->Window_width * 0.104, panel->Window_height * 0.056);
            sourceOV_EnergyspectrumFileNameText->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            sourceOV_EnergyspectrumFileNameText->setStyleSheet("QTextEdit { background-color: rgb(255, 255, 255) }");
            sourceOV_EnergyspectrumFileNameText->setFont(panel->font_D_MAT);
            sourceOV_EnergyspectrumFileNameText->setPlaceholderText(QString::fromLocal8Bit("empty"));
            sourceOV_EnergyspectrumFileNameText->hide();

            hLayout->addWidget(ES_FileName_sourceOV_QLabel);
            hLayout->addStretch();
            hLayout->addWidget(sourceOV_EnergyspectrumFileNameText);

            vLayout->addLayout(hLayout);
        }

        // [Intensity UI]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setContentsMargins(panel->Window_width * 0.003, 0, panel->Window_width * 0.003, 0);
            hLayout->setSpacing(panel->Window_width * 0.003);
            hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

            ES_Intensity_sourceOV_QLabel = new QLabel;
            ES_Intensity_sourceOV_QLabel->setFixedSize(panel->Window_width * 0.068, panel->Window_height * 0.028);
            ES_Intensity_sourceOV_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
            ES_Intensity_sourceOV_QLabel->setFont(panel->font_D_LBL1);
            ES_Intensity_sourceOV_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            ES_Intensity_sourceOV_QLabel->setText(QString::fromLocal8Bit("Intensity"));
            ES_Intensity_sourceOV_QLabel->hide();

            ES_Intensity_sourceOV_QLineEdit = new QLineEdit;
            ES_Intensity_sourceOV_QLineEdit->setFixedSize(panel->Window_width * 0.104, panel->Window_height * 0.028);
            ES_Intensity_sourceOV_QLineEdit->setAlignment(Qt::AlignCenter);
            ES_Intensity_sourceOV_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
            ES_Intensity_sourceOV_QLineEdit->setFont(panel->font_D_LE1);
            ES_Intensity_sourceOV_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("particles/s/cm3"));
            ES_Intensity_sourceOV_QLineEdit->hide();

            hLayout->addWidget(ES_Intensity_sourceOV_QLabel);
            hLayout->addStretch();
            hLayout->addWidget(ES_Intensity_sourceOV_QLineEdit);

            vLayout->addLayout(hLayout);
        }

        // [Energy Spectrum Add Button]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setContentsMargins(panel->Window_width * 0.003, 0, panel->Window_width * 0.003, 0);
            hLayout->setSpacing(panel->Window_width * 0.003);
            hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

            ES_List_sourceOV_QLabel = new QLabel;
            ES_List_sourceOV_QLabel->setFixedSize(panel->Window_width * 0.088, panel->Window_height * 0.028);
            ES_List_sourceOV_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
            ES_List_sourceOV_QLabel->setFont(panel->font_D_LBL1);
            ES_List_sourceOV_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
            ES_List_sourceOV_QLabel->setText(QString::fromLocal8Bit("Entered sources"));
            ES_List_sourceOV_QLabel->hide();

            sourceOV_EnergyspectrumAddButton = new QPushButton(QString::fromLocal8Bit("Add"));
            panel->Buttons_FunctionPanelRight.append(sourceOV_EnergyspectrumAddButton);
            sourceOV_EnergyspectrumAddButton->setFixedSize(panel->Window_width * 0.042, panel->Window_height * 0.037);
            sourceOV_EnergyspectrumAddButton->setCheckable(false);
            sourceOV_EnergyspectrumAddButton->setStyleSheet(DialogStyle::MENU_BTN2);
            sourceOV_EnergyspectrumAddButton->setFont(panel->font_D_BTN2);
            sourceOV_EnergyspectrumAddButton->setChecked(false);
            sourceOV_EnergyspectrumAddButton->hide();

            connect(sourceOV_EnergyspectrumAddButton, SIGNAL(clicked()), this, SLOT(slot_ESList_Add_sourceOV_ButtonClicked())); 
            connect(sourceOV_EnergyspectrumAddButton, SIGNAL(clicked()), panel, SLOT(slot_allButtonClicked()));

            hLayout->addWidget(ES_List_sourceOV_QLabel);
            hLayout->addStretch();
            hLayout->addWidget(sourceOV_EnergyspectrumAddButton);

            vLayout->addLayout(hLayout);
        }

        // [Energy Spectrum List]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setContentsMargins(panel->Window_width * 0.003, 0, panel->Window_width * 0.003, 0);
            hLayout->setSpacing(panel->Window_width * 0.003);
            hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

            ES_List_sourceOV_QListWidget = new QListWidget;
            ES_List_sourceOV_QListWidget->setStyleSheet("QListWidget { border-radius: 15px; background-color: white; }");
            ES_List_sourceOV_QListWidget->setFixedSize(panel->Window_width * 0.175, panel->Window_height * 0.155); 
            ES_List_sourceOV_QListWidget->setFont(panel->font_D_LW1);
            ES_List_sourceOV_QListWidget->hide();

            hLayout->addWidget(ES_List_sourceOV_QListWidget);

            vLayout->addLayout(hLayout);
        }

        subLayout->addLayout(vLayout);
    }
    
    this->setLayout(subLayout);
    return true;
}

void ObjectVolumeWidget::ExecuteSourceOVAddRoutines()
{
	if (b_IsSourceOV_AddingSettingClosed == true) // X버튼으로 나갔을때
	{
		b_IsSourceOV_AddingSettingClosed = false;
		return;
	}

	Object_sourceOV_SelectedIndex = m_Object_SequenceVector[m_comboBoxSetting_sourceOV_objectList->currentIndex()]; // 선택된 object index 설정

	// 이미 해당 object 선택되었을 때 종료
	auto it = std::find(m_sourceOV_objectSequenceVector.begin(), m_sourceOV_objectSequenceVector.end(), Object_sourceOV_SelectedIndex);
	if (it != m_sourceOV_objectSequenceVector.end()) 
	{
		theApp.SetMessageBox("This object is already set to the volume source");
		return;
	}

	// == Source Object버튼 생성 ==
	sourceOV_ObjectButton[Object_sourceOV_SelectedIndex] = new QPushButton(m_Object_ButtonName[Object_sourceOV_SelectedIndex]); // 구조물 이름 
	panel->Buttons_FunctionPanelRight.append(sourceOV_ObjectButton[Object_sourceOV_SelectedIndex]);
	sourceOV_ObjectButton[Object_sourceOV_SelectedIndex]->setObjectName(QString::number(Object_sourceOV_SelectedIndex));
	sourceOV_ObjectButton[Object_sourceOV_SelectedIndex]->setFixedSize(panel->Window_width * 0.05, panel->Window_height * 0.040);
	sourceOV_ObjectButton[Object_sourceOV_SelectedIndex]->setStyleSheet(DialogStyle::MENU_BTN7);
	sourceOV_ObjectButton[Object_sourceOV_SelectedIndex]->setFont(panel->font_D_BTN7);
	sourceOV_ObjectButton[Object_sourceOV_SelectedIndex]->setCheckable(true);
	sourceOV_ObjectButton[Object_sourceOV_SelectedIndex]->setChecked(true);
	connect(sourceOV_ObjectButton[Object_sourceOV_SelectedIndex], SIGNAL(clicked()), this, SLOT(slot_sourceOV_ObjectSelect_ButtonClicked())); connect(sourceOV_ObjectButton[Object_sourceOV_SelectedIndex], SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
	layout_sourceOV_ObjectList->addWidget(sourceOV_ObjectButton[Object_sourceOV_SelectedIndex]);

	for (auto itr : m_sourceOV_objectSequenceVector)
	{
		if (itr != Object_sourceOV_SelectedIndex)
		{
			sourceOV_ObjectButton[itr]->setChecked(false);
		}
	}

	// List 클리어 및 초기화
	RI_Radionuclide_sourceOV_QLineEdit->clear();
	RI_Activity_sourceOV_QLineEdit->clear();
	RI_List_sourceOV_QListWidget->clear();
	RI_ListDelete_sourceOV_QButton.clear();
	RI_ListInfo_sourceOV_QButton.clear();

	ES_Intensity_sourceOV_QLineEdit->clear();
	sourceOV_EnergyspectrumFileNameText->clear();
	ES_List_sourceOV_QListWidget->clear();	
	ES_ListDelete_sourceOV_QButton.clear();
	ES_ListInfo_sourceOV_QButton.clear();
	
	List_sourceOV_MakingIndex = 0;	 // 하나의 object에서의 List 방사선원 makingindex
	
	// Set global variables 
	m_sourceOV_objectSequenceVector.push_back(Object_sourceOV_SelectedIndex);

	theApp.ObjectPanelActors[Object_sourceOV_SelectedIndex]->GetProperty()->SetColor(1.0, 0., 0.);
	double* bounds = theApp.ObjectPanelActors[Object_sourceOV_SelectedIndex]->GetBounds();
	sourceOV_objectBound[Object_sourceOV_SelectedIndex][0] = bounds[0];
	sourceOV_objectBound[Object_sourceOV_SelectedIndex][1] = bounds[1];
	sourceOV_objectBound[Object_sourceOV_SelectedIndex][2] = bounds[2];
	sourceOV_objectBound[Object_sourceOV_SelectedIndex][3] = bounds[3];
	sourceOV_objectBound[Object_sourceOV_SelectedIndex][4] = bounds[4];
	sourceOV_objectBound[Object_sourceOV_SelectedIndex][5] = bounds[5];

	theApp.m_pVTKWidget->renderWindow()->Render();
}
void ObjectVolumeWidget::sourceOV_ObjectAddingSetting_Create()
{
	b_IsSourceOV_AddingSettingOKClicked = false;
	// Create a new dialog
	sourceOV_ObjectAddingDialog = new MultipleUIDialog(this);
	sourceOV_ObjectAddingDialog->setStyleSheet("background-color: rgb(250, 204, 207);");
	sourceOV_ObjectAddingDialog->setFixedWidth(panel->Window_width * 0.3);
	sourceOV_ObjectAddingDialog->setFixedHeight(panel->Window_height * 0.15);
	sourceOV_ObjectAddingDialog->setWindowTitle("Object volume source setting");
	sourceOV_ObjectAddingDialog->setFocusPolicy(Qt::StrongFocus);
	QScrollArea* scrollArea = new QScrollArea(sourceOV_ObjectAddingDialog);
	scrollArea->setWidgetResizable(true);
	scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	QWidget* containerWidget = new QWidget;
	QVBoxLayout* layout = new QVBoxLayout(containerWidget);

	{// Name
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.015, panel->Window_width * 0.003, 0);
		hLayout->setSpacing(panel->Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		QLabel* label0 = new QLabel;
		label0->setFixedSize(panel->Window_width * 0.090, panel->Window_height * 0.028);
		label0->setStyleSheet(DialogStyle::DATA_LABEL);
		label0->setFont(panel->font_D_LBL1);
		label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		label0->setText(QString::fromLocal8Bit("Object:"));

		m_comboBoxSetting_sourceOV_objectList = new QComboBox;
		m_comboBoxSetting_sourceOV_objectList->setFixedSize(panel->Window_width * 0.170, panel->Window_height * 0.028);
		m_comboBoxSetting_sourceOV_objectList->setStyleSheet(DialogStyle::COMBOBOX);
		m_comboBoxSetting_sourceOV_objectList->setFont(panel->font_D_CB1);

		if (m_Object_SequenceVector.size() != 0)
		{
			for (auto itr_objectList : m_Object_SequenceVector)
			{
				m_comboBoxSetting_sourceOV_objectList->addItem(m_Object_ButtonName[itr_objectList]);
			}
			m_comboBoxSetting_sourceOV_objectList->setCurrentIndex(0);
		}

		m_comboBoxSetting_sourceOV_objectList->setFocusPolicy(Qt::StrongFocus); // For focusing when scroll the comobox
		connect(m_comboBoxSetting_sourceOV_objectList, SIGNAL(activated(int)), this, SLOT(slot_sourceOV_ObjectSettingChangeCombo(int)));

		hLayout->addWidget(label0);
		hLayout->addWidget(m_comboBoxSetting_sourceOV_objectList);

		layout->addLayout(hLayout);
	}

	{
		QHBoxLayout* hLayout = new QHBoxLayout;
		hLayout->setSpacing(panel->Window_width * 0.003);
		hLayout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		hLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.015, panel->Window_width * 0.003, 0);

		m_sourceOV_SettingOKButton = new QPushButton(QString::fromLocal8Bit("OK"));
		panel->Buttons_FunctionPanelRight.append(m_sourceOV_SettingOKButton);
		m_sourceOV_SettingOKButton->setFixedSize(panel->Window_width * 0.104, panel->Window_height * 0.028);
		m_sourceOV_SettingOKButton->setCheckable(false);
		m_sourceOV_SettingOKButton->setStyleSheet(DialogStyle::MENU_BTN3);
		m_sourceOV_SettingOKButton->setFont(panel->font_D_BTN3);

		connect(m_sourceOV_SettingOKButton, SIGNAL(clicked()), this, SLOT(slot_sourceOV_SettingOK_ButtonClicked())); connect(m_sourceOV_SettingOKButton, SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));

		hLayout->addWidget(m_sourceOV_SettingOKButton);

		layout->addLayout(hLayout);
	}

	scrollArea->setWidget(containerWidget);
	QVBoxLayout* dialogLayout = new QVBoxLayout(sourceOV_ObjectAddingDialog);
	dialogLayout->addWidget(scrollArea);
	sourceOV_ObjectAddingDialog->setLayout(dialogLayout);
	sourceOV_ObjectAddingDialog->adjustSize();
}

// --- Slot Functions ---
void ObjectVolumeWidget::slot_ObjectVolumeSourceVisualization_ButtonClicked()
{
	b_IsSourceLocationVisualized_sourceOV = !b_IsSourceLocationVisualized_sourceOV;

}

// Object List Management
void ObjectVolumeWidget::slot_sourceOV_ObjectAdd_ButtonClicked()
{
	sourceOV_ObjectAddingSetting_Create(); // Adding setting dialog 실행
	sourceOV_ObjectAddingDialog->exec(); // Use exec() for a modal dialog or show() for a non-modal dialog
	// == Adding setting 완료 == 

	// == Add 루틴들 실행 ==
	ExecuteSourceOVAddRoutines();	
}
void ObjectVolumeWidget::slot_sourceOV_ObjectDelete_ButtonClicked()
{

}
void ObjectVolumeWidget::slot_sourceOV_ObjectSelect_ButtonClicked()
{
	QPushButton* buttonSender = qobject_cast<QPushButton*>(sender());
	
	// 원래 버튼 클릭 시 다시 check하고 돌아가기
	if (buttonSender->objectName().toInt() == Object_sourceOV_SelectedIndex)
	{
		sourceOV_ObjectButton[Object_sourceOV_SelectedIndex]->setChecked(true);
		return;
	}
	
	// List 클리어 및 초기화
	RI_Radionuclide_sourceOV_QLineEdit->clear();
	RI_Activity_sourceOV_QLineEdit->clear();
	RI_List_sourceOV_QListWidget->clear();
	RI_ListDelete_sourceOV_QButton.clear();
	RI_ListInfo_sourceOV_QButton.clear();
	
	ES_Intensity_sourceOV_QLineEdit->clear();
	sourceOV_EnergyspectrumFileNameText->clear();
	ES_List_sourceOV_QListWidget->clear();
	ES_ListDelete_sourceOV_QButton.clear();
	ES_ListInfo_sourceOV_QButton.clear();
	
	List_sourceOV_MakingIndex = 0;	 // 하나의 object에서의 List 방사선원 makingindex

	////////////////// seleceted index 설정/////////////////////
	Object_sourceOV_SelectedIndex = buttonSender->objectName().toInt();
	int id = Object_sourceOV_SelectedIndex;
	
	// 버튼 업데이트
	for (auto itr : m_sourceOV_objectSequenceVector)
	{
		if (itr != id)
		{
			sourceOV_ObjectButton[itr]->setChecked(false);
		}
	}
		
	// List 업데이트
	for (auto itr : ListInfo_sourceOV[Object_sourceOV_SelectedIndex])
	{
		
		if (itr[1].toStdString().find_first_of("/\\:") == std::string::npos)// ListInfo 각 요소에서 경로구분자가 없으면 RI
		{
			
			b_sourceOV_InObjectSelect = true;
			slot_sourceOV_RadionuclideAdd_ButtonClicked();
			b_sourceOV_InObjectSelect = false;
		}
		else if (itr[1].toStdString().find_first_of("/\\:") != std::string::npos)// ListInfo 각 요소에서 경로구분자가 있으면 ES
		{
			
			b_sourceOV_InObjectSelect = true;
			slot_ESList_Add_sourceOV_ButtonClicked();
			b_sourceOV_InObjectSelect = false;
		}
	}
	
}


// Type Selection
void ObjectVolumeWidget::slot_sourceOV_RadionuclideSelect_RadioButtonClicked()
{
	sourceOV_Radionuclide_radioButton->setChecked(true);
	sourceOV_Energyspectrum_radioButton->setChecked(false);

	RI_Title_sourceOV_QLabel->show();
	RI_Radionuclide_sourceOV_QLineEdit->show();
	RI_Activity_sourceOV_QLabel->show();
	RI_Activity_sourceOV_QLineEdit->show();
	sourceOV_RadionuclideAddButton->show();
	RI_List_sourceOV_QListWidget->show();
	RI_List_sourceOV_QLabel->show();

	ES_Title_sourceOV_QLabel->hide();
	sourceOV_EnergyspectrumFileLoadButton->hide();
	ES_Intensity_sourceOV_QLabel->hide();
	ES_Intensity_sourceOV_QLineEdit->hide();
	ES_FileName_sourceOV_QLabel->hide();
	sourceOV_EnergyspectrumFileNameText->hide();
	ES_List_sourceOV_QLabel->hide();
	sourceOV_EnergyspectrumAddButton->hide();
	ES_List_sourceOV_QListWidget->hide();

	// === ES 관련 초기화 ===

	// 2. QListWidget의 모든 아이템 제거
	ES_List_sourceOV_QListWidget->clear();

	// 3. 관리하던 모든 vector의 내용 비우기
	ListInfo_sourceOV[Object_sourceOV_SelectedIndex].clear();
	ES_ListDelete_sourceOV_QButton.clear();
	ES_ListInfo_sourceOV_QButton.clear();

	// 4. 인덱스 카운터 초기화
	List_sourceOV_MakingIndex = 0;
}
void ObjectVolumeWidget::slot_sourceOV_EnergySpectrumSelect_RadioButtonClicked()
{
	sourceOV_Radionuclide_radioButton->setChecked(false);
	sourceOV_Energyspectrum_radioButton->setChecked(true);

	RI_Title_sourceOV_QLabel->hide();
	RI_Radionuclide_sourceOV_QLineEdit->hide();
	RI_Activity_sourceOV_QLabel->hide();
	RI_Activity_sourceOV_QLineEdit->hide();
	sourceOV_RadionuclideAddButton->hide();
	RI_List_sourceOV_QListWidget->hide();
	RI_List_sourceOV_QLabel->hide();

	ES_Title_sourceOV_QLabel->show();
	sourceOV_EnergyspectrumFileLoadButton->show();
	ES_Intensity_sourceOV_QLabel->show();
	ES_Intensity_sourceOV_QLineEdit->show();
	ES_FileName_sourceOV_QLabel->show();
	sourceOV_EnergyspectrumFileNameText->show();
	ES_List_sourceOV_QLabel->show();
	sourceOV_EnergyspectrumAddButton->show();
	ES_List_sourceOV_QListWidget->show();

	// === RI 관련 초기화 ===

	// 2. QListWidget의 모든 아이템 제거
	RI_List_sourceOV_QListWidget->clear();

	// 3. 관리하던 모든 vector의 내용 비우기
	ListInfo_sourceOV[Object_sourceOV_SelectedIndex].clear();
	RI_ListDelete_sourceOV_QButton.clear();
	RI_ListInfo_sourceOV_QButton.clear();

	// 4. 인덱스 카운터 초기화
	List_sourceOV_MakingIndex = 0;
}

// Dialog Interaction
void ObjectVolumeWidget::slot_sourceOV_ObjectSettingChangeCombo(int idx)
{
	m_comboBoxSetting_sourceOV_objectList->hide();	m_comboBoxSetting_sourceOV_objectList->show(); // This is for focusing (Do not show white blank)
}
void ObjectVolumeWidget::slot_sourceOV_SettingOK_ButtonClicked()
{	
	// AddingOK에 Setting widget 정상 종료를 위해 반드시 필요함
	b_IsSourceOV_AddingSettingOKClicked = true;
	sourceOV_ObjectAddingDialog->close();
}

// Radionuclide Operations
void ObjectVolumeWidget::slot_sourceOV_RadionuclideAdd_ButtonClicked()
{
	if (b_sourceOV_InObjectSelect == false) // 새롭게 추가한 상황에서만 판단
	{
		// 지원하는 RI 리스트에 있는지 판별
		QString inputText = RI_Radionuclide_sourceOV_QLineEdit->text().trimmed();
		if (inputText.isEmpty()) {
			theApp.SetMessageBox("Radionuclide name is empty!");
			return;
		}

		// 1) 대소문자 구분하여 리스트에 있는지 검사
		if (!panel->RIsourceList.contains(inputText, Qt::CaseSensitive)) {
			// 2) 자동 보정: 첫 글자만 대문자, 나머지는 소문자
			QString corrected = inputText.toLower();
			if (!corrected.isEmpty()) {
				corrected[0] = corrected[0].toUpper();
			}
			// 보정된 문자열이 리스트에 있으면 QLineEdit에 반영
			if (panel->RIsourceList.contains(corrected, Qt::CaseSensitive)) {
				RI_Radionuclide_sourceOV_QLineEdit->setText(corrected);
				inputText = corrected;
			}
			else {
				// 보정해도 없으면 에러
				theApp.SetMessageBox(inputText + ": Radionuclide name error!");
				return;
			}
		}
	}
	
	QString str_RI; 
	QString str_activity; 
	if (b_sourceOV_InObjectSelect == false) // 새롭게 추가하는 상황
	{
		str_RI = RI_Radionuclide_sourceOV_QLineEdit->text();
		str_activity = RI_Activity_sourceOV_QLineEdit->text();
	}
	else if (b_sourceOV_InObjectSelect == true) // object select한 상황
	{
		str_RI = ListInfo_sourceOV[Object_sourceOV_SelectedIndex][List_sourceOV_MakingIndex][1];
		str_activity = ListInfo_sourceOV[Object_sourceOV_SelectedIndex][List_sourceOV_MakingIndex][2];
	}		
	
	QListWidgetItem* item = new QListWidgetItem(RI_List_sourceOV_QListWidget);
	QWidget* widget = new QWidget;
	QHBoxLayout* hLayout = new QHBoxLayout;
	widget->setStyleSheet("background-color: white;");

	QString arg1 = str_RI;
	QLabel* label1 = new QLabel(arg1);
	label1->setFixedWidth(panel->Window_width * 0.025);
	label1->setFont(panel->font_D_LW2); // Set font for the label

	QString arg2 = QString::number(str_activity.toDouble(), 'e', 3);
	QLabel* label2 = new QLabel(arg2);
	label2->setFixedWidth(panel->Window_width * 0.025);
	label2->setFont(panel->font_D_LW2); // Set font for the label

	QString arg3 = "";
	QLabel* labelPosX = new QLabel(arg3);
	labelPosX->setFixedWidth(panel->Window_width * 0.025);
	labelPosX->setFont(panel->font_D_LW2); // Set font for the label

	QString arg4 = "";
	QLabel* labelPosY = new QLabel(arg4);
	labelPosY->setFixedWidth(panel->Window_width * 0.025);
	labelPosY->setFont(panel->font_D_LW2); // Set font for the label

	QString arg5 = "";
	QLabel* labelPosZ = new QLabel(arg5);
	labelPosZ->setFixedWidth(panel->Window_width * 0.025);
	labelPosZ->setFont(panel->font_D_LW2); // Set font for the label

	RI_ListDelete_sourceOV_QButton.push_back(new QPushButton());	
	RI_ListDelete_sourceOV_QButton[List_sourceOV_MakingIndex]->setCheckable(false);
	RI_ListDelete_sourceOV_QButton[List_sourceOV_MakingIndex]->setStyleSheet(DialogStyle::MENU_BTN13);
	RI_ListDelete_sourceOV_QButton[List_sourceOV_MakingIndex]->setFont(panel->font_D_BTN13);
	RI_ListDelete_sourceOV_QButton[List_sourceOV_MakingIndex]->setFixedSize(panel->Window_width * 0.008, panel->Window_height * 0.012);
	RI_ListDelete_sourceOV_QButton[List_sourceOV_MakingIndex]->setIcon(QIcon("./data/image/delete.png"));
	RI_ListDelete_sourceOV_QButton[List_sourceOV_MakingIndex]->setObjectName(QString::number(List_sourceOV_MakingIndex));

	RI_ListInfo_sourceOV_QButton.push_back(new QPushButton());
	RI_ListInfo_sourceOV_QButton[List_sourceOV_MakingIndex]->setCheckable(false);
	RI_ListInfo_sourceOV_QButton[List_sourceOV_MakingIndex]->setStyleSheet(DialogStyle::MENU_BTN13);
	RI_ListInfo_sourceOV_QButton[List_sourceOV_MakingIndex]->setFont(panel->font_D_BTN13);
	RI_ListInfo_sourceOV_QButton[List_sourceOV_MakingIndex]->setFixedSize(panel->Window_width * 0.008, panel->Window_height * 0.012);
	RI_ListInfo_sourceOV_QButton[List_sourceOV_MakingIndex]->setIcon(QIcon("./data/image/info.png"));
	RI_ListInfo_sourceOV_QButton[List_sourceOV_MakingIndex]->setObjectName(QString::number(List_sourceOV_MakingIndex));
	
	hLayout->addWidget(label1);
	hLayout->addWidget(label2);
	hLayout->addWidget(labelPosX);
	hLayout->addWidget(labelPosY);
	hLayout->addWidget(labelPosZ);
	hLayout->addWidget(RI_ListDelete_sourceOV_QButton[List_sourceOV_MakingIndex]);
	hLayout->addWidget(RI_ListInfo_sourceOV_QButton[List_sourceOV_MakingIndex]);
	widget->setLayout(hLayout);

	RI_List_sourceOV_QListWidget->setItemWidget(item, widget);
	item->setSizeHint(widget->sizeHint());
	
	// Connect signals of the new buttons
	connect(RI_ListDelete_sourceOV_QButton[List_sourceOV_MakingIndex], SIGNAL(clicked()), this, SLOT(slot_sourceOV_RadionuclideDelete_ButtonClicked())); connect(RI_ListDelete_sourceOV_QButton[List_sourceOV_MakingIndex], SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
	connect(RI_ListInfo_sourceOV_QButton[List_sourceOV_MakingIndex], SIGNAL(clicked()), this, SLOT(slot_sourceOV_RadionuclideInfo_ButtonClicked())); connect(RI_ListInfo_sourceOV_QButton[List_sourceOV_MakingIndex], SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
	
	// Set Global Variables
	if (b_sourceOV_InObjectSelect == false) // 새롭게 추가하는 상황
	{
		std::map<int, QString> tmp_map;
		tmp_map[0] = "True";
		tmp_map[1] = str_RI;
		tmp_map[2] = str_activity; if (tmp_map[2] == "") tmp_map[2] = "0";
		ListInfo_sourceOV[Object_sourceOV_SelectedIndex].push_back(tmp_map);

		theApp.SetMessageBox_RadionuclideWarning("The radionuclide data provided in this code are based on the data provided in the ICRP 107 publication.\nTHIS DATA DO NOT INCLUDE ANY DAUGHTER NUCLIDES!!!");
	}
	List_sourceOV_MakingIndex++;	

	// Set panel info 
	RI_Radionuclide_sourceOV_QLineEdit->clear();
	RI_Activity_sourceOV_QLineEdit->clear();
}
void ObjectVolumeWidget::slot_sourceOV_RadionuclideDelete_ButtonClicked()
{
	// Button의 Index 찾기
	QPushButton* buttonSender = qobject_cast<QPushButton*>(sender());
	QString buttonText = buttonSender->objectName();

	// The first part is the ID
	int RadonuclideIndex = buttonText.toDouble();
	int row = RI_List_sourceOV_QListWidget->row(RI_List_sourceOV_QListWidget->item(RadonuclideIndex));
	QListWidgetItem* toRemove = RI_List_sourceOV_QListWidget->takeItem(row);
	delete toRemove; // Delete the item to free memory

	if (RadonuclideIndex >= 0 && RadonuclideIndex < ListInfo_sourceOV[Object_sourceOV_SelectedIndex].size()) {
		ListInfo_sourceOV[Object_sourceOV_SelectedIndex].erase(ListInfo_sourceOV[Object_sourceOV_SelectedIndex].begin() + RadonuclideIndex);
		RI_ListDelete_sourceOV_QButton.erase(RI_ListDelete_sourceOV_QButton.begin() + RadonuclideIndex);
		RI_ListInfo_sourceOV_QButton.erase(RI_ListInfo_sourceOV_QButton.begin() + RadonuclideIndex);
	}
	else {
		theApp.SetMessageBox("RI_list_Info_sourceOV Vecetor index error");
	}

	for (int index = 0; index < RI_ListDelete_sourceOV_QButton.size(); index++)
	{
		RI_ListDelete_sourceOV_QButton[index]->setObjectName(QString::number(index));
		RI_ListInfo_sourceOV_QButton[index]->setObjectName(QString::number(index));
	}
	List_sourceOV_MakingIndex--; // listwidget은 vector 형태로 동작해야 함(row의 item이 실제 몇 행인지로 결정됨)	
}
void ObjectVolumeWidget::slot_sourceOV_RadionuclideInfo_ButtonClicked()
{
	// Button의 Index 찾기
	QPushButton* buttonSender = qobject_cast<QPushButton*>(sender());
	QString buttonText = buttonSender->objectName();

	QStringList list_button = buttonText.split("_");

	// The first part is the ID
	int RadonuclideIndex = list_button[0].toInt();
	QListWidgetItem* item = RI_List_sourceOV_QListWidget->item(RadonuclideIndex);
	if (!item) {
		// Handle error: item not found
		return;
	}

	QWidget* widget = RI_List_sourceOV_QListWidget->itemWidget(item);
	QHBoxLayout* hLayout = qobject_cast<QHBoxLayout*>(widget->layout());
	if (!hLayout) {
		// Handle error: layout not found
		return;
	}

	// Assuming the order of labels in the layout is known
	QLabel* labelRadionuclide = qobject_cast<QLabel*>(hLayout->itemAt(0)->widget());
	QLabel* labelActivity = qobject_cast<QLabel*>(hLayout->itemAt(1)->widget());

	// Create a new dialog
	QDialog* infoDialog = new QDialog(this);
	infoDialog->setStyleSheet("background-color: white;");
	infoDialog->resize(panel->Window_width * 0.3, panel->Window_height * 0.55);
	infoDialog->setWindowTitle("Radionuclide Information");

	// Create a layout and a label for the dialog
	QVBoxLayout* layout = new QVBoxLayout(infoDialog);
	// Create a QTextEdit for the dialog
	QTextEdit* textEdit = new QTextEdit(infoDialog);
	textEdit->setReadOnly(true); // Make it read-only if editing is not required

	// Create a QTextCursor for inserting formatted text
	QTextCursor cursor = textEdit->textCursor();

	// Define different text formats
	QTextCharFormat boldFormat;
	boldFormat.setFontFamily("Courier New"); // This is monospaced font
	boldFormat.setFontWeight(QFont::Bold);
	boldFormat.setFontPointSize(14);

	QTextCharFormat boldMediumFormat;
	boldMediumFormat.setFontFamily("Courier New");
	boldMediumFormat.setFontWeight(QFont::Bold);
	boldMediumFormat.setFontPointSize(12);

	QTextCharFormat boldSmallFormat;
	boldSmallFormat.setFontFamily("Courier New");
	boldSmallFormat.setFontWeight(QFont::Bold);
	boldSmallFormat.setFontPointSize(10);

	QTextCharFormat normalFormat;
	normalFormat.setFontFamily("Courier New");
	normalFormat.setFontPointSize(10);

	QTextCharFormat smallFormat;
	smallFormat.setFontFamily("Courier New");
	smallFormat.setFontPointSize(8);

	QTextCharFormat verySmallFormat;
	verySmallFormat.setFontFamily("Courier New");
	verySmallFormat.setFontPointSize(6);

	// Insert formatted text into QTextEdit
	cursor.insertText("*This radionuclide information is based on ICRP 107 Publication\n\n", smallFormat);
	cursor.insertText("Radionuclide Information\n", boldFormat);
	cursor.insertText("Radionuclide: " + labelRadionuclide->text() + "\n", normalFormat);
	cursor.insertText("Activity (Bq): " + labelActivity->text() + "\n", normalFormat);

	// Fine RN information from ICRP07.NDX file (ICRP 107 Pub.)
	QFile file("./data/radioisotopes/_ICRP-07.NDX");
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		// Handle error: Unable to open file
		return;
	}
	QTextStream in(&file);
	QString line;
	bool isDataFound = false;
	QString halfLife, daughterNucleus1, branchingFraction1, daughterNucleus2, branchingFraction2, daughterNucleus3, branchingFraction3, daughterNucleus4, branchingFraction4;
	int daughterNucleus1_index = 7; // 이상적으로 8열(Index: 7)에서 딸핵종1 시작

	while (!in.atEnd()) {
		line = in.readLine();
		if (line.trimmed().isEmpty()) continue; // Skip empty lines
		QStringList columns = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
		if (columns[0] == labelRadionuclide->text()) {
			isDataFound = true;
			halfLife = columns[1]; // Extract HalfLife
			bool isNumber; double tmpNum;
			tmpNum = columns[3].toDouble(&isNumber); // 이상적으로는 숫자, 하지만 DecayMode 알파벳이 할당되어 있을수도 있음
			if (!isNumber) daughterNucleus1_index++; // Decay Mode has assgined to two columns
			daughterNucleus1 = columns[daughterNucleus1_index];
			branchingFraction1 = columns[daughterNucleus1_index + 2];
			tmpNum = columns[daughterNucleus1_index + 3].toDouble(&isNumber);
			if (isNumber) break;// 이상적으로는 딸핵종, 하지만 딸핵종이 더 이상 없어 0이 할당되어 있을수도 있음

			daughterNucleus2 = columns[daughterNucleus1_index + 3];
			branchingFraction2 = columns[daughterNucleus1_index + 5];
			tmpNum = columns[daughterNucleus1_index + 6].toDouble(&isNumber);
			if (isNumber) break;// 이상적으로는 딸핵종, 하지만 딸핵종이 더 이상 없어 0이 할당되어 있을수도 있음

			daughterNucleus3 = columns[daughterNucleus1_index + 6];
			branchingFraction3 = columns[daughterNucleus1_index + 8];
			tmpNum = columns[daughterNucleus1_index + 9].toDouble(&isNumber);
			if (isNumber) break;// 이상적으로는 딸핵종, 하지만 딸핵종이 더 이상 없어 0이 할당되어 있을수도 있음

			daughterNucleus4 = columns[daughterNucleus1_index + 9];
			branchingFraction4 = columns[daughterNucleus1_index + 11];
			break; // Stop reading further as the data is found
		}
	}
	file.close();

	cursor.insertText("HalfLife: " + halfLife + "\n\n", normalFormat);
	cursor.insertText("Daughter Nuclides Information\n", boldFormat);
	cursor.insertText("Radionuclide   Fraction\n", boldSmallFormat);

	// Insert text into QTextEdit with padding
	if (!daughterNucleus1.isEmpty()) {
		QString padding(15 - daughterNucleus1.length(), ' ');
		cursor.insertText(daughterNucleus1 + padding + branchingFraction1 + "\n", normalFormat);
	}
	if (!daughterNucleus2.isEmpty()) {
		QString padding(15 - daughterNucleus2.length(), ' ');
		cursor.insertText(daughterNucleus2 + padding + branchingFraction2 + "\n", normalFormat);
	}
	if (!daughterNucleus3.isEmpty()) {
		QString padding(15 - daughterNucleus3.length(), ' ');
		cursor.insertText(daughterNucleus3 + padding + branchingFraction3 + "\n", normalFormat);
	}
	if (!daughterNucleus4.isEmpty()) {
		QString padding(15 - daughterNucleus4.length(), ' ');
		cursor.insertText(daughterNucleus4 + padding + branchingFraction4 + "\n", normalFormat);
	}

	cursor.insertText("\nRadiation Spectrum of the Entered Radionuclide\n", boldFormat);
	cursor.insertText("Radiation      Fraction       Energy (MeV)\n", boldSmallFormat);

	QString RNspectrumFileName = ("./data/radioisotopes/" + labelRadionuclide->text() + ".txt");
	QFile RNspectrumFile(RNspectrumFileName);
	if (!RNspectrumFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		// Handle error: Unable to open file
		return;
	}

	QTextStream RNspectrumInput(&RNspectrumFile);
	QString RNspectrumLine;

	// Skip the first line
	if (!RNspectrumInput.atEnd()) {
		RNspectrumLine = RNspectrumInput.readLine();
	}
	
	while (!RNspectrumInput.atEnd()) {
		RNspectrumLine = RNspectrumInput.readLine();
		QStringList columns = RNspectrumLine.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);

		if (columns.size() >= 4) {
			QString radiation = columns[3];
			QString fraction = columns[1];
			QString energy = columns[2];

			QString padding1(15 - radiation.length(), ' ');
			QString padding2(15 - fraction.length(), ' ');
			cursor.insertText(radiation + padding1 + fraction + padding2 + energy + "\n", normalFormat);
		}
	}
	RNspectrumFile.close();

	cursor.insertText("\n*Alpha recoil nuclei and fission fragments are excluded.\n*PG, DG, and DB are radiations of spontaneous fission.\n", smallFormat);
	cursor.insertText("G: Gamma rays\n", verySmallFormat);
	cursor.insertText("PG: Prompt gamma rays\n", verySmallFormat);
	cursor.insertText("DG: Delayed gamma rays\n", verySmallFormat);
	cursor.insertText("X: X rays\n", verySmallFormat);
	cursor.insertText("AQ: Annihilation photons\n", verySmallFormat);
	cursor.insertText("B+: Beta-plus particles\n", verySmallFormat);
	cursor.insertText("B-: Beta-minus particles\n", verySmallFormat);
	cursor.insertText("DB: Delayed beta particles\n", verySmallFormat);
	cursor.insertText("IE: Internal conversion electrons\n", verySmallFormat);
	cursor.insertText("AE: Auger electrons\n", verySmallFormat);
	cursor.insertText("A: Alpha particles\n", verySmallFormat);
	cursor.insertText("N: Neutrons\n", verySmallFormat);

	cursor.movePosition(QTextCursor::Start);
	textEdit->setTextCursor(cursor);

	// Add the QTextEdit to the layout
	layout->addWidget(textEdit);

	// Set the layout to the dialog
	infoDialog->setLayout(layout);

	// Show the dialog
	infoDialog->exec(); // Use exec() for a modal dialog or show() for a non-modal dialog
}

// Energy Spectrum Operations
void ObjectVolumeWidget::slot_sourceOV_EnergyspectrumFileLoad_ButtonClicked()
{
	QString filter;
	filter = tr("(*.*)");
	QString dir = QFileDialog::getOpenFileName(
		this,
		tr("Select a file to open"),
		"",
		filter);

	if (!dir.isEmpty())
	{
		sourceOV_EnergyspectrumFileNameText->setText(dir);
		sourceOV_EnergyspectrumFileNameText->setReadOnly(true);
		QFont font;
		font.setFamily("Arial");    // Font family
		font.setPointSize(panel->FontSizeScaling(7));      // Font size
		sourceOV_EnergyspectrumFileNameText->setFont(font);
	}
}
void ObjectVolumeWidget::slot_ESList_Add_sourceOV_ButtonClicked()
{
	if (b_sourceOV_InObjectSelect == false)
	{		
		QString inputText = sourceOV_EnergyspectrumFileNameText->toPlainText();
		if (inputText.isEmpty()) {
			theApp.SetMessageBox("Energy spectrum file is empty!");
			return;
		}
	}
	
	QString str_ES;
	QString str_intensity;
	if (b_sourceOV_InObjectSelect == false) // 새롭게 추가하는 상황
	{
		str_ES = sourceOV_EnergyspectrumFileNameText->toPlainText();
		str_intensity = ES_Intensity_sourceOV_QLineEdit->text();
	}
	else if (b_sourceOV_InObjectSelect == true) // object select한 상황
	{
		str_ES = ListInfo_sourceOV[Object_sourceOV_SelectedIndex][List_sourceOV_MakingIndex][1];
		str_intensity = ListInfo_sourceOV[Object_sourceOV_SelectedIndex][List_sourceOV_MakingIndex][2];
	}

	QListWidgetItem* item = new QListWidgetItem(ES_List_sourceOV_QListWidget);
	QWidget* widget = new QWidget;
	QHBoxLayout* hLayout = new QHBoxLayout;
	widget->setStyleSheet("background-color: white;");

	QString arg1 = QFileInfo(str_ES).fileName();
	QLabel* label1 = new QLabel(arg1);
	label1->setFixedWidth(panel->Window_width * 0.025);
	label1->setFont(panel->font_D_LW2); // Set font for the label
	label1->setToolTip(arg1); // 전체 파일명 툴팁
	label1->setScaledContents(false);
	label1->setTextInteractionFlags(Qt::TextSelectableByMouse); // 선택 가능 (옵션)

	QString arg2 = QString::number(str_intensity.toDouble(), 'e', 3);
	QLabel* label2 = new QLabel(arg2);
	label2->setFixedWidth(panel->Window_width * 0.025);
	label2->setFont(panel->font_D_LW2); // Set font for the label

	QString arg3 = "";
	QLabel* labelPosX = new QLabel(arg3);
	labelPosX->setFixedWidth(panel->Window_width * 0.025);
	labelPosX->setFont(panel->font_D_LW2); // Set font for the label

	QString arg4 = "";
	QLabel* labelPosY = new QLabel(arg4);
	labelPosY->setFixedWidth(panel->Window_width * 0.025);
	labelPosY->setFont(panel->font_D_LW2); // Set font for the label

	QString arg5 = "";
	QLabel* labelPosZ = new QLabel(arg5);
	labelPosZ->setFixedWidth(panel->Window_width * 0.025);
	labelPosZ->setFont(panel->font_D_LW2); // Set font for the label

	ES_ListDelete_sourceOV_QButton.push_back(new QPushButton());
	if (List_sourceOV_MakingIndex >= ES_ListDelete_sourceOV_QButton.size()) theApp.SetMessageBox("m_sourceOV_ES_listDeleteButton Vector index error");
	ES_ListDelete_sourceOV_QButton[List_sourceOV_MakingIndex]->setCheckable(false);
	ES_ListDelete_sourceOV_QButton[List_sourceOV_MakingIndex]->setStyleSheet(DialogStyle::MENU_BTN13);
	ES_ListDelete_sourceOV_QButton[List_sourceOV_MakingIndex]->setFont(panel->font_D_BTN13);
	ES_ListDelete_sourceOV_QButton[List_sourceOV_MakingIndex]->setFixedSize(panel->Window_width * 0.008, panel->Window_height * 0.012);
	ES_ListDelete_sourceOV_QButton[List_sourceOV_MakingIndex]->setIcon(QIcon("./data/image/delete.png"));
	ES_ListDelete_sourceOV_QButton[List_sourceOV_MakingIndex]->setObjectName(QString::number(List_sourceOV_MakingIndex));

	ES_ListInfo_sourceOV_QButton.push_back(new QPushButton());
	if (List_sourceOV_MakingIndex >= ES_ListInfo_sourceOV_QButton.size()) theApp.SetMessageBox("m_sourceOV_ES_listInfoButton Vector index error");
	ES_ListInfo_sourceOV_QButton[List_sourceOV_MakingIndex]->setCheckable(false);
	ES_ListInfo_sourceOV_QButton[List_sourceOV_MakingIndex]->setStyleSheet(DialogStyle::MENU_BTN13);
	ES_ListInfo_sourceOV_QButton[List_sourceOV_MakingIndex]->setFont(panel->font_D_BTN13);
	ES_ListInfo_sourceOV_QButton[List_sourceOV_MakingIndex]->setFixedSize(panel->Window_width * 0.008, panel->Window_height * 0.012);
	ES_ListInfo_sourceOV_QButton[List_sourceOV_MakingIndex]->setIcon(QIcon("./data/image/info.png"));
	ES_ListInfo_sourceOV_QButton[List_sourceOV_MakingIndex]->setObjectName(QString::number(List_sourceOV_MakingIndex));

	hLayout->addWidget(label1);
	hLayout->addWidget(label2);
	hLayout->addWidget(labelPosX);
	hLayout->addWidget(labelPosY);
	hLayout->addWidget(labelPosZ);
	hLayout->addWidget(ES_ListDelete_sourceOV_QButton[List_sourceOV_MakingIndex]);
	hLayout->addWidget(ES_ListInfo_sourceOV_QButton[List_sourceOV_MakingIndex]);
	widget->setLayout(hLayout);

	ES_List_sourceOV_QListWidget->setItemWidget(item, widget);
	item->setSizeHint(widget->sizeHint());

	// Connect signals of the new buttons
	connect(ES_ListDelete_sourceOV_QButton[List_sourceOV_MakingIndex], SIGNAL(clicked()), this, SLOT(slot_ESList_Delete_sourceOV_ButtonClicked())); connect(ES_ListDelete_sourceOV_QButton[List_sourceOV_MakingIndex], SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
	connect(ES_ListInfo_sourceOV_QButton[List_sourceOV_MakingIndex], SIGNAL(clicked()), this, SLOT(slot_ESList_Info_sourceOV_ButtonClicked())); connect(ES_ListInfo_sourceOV_QButton[List_sourceOV_MakingIndex], SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));

	// Set Global Variables
	if (b_sourceOV_InObjectSelect == false) // 새롭게 추가하는 상황
	{
		std::map<int, QString> tmp_map;
		tmp_map[0] = "True";
		tmp_map[1] = str_ES;
		tmp_map[2] = str_intensity; if (tmp_map[2] == "") tmp_map[2] = "0";
		ListInfo_sourceOV[Object_sourceOV_SelectedIndex].push_back(tmp_map);
	}
	List_sourceOV_MakingIndex++;

	// Set panel info 
	ES_Intensity_sourceOV_QLineEdit->clear();
	sourceOV_EnergyspectrumFileNameText->clear();
}
void ObjectVolumeWidget::slot_ESList_Delete_sourceOV_ButtonClicked()
{
	// Button의 Index 찾기
	QPushButton* buttonSender = qobject_cast<QPushButton*>(sender());
	QString buttonText = buttonSender->objectName();

	// The first part is the ID
	int EnersySpectrumIndex = buttonText.toDouble();
	int row = ES_List_sourceOV_QListWidget->row(ES_List_sourceOV_QListWidget->item(EnersySpectrumIndex));
	QListWidgetItem* toRemove = ES_List_sourceOV_QListWidget->takeItem(row);
	delete toRemove; // Delete the item to free memory

	if (EnersySpectrumIndex >= 0 && EnersySpectrumIndex < ListInfo_sourceOV[Object_sourceOV_SelectedIndex].size())
	{
		ListInfo_sourceOV[Object_sourceOV_SelectedIndex].erase(ListInfo_sourceOV[Object_sourceOV_SelectedIndex].begin() + EnersySpectrumIndex);
		ES_ListDelete_sourceOV_QButton.erase(ES_ListDelete_sourceOV_QButton.begin() + EnersySpectrumIndex);
		ES_ListInfo_sourceOV_QButton.erase(ES_ListInfo_sourceOV_QButton.begin() + EnersySpectrumIndex);

	}
	else {
		theApp.SetMessageBox("sourceOV_info Vecetor index error");
	}

	for (int index = 0; index < ES_ListDelete_sourceOV_QButton.size(); index++)
	{
		ES_ListDelete_sourceOV_QButton[index]->setObjectName(QString::number(index));
		ES_ListInfo_sourceOV_QButton[index]->setObjectName(QString::number(index));
	}
	List_sourceOV_MakingIndex--; // listwidget은 vector 형태로 동작해야 함(row의 item이 실제 몇 행인지로 결정됨)	
}
void ObjectVolumeWidget::slot_ESList_Info_sourceOV_ButtonClicked()
{
	// Button의 Index 찾기
	QPushButton* buttonSender = qobject_cast<QPushButton*>(sender());
	QString buttonText = buttonSender->objectName();

	QStringList list_button = buttonText.split("_");

	// The first part is the ID
	int EnergySpectrumIndex = list_button[0].toInt();

	QListWidgetItem* item = ES_List_sourceOV_QListWidget->item(EnergySpectrumIndex);
	if (!item) {
		// Handle error: item not found
		return;
	}

	QWidget* widget = ES_List_sourceOV_QListWidget->itemWidget(item);
	QHBoxLayout* hLayout = qobject_cast<QHBoxLayout*>(widget->layout());
	if (!hLayout) {
		// Handle error: layout not found
		return;
	}

	// Assuming the order of labels in the layout is known
	QLabel* labelEnergySpectrumFile = qobject_cast<QLabel*>(hLayout->itemAt(0)->widget());
	QLabel* labelIntensity = qobject_cast<QLabel*>(hLayout->itemAt(1)->widget());

	// Create a new dialog
	QDialog* infoDialog = new QDialog(this);
	infoDialog->setStyleSheet("background-color: white;");
	infoDialog->resize(panel->Window_width * 0.3, panel->Window_height * 0.55);
	infoDialog->setWindowTitle("Radionuclide Information");

	// Create a layout and a label for the dialog
	QVBoxLayout* layout = new QVBoxLayout(infoDialog);
	// Create a QTextEdit for the dialog
	QTextEdit* textEdit = new QTextEdit(infoDialog);
	textEdit->setReadOnly(true); // Make it read-only if editing is not required

	// Create a QTextCursor for inserting formatted text
	QTextCursor cursor = textEdit->textCursor();

	// Define different text formats
	QTextCharFormat boldFormat;
	boldFormat.setFontFamily("Courier New"); // This is monospaced font
	boldFormat.setFontWeight(QFont::Bold);
	boldFormat.setFontPointSize(14);

	QTextCharFormat boldMediumFormat;
	boldMediumFormat.setFontFamily("Courier New");
	boldMediumFormat.setFontWeight(QFont::Bold);
	boldMediumFormat.setFontPointSize(12);

	QTextCharFormat boldSmallFormat;
	boldSmallFormat.setFontFamily("Courier New");
	boldSmallFormat.setFontWeight(QFont::Bold);
	boldSmallFormat.setFontPointSize(10);

	QTextCharFormat normalFormat;
	normalFormat.setFontFamily("Courier New");
	normalFormat.setFontPointSize(10);

	QTextCharFormat smallFormat;
	smallFormat.setFontFamily("Courier New");
	smallFormat.setFontPointSize(8);

	QTextCharFormat verySmallFormat;
	verySmallFormat.setFontFamily("Courier New");
	verySmallFormat.setFontPointSize(6);

	// Insert formatted text into QTextEdit	
	cursor.insertText("EnergySpectrum Information\n", boldFormat);
	cursor.insertText("FileName: " + labelEnergySpectrumFile->text() + "\n", normalFormat);
	cursor.insertText("Intensity (#/s): " + labelIntensity->text() + "\n", normalFormat);

	cursor.insertText("\nRadiation Spectrum of the Entered File\n", boldFormat);
	cursor.insertText("Radiation      Fraction       Energy (MeV)\n", boldSmallFormat);

	QString ESFileName = ListInfo_sourceOV[Object_sourceOV_SelectedIndex][EnergySpectrumIndex][1];
	QFile ESFile(ESFileName);
	if (!ESFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		// Handle error: Unable to open file
		return;
	}

	QTextStream ESInput(&ESFile);
	while (!ESInput.atEnd()) {
		QString ESLine = ESInput.readLine();
		QStringList columns = ESLine.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);

		if (columns.size() >= 3) {
			QString radiation = columns[0];
			QString fraction = columns[1];
			QString energy = columns[2];

			QString padding1(15 - radiation.length(), ' ');
			QString padding2(15 - fraction.length(), ' ');
			cursor.insertText(radiation + padding1 + fraction + padding2 + energy + "\n", normalFormat);
		}
	}
	ESFile.close();

	cursor.movePosition(QTextCursor::Start);
	textEdit->setTextCursor(cursor);

	// Add the QTextEdit to the layout
	layout->addWidget(textEdit);

	// Set the layout to the dialog
	infoDialog->setLayout(layout);

	// Show the dialog
	infoDialog->exec(); // Use exec() for a modal dialog or show() for a non-modal dialog
}

///////////////////////////////////////////////////////////////////////////
// PhaseSpaceWidget Implementation
///////////////////////////////////////////////////////////////////////////

PhaseSpaceWidget::PhaseSpaceWidget(QWidget* parent)
    : ParentT(parent)
{
}

PhaseSpaceWidget::~PhaseSpaceWidget()
{
}

bool PhaseSpaceWidget::initialize()
{
    if (!ParentT::initialize()) return false;

    QVBoxLayout* subLayout = new QVBoxLayout;
    subLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.005, panel->Window_width * 0.003, panel->Window_height * 0.005);
    subLayout->setSpacing(panel->Window_width * 0.003);
    subLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    {
        QVBoxLayout* vLayout = new QVBoxLayout;
        vLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.005, panel->Window_width * 0.003, panel->Window_height * 0.005);
        vLayout->setSpacing(panel->Window_width * 0.003);
        vLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

        // [Visualization & Direction]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setSpacing(panel->Window_width * 0.005);
            hLayout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

            QLabel* labelTitle = new QLabel;
            labelTitle->setFixedSize(panel->Window_width * 0.104, panel->Window_height * 0.028);
            labelTitle->setStyleSheet(DialogStyle::DATA_LABEL);
            labelTitle->setFont(panel->font_D_LBL1);
            labelTitle->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            labelTitle->setText(QString::fromLocal8Bit("Visualization"));

            SourceVisualziation_sourePS_QButton = new QPushButton(QString::fromLocal8Bit("On"));
            panel->Buttons_FunctionPanelRight.append(SourceVisualziation_sourePS_QButton);
            SourceVisualziation_sourePS_QButton->setFixedSize(panel->Window_width * 0.05, panel->Window_height * 0.028);
            SourceVisualziation_sourePS_QButton->setCheckable(true);
            SourceVisualziation_sourePS_QButton->setChecked(true);
            SourceVisualziation_sourePS_QButton->setStyleSheet(DialogStyle::MENU_BTN3);
            SourceVisualziation_sourePS_QButton->setFont(panel->font_D_BTN3);
            
            QLabel* labelDir = new QLabel;
            labelDir->setFixedSize(panel->Window_width * 0.03, panel->Window_height * 0.028);
            labelDir->setStyleSheet(DialogStyle::DATA_LABEL);
            labelDir->setFont(panel->font_D_LBL4);
            labelDir->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            labelDir->setText(QString::fromLocal8Bit("Direction"));

            DirectionCheck_sourcePS_QCheckBox = new QCheckBox;
            DirectionCheck_sourcePS_QCheckBox->setChecked(true);
            DirectionCheck_sourcePS_QCheckBox->setFixedSize(panel->Window_width * 0.02, panel->Window_height * 0.02);
            DirectionCheck_sourcePS_QCheckBox->setStyleSheet("QCheckBox { background-color: transparent; }");

            // 기능은 this, 전체 갱신은 panel
            connect(SourceVisualziation_sourePS_QButton, SIGNAL(clicked()), this, SLOT(slot_PhaseSpaceSourceVisualization_ButtonClicked())); 
            connect(SourceVisualziation_sourePS_QButton, SIGNAL(clicked()), panel, SLOT(slot_allButtonClicked()));
            
            connect(DirectionCheck_sourcePS_QCheckBox, SIGNAL(clicked()), this, SLOT(slot_DirectionCheck_sourcePS_QCheckBox_ButtonClicked()));

            hLayout->addWidget(labelTitle);
            hLayout->addWidget(SourceVisualziation_sourePS_QButton);
            hLayout->addStretch();
            hLayout->addWidget(labelDir);
            hLayout->addWidget(DirectionCheck_sourcePS_QCheckBox);

            vLayout->addLayout(hLayout);
        }

        // [Format Selection Buttons]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setSpacing(panel->Window_width * 0.005);
            hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            hLayout->setContentsMargins(panel->Window_width * 0.005, 0, panel->Window_height * 0.005, 0);

            // MCNP6
            sourcePS_MCNP_button = new QPushButton(QString::fromLocal8Bit("MCNP6"));
            panel->Buttons_FunctionPanelRight.append(sourcePS_MCNP_button);
            sourcePS_MCNP_button->setFixedSize(panel->Window_width * 0.03, panel->Window_height * 0.037);
            sourcePS_MCNP_button->setCheckable(true);
            sourcePS_MCNP_button->setStyleSheet(DialogStyle::MENU_BTN3);
            sourcePS_MCNP_button->setFont(panel->font_D_BTN16);
            sourcePS_MCNP_button->setChecked(false);

            // PHITS
            sourcePS_PHITS_button = new QPushButton(QString::fromLocal8Bit("PHITS"));
            panel->Buttons_FunctionPanelRight.append(sourcePS_PHITS_button);
            sourcePS_PHITS_button->setFixedSize(panel->Window_width * 0.03, panel->Window_height * 0.037);
            sourcePS_PHITS_button->setCheckable(true);
            sourcePS_PHITS_button->setStyleSheet(DialogStyle::MENU_BTN3);
            sourcePS_PHITS_button->setFont(panel->font_D_BTN16);
            sourcePS_PHITS_button->setChecked(false);

            // FLUKA
            sourcePS_FLUKA_button = new QPushButton(QString::fromLocal8Bit("FLUKA"));
            panel->Buttons_FunctionPanelRight.append(sourcePS_FLUKA_button);
            sourcePS_FLUKA_button->setFixedSize(panel->Window_width * 0.03, panel->Window_height * 0.037);
            sourcePS_FLUKA_button->setCheckable(true);
            sourcePS_FLUKA_button->setStyleSheet(DialogStyle::MENU_BTN3);
            sourcePS_FLUKA_button->setFont(panel->font_D_BTN16);
            sourcePS_FLUKA_button->setChecked(false);

            // IAEA
            sourcePS_IAEA_button = new QPushButton(QString::fromLocal8Bit("IAEA"));
            panel->Buttons_FunctionPanelRight.append(sourcePS_IAEA_button);
            sourcePS_IAEA_button->setFixedSize(panel->Window_width * 0.03, panel->Window_height * 0.037);
            sourcePS_IAEA_button->setCheckable(true);
            sourcePS_IAEA_button->setStyleSheet(DialogStyle::MENU_BTN3);
            sourcePS_IAEA_button->setFont(panel->font_D_BTN16);
            sourcePS_IAEA_button->setChecked(false);

            // USER
            sourcePS_USER_Button = new QPushButton(QString::fromLocal8Bit("USER"));
            panel->Buttons_FunctionPanelRight.append(sourcePS_USER_Button);
            sourcePS_USER_Button->setFixedSize(panel->Window_width * 0.03, panel->Window_height * 0.037);
            sourcePS_USER_Button->setCheckable(true);
            sourcePS_USER_Button->setStyleSheet(DialogStyle::MENU_BTN3);
            sourcePS_USER_Button->setFont(panel->font_D_BTN16);
            sourcePS_USER_Button->setChecked(true);

            // Connections
            connect(sourcePS_MCNP_button, SIGNAL(clicked()), this, SLOT(slot_SourcePS_MCNP_ButtonClicked())); 
            connect(sourcePS_MCNP_button, SIGNAL(clicked()), panel, SLOT(slot_allButtonClicked()));
            
            connect(sourcePS_PHITS_button, SIGNAL(clicked()), this, SLOT(slot_SourcePS_PHITS_ButtonClicked())); 
            connect(sourcePS_PHITS_button, SIGNAL(clicked()), panel, SLOT(slot_allButtonClicked()));
            
            connect(sourcePS_FLUKA_button, SIGNAL(clicked()), this, SLOT(slot_SourcePS_FLUKA_ButtonClicked())); 
            connect(sourcePS_FLUKA_button, SIGNAL(clicked()), panel, SLOT(slot_allButtonClicked()));
            
            connect(sourcePS_IAEA_button, SIGNAL(clicked()), this, SLOT(slot_SourcePS_IAEA_ButtonClicked())); 
            connect(sourcePS_IAEA_button, SIGNAL(clicked()), panel, SLOT(slot_allButtonClicked()));
            
            connect(sourcePS_USER_Button, SIGNAL(clicked()), this, SLOT(slot_SourcePS_USER_ButtonClicked())); 
            connect(sourcePS_USER_Button, SIGNAL(clicked()), panel, SLOT(slot_allButtonClicked()));

            hLayout->addWidget(sourcePS_MCNP_button);
            hLayout->addWidget(sourcePS_PHITS_button);
            hLayout->addWidget(sourcePS_FLUKA_button);
            hLayout->addWidget(sourcePS_IAEA_button);
            hLayout->addWidget(sourcePS_USER_Button);

            vLayout->addLayout(hLayout);
        }
        
        // [File Load Button]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setContentsMargins(panel->Window_width * 0.003, 0, panel->Window_width * 0.003, 0);
            hLayout->setSpacing(panel->Window_width * 0.003);
            hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

            QLabel* label0 = new QLabel;
            label0->setFixedSize(panel->Window_width * 0.068, panel->Window_height * 0.028);
            label0->setStyleSheet(DialogStyle::DATA_LABEL);
            label0->setFont(panel->font_D_LBL1);
            label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            label0->setText(QString::fromLocal8Bit("Phase space file"));

            sourcePS_loadPSFButton = new QPushButton(QString::fromLocal8Bit("Load file"));
            panel->Buttons_FunctionPanelRight.append(sourcePS_loadPSFButton);
            sourcePS_loadPSFButton->setFixedSize(panel->Window_width * 0.104, panel->Window_height * 0.028);
            sourcePS_loadPSFButton->setStyleSheet(DialogStyle::MENU_BTN11);
            sourcePS_loadPSFButton->setFont(panel->font_D_BTN11);

            connect(sourcePS_loadPSFButton, SIGNAL(clicked()), this, SLOT(slot_sourcePS_loadPSF_ButtonClicked())); 
            connect(sourcePS_loadPSFButton, SIGNAL(clicked()), panel, SLOT(slot_allButtonClicked()));

            hLayout->addWidget(label0);
            hLayout->addStretch();
            hLayout->addWidget(sourcePS_loadPSFButton);

            vLayout->addLayout(hLayout);
        }

        // [File Name Display]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setContentsMargins(panel->Window_width * 0.003, 0, panel->Window_width * 0.003, 0);
            hLayout->setSpacing(panel->Window_width * 0.005);
            hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

            QLabel* label0 = new QLabel;
            label0->setFixedSize(panel->Window_width * 0.068, panel->Window_height * 0.028);
            label0->setStyleSheet(DialogStyle::DATA_LABEL);
            label0->setFont(panel->font_D_LBL1);
            label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            label0->setText(QString::fromLocal8Bit("File name"));

            sourcePS_PSFname = new QTextEdit;
            sourcePS_PSFname->setFixedSize(panel->Window_width * 0.104, panel->Window_height * 0.056);
            sourcePS_PSFname->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            sourcePS_PSFname->setStyleSheet("QTextEdit { background-color: rgb(255, 255, 255) }");
            sourcePS_PSFname->setFont(panel->font_D_MAT);
            sourcePS_PSFname->setPlaceholderText(QString::fromLocal8Bit("empty"));

            hLayout->addWidget(label0);
            hLayout->addStretch();
            hLayout->addWidget(sourcePS_PSFname);

            vLayout->addLayout(hLayout);
        }		
        subLayout->addLayout(vLayout);
    }

    this->setLayout(subLayout);
    return true;
}

// === Slot Functions ===
void PhaseSpaceWidget::slot_PhaseSpaceSourceVisualization_ButtonClicked()
{
	if (SourceVisualziation_sourePS_QButton->isChecked())
	{
		SourceVisualziation_sourePS_QButton->setText("On");
		for (auto itr : theApp.SourcePanelActor_Position) theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(itr);
		if (DirectionCheck_sourcePS_QCheckBox->isChecked())
		{
			for (auto itr : theApp.SourcePanelActor_Direction) theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(itr);
		}
		else
		{
			for (auto itr : theApp.SourcePanelActor_Direction) theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(itr);
		}
	}
	else
	{
		SourceVisualziation_sourePS_QButton->setText("Off");
		for (auto itr : theApp.SourcePanelActor_Position) theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(itr);
		for (auto itr : theApp.SourcePanelActor_Direction) theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(itr);
	}
	theApp.m_pVTKWidget->renderWindow()->Render();
}
void PhaseSpaceWidget::slot_DirectionCheck_sourcePS_QCheckBox_ButtonClicked()
{
	if (SourceVisualziation_sourePS_QButton->isChecked())
	{
		if (DirectionCheck_sourcePS_QCheckBox->isChecked())
		{
			for (auto itr : theApp.SourcePanelActor_Direction) theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(itr);
		}
		else
		{
			for (auto itr : theApp.SourcePanelActor_Direction) theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(itr);
		}
	}
	theApp.m_pVTKWidget->renderWindow()->Render();
}

// Format Selection (Radio behavior using PushButtons)
void PhaseSpaceWidget::slot_SourcePS_MCNP_ButtonClicked()
{
	sourcePS_MCNP_button->setChecked(true);
	sourcePS_PHITS_button->setChecked(false);
	sourcePS_FLUKA_button->setChecked(false);
	sourcePS_USER_Button->setChecked(false);
}
void PhaseSpaceWidget::slot_SourcePS_PHITS_ButtonClicked()
{
	sourcePS_MCNP_button->setChecked(false);
	sourcePS_PHITS_button->setChecked(true);
	sourcePS_FLUKA_button->setChecked(false);
	sourcePS_USER_Button->setChecked(false);
}
void PhaseSpaceWidget::slot_SourcePS_FLUKA_ButtonClicked()
{
	sourcePS_MCNP_button->setChecked(false);
	sourcePS_PHITS_button->setChecked(false);
	sourcePS_FLUKA_button->setChecked(true);
	sourcePS_USER_Button->setChecked(false);
}
void PhaseSpaceWidget::slot_SourcePS_IAEA_ButtonClicked()
{
	sourcePS_MCNP_button->setChecked(false);
	sourcePS_PHITS_button->setChecked(false);
	sourcePS_FLUKA_button->setChecked(false);
	sourcePS_IAEA_button->setChecked(true);
	sourcePS_USER_Button->setChecked(false);
}
void PhaseSpaceWidget::slot_SourcePS_USER_ButtonClicked()
{
	sourcePS_MCNP_button->setChecked(false);
	sourcePS_PHITS_button->setChecked(false);
	sourcePS_FLUKA_button->setChecked(false);
	sourcePS_USER_Button->setChecked(true);
}

// File Operation
void PhaseSpaceWidget::slot_sourcePS_loadPSF_ButtonClicked()
{	
	QString filter;
	if (sourcePS_IAEA_button->isChecked()) {  // 특정 조건
		filter = tr("IAEA Files (*.IAEAphsp)");
	}
	else if (sourcePS_MCNP_button->isChecked()) {  // 특정 조건
		filter = tr("MCNP WSSA Files (*.w)");
	}
	else {
		filter = tr("ASCII Files (*.*)");  // 다른 필터
	}
	QString dir = QFileDialog::getOpenFileName(
		this,
		tr("Select a file to open"),
		"",
		filter);

	bool is_load_sucessfully = false;
	if (!dir.isEmpty())
	{
		if (sourcePS_MCNP_button->isChecked())
		{
			theApp.sourceObjects->PSF_MCNP_ssw_parse_file(dir.toStdString()); // 연산과 관련된 일은 sourceObject가 처리한다.			
			is_load_sucessfully = theApp.sourceObjects->LoadPSF_data("./data/WSSA_converted_ASCII_file"); // Load converted ASCII file
			remove("./data/WSSA_converted_ASCII_file"); // 임시로 만든 ASCII 파일 삭제
		}
		else is_load_sucessfully = theApp.sourceObjects->LoadPSF_data(dir);

		if (is_load_sucessfully)
		{
			int tmp_size = dir.size() - 19;
			QString FinalDir = dir.right(tmp_size);
			sourcePS_PSFname->setPlaceholderText(dir);
			sourcePS_PSFname->setReadOnly(true);
			QFont font;
			font.setFamily("Arial");    // Font family
			font.setPointSize(panel->FontSizeScaling(7));      // Font size
			sourcePS_PSFname->setFont(font);
		}		
	}
}

///////////////////////////////////////////////////////////////////////////
// HotParticleWidget Implementation
///////////////////////////////////////////////////////////////////////////

HotParticleWidget::HotParticleWidget(QWidget* parent)
    : ParentT(parent)
{
}

HotParticleWidget::~HotParticleWidget()
{
}

bool HotParticleWidget::initialize()
{
    if (!ParentT::initialize()) return false;

    QVBoxLayout* subLayout = new QVBoxLayout;
    subLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.005, panel->Window_width * 0.003, panel->Window_height * 0.005);
    subLayout->setSpacing(panel->Window_width * 0.003);
    subLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    {
        QVBoxLayout* vLayout = new QVBoxLayout;
        vLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.005, panel->Window_width * 0.003, panel->Window_height * 0.005);
        vLayout->setSpacing(panel->Window_width * 0.003);
        vLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

        // [Visualization & Sphere Size]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setSpacing(panel->Window_width * 0.005);
            hLayout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

            QLabel* labelTitle = new QLabel;
            labelTitle->setFixedSize(panel->Window_width * 0.078, panel->Window_height * 0.028);
            labelTitle->setStyleSheet(DialogStyle::DATA_LABEL);
            labelTitle->setFont(panel->font_D_LBL1);
            labelTitle->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            labelTitle->setText(QString::fromLocal8Bit("Visualization"));

            QSpacerItem *spacer1 = new QSpacerItem(panel->Window_width * 0.06, panel->Window_height * 0.028);

            SourceHP_SphereSize_Minus_QPushButton = new QPushButton(QString::fromLocal8Bit("-"));
            panel->Buttons_FunctionPanelRight.append(SourceHP_SphereSize_Minus_QPushButton);
            SourceHP_SphereSize_Minus_QPushButton->setFixedSize(panel->Window_width * 0.015, panel->Window_height * 0.028);
            SourceHP_SphereSize_Minus_QPushButton->setStyleSheet(DialogStyle::MENU_BTN3);
            SourceHP_SphereSize_Minus_QPushButton->setFont(panel->font_D_BTN3);

            SourceHP_SphereSize_Plus_QPushButton = new QPushButton(QString::fromLocal8Bit("+"));
            panel->Buttons_FunctionPanelRight.append(SourceHP_SphereSize_Plus_QPushButton);
            SourceHP_SphereSize_Plus_QPushButton->setFixedSize(panel->Window_width * 0.015, panel->Window_height * 0.028);
            SourceHP_SphereSize_Plus_QPushButton->setStyleSheet(DialogStyle::MENU_BTN3);
            SourceHP_SphereSize_Plus_QPushButton->setFont(panel->font_D_BTN3);

            m_HotParticleSourceVisualizationButton = new QPushButton(QString::fromLocal8Bit("On"));
            panel->Buttons_FunctionPanelRight.append(m_HotParticleSourceVisualizationButton);
            m_HotParticleSourceVisualizationButton->setFixedSize(panel->Window_width * 0.05, panel->Window_height * 0.028);
            m_HotParticleSourceVisualizationButton->setCheckable(true);
            m_HotParticleSourceVisualizationButton->setChecked(true);
            m_HotParticleSourceVisualizationButton->setStyleSheet(DialogStyle::MENU_BTN3);
            m_HotParticleSourceVisualizationButton->setFont(panel->font_D_BTN3);
            
            connect(SourceHP_SphereSize_Minus_QPushButton, SIGNAL(clicked()), this, SLOT(slot_SourceHP_SphereSize_Minus_ButtonClicked())); 
            connect(SourceHP_SphereSize_Minus_QPushButton, SIGNAL(clicked()), panel, SLOT(slot_allButtonClicked()));
            
            connect(SourceHP_SphereSize_Plus_QPushButton, SIGNAL(clicked()), this, SLOT(slot_SourceHP_SphereSize_Plus_ButtonClicked())); 
            connect(SourceHP_SphereSize_Plus_QPushButton, SIGNAL(clicked()), panel, SLOT(slot_allButtonClicked()));
            
            connect(m_HotParticleSourceVisualizationButton, SIGNAL(clicked()), this, SLOT(slot_HotParticleSourceVisualization_ButtonClicked())); 
            connect(m_HotParticleSourceVisualizationButton, SIGNAL(clicked()), panel, SLOT(slot_allButtonClicked()));

            hLayout->addWidget(labelTitle);
            hLayout->addSpacerItem(spacer1);
            hLayout->addStretch();
            hLayout->addWidget(SourceHP_SphereSize_Minus_QPushButton);
            hLayout->addWidget(SourceHP_SphereSize_Plus_QPushButton);
            hLayout->addWidget(m_HotParticleSourceVisualizationButton);

            vLayout->addLayout(hLayout);
        }           
    
        // [Position XYZ Label & LineEdits]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setContentsMargins(panel->Window_width * 0.003, 0, panel->Window_width * 0.003, 0);
            hLayout->setSpacing(panel->Window_width * 0.003);
            hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

            QLabel* label0 = new QLabel;
            label0->setFixedSize(panel->Window_width * 0.058, panel->Window_height * 0.028);
            label0->setStyleSheet(DialogStyle::DATA_LABEL);
            label0->setFont(panel->font_D_LBL1);
            label0->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
            label0->setText(QString::fromLocal8Bit("Position XYZ:"));

            PosX_SourceHP_QLineEdit = new QLineEdit;
            PosX_SourceHP_QLineEdit->setFixedSize(panel->Window_width * 0.033, panel->Window_height * 0.028);
            PosX_SourceHP_QLineEdit->setAlignment(Qt::AlignCenter);
            PosX_SourceHP_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME5);
            PosX_SourceHP_QLineEdit->setFont(panel->font_D_LE5);
            PosX_SourceHP_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(cm)"));
            PosX_SourceHP_QLineEdit->setReadOnly(true);

            PosY_SourceHP_QLineEdit = new QLineEdit;
            PosY_SourceHP_QLineEdit->setFixedSize(panel->Window_width * 0.033, panel->Window_height * 0.028);
            PosY_SourceHP_QLineEdit->setAlignment(Qt::AlignCenter);
            PosY_SourceHP_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME5);
            PosY_SourceHP_QLineEdit->setFont(panel->font_D_LE5);
            PosY_SourceHP_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(cm)"));
            PosY_SourceHP_QLineEdit->setReadOnly(true);

            PosZ_SourceHP_QLineEdit = new QLineEdit;
            PosZ_SourceHP_QLineEdit->setFixedSize(panel->Window_width * 0.033, panel->Window_height * 0.028);
            PosZ_SourceHP_QLineEdit->setAlignment(Qt::AlignCenter);
            PosZ_SourceHP_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME5);
            PosZ_SourceHP_QLineEdit->setFont(panel->font_D_LE5);
            PosZ_SourceHP_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(cm)"));
            PosZ_SourceHP_QLineEdit->setReadOnly(true);

            hLayout->addWidget(label0);
            hLayout->addStretch();
            hLayout->addWidget(PosX_SourceHP_QLineEdit);
            hLayout->addWidget(PosY_SourceHP_QLineEdit);
            hLayout->addWidget(PosZ_SourceHP_QLineEdit);

            vLayout->addLayout(hLayout);
        }
        
        // [Separation Distance]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setContentsMargins(panel->Window_width * 0.003, 0, panel->Window_width * 0.003, 0);
            hLayout->setSpacing(panel->Window_width * 0.003);
            hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

            QLabel* label0 = new QLabel;
            label0->setFixedSize(panel->Window_width * 0.065, panel->Window_height * 0.028);
            label0->setStyleSheet(DialogStyle::DATA_LABEL);
            label0->setFont(panel->font_D_LBL1);
            label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            label0->setText(QString::fromLocal8Bit("Separation Dist.:"));
            
            SeparationDist_SourceHP_QLineEdit = new QLineEdit;
            SeparationDist_SourceHP_QLineEdit->setFixedSize(panel->Window_width * 0.104, panel->Window_height * 0.028);
            SeparationDist_SourceHP_QLineEdit->setAlignment(Qt::AlignCenter);
            SeparationDist_SourceHP_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
            SeparationDist_SourceHP_QLineEdit->setFont(panel->font_D_LE1);
            SeparationDist_SourceHP_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(cm)"));     
            
            hLayout->addWidget(label0);
            hLayout->addStretch();
            hLayout->addWidget(SeparationDist_SourceHP_QLineEdit);

            vLayout->addLayout(hLayout);
        }   
        
        // [3D Pick & Update Buttons]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setContentsMargins(panel->Window_width * 0.003, 0, panel->Window_width * 0.003, 0);
            hLayout->setSpacing(panel->Window_width * 0.003);
            hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

            Pick3D_HP_QButton = new QPushButton(QString::fromLocal8Bit("3D Pick"));
            panel->Buttons_FunctionPanelRight.append(Pick3D_HP_QButton);
            Pick3D_HP_QButton->setFixedSize(panel->Window_width * 0.050, panel->Window_height * 0.036);
            Pick3D_HP_QButton->setCheckable(true);
            Pick3D_HP_QButton->setStyleSheet(DialogStyle::MENU_BTN2);
            Pick3D_HP_QButton->setFont(panel->font_D_BTN2);

            UpdatePosition_sourceHP_QButton = new QPushButton(QString::fromLocal8Bit("Update"));
            panel->Buttons_FunctionPanelRight.append(UpdatePosition_sourceHP_QButton);
            UpdatePosition_sourceHP_QButton->setFixedSize(panel->Window_width * 0.050, panel->Window_height * 0.036);
            UpdatePosition_sourceHP_QButton->setStyleSheet(DialogStyle::MENU_BTN2);
            UpdatePosition_sourceHP_QButton->setFont(panel->font_D_BTN2);

            connect(Pick3D_HP_QButton, SIGNAL(clicked()), this, SLOT(slot_HotParticle_Pick3D_ButtonClicked())); 
            connect(Pick3D_HP_QButton, SIGNAL(clicked()), panel, SLOT(slot_allButtonClicked()));
            
            connect(UpdatePosition_sourceHP_QButton, SIGNAL(clicked()), this, SLOT(slot_Update_Position_HP_ButtonClicked())); 
            connect(UpdatePosition_sourceHP_QButton, SIGNAL(clicked()), panel, SLOT(slot_allButtonClicked()));

            hLayout->addStretch();
            hLayout->addWidget(Pick3D_HP_QButton);
            hLayout->addWidget(UpdatePosition_sourceHP_QButton);

            vLayout->addLayout(hLayout);
        }

        // [Radio Buttons: RI vs ES]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setSpacing(panel->Window_width * 0.010);
            hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            hLayout->setContentsMargins(panel->Window_width * 0.005, panel->Window_height * 0.009, 0, 0);

            RI_Select_sourceHP_QRadioButton = new QRadioButton(QString::fromLocal8Bit("Radionuclide"));
            RI_Select_sourceHP_QRadioButton->setStyleSheet(DialogStyle::RADIOBUTTON_WIDGET);
            RI_Select_sourceHP_QRadioButton->setFont(panel->font_D_RB2);
            RI_Select_sourceHP_QRadioButton->setFixedSize(panel->Window_width * 0.081, panel->Window_height * 0.028);
            RI_Select_sourceHP_QRadioButton->setChecked(true);

            ES_Select_sourceHP_QRadioButton = new QRadioButton(QString::fromLocal8Bit("Energy spectrum"));
            ES_Select_sourceHP_QRadioButton->setStyleSheet(DialogStyle::RADIOBUTTON_WIDGET);
            ES_Select_sourceHP_QRadioButton->setFont(panel->font_D_RB2);
            ES_Select_sourceHP_QRadioButton->setFixedSize(panel->Window_width * 0.081, panel->Window_height * 0.028);
            ES_Select_sourceHP_QRadioButton->setChecked(false);

            connect(RI_Select_sourceHP_QRadioButton, SIGNAL(clicked()), this, SLOT(slot_RI_Select_sourceHP_RadioButtonClicked())); 
            connect(RI_Select_sourceHP_QRadioButton, SIGNAL(clicked()), panel, SLOT(slot_allButtonClicked()));
            
            connect(ES_Select_sourceHP_QRadioButton, SIGNAL(clicked()), this, SLOT(slot_ES_Select_sourceHP_RadioButtonClicked())); 
            connect(ES_Select_sourceHP_QRadioButton, SIGNAL(clicked()), panel, SLOT(slot_allButtonClicked()));

            hLayout->addWidget(RI_Select_sourceHP_QRadioButton);
            hLayout->addWidget(ES_Select_sourceHP_QRadioButton);

            vLayout->addLayout(hLayout);
        }

        // [Radionuclide UI]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setContentsMargins(panel->Window_width * 0.003, 0, panel->Window_width * 0.003, 0);
            hLayout->setSpacing(panel->Window_width * 0.003);
            hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

            RI_Title_sourceHP_QLabel = new QLabel;
            RI_Title_sourceHP_QLabel->setFixedSize(panel->Window_width * 0.068, panel->Window_height * 0.028);
            RI_Title_sourceHP_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
            RI_Title_sourceHP_QLabel->setFont(panel->font_D_LBL1);
            RI_Title_sourceHP_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            RI_Title_sourceHP_QLabel->setText(QString::fromLocal8Bit("Radionuclide"));

            RI_Radionuclide_sourceHP_QLineEdit = new QLineEdit;
            RI_Radionuclide_sourceHP_QLineEdit->setFixedSize(panel->Window_width * 0.104, panel->Window_height * 0.028);
            RI_Radionuclide_sourceHP_QLineEdit->setAlignment(Qt::AlignCenter);
            RI_Radionuclide_sourceHP_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
            RI_Radionuclide_sourceHP_QLineEdit->setFont(panel->font_D_LE1);
            RI_Radionuclide_sourceHP_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("Enter radionuclide"));

            QCompleter *completer = new QCompleter(panel->RIsourceList, this);
            completer->setCaseSensitivity(Qt::CaseInsensitive);
            RI_Radionuclide_sourceHP_QLineEdit->setCompleter(completer);

            hLayout->addWidget(RI_Title_sourceHP_QLabel);
            hLayout->addStretch();
            hLayout->addWidget(RI_Radionuclide_sourceHP_QLineEdit);

            vLayout->addLayout(hLayout);
        }

        // [Activity UI]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setContentsMargins(panel->Window_width * 0.003, 0, panel->Window_width * 0.003, 0);
            hLayout->setSpacing(panel->Window_width * 0.003);
            hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

            RI_Activity_sourceHP_QLabel = new QLabel;
            RI_Activity_sourceHP_QLabel->setFixedSize(panel->Window_width * 0.068, panel->Window_height * 0.028);
            RI_Activity_sourceHP_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
            RI_Activity_sourceHP_QLabel->setFont(panel->font_D_LBL1);
            RI_Activity_sourceHP_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            RI_Activity_sourceHP_QLabel->setText(QString::fromLocal8Bit("Activity"));

            RI_Activity_sourceHP_QLineEdit = new QLineEdit;
            RI_Activity_sourceHP_QLineEdit->setFixedSize(panel->Window_width * 0.104, panel->Window_height * 0.028);
            RI_Activity_sourceHP_QLineEdit->setAlignment(Qt::AlignCenter);
            RI_Activity_sourceHP_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
            RI_Activity_sourceHP_QLineEdit->setFont(panel->font_D_LE1);
            RI_Activity_sourceHP_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("Bq"));

            hLayout->addWidget(RI_Activity_sourceHP_QLabel);
            hLayout->addStretch();
            hLayout->addWidget(RI_Activity_sourceHP_QLineEdit);

            vLayout->addLayout(hLayout);
        }

        // [Radionuclide Add Button]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setContentsMargins(panel->Window_width * 0.003, 0, panel->Window_width * 0.003, 0);
            hLayout->setSpacing(panel->Window_width * 0.003);
            hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

            RI_List_sourceHP_QLabel = new QLabel;
            RI_List_sourceHP_QLabel->setFixedSize(panel->Window_width * 0.088, panel->Window_height * 0.028);
            RI_List_sourceHP_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
            RI_List_sourceHP_QLabel->setFont(panel->font_D_LBL1);
            RI_List_sourceHP_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
            RI_List_sourceHP_QLabel->setText(QString::fromLocal8Bit("Entered sources"));

            RIList_Add_sourceHP_QButton = new QPushButton(QString::fromLocal8Bit("Add"));
            panel->Buttons_FunctionPanelRight.append(RIList_Add_sourceHP_QButton);
            RIList_Add_sourceHP_QButton->setFixedSize(panel->Window_width * 0.042, panel->Window_height * 0.037);
            RIList_Add_sourceHP_QButton->setCheckable(false);
            RIList_Add_sourceHP_QButton->setStyleSheet(DialogStyle::MENU_BTN2);
            RIList_Add_sourceHP_QButton->setFont(panel->font_D_BTN2);
            RIList_Add_sourceHP_QButton->setChecked(false);

            connect(RIList_Add_sourceHP_QButton, SIGNAL(clicked()), this, SLOT(slot_RIList_Add_sourceHP_ButtonClicked())); 
            connect(RIList_Add_sourceHP_QButton, SIGNAL(clicked()), panel, SLOT(slot_allButtonClicked()));

            hLayout->addWidget(RI_List_sourceHP_QLabel);
            hLayout->addStretch();
            hLayout->addWidget(RIList_Add_sourceHP_QButton);

            vLayout->addLayout(hLayout);
        }

        // [Radionuclide List]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setContentsMargins(panel->Window_width * 0.003, 0, panel->Window_width * 0.003, 0);
            hLayout->setSpacing(panel->Window_width * 0.003);
            hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

            RI_List_sourceHP_QListWidget = new QListWidget;
            RI_List_sourceHP_QListWidget->setStyleSheet("QListWidget { border-radius: 15px; background-color: white; }");
            RI_List_sourceHP_QListWidget->setFixedSize(panel->Window_width * 0.175, panel->Window_height * 0.155);
            RI_List_sourceHP_QListWidget->setFont(panel->font_D_LW1);

            hLayout->addWidget(RI_List_sourceHP_QListWidget);

            vLayout->addLayout(hLayout);
        }

        // [Energy Spectrum - Load File]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.010, panel->Window_width * 0.003, 0);
            hLayout->setSpacing(panel->Window_width * 0.003);
            hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

            ES_Title_sourceHP_QLabel = new QLabel;
            ES_Title_sourceHP_QLabel->setFixedSize(panel->Window_width * 0.095, panel->Window_height * 0.028);
            ES_Title_sourceHP_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
            ES_Title_sourceHP_QLabel->setFont(panel->font_D_LBL1);
            ES_Title_sourceHP_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            ES_Title_sourceHP_QLabel->setText(QString::fromLocal8Bit("Energy spectrum file"));
            ES_Title_sourceHP_QLabel->hide();

            sourceHP_EnergyspectrumFileLoadButton = new QPushButton(QString::fromLocal8Bit("Load file"));
            panel->Buttons_FunctionPanelRight.append(sourceHP_EnergyspectrumFileLoadButton);
            sourceHP_EnergyspectrumFileLoadButton->setFixedSize(panel->Window_width * 0.067, panel->Window_height * 0.028);
            sourceHP_EnergyspectrumFileLoadButton->setStyleSheet(DialogStyle::MENU_BTN11);
            sourceHP_EnergyspectrumFileLoadButton->setFont(panel->font_D_BTN11);
            sourceHP_EnergyspectrumFileLoadButton->hide();

            connect(sourceHP_EnergyspectrumFileLoadButton, SIGNAL(clicked()), this, SLOT(slot_EnergyspectrumFileLoad_sourceHP_ButtonClicked())); 
            connect(sourceHP_EnergyspectrumFileLoadButton, SIGNAL(clicked()), panel, SLOT(slot_allButtonClicked()));

            hLayout->addWidget(ES_Title_sourceHP_QLabel);
            hLayout->addWidget(sourceHP_EnergyspectrumFileLoadButton);

            vLayout->addLayout(hLayout);
        }

        // [Energy Spectrum - File Name]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setContentsMargins(panel->Window_width * 0.003, 0, panel->Window_width * 0.003, 0);
            hLayout->setSpacing(panel->Window_width * 0.005);
            hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

            ES_FileName_sourceHP_QLabel = new QLabel;
            ES_FileName_sourceHP_QLabel->setFixedSize(panel->Window_width * 0.068, panel->Window_height * 0.028);
            ES_FileName_sourceHP_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
            ES_FileName_sourceHP_QLabel->setFont(panel->font_D_LBL1);
            ES_FileName_sourceHP_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            ES_FileName_sourceHP_QLabel->setText(QString::fromLocal8Bit("File name"));
            ES_FileName_sourceHP_QLabel->hide();

            sourceHP_EnergyspectrumFileNameText = new QTextEdit;
            sourceHP_EnergyspectrumFileNameText->setFixedSize(panel->Window_width * 0.104, panel->Window_height * 0.056);
            sourceHP_EnergyspectrumFileNameText->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            sourceHP_EnergyspectrumFileNameText->setStyleSheet("QTextEdit { background-color: rgb(255, 255, 255) }");
            sourceHP_EnergyspectrumFileNameText->setFont(panel->font_D_MAT);
            sourceHP_EnergyspectrumFileNameText->setPlaceholderText(QString::fromLocal8Bit("empty"));
            sourceHP_EnergyspectrumFileNameText->hide();

            hLayout->addWidget(ES_FileName_sourceHP_QLabel);
            hLayout->addStretch();
            hLayout->addWidget(sourceHP_EnergyspectrumFileNameText);

            vLayout->addLayout(hLayout);
        }

        // [Intensity UI]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setContentsMargins(panel->Window_width * 0.003, 0, panel->Window_width * 0.003, 0);
            hLayout->setSpacing(panel->Window_width * 0.003);
            hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

            ES_Intensity_sourceHP_QLabel = new QLabel;
            ES_Intensity_sourceHP_QLabel->setFixedSize(panel->Window_width * 0.068, panel->Window_height * 0.028);
            ES_Intensity_sourceHP_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
            ES_Intensity_sourceHP_QLabel->setFont(panel->font_D_LBL1);
            ES_Intensity_sourceHP_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            ES_Intensity_sourceHP_QLabel->setText(QString::fromLocal8Bit("Intensity"));
            ES_Intensity_sourceHP_QLabel->hide();

            ES_Intensity_sourceHP_QLineEdit = new QLineEdit;
            ES_Intensity_sourceHP_QLineEdit->setFixedSize(panel->Window_width * 0.104, panel->Window_height * 0.028);
            ES_Intensity_sourceHP_QLineEdit->setAlignment(Qt::AlignCenter);
            ES_Intensity_sourceHP_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
            ES_Intensity_sourceHP_QLineEdit->setFont(panel->font_D_LE1);
            ES_Intensity_sourceHP_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("particles/s"));
            ES_Intensity_sourceHP_QLineEdit->hide();

            hLayout->addWidget(ES_Intensity_sourceHP_QLabel);
            hLayout->addStretch();
            hLayout->addWidget(ES_Intensity_sourceHP_QLineEdit);

            vLayout->addLayout(hLayout);
        }

        // [Energy Spectrum Add Button]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setContentsMargins(panel->Window_width * 0.003, 0, panel->Window_width * 0.003, 0);
            hLayout->setSpacing(panel->Window_width * 0.003);
            hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

            ES_List_sourceHP_QLabel = new QLabel;
            ES_List_sourceHP_QLabel->setFixedSize(panel->Window_width * 0.088, panel->Window_height * 0.028);
            ES_List_sourceHP_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
            ES_List_sourceHP_QLabel->setFont(panel->font_D_LBL1);
            ES_List_sourceHP_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
            ES_List_sourceHP_QLabel->setText(QString::fromLocal8Bit("Entered sources"));
            ES_List_sourceHP_QLabel->hide();

            ESList_Add_sourceHP_QButton = new QPushButton(QString::fromLocal8Bit("Add"));
            panel->Buttons_FunctionPanelRight.append(ESList_Add_sourceHP_QButton);
            ESList_Add_sourceHP_QButton->setFixedSize(panel->Window_width * 0.042, panel->Window_height * 0.037);
            ESList_Add_sourceHP_QButton->setCheckable(false);
            ESList_Add_sourceHP_QButton->setStyleSheet(DialogStyle::MENU_BTN2);
            ESList_Add_sourceHP_QButton->setFont(panel->font_D_BTN2);
            ESList_Add_sourceHP_QButton->setChecked(false);
            ESList_Add_sourceHP_QButton->hide();

            connect(ESList_Add_sourceHP_QButton, SIGNAL(clicked()), this, SLOT(slot_ESList_Add_sourceHP_ButtonClicked())); 
            connect(ESList_Add_sourceHP_QButton, SIGNAL(clicked()), panel, SLOT(slot_allButtonClicked()));

            hLayout->addWidget(ES_List_sourceHP_QLabel);
            hLayout->addStretch();
            hLayout->addWidget(ESList_Add_sourceHP_QButton);

            vLayout->addLayout(hLayout);
        }

        // [Energy Spectrum List]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setContentsMargins(panel->Window_width * 0.003, 0, panel->Window_width * 0.003, 0);
            hLayout->setSpacing(panel->Window_width * 0.003);
            hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

            ES_List_sourceHP_QListWidget = new QListWidget;
            ES_List_sourceHP_QListWidget->setStyleSheet("QListWidget { border-radius: 15px; background-color: white; }");
            ES_List_sourceHP_QListWidget->setFixedSize(panel->Window_width * 0.175, panel->Window_height * 0.155);
            ES_List_sourceHP_QListWidget->setFont(panel->font_D_LW1);
            ES_List_sourceHP_QListWidget->hide();

            hLayout->addWidget(ES_List_sourceHP_QListWidget);

            vLayout->addLayout(hLayout);
        }

        subLayout->addLayout(vLayout);
    }

    this->setLayout(subLayout);
    return true;
}

void HotParticleWidget::slot_SourceHP_SphereSize_Minus_ButtonClicked()
{
	theApp.m_Point_Source_Scale--;
	for (auto itr_source_actor : theApp.SourcePanelActor_Position)
	{
		itr_source_actor->SetScale(pow(theApp.const_PointSource_ScaleFactorPerClick, theApp.m_Point_Source_Scale));
	}
	theApp.m_pVTKWidget->renderWindow()->Render();
}
void HotParticleWidget::slot_SourceHP_SphereSize_Plus_ButtonClicked()
{
	theApp.m_Point_Source_Scale++;
	for (auto itr_source_actor : theApp.SourcePanelActor_Position)
	{
		itr_source_actor->SetScale(pow(theApp.const_PointSource_ScaleFactorPerClick, theApp.m_Point_Source_Scale));
	}
	theApp.m_pVTKWidget->renderWindow()->Render();
}
void HotParticleWidget::slot_HotParticleSourceVisualization_ButtonClicked()
{
	if (m_HotParticleSourceVisualizationButton->isChecked())
	{
		m_HotParticleSourceVisualizationButton->setText("On");
		for (auto itr : theApp.SourcePanelActor_Position) theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(itr);
	}
	else
	{
		m_HotParticleSourceVisualizationButton->setText("Off");
		for (auto itr : theApp.SourcePanelActor_Position) theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(itr);
	}
	theApp.m_pVTKWidget->renderWindow()->Render();
}
void HotParticleWidget::slot_HotParticle_Pick3D_ButtonClicked()
{
	// Pick3D 끄거나 켤때 정보 초기화
	theApp.sourceObjects->Selecting3DShpere_Delete();
	PosX_SourceHP_QLineEdit->clear();
	PosY_SourceHP_QLineEdit->clear();
	PosZ_SourceHP_QLineEdit->clear();

	QPushButton* clickedButton = qobject_cast<QPushButton*>(sender());

	if (Pick3D_HP_QButton->isChecked() == true) // Pick 3D 켤때
	{
		theApp.st_Pick3D_sourceHP = true;

		for (QList<QPushButton*>::iterator it = panel->Buttons_FunctionPanelRight.begin(); it != panel->Buttons_FunctionPanelRight.end(); ++it)
		{
			if (*it != clickedButton && *it != UpdatePosition_sourceHP_QButton)
			{
				if ((*it)->isEnabled() == true) panel->TempEnabledButtons.append((*it));
				if ((*it)->isEnabled() == false) panel->TempDisabledButtons.append((*it));
				(*it)->setDisabled(true);
			}
		}
	}
	else // Pick 3D 끌때
	{
		theApp.st_Pick3D_sourceHP = false;

		for (QList<QPushButton*>::iterator it = panel->TempEnabledButtons.begin(); it != panel->TempEnabledButtons.end(); ++it)
		{
			(*it)->setEnabled(true);
		}
		panel->TempEnabledButtons.clear();

		for (QList<QPushButton*>::iterator it = panel->TempDisabledButtons.begin(); it != panel->TempDisabledButtons.end(); ++it)
		{
			(*it)->setDisabled(true);
		}
		panel->TempDisabledButtons.clear();
	}
}
void HotParticleWidget::slot_Update_Position_HP_ButtonClicked()
{
	double center[3] = { 0.0, };
	center[0] = PosX_SourceHP_QLineEdit->text().toFloat() + SelectedPointNormalVector_sourceHP[0] * SeparationDist_SourceHP_QLineEdit->text().toFloat();
	center[1] = PosY_SourceHP_QLineEdit->text().toFloat() + SelectedPointNormalVector_sourceHP[1] * SeparationDist_SourceHP_QLineEdit->text().toFloat();
	center[2] = PosZ_SourceHP_QLineEdit->text().toFloat() + SelectedPointNormalVector_sourceHP[2] * SeparationDist_SourceHP_QLineEdit->text().toFloat();

	theApp.sourceObjects->Selecting3DShpere(center); // 임시 가리키는 임시 sphere 생성
	
	PosX_SourceHP_QLineEdit->setText(theApp.getQStringNumberInSpecificDigit(center[0], 6));
	PosY_SourceHP_QLineEdit->setText(theApp.getQStringNumberInSpecificDigit(center[1], 6));
	PosZ_SourceHP_QLineEdit->setText(theApp.getQStringNumberInSpecificDigit(center[2], 6));

	if (theApp.st_Pick3D_sourceHP == true)
	{
		// Pick 완료로 버튼들 원복하기
		Pick3D_HP_QButton->setChecked(false);
		theApp.st_Pick3D_sourceHP = false;

		for (QList<QPushButton*>::iterator it = panel->TempEnabledButtons.begin(); it != panel->TempEnabledButtons.end(); ++it)
		{
			(*it)->setEnabled(true);
		}
		panel->TempEnabledButtons.clear();

		for (QList<QPushButton*>::iterator it = panel->TempDisabledButtons.begin(); it != panel->TempDisabledButtons.end(); ++it)
		{
			(*it)->setDisabled(true);
		}
		panel->TempDisabledButtons.clear();
	}
}

void HotParticleWidget::slot_RI_Select_sourceHP_RadioButtonClicked()
{
	RI_Select_sourceHP_QRadioButton->setChecked(true);
	ES_Select_sourceHP_QRadioButton->setChecked(false);

	RI_Title_sourceHP_QLabel->show();
	RI_Radionuclide_sourceHP_QLineEdit->show();
	RI_Activity_sourceHP_QLabel->show();
	RI_Activity_sourceHP_QLineEdit->show();
	RI_List_sourceHP_QLabel->show();
	RIList_Add_sourceHP_QButton->show();
	RI_List_sourceHP_QListWidget->show();

	ES_Title_sourceHP_QLabel->hide();
	sourceHP_EnergyspectrumFileLoadButton->hide();
	ES_Intensity_sourceHP_QLabel->hide();
	ES_Intensity_sourceHP_QLineEdit->hide();
	ES_FileName_sourceHP_QLabel->hide();
	sourceHP_EnergyspectrumFileNameText->hide();
	ES_List_sourceHP_QLabel->hide();
	ESList_Add_sourceHP_QButton->hide();
	ES_List_sourceHP_QListWidget->hide();

	// === ES 관련 초기화 ===

	// 1. VTK 렌더러에서 모든 관련 Actor 제거
	// a. 먼저 루프를 돌면서 각 Actor를 렌더러에서 제거합니다.
	for (vtkActor* actor : theApp.SourcePanelActor_Position)
	{
		if (actor) // 포인터가 유효한지 확인
		{
			theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(actor);
		}
	}
	// b. VTK 렌더 윈도우를 업데이트하여 변경 사항을 적용합니다.
	theApp.m_pVTKWidget->renderWindow()->Render();

	// 2. QListWidget의 모든 아이템 제거
	// clear() 함수는 위젯의 모든 아이템을 삭제하고 메모리에서 해제합니다.
	ES_List_sourceHP_QListWidget->clear();

	// 3. 관리하던 모든 vector의 내용 비우기
	// 각 vector의 clear() 멤버 함수를 호출하여 모든 요소를 제거합니다.
	ListInfo_sourceHP.clear();
	ESList_Delete_sourceHP_QButton.clear();
	ESList_Info_sourceHP_QButton.clear();
	theApp.SourcePanelActor_Position.clear();

	// 4. 인덱스 카운터 초기화
	// 리스트 아이템 개수를 추적하는 변수를 초기값(보통 0)으로 리셋합니다.
	RIList_sourceHP_MakingIndex = 0;
}
void HotParticleWidget::slot_RIList_Add_sourceHP_ButtonClicked()
{
	// 지원하는 RI 리스트에 있는지 판별
	QString inputText = RI_Radionuclide_sourceHP_QLineEdit->text().trimmed();
	if (inputText.isEmpty()) {
		theApp.SetMessageBox("Radionuclide name is empty!");
		return;
	}

	// 1) 대소문자 구분하여 리스트에 있는지 검사
	if (!panel->RIsourceList.contains(inputText, Qt::CaseSensitive)) {
		// 2) 자동 보정: 첫 글자만 대문자, 나머지는 소문자
		QString corrected = inputText.toLower();
		if (!corrected.isEmpty()) {
			corrected[0] = corrected[0].toUpper();
		}
		// 보정된 문자열이 리스트에 있으면 QLineEdit에 반영
		if (panel->RIsourceList.contains(corrected, Qt::CaseSensitive)) {
			RI_Radionuclide_sourceHP_QLineEdit->setText(corrected);
			inputText = corrected;
		}
		else {
			// 보정해도 없으면 에러
			theApp.SetMessageBox(inputText + ": Radionuclide name error!");
			return;
		}
	}

	QListWidgetItem* item = new QListWidgetItem(RI_List_sourceHP_QListWidget);
	QWidget* widget = new QWidget;
	QHBoxLayout* hLayout = new QHBoxLayout;
	widget->setStyleSheet("background-color: white;");

	QString arg1 = RI_Radionuclide_sourceHP_QLineEdit->text();
	QLabel* label1 = new QLabel(arg1);
	label1->setFixedWidth(panel->Window_width * 0.025);
	label1->setFont(panel->font_D_LW2); // Set font for the label

	QString arg2 = QString::number(RI_Activity_sourceHP_QLineEdit->text().toDouble(), 'e', 3);
	QLabel* label2 = new QLabel(arg2);
	label2->setFixedWidth(panel->Window_width * 0.025);
	label2->setFont(panel->font_D_LW2); // Set font for the label

	QString arg3 = PosX_SourceHP_QLineEdit->text().left(6);
	if (arg3 == "") arg3 = "0";
	QLabel* labelPosX = new QLabel(arg3);
	labelPosX->setFixedWidth(panel->Window_width * 0.025);
	labelPosX->setFont(panel->font_D_LW2); // Set font for the label

	QString arg4 = PosY_SourceHP_QLineEdit->text().left(6);
	if (arg4 == "") arg4 = "0";
	QLabel* labelPosY = new QLabel(arg4);
	labelPosY->setFixedWidth(panel->Window_width * 0.025);
	labelPosY->setFont(panel->font_D_LW2); // Set font for the label

	QString arg5 = PosZ_SourceHP_QLineEdit->text().left(6);
	if (arg5 == "") arg5 = "0";
	QLabel* labelPosZ = new QLabel(arg5);
	labelPosZ->setFixedWidth(panel->Window_width * 0.025);
	labelPosZ->setFont(panel->font_D_LW2); // Set font for the label

	RIList_Delete_sourceHP_QButton.push_back(new QPushButton());
	if (RIList_sourceHP_MakingIndex >= RIList_Delete_sourceHP_QButton.size()) theApp.SetMessageBox("RI_Delete_sourceHP_QButton Vector index error");
	RIList_Delete_sourceHP_QButton[RIList_sourceHP_MakingIndex]->setCheckable(false);
	RIList_Delete_sourceHP_QButton[RIList_sourceHP_MakingIndex]->setStyleSheet(DialogStyle::MENU_BTN13);
	RIList_Delete_sourceHP_QButton[RIList_sourceHP_MakingIndex]->setFont(panel->font_D_BTN13);
	RIList_Delete_sourceHP_QButton[RIList_sourceHP_MakingIndex]->setFixedSize(panel->Window_width * 0.008, panel->Window_height * 0.012);
	RIList_Delete_sourceHP_QButton[RIList_sourceHP_MakingIndex]->setIcon(QIcon("./data/image/delete.png"));
	RIList_Delete_sourceHP_QButton[RIList_sourceHP_MakingIndex]->setObjectName(QString::number(RIList_sourceHP_MakingIndex));

	RIList_Info_sourceHP_QButton.push_back(new QPushButton());
	if (RIList_sourceHP_MakingIndex >= RIList_Delete_sourceHP_QButton.size()) theApp.SetMessageBox("RI_Info_sourceHP_QButton Vector index error");
	RIList_Info_sourceHP_QButton[RIList_sourceHP_MakingIndex]->setCheckable(false);
	RIList_Info_sourceHP_QButton[RIList_sourceHP_MakingIndex]->setStyleSheet(DialogStyle::MENU_BTN13);
	RIList_Info_sourceHP_QButton[RIList_sourceHP_MakingIndex]->setFont(panel->font_D_BTN13);
	RIList_Info_sourceHP_QButton[RIList_sourceHP_MakingIndex]->setFixedSize(panel->Window_width * 0.008, panel->Window_height * 0.012);
	RIList_Info_sourceHP_QButton[RIList_sourceHP_MakingIndex]->setIcon(QIcon("./data/image/info.png"));
	RIList_Info_sourceHP_QButton[RIList_sourceHP_MakingIndex]->setObjectName(QString::number(RIList_sourceHP_MakingIndex));

	hLayout->addWidget(label1);
	hLayout->addWidget(label2);
	hLayout->addWidget(labelPosX);
	hLayout->addWidget(labelPosY);
	hLayout->addWidget(labelPosZ);
	hLayout->addWidget(RIList_Delete_sourceHP_QButton[RIList_sourceHP_MakingIndex]);
	hLayout->addWidget(RIList_Info_sourceHP_QButton[RIList_sourceHP_MakingIndex]);
	widget->setLayout(hLayout);
	RI_List_sourceHP_QListWidget->setItemWidget(item, widget);
	item->setSizeHint(widget->sizeHint());
	// Connect signals of the new buttons
	connect(RIList_Delete_sourceHP_QButton[RIList_sourceHP_MakingIndex], SIGNAL(clicked()), this, SLOT(slot_RIList_Delete_sourceHP_ButtonClicked())); connect(RIList_Delete_sourceHP_QButton[RIList_sourceHP_MakingIndex], SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
	connect(RIList_Info_sourceHP_QButton[RIList_sourceHP_MakingIndex], SIGNAL(clicked()), this, SLOT(slot_RIList_Info_sourceHP_ButtonClicked())); connect(RIList_Info_sourceHP_QButton[RIList_sourceHP_MakingIndex], SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
	// Set Global Variables
	std::map<int, QString> tmp_map;
	tmp_map[0] = "True";
	tmp_map[1] = PosX_SourceHP_QLineEdit->text(); if (tmp_map[1] == "") tmp_map[1] = "0";
	tmp_map[2] = PosY_SourceHP_QLineEdit->text(); if (tmp_map[2] == "") tmp_map[2] = "0";
	tmp_map[3] = PosZ_SourceHP_QLineEdit->text(); if (tmp_map[3] == "") tmp_map[3] = "0";
	tmp_map[4] = RI_Radionuclide_sourceHP_QLineEdit->text();
	tmp_map[5] = RI_Activity_sourceHP_QLineEdit->text(); if (tmp_map[5] == "") tmp_map[5] = "0";
	ListInfo_sourceHP.push_back(tmp_map);
	RIList_sourceHP_MakingIndex++;
	// Generate actor
	double center[3] = { 0.0, };
	center[0] = PosX_SourceHP_QLineEdit->text().toFloat();
	center[1] = PosY_SourceHP_QLineEdit->text().toFloat();
	center[2] = PosZ_SourceHP_QLineEdit->text().toFloat();
	theApp.sourceObjects->GenerateSourceActor_sourceHP(center);
	// Set panel info 
	PosX_SourceHP_QLineEdit->clear();
	PosY_SourceHP_QLineEdit->clear();
	PosZ_SourceHP_QLineEdit->clear();
	RI_Radionuclide_sourceHP_QLineEdit->clear();
	RI_Activity_sourceHP_QLineEdit->clear();

	theApp.SetMessageBox_RadionuclideWarning("The radionuclide data provided in this code are based on the data provided in the ICRP 107 publication.\nTHIS DATA DO NOT INCLUDE ANY DAUGHTER NUCLIDES!!!");
}
void HotParticleWidget::slot_RIList_Delete_sourceHP_ButtonClicked()
{
	// Button의 Index 찾기
	QPushButton* buttonSender = qobject_cast<QPushButton*>(sender());
	QString buttonText = buttonSender->objectName();

	// The first part is the ID
	int RadonuclideIndex = buttonText.toDouble();
	int row = RI_List_sourceHP_QListWidget->row(RI_List_sourceHP_QListWidget->item(RadonuclideIndex));
	QListWidgetItem* toRemove = RI_List_sourceHP_QListWidget->takeItem(row);
	delete toRemove; // Delete the item to free memory

	if (RadonuclideIndex >= 0 && RadonuclideIndex < ListInfo_sourceHP.size()) 
	{
		ListInfo_sourceHP.erase(ListInfo_sourceHP.begin() + RadonuclideIndex);
		RIList_Delete_sourceHP_QButton.erase(RIList_Delete_sourceHP_QButton.begin() + RadonuclideIndex);
		RIList_Info_sourceHP_QButton.erase(RIList_Info_sourceHP_QButton.begin() + RadonuclideIndex);
		// Actor delete
		theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(theApp.SourcePanelActor_Position[RadonuclideIndex]);
		theApp.m_pVTKWidget->renderWindow()->Render();
		theApp.SourcePanelActor_Position.erase(theApp.SourcePanelActor_Position.begin() + RadonuclideIndex);
	}
	else
	{
		theApp.SetMessageBox("RI_list_Info_sourceHP Vecetor index error");
	}

	for (int index = 0; index < RIList_Delete_sourceHP_QButton.size(); index++)
	{
		RIList_Delete_sourceHP_QButton[index]->setObjectName(QString::number(index));
		RIList_Info_sourceHP_QButton[index]->setObjectName(QString::number(index));
	}
	RIList_sourceHP_MakingIndex--; // listwidget은 vector 형태로 동작해야 함(row의 item이 실제 몇 행인지로 결정됨)	
}
void HotParticleWidget::slot_RIList_Info_sourceHP_ButtonClicked()
{
	// Button의 Index 찾기
	QPushButton* buttonSender = qobject_cast<QPushButton*>(sender());
	QString buttonText = buttonSender->objectName();

	QStringList list_button = buttonText.split("_");

	// The first part is the ID
	int RadonuclideIndex = list_button[0].toInt();
	QListWidgetItem* item = RI_List_sourceHP_QListWidget->item(RadonuclideIndex);
	if (!item) {
		// Handle error: item not found
		return;
	}

	QWidget* widget = RI_List_sourceHP_QListWidget->itemWidget(item);
	QHBoxLayout* hLayout = qobject_cast<QHBoxLayout*>(widget->layout());
	if (!hLayout) {
		// Handle error: layout not found
		return;
	}

	// Assuming the order of labels in the layout is known
	QLabel* labelRadionuclide = qobject_cast<QLabel*>(hLayout->itemAt(0)->widget());
	QLabel* labelActivity = qobject_cast<QLabel*>(hLayout->itemAt(1)->widget());

	// Create a new dialog
	QDialog* infoDialog = new QDialog(this);
	infoDialog->setStyleSheet("background-color: white;");
	infoDialog->resize(panel->Window_width * 0.3, panel->Window_height * 0.55);
	infoDialog->setWindowTitle("Radionuclide Information");

	// Create a layout and a label for the dialog
	QVBoxLayout* layout = new QVBoxLayout(infoDialog);
	// Create a QTextEdit for the dialog
	QTextEdit* textEdit = new QTextEdit(infoDialog);
	textEdit->setReadOnly(true); // Make it read-only if editing is not required

	// Create a QTextCursor for inserting formatted text
	QTextCursor cursor = textEdit->textCursor();

	// Define different text formats
	QTextCharFormat boldFormat;
	boldFormat.setFontFamily("Courier New"); // This is monospaced font
	boldFormat.setFontWeight(QFont::Bold);
	boldFormat.setFontPointSize(14);

	QTextCharFormat boldMediumFormat;
	boldMediumFormat.setFontFamily("Courier New");
	boldMediumFormat.setFontWeight(QFont::Bold);
	boldMediumFormat.setFontPointSize(12);

	QTextCharFormat boldSmallFormat;
	boldSmallFormat.setFontFamily("Courier New");
	boldSmallFormat.setFontWeight(QFont::Bold);
	boldSmallFormat.setFontPointSize(10);

	QTextCharFormat normalFormat;
	normalFormat.setFontFamily("Courier New");
	normalFormat.setFontPointSize(10);

	QTextCharFormat smallFormat;
	smallFormat.setFontFamily("Courier New");
	smallFormat.setFontPointSize(8);

	QTextCharFormat verySmallFormat;
	verySmallFormat.setFontFamily("Courier New");
	verySmallFormat.setFontPointSize(6);

	// Insert formatted text into QTextEdit
	cursor.insertText("*This radionuclide information is based on ICRP 107 Publication\n\n", smallFormat);
	cursor.insertText("Radionuclide Information\n", boldFormat);
	cursor.insertText("Radionuclide: " + labelRadionuclide->text() + "\n", normalFormat);
	cursor.insertText("Activity (Bq): " + labelActivity->text() + "\n", normalFormat);

	// Fine RN information from ICRP07.NDX file (ICRP 107 Pub.)
	QFile file("./data/radioisotopes/_ICRP-07.NDX");
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		// Handle error: Unable to open file
		return;
	}
	QTextStream in(&file);
	QString line;
	bool isDataFound = false;
	QString halfLife, daughterNucleus1, branchingFraction1, daughterNucleus2, branchingFraction2, daughterNucleus3, branchingFraction3, daughterNucleus4, branchingFraction4;
	int daughterNucleus1_index = 7; // 이상적으로 8열(Index: 7)에서 딸핵종1 시작

	while (!in.atEnd()) {
		line = in.readLine();
		if (line.trimmed().isEmpty()) continue; // Skip empty lines
		QStringList columns = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
		if (columns[0] == labelRadionuclide->text()) {
			isDataFound = true;
			halfLife = columns[1]; // Extract HalfLife
			bool isNumber; double tmpNum;
			tmpNum = columns[3].toDouble(&isNumber); // 이상적으로는 숫자, 하지만 DecayMode 알파벳이 할당되어 있을수도 있음
			if (!isNumber) daughterNucleus1_index++; // Decay Mode has assgined to two columns
			daughterNucleus1 = columns[daughterNucleus1_index];
			branchingFraction1 = columns[daughterNucleus1_index + 2];
			tmpNum = columns[daughterNucleus1_index + 3].toDouble(&isNumber);
			if (isNumber) break;// 이상적으로는 딸핵종, 하지만 딸핵종이 더 이상 없어 0이 할당되어 있을수도 있음

			daughterNucleus2 = columns[daughterNucleus1_index + 3];
			branchingFraction2 = columns[daughterNucleus1_index + 5];
			tmpNum = columns[daughterNucleus1_index + 6].toDouble(&isNumber);
			if (isNumber) break;// 이상적으로는 딸핵종, 하지만 딸핵종이 더 이상 없어 0이 할당되어 있을수도 있음

			daughterNucleus3 = columns[daughterNucleus1_index + 6];
			branchingFraction3 = columns[daughterNucleus1_index + 8];
			tmpNum = columns[daughterNucleus1_index + 9].toDouble(&isNumber);
			if (isNumber) break;// 이상적으로는 딸핵종, 하지만 딸핵종이 더 이상 없어 0이 할당되어 있을수도 있음

			daughterNucleus4 = columns[daughterNucleus1_index + 9];
			branchingFraction4 = columns[daughterNucleus1_index + 11];
			break; // Stop reading further as the data is found
		}
	}
	file.close();

	cursor.insertText("HalfLife: " + halfLife + "\n\n", normalFormat);
	cursor.insertText("Daughter Nuclides Information\n", boldFormat);
	cursor.insertText("Radionuclide   Fraction\n", boldSmallFormat);

	// Insert text into QTextEdit with padding
	if (!daughterNucleus1.isEmpty()) {
		QString padding(15 - daughterNucleus1.length(), ' ');
		cursor.insertText(daughterNucleus1 + padding + branchingFraction1 + "\n", normalFormat);
	}
	if (!daughterNucleus2.isEmpty()) {
		QString padding(15 - daughterNucleus2.length(), ' ');
		cursor.insertText(daughterNucleus2 + padding + branchingFraction2 + "\n", normalFormat);
	}
	if (!daughterNucleus3.isEmpty()) {
		QString padding(15 - daughterNucleus3.length(), ' ');
		cursor.insertText(daughterNucleus3 + padding + branchingFraction3 + "\n", normalFormat);
	}
	if (!daughterNucleus4.isEmpty()) {
		QString padding(15 - daughterNucleus4.length(), ' ');
		cursor.insertText(daughterNucleus4 + padding + branchingFraction4 + "\n", normalFormat);
	}

	cursor.insertText("\nRadiation Spectrum of the Entered Radionuclide\n", boldFormat);
	cursor.insertText("Radiation      Fraction       Energy (MeV)\n", boldSmallFormat);

	QString RNspectrumFileName = ("./data/radioisotopes/" + labelRadionuclide->text() + ".txt");
	QFile RNspectrumFile(RNspectrumFileName);
	if (!RNspectrumFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		// Handle error: Unable to open file
		return;
	}

	QTextStream RNspectrumInput(&RNspectrumFile);
	QString RNspectrumLine;

	// Skip the first line
	if (!RNspectrumInput.atEnd()) {
		RNspectrumLine = RNspectrumInput.readLine();
	}
		
	while (!RNspectrumInput.atEnd()) {
		RNspectrumLine = RNspectrumInput.readLine();
		QStringList columns = RNspectrumLine.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);

		if (columns.size() >= 4) {
			QString radiation = columns[3];
			QString fraction = columns[1];
			QString energy = columns[2];
			
 			QString padding1(15 - radiation.length(), ' ');
			QString padding2(15 - fraction.length(), ' ');
			cursor.insertText(radiation + padding1 + fraction + padding2 + energy + "\n", normalFormat);
		}
	}
	RNspectrumFile.close();

	cursor.insertText("\n*Alpha recoil nuclei and fission fragments are excluded.\n*PG, DG, and DB are radiations of spontaneous fission.\n", smallFormat);
	cursor.insertText("G: Gamma rays\n", verySmallFormat);
	cursor.insertText("PG: Prompt gamma rays\n", verySmallFormat);
	cursor.insertText("DG: Delayed gamma rays\n", verySmallFormat);
	cursor.insertText("X: X rays\n", verySmallFormat);
	cursor.insertText("AQ: Annihilation photons\n", verySmallFormat);
	cursor.insertText("B+: Beta-plus particles\n", verySmallFormat);
	cursor.insertText("B-: Beta-minus particles\n", verySmallFormat);
	cursor.insertText("DB: Delayed beta particles\n", verySmallFormat);
	cursor.insertText("IE: Internal conversion electrons\n", verySmallFormat);
	cursor.insertText("AE: Auger electrons\n", verySmallFormat);
	cursor.insertText("A: Alpha particles\n", verySmallFormat);
	cursor.insertText("N: Neutrons\n", verySmallFormat);

	cursor.movePosition(QTextCursor::Start);
	textEdit->setTextCursor(cursor);

	// Add the QTextEdit to the layout
	layout->addWidget(textEdit);

	// Set the layout to the dialog
	infoDialog->setLayout(layout);

	// Show the dialog
	infoDialog->exec(); // Use exec() for a modal dialog or show() for a non-modal dialog
}

void HotParticleWidget::slot_ES_Select_sourceHP_RadioButtonClicked()
{
	RI_Select_sourceHP_QRadioButton->setChecked(false);
	ES_Select_sourceHP_QRadioButton->setChecked(true);

	RI_Title_sourceHP_QLabel->hide();
	RI_Radionuclide_sourceHP_QLineEdit->hide();
	RI_Activity_sourceHP_QLabel->hide();
	RI_Activity_sourceHP_QLineEdit->hide();
	RI_List_sourceHP_QLabel->hide();
	RIList_Add_sourceHP_QButton->hide();
	RI_List_sourceHP_QListWidget->hide();

	ES_Title_sourceHP_QLabel->show();
	sourceHP_EnergyspectrumFileLoadButton->show();
	ES_Intensity_sourceHP_QLabel->show();
	ES_Intensity_sourceHP_QLineEdit->show();
	ES_FileName_sourceHP_QLabel->show();
	sourceHP_EnergyspectrumFileNameText->show();
	ES_List_sourceHP_QLabel->show();
	ESList_Add_sourceHP_QButton->show();
	ES_List_sourceHP_QListWidget->show();

	// === RI 관련 초기화 ===

	// 1. VTK 렌더러에서 모든 관련 Actor 제거
	// a. 먼저 루프를 돌면서 각 Actor를 렌더러에서 제거합니다.
	for (vtkActor* actor : theApp.SourcePanelActor_Position)
	{
		if (actor) // 포인터가 유효한지 확인
		{
			theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(actor);
		}
	}
	// b. VTK 렌더 윈도우를 업데이트하여 변경 사항을 적용합니다.
	theApp.m_pVTKWidget->renderWindow()->Render();

	// 2. QListWidget의 모든 아이템 제거
	// clear() 함수는 위젯의 모든 아이템을 삭제하고 메모리에서 해제합니다.
	RI_List_sourceHP_QListWidget->clear();

	// 3. 관리하던 모든 vector의 내용 비우기
	// 각 vector의 clear() 멤버 함수를 호출하여 모든 요소를 제거합니다.
	ListInfo_sourceHP.clear();
	RIList_Delete_sourceHP_QButton.clear();
	RIList_Info_sourceHP_QButton.clear();
	theApp.SourcePanelActor_Position.clear();

	// 4. 인덱스 카운터 초기화
	// 리스트 아이템 개수를 추적하는 변수를 초기값(보통 0)으로 리셋합니다.
	RIList_sourceHP_MakingIndex = 0;
}
void HotParticleWidget::slot_ESList_Add_sourceHP_ButtonClicked()
{
	// 지원하는 RI 리스트에 있는지 판별
	QString inputText = sourceHP_EnergyspectrumFileNameText->toPlainText();
	if (inputText.isEmpty()) {
		theApp.SetMessageBox("Energy spectrum file is empty!");
		return;
	}

	QListWidgetItem* item = new QListWidgetItem(ES_List_sourceHP_QListWidget);
	QWidget* widget = new QWidget;
	QHBoxLayout* hLayout = new QHBoxLayout;
	widget->setStyleSheet("background-color: white;");

	QString arg1 = QFileInfo(inputText).fileName();
	QLabel* label1 = new QLabel(arg1);
	label1->setFixedWidth(panel->Window_width * 0.025);
	label1->setFont(panel->font_D_LW2); // Set font for the label
	label1->setToolTip(arg1); // 전체 파일명 툴팁
	label1->setScaledContents(false);
	label1->setTextInteractionFlags(Qt::TextSelectableByMouse); // 선택 가능 (옵션)

	QString arg2 = QString::number(ES_Intensity_sourceHP_QLineEdit->text().toDouble(), 'e', 3);
	QLabel* label2 = new QLabel(arg2);
	label2->setFixedWidth(panel->Window_width * 0.025);
	label2->setFont(panel->font_D_LW2); // Set font for the label

	QString arg3 = PosX_SourceHP_QLineEdit->text().left(6);
	if (arg3 == "") arg3 = "0";
	QLabel* labelPosX = new QLabel(arg3);
	labelPosX->setFixedWidth(panel->Window_width * 0.025);
	labelPosX->setFont(panel->font_D_LW2); // Set font for the label

	QString arg4 = PosY_SourceHP_QLineEdit->text().left(6);
	if (arg4 == "") arg4 = "0";
	QLabel* labelPosY = new QLabel(arg4);
	labelPosY->setFixedWidth(panel->Window_width * 0.025);
	labelPosY->setFont(panel->font_D_LW2); // Set font for the label

	QString arg5 = PosZ_SourceHP_QLineEdit->text().left(6);
	if (arg5 == "") arg5 = "0";
	QLabel* labelPosZ = new QLabel(arg5);
	labelPosZ->setFixedWidth(panel->Window_width * 0.025);
	labelPosZ->setFont(panel->font_D_LW2); // Set font for the label

	ESList_Delete_sourceHP_QButton.push_back(new QPushButton());
	if (RIList_sourceHP_MakingIndex >= ESList_Delete_sourceHP_QButton.size()) theApp.SetMessageBox("ESList_Delete_sourceHP_QButton Vector index error");
	ESList_Delete_sourceHP_QButton[RIList_sourceHP_MakingIndex]->setCheckable(false);
	ESList_Delete_sourceHP_QButton[RIList_sourceHP_MakingIndex]->setStyleSheet(DialogStyle::MENU_BTN13);
	ESList_Delete_sourceHP_QButton[RIList_sourceHP_MakingIndex]->setFont(panel->font_D_BTN13);
	ESList_Delete_sourceHP_QButton[RIList_sourceHP_MakingIndex]->setFixedSize(panel->Window_width * 0.008, panel->Window_height * 0.012);
	ESList_Delete_sourceHP_QButton[RIList_sourceHP_MakingIndex]->setIcon(QIcon("./data/image/delete.png"));
	ESList_Delete_sourceHP_QButton[RIList_sourceHP_MakingIndex]->setObjectName(QString::number(RIList_sourceHP_MakingIndex));

	ESList_Info_sourceHP_QButton.push_back(new QPushButton());
	if (RIList_sourceHP_MakingIndex >= ESList_Info_sourceHP_QButton.size()) theApp.SetMessageBox("ESList_Info_sourceHP_QButton Vector index error");
	ESList_Info_sourceHP_QButton[RIList_sourceHP_MakingIndex]->setCheckable(false);
	ESList_Info_sourceHP_QButton[RIList_sourceHP_MakingIndex]->setStyleSheet(DialogStyle::MENU_BTN13);
	ESList_Info_sourceHP_QButton[RIList_sourceHP_MakingIndex]->setFont(panel->font_D_BTN13);
	ESList_Info_sourceHP_QButton[RIList_sourceHP_MakingIndex]->setFixedSize(panel->Window_width * 0.008, panel->Window_height * 0.012);
	ESList_Info_sourceHP_QButton[RIList_sourceHP_MakingIndex]->setIcon(QIcon("./data/image/info.png"));
	ESList_Info_sourceHP_QButton[RIList_sourceHP_MakingIndex]->setObjectName(QString::number(RIList_sourceHP_MakingIndex));

	hLayout->addWidget(label1);
	hLayout->addWidget(label2);
	hLayout->addWidget(labelPosX);
	hLayout->addWidget(labelPosY);
	hLayout->addWidget(labelPosZ);
	hLayout->addWidget(ESList_Delete_sourceHP_QButton[RIList_sourceHP_MakingIndex]);
	hLayout->addWidget(ESList_Info_sourceHP_QButton[RIList_sourceHP_MakingIndex]);
	widget->setLayout(hLayout);

	ES_List_sourceHP_QListWidget->setItemWidget(item, widget);
	item->setSizeHint(widget->sizeHint());

	// Connect signals of the new buttons
	connect(ESList_Delete_sourceHP_QButton[RIList_sourceHP_MakingIndex], SIGNAL(clicked()), this, SLOT(slot_ESList_Delete_sourceHP_ButtonClicked())); connect(ESList_Delete_sourceHP_QButton[RIList_sourceHP_MakingIndex], SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
	connect(ESList_Info_sourceHP_QButton[RIList_sourceHP_MakingIndex], SIGNAL(clicked()), this, SLOT(slot_ESList_Info_sourceHP_ButtonClicked())); connect(ESList_Info_sourceHP_QButton[RIList_sourceHP_MakingIndex], SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));

	// Generate actor
	double center[3] = { 0.0, };
	center[0] = PosX_SourceHP_QLineEdit->text().toFloat();
	center[1] = PosY_SourceHP_QLineEdit->text().toFloat();
	center[2] = PosZ_SourceHP_QLineEdit->text().toFloat();

	theApp.sourceObjects->GenerateSourceActor_sourceHP(center);

	// Set Global Variables
	std::map<int, QString> tmp_map;
	tmp_map[0] = "True";
	tmp_map[1] = PosX_SourceHP_QLineEdit->text(); if (tmp_map[1] == "") tmp_map[1] = "0";
	tmp_map[2] = PosY_SourceHP_QLineEdit->text(); if (tmp_map[2] == "") tmp_map[2] = "0";
	tmp_map[3] = PosZ_SourceHP_QLineEdit->text(); if (tmp_map[3] == "") tmp_map[3] = "0";
	tmp_map[4] = inputText;
	tmp_map[5] = ES_Intensity_sourceHP_QLineEdit->text(); if (tmp_map[5] == "") tmp_map[5] = "0";
	ListInfo_sourceHP.push_back(tmp_map);
	RIList_sourceHP_MakingIndex++;

	// Set panel info 
	PosX_SourceHP_QLineEdit->clear();
	PosY_SourceHP_QLineEdit->clear();
	PosZ_SourceHP_QLineEdit->clear();
	ES_Intensity_sourceHP_QLineEdit->clear();
	sourceHP_EnergyspectrumFileNameText->clear();
}
void HotParticleWidget::slot_ESList_Delete_sourceHP_ButtonClicked()
{
	// Button의 Index 찾기
	QPushButton* buttonSender = qobject_cast<QPushButton*>(sender());
	QString buttonText = buttonSender->objectName();

	// The first part is the ID
	int EnersySpectrumIndex = buttonText.toDouble();
	int row = ES_List_sourceHP_QListWidget->row(ES_List_sourceHP_QListWidget->item(EnersySpectrumIndex));
	QListWidgetItem* toRemove = ES_List_sourceHP_QListWidget->takeItem(row);
	delete toRemove; // Delete the item to free memory

	if (EnersySpectrumIndex >= 0 && EnersySpectrumIndex < ListInfo_sourceHP.size())
	{
		ListInfo_sourceHP.erase(ListInfo_sourceHP.begin() + EnersySpectrumIndex);
		ESList_Delete_sourceHP_QButton.erase(ESList_Delete_sourceHP_QButton.begin() + EnersySpectrumIndex);
		ESList_Info_sourceHP_QButton.erase(ESList_Info_sourceHP_QButton.begin() + EnersySpectrumIndex);
		// Actor delete
		theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(theApp.SourcePanelActor_Position[EnersySpectrumIndex]);
		theApp.m_pVTKWidget->renderWindow()->Render();
		theApp.SourcePanelActor_Position.erase(theApp.SourcePanelActor_Position.begin() + EnersySpectrumIndex);
	}
	else {
		theApp.SetMessageBox("sourceHP_info Vecetor index error");
	}

	for (int index = 0; index < ESList_Delete_sourceHP_QButton.size(); index++)
	{
		ESList_Delete_sourceHP_QButton[index]->setObjectName(QString::number(index));
		ESList_Info_sourceHP_QButton[index]->setObjectName(QString::number(index));
	}
	RIList_sourceHP_MakingIndex--; // listwidget은 vector 형태로 동작해야 함(row의 item이 실제 몇 행인지로 결정됨)	
}
void HotParticleWidget::slot_ESList_Info_sourceHP_ButtonClicked()
{
	// Button의 Index 찾기
	QPushButton* buttonSender = qobject_cast<QPushButton*>(sender());
	QString buttonText = buttonSender->objectName();

	QStringList list_button = buttonText.split("_");

	// The first part is the ID
	int EnergySpectrumIndex = list_button[0].toInt();

	QListWidgetItem* item = ES_List_sourceHP_QListWidget->item(EnergySpectrumIndex);
	if (!item) {
		// Handle error: item not found
		return;
	}

	QWidget* widget = ES_List_sourceHP_QListWidget->itemWidget(item);
	QHBoxLayout* hLayout = qobject_cast<QHBoxLayout*>(widget->layout());
	if (!hLayout) {
		// Handle error: layout not found
		return;
	}

	// Assuming the order of labels in the layout is known
	QLabel* labelEnergySpectrumFile = qobject_cast<QLabel*>(hLayout->itemAt(0)->widget());
	QLabel* labelIntensity = qobject_cast<QLabel*>(hLayout->itemAt(1)->widget());
	QLabel* labelX = qobject_cast<QLabel*>(hLayout->itemAt(2)->widget());
	QLabel* labelY = qobject_cast<QLabel*>(hLayout->itemAt(3)->widget());
	QLabel* labelZ = qobject_cast<QLabel*>(hLayout->itemAt(4)->widget());

	// Create a new dialog
	QDialog* infoDialog = new QDialog(this);
	infoDialog->setStyleSheet("background-color: white;");
	infoDialog->resize(panel->Window_width * 0.3, panel->Window_height * 0.55);
	infoDialog->setWindowTitle("Radionuclide Information");

	// Create a layout and a label for the dialog
	QVBoxLayout* layout = new QVBoxLayout(infoDialog);
	// Create a QTextEdit for the dialog
	QTextEdit* textEdit = new QTextEdit(infoDialog);
	textEdit->setReadOnly(true); // Make it read-only if editing is not required

	// Create a QTextCursor for inserting formatted text
	QTextCursor cursor = textEdit->textCursor();

	// Define different text formats
	QTextCharFormat boldFormat;
	boldFormat.setFontFamily("Courier New"); // This is monospaced font
	boldFormat.setFontWeight(QFont::Bold);
	boldFormat.setFontPointSize(14);

	QTextCharFormat boldMediumFormat;
	boldMediumFormat.setFontFamily("Courier New");
	boldMediumFormat.setFontWeight(QFont::Bold);
	boldMediumFormat.setFontPointSize(12);

	QTextCharFormat boldSmallFormat;
	boldSmallFormat.setFontFamily("Courier New");
	boldSmallFormat.setFontWeight(QFont::Bold);
	boldSmallFormat.setFontPointSize(10);

	QTextCharFormat normalFormat;
	normalFormat.setFontFamily("Courier New");
	normalFormat.setFontPointSize(10);

	QTextCharFormat smallFormat;
	smallFormat.setFontFamily("Courier New");
	smallFormat.setFontPointSize(8);

	QTextCharFormat verySmallFormat;
	verySmallFormat.setFontFamily("Courier New");
	verySmallFormat.setFontPointSize(6);

	// Insert formatted text into QTextEdit	
	cursor.insertText("EnergySpectrum Information\n", boldFormat);
	cursor.insertText("FileName: " + labelEnergySpectrumFile->text() + "\n", normalFormat);
	cursor.insertText("Intensity (#/s): " + labelIntensity->text() + "\n", normalFormat);
	cursor.insertText("PosX (cm): " + labelX->text() + "\n", normalFormat);
	cursor.insertText("PosY (cm): " + labelY->text() + "\n", normalFormat);
	cursor.insertText("PosZ (cm): " + labelZ->text() + "\n", normalFormat);

	cursor.insertText("\nRadiation Spectrum of the Entered File\n", boldFormat);
	cursor.insertText("Radiation      Fraction       Energy (MeV)\n", boldSmallFormat);

	QString ESFileName = ListInfo_sourceHP[EnergySpectrumIndex][4];
	QFile ESFile(ESFileName);
	if (!ESFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		// Handle error: Unable to open file
		return;
	}

	QTextStream ESInput(&ESFile);
	while (!ESInput.atEnd()) {
		QString ESLine = ESInput.readLine();
		QStringList columns = ESLine.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);

		if (columns.size() >= 3) {
			QString radiation = columns[0];
			QString fraction = columns[1];
			QString energy = columns[2];

			QString padding1(15 - radiation.length(), ' ');
			QString padding2(15 - fraction.length(), ' ');
			cursor.insertText(radiation + padding1 + fraction + padding2 + energy + "\n", normalFormat);
		}
	}
	ESFile.close();

	cursor.movePosition(QTextCursor::Start);
	textEdit->setTextCursor(cursor);

	// Add the QTextEdit to the layout
	layout->addWidget(textEdit);

	// Set the layout to the dialog
	infoDialog->setLayout(layout);

	// Show the dialog
	infoDialog->exec(); // Use exec() for a modal dialog or show() for a non-modal dialog
}
void HotParticleWidget::slot_EnergyspectrumFileLoad_sourceHP_ButtonClicked()
{
	QString filter;
	filter = tr("(*.*)");
	QString dir = QFileDialog::getOpenFileName(
		this,
		tr("Select a file to open"),
		"",
		filter);

	if (!dir.isEmpty())
	{
		sourceHP_EnergyspectrumFileNameText->setText(dir);
		sourceHP_EnergyspectrumFileNameText->setReadOnly(true);
		QFont font;
		font.setFamily("Arial");    // Font family
		font.setPointSize(panel->FontSizeScaling(7));      // Font size
		sourceHP_EnergyspectrumFileNameText->setFont(font);
	}
}
//////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
// ConeBeamWidget Implementation
///////////////////////////////////////////////////////////////////////////

ConeBeamWidget::ConeBeamWidget(QWidget* parent)
    : ParentT(parent)
{
}

ConeBeamWidget::~ConeBeamWidget()
{
}

bool ConeBeamWidget::initialize()
{
    if (!ParentT::initialize()) return false;

    QVBoxLayout* subLayout = new QVBoxLayout;
    subLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.005, panel->Window_width * 0.003, panel->Window_height * 0.005);
    subLayout->setSpacing(panel->Window_width * 0.003);
    subLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    {
        QVBoxLayout* vLayout = new QVBoxLayout;
        vLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.005, panel->Window_width * 0.003, panel->Window_height * 0.005);
        vLayout->setSpacing(panel->Window_width * 0.003);
        vLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

        // [Visualization & Direction]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setSpacing(panel->Window_width * 0.005);
            hLayout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

            QLabel* labelTitle = new QLabel;
            labelTitle->setFixedSize(panel->Window_width * 0.104, panel->Window_height * 0.028);
            labelTitle->setStyleSheet(DialogStyle::DATA_LABEL);
            labelTitle->setFont(panel->font_D_LBL1);
            labelTitle->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            labelTitle->setText(QString::fromLocal8Bit("Visualization"));

            m_ConeBeamSourceVisualizationButton = new QPushButton(QString::fromLocal8Bit("On"));
            panel->Buttons_FunctionPanelRight.append(m_ConeBeamSourceVisualizationButton);
            m_ConeBeamSourceVisualizationButton->setFixedSize(panel->Window_width * 0.05, panel->Window_height * 0.028);
            m_ConeBeamSourceVisualizationButton->setCheckable(true);
            m_ConeBeamSourceVisualizationButton->setChecked(true);
            m_ConeBeamSourceVisualizationButton->setStyleSheet(DialogStyle::MENU_BTN3);
            m_ConeBeamSourceVisualizationButton->setFont(panel->font_D_BTN3);

            QLabel* labelDir = new QLabel;
            labelDir->setFixedSize(panel->Window_width * 0.03, panel->Window_height * 0.028);
            labelDir->setStyleSheet(DialogStyle::DATA_LABEL);
            labelDir->setFont(panel->font_D_LBL4);
            labelDir->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            labelDir->setText(QString::fromLocal8Bit("Direction"));

            sourceCB_dirCheckBox = new QCheckBox;
            sourceCB_dirCheckBox->setChecked(true);             
            sourceCB_dirCheckBox->setFixedSize(panel->Window_width * 0.02, panel->Window_height * 0.02);
            sourceCB_dirCheckBox->setStyleSheet("QCheckBox { background-color: transparent; }");
            
            connect(m_ConeBeamSourceVisualizationButton, SIGNAL(clicked()), this, SLOT(slot_ConeBeamSourceVisualization_ButtonClicked())); 
            connect(m_ConeBeamSourceVisualizationButton, SIGNAL(clicked()), panel, SLOT(slot_allButtonClicked()));

            hLayout->addWidget(labelTitle);         
            hLayout->addWidget(m_ConeBeamSourceVisualizationButton);
            hLayout->addStretch();
            hLayout->addWidget(labelDir);
            hLayout->addWidget(sourceCB_dirCheckBox);

            vLayout->addLayout(hLayout);
        }   
    
        // [Position XYZ]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setContentsMargins(panel->Window_width * 0.003, 0, panel->Window_width * 0.003, 0);
            hLayout->setSpacing(panel->Window_width * 0.003);
            hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

            QLabel* label0 = new QLabel;
            label0->setFixedSize(panel->Window_width * 0.058, panel->Window_height * 0.028);
            label0->setStyleSheet(DialogStyle::DATA_LABEL);
            label0->setFont(panel->font_D_LBL1);
            label0->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
            label0->setText(QString::fromLocal8Bit("Position XYZ:"));

            m_lineEditConeBeamPointX = new QLineEdit;
            m_lineEditConeBeamPointX->setFixedSize(panel->Window_width * 0.033, panel->Window_height * 0.028);
            m_lineEditConeBeamPointX->setAlignment(Qt::AlignCenter);
            m_lineEditConeBeamPointX->setStyleSheet(DialogStyle::LINEEDIT_NAME5);
            m_lineEditConeBeamPointX->setFont(panel->font_D_LE5);
            m_lineEditConeBeamPointX->setPlaceholderText(QString::fromLocal8Bit("(cm)"));

            m_lineEditConeBeamPointY = new QLineEdit;
            m_lineEditConeBeamPointY->setFixedSize(panel->Window_width * 0.033, panel->Window_height * 0.028);
            m_lineEditConeBeamPointY->setAlignment(Qt::AlignCenter);
            m_lineEditConeBeamPointY->setStyleSheet(DialogStyle::LINEEDIT_NAME5);
            m_lineEditConeBeamPointY->setFont(panel->font_D_LE5);
            m_lineEditConeBeamPointY->setPlaceholderText(QString::fromLocal8Bit("(cm)"));

            m_lineEditConeBeamPointZ = new QLineEdit;
            m_lineEditConeBeamPointZ->setFixedSize(panel->Window_width * 0.033, panel->Window_height * 0.028);
            m_lineEditConeBeamPointZ->setAlignment(Qt::AlignCenter);
            m_lineEditConeBeamPointZ->setStyleSheet(DialogStyle::LINEEDIT_NAME5);
            m_lineEditConeBeamPointZ->setFont(panel->font_D_LE5);
            m_lineEditConeBeamPointZ->setPlaceholderText(QString::fromLocal8Bit("(cm)"));

            hLayout->addWidget(label0);
            hLayout->addStretch();
            hLayout->addWidget(m_lineEditConeBeamPointX);
            hLayout->addWidget(m_lineEditConeBeamPointY);
            hLayout->addWidget(m_lineEditConeBeamPointZ);

            vLayout->addLayout(hLayout);
        }           

        // [Direction Vector XYZ]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setContentsMargins(panel->Window_width * 0.003, 0, panel->Window_width * 0.003, 0);
            hLayout->setSpacing(panel->Window_width * 0.003);
            hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

            QLabel* label0 = new QLabel;
            label0->setFixedSize(panel->Window_width * 0.058, panel->Window_height * 0.028);
            label0->setStyleSheet(DialogStyle::DATA_LABEL);
            label0->setFont(panel->font_D_LBL1);
            label0->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
            label0->setText(QString::fromLocal8Bit("Vector XYZ:"));

            m_lineEditConeBeamDirectionX = new QLineEdit;
            m_lineEditConeBeamDirectionX->setFixedSize(panel->Window_width * 0.033, panel->Window_height * 0.028);
            m_lineEditConeBeamDirectionX->setAlignment(Qt::AlignCenter);
            m_lineEditConeBeamDirectionX->setStyleSheet(DialogStyle::LINEEDIT_NAME5);
            m_lineEditConeBeamDirectionX->setFont(panel->font_D_LE5);

            m_lineEditConeBeamDirectionY = new QLineEdit;
            m_lineEditConeBeamDirectionY->setFixedSize(panel->Window_width * 0.033, panel->Window_height * 0.028);
            m_lineEditConeBeamDirectionY->setAlignment(Qt::AlignCenter);
            m_lineEditConeBeamDirectionY->setStyleSheet(DialogStyle::LINEEDIT_NAME5);
            m_lineEditConeBeamDirectionY->setFont(panel->font_D_LE5);

            m_lineEditConeBeamDirectionZ = new QLineEdit;
            m_lineEditConeBeamDirectionZ->setFixedSize(panel->Window_width * 0.033, panel->Window_height * 0.028);
            m_lineEditConeBeamDirectionZ->setAlignment(Qt::AlignCenter);
            m_lineEditConeBeamDirectionZ->setStyleSheet(DialogStyle::LINEEDIT_NAME5);
            m_lineEditConeBeamDirectionZ->setFont(panel->font_D_LE5);

            hLayout->addWidget(label0);
            hLayout->addStretch();
            hLayout->addWidget(m_lineEditConeBeamDirectionX);
            hLayout->addWidget(m_lineEditConeBeamDirectionY);
            hLayout->addWidget(m_lineEditConeBeamDirectionZ);

            vLayout->addLayout(hLayout);
        }

        // [Solid Angle]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setContentsMargins(panel->Window_width * 0.003, 0, panel->Window_width * 0.003, 0);
            hLayout->setSpacing(panel->Window_width * 0.003);
            hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

            QLabel* label0 = new QLabel;
            label0->setFixedSize(panel->Window_width * 0.058, panel->Window_height * 0.028);
            label0->setStyleSheet(DialogStyle::DATA_LABEL);
            label0->setFont(panel->font_D_LBL1);
            label0->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
            label0->setText(QString::fromLocal8Bit("Solid angle: "));

            m_lineEditConeBeamDirectionSolidAngle = new QLineEdit;
            m_lineEditConeBeamDirectionSolidAngle->setFixedSize(panel->Window_width * 0.103, panel->Window_height * 0.028);
            m_lineEditConeBeamDirectionSolidAngle->setAlignment(Qt::AlignCenter);
            m_lineEditConeBeamDirectionSolidAngle->setStyleSheet(DialogStyle::LINEEDIT_NAME5);
            m_lineEditConeBeamDirectionSolidAngle->setFont(panel->font_D_LE5);
            m_lineEditConeBeamDirectionSolidAngle->setPlaceholderText(QString::fromLocal8Bit("(deg)"));

            hLayout->addWidget(label0);
            hLayout->addStretch();
            hLayout->addWidget(m_lineEditConeBeamDirectionSolidAngle);

            vLayout->addLayout(hLayout);
        }               

        // [3D Pick & Update Buttons]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setContentsMargins(panel->Window_width * 0.003, 0, panel->Window_width * 0.003, 0);
            hLayout->setSpacing(panel->Window_width * 0.003);
            hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

            pickConeBeam3DButton = new QPushButton(QString::fromLocal8Bit("3D Pick"));
            panel->Buttons_FunctionPanelRight.append(pickConeBeam3DButton);
            pickConeBeam3DButton->setFixedSize(panel->Window_width * 0.050, panel->Window_height * 0.036);
            pickConeBeam3DButton->setCheckable(true);
            pickConeBeam3DButton->setStyleSheet(DialogStyle::MENU_BTN2);
            pickConeBeam3DButton->setFont(panel->font_D_BTN2);

            sourceCB_UpdateButton = new QPushButton(QString::fromLocal8Bit("Update"));
            panel->Buttons_FunctionPanelRight.append(sourceCB_UpdateButton);
            sourceCB_UpdateButton->setFixedSize(panel->Window_width * 0.050, panel->Window_height * 0.036);
            sourceCB_UpdateButton->setStyleSheet(DialogStyle::MENU_BTN2);
            sourceCB_UpdateButton->setFont(panel->font_D_BTN2);

            connect(pickConeBeam3DButton, SIGNAL(clicked()), this, SLOT(slot_ConeBeam_Pick3D_ButtonClicked())); 
            connect(pickConeBeam3DButton, SIGNAL(clicked()), panel, SLOT(slot_allButtonClicked()));
            
            connect(sourceCB_UpdateButton, SIGNAL(clicked()), this, SLOT(slot_ConeBeamUpdate_ButtonClicked())); 
            connect(sourceCB_UpdateButton, SIGNAL(clicked()), panel, SLOT(slot_allButtonClicked()));

            hLayout->addStretch();
            hLayout->addWidget(pickConeBeam3DButton);
            hLayout->addWidget(sourceCB_UpdateButton);

            vLayout->addLayout(hLayout);
        }

        // [Radio Buttons: RI vs ES]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setSpacing(panel->Window_width * 0.010);
            hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            hLayout->setContentsMargins(panel->Window_width * 0.005, panel->Window_height * 0.009, 0, 0);

            sourceCB_Radionuclide_radioButton = new QRadioButton(QString::fromLocal8Bit("Radionuclide"));
            sourceCB_Radionuclide_radioButton->setStyleSheet(DialogStyle::RADIOBUTTON_WIDGET);
            sourceCB_Radionuclide_radioButton->setFont(panel->font_D_RB2);
            sourceCB_Radionuclide_radioButton->setFixedSize(panel->Window_width * 0.081, panel->Window_height * 0.028);
            sourceCB_Radionuclide_radioButton->setChecked(true);

            sourceCB_Energyspectrum_radioButton = new QRadioButton(QString::fromLocal8Bit("Energy spectrum"));
            sourceCB_Energyspectrum_radioButton->setStyleSheet(DialogStyle::RADIOBUTTON_WIDGET);
            sourceCB_Energyspectrum_radioButton->setFont(panel->font_D_RB2);
            sourceCB_Energyspectrum_radioButton->setFixedSize(panel->Window_width * 0.081, panel->Window_height * 0.028);
            sourceCB_Energyspectrum_radioButton->setChecked(false);

            // [수정] 빈 슬롯이 아니라 올바른 슬롯 연결
            connect(sourceCB_Radionuclide_radioButton, SIGNAL(clicked()), this, SLOT(slot_ConeBeamRadionuclide_ButtonClicked())); 
            connect(sourceCB_Radionuclide_radioButton, SIGNAL(clicked()), panel, SLOT(slot_allButtonClicked()));
            
            connect(sourceCB_Energyspectrum_radioButton, SIGNAL(clicked()), this, SLOT(slot_ConeBeamEnergyspectrum_ButtonClicked())); 
            connect(sourceCB_Energyspectrum_radioButton, SIGNAL(clicked()), panel, SLOT(slot_allButtonClicked()));

            hLayout->addWidget(sourceCB_Radionuclide_radioButton);
            hLayout->addWidget(sourceCB_Energyspectrum_radioButton);

            vLayout->addLayout(hLayout);
        }

        // [Radionuclide Title & Input]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setContentsMargins(panel->Window_width * 0.003, 0, panel->Window_width * 0.003, 0);
            hLayout->setSpacing(panel->Window_width * 0.003);
            hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

            labelConeBeamRadionuclideTitle = new QLabel;
            labelConeBeamRadionuclideTitle->setFixedSize(panel->Window_width * 0.068, panel->Window_height * 0.028);
            labelConeBeamRadionuclideTitle->setStyleSheet(DialogStyle::DATA_LABEL);
            labelConeBeamRadionuclideTitle->setFont(panel->font_D_LBL1);
            labelConeBeamRadionuclideTitle->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            labelConeBeamRadionuclideTitle->setText(QString::fromLocal8Bit("Radionuclide"));

            m_lineEditConeBeamRadionuclide = new QLineEdit;
            m_lineEditConeBeamRadionuclide->setFixedSize(panel->Window_width * 0.104, panel->Window_height * 0.028);
            m_lineEditConeBeamRadionuclide->setAlignment(Qt::AlignCenter);
            m_lineEditConeBeamRadionuclide->setStyleSheet(DialogStyle::LINEEDIT_NAME);
            m_lineEditConeBeamRadionuclide->setFont(panel->font_D_LE1);
            m_lineEditConeBeamRadionuclide->setPlaceholderText(QString::fromLocal8Bit("Enter radionuclide"));

            QCompleter *completer = new QCompleter(panel->RIsourceList, this);
            completer->setCaseSensitivity(Qt::CaseInsensitive);
            m_lineEditConeBeamRadionuclide->setCompleter(completer);

            hLayout->addWidget(labelConeBeamRadionuclideTitle);
            hLayout->addStretch();
            hLayout->addWidget(m_lineEditConeBeamRadionuclide);

            vLayout->addLayout(hLayout);
        }

        // [Activity]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setContentsMargins(panel->Window_width * 0.003, 0, panel->Window_width * 0.003, 0);
            hLayout->setSpacing(panel->Window_width * 0.003);
            hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

            labelConeBeamRadionuclideActivity = new QLabel;
            labelConeBeamRadionuclideActivity->setFixedSize(panel->Window_width * 0.068, panel->Window_height * 0.028);
            labelConeBeamRadionuclideActivity->setStyleSheet(DialogStyle::DATA_LABEL);
            labelConeBeamRadionuclideActivity->setFont(panel->font_D_LBL1);
            labelConeBeamRadionuclideActivity->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            labelConeBeamRadionuclideActivity->setText(QString::fromLocal8Bit("Activity"));

            m_lineEditConeBeamActivity = new QLineEdit;
            m_lineEditConeBeamActivity->setFixedSize(panel->Window_width * 0.104, panel->Window_height * 0.028);
            m_lineEditConeBeamActivity->setAlignment(Qt::AlignCenter);
            m_lineEditConeBeamActivity->setStyleSheet(DialogStyle::LINEEDIT_NAME);
            m_lineEditConeBeamActivity->setFont(panel->font_D_LE1);
            m_lineEditConeBeamActivity->setPlaceholderText(QString::fromLocal8Bit("Bq"));

            hLayout->addWidget(labelConeBeamRadionuclideActivity);
            hLayout->addStretch();
            hLayout->addWidget(m_lineEditConeBeamActivity);

            vLayout->addLayout(hLayout);
        }

        // [Radionuclide Add Button]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setContentsMargins(panel->Window_width * 0.003, 0, panel->Window_width * 0.003, 0);
            hLayout->setSpacing(panel->Window_width * 0.003);
            hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

            labelConeBeamRadionuclideList = new QLabel;
            labelConeBeamRadionuclideList->setFixedSize(panel->Window_width * 0.088, panel->Window_height * 0.028);
            labelConeBeamRadionuclideList->setStyleSheet(DialogStyle::DATA_LABEL);
            labelConeBeamRadionuclideList->setFont(panel->font_D_LBL1);
            labelConeBeamRadionuclideList->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
            labelConeBeamRadionuclideList->setText(QString::fromLocal8Bit("Entered radionuclides"));

            ConeBeamRadionuclideAddButton = new QPushButton(QString::fromLocal8Bit("Add"));
            panel->Buttons_FunctionPanelRight.append(ConeBeamRadionuclideAddButton);
            ConeBeamRadionuclideAddButton->setFixedSize(panel->Window_width * 0.042, panel->Window_height * 0.037);
            ConeBeamRadionuclideAddButton->setCheckable(false);
            ConeBeamRadionuclideAddButton->setStyleSheet(DialogStyle::MENU_BTN2);
            ConeBeamRadionuclideAddButton->setFont(panel->font_D_BTN2);

            ConeBeamRadionuclideAddButton->setChecked(false);

            connect(ConeBeamRadionuclideAddButton, SIGNAL(clicked()), this, SLOT(slot_ConeBeamRadionuclideAdd_ButtonClicked())); 
            connect(ConeBeamRadionuclideAddButton, SIGNAL(clicked()), panel, SLOT(slot_allButtonClicked()));

            hLayout->addWidget(labelConeBeamRadionuclideList);
            hLayout->addStretch();
            hLayout->addWidget(ConeBeamRadionuclideAddButton);

            vLayout->addLayout(hLayout);
        }

        // [Radionuclide List]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setContentsMargins(panel->Window_width * 0.003, 0, panel->Window_width * 0.003, 0);
            hLayout->setSpacing(panel->Window_width * 0.003);
            hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

            RI_List_sourceCB_QListWidget = new QListWidget;
            RI_List_sourceCB_QListWidget->setStyleSheet("QListWidget { border-radius: 15px; background-color: white; }");
            RI_List_sourceCB_QListWidget->setFixedSize(panel->Window_width * 0.175, panel->Window_height * 0.155);
            RI_List_sourceCB_QListWidget->setFont(panel->font_D_LW1);

            hLayout->addWidget(RI_List_sourceCB_QListWidget);

            vLayout->addLayout(hLayout);
        }

        // [Energy Spectrum - Load File]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.010, panel->Window_width * 0.003, 0);
            hLayout->setSpacing(panel->Window_width * 0.003);
            hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

            labelConeBeamEnergyspectrumTitle = new QLabel;
            labelConeBeamEnergyspectrumTitle->setFixedSize(panel->Window_width * 0.095, panel->Window_height * 0.028);
            labelConeBeamEnergyspectrumTitle->setStyleSheet(DialogStyle::DATA_LABEL);
            labelConeBeamEnergyspectrumTitle->setFont(panel->font_D_LBL1);
            labelConeBeamEnergyspectrumTitle->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            labelConeBeamEnergyspectrumTitle->setText(QString::fromLocal8Bit("Energy spectrum file"));
            labelConeBeamEnergyspectrumTitle->hide();
             
            ConeBeamEnergyspectrumFileLoadButton = new QPushButton(QString::fromLocal8Bit("Load file"));
            panel->Buttons_FunctionPanelRight.append(ConeBeamEnergyspectrumFileLoadButton);
            ConeBeamEnergyspectrumFileLoadButton->setFixedSize(panel->Window_width * 0.067, panel->Window_height * 0.028);
            ConeBeamEnergyspectrumFileLoadButton->setStyleSheet(DialogStyle::MENU_BTN11);
            ConeBeamEnergyspectrumFileLoadButton->setFont(panel->font_D_BTN11);
            ConeBeamEnergyspectrumFileLoadButton->hide();

            connect(ConeBeamEnergyspectrumFileLoadButton, SIGNAL(clicked()), this, SLOT(slot_ConeBeamEnergyspectrumFileLoad_ButtonClicked())); 
            connect(ConeBeamEnergyspectrumFileLoadButton, SIGNAL(clicked()), panel, SLOT(slot_allButtonClicked()));

            hLayout->addWidget(labelConeBeamEnergyspectrumTitle);
            hLayout->addWidget(ConeBeamEnergyspectrumFileLoadButton);

            vLayout->addLayout(hLayout);
        }

        // [Energy Spectrum - File Name]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setContentsMargins(panel->Window_width * 0.003, 0, panel->Window_width * 0.003, 0);
            hLayout->setSpacing(panel->Window_width * 0.003);
            hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

            labelConeBeamEnergyspectrumFileName = new QLabel;
            labelConeBeamEnergyspectrumFileName->setFixedSize(panel->Window_width * 0.068, panel->Window_height * 0.028);
            labelConeBeamEnergyspectrumFileName->setStyleSheet(DialogStyle::DATA_LABEL);
            labelConeBeamEnergyspectrumFileName->setFont(panel->font_D_LBL1);
            labelConeBeamEnergyspectrumFileName->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            labelConeBeamEnergyspectrumFileName->setText(QString::fromLocal8Bit("File name"));
            labelConeBeamEnergyspectrumFileName->hide();

            sourceCB_EnergyspectrumFileNameText = new QTextEdit;
            sourceCB_EnergyspectrumFileNameText->setFixedSize(panel->Window_width * 0.104, panel->Window_height * 0.056);
            sourceCB_EnergyspectrumFileNameText->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            sourceCB_EnergyspectrumFileNameText->setStyleSheet("QTextEdit { background-color: rgb(255, 255, 255) }");
            sourceCB_EnergyspectrumFileNameText->setFont(panel->font_D_MAT);
            sourceCB_EnergyspectrumFileNameText->setPlaceholderText(QString::fromLocal8Bit("empty"));
            sourceCB_EnergyspectrumFileNameText->hide();

            hLayout->addWidget(labelConeBeamEnergyspectrumFileName);
            hLayout->addStretch();
            hLayout->addWidget(sourceCB_EnergyspectrumFileNameText);

            vLayout->addLayout(hLayout);
        }

        // [Activity / Intensity]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setContentsMargins(panel->Window_width * 0.003, 0, panel->Window_width * 0.003, 0);
            hLayout->setSpacing(panel->Window_width * 0.003);
            hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

            labelConeBeamEnergyspectrumIntensity = new QLabel;
            labelConeBeamEnergyspectrumIntensity->setFixedSize(panel->Window_width * 0.068, panel->Window_height * 0.028);
            labelConeBeamEnergyspectrumIntensity->setStyleSheet(DialogStyle::DATA_LABEL);
            labelConeBeamEnergyspectrumIntensity->setFont(panel->font_D_LBL1);
            labelConeBeamEnergyspectrumIntensity->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            labelConeBeamEnergyspectrumIntensity->setText(QString::fromLocal8Bit("Intensity"));
            labelConeBeamEnergyspectrumIntensity->hide();

            m_lineEditConeBeamIntensity = new QLineEdit;
            m_lineEditConeBeamIntensity->setFixedSize(panel->Window_width * 0.104, panel->Window_height * 0.028);
            m_lineEditConeBeamIntensity->setAlignment(Qt::AlignCenter);
            m_lineEditConeBeamIntensity->setStyleSheet(DialogStyle::LINEEDIT_NAME);
            m_lineEditConeBeamIntensity->setFont(panel->font_D_LE1);
            m_lineEditConeBeamIntensity->setPlaceholderText(QString::fromLocal8Bit("particles/s"));
            m_lineEditConeBeamIntensity->hide();

            hLayout->addWidget(labelConeBeamEnergyspectrumIntensity);
            hLayout->addStretch();
            hLayout->addWidget(m_lineEditConeBeamIntensity);

            vLayout->addLayout(hLayout);
        }       

        // [Energy Spectrum Add]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setContentsMargins(panel->Window_width * 0.003, 0, panel->Window_width * 0.003, 0);
            hLayout->setSpacing(panel->Window_width * 0.003);
            hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

            labelConeBeamEnergyspectrumList = new QLabel;
            labelConeBeamEnergyspectrumList->setFixedSize(panel->Window_width * 0.120, panel->Window_height * 0.028);
            labelConeBeamEnergyspectrumList->setStyleSheet(DialogStyle::DATA_LABEL);
            labelConeBeamEnergyspectrumList->setFont(panel->font_D_LBL1);
            labelConeBeamEnergyspectrumList->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
            labelConeBeamEnergyspectrumList->setText(QString::fromLocal8Bit("Entered energy spectrum list"));
            labelConeBeamEnergyspectrumList->hide();

            ConeBeamEnergyspectrumAddButton = new QPushButton(QString::fromLocal8Bit("Add"));
            panel->Buttons_FunctionPanelRight.append(ConeBeamEnergyspectrumAddButton);
            ConeBeamEnergyspectrumAddButton->setFixedSize(panel->Window_width * 0.042, panel->Window_height * 0.037);
            ConeBeamEnergyspectrumAddButton->setCheckable(false);
            ConeBeamEnergyspectrumAddButton->setStyleSheet(DialogStyle::MENU_BTN2);
            ConeBeamEnergyspectrumAddButton->setFont(panel->font_D_BTN2);
            ConeBeamEnergyspectrumAddButton->setChecked(false);
            ConeBeamEnergyspectrumAddButton->hide();

            connect(ConeBeamEnergyspectrumAddButton, SIGNAL(clicked()), this, SLOT(slot_ConeBeamEnergyspectrumAdd_ButtonClicked())); 
            connect(ConeBeamEnergyspectrumAddButton, SIGNAL(clicked()), panel, SLOT(slot_allButtonClicked()));

            hLayout->addWidget(labelConeBeamEnergyspectrumList);
            hLayout->addStretch();
            hLayout->addWidget(ConeBeamEnergyspectrumAddButton);

            vLayout->addLayout(hLayout);
        }

        // [Energy Spectrum List]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setContentsMargins(panel->Window_width * 0.003, 0, panel->Window_width * 0.003, 0);
            hLayout->setSpacing(panel->Window_width * 0.003);
            hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

            ES_List_sourceCB_QListWidget = new QListWidget;
            ES_List_sourceCB_QListWidget->setStyleSheet("QListWidget { border-radius: 15px; background-color: white; }");
            ES_List_sourceCB_QListWidget->setFixedSize(panel->Window_width * 0.175, panel->Window_height * 0.155);
            ES_List_sourceCB_QListWidget->setFont(panel->font_D_LW1);
            ES_List_sourceCB_QListWidget->hide();

            hLayout->addWidget(ES_List_sourceCB_QListWidget);

            vLayout->addLayout(hLayout);
        }
        subLayout->addLayout(vLayout);
    }
    
    this->setLayout(subLayout);
    return true;
}

// Visualization
void ConeBeamWidget::slot_ConeBeamSourceVisualization_ButtonClicked()
{
	if (m_ConeBeamSourceVisualizationButton->isChecked() == true)
	{
		b_IsSourceLocationVisualized_sourceCB = true;
		m_ConeBeamSourceVisualizationButton->setText("On");
		for (auto itr_actor : theApp.SourcePanelActor_Position)
		{
			theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(itr_actor);
		}


		theApp.m_pVTKWidget->renderWindow()->Render();
	}
	else if (m_ConeBeamSourceVisualizationButton->isChecked() == false)
	{
		b_IsSourceLocationVisualized_sourceCB = false;
		m_ConeBeamSourceVisualizationButton->setText("Off");
		for (auto itr_actor : theApp.SourcePanelActor_Position)
		{
			theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(itr_actor);
		}

		theApp.m_pVTKWidget->renderWindow()->Render();
	}
}
// Geometry & Interaction
void ConeBeamWidget::slot_ConeBeamUpdate_ButtonClicked()
{
	// 위치 
	double center[3] = { 0.0, };
	center[0] = m_lineEditConeBeamPointX->text().toFloat();
	center[1] = m_lineEditConeBeamPointY->text().toFloat();
	center[2] = m_lineEditConeBeamPointZ->text().toFloat();

	theApp.sourceObjects->Selecting3DShpere(center);
}
void ConeBeamWidget::slot_ConeBeam_Pick3D_ButtonClicked()
{
	if (theApp.st_Pick3D_sourceCB == true)
	{
		theApp.st_Pick3D_sourceCB = false;
		pickConeBeam3DButton->setChecked(false);

	}
	else
	{
		theApp.st_Pick3D_sourceCB = true;
		pickConeBeam3DButton->setChecked(true);
	}
}
void ConeBeamWidget::slot_CBreset_ButtonClicked()
{

}
// Type Selection
void ConeBeamWidget::slot_ConeBeamRadionuclide_ButtonClicked()
{
	ConeBeamRadionuclideButton->setChecked(true);
	ConeBeamEnergyspectrumButton->setChecked(false);

	labelConeBeamRadionuclideTitle->show();
	m_lineEditConeBeamRadionuclide->show();
	labelConeBeamRadionuclideActivity->show();
	m_lineEditConeBeamActivity->show();
	labelConeBeamRadionuclideList->show();
	ConeBeamRadionuclideAddButton->show();
	RI_List_sourceCB_QListWidget->show();

	labelConeBeamEnergyspectrumTitle->hide();
	ConeBeamEnergyspectrumFileLoadButton->hide();
	labelConeBeamEnergyspectrumIntensity->hide();
	m_lineEditConeBeamIntensity->hide();
	labelConeBeamEnergyspectrumList->hide();
	labelConeBeamEnergyspectrumFileName->hide();
	sourceCB_EnergyspectrumFileNameText->hide();
	ConeBeamEnergyspectrumAddButton->hide();
	ES_List_sourceCB_QListWidget->hide();
}
void ConeBeamWidget::slot_ConeBeamEnergyspectrum_ButtonClicked()
{
	ConeBeamRadionuclideButton->setChecked(false);
	ConeBeamEnergyspectrumButton->setChecked(true);

	labelConeBeamRadionuclideTitle->hide();
	m_lineEditConeBeamRadionuclide->hide();
	labelConeBeamRadionuclideActivity->hide();
	m_lineEditConeBeamActivity->hide();
	labelConeBeamRadionuclideList->hide();
	ConeBeamRadionuclideAddButton->hide();
	RI_List_sourceCB_QListWidget->hide();

	labelConeBeamEnergyspectrumTitle->show();
	ConeBeamEnergyspectrumFileLoadButton->show();
	labelConeBeamEnergyspectrumIntensity->show();
	m_lineEditConeBeamIntensity->show();
	labelConeBeamEnergyspectrumList->show();
	ConeBeamEnergyspectrumAddButton->show();
	labelConeBeamEnergyspectrumFileName->show();
	sourceCB_EnergyspectrumFileNameText->show();
	ES_List_sourceCB_QListWidget->show();
}
// Radionuclide Operations
void ConeBeamWidget::slot_ConeBeamRadionuclideAdd_ButtonClicked()
{	
	QListWidgetItem* item = new QListWidgetItem(RI_List_sourceCB_QListWidget);
	QWidget* widget = new QWidget;
	QHBoxLayout* hLayout = new QHBoxLayout;
	widget->setStyleSheet("background-color: white;");

	QString arg1 = m_lineEditConeBeamRadionuclide->text();
	QLabel* label1 = new QLabel(arg1);
	label1->setFixedWidth(panel->Window_width * 0.025);
	label1->setFont(panel->font_D_LW2); // Set font for the label

	QString arg2 = QString::number(m_lineEditConeBeamActivity->text().toDouble(), 'e', 3);
	QLabel* label2 = new QLabel(arg2);
	label2->setFixedWidth(panel->Window_width * 0.025);
	label2->setFont(panel->font_D_LW2); // Set font for the label

	QString arg3 = "";
	QLabel* labelPosX = new QLabel(arg3);
	labelPosX->setFixedWidth(panel->Window_width * 0.025);
	labelPosX->setFont(panel->font_D_LW2); // Set font for the label

	QString arg4 = "";
	QLabel* labelPosY = new QLabel(arg4);
	labelPosY->setFixedWidth(panel->Window_width * 0.025);
	labelPosY->setFont(panel->font_D_LW2); // Set font for the label

	QString arg5 = "";
	QLabel* labelPosZ = new QLabel(arg5);
	labelPosZ->setFixedWidth(panel->Window_width * 0.025);
	labelPosZ->setFont(panel->font_D_LW2); // Set font for the label

	m_sourceCB_listDeleteButton.push_back(new QPushButton());
	if (RIList_sourceCB_MakingIndex >= m_sourceCB_listDeleteButton.size()) theApp.SetMessageBox("m_sourceCB_listDeleteButton Vector index error");
	m_sourceCB_listDeleteButton[RIList_sourceCB_MakingIndex]->setCheckable(false);
	m_sourceCB_listDeleteButton[RIList_sourceCB_MakingIndex]->setStyleSheet(DialogStyle::MENU_BTN13);
	m_sourceCB_listDeleteButton[RIList_sourceCB_MakingIndex]->setFont(panel->font_D_BTN13);
	m_sourceCB_listDeleteButton[RIList_sourceCB_MakingIndex]->setFixedSize(panel->Window_width * 0.008, panel->Window_height * 0.012);
	m_sourceCB_listDeleteButton[RIList_sourceCB_MakingIndex]->setIcon(QIcon("./data/image/delete.png"));
	m_sourceCB_listDeleteButton[RIList_sourceCB_MakingIndex]->setObjectName(QString::number(RIList_sourceCB_MakingIndex));

	m_sourceCB_listInfoButton.push_back(new QPushButton());
	if (RIList_sourceCB_MakingIndex >= m_sourceCB_listInfoButton.size()) theApp.SetMessageBox("m_sourceCB_listInfoButton Vector index error");
	m_sourceCB_listInfoButton[RIList_sourceCB_MakingIndex]->setCheckable(false);
	m_sourceCB_listInfoButton[RIList_sourceCB_MakingIndex]->setStyleSheet(DialogStyle::MENU_BTN13);
	m_sourceCB_listInfoButton[RIList_sourceCB_MakingIndex]->setFont(panel->font_D_BTN13);
	m_sourceCB_listInfoButton[RIList_sourceCB_MakingIndex]->setFixedSize(panel->Window_width * 0.008, panel->Window_height * 0.012);
	m_sourceCB_listInfoButton[RIList_sourceCB_MakingIndex]->setIcon(QIcon("./data/image/info.png"));
	m_sourceCB_listInfoButton[RIList_sourceCB_MakingIndex]->setObjectName(QString::number(RIList_sourceCB_MakingIndex));

	hLayout->addWidget(label1);
	hLayout->addWidget(label2);
	hLayout->addWidget(labelPosX);
	hLayout->addWidget(labelPosY);
	hLayout->addWidget(labelPosZ);
	hLayout->addWidget(m_sourceCB_listDeleteButton[RIList_sourceCB_MakingIndex]);
	hLayout->addWidget(m_sourceCB_listInfoButton[RIList_sourceCB_MakingIndex]);
	widget->setLayout(hLayout);

	RI_List_sourceCB_QListWidget->setItemWidget(item, widget);
	item->setSizeHint(widget->sizeHint());

	// Connect signals of the new buttons
	connect(m_sourceCB_listDeleteButton[RIList_sourceCB_MakingIndex], SIGNAL(clicked()), this, SLOT(slot_RIList_Delete_sourceEP_ButtonClicked())); connect(m_sourceCB_listDeleteButton[RIList_sourceCB_MakingIndex], SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
	connect(m_sourceCB_listInfoButton[RIList_sourceCB_MakingIndex], SIGNAL(clicked()), this, SLOT(slot_RIList_Info_sourceEP_ButtonClicked())); connect(m_sourceCB_listInfoButton[RIList_sourceCB_MakingIndex], SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));

	// Generate actor only when first set	
	double center[3] = { 0.0, };
	center[0] = m_lineEditConeBeamPointX->text().toFloat();
	center[1] = m_lineEditConeBeamPointY->text().toFloat();
	center[2] = m_lineEditConeBeamPointZ->text().toFloat();

	theApp.sourceObjects->GenerateSourceActor_sourceCB(center);
	theApp.sourceObjects->GenerateSourceDirectionActor_sourceCB();

	// Set Global Variables
	std::map<int, QString> tmp_map;
	tmp_map[0] = "True";
	tmp_map[1] = m_lineEditConeBeamPointX->text();
	tmp_map[2] = m_lineEditConeBeamPointY->text();
	tmp_map[3] = m_lineEditConeBeamPointZ->text();
	tmp_map[4] = m_lineEditConeBeamRadionuclide->text();
	tmp_map[5] = m_lineEditConeBeamActivity->text();
	sourceCB_info.push_back(tmp_map);
	RIList_sourceCB_MakingIndex++;

	theApp.SetMessageBox_RadionuclideWarning("The radionuclide data provided in this code are based on the data provided in the ICRP 107 publication.\nTHIS DATA DO NOT INCLUDE ANY DAUGHTER NUCLIDES!!!");

}
void ConeBeamWidget::slot_ConeBeamRadionuclideDelete_ButtonClicked()
{

}
void ConeBeamWidget::slot_ConeBeamRadionuclideInfo_ButtonClicked()
{

}
// Energy Spectrum Operations
void ConeBeamWidget::slot_ConeBeamEnergyspectrumFileLoad_ButtonClicked()
{	
	QString dir = QFileDialog::getOpenFileName(
		this,
		tr("Select a file to open"),
		"",
		tr("Energy Spectrum Files (*.*)"));

	if (!dir.isEmpty())
	{
		int tmp_size = dir.size() - 19;
		QString FinalDir = dir.right(tmp_size);
		sourceCB_EnergyspectrumFileNameText->setPlaceholderText(dir);
		sourceCB_EnergyspectrumFileNameText->setReadOnly(true);
	}
}
void ConeBeamWidget::slot_ConeBeamEnergyspectrumAdd_ButtonClicked()
{
	EnergyspectrumListTotalCount_CB++;
	QListWidgetItem* item = new QListWidgetItem(ES_List_sourceCB_QListWidget);
	QWidget* widget = new QWidget;
	QHBoxLayout* hLayout = new QHBoxLayout;
	widget->setStyleSheet("background-color: white;");
	
	QString arg1 = "File";
	QLabel* label1 = new QLabel(arg1);
	label1->setFixedWidth(panel->Window_width * 0.025);
	label1->setFont(panel->font_D_LW2); // Set font for the label

	QString arg2 = QString::number(m_lineEditConeBeamIntensity->text().toDouble(), 'e', 3);
	QLabel* label2 = new QLabel(arg2);
	label2->setFixedWidth(panel->Window_width * 0.025);
	label2->setFont(panel->font_D_LW2); // Set font for the label

	QString arg3 = m_lineEditConeBeamPointX->text().left(6);
	if (arg3 == "") arg3 = "0";
	QLabel* labelPosX = new QLabel(arg3);
	labelPosX->setFixedWidth(panel->Window_width * 0.025);
	labelPosX->setFont(panel->font_D_LW2); // Set font for the label

	QString arg4 = m_lineEditConeBeamPointY->text().left(6);
	if (arg4 == "") arg4 = "0";
	QLabel* labelPosY = new QLabel(arg4);
	labelPosY->setFixedWidth(panel->Window_width * 0.025);
	labelPosY->setFont(panel->font_D_LW2); // Set font for the label

	QString arg5 = m_lineEditConeBeamPointZ->text().left(6);
	if (arg5 == "") arg5 = "0";
	QLabel* labelPosZ = new QLabel(arg5);
	labelPosZ->setFixedWidth(panel->Window_width * 0.025);
	labelPosZ->setFont(panel->font_D_LW2); // Set font for the label

	QPushButton* deleteButton = new QPushButton();
	deleteButton->setCheckable(false);
	deleteButton->setStyleSheet(DialogStyle::MENU_BTN13);
	deleteButton->setFont(panel->font_D_BTN13);
	deleteButton->setFixedSize(panel->Window_width * 0.008, panel->Window_height * 0.012);
	deleteButton->setIcon(QIcon("./data/image/delete.png"));
	QString deleteButton_tmp = QString::number(EnergyspectrumListTotalCount_CB - 1) + "_Delete";
	deleteButton->setObjectName(deleteButton_tmp);

	QPushButton* infoButton = new QPushButton();
	infoButton->setCheckable(false);
	infoButton->setStyleSheet(DialogStyle::MENU_BTN13);
	infoButton->setFont(panel->font_D_BTN13);
	infoButton->setFixedSize(panel->Window_width * 0.008, panel->Window_height * 0.012);
	infoButton->setIcon(QIcon("./data/image/info.png"));
	QString infoButton_tmp = QString::number(EnergyspectrumListTotalCount_CB - 1) + "_Info";
	infoButton->setObjectName(infoButton_tmp);

	hLayout->addWidget(label1);
	hLayout->addWidget(label2);
	hLayout->addWidget(labelPosX);
	hLayout->addWidget(labelPosY);
	hLayout->addWidget(labelPosZ);
	hLayout->addWidget(deleteButton);
	hLayout->addWidget(infoButton);
	widget->setLayout(hLayout);

	ES_List_sourceCB_QListWidget->setItemWidget(item, widget);
	item->setSizeHint(widget->sizeHint());

	// Connect signals of the new buttons
	connect(deleteButton, SIGNAL(clicked()), this, SLOT(slot_RIList_Delete_sourceEP_ButtonClicked())); connect(deleteButton, SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
	connect(infoButton, SIGNAL(clicked()), this, SLOT(slot_ConeBeamEnergyspectrumInfo_ButtonClicked())); connect(infoButton, SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
}
void ConeBeamWidget::slot_ConeBeamEnergyspectrumDelete_ButtonClicked()
{

}
void ConeBeamWidget::slot_ConeBeamEnergyspectrumInfo_ButtonClicked()
{
	// Button의 Index 찾기
	QPushButton* buttonSender = qobject_cast<QPushButton*>(sender());
	QString buttonText = buttonSender->objectName();

	QStringList list_button = buttonText.split("_");

	// The first part is the ID
	int EnergyspectrumIndex = list_button[0].toInt();

	QListWidgetItem* item = ES_List_sourceCB_QListWidget->item(EnergyspectrumIndex);
	if (!item) {
		// Handle error: item not found
		return;
	}

	QWidget* widget = ES_List_sourceCB_QListWidget->itemWidget(item);
	QHBoxLayout* hLayout = qobject_cast<QHBoxLayout*>(widget->layout());
	if (!hLayout) {
		// Handle error: layout not found
		return;
	}

	// Assuming the order of labels in the layout is known
	QLabel* labelFile = qobject_cast<QLabel*>(hLayout->itemAt(0)->widget());
	QLabel* labelIntensity = qobject_cast<QLabel*>(hLayout->itemAt(1)->widget());
	QLabel* labelX = qobject_cast<QLabel*>(hLayout->itemAt(2)->widget());
	QLabel* labelY = qobject_cast<QLabel*>(hLayout->itemAt(3)->widget());
	QLabel* labelZ = qobject_cast<QLabel*>(hLayout->itemAt(4)->widget());

	// Create a new dialog
	QDialog* infoDialog = new QDialog(this);
	infoDialog->setStyleSheet("background-color: white;");
	infoDialog->resize(panel->Window_width * 0.3, panel->Window_height * 0.55);
	infoDialog->setWindowTitle("Energyspectrum Information");

	// Create a layout and a label for the dialog
	QVBoxLayout* layout = new QVBoxLayout(infoDialog);
	// Create a QTextEdit for the dialog
	QTextEdit* textEdit = new QTextEdit(infoDialog);
	textEdit->setReadOnly(true); // Make it read-only if editing is not required

	// Create a QTextCursor for inserting formatted text
	QTextCursor cursor = textEdit->textCursor();

	// Define different text formats
	QTextCharFormat boldFormat;
	boldFormat.setFontFamily("Courier New"); // This is monospaced font
	boldFormat.setFontWeight(QFont::Bold);
	boldFormat.setFontPointSize(14);

	QTextCharFormat boldMediumFormat;
	boldMediumFormat.setFontFamily("Courier New");
	boldMediumFormat.setFontWeight(QFont::Bold);
	boldMediumFormat.setFontPointSize(12);

	QTextCharFormat boldSmallFormat;
	boldSmallFormat.setFontFamily("Courier New");
	boldSmallFormat.setFontWeight(QFont::Bold);
	boldSmallFormat.setFontPointSize(10);

	QTextCharFormat normalFormat;
	normalFormat.setFontFamily("Courier New");
	normalFormat.setFontPointSize(10);

	QTextCharFormat smallFormat;
	smallFormat.setFontFamily("Courier New");
	smallFormat.setFontPointSize(8);

	QTextCharFormat verySmallFormat;
	verySmallFormat.setFontFamily("Courier New");
	verySmallFormat.setFontPointSize(6);

	// Insert formatted text into QTextEdit
	cursor.insertText("Energyspectrum Information\n", boldFormat);
	cursor.insertText("Radionuclide: " + sourceCB_EnergyspectrumFileNameText->placeholderText() + "\n", normalFormat);
	cursor.insertText("Activity (Bq): " + labelIntensity->text() + "\n", normalFormat);
	cursor.insertText("PosX (cm): " + labelX->text() + "\n", normalFormat);
	cursor.insertText("PosY (cm): " + labelY->text() + "\n", normalFormat);
	cursor.insertText("PosZ (cm): " + labelZ->text() + "\n", normalFormat);

	cursor.insertText("\nRadiation Spectrum of the Entered Energy Spectrum\n", boldFormat);
	cursor.insertText("Radiation      Fraction       Energy (MeV)\n", boldSmallFormat);

	QString ESFileName = (sourceCB_EnergyspectrumFileNameText->placeholderText());
	QFile ESFile(ESFileName);
	if (!ESFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		// Handle error: Unable to open file
		return;
	}

	QTextStream ESFileInput(&ESFile);
	QString ESFileLine;

	// Skip the first line
	if (!ESFileInput.atEnd()) {
		ESFileLine = ESFileInput.readLine();
	}

	while (!ESFileInput.atEnd()) {
		ESFileLine = ESFileInput.readLine();
		QStringList columns = ESFileLine.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);

		if (columns.size() >= 1) {
			QString radiation = columns[0];
			QString fraction = columns[1];
			QString energy = columns[2];
			QString padding1(15 - radiation.length(), ' ');
			QString padding2(15 - fraction.length(), ' ');
			cursor.insertText(radiation + padding1 + fraction + padding2 + energy + "\n", normalFormat);
		}
	}
	ESFile.close();

	cursor.movePosition(QTextCursor::Start);
	textEdit->setTextCursor(cursor);

	// Add the QTextEdit to the layout
	layout->addWidget(textEdit);

	// Set the layout to the dialog
	infoDialog->setLayout(layout);

	// Show the dialog
	infoDialog->exec(); // Use exec() for a modal dialog or show() for a non-modal dialog
}

///////////////////////////////////////////////////////////////////////////
// RoomContaminationWidget Implementation
///////////////////////////////////////////////////////////////////////////

RoomContaminationWidget::RoomContaminationWidget(QWidget* parent)
    : ParentT(parent)
{
}

RoomContaminationWidget::~RoomContaminationWidget()
{
}

bool RoomContaminationWidget::initialize()
{
    if (!ParentT::initialize()) return false;

    QVBoxLayout* subLayout = new QVBoxLayout;
    subLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.005, panel->Window_width * 0.003, panel->Window_height * 0.005);
    subLayout->setSpacing(panel->Window_width * 0.003);
    subLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    {
        QVBoxLayout* vLayout = new QVBoxLayout;
        vLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.005, panel->Window_width * 0.003, panel->Window_height * 0.005);
        vLayout->setSpacing(panel->Window_width * 0.003);
        vLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

        // [Visualization]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setSpacing(panel->Window_width * 0.005);
            hLayout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

            QLabel* labelTitle = new QLabel; 
            labelTitle->setFixedSize(panel->Window_width * 0.104, panel->Window_height * 0.028);
            labelTitle->setStyleSheet(DialogStyle::DATA_LABEL);
            labelTitle->setFont(panel->font_D_LBL1);
            labelTitle->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            labelTitle->setText(QString::fromLocal8Bit("Visualization"));

            m_RoomContaminationSourceVisualizationButton = new QPushButton(QString::fromLocal8Bit("On"));
            panel->Buttons_FunctionPanelRight.append(m_RoomContaminationSourceVisualizationButton);
            m_RoomContaminationSourceVisualizationButton->setFixedSize(panel->Window_width * 0.05, panel->Window_height * 0.028);
            m_RoomContaminationSourceVisualizationButton->setCheckable(true);
            m_RoomContaminationSourceVisualizationButton->setChecked(true);
            m_RoomContaminationSourceVisualizationButton->setStyleSheet(DialogStyle::MENU_BTN3);
            m_RoomContaminationSourceVisualizationButton->setFont(panel->font_D_BTN3);
            
            connect(m_RoomContaminationSourceVisualizationButton, SIGNAL(clicked()), this, SLOT(slot_RoomContaminationSourceVisualization_ButtonClicked())); 
            connect(m_RoomContaminationSourceVisualizationButton, SIGNAL(clicked()), panel, SLOT(slot_allButtonClicked()));

            hLayout->addWidget(labelTitle);
            hLayout->addStretch();
            hLayout->addWidget(m_RoomContaminationSourceVisualizationButton);

            vLayout->addLayout(hLayout);
        }

        // [Box Dimensions (Half Length)]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.003, panel->Window_width * 0.003, panel->Window_height * 0.003);
            hLayout->setSpacing(panel->Window_width * 0.003);
            hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

            label_sourceRC_BoxLength = new QLabel;
            label_sourceRC_BoxLength->setFixedSize(panel->Window_width * 0.065, panel->Window_height * 0.028);
            label_sourceRC_BoxLength->setStyleSheet(DialogStyle::DATA_LABEL);
            label_sourceRC_BoxLength->setFont(panel->font_D_LBL1);
            label_sourceRC_BoxLength->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            label_sourceRC_BoxLength->setText(QString::fromLocal8Bit("Half len. XYZ:"));

            Box_HalfLengthX_sourceRC_QLineEdit = new QLineEdit;
            Box_HalfLengthX_sourceRC_QLineEdit->setFixedSize(panel->Window_width * 0.035, panel->Window_height * 0.028);
            Box_HalfLengthX_sourceRC_QLineEdit->setAlignment(Qt::AlignCenter);
            Box_HalfLengthX_sourceRC_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME4);
            Box_HalfLengthX_sourceRC_QLineEdit->setFont(panel->font_D_LE4);
            Box_HalfLengthX_sourceRC_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(cm)"));

            Box_HalfLengthY_sourceRC_QLineEdit = new QLineEdit;
            Box_HalfLengthY_sourceRC_QLineEdit->setFixedSize(panel->Window_width * 0.035, panel->Window_height * 0.028);
            Box_HalfLengthY_sourceRC_QLineEdit->setAlignment(Qt::AlignCenter);
            Box_HalfLengthY_sourceRC_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME4);
            Box_HalfLengthY_sourceRC_QLineEdit->setFont(panel->font_D_LE4);
            Box_HalfLengthY_sourceRC_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(cm)"));

            Box_HalfLengthZ_sourceRC_QLineEdit = new QLineEdit;
            Box_HalfLengthZ_sourceRC_QLineEdit->setFixedSize(panel->Window_width * 0.035, panel->Window_height * 0.028);
            Box_HalfLengthZ_sourceRC_QLineEdit->setAlignment(Qt::AlignCenter);
            Box_HalfLengthZ_sourceRC_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME4);
            Box_HalfLengthZ_sourceRC_QLineEdit->setFont(panel->font_D_LE4);
            Box_HalfLengthZ_sourceRC_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(cm)"));

            hLayout->addWidget(label_sourceRC_BoxLength);
            hLayout->addWidget(Box_HalfLengthX_sourceRC_QLineEdit);
            hLayout->addWidget(Box_HalfLengthY_sourceRC_QLineEdit);
            hLayout->addWidget(Box_HalfLengthZ_sourceRC_QLineEdit);

            vLayout->addLayout(hLayout);
        }

        // [Box Center]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.003, panel->Window_width * 0.003, panel->Window_height * 0.003);
            hLayout->setSpacing(panel->Window_width * 0.003);
            hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

            label_sourceRC_BoxCenter = new QLabel;
            label_sourceRC_BoxCenter->setFixedSize(panel->Window_width * 0.065, panel->Window_height * 0.028);
            label_sourceRC_BoxCenter->setStyleSheet(DialogStyle::DATA_LABEL);
            label_sourceRC_BoxCenter->setFont(panel->font_D_LBL1);
            label_sourceRC_BoxCenter->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            label_sourceRC_BoxCenter->setText(QString::fromLocal8Bit("Center XYZ:"));

            Box_CenterX_sourceRC_QLineEdit = new QLineEdit;
            Box_CenterX_sourceRC_QLineEdit->setFixedSize(panel->Window_width * 0.035, panel->Window_height * 0.028);
            Box_CenterX_sourceRC_QLineEdit->setAlignment(Qt::AlignCenter);
            Box_CenterX_sourceRC_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME4);
            Box_CenterX_sourceRC_QLineEdit->setFont(panel->font_D_LE4);
            Box_CenterX_sourceRC_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(cm)"));

            Box_CenterY_sourceRC_QLineEdit = new QLineEdit;
            Box_CenterY_sourceRC_QLineEdit->setFixedSize(panel->Window_width * 0.035, panel->Window_height * 0.028);
            Box_CenterY_sourceRC_QLineEdit->setAlignment(Qt::AlignCenter);
            Box_CenterY_sourceRC_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME4);
            Box_CenterY_sourceRC_QLineEdit->setFont(panel->font_D_LE4);
            Box_CenterY_sourceRC_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(cm)"));

            Box_CenterZ_sourceRC_QLineEdit = new QLineEdit;
            Box_CenterZ_sourceRC_QLineEdit->setFixedSize(panel->Window_width * 0.035, panel->Window_height * 0.028);
            Box_CenterZ_sourceRC_QLineEdit->setAlignment(Qt::AlignCenter);
            Box_CenterZ_sourceRC_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME4);
            Box_CenterZ_sourceRC_QLineEdit->setFont(panel->font_D_LE4);
            Box_CenterZ_sourceRC_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("(cm)"));

            hLayout->addWidget(label_sourceRC_BoxCenter);
            hLayout->addWidget(Box_CenterX_sourceRC_QLineEdit);
            hLayout->addWidget(Box_CenterY_sourceRC_QLineEdit);
            hLayout->addWidget(Box_CenterZ_sourceRC_QLineEdit);

            vLayout->addLayout(hLayout);
        }

        // [Update Button]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.003, panel->Window_width * 0.003, panel->Window_height * 0.005);
            hLayout->setSpacing(panel->Window_width * 0.003);
            hLayout->setAlignment(Qt::AlignRight | Qt::AlignTop);
            
            UpdateRCposButton = new QPushButton(QString::fromLocal8Bit("Update"));
            panel->Buttons_FunctionPanelRight.append(UpdateRCposButton);
            UpdateRCposButton->setFixedSize(panel->Window_width * 0.050, panel->Window_height * 0.036);
            UpdateRCposButton->setStyleSheet(DialogStyle::MENU_BTN2);
            UpdateRCposButton->setFont(panel->font_D_BTN2);

            connect(UpdateRCposButton, SIGNAL(clicked()), this, SLOT(slot_UpdateRCpos_ButtonClicked())); 
            connect(UpdateRCposButton, SIGNAL(clicked()), panel, SLOT(slot_allButtonClicked()));

            hLayout->addStretch();
            hLayout->addWidget(UpdateRCposButton);

            vLayout->addLayout(hLayout);
        }

        // [RI or ES Selection]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;         
            hLayout->setSpacing(panel->Window_width * 0.003);
            hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
            hLayout->setContentsMargins(panel->Window_width * 0.003, 0, panel->Window_width * 0.003, 0);

            RI_Select_sourceRC_QRadioButton = new QRadioButton(QString::fromLocal8Bit("Radionuclide"));
            RI_Select_sourceRC_QRadioButton->setStyleSheet(DialogStyle::RADIOBUTTON_WIDGET);
            RI_Select_sourceRC_QRadioButton->setFont(panel->font_D_RB2);
            RI_Select_sourceRC_QRadioButton->setFixedSize(panel->Window_width * 0.081, panel->Window_height * 0.028);
            RI_Select_sourceRC_QRadioButton->setChecked(true);

            ES_Select_sourceRC_QRadioButton = new QRadioButton(QString::fromLocal8Bit("Energy spectrum"));
            ES_Select_sourceRC_QRadioButton->setStyleSheet(DialogStyle::RADIOBUTTON_WIDGET);
            ES_Select_sourceRC_QRadioButton->setFont(panel->font_D_RB2);
            ES_Select_sourceRC_QRadioButton->setFixedSize(panel->Window_width * 0.081, panel->Window_height * 0.028);
            ES_Select_sourceRC_QRadioButton->setChecked(false);

            connect(RI_Select_sourceRC_QRadioButton, SIGNAL(clicked()), this, SLOT(slot_RI_Select_sourceRC_RadioButtonClicked())); 
            connect(RI_Select_sourceRC_QRadioButton, SIGNAL(clicked()), panel, SLOT(slot_allButtonClicked()));
            
            connect(ES_Select_sourceRC_QRadioButton, SIGNAL(clicked()), this, SLOT(slot_ES_Select_sourceRC_RadioButtonClicked())); 
            connect(ES_Select_sourceRC_QRadioButton, SIGNAL(clicked()), panel, SLOT(slot_allButtonClicked()));

            hLayout->addWidget(RI_Select_sourceRC_QRadioButton);
            hLayout->addWidget(ES_Select_sourceRC_QRadioButton);

            vLayout->addLayout(hLayout);
        }

        // [Radionuclide Input]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setContentsMargins(panel->Window_width * 0.003, 0, panel->Window_width * 0.003, 0);
            hLayout->setSpacing(panel->Window_width * 0.003);
            hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

            QLabel* label0 = new QLabel;
            label0->setFixedSize(panel->Window_width * 0.068, panel->Window_height * 0.028);
            label0->setStyleSheet(DialogStyle::DATA_LABEL);
            label0->setFont(panel->font_D_LBL1);
            label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            label0->setText(QString::fromLocal8Bit("Radionuclide"));

            RI_sourceRC_QLineEdit = new QLineEdit;
            RI_sourceRC_QLineEdit->setFixedSize(panel->Window_width * 0.104, panel->Window_height * 0.028);
            RI_sourceRC_QLineEdit->setAlignment(Qt::AlignCenter);
            RI_sourceRC_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
            RI_sourceRC_QLineEdit->setFont(panel->font_D_LE1);
            RI_sourceRC_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("Enter radionuclide"));

            QCompleter *completer = new QCompleter(panel->RIsourceList, this);
            completer->setCaseSensitivity(Qt::CaseInsensitive);
            RI_sourceRC_QLineEdit->setCompleter(completer);

            hLayout->addWidget(label0);
            hLayout->addStretch();
            hLayout->addWidget(RI_sourceRC_QLineEdit);

            vLayout->addLayout(hLayout);
        }

        // [Activity Input]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setContentsMargins(panel->Window_width * 0.003, 0, panel->Window_width * 0.003, 0);
            hLayout->setSpacing(panel->Window_width * 0.003);
            hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

            QLabel* label0 = new QLabel;
            label0->setFixedSize(panel->Window_width * 0.068, panel->Window_height * 0.028);
            label0->setStyleSheet(DialogStyle::DATA_LABEL);
            label0->setFont(panel->font_D_LBL1);
            label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            label0->setText(QString::fromLocal8Bit("Activity"));

            Activity_sourceRC_QLineEdit = new QLineEdit;
            Activity_sourceRC_QLineEdit->setFixedSize(panel->Window_width * 0.104, panel->Window_height * 0.028);
            Activity_sourceRC_QLineEdit->setAlignment(Qt::AlignCenter);
            Activity_sourceRC_QLineEdit->setStyleSheet(DialogStyle::LINEEDIT_NAME);
            Activity_sourceRC_QLineEdit->setFont(panel->font_D_LE1);
            Activity_sourceRC_QLineEdit->setPlaceholderText(QString::fromLocal8Bit("Bq/cm3"));

            hLayout->addWidget(label0);
            hLayout->addStretch();
            hLayout->addWidget(Activity_sourceRC_QLineEdit);

            vLayout->addLayout(hLayout);
        }

        // [Radionuclide Add Button]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setContentsMargins(panel->Window_width * 0.003, 0, panel->Window_width * 0.003, 0);
            hLayout->setSpacing(panel->Window_width * 0.003);
            hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

            RadionuclideList_sourceRC_QLabel = new QLabel;
            RadionuclideList_sourceRC_QLabel->setFixedSize(panel->Window_width * 0.088, panel->Window_height * 0.028);
            RadionuclideList_sourceRC_QLabel->setStyleSheet(DialogStyle::DATA_LABEL);
            RadionuclideList_sourceRC_QLabel->setFont(panel->font_D_LBL1);
            RadionuclideList_sourceRC_QLabel->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
            RadionuclideList_sourceRC_QLabel->setText(QString::fromLocal8Bit("Entered sources"));

            RIList_Add_sourceRC_QButton = new QPushButton(QString::fromLocal8Bit("Add"));
            panel->Buttons_FunctionPanelRight.append(RIList_Add_sourceRC_QButton);
            RIList_Add_sourceRC_QButton->setFixedSize(panel->Window_width * 0.042, panel->Window_height * 0.037);
            RIList_Add_sourceRC_QButton->setCheckable(false);
            RIList_Add_sourceRC_QButton->setStyleSheet(DialogStyle::MENU_BTN2);
            RIList_Add_sourceRC_QButton->setFont(panel->font_D_BTN2);

            RIList_Add_sourceRC_QButton->setChecked(false);

            connect(RIList_Add_sourceRC_QButton, SIGNAL(clicked()), this, SLOT(slot_RIList_Add_sourceRC_ButtonClicked())); 
            connect(RIList_Add_sourceRC_QButton, SIGNAL(clicked()), panel, SLOT(slot_allButtonClicked()));

            hLayout->addWidget(RadionuclideList_sourceRC_QLabel);
            hLayout->addStretch();
            hLayout->addWidget(RIList_Add_sourceRC_QButton);

            vLayout->addLayout(hLayout);
        }

        // [Radionuclide List]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setContentsMargins(panel->Window_width * 0.003, 0, panel->Window_width * 0.003, 0);
            hLayout->setSpacing(panel->Window_width * 0.003);
            hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

            RI_List_sourceRC_QListWidget = new QListWidget;
            RI_List_sourceRC_QListWidget->setStyleSheet("QListWidget { border-radius: 15px; background-color: white; }");
            RI_List_sourceRC_QListWidget->setFixedSize(panel->Window_width * 0.175, panel->Window_height * 0.155);
            RI_List_sourceRC_QListWidget->setFont(panel->font_D_LW1);

            hLayout->addWidget(RI_List_sourceRC_QListWidget);

            vLayout->addLayout(hLayout);
        }

        subLayout->addLayout(vLayout);
    }
    
    this->setLayout(subLayout);
    return true;
}

// Visualization
void RoomContaminationWidget::slot_RoomContaminationSourceVisualization_ButtonClicked()
{
	if (m_RoomContaminationSourceVisualizationButton->isChecked()) // On으로 켤 때
	{
		m_RoomContaminationSourceVisualizationButton->setText("On");
		for (auto itr_sourceActors_location : theApp.SourcePanelActor_Position)
		{
			theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(itr_sourceActors_location);
		}
	}
	else // Off로 끌 때
	{
		m_RoomContaminationSourceVisualizationButton->setText("Off");
		for (auto itr_sourceActors_location : theApp.SourcePanelActor_Position)
		{
			theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(itr_sourceActors_location);
		}
	}
	theApp.m_pVTKWidget->renderWindow()->Render();
}

// Geometry Update
void RoomContaminationWidget::slot_UpdateRCpos_ButtonClicked()
{
	// Parameter 불러오기
	double XHalfLength = Box_HalfLengthX_sourceRC_QLineEdit->text().toDouble();
	double YHalfLength = Box_HalfLengthY_sourceRC_QLineEdit->text().toDouble();
	double ZHalfLength = Box_HalfLengthZ_sourceRC_QLineEdit->text().toDouble();
	double XCenter = Box_CenterX_sourceRC_QLineEdit->text().toDouble();
	double YCenter = Box_CenterY_sourceRC_QLineEdit->text().toDouble();
	double ZCenter = Box_CenterZ_sourceRC_QLineEdit->text().toDouble();

	theApp.sourceObjects->GenerateSourceActor_sourceRC(XHalfLength, YHalfLength, ZHalfLength, XCenter, YCenter, ZCenter);
	// // Parameter 불러오기 ENDS
	// auto property1 = Util::GetProperty();
	// double color1[3] = { 1., 0., 0. };
	// property1->SetColor(color1);
	// property1->SetInterpolationToPhong();
	// property1->SetOpacity(0.2);

	// // Create the outer cube
	// vtkSmartPointer<vtkCubeSource> outerCube = vtkSmartPointer<vtkCubeSource>::New();
	// outerCube->SetXLength(2 * XHalfLength);
	// outerCube->SetYLength(2 * YHalfLength);
	// outerCube->SetZLength(2 * ZHalfLength);
	// outerCube->SetCenter(0, 0, 0);
	// outerCube->Update();

	// vtkSmartPointer<vtkPolyDataMapper> mapper1 = vtkSmartPointer<vtkPolyDataMapper>::New();
	// mapper1->SetInputConnection(outerCube->GetOutputPort());
		
	// theApp.SourcePanelActor_Position.push_back(vtkSmartPointer<vtkActor>::New());
	// if (theApp.SourcePanelActor_Position.size() != 1)// 벡터 크기가 1이 아니어서 기존에 있었을때
	// {
	// 	theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(theApp.SourcePanelActor_Position[0]);
	// 	theApp.SourcePanelActor_Position = { theApp.SourcePanelActor_Position.back() };
	// }
	// theApp.SourcePanelActor_Position[0]->SetMapper(mapper1);

	// theApp.SourcePanelActor_Position[0]->SetProperty(property1);
	// theApp.SourcePanelActor_Position[0]->PickableOff();
	// theApp.SourcePanelActor_Position[0]->SetPosition(XCenter, YCenter, ZCenter);
	
	// theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(theApp.SourcePanelActor_Position[0]);
	// theApp.m_pVTKWidget->renderWindow()->Render();

	// [변경] 인자로 받은 변수 사용
    //theApp.SourcePanelActor_Position[0]->SetPosition(cenX, cenY, cenZ);
	theApp.SourcePanelActor_Position[0]->SetPosition(XCenter, YCenter, ZCenter);
    
    theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(theApp.SourcePanelActor_Position[0]);
    theApp.m_pVTKWidget->renderWindow()->Render();
}

// Type Selection (현재 비어있지만 선언 유지)
void RoomContaminationWidget::slot_RI_Select_sourceRC_RadioButtonClicked()
{
}
void RoomContaminationWidget::slot_ES_Select_sourceRC_RadioButtonClicked()
{
}

// Radionuclide Operations
void RoomContaminationWidget::slot_RIList_Add_sourceRC_ButtonClicked()
{
	QListWidgetItem* item = new QListWidgetItem(RI_List_sourceRC_QListWidget);
	QWidget* widget = new QWidget;
	QHBoxLayout* hLayout = new QHBoxLayout;
	widget->setStyleSheet("background-color: white;");

	QString arg1 = RI_sourceRC_QLineEdit->text();
	QLabel* label1 = new QLabel(arg1);
	label1->setFixedWidth(panel->Window_width * 0.025);
	label1->setFont(panel->font_D_LW2); // Set font for the label

	QString arg2 = QString::number(Activity_sourceRC_QLineEdit->text().toDouble(), 'e', 3);
	QLabel* label2 = new QLabel(arg2);
	label2->setFixedWidth(panel->Window_width * 0.025);
	label2->setFont(panel->font_D_LW2); // Set font for the label

	QString arg3 = "";
	QLabel* labelPosX = new QLabel(arg3);
	labelPosX->setFixedWidth(panel->Window_width * 0.025);
	labelPosX->setFont(panel->font_D_LW2); // Set font for the label

	QString arg4 = "";
	QLabel* labelPosY = new QLabel(arg4);
	labelPosY->setFixedWidth(panel->Window_width * 0.025);
	labelPosY->setFont(panel->font_D_LW2); // Set font for the label

	QString arg5 = "";
	QLabel* labelPosZ = new QLabel(arg5);
	labelPosZ->setFixedWidth(panel->Window_width * 0.025);
	labelPosZ->setFont(panel->font_D_LW2); // Set font for the label

	RIList_Delete_sourceRC_QButton.push_back(new QPushButton());
	if (RIList_sourceRC_MakingIndex >= RIList_Delete_sourceRC_QButton.size()) theApp.SetMessageBox("RI_Delete_sourceRC_QButton Vector index error");
	RIList_Delete_sourceRC_QButton[RIList_sourceRC_MakingIndex]->setCheckable(false);
	RIList_Delete_sourceRC_QButton[RIList_sourceRC_MakingIndex]->setStyleSheet(DialogStyle::MENU_BTN13);
	RIList_Delete_sourceRC_QButton[RIList_sourceRC_MakingIndex]->setFont(panel->font_D_BTN13);
	RIList_Delete_sourceRC_QButton[RIList_sourceRC_MakingIndex]->setFixedSize(panel->Window_width * 0.008, panel->Window_height * 0.012);
	RIList_Delete_sourceRC_QButton[RIList_sourceRC_MakingIndex]->setIcon(QIcon("./data/image/delete.png"));
	RIList_Delete_sourceRC_QButton[RIList_sourceRC_MakingIndex]->setObjectName(QString::number(RIList_sourceRC_MakingIndex));

	RIList_Info_sourceRC_QButton.push_back(new QPushButton());
	if (RIList_sourceRC_MakingIndex >= RIList_Delete_sourceRC_QButton.size()) theApp.SetMessageBox("RI_Info_sourceRC_QButton Vector index error");
	RIList_Info_sourceRC_QButton[RIList_sourceRC_MakingIndex]->setCheckable(false);
	RIList_Info_sourceRC_QButton[RIList_sourceRC_MakingIndex]->setStyleSheet(DialogStyle::MENU_BTN13);
	RIList_Info_sourceRC_QButton[RIList_sourceRC_MakingIndex]->setFont(panel->font_D_BTN13);
	RIList_Info_sourceRC_QButton[RIList_sourceRC_MakingIndex]->setFixedSize(panel->Window_width * 0.008, panel->Window_height * 0.012);
	RIList_Info_sourceRC_QButton[RIList_sourceRC_MakingIndex]->setIcon(QIcon("./data/image/info.png"));
	RIList_Info_sourceRC_QButton[RIList_sourceRC_MakingIndex]->setObjectName(QString::number(RIList_sourceRC_MakingIndex));

	hLayout->addWidget(label1);
	hLayout->addWidget(label2);
	hLayout->addWidget(labelPosX);
	hLayout->addWidget(labelPosY);
	hLayout->addWidget(labelPosZ);
	hLayout->addWidget(RIList_Delete_sourceRC_QButton[RIList_sourceRC_MakingIndex]);
	hLayout->addWidget(RIList_Info_sourceRC_QButton[RIList_sourceRC_MakingIndex]);
	widget->setLayout(hLayout);

	RI_List_sourceRC_QListWidget->setItemWidget(item, widget);
	item->setSizeHint(widget->sizeHint());

	// Connect signals of the new buttons
	connect(RIList_Delete_sourceRC_QButton[RIList_sourceRC_MakingIndex], SIGNAL(clicked()), this, SLOT(slot_RIList_Delete_sourceRC_ButtonClicked())); connect(RIList_Delete_sourceRC_QButton[RIList_sourceRC_MakingIndex], SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
	connect(RIList_Info_sourceRC_QButton[RIList_sourceRC_MakingIndex], SIGNAL(clicked()), this, SLOT(slot_RIList_Info_sourceRC_ButtonClicked())); connect(RIList_Info_sourceRC_QButton[RIList_sourceRC_MakingIndex], SIGNAL(clicked()), this, SLOT(slot_allButtonClicked()));
	
	// Set Global Variables
	std::map<int, QString> tmp_map;
	tmp_map[0] = "True";
	tmp_map[1] = RI_sourceRC_QLineEdit->text();
	tmp_map[2] = Activity_sourceRC_QLineEdit->text(); if (tmp_map[2] == "") tmp_map[2] = "0";
	RI_Info_sourceRC.push_back(tmp_map);
	RIList_sourceRC_MakingIndex++;

	theApp.SetMessageBox_RadionuclideWarning("The radionuclide data provided in this code are based on the data provided in the ICRP 107 publication.\nTHIS DATA DO NOT INCLUDE ANY DAUGHTER NUCLIDES!!!");
}
void RoomContaminationWidget::slot_RIList_Delete_sourceRC_ButtonClicked()
{
	// Button의 Index 찾기
	QPushButton* buttonSender = qobject_cast<QPushButton*>(sender());
	QString buttonText = buttonSender->objectName();

	// The first part is the ID
	int RadonuclideIndex = buttonText.toDouble();
	int row = RI_List_sourceRC_QListWidget->row(RI_List_sourceRC_QListWidget->item(RadonuclideIndex));
	QListWidgetItem* toRemove = RI_List_sourceRC_QListWidget->takeItem(row);
	delete toRemove; // Delete the item to free memory

	if (RadonuclideIndex >= 0 && RadonuclideIndex < RI_Info_sourceRC.size()) {
		RI_Info_sourceRC.erase(RI_Info_sourceRC.begin() + RadonuclideIndex);
		RIList_Delete_sourceRC_QButton.erase(RIList_Delete_sourceRC_QButton.begin() + RadonuclideIndex);
		RIList_Info_sourceRC_QButton.erase(RIList_Info_sourceRC_QButton.begin() + RadonuclideIndex);	
	}
	else {
		theApp.SetMessageBox("RI_list_Info_sourceRC Vecetor index error");
	}

	for (int index = 0; index < RIList_Delete_sourceRC_QButton.size(); index++)
	{
		RIList_Delete_sourceRC_QButton[index]->setObjectName(QString::number(index));
		RIList_Info_sourceRC_QButton[index]->setObjectName(QString::number(index));
	}
	RIList_sourceRC_MakingIndex--; // listwidget은 vector 형태로 동작해야 함(row의 item이 실제 몇 행인지로 결정됨)	
}
void RoomContaminationWidget::slot_RIList_Info_sourceRC_ButtonClicked()
{
	// Button의 Index 찾기
	QPushButton* buttonSender = qobject_cast<QPushButton*>(sender());
	QString buttonText = buttonSender->objectName();

	QStringList list_button = buttonText.split("_");

	// The first part is the ID
	int RadonuclideIndex = list_button[0].toInt();	
	QListWidgetItem* item = RI_List_sourceRC_QListWidget->item(RadonuclideIndex);
	if (!item) {
		// Handle error: item not found
		return;
	}

	QWidget* widget = RI_List_sourceRC_QListWidget->itemWidget(item);
	QHBoxLayout* hLayout = qobject_cast<QHBoxLayout*>(widget->layout());
	if (!hLayout) {
		// Handle error: layout not found
		return;
	}

	// Assuming the order of labels in the layout is known
	QLabel* labelRadionuclide = qobject_cast<QLabel*>(hLayout->itemAt(0)->widget());
	QLabel* labelActivity = qobject_cast<QLabel*>(hLayout->itemAt(1)->widget());

	// Create a new dialog
	QDialog* infoDialog = new QDialog(this);
	infoDialog->setStyleSheet("background-color: white;");
	infoDialog->resize(panel->Window_width * 0.3, panel->Window_height * 0.55);
	infoDialog->setWindowTitle("Radionuclide Information");

	// Create a layout and a label for the dialog
	QVBoxLayout* layout = new QVBoxLayout(infoDialog);
	// Create a QTextEdit for the dialog
	QTextEdit* textEdit = new QTextEdit(infoDialog);
	textEdit->setReadOnly(true); // Make it read-only if editing is not required

	// Create a QTextCursor for inserting formatted text
	QTextCursor cursor = textEdit->textCursor();

	// Define different text formats
	QTextCharFormat boldFormat;
	boldFormat.setFontFamily("Courier New"); // This is monospaced font
	boldFormat.setFontWeight(QFont::Bold);
	boldFormat.setFontPointSize(14);

	QTextCharFormat boldMediumFormat;
	boldMediumFormat.setFontFamily("Courier New");
	boldMediumFormat.setFontWeight(QFont::Bold);
	boldMediumFormat.setFontPointSize(12);

	QTextCharFormat boldSmallFormat;
	boldSmallFormat.setFontFamily("Courier New");
	boldSmallFormat.setFontWeight(QFont::Bold);
	boldSmallFormat.setFontPointSize(10);

	QTextCharFormat normalFormat;
	normalFormat.setFontFamily("Courier New");
	normalFormat.setFontPointSize(10);

	QTextCharFormat smallFormat;
	smallFormat.setFontFamily("Courier New");
	smallFormat.setFontPointSize(8);

	QTextCharFormat verySmallFormat;
	verySmallFormat.setFontFamily("Courier New");
	verySmallFormat.setFontPointSize(6);

	// Insert formatted text into QTextEdit
	cursor.insertText("*This radionuclide information is based on ICRP 107 Publication\n\n", smallFormat);
	cursor.insertText("Radionuclide Information\n", boldFormat);
	cursor.insertText("Radionuclide: " + labelRadionuclide->text() + "\n", normalFormat);
	cursor.insertText("Activity (Bq): " + labelActivity->text() + "\n", normalFormat);

	// Fine RN information from ICRP07.NDX file (ICRP 107 Pub.)
	QFile file("./data/radioisotopes/_ICRP-07.NDX");
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		// Handle error: Unable to open file
		return;
	}
	QTextStream in(&file);
	QString line;
	bool isDataFound = false;
	QString halfLife, daughterNucleus1, branchingFraction1, daughterNucleus2, branchingFraction2, daughterNucleus3, branchingFraction3, daughterNucleus4, branchingFraction4;
	int daughterNucleus1_index = 7; // 이상적으로 8열(Index: 7)에서 딸핵종1 시작

	while (!in.atEnd()) {
		line = in.readLine();
		if (line.trimmed().isEmpty()) continue; // Skip empty lines
		QStringList columns = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
		if (columns[0] == labelRadionuclide->text()) {
			isDataFound = true;
			halfLife = columns[1]; // Extract HalfLife
			bool isNumber; double tmpNum;
			tmpNum = columns[3].toDouble(&isNumber); // 이상적으로는 숫자, 하지만 DecayMode 알파벳이 할당되어 있을수도 있음
			if (!isNumber) daughterNucleus1_index++; // Decay Mode has assgined to two columns
			daughterNucleus1 = columns[daughterNucleus1_index];
			branchingFraction1 = columns[daughterNucleus1_index + 2];
			tmpNum = columns[daughterNucleus1_index + 3].toDouble(&isNumber);
			if (isNumber) break;// 이상적으로는 딸핵종, 하지만 딸핵종이 더 이상 없어 0이 할당되어 있을수도 있음

			daughterNucleus2 = columns[daughterNucleus1_index + 3];
			branchingFraction2 = columns[daughterNucleus1_index + 5];
			tmpNum = columns[daughterNucleus1_index + 6].toDouble(&isNumber);
			if (isNumber) break;// 이상적으로는 딸핵종, 하지만 딸핵종이 더 이상 없어 0이 할당되어 있을수도 있음

			daughterNucleus3 = columns[daughterNucleus1_index + 6];
			branchingFraction3 = columns[daughterNucleus1_index + 8];
			tmpNum = columns[daughterNucleus1_index + 9].toDouble(&isNumber);
			if (isNumber) break;// 이상적으로는 딸핵종, 하지만 딸핵종이 더 이상 없어 0이 할당되어 있을수도 있음

			daughterNucleus4 = columns[daughterNucleus1_index + 9];
			branchingFraction4 = columns[daughterNucleus1_index + 11];
			break; // Stop reading further as the data is found
		}
	}
	file.close();

	cursor.insertText("HalfLife: " + halfLife + "\n\n", normalFormat);
	cursor.insertText("Daughter Nuclides Information\n", boldFormat);
	cursor.insertText("Radionuclide   Fraction\n", boldSmallFormat);

	// Insert text into QTextEdit with padding
	if (!daughterNucleus1.isEmpty()) {
		QString padding(15 - daughterNucleus1.length(), ' ');
		cursor.insertText(daughterNucleus1 + padding + branchingFraction1 + "\n", normalFormat);
	}
	if (!daughterNucleus2.isEmpty()) {
		QString padding(15 - daughterNucleus2.length(), ' ');
		cursor.insertText(daughterNucleus2 + padding + branchingFraction2 + "\n", normalFormat);
	}
	if (!daughterNucleus3.isEmpty()) {
		QString padding(15 - daughterNucleus3.length(), ' ');
		cursor.insertText(daughterNucleus3 + padding + branchingFraction3 + "\n", normalFormat);
	}
	if (!daughterNucleus4.isEmpty()) {
		QString padding(15 - daughterNucleus4.length(), ' ');
		cursor.insertText(daughterNucleus4 + padding + branchingFraction4 + "\n", normalFormat);
	}

	cursor.insertText("\nRadiation Spectrum of the Entered Radionuclide\n", boldFormat);
	cursor.insertText("Radiation      Fraction       Energy (MeV)\n", boldSmallFormat);

	QString RNspectrumFileName = ("./data/radioisotopes/" + labelRadionuclide->text() + ".txt");
	QFile RNspectrumFile(RNspectrumFileName);
	if (!RNspectrumFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		// Handle error: Unable to open file
		return;
	}

	QTextStream RNspectrumInput(&RNspectrumFile);
	QString RNspectrumLine;

	// Skip the first line
	if (!RNspectrumInput.atEnd()) {
		RNspectrumLine = RNspectrumInput.readLine();
	}
	
	while (!RNspectrumInput.atEnd()) {
		RNspectrumLine = RNspectrumInput.readLine();
		QStringList columns = RNspectrumLine.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);

		if (columns.size() >= 4) {
			QString radiation = columns[3];
			QString fraction = columns[1];
			QString energy = columns[2];
			
			QString padding1(15 - radiation.length(), ' ');
			QString padding2(15 - fraction.length(), ' ');
			cursor.insertText(radiation + padding1 + fraction + padding2 + energy + "\n", normalFormat);
		}
	}
	RNspectrumFile.close();

	cursor.insertText("\n*Alpha recoil nuclei and fission fragments are excluded.\n*PG, DG, and DB are radiations of spontaneous fission.\n", smallFormat);
	cursor.insertText("G: Gamma rays\n", verySmallFormat);
	cursor.insertText("PG: Prompt gamma rays\n", verySmallFormat);
	cursor.insertText("DG: Delayed gamma rays\n", verySmallFormat);
	cursor.insertText("X: X rays\n", verySmallFormat);
	cursor.insertText("AQ: Annihilation photons\n", verySmallFormat);
	cursor.insertText("B+: Beta-plus particles\n", verySmallFormat);
	cursor.insertText("B-: Beta-minus particles\n", verySmallFormat);
	cursor.insertText("DB: Delayed beta particles\n", verySmallFormat);
	cursor.insertText("IE: Internal conversion electrons\n", verySmallFormat);
	cursor.insertText("AE: Auger electrons\n", verySmallFormat);
	cursor.insertText("A: Alpha particles\n", verySmallFormat);
	cursor.insertText("N: Neutrons\n", verySmallFormat);

	cursor.movePosition(QTextCursor::Start);
	textEdit->setTextCursor(cursor);

	// Add the QTextEdit to the layout
	layout->addWidget(textEdit);

	// Set the layout to the dialog
	infoDialog->setLayout(layout);

	// Show the dialog
	infoDialog->exec(); // Use exec() for a modal dialog or show() for a non-modal dialog
}

///////////////////////////////////////////////////////////////////////////
// EnvironmentalContaminationWidget Implementation
///////////////////////////////////////////////////////////////////////////

EnvironmentalContaminationWidget::EnvironmentalContaminationWidget(QWidget* parent)
    : ParentT(parent)
{
}

EnvironmentalContaminationWidget::~EnvironmentalContaminationWidget()
{
}

bool EnvironmentalContaminationWidget::initialize()
{
    if (!ParentT::initialize()) return false;

    QVBoxLayout* subLayout = new QVBoxLayout;
    subLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.005, panel->Window_width * 0.003, panel->Window_height * 0.005);
    subLayout->setSpacing(panel->Window_width * 0.003);
    subLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    {
        QVBoxLayout* vLayout = new QVBoxLayout;
        vLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.005, panel->Window_width * 0.003, panel->Window_height * 0.005);
        vLayout->setSpacing(panel->Window_width * 0.003);
        vLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

        // [Visualization]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setSpacing(panel->Window_width * 0.005);
            hLayout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

            QLabel* labelTitle = new QLabel;
            labelTitle->setFixedSize(panel->Window_width * 0.104, panel->Window_height * 0.028);
            labelTitle->setStyleSheet(DialogStyle::DATA_LABEL);
            labelTitle->setFont(panel->font_D_LBL1);
            labelTitle->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            labelTitle->setText(QString::fromLocal8Bit("Visualization"));

            m_EnvironmentalSourceVisualizationButton = new QPushButton(QString::fromLocal8Bit("Source"));
            panel->Buttons_FunctionPanelRight.append(m_EnvironmentalSourceVisualizationButton);
            m_EnvironmentalSourceVisualizationButton->setFixedSize(panel->Window_width * 0.05, panel->Window_height * 0.028);
            m_EnvironmentalSourceVisualizationButton->setCheckable(true);
            m_EnvironmentalSourceVisualizationButton->setChecked(true);
            m_EnvironmentalSourceVisualizationButton->setStyleSheet(DialogStyle::MENU_BTN3);
            m_EnvironmentalSourceVisualizationButton->setFont(panel->font_D_BTN3);
            
            connect(m_EnvironmentalSourceVisualizationButton, SIGNAL(clicked()), this, SLOT(slot_EnvironmentalSourceVisualization_ButtonClicked())); 
            connect(m_EnvironmentalSourceVisualizationButton, SIGNAL(clicked()), panel, SLOT(slot_allButtonClicked()));

            hLayout->addWidget(labelTitle);
            hLayout->addStretch();
            hLayout->addWidget(m_EnvironmentalSourceVisualizationButton);

            vLayout->addLayout(hLayout);
        }

        // [Type Selection]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setContentsMargins(panel->Window_width * 0.003, 0, panel->Window_width * 0.003, 0);
            hLayout->setSpacing(panel->Window_width * 0.003);
            hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

            QLabel* label0 = new QLabel;
            label0->setFixedSize(panel->Window_width * 0.068, panel->Window_height * 0.028);
            label0->setStyleSheet(DialogStyle::DATA_LABEL);
            label0->setFont(panel->font_D_LBL1);
            label0->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            label0->setText(QString::fromLocal8Bit("Environment type:"));

            m_comboBoxEnvironmentalSourceType = new QComboBox;
            m_comboBoxEnvironmentalSourceType->setFixedSize(panel->Window_width * 0.104, panel->Window_height * 0.028);
            m_comboBoxEnvironmentalSourceType->setStyleSheet(DialogStyle::COMBOBOX);
            m_comboBoxEnvironmentalSourceType->setFont(panel->font_D_CB1);

            m_comboBoxEnvironmentalSourceType->setEditable(true);
            m_comboBoxEnvironmentalSourceType->lineEdit()->setReadOnly(true);
            m_comboBoxEnvironmentalSourceType->lineEdit()->setAlignment(Qt::AlignCenter);
            m_comboBoxEnvironmentalSourceType->lineEdit()->setFont(panel->font_D_CB1);

            m_comboBoxEnvironmentalSourceType->addItem("Soil");
            m_comboBoxEnvironmentalSourceType->addItem("Air");
            m_comboBoxEnvironmentalSourceType->addItem("Water immersion");
            m_comboBoxEnvironmentalSourceType->setCurrentIndex(0);

            hLayout->addWidget(label0);
            hLayout->addStretch();
            hLayout->addWidget(m_comboBoxEnvironmentalSourceType);
            
            // 로직은 this (Change Type 슬롯)
            connect(m_comboBoxEnvironmentalSourceType, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_ChangeEnvironmentalSourceType(int)));

            vLayout->addLayout(hLayout);
        }

        // [Energy / Depth]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setContentsMargins(panel->Window_width * 0.003, 0, panel->Window_width * 0.003, 0);
            hLayout->setSpacing(panel->Window_width * 0.003);
            hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

            labelEnvironmentalSoilDepth = new QLabel;
            labelEnvironmentalSoilDepth->setFixedSize(panel->Window_width * 0.068, panel->Window_height * 0.028);
            labelEnvironmentalSoilDepth->setStyleSheet(DialogStyle::DATA_LABEL);
            labelEnvironmentalSoilDepth->setFont(panel->font_D_LBL1);
            labelEnvironmentalSoilDepth->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            labelEnvironmentalSoilDepth->setText(QString::fromLocal8Bit("Depth:"));

            m_lineEditEnvironmentalSoilDepth = new QLineEdit;
            m_lineEditEnvironmentalSoilDepth->setFixedSize(panel->Window_width * 0.104, panel->Window_height * 0.028);
            m_lineEditEnvironmentalSoilDepth->setAlignment(Qt::AlignCenter);
            m_lineEditEnvironmentalSoilDepth->setStyleSheet(DialogStyle::LINEEDIT_NAME);
            m_lineEditEnvironmentalSoilDepth->setFont(panel->font_D_LE1);
            m_lineEditEnvironmentalSoilDepth->setPlaceholderText(QString::fromLocal8Bit("cm"));

            hLayout->addWidget(labelEnvironmentalSoilDepth);
            hLayout->addStretch();
            hLayout->addWidget(m_lineEditEnvironmentalSoilDepth);

            vLayout->addLayout(hLayout);
        }

        // [Radioisotope Input]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setContentsMargins(panel->Window_width * 0.003, 0, panel->Window_width * 0.003, 0);
            hLayout->setSpacing(panel->Window_width * 0.003);
            hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

            labelEnvironmentalRadionuclideTitle = new QLabel;
            labelEnvironmentalRadionuclideTitle->setFixedSize(panel->Window_width * 0.068, panel->Window_height * 0.028);
            labelEnvironmentalRadionuclideTitle->setStyleSheet(DialogStyle::DATA_LABEL);
            labelEnvironmentalRadionuclideTitle->setFont(panel->font_D_LBL1);
            labelEnvironmentalRadionuclideTitle->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            labelEnvironmentalRadionuclideTitle->setText(QString::fromLocal8Bit("Radionuclide"));

            m_lineEditEnvironmentalRadionuclide = new QLineEdit;
            m_lineEditEnvironmentalRadionuclide->setFixedSize(panel->Window_width * 0.104, panel->Window_height * 0.028);
            m_lineEditEnvironmentalRadionuclide->setAlignment(Qt::AlignCenter);
            m_lineEditEnvironmentalRadionuclide->setStyleSheet(DialogStyle::LINEEDIT_NAME);
            m_lineEditEnvironmentalRadionuclide->setFont(panel->font_D_LE1);
            m_lineEditEnvironmentalRadionuclide->setPlaceholderText(QString::fromLocal8Bit("Enter radionuclide"));

            // panel의 panel->RIsourceList 사용
            QCompleter *completer = new QCompleter(panel->RIsourceList, this);
            completer->setCaseSensitivity(Qt::CaseInsensitive);
            m_lineEditEnvironmentalRadionuclide->setCompleter(completer);

            hLayout->addWidget(labelEnvironmentalRadionuclideTitle);
            hLayout->addStretch();
            hLayout->addWidget(m_lineEditEnvironmentalRadionuclide);

            vLayout->addLayout(hLayout);
        }

        // [Activity Input]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setContentsMargins(panel->Window_width * 0.003, 0, panel->Window_width * 0.003, 0);
            hLayout->setSpacing(panel->Window_width * 0.003);
            hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

            labelEnvironmentalRadionuclideActivity = new QLabel;
            labelEnvironmentalRadionuclideActivity->setFixedSize(panel->Window_width * 0.068, panel->Window_height * 0.028);
            labelEnvironmentalRadionuclideActivity->setStyleSheet(DialogStyle::DATA_LABEL);
            labelEnvironmentalRadionuclideActivity->setFont(panel->font_D_LBL1);
            labelEnvironmentalRadionuclideActivity->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            labelEnvironmentalRadionuclideActivity->setText(QString::fromLocal8Bit("Activity"));

            m_lineEditEnvironmentalActivity = new QLineEdit;
            m_lineEditEnvironmentalActivity->setFixedSize(panel->Window_width * 0.104, panel->Window_height * 0.028);
            m_lineEditEnvironmentalActivity->setAlignment(Qt::AlignCenter);
            m_lineEditEnvironmentalActivity->setStyleSheet(DialogStyle::LINEEDIT_NAME);
            m_lineEditEnvironmentalActivity->setFont(panel->font_D_LE1);
            m_lineEditEnvironmentalActivity->setPlaceholderText(QString::fromLocal8Bit("Bq"));

            hLayout->addWidget(labelEnvironmentalRadionuclideActivity);
            hLayout->addStretch();
            hLayout->addWidget(m_lineEditEnvironmentalActivity);

            vLayout->addLayout(hLayout);
        }

        // [Radionuclide Add Button]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setContentsMargins(panel->Window_width * 0.003, 0, panel->Window_width * 0.003, 0);
            hLayout->setSpacing(panel->Window_width * 0.003);
            hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

            labelEnvironmentalRadionuclideList = new QLabel;
            labelEnvironmentalRadionuclideList->setFixedSize(panel->Window_width * 0.088, panel->Window_height * 0.028);
            labelEnvironmentalRadionuclideList->setStyleSheet(DialogStyle::DATA_LABEL);
            labelEnvironmentalRadionuclideList->setFont(panel->font_D_LBL1);
            labelEnvironmentalRadionuclideList->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
            labelEnvironmentalRadionuclideList->setText(QString::fromLocal8Bit("Entered radionuclides"));

            EnvironmentalRadionuclideAddButton = new QPushButton(QString::fromLocal8Bit("Add"));
            panel->Buttons_FunctionPanelRight.append(EnvironmentalRadionuclideAddButton);
            EnvironmentalRadionuclideAddButton->setFixedSize(panel->Window_width * 0.042, panel->Window_height * 0.037);
            EnvironmentalRadionuclideAddButton->setCheckable(false);
            EnvironmentalRadionuclideAddButton->setStyleSheet(DialogStyle::MENU_BTN2);
            EnvironmentalRadionuclideAddButton->setFont(panel->font_D_BTN2);

            EnvironmentalRadionuclideAddButton->setChecked(false);

            connect(EnvironmentalRadionuclideAddButton, SIGNAL(clicked()), this, SLOT(slot_EnvironmentalRadionuclideAdd_ButtonClicked())); 
            connect(EnvironmentalRadionuclideAddButton, SIGNAL(clicked()), panel, SLOT(slot_allButtonClicked()));

            hLayout->addWidget(labelEnvironmentalRadionuclideList);
            hLayout->addStretch();
            hLayout->addWidget(EnvironmentalRadionuclideAddButton);

            vLayout->addLayout(hLayout);
        }

        // [Radionuclide List]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setContentsMargins(panel->Window_width * 0.003, 0, panel->Window_width * 0.003, 0);
            hLayout->setSpacing(panel->Window_width * 0.003);
            hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

            listWidget_E_Radionuclide = new QListWidget;

            listWidget_E_Radionuclide->setStyleSheet("QListWidget { border-radius: 15px; background-color: white; }");
            listWidget_E_Radionuclide->setFixedSize(panel->Window_width * 0.175, panel->Window_height * 0.155);
            listWidget_E_Radionuclide->setFont(panel->font_D_LW1);

            hLayout->addWidget(listWidget_E_Radionuclide);

            vLayout->addLayout(hLayout);
        }
        subLayout->addLayout(vLayout);
    }
    
    this->setLayout(subLayout);
    return true;
}

// === Slot Functions ===

void EnvironmentalContaminationWidget::slot_EnvironmentalSourceVisualization_ButtonClicked()
{
    // 기존 코드 비어 있음 (구현 예정)
}

void EnvironmentalContaminationWidget::slot_ChangeEnvironmentalSourceType(int idx)
{
    if (idx == 0) // Soil
    {
        labelEnvironmentalSoilDepth->show();
        m_lineEditEnvironmentalSoilDepth->show();
    }
    else // Air, Water immersion
    {
        labelEnvironmentalSoilDepth->hide();
        m_lineEditEnvironmentalSoilDepth->hide();
    }
}

void EnvironmentalContaminationWidget::slot_EnvironmentalRadionuclideAdd_ButtonClicked()
{
    // 기존 코드에 메시지박스만 있음
    theApp.SetMessageBox("not-developed");
}

void EnvironmentalContaminationWidget::slot_Ereset_ButtonClicked()
{
    // 기존 코드 비어 있음
}


///////////////////////////////////////////////////////////////////////////
// ParallelBeamWidget Implementation
///////////////////////////////////////////////////////////////////////////

ParallelBeamWidget::ParallelBeamWidget(QWidget* parent)
    : ParentT(parent)
{
}

ParallelBeamWidget::~ParallelBeamWidget()
{
}

bool ParallelBeamWidget::initialize()
{
    if (!ParentT::initialize()) return false;

    QVBoxLayout* subLayout = new QVBoxLayout;
    subLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.005, panel->Window_width * 0.003, panel->Window_height * 0.005);
    subLayout->setSpacing(panel->Window_width * 0.003);
    subLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    {
        QVBoxLayout* vLayout = new QVBoxLayout;
        vLayout->setContentsMargins(panel->Window_width * 0.003, panel->Window_height * 0.005, panel->Window_width * 0.003, panel->Window_height * 0.005);
        vLayout->setSpacing(panel->Window_width * 0.003);
        vLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

        // ... [Visualization, Position, Radius, Theta, Phi, Buttons 부분은 이전과 동일하여 생략] ...
        // (필요하시면 이 부분도 다시 채워드립니다. 위쪽 코드와 동일합니다.)
        
        // [Visualization]
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setSpacing(panel->Window_width * 0.005);
            hLayout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

            QLabel* labelTitle = new QLabel;
            labelTitle->setFixedSize(panel->Window_width * 0.104, panel->Window_height * 0.028);
            labelTitle->setStyleSheet(DialogStyle::DATA_LABEL);
            labelTitle->setFont(panel->font_D_LBL1);
            labelTitle->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            labelTitle->setText(QString::fromLocal8Bit("Visualization"));

            m_ParallelBeamSourceVisualizationButton = new QPushButton(QString::fromLocal8Bit("On"));
            panel->Buttons_FunctionPanelRight.append(m_ParallelBeamSourceVisualizationButton);
            m_ParallelBeamSourceVisualizationButton->setFixedSize(panel->Window_width * 0.05, panel->Window_height * 0.028);
            m_ParallelBeamSourceVisualizationButton->setCheckable(true);
            m_ParallelBeamSourceVisualizationButton->setChecked(true);
            m_ParallelBeamSourceVisualizationButton->setStyleSheet(DialogStyle::MENU_BTN3);
            m_ParallelBeamSourceVisualizationButton->setFont(panel->font_D_BTN3);

            QLabel* labelDir = new QLabel;
            labelDir->setFixedSize(panel->Window_width * 0.03, panel->Window_height * 0.028);
            labelDir->setStyleSheet(DialogStyle::DATA_LABEL);
            labelDir->setFont(panel->font_D_LBL4);
            labelDir->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            labelDir->setText(QString::fromLocal8Bit("Direction"));

            sourceCB_dirCheckBox = new QCheckBox;
            sourceCB_dirCheckBox->setChecked(true);
            sourceCB_dirCheckBox->setFixedSize(panel->Window_width * 0.02, panel->Window_height * 0.02);
            sourceCB_dirCheckBox->setStyleSheet("QCheckBox { background-color: transparent; }");

            connect(m_ParallelBeamSourceVisualizationButton, SIGNAL(clicked()), this, SLOT(slot_ParallelBeamSourceVisualization_ButtonClicked())); 
            connect(m_ParallelBeamSourceVisualizationButton, SIGNAL(clicked()), panel, SLOT(slot_allButtonClicked()));

            hLayout->addWidget(labelTitle);
            hLayout->addWidget(m_ParallelBeamSourceVisualizationButton);
            hLayout->addStretch();
            hLayout->addWidget(labelDir);
            hLayout->addWidget(sourceCB_dirCheckBox);

            vLayout->addLayout(hLayout);
        }
        
        // ... (중간 생략: Position, Radius, Theta, Phi, Pick3D 버튼 등은 위와 동일) ...
        // (코드 양을 줄이기 위해 생략했습니다. 전체 코드가 필요하면 말씀해주세요)

        // [Radio Buttons: Mono vs Spectrum]
        // [변경점] connect 부분에 새로운 슬롯 함수 이름 적용
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setSpacing(panel->Window_width * 0.010);
            hLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            hLayout->setContentsMargins(panel->Window_width * 0.005, panel->Window_height * 0.009, 0, 0);

            sourcePB_MonoEnergy_radioButton = new QRadioButton(QString::fromLocal8Bit("Mono energy"));
            sourcePB_MonoEnergy_radioButton->setStyleSheet(DialogStyle::RADIOBUTTON_WIDGET);
            sourcePB_MonoEnergy_radioButton->setFont(panel->font_D_RB2);
            sourcePB_MonoEnergy_radioButton->setFixedSize(panel->Window_width * 0.081, panel->Window_height * 0.028);
            sourcePB_MonoEnergy_radioButton->setChecked(true);

            sourcePB_EnergySpectrum_radioButton = new QRadioButton(QString::fromLocal8Bit("Energy spectrum"));
            sourcePB_EnergySpectrum_radioButton->setStyleSheet(DialogStyle::RADIOBUTTON_WIDGET);
            sourcePB_EnergySpectrum_radioButton->setFont(panel->font_D_RB2);
            sourcePB_EnergySpectrum_radioButton->setFixedSize(panel->Window_width * 0.081, panel->Window_height * 0.028);
            sourcePB_EnergySpectrum_radioButton->setChecked(false);

            // [NEW] 새로 만든 슬롯 연결
            connect(sourcePB_MonoEnergy_radioButton, SIGNAL(clicked()), this, SLOT(slot_PB_MonoEnergy_RadioButtonClicked())); 
            connect(sourcePB_MonoEnergy_radioButton, SIGNAL(clicked()), panel, SLOT(slot_allButtonClicked()));
            
            // [NEW] 새로 만든 슬롯 연결
            connect(sourcePB_EnergySpectrum_radioButton, SIGNAL(clicked()), this, SLOT(slot_PB_EnergySpectrum_RadioButtonClicked())); 
            connect(sourcePB_EnergySpectrum_radioButton, SIGNAL(clicked()), panel, SLOT(slot_allButtonClicked()));

            hLayout->addWidget(sourcePB_MonoEnergy_radioButton);
            hLayout->addWidget(sourcePB_EnergySpectrum_radioButton);

            vLayout->addLayout(hLayout);
        }

        // ... (Mono Energy 관련 UI 생략) ...

        // [ES: Add Button]
        // [변경점] connect 부분에 새로운 슬롯 함수 이름 적용
        {
            QHBoxLayout* hLayout = new QHBoxLayout;
            hLayout->setContentsMargins(panel->Window_width * 0.003, 0, panel->Window_width * 0.003, 0);
            hLayout->setSpacing(panel->Window_width * 0.003);
            hLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

            labelParallelBeamEnergyspectrumList = new QLabel;
            labelParallelBeamEnergyspectrumList->setFixedSize(panel->Window_width * 0.120, panel->Window_height * 0.028);
            labelParallelBeamEnergyspectrumList->setStyleSheet(DialogStyle::DATA_LABEL);
            labelParallelBeamEnergyspectrumList->setFont(panel->font_D_LBL1);
            labelParallelBeamEnergyspectrumList->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
            labelParallelBeamEnergyspectrumList->setText(QString::fromLocal8Bit("Entered energy spectrum list"));
            labelParallelBeamEnergyspectrumList->hide();

            ParallelBeamEnergyspectrumAddButton = new QPushButton(QString::fromLocal8Bit("Add"));
            panel->Buttons_FunctionPanelRight.append(ParallelBeamEnergyspectrumAddButton);
            ParallelBeamEnergyspectrumAddButton->setFixedSize(panel->Window_width * 0.042, panel->Window_height * 0.037);
            ParallelBeamEnergyspectrumAddButton->setCheckable(false);
            ParallelBeamEnergyspectrumAddButton->setStyleSheet(DialogStyle::MENU_BTN2);
            ParallelBeamEnergyspectrumAddButton->setFont(panel->font_D_BTN2);
            ParallelBeamEnergyspectrumAddButton->setChecked(false);
            ParallelBeamEnergyspectrumAddButton->hide();

            connect(ParallelBeamEnergyspectrumAddButton, SIGNAL(clicked()), this, SLOT(slot_ParallelBeamEnergyspectrumAdd_ButtonClicked())); 
            connect(ParallelBeamEnergyspectrumAddButton, SIGNAL(clicked()), panel, SLOT(slot_allButtonClicked()));

            hLayout->addWidget(labelParallelBeamEnergyspectrumList);
            hLayout->addStretch();
            hLayout->addWidget(ParallelBeamEnergyspectrumAddButton);

            vLayout->addLayout(hLayout);
        }

        // ... (나머지 리스트 위젯 등 생략) ...
        // (이전 답변과 동일하게 마무리)
        
        subLayout->addLayout(vLayout);
    }
    
    this->setLayout(subLayout);
    return true;
}

// === Slot Functions (새로 추가된 함수들만 표시) ===

// 1. Visualization (Existing)
void ParallelBeamWidget::slot_ParallelBeamSourceVisualization_ButtonClicked()
{
    if (m_ParallelBeamSourceVisualizationButton->isChecked())
    {
        b_IsSourceLocationVisualized_sourcePB = true;
        m_ParallelBeamSourceVisualizationButton->setText("On");
        for (auto itr_actor : theApp.SourcePanelActor_Position)
        {
            theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(itr_actor);
        }
    }
    else
    {
        b_IsSourceLocationVisualized_sourcePB = false;
        m_ParallelBeamSourceVisualizationButton->setText("Off");
        for (auto itr_actor : theApp.SourcePanelActor_Position)
        {
            theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(itr_actor);
        }
    }
    theApp.m_pVTKWidget->renderWindow()->Render();
}

// 2. 3D Pick (Existing - Empty)
void ParallelBeamWidget::slot_ParallelBeam_Pick3D_ButtonClicked()
{
    // 기능 구현이 필요하면 여기에 작성 (현재는 비어 있음)
}

// 3. Update Geometry (Existing)
void ParallelBeamWidget::slot_ParallelBeamUpdate_ButtonClicked()
{
    // 위치 
    double center_radius[4] = { 0.0, };
    center_radius[0] = m_lineEditParallelBeamPointX->text().toFloat();
    center_radius[1] = m_lineEditParallelBeamPointY->text().toFloat();
    center_radius[2] = m_lineEditParallelBeamPointZ->text().toFloat();
    center_radius[3] = m_lineEditParallelBeamRadius->text().toFloat();

    // 방향 
    double theta_phi[2] = { 0.0, };
    theta_phi[0] = m_lineEditParallelBeamDirectionTheta->text().toFloat();
    theta_phi[1] = m_lineEditParallelBeamDirectionPhi->text().toFloat();

    theApp.sourceObjects->GenerateSourceActor_sourcePB(center_radius, theta_phi);
}

// 4. Radio Button: Mono Energy (NEW)
void ParallelBeamWidget::slot_PB_MonoEnergy_RadioButtonClicked()
{
    sourcePB_MonoEnergy_radioButton->setChecked(true);
    sourcePB_EnergySpectrum_radioButton->setChecked(false);

    // Show Mono Energy Widgets
    labelParallelBeamParticleTypeTitle->show();
    m_comboBoxParallelBeamParticleType->show();
    labelParallelBeamEnergyTitle->show();
    m_lineEditParallelBeamEnergy->show();
    labelParallelBeamIntensityTitle->show();
    m_lineEditParallelBeamIntensity_MonoEnergy->show();

    // Hide ES Widgets
    labelParallelBeamEnergyspectrumTitle->hide();
    ParallelBeamEnergyspectrumFileLoadButton->hide();
    labelParallelBeamEnergyspectrumFileName->hide();
    sourcePB_EnergyspectrumFileNameText->hide();
    labelParallelBeamEnergyspectrumIntensity->hide();
    m_lineEditParallelBeamIntensity_EnergySpectrum->hide();
    labelParallelBeamEnergyspectrumList->hide();
    ParallelBeamEnergyspectrumAddButton->hide();
    ES_List_sourcePB_QListWidget->hide();
}

// 5. Radio Button: Energy Spectrum (NEW)
void ParallelBeamWidget::slot_PB_EnergySpectrum_RadioButtonClicked()
{
    sourcePB_MonoEnergy_radioButton->setChecked(false);
    sourcePB_EnergySpectrum_radioButton->setChecked(true);

    // Hide Mono Energy Widgets
    labelParallelBeamParticleTypeTitle->hide();
    m_comboBoxParallelBeamParticleType->hide();
    labelParallelBeamEnergyTitle->hide();
    m_lineEditParallelBeamEnergy->hide();
    labelParallelBeamIntensityTitle->hide();
    m_lineEditParallelBeamIntensity_MonoEnergy->hide();

    // Show ES Widgets
    labelParallelBeamEnergyspectrumTitle->show();
    ParallelBeamEnergyspectrumFileLoadButton->show();
    labelParallelBeamEnergyspectrumFileName->show();
    sourcePB_EnergyspectrumFileNameText->show();
    labelParallelBeamEnergyspectrumIntensity->show();
    m_lineEditParallelBeamIntensity_EnergySpectrum->show();
    labelParallelBeamEnergyspectrumList->show();
    ParallelBeamEnergyspectrumAddButton->show();
    ES_List_sourcePB_QListWidget->show();
}

// 6. File Load (Existing)
void ParallelBeamWidget::slot_ParallelBeamEnergyspectrumFileLoad_ButtonClicked()
{
    QString dir = QFileDialog::getOpenFileName(
        this,
        tr("Select a file to open"),
        "",
        tr("Energy Spectrum Files (*.*)"));

    if (!dir.isEmpty())
    {
        sourcePB_EnergyspectrumFileNameText->setPlaceholderText(dir);
        sourcePB_EnergyspectrumFileNameText->setReadOnly(true);
    }
}

// 7. List Add (NEW & Existing Logic Combined)
void ParallelBeamWidget::slot_ParallelBeamEnergyspectrumAdd_ButtonClicked()
{
    QListWidgetItem* item = new QListWidgetItem(ES_List_sourcePB_QListWidget);
    QWidget* widget = new QWidget;
    QHBoxLayout* hLayout = new QHBoxLayout;
    widget->setStyleSheet("background-color: white;");

    QString arg1 = QFileInfo(sourcePB_EnergyspectrumFileNameText->placeholderText()).fileName();
    QLabel* label1 = new QLabel(arg1);
    label1->setFixedWidth(panel->Window_width * 0.025);
    label1->setFont(panel->font_D_LW2); 

    QString arg2 = QString::number(m_lineEditParallelBeamIntensity_EnergySpectrum->text().toDouble(), 'e', 3);
    QLabel* label2 = new QLabel(arg2);
    label2->setFixedWidth(panel->Window_width * 0.025);
    label2->setFont(panel->font_D_LW2); 

    // 버튼 생성 및 리스트 관리
    m_sourcePB_ES_listDeleteButton.push_back(new QPushButton());
    if (ESList_sourcePB_MakingIndex >= m_sourcePB_ES_listDeleteButton.size()) 
        theApp.SetMessageBox("PB ES Delete Button Index Error");
    
    int currentIndex = ESList_sourcePB_MakingIndex;
    m_sourcePB_ES_listDeleteButton[currentIndex]->setCheckable(false);
    m_sourcePB_ES_listDeleteButton[currentIndex]->setStyleSheet(DialogStyle::MENU_BTN13);
    m_sourcePB_ES_listDeleteButton[currentIndex]->setFont(panel->font_D_BTN13);
    m_sourcePB_ES_listDeleteButton[currentIndex]->setFixedSize(panel->Window_width * 0.008, panel->Window_height * 0.012);
    m_sourcePB_ES_listDeleteButton[currentIndex]->setIcon(QIcon("./data/image/delete.png"));
    m_sourcePB_ES_listDeleteButton[currentIndex]->setObjectName(QString::number(currentIndex));

    m_sourcePB_ES_listInfoButton.push_back(new QPushButton());
    m_sourcePB_ES_listInfoButton[currentIndex]->setCheckable(false);
    m_sourcePB_ES_listInfoButton[currentIndex]->setStyleSheet(DialogStyle::MENU_BTN13);
    m_sourcePB_ES_listInfoButton[currentIndex]->setFont(panel->font_D_BTN13);
    m_sourcePB_ES_listInfoButton[currentIndex]->setFixedSize(panel->Window_width * 0.008, panel->Window_height * 0.012);
    m_sourcePB_ES_listInfoButton[currentIndex]->setIcon(QIcon("./data/image/info.png"));
    m_sourcePB_ES_listInfoButton[currentIndex]->setObjectName(QString::number(currentIndex));

    hLayout->addWidget(label1);
    hLayout->addWidget(label2);
    hLayout->addWidget(m_sourcePB_ES_listDeleteButton[currentIndex]);
    hLayout->addWidget(m_sourcePB_ES_listInfoButton[currentIndex]);
    widget->setLayout(hLayout);

    ES_List_sourcePB_QListWidget->setItemWidget(item, widget);
    item->setSizeHint(widget->sizeHint());

    connect(m_sourcePB_ES_listDeleteButton[currentIndex], SIGNAL(clicked()), this, SLOT(slot_ParallelBeamEnergyspectrumDelete_ButtonClicked())); 
    connect(m_sourcePB_ES_listDeleteButton[currentIndex], SIGNAL(clicked()), panel, SLOT(slot_allButtonClicked()));
    
    connect(m_sourcePB_ES_listInfoButton[currentIndex], SIGNAL(clicked()), this, SLOT(slot_ParallelBeamEnergyspectrumInfo_ButtonClicked())); 
    connect(m_sourcePB_ES_listInfoButton[currentIndex], SIGNAL(clicked()), panel, SLOT(slot_allButtonClicked()));

    // 데이터 저장
    std::map<int, QString> tmp_map;
    tmp_map[0] = sourcePB_EnergyspectrumFileNameText->placeholderText(); // Full path
    tmp_map[1] = m_lineEditParallelBeamIntensity_EnergySpectrum->text();
    sourcePB_ES_info.push_back(tmp_map);
    
    ESList_sourcePB_MakingIndex++;
}

// 8. List Delete (NEW - Replacing EP Logic)
void ParallelBeamWidget::slot_ParallelBeamEnergyspectrumDelete_ButtonClicked()
{
    QPushButton* buttonSender = qobject_cast<QPushButton*>(sender());
    int index = buttonSender->objectName().toInt();

    int row = ES_List_sourcePB_QListWidget->row(ES_List_sourcePB_QListWidget->item(index));
    QListWidgetItem* toRemove = ES_List_sourcePB_QListWidget->takeItem(row);
    delete toRemove;

    if (index >= 0 && index < sourcePB_ES_info.size()) {
        sourcePB_ES_info.erase(sourcePB_ES_info.begin() + index);
        m_sourcePB_ES_listDeleteButton.erase(m_sourcePB_ES_listDeleteButton.begin() + index);
        m_sourcePB_ES_listInfoButton.erase(m_sourcePB_ES_listInfoButton.begin() + index);
    }

    // 인덱스 재정렬
    for (int i = 0; i < m_sourcePB_ES_listDeleteButton.size(); ++i) {
        m_sourcePB_ES_listDeleteButton[i]->setObjectName(QString::number(i));
        m_sourcePB_ES_listInfoButton[i]->setObjectName(QString::number(i));
    }
    ESList_sourcePB_MakingIndex--;
}

// 9. List Info (NEW - Empty)
void ParallelBeamWidget::slot_ParallelBeamEnergyspectrumInfo_ButtonClicked()
{
    // Info 다이얼로그 로직 필요시 추가
}
//////////////////////////////////////////////////////
SourceGeometryWidget* SourceGeometryWidgetFactory::CreateSourceGeometry(const std::string& name)
{
	SourceGeometryWidget* widget = Q_NULLPTR;

	if (name == "BroadBeam") 
	{
		widget = new BroadBeamWidget();
	}
	else if (name == "ExternalPoint") 
	{
		widget = new ExternalPointWidget();
	}
	else if (name == "FloorDisk") 
	{
		widget = new FloorDiskWidget();
	}
	else if (name == "ObjectVolume") 
	{
		widget = new ObjectVolumeWidget();
	}
	else if (name == "PhaseSpace") 
	{
		widget = new PhaseSpaceWidget();
	}
	else if (name == "HotParticle") 
	{
		widget = new HotParticleWidget();
	}
	else if (name == "ConeBeam") 
	{
		widget = new ConeBeamWidget();
	}
	else if (name == "RoomContamination") 
	{
		widget = new RoomContaminationWidget();
	}
	else if (name == "EnvironmentalContamination") 
	{
		widget = new EnvironmentalContaminationWidget();
	}
	else if (name == "ParallelBeam") 
	{
		widget = new ParallelBeamWidget();
	}


	return widget;
}