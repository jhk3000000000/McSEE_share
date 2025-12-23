#include "Manager_Calculation.h"
#include "ETHuman3DApp.h"        // theApp 접근
#include "FunctionPanelRight.h"  // pRt 접근
#include "ProcessInformationPanel.h" // pInf 접근
#include "RealTimeResultPanel.h"     // pRslt 접근
#include "ETQVTKWidget.h"
#include "SimpleLogger.h"

#include "PhantomObjects.h"
#include "SourceObjects.h"

Manager_Calculation::Manager_Calculation(QObject *parent)
    : QObject(parent)
{
    // 타이머 초기화
    m_timer_LocalInit = new QTimer(this);
    connect(m_timer_LocalInit, &QTimer::timeout, this, &Manager_Calculation::DataInitialization_Local);

    m_timer_LocalCheck = new QTimer(this);
    connect(m_timer_LocalCheck, &QTimer::timeout, this, &Manager_Calculation::checkProcessInfo_Local);
    
    // ... 서버 타이머도 동일하게 연결 ...
    m_timer_ServerInit = new QTimer(this);
    connect(m_timer_ServerInit, &QTimer::timeout, this, &Manager_Calculation::DataInitialization_Server);

    m_timer_ServerCheck = new QTimer(this);
    connect(m_timer_ServerCheck, &QTimer::timeout, this, &Manager_Calculation::checkProcessInfo_Server);

	m_timer_uiRunning = new QTimer(this);
    connect(m_timer_uiRunning, &QTimer::timeout, this, &Manager_Calculation::uiRunning);
}

Manager_Calculation::~Manager_Calculation()
{
    if (serverConnect) {
        serverConnect->disconnectServer();
        delete serverConnect;
    }
}

// =========================================================
// [Local Calculation Implementation]
// =========================================================

void Manager_Calculation::Calculate_Local()
{
    // [기존 FunctionPanelRight::Calculate_Local 내용 이동]
    // UI 관련 변수는 theApp.pRt->... 로 접근

    // 1. 타임스탬프 및 파일명 설정
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
	timeStamp = std::to_string(tm.tm_year + 1900) + "-" + std::to_string(tm.tm_mon + 1) + "-" + std::to_string(tm.tm_mday)
		+ "_" + std::to_string(tm.tm_hour) + "h" + std::to_string(tm.tm_min) + "m" + std::to_string(tm.tm_sec) + "s";
    // ... (날짜 생성 로직) ...
    
    // theApp.pRt->m_CalculationSetting_ResultFileName 접근 및 수정
    if (theApp.pRt->m_CalculationSetting_ResultFileName == "") 
        theApp.pRt->m_CalculationSetting_ResultFileName = QString::fromStdString(timeStamp);

    // 2. 상태 설정
    theApp.pRt->b_IsCalculationExecuted = true;
    theApp.pRt->b_IsRealtimeCalculation = true;

    theApp.dbsendFilesClear(); // Clear exist dbsend files

    // 3. UI 패널 전환 (GUI 함수 호출)
    theApp.pRt->SetOutputPanelMode(); 
    theApp.UpdatePhantom_ActorHighlighted(theApp.pRt->OutputPhantomSelectedIndex);

    // 4. 그래프 생성 (GUI 함수 호출)
    // [중요] 그래프를 만드는 건 GUI의 역할이므로, 매니저는 GUI에게 "만들어라"라고 명령만 내림

    theApp.pRt->PhantomList_button_generate();

	// Create Organ-Dose bar graph - Organ/eff. dose calculation (Real time)
	theApp.pRt->Create_Dose_BarGraphWidgets(theApp.pRt->OrganDose_BarGraph_QVBoxLayout);

	// Create Personal dose equivalent bar graph - Personal dose equivalent calculation (Real time)
	theApp.pRt->Create_Dosimeter_BarGraphWidgets(theApp.pRt->DosimeterDose_BarGraph_QVBoxLayout);

	// Create H_AK bar graph - Ambient dose equivalent & Air kerma calculation (After completed)
	theApp.pRt->Create_H_AK_BarGraphWidgets(theApp.pRt->H_AK_BarGraph_QVBoxLayout); // Bar graph는 선택된 phantom에 무관하게 모든 phantom 및 사용자지정 sphere에 대한 값이 나옴

	// Create HP skin dose depth distribution line graph - HP skin dose depth dstribution calculation (After completed)
	theApp.pRt->Create_Skindose_LineGraphWidgets(theApp.pRt->SkinDepthDose_LineGraph_QVBoxLayout); // HP는 단일 팬텀에 대해서만 계산

	// Initialize realtime Time-Dose graph	
	for (int organID = 0; organID < theApp.pRt->OrganNameList.size(); organID++)
	{
		theApp.pRslt->Create_OrganDose_RealtimeLineGraphWidgets(theApp.pRslt->Layout_OrganDose_RealtimeLineGraph_QVBoxLayout, organID);
	}
	for (int dosimID = 0; dosimID < theApp.pRt->m_Dosimeter_Maximum_Count; dosimID++) // 선량계 최대 10개
	{
		theApp.pRslt->Create_Dosimeter_RealtimeLineGraphWidgets(theApp.pRslt->Layout_Dosimeter_RealtimeLineGraph_QVBoxLayout, dosimID);
	}
	//////////////////////////////////
	//std::string file_name_HP = "./Phase-space/HP_LocalSkinDoseDistribution.txt";
	//std::ifstream ifs_HP(file_name_HP);
	//std::stringstream ss_SkinDoseFile_HP;
	//ss_SkinDoseFile_HP << ifs_HP.rdbuf(); // Read the entire file into the stringstream
	//ifs_HP.close();
	//std::string file_name_Dose = "./Phase-space/SkinDistResult";
	//std::ifstream ifs_skinDose(file_name_Dose);
	//std::stringstream ss_SkinDoseFile;
	//ss_SkinDoseFile << ifs_skinDose.rdbuf(); // Read the entire file into the stringstream
	//ifs_skinDose.close();	
	////
	//theApp.SkinLayerGeneration();
	//theApp.ReadSkinDoseData(ss_SkinDoseFile);
	//theApp.ReadSkinDoseData_HP(ss_SkinDoseFile_HP);
	//Update_SkinDose_HP_LineGraphWidgets();
	theApp.pRslt->ForcedGraphUpdate_H_AK_Dose();

    // 5. 초기화 타이머 시작
    theApp.pInf->ProgressLabel->setText("Data initialization...");
    m_timer_LocalInit->start(100);
}

void Manager_Calculation::DataInitialization_Local()
{
    m_timer_LocalInit->stop();
	// QFont font;
	// font.setFamily("Arial");    // Font family
	// font.setPointSize(theApp.pRt->FontSizeScaling(10));      // Font size
	// font.setWeight(QFont::Bold);   // Font weight
	// QMessageBox MsgBox;
	// MsgBox.setWindowFlag(Qt::WindowStaysOnTopHint);
	// MsgBox.setFont(font);

	// MsgBox.setText("Initialization...\nPlease wait!!! (~ few min)");
	// MsgBox.setStandardButtons(QMessageBox::NoButton);
	// MsgBox.show();
	// QCoreApplication::processEvents();

	// // 의복, 피부 layer 생성	
	// theApp.phantomObjects->SkinLayerGeneration(); 
	// MsgBox.hide();
	// MsgBox.close();
	// QCoreApplication::processEvents();
// ---------------------------------------------------------
    // 1. 첫 번째 로딩창 띄우기
    // ---------------------------------------------------------
    // [수정] 직접 띄우지 않고 신호 전송
    emit requestShowProgress("Initialization...\nPlease wait!!! (~ few min)");

    // [기존 로직] 무거운 작업 수행
    theApp.phantomObjects->SkinLayerGeneration(); 
    
    // [수정] 작업 끝났으니 숨기기 신호 전송
    emit requestHideProgress();

	if (theApp.pRt->m_comboBoxSourceGeometry->currentIndex() == 4) //Phase space 선원 (PS) 선원항 파일 생성
	{
		int sec_phsp = std::ceil((std::ceil((double)theApp.pRt->PhaseSpaceFileRow * 0.000001) * 10));
        // 텍스트만 만들어서 신호로 보냄
        QString msg = "phspSource Generating...\nPlease wait!!! (~" + QString::number(sec_phsp) + "sec)";
        
        // [수정] 신호 전송
        emit requestShowProgress(msg);
        
        // [기존 로직] 무거운 작업 수행
        theApp.Generate_PSFdata();

        // [수정] 숨기기 신호 전송
        emit requestHideProgress();
		// MsgBox.setText("phspSource Generating...\nPlease wait!!! (~" + QString::number(sec_phsp) + "sec)");
		// MsgBox.setStandardButtons(QMessageBox::NoButton);  // No buttons
		// MsgBox.show();
		// QCoreApplication::processEvents();
		// theApp.Generate_PSFdata();
		// MsgBox.hide();
		// MsgBox.close();
		// QCoreApplication::processEvents();
	}

	// 결과 폴더 생성
	std::string result_folder_name = theApp.pRt->m_CalculationSetting_ResultFileName.toStdString();
	QString baseName = QString::fromStdString(result_folder_name);

	// result 폴더 안의 최종 생성할 폴더 경로를 찾는다
	QString folderPath = QString("./result/%1").arg(baseName);
	QDir folderDir(folderPath);

	// 이미 있으면 _(1), _(2), … 를 붙여서 유니크한 이름 찾기
	if (folderDir.exists()) {
		int idx = 1;
		while (true) {
			// 예: "./result/yourName_(1)"
			QString tryPath = QString("./result/%1_(%2)")
				.arg(baseName)
				.arg(idx);
			if (!QDir(tryPath).exists()) {
				folderPath = tryPath;
				break;
			}
			++idx;
		}
		theApp.pRt->m_CalculationSetting_ResultFileName = theApp.pRt->m_CalculationSetting_ResultFileName + "_(" + QString::number(idx) + ")";
	}	
	QDir().mkpath(folderPath);// 폴더 생성

	//theApp.Make_mcsee_File_extdata_previous(folderPath + "/" + m_CalculationSetting_ResultFileName + ".extdata"); // Make extdata file
	theApp.SaveData();// Make 다른 모든 파일들 -> dbsend에 정보 출력 및 저장

	std::string Simulation_info = "Simulation_info"; //SaveData로 생성된 인풋정보
	std::string phantom_collection = "phantom_collection"; // SaveData로 생성된 각 팬텀의 정보(이름, 위치, 회전, 스케일링팩터)
	std::string list_collection = "list_collection"; // SaveData로 생성된 팬텀을 제외한 차폐체, 의복 layer 여부, 안경, 등등(외부환경, 선량당량 위치, ...)
	std::string source_macro = "source_macro"; // SaveData로 생성된 선원종류에 따른 macro 파일
	std::string calc_collection = "calc_collection"; // SaveData로 생성된 장기별 target error 파일
	std::string PSFdata = "PSFdata"; // phase space 입자 정보 파일
	std::string vis_mac = "vis.mac"; // 가시화용 고정 파일
	std::string command = "\"./Ext_lib/McSEE_Docal.exe\"";

	QString tmpdataPath = folderPath + "/tmpdata";
	QDir().mkpath(tmpdataPath);
	// tmpdata 폴더에 복사
	QString sourcePath = "./data/dbsend/";
	QStringList fileNames = { QString::fromStdString(Simulation_info), QString::fromStdString(phantom_collection), QString::fromStdString(list_collection), QString::fromStdString(source_macro), QString::fromStdString(calc_collection), QString::fromStdString(vis_mac) };
	if (theApp.pRt->m_comboBoxSourceGeometry->currentIndex() == 4) fileNames.append(QString::fromStdString(PSFdata));

	for (const QString &fileName : fileNames) {
		QFile::remove(tmpdataPath + "/" + fileName); // Overwrite any existing file
		QFile::copy(sourcePath + fileName, tmpdataPath + "/" + fileName); // Copy file
	}

	// 사면체화 실행
    /*
    MsgBox.setText("Tetrahedralization...\nPlease wait!!! (~10 sec)");
    MsgBox.setStandardButtons(QMessageBox::NoButton);
    MsgBox.show();
    QCoreApplication::processEvents();
    */
    emit requestShowProgress("Tetrahedralization...\nPlease wait!!! (~10 sec)");
	// 의복 사면체화 생성
	theApp.WearableTetrahedralization(); 
	theApp.Generate_MaterialFile();
	// 사면체 팬텀/의복류 transformation (local mode에서만 Geant4가 아닌 GUI 모듈에서 실행)
	for (int reset_phantomID = 0; reset_phantomID < theApp.pRt->m_Phantom_SequenceVector.size(); reset_phantomID++)
	{
		if (theApp.pRt->m_Phantom_MainInfo[theApp.pRt->m_Phantom_SequenceVector[reset_phantomID]][theApp.pRt->E_PHANTOMMAININFO_CATEGORY] == theApp.pRt->E_PHANTOMCATEGORY_AIR) continue; // Air sphere면 node 파일 생성 X
		theApp.TranslatePhantomTetFile(reset_phantomID); // Sequnce vector ID가 아닌, 있는 개수대로 0,1,2,3 ... 으로 들어가야 함
		theApp.TranslateClothingTetFile(reset_phantomID); // Sequnce vector ID가 아닌, 있는 개수대로 0,1,2,3 ... 으로 들어가야 함
	}
	
    // [수정 전] MsgBox 끄기
    /*
    MsgBox.hide();
    MsgBox.close();
    QCoreApplication::processEvents();
    */
	emit requestHideProgress();

	// 피부선량 누적용 bin 파일 초기화
	theApp.pRt->SkinDose_BinaryFilePath = "./result/" + theApp.pRt->m_CalculationSetting_ResultFileName + "/tmpdata/total_skindose.bin";
	SkinDoseBinaryUtils::initializeBinaryFile(theApp.pRt->SkinDose_BinaryFilePath);

	// CheckProcessInfo에 사용되는 변수들 초기화
	if (theApp.pRt->TargetOrganError_Map.size() == 0) // TargetTargetOrganError_Map[Organ 이름] = 에러; 에러가 1.0인건 저장 안 되어 있음; size 0 = no target organ error
	{
		theApp.pRt->b_IsTargetErrorAbsent = true;
	}
	if (theApp.pRt->m_CalculationSetting_TargetNPS == "") theApp.pRt->b_IsTargetNPSAbsent = true;

	// Local mode 실행 
	STARTUPINFOA startupInfo = { sizeof(startupInfo) };
	startupInfo.dwFlags = STARTF_USESHOWWINDOW;

	//startupInfo.wShowWindow = SW_HIDE; // Hide the window

	startupInfo.wShowWindow = SW_SHOW; // Show the window

	// Launch the command prompt
	PROCESS_INFORMATION processInfo;
	BOOL success = CreateProcessA(
		nullptr,                    // lpApplicationName (null to use command)
		const_cast<char*>(command.c_str()),  // lpCommandLine
		nullptr,                    // lpProcessAttributes
		nullptr,                    // lpThreadAttributes
		FALSE,                      // bInheritHandles
		0,                          // dwCreationFlags
		nullptr,                    // lpEnvironment
		nullptr,                    // lpCurrentDirectory
		&startupInfo,               // lpStartupInfo
		&processInfo                // lpProcessInformation
	);
    m_timer_LocalCheck->start(theApp.pRt->Time_Interval * 1000);
}

void Manager_Calculation::checkProcessInfo_Local()
{
    // [기존 FunctionPanelRight::checkProcessInfo_Local 내용 이동]
	try {
	QString tmpdata_path = "./result/" + theApp.pRt->m_CalculationSetting_ResultFileName + "/tmpdata/";
	QString ProgressQstr; // 상태 표시 메시지
	theApp.pRt->RunningIndex++; // 진행에 따라 올라가는 index
	int nowRunningIndex = theApp.pRt->RunningIndex % 3 + 1; // ... 나타내는 것 (팬텀 하나니까 길어야 5분)

	// Update UI signal
	QString outFile_UIsignal_temp = tmpdata_path + "UIsignal.tmp";
	QString outFile_UIsignal = tmpdata_path + "UIsignal";
	QFile tempFile(outFile_UIsignal_temp);
	if (!tempFile.open(QIODevice::WriteOnly)) {
		LOG_ERROR(QString("Failed to open UIsignal temp file: %1").arg(outFile_UIsignal_temp));
		return;
	}
	QIODeviceOStream temp_stream(&tempFile);  // 주소 전달
	temp_stream << theApp.pRt->RunningIndex << std::flush;
	tempFile.close();
	// 원자적 이동 (Windows에서는 MoveFile, Linux에서는 rename)
	QFile::remove(outFile_UIsignal);  // 기존 파일 삭제
	if (QFile::rename(outFile_UIsignal_temp, outFile_UIsignal)) {
	}
	else {
		LOG_ERROR("Failed to rename UIsignal temp file");
	}

	// Update processInfo
	QFile file_ram(tmpdata_path + "RAM_Threads");
	if (file_ram.exists())
	{
		if (file_ram.open(QIODevice::ReadOnly)) {
			std::string nOfThreads;
			std::string RAM_MB;
			std::string PageFault;
			QIODeviceIStream ifp_ram_thread(&file_ram);  // 주소 전달
			ifp_ram_thread >> RAM_MB >> nOfThreads >> PageFault;
			file_ram.close();

			try {
				double RAM_GB = std::stod(RAM_MB) / 1024.;
				std::ostringstream oss;
				oss << std::fixed << std::setprecision(2) << RAM_GB;
				std::string RAM_str = oss.str();
				//RAM usage
				theApp.pInf->labelMemUsage->setText(QString::fromStdString(RAM_str) + " GB");
				//Thread usage
				theApp.pInf->labelThreadUsage->setText(QString::fromStdString(nOfThreads));
				//Page Fault
				if (PageFault == "Yes") PageFault = "";
				else PageFault = "";
				theApp.pInf->labelPageFault->setText(QString::fromStdString(PageFault));
			}
			catch (const std::exception& e) {
				LOG_ERROR(QString("Error parsing RAM/Thread data: %1").arg(e.what()));
			}
		}
		else {
			LOG_ERROR("Failed to open RAM_Threads file for reading");
		}
	}
	else {
		LOG_ERROR("RAM_Threads file does not exist");
	}
	
	// Running
	QString organDoseFile = tmpdata_path + "_UpdatedOrganDoseResult";
	bool organDoseExists = QFile::exists(organDoseFile);

	if (!QFile::exists(tmpdata_path + "_UpdatedOrganDoseResult")) // 초기화중: 최초 선량계산 파일 존재하지 않음
	{
		// 상태 업데이트
		ProgressQstr = "MC Simulation Initilization";
		for (int i = 0; i < nowRunningIndex; i++)
		{
			ProgressQstr = ProgressQstr + "."; // MC Simuilation Running...
		}
		theApp.pInf->labelNowTimeRemaining->setText("Calculating");
		theApp.pInf->ProgressLabel->setText(ProgressQstr);
	}
	else // 초기화 후: 최초 선량계산 파일 존재
	{
		// 상태 업데이트
		ProgressQstr = "MC Simulation Running";
		for (int i = 0; i < nowRunningIndex; i++)
		{
			ProgressQstr = ProgressQstr + "."; // MC Simuilation Running...
		}
		theApp.pInf->ProgressLabel->setText(ProgressQstr);
		
		// 새로 계산된 선량결과 파일인지 확인
		bool is_new_OrganDose_resultfile = false;
		std::string file_name_OrganDose = tmpdata_path.toStdString() + "_UpdatedOrganDoseResult";
		{			
			FILETIME current_modified_time = theApp.getLastWriteTime(file_name_OrganDose);	// 새 파일인지 생성 시간으로 비교		

			if ((OrgandoseFile_PRE_ModifiedTime.dwLowDateTime == 0 && OrgandoseFile_PRE_ModifiedTime.dwHighDateTime == 0) // 처음 선량계산이거나
				|| !(current_modified_time.dwLowDateTime == OrgandoseFile_PRE_ModifiedTime.dwLowDateTime && current_modified_time.dwHighDateTime == OrgandoseFile_PRE_ModifiedTime.dwHighDateTime)) // 이전 파일과 다르면		
			{
				is_new_OrganDose_resultfile = true;
			}
			else {
			}
			OrgandoseFile_PRE_ModifiedTime = current_modified_time; // 현재 파일 시간정보를 할당
		}

		// 그래프용 선량결과 업데이트 -> target number_of_event/시간 계산때문에 필요
		QFile inputFile_OrganDose(QString::fromStdString(file_name_OrganDose));  // 스택 할당
		if (!inputFile_OrganDose.open(QIODevice::ReadOnly)) {
			LOG_ERROR(QString("Failed to open OrganDose file: %1").arg(file_name_OrganDose.c_str()));
			return;
		}

		qint64 fileSize = inputFile_OrganDose.size();
		// 파일 크기 제한 (메모리 부족으로 인한 크래시 방지)
		if (fileSize > 100 * 1024 * 1024) { // 100MB 제한
			LOG_ERROR(QString("OrganDose file is too large: %1 bytes, skipping processing").arg(fileSize));
			inputFile_OrganDose.close();
			return;
		}

		QIODeviceIStream ifs_ss(&inputFile_OrganDose);
		std::stringstream ss_organ_dose_result;
		ss_organ_dose_result << ifs_ss.rdbuf();
		inputFile_OrganDose.close();

		std::string tmpStr;
		std::string OrganStr;
		std::string DoseStr;
		std::string errorStr;
		double error;
		std::uint64_t number_of_event;
		double initTimeSec;
		double runTimeSec;
		double totalTimeSec;
		double TargetFactor = 1;
		std::vector<double> TargerFactor_tmpVec;

		for (auto phantomIdx : theApp.pRt->m_Phantom_SequenceVector)
		{
			if (theApp.pRt->m_Phantom_MainInfo[phantomIdx][FunctionPanelRight::E_PHANTOMMAININFO_DUMMY] == FunctionPanelRight::E_PHANTOMDUMMY_YES) continue;

			// ss_organ_dose_result의 복사본 생성 (위치 보존을 위해)
			std::stringstream ss_copy(ss_organ_dose_result.str());
			int lineCount = 0;
			const int MAX_LINES = 50000; // 무한루프 방지

			while (ss_copy >> tmpStr && lineCount < MAX_LINES) // 무한루프 방지 조건
			{
				lineCount++; // 라인 카운터 증가
				if (tmpStr == "Number_of_event_processed:")
				{
					ss_copy >> number_of_event;
				}
				if (tmpStr == "Initialization_time:")
				{
					ss_copy >> initTimeSec;
				}
				if (tmpStr == "Run_time:")
				{
					ss_copy >> runTimeSec;
				}
				if (tmpStr == "Relative_error")
				{
					// Update time-dose value and calculate progress information factor
					for (int organID = 0; organID < theApp.pRt->OrganNameList.size(); organID++)
					{
						ss_copy >> OrganStr; // Organ Name
						ss_copy >> DoseStr; // Dose
						ss_copy >> errorStr; // Relative Error    
						try {
							theApp.pRt->m_graphInfo_OrganDose_map[phantomIdx][organID] = (std::make_pair(QString::fromStdString(OrganStr), std::stod(DoseStr))); // 각 그래프 데이터 덮어 씌움
							theApp.pRt->m_graphInfo_TimeDose_map[phantomIdx][organID].push_back(std::make_pair(runTimeSec + initTimeSec, std::stod(DoseStr))); // 각 그래프 데이터 추가해감
							theApp.pRt->m_graphInfo_Error_map[phantomIdx][organID] = std::stod(errorStr);

							// Calculate progress information factor
							if (theApp.pRt->TargetOrganError_Map.find(OrganStr) != theApp.pRt->TargetOrganError_Map.end())
							{
								error = std::stod(errorStr);
								TargerFactor_tmpVec.push_back((error / theApp.pRt->TargetOrganError_Map[OrganStr]) * (error / theApp.pRt->TargetOrganError_Map[OrganStr]));
							}
						}
						catch (const std::exception& e) {
							LOG_ERROR(QString("Error parsing organ %1 data: %2").arg(QString::fromStdString(OrganStr)).arg(e.what()));
						}
					}
					break;
				}
			}
			if (lineCount >= MAX_LINES) { // 무한루프 방지 경고
				LOG_WARNING(QString("Reached maximum line limit (%1) while parsing organ dose data for phantom %2").arg(MAX_LINES).arg(phantomIdx));
			}
		}

		totalTimeSec = runTimeSec + initTimeSec; // 전체 running time은 init_time + run_time
		theApp.pRt->nEvent = number_of_event; // 전역변수 nEvent에 선량결과 파일에서 읽은 number_of_event 할당
		theApp.pRslt->flg_GraphUpdate = true; // 계산진행 중에 실시간 그래프 정보 업데이트 ON -> 팬텀버튼 눌러서 그래프 강제 업데이트 하여 볼 수 있도록

		// 실시간 그래프 업데이트
		theApp.pRslt->RealtimeGraphUpdate_TimeDose();
		theApp.pRslt->RealtimeGraphUpdate_OrganDose();

		// Update output 패널 값 업데이트	
		if (is_new_OrganDose_resultfile)
		{
			std::stringstream ss_value(ss_organ_dose_result.str());
			theApp.ResultLoad_OrganDose_OutputPanel(ss_value);
		}

		// NPS, Runtime 상태 업데이트
		theApp.pInf->labelTransportedParticles->setText(QLocale(QLocale::English).toString(static_cast<qulonglong>(theApp.pRt->nEvent))); // qulonglong is uint_64
		{
			int days = totalTimeSec / 86400;
			int rem_sec = (int)totalTimeSec % 86400;
			int hours = rem_sec / 3600;
			rem_sec %= 3600;
			int minutes = rem_sec / 60;
			int seconds = rem_sec % 60;
			QString hours_Qstr;
			QString minutes_Qstr;
			QString seconds_Qstr;
			if (hours < 10) hours_Qstr = "0" + QString::number(hours);
			else hours_Qstr = QString::number(hours);
			if (minutes < 10) minutes_Qstr = "0" + QString::number(minutes);
			else minutes_Qstr = QString::number(minutes);
			if (seconds < 10) seconds_Qstr = "0" + QString::number(seconds);
			else seconds_Qstr = QString::number(seconds);

			theApp.pInf->labelRunTime->setText(QString::number(days) + "d " + hours_Qstr + ":" + minutes_Qstr + ":" + seconds_Qstr);
		}


		// Progress bar 값 및 상태 업데이트 -> Targert factor 계산		
		double TargetNPS = theApp.pRt->m_CalculationSetting_TargetNPS.toDouble();

		if (theApp.pRt->b_IsTargetErrorAbsent == true && theApp.pRt->b_IsTargetNPSAbsent == true) // 타겟 에러나 number_of_event가 둘다 설정되지 않았을 때 continuous Run
		{
			theApp.pInf->labelNowTimeRemaining->setText("Not estimated");
			theApp.pInf->bar->setFormat("Continuous Run");			
		}
		else
		{
			if (theApp.pRt->b_IsTargetErrorAbsent == false && theApp.pRt->b_IsTargetNPSAbsent == true) // Target error만 있을 때
			{
				double maxVal = std::numeric_limits<double>::lowest();
				for (const double& val : TargerFactor_tmpVec) { // 여러 팬텀 중 target factor가 가장 높은 값(가장 오래 걸리는 값)
					if (val > maxVal) {
						maxVal = val;
					}
				}
				TargetFactor = maxVal;
			}
			if (theApp.pRt->b_IsTargetErrorAbsent == true && theApp.pRt->b_IsTargetNPSAbsent == false) // Target number_of_event만 있을 때
			{
				TargetFactor = TargetNPS / theApp.pRt->nEvent;
			}
			if (theApp.pRt->b_IsTargetErrorAbsent == false && theApp.pRt->b_IsTargetNPSAbsent == false) // target error / number_of_event 모두 설정되어 있을 때
			{
				double maxVal = std::numeric_limits<double>::lowest();
				for (const double& val : TargerFactor_tmpVec) { // 여러 팬텀 중 target factor가 가장 높은 값(가장 오래 걸리는 값)
					if (val > maxVal) {
						maxVal = val;
					}
				}
				if (maxVal <= TargetNPS / theApp.pRt->nEvent) TargetFactor = maxVal;
				else TargetFactor = TargetNPS / theApp.pRt->nEvent;
			}
			// Set progrress bar value(%) and time remaining
			double EstimatedTotalTime = totalTimeSec * TargetFactor; 
			double RemainingSec = EstimatedTotalTime - totalTimeSec;
			int days = RemainingSec / 86400;
			int rem_sec = (int)RemainingSec % 86400;
			int hours = rem_sec / 3600;
			rem_sec %= 3600;
			int minutes = rem_sec / 60;
			int seconds = rem_sec % 60;
			QString hours_Qstr;
			QString minutes_Qstr;
			QString seconds_Qstr;
			if (hours < 10) hours_Qstr = "0" + QString::number(hours);
			else hours_Qstr = QString::number(hours);
			if (minutes < 10) minutes_Qstr = "0" + QString::number(minutes);
			else minutes_Qstr = QString::number(minutes);
			if (seconds < 10) seconds_Qstr = "0" + QString::number(seconds);
			else seconds_Qstr = QString::number(seconds);

			theApp.pInf->labelNowTimeRemaining->setText(QString::number(days) + "d " + hours_Qstr + ":" + minutes_Qstr + ":" + seconds_Qstr);
			theApp.pRt->ProgressBarValue = totalTimeSec / EstimatedTotalTime * 100;
			theApp.pRt->ProgressFactor = 1. / EstimatedTotalTime * 100. * theApp.pRt->Time_Interval;
			theApp.pInf->bar->setValue(theApp.pRt->ProgressBarValue);
		}		
				
		// 새로 계산된 피부선량분포 파일인지 확인
		bool is_new_SkinDose_resultfile = false;
		std::string file_name_SkinDose = tmpdata_path.toStdString() + "_UpdatedSkinDoseResult";
		{
			FILETIME current_modified_time = theApp.getLastWriteTime(file_name_SkinDose);	// 새 파일인지 생성 시간으로 비교		

			if ((SkindoseFile_PRE_ModifiedTime.dwLowDateTime == 0 && SkindoseFile_PRE_ModifiedTime.dwHighDateTime == 0) // 처음 선량계산이거나
				|| !(current_modified_time.dwLowDateTime == SkindoseFile_PRE_ModifiedTime.dwLowDateTime && current_modified_time.dwHighDateTime == SkindoseFile_PRE_ModifiedTime.dwHighDateTime)) // 이전 파일과 다르면		
			{
				is_new_SkinDose_resultfile = true;
			}
			else {
			}
			SkindoseFile_PRE_ModifiedTime = current_modified_time; // 현재 파일 시간정보를 할당
		}
		//피부선량분포 가시화
		if (is_new_SkinDose_resultfile)
		{
			// 바이너리 파일에 데이터 추가 (함수 직접 호출)
			SkinDoseBinaryUtils::appendFromTextFile(
				QString::fromStdString(file_name_SkinDose),
				theApp.pRt->SkinDose_BinaryFilePath
			);

			QFile inputFile_SkinDose(QString::fromStdString(file_name_SkinDose));  // 스택 할당
			if (inputFile_SkinDose.open(QIODevice::ReadOnly)) {
				QIODeviceIStream ifs_skinDose(&inputFile_SkinDose);  // 주소 전달
				std::stringstream ss_SkinDoseFile;
				ss_SkinDoseFile << ifs_skinDose.rdbuf(); // Read the entire file into the stringstream
				inputFile_SkinDose.close();
				theApp.ReadSkinDoseData(ss_SkinDoseFile);				
				theApp.pRt->m_skinDoseVisualizationButton->setEnabled(true);
			}
			else {
				LOG_ERROR(QString("Failed to open SkinDose file: %1").arg(file_name_SkinDose.c_str()));
			}
		}

		// 디버깅용 속도 로그
		//LOG_DEBUG("Time(s): " + QString::number(totalTimeSec) + " Particles: " + QString::number(nEvent));
	}

	// UI에서 stop 했을때
	QFile stop_file_UI(QString(tmpdata_path + "StopSignal_UI"));
	if (stop_file_UI.exists())
	{
		// 상태 업데이트
		ProgressQstr = "MC Simulation will be ended soon";

		for (int i = 0; i < nowRunningIndex; i++)
		{
			ProgressQstr = ProgressQstr + "."; // MC Simuilation Running...
		}
		theApp.pInf->ProgressLabel->setText(ProgressQstr);
	}

	// Geant4 초기화중 계산이 시작되지 않은 채 종료되었을 때
	QFile UI_stopped_file_Geant4(QString(tmpdata_path + "UIStoppedSignal_Geant4"));
	if (UI_stopped_file_Geant4.exists())
	{
		m_timer_LocalCheck->stop();
		// QFont font;
		// font.setFamily("Arial");    // Font family		
		// font.setPointSize(FontSizeScaling(8));      // Font size
		// font.setWeight(QFont::Bold);   // Font weight
		// QMessageBox MsgBox;
		// MsgBox.setWindowFlag(Qt::WindowStaysOnTopHint);
		// MsgBox.setText("Geant4 intiailization is stopped!");
		// MsgBox.setFont(font);
		// MsgBox.exec();
        emit requestShowProgress("Geant4 intiailization is stopped!");
		// 패널 모드 셋팅
		theApp.pRt->b_IsRealtimeCalculation = false;
		theApp.pRslt->flg_GraphUpdate = false;
		theApp.pInf->hide();
		theApp.pRslt->hide();
		theApp.m_pVTKWidget->runButton->hide();
		theApp.m_pVTKWidget->stopButton->hide();
		theApp.m_pVTKWidget->resetButton->show();

		LOG_INFO("UI state reset completed after Geant4 initialization stop");
	}

	// Geant4 계산 종료되었을때
	QFile stop_file_Geant4(QString(tmpdata_path + "StopSignal_Geant4"));
	if (stop_file_Geant4.exists())
	{
		LOG_INFO("Geant4 stop signal detected - calculation finished");

		m_timer_LocalCheck->stop();
		//theApp.pRslt->timer_RealtimeGraphUpdate->stop();

		// 계산 종료 메시지
		// QFont font;
		// font.setFamily("Arial");    // Font family		
		// font.setPointSize(FontSizeScaling(8));      // Font size
		// font.setWeight(QFont::Bold);   // Font weight
		// QMessageBox MsgBox;
		// MsgBox.setWindowFlag(Qt::WindowStaysOnTopHint);
		if (!QFile::exists(tmpdata_path + "_UpdatedOrganDoseResult")) // No Run but stop with stop signal
		{
			// MsgBox.setText("Geant4 intiailization is stopped!");
			// MsgBox.setFont(font);
			// MsgBox.exec();
            emit requestShowProgress("Geant4 intiailization is stopped!");
			LOG_INFO("Displayed initialization stop message");
		}
		else
		{
			// MsgBox.setText("Calculation has been finished!");
			// MsgBox.setFont(font);
			// MsgBox.exec();
            emit requestShowProgress("Calculation has been finished!");
			LOG_INFO("Displayed calculation finished message");
		}

		//1, 10cm2 피부선량 계산
		{
			// QMessageBox MsgBox;
			// MsgBox.setWindowFlag(Qt::WindowStaysOnTopHint);
			// MsgBox.setStandardButtons(QMessageBox::NoButton);
			// MsgBox.setText("1, 10cm2 skin dose calculation...\nPlease wait!!!");
			// MsgBox.setStandardButtons(QMessageBox::NoButton);  // No buttons
			// MsgBox.show();
			// QCoreApplication::processEvents();
            emit requestShowProgress("1, 10cm2 skin dose calculation...\nPlease wait!!!");
			auto result = SkinDoseBinaryUtils::readAllData(theApp.pRt->SkinDose_BinaryFilePath);
			qint64 final_nps = result.first.first;
			double final_conv_factor = result.first.second;
			std::vector<SkinDoseData>& totalDoseData = result.second;
			theApp.ReadSkinDoseData_1_10cm2(totalDoseData, final_nps, final_conv_factor);
			theApp.pRt->Update_SkinDose_1_10cm2_GraphWidgets();

			// MsgBox.hide();
			// MsgBox.close();
            emit requestHideProgress();
		}		

		// 국부 피부선량 깊이분포 계산 -> Imported 팬텀 구분 해야 함.
		if (theApp.pRt->SettingSkinDepthModeInfo == 1) // 현재 서버모드 radiobutton 클릭이 skin dose depth 계산임
		{
			// QMessageBox MsgBox;
			// MsgBox.setWindowFlag(Qt::WindowStaysOnTopHint);
			// MsgBox.setStandardButtons(QMessageBox::NoButton);
			// MsgBox.setText("HP calculation...\nPlease wait!!!");
			// MsgBox.setStandardButtons(QMessageBox::NoButton);  // No buttons
			// MsgBox.show();
			// QCoreApplication::processEvents();
            emit requestShowProgress("HP calculation...\nPlease wait!!!");
			std::string file_name_HP = tmpdata_path.toStdString() + "SkinDistResult";
			QFile inputFile_HP(QString::fromStdString(file_name_HP));  // 스택 할당
			if (inputFile_HP.open(QIODevice::ReadOnly)) {
				QIODeviceIStream ifs_HP(&inputFile_HP);  // 주소 전달
				std::stringstream ss_skinDoseFile_HP;
				ss_skinDoseFile_HP << ifs_HP.rdbuf(); // Read the entire file into the stringstream			
				inputFile_HP.close();
				theApp.ReadSkinDoseData_HP(ss_skinDoseFile_HP);
				theApp.pRt->Update_SkinDose_HP_LineGraphWidgets();
			}
			else {
				LOG_ERROR(QString("Failed to open HP file: %1").arg(file_name_HP.c_str()));
			}

			// MsgBox.hide();
			// MsgBox.close();
            emit requestHideProgress();
		}

		// 패널 모드 셋팅
		theApp.pRt->b_IsRealtimeCalculation = false;
		theApp.pRslt->flg_GraphUpdate = false;
		theApp.pInf->hide();
		theApp.pRslt->hide();
		theApp.m_pVTKWidget->runButton->hide();
		theApp.m_pVTKWidget->stopButton->hide();
		theApp.m_pVTKWidget->resetButton->show();

		LOG_INFO("Final UI state reset completed after calculation finish");
	}

	//  그래프 포인트 쌓임에 따라 메모리 정리 및 상태 보고 (주기적)
	static int callCounter = 0;
	static int memoryCleanupCounter = 0;
	callCounter++;

	// 매 50번째 호출마다 적응적 메모리 정리
	if (++memoryCleanupCounter % 100 == 0) {		

		const int MAX_GRAPH_POINTS = 100000;  // 벡터당 최대 100000개 데이터 유지
		const int CLEANUP_THRESHOLD = 200000; // 200000개 초과시 정리 시작(100000개로 줄임)

		int totalProcessed = 0;
		int totalReduced = 0;

		for (auto& phantomPair : theApp.pRt->m_graphInfo_TimeDose_map) {
			for (auto& organPair : phantomPair.second) {
				auto& timeDoseVector = organPair.second;

				// 벡터 크기가 임계값을 초과한 경우에만 정리
				if (timeDoseVector.size() > CLEANUP_THRESHOLD) {
					int originalSize = timeDoseVector.size();

					// 적응적 다운샘플링 적용
					auto downsampledData = performAdaptiveDownsampling(timeDoseVector, MAX_GRAPH_POINTS);

					// 원본 데이터 교체
					timeDoseVector = std::move(downsampledData);

					int reducedCount = originalSize - timeDoseVector.size();
					totalProcessed++;
					totalReduced += reducedCount;

					// 처음 3개 벡터만 상세 로깅
					if (totalProcessed <= 3) {
						LOG_DEBUG(QString("Phantom %1, Organ %2: %3 → %4 points (reduced %5)")
							.arg(phantomPair.first)
							.arg(organPair.first)
							.arg(originalSize)
							.arg(timeDoseVector.size())
							.arg(reducedCount));
					}
				}
			}
		}

		if (totalProcessed > 0) {
			LOG_INFO(QString("Memory cleanup: %1 vectors processed, %2 points reduced")
				.arg(totalProcessed).arg(totalReduced));
		}
	}

	}

	catch (const std::exception& e) {
		LOG_ERROR(QString("Standard exception in checkProcessInfo_Local: %1").arg(e.what()));
		handleProcessError("Standard exception occurred");
	}
	catch (...) {
		LOG_ERROR("Unknown exception in checkProcessInfo_Local");
		handleProcessError("Unknown exception occurred");
	}
}

void Manager_Calculation::DoseCalculation_StopAndSave()
{
    // [기존 FunctionPanelRight::DoseCalculation_StopAndSave 내용 이동]
    // Stop 파일 생성 로직
    QString tmpdata_path = "./result/" + theApp.pRt->m_CalculationSetting_ResultFileName + "/tmpdata/";
    std::ofstream ofs_stop((tmpdata_path + "StopSignal_UI").toStdString());
    ofs_stop << "Stop";
    ofs_stop.close();
}

// =========================================================
// [Local Calculation Implementation]
// =========================================================

void Manager_Calculation::ServerInitialization()
{
	serverConnect = new DBServerConnect;
	serverConnect->connectServer();
}

void Manager_Calculation::Calculate_Server()
{
	// time_t t = time(NULL);
	// struct tm tm = *localtime(&t);
	// timeStamp = std::to_string(tm.tm_year + 1900) + "-" + std::to_string(tm.tm_mon + 1) + "-" + std::to_string(tm.tm_mday)
	// 	+ "-" + std::to_string(tm.tm_hour) + "h" + std::to_string(tm.tm_min) + "m" + std::to_string(tm.tm_sec) + "s";
	// ResultFileName_forServer = "./result/" + timeStamp + ".txt"; // For server mode
	// if (m_CalculationSetting_ResultFileName == "") m_CalculationSetting_ResultFileName = QString::fromStdString(timeStamp + ".txt");
	// ResultFileName_notused = m_CalculationSetting_ResultFileName.toStdString(); // For local mode
	// m_lineEditResultFileName->setText(QString::fromStdString(ResultFileName_notused)); // Output panel에 띄우기

	// // Calculation Start
	// b_IsCalculationExecuted = true;
	// b_IsRealtimeCalculation = true;

	// theApp.dbsendFilesClear(); // Clear exist dbsend macro files

	// SetOutputPanelMode();
	// OutputPhantomButton[0]->show();
	// OutputPhantomButton[0]->setChecked(true);
	// theApp.UpdatePhantom_ActorHighlighted(0);
	// for (int i = 1; i < m_Phantom_SequenceVector.size() + 1; i++)
	// {
	// 	OutputPhantomButton[i]->show();
	// 	OutputPhantomButton[i]->setChecked(false);
	// }
	// // Calculate 누르면 source geometry 변경 불가
	// m_comboBoxSourceGeometry->setDisabled(true);

	// // Determine unit
	// if (m_comboBoxSourceGeometry->currentIndex() == 0) DoseUnit_QString = "pGy*cm2";
	// if (m_comboBoxSourceGeometry->currentIndex() == 1) DoseUnit_QString = "pGy/h";
	// if (m_comboBoxSourceGeometry->currentIndex() == 2) DoseUnit_QString = "pGy/h";
	// if (m_comboBoxSourceGeometry->currentIndex() == 3) DoseUnit_QString = "pGy/h";
	// if (m_comboBoxSourceGeometry->currentIndex() == 4) DoseUnit_QString = "pGy/nps";
	// if (m_comboBoxSourceGeometry->currentIndex() == 5) DoseUnit_QString = "pGy/h";
	// if (m_comboBoxSourceGeometry->currentIndex() == 6) DoseUnit_QString = "pGy/h";
	// if (m_comboBoxSourceGeometry->currentIndex() == 7) DoseUnit_QString = "pGy/h";
	// if (m_comboBoxSourceGeometry->currentIndex() == 7) DoseUnit_QString = "pGy/h";

	// // Create Organ-Dose Bar graph
	// if (m_comboBoxSourceGeometry->currentIndex() == 0) Create_Dose_BarGraphWidgets(OrganDose_BarGraph_QVBoxLayout);
	// if (m_comboBoxSourceGeometry->currentIndex() == 1) Create_Dose_BarGraphWidgets(layoutInnerIn_graphEP);
	// if (m_comboBoxSourceGeometry->currentIndex() == 2) Create_Dose_BarGraphWidgets(layoutInnerIn_graphFD);
	// if (m_comboBoxSourceGeometry->currentIndex() == 3) Create_Dose_BarGraphWidgets(layoutInnerIn_graphOV);
	// if (m_comboBoxSourceGeometry->currentIndex() == 4) Create_Dose_BarGraphWidgets(layoutInnerIn_graphPS);
	// if (m_comboBoxSourceGeometry->currentIndex() == 5) Create_Dose_BarGraphWidgets(layoutInnerIn_graphHP);
	// if (m_comboBoxSourceGeometry->currentIndex() == 6);
	// if (m_comboBoxSourceGeometry->currentIndex() == 7);

	// // Initialize realtime Time-Dose graph
	// int phantomIdx = 0; // 최초 0번 팬텀으로 고정
	// for (int organID = 0; organID < OrganNameList.size(); organID++) 
	// {
	// 	theApp.pRslt->Create_OrganDose_RealtimeLineGraphWidgets(theApp.pRslt->Layout_OrganDose_RealtimeLineGraph_QVBoxLayout, organID);
	// }
	// for (int dosimID = 0; dosimID < m_Dosimeter_Maximum_Count; dosimID++) // 선량계 최대 10개
	// {
	// 	theApp.pRslt->Create_OrganDose_RealtimeLineGraphWidgets(theApp.pRslt->Layout_OrganDose_RealtimeLineGraph_QVBoxLayout, dosimID);
	// }

	// theApp.pInf->ProgressLabel->setText("Data initialization...");

	// timer_DataInitialization_Server->start(100); // 시간 margin 주기 위함

	return;
}

void Manager_Calculation::uiRunning()
{
	serverConnect->uiRunningSignalUpdate();
}
void Manager_Calculation::DataInitialization_Server()
{
// 	timer_DataInitialization_Server->stop();
	
// 	QFont font;
// 	font.setFamily("Arial");    // Font family
// 	font.setPointSize(FontSizeScaling(10));      // Font size
// 	font.setWeight(QFont::Bold);   // Font weight

// 	QMessageBox MsgBox;
// 	MsgBox.setWindowFlag(Qt::WindowStaysOnTopHint);
// 	MsgBox.setStandardButtons(QMessageBox::NoButton);
// 	MsgBox.setFont(font);	
// 	MsgBox.setText("Tetrahedralization...\nPlease wait!!! (~ few min)");
// 	MsgBox.setStandardButtons(QMessageBox::NoButton);  // No buttons
// 	MsgBox.show();
// 	QCoreApplication::processEvents();
// //theApp.WearableTetrahedralization(); // 의복 생성	
// 	theApp.phantomObjects->SkinLayerGeneration(); //SKINLAYERGENERATION_skindose
// 	MsgBox.hide();
// 	MsgBox.close();
// 	QCoreApplication::processEvents();

// 	if (m_comboBoxSourceGeometry->currentIndex() == 4)
// 	{
// 		int sec_phsp = std::ceil((std::ceil((double)PhaseSpaceFileRow * 0.000001) * 10));
// 		MsgBox.setText("phspSource Generating...\nPlease wait!!! (~" + QString::number(sec_phsp) + "sec)");
// 		MsgBox.setStandardButtons(QMessageBox::NoButton);  // No buttons
// 		MsgBox.show();
// 		QCoreApplication::processEvents();
// 	}
// 	theApp.SaveData(); // dbsend 정보 출력 및 저장

// 	if (m_comboBoxSourceGeometry->currentIndex() == 4)
// 	{
// 		MsgBox.hide();
// 		MsgBox.close();
// 	}
// 	QCoreApplication::processEvents();

// 	//Generate Err data for calculation of process infomration from generated Err file
// 	TargetOrganError_Map.clear();
// 	std::ifstream errInfo("./data/dbsend/err");
// 	std::string tmpStr;
// 	double tmpDbl;
// 	for (int i = 0; i < OrganNameList.size(); i++)
// 	{
// 		errInfo >> tmpStr >> tmpDbl;
// 		if (tmpDbl < 1) TargetOrganError_Map[tmpStr] = tmpDbl;
// 	}
// 	if (TargetOrganError_Map.size() == 0)
// 	{
// 		b_IsTargetErrorAbsent = true;
// 	}
// 	errInfo.close();	

// 	std::string Simulation_info = "./data/dbsend/Simulation_info"; //SaveData로 생성된 인풋정보
// 	std::string phantom_collection = "./data/dbsend/phantom_collection"; // SaveData로 생성된 각 팬텀의 정보(이름, 위치, 회전, 스케일링팩터)
// 	std::string list_collection = "./data/dbsend/list_collection"; // SaveData로 생성된 팬텀을 제외한 차폐체, 의복 layer 여부, 안경, 등등(외부환경, 선량당량 위치, ...)
// 	std::string macro; // SaveData로 생성된 선원종류에 따른 macro 파일
// 	std::string err = "./data/dbsend/err"; // SaveData로 생성된 장기별 target error 파일
// 	std::string Activity = "./data/dbsend/activity"; // UI상에서 입력한 activity 값과 총 yield가 들어있는 파일
// 	std::string phspFile = "./data/dbsend/phspFile"; // phase space 입자 정보 파일
// 	std::string command; // Command line with arguments
// 	std::string layer_ele = "./data/dbsend/wearable_send/layer.ele";
// 	std::string layer_node = "./data/dbsend/wearable_send/layer.node";
// 	std::string layer_material = "./data/dbsend/wearable_send/layer.material";
// 	std::string glasses_ele = "./data/dbsend/wearable_send/glasses.ele";
// 	std::string glasses_node = "./data/dbsend/wearable_send/glasses.node";
// 	std::string glasses_material = "./data/dbsend/wearable_send/glasses.material";
// 	std::string layer_dosimeter_ele = "./data/dbsend/wearable_send/layer_dosimeter.ele";
// 	std::string layer_dosimeter_node = "./data/dbsend/wearable_send/layer_dosimeter.node";
// 	std::string layer_dosimeter_material = "./data/dbsend/wearable_send/layer_dosimeter.material";
	
// 	if (m_comboBoxSourceGeometry->currentText() == "Broad beam") {
// 		macro = "./data/dbsend/broad_beam_source";
// 		command = "-i " + Simulation_info + " -o " + ResultFileName_forServer + " -p " + phantom_collection + " -l " + list_collection + " -m " + macro + " -e " + err;
// 	}
// 	else if (m_comboBoxSourceGeometry->currentText() == "External point") {
// 		macro = "./data/dbsend/external_point_source";
// 		command = "-i " + Simulation_info + " -o " + ResultFileName_forServer + " -p " + phantom_collection + " -l " + list_collection + " -m " + macro + " -g " + Activity + " -e " + err;
// 	}
// 	else if (m_comboBoxSourceGeometry->currentText() == "Hot particle point") {
// 		macro = "./data/dbsend/hot_particle_point_source";
// 		command = "-i " + Simulation_info + " -o " + ResultFileName_forServer + " -p " + phantom_collection + " -l " + list_collection + " -m " + macro + " -g " + Activity + " -e " + err;
// 	}
// 	else if (m_comboBoxSourceGeometry->currentText() == "Floor disk") {
// 		macro = "./data/dbsend/floor_disk_source";
// 		command = "-i " + Simulation_info + " -o " + ResultFileName_forServer + " -p " + phantom_collection + " -l " + list_collection + " -m " + macro + " -g " + Activity + " -e " + err;
// 	}
// 	else if (m_comboBoxSourceGeometry->currentText() == "Phase space") {
// 		macro = "./data/dbsend/phase_space_source";	
// 		command = "-i " + Simulation_info + " -o " + ResultFileName_forServer + " -p " + phantom_collection + " -l " + list_collection + " -m " + macro + " -e " + err;
// 	}
// 	else if (m_comboBoxSourceGeometry->currentText() == "Object volume") {
// 		macro = "./data/dbsend/object_volume_source";
// 		command = "-i " + Simulation_info + " -o " + ResultFileName_forServer + " -p " + phantom_collection + " -l " + list_collection + " -m " + macro + " -g " + Activity + " -e " + err;
// 	}

// 	// MariaDB DB connect
// 	ServerInitialization();

// 	//serverConnect->createTable(); // 초기에는 CreateTable 써서 초기 table 만들고 이후에는 주석처리 -> 초기 데이터로 쓰일 수 있음.
		
// 	serverConnect->sendRowInitialData(command);	

// 	timer_uiRunning->start(3500);

// 	if (m_comboBoxSourceGeometry->currentText() == "Phase space")
// 	{
// 		//int sec_phspUploading = std::ceil((theApp.pRt->PSF_FileSize / 70000000 / 5.0)) * 5;
// 		//MsgBox.setText("phspSource Uploading...\nPlease wait!!! (~" + QString::number(sec_phspUploading) + "sec)");
// 		MsgBox.setText("phspSource Uploading...\n0% completed\nPlease wait!!! (~ few min)");
// 		MsgBox.setStandardButtons(QMessageBox::NoButton);  // No buttons
// 		MsgBox.show();
// 		QCoreApplication::processEvents();

// 		std::vector<std::string> chunks = serverConnect->readInChunks(phspFile);
// 		for (int i = 0; i < chunks.size(); i++)
// 		{
// 			serverConnect->sendDBFileContent_HugeData(i, chunks[i]); 

// 			MsgBox.setText("phspSource Uploading...\n" + QString::number((double)i/chunks.size() * 100) + "% completed\nPlease wait!!! (~ few min)");
// 			MsgBox.setStandardButtons(QMessageBox::NoButton);  // No buttons
// 			MsgBox.show();
// 			QCoreApplication::processEvents();
// 		}
// 		MsgBox.hide();
// 		MsgBox.close();
// 		QCoreApplication::processEvents();
// 	}
// 	serverConnect->sendDBFileContent(6, Simulation_info);
// 	serverConnect->sendDBFileContent(7, phantom_collection);
// 	serverConnect->sendDBFileContent(8, list_collection);
// 	serverConnect->sendDBFileContent(9, macro);
// 	serverConnect->sendDBFileContent(10, err);
// 	serverConnect->sendDBFileContent(11, Activity);
// 	serverConnect->sendDBFileContent(12, layer_ele);
// 	serverConnect->sendDBFileContent(13, layer_node);
// 	serverConnect->sendDBFileContent(14, layer_material);
// 	serverConnect->sendDBFileContent(15, glasses_ele);
// 	serverConnect->sendDBFileContent(16, glasses_node);
// 	serverConnect->sendDBFileContent(17, glasses_material);
// 	serverConnect->sendDBFileContent(18, layer_dosimeter_ele);
// 	serverConnect->sendDBFileContent(19, layer_dosimeter_node);
// 	serverConnect->sendDBFileContent(20, layer_dosimeter_material);
// 	std::string Flag = "0";
// 	serverConnect->sendDBFileContent(1, Flag);

// 	timer_checkProcessInfo_Server->start(1500); // ProgressFactor와 동기화 필요
}
void Manager_Calculation::checkProcessInfo_Server()
{
	// //Check Geant4 simulation status
	// std::string StatusFlag;	
	// serverConnect->CheckGeant4Status(StatusFlag);

	// QString ProgressQstr; 
	// RunningIndex++;
	// int nowRunningIndex = RunningIndex % 3 + 1;

	// if (StatusFlag == "0") // No data received by McSEE_Commander
	// {
	// 	ProgressQstr = "Wait for server connection"; // Do not execute another UI during this state
	// 	for (int i = 0; i < nowRunningIndex; i++)
	// 	{
	// 		ProgressQstr = ProgressQstr + "."; // MC Simuilation Running...
	// 	}
	// 	theApp.pInf->ProgressLabel->setText(ProgressQstr);
	// 	return; 
	// }
	// if (StatusFlag == "1") // Running
	// {
	// 	//Check ServerPC Info
	// 	std::string nOfThreads;
	// 	std::string RAM_MB;		
	// 	serverConnect->recvServerPCInfo(nOfThreads, RAM_MB);
	// 	if (nOfThreads == "NULL" || RAM_MB == "NULL") return;
	// 	RAM_MB = std::to_string(std::stod(RAM_MB) / 1024.0);
	// 	std::string RAM_GB;
	// 	std::stringstream ss(RAM_GB);
	// 	ss << std::fixed << std::setprecision(1) << std::stod(RAM_MB);
	// 	RAM_GB = ss.str();
	// 	//RAM usage
	// 	theApp.pInf->labelMemUsage->setText(QString::fromStdString(RAM_GB) + " GB");
	// 	//Thread usage
	// 	theApp.pInf->labelThreadUsage->setText(QString::fromStdString(nOfThreads));
	// }
	// else if(StatusFlag == "2") // Geant4 completed
	// {
	// 	timer_checkProcessInfo_Server->stop();
	// 	timer_uiRunning->stop();

	// 	b_IsRealtimeCalculation = false;
	// 	theApp.pInf->hide();
	// 	theApp.pRslt->hide();

	// 	QFont font;
	// 	font.setFamily("Arial");    // Font family		
	// 	font.setPointSize(FontSizeScaling(8));      // Font size
	// 	font.setWeight(QFont::Bold);   // Font weight

	// 	QMessageBox MsgBox;
	// 	MsgBox.setWindowFlag(Qt::WindowStaysOnTopHint);
	// 	std::string Cumulative;
	// 	int Run_ID;
	// 	serverConnect->recvCumulativeData(Run_ID, Cumulative);
	// 	if (Run_ID == 0) // No Run but stop with stop signal
	// 	{
	// 		MsgBox.setText("Geant4 intiailization stopped!");
	// 		MsgBox.setFont(font);
	// 		MsgBox.exec();
	// 		theApp.pInf->StopSaveButton->setDisabled(true); // Stop and Save button disable
	// 		return;
	// 	}
	// 	MsgBox.setText("Calculation has been finished!");
	// 	MsgBox.setFont(font);
	// 	MsgBox.exec();

	// 	// Get result from DBserver
	// 	QDir dir;
	// 	if (!dir.exists(resultDirectoryQstr)) {
	// 		bool success = dir.mkpath(resultDirectoryQstr);
	// 		if (!success) {
	// 			theApp.SetMessageBox("Failed to create directory!");
	// 			return;
	// 		}
	// 	}
	// 	std::string ResultFilePathStr = resultDirectoryQstr.toStdString() + "/" + ResultFileName_notused;
	// 	std::ofstream ofs_result(ResultFilePathStr);
	// 	std::string ResultStr;
	// 	serverConnect->recvResultData(ResultStr);
	// 	ofs_result << ResultStr;
	// 	ofs_result.close();

	// 	// Disconnect DBserver
	// 	serverConnect->disconnectServer();

	// 	//theApp.LoadOutputData(QString::fromStdString(ResultFilePathStr));

	// 	theApp.pInf->StopSaveButton->setDisabled(true); // Stop and Save button disable
	// 	return;
	// }
	// else if (StatusFlag == "3") // Geant4 user stop
	// {		
	// 	ProgressQstr = "MC Simulation will end soon";
	// 	for (int i = 0; i < nowRunningIndex; i++)
	// 	{
	// 		ProgressQstr = ProgressQstr + "."; // MC Simuilation Running...
	// 	}
	// 	theApp.pInf->ProgressLabel->setText(ProgressQstr);
	// 	return;
	// }
	// else if (StatusFlag == "4") // Geant4 Target NPS stop
	// {
	// 	ProgressQstr = "MC Simulation will end soon";
	// 	for (int i = 0; i < nowRunningIndex; i++)
	// 	{
	// 		ProgressQstr = ProgressQstr + "."; // MC Simuilation Running...
	// 	}
	// 	theApp.pInf->ProgressLabel->setText(ProgressQstr);

	// 	theApp.pInf->StopSaveButton->setDisabled(true); // NPS stop 이 반영되는 동안 멈출수 없도록
	// //Check ServerPC Info
	// 	std::string nOfThreads;
	// 	std::string RAM_MB;
	// 	serverConnect->recvServerPCInfo(nOfThreads, RAM_MB);
	// 	if (nOfThreads == "NULL" || RAM_MB == "NULL") return;
	// 	RAM_MB = std::to_string(std::stod(RAM_MB) / 1024.0);
	// 	std::string RAM_GB;
	// 	std::stringstream ss(RAM_GB);
	// 	ss << std::fixed << std::setprecision(1) << std::stod(RAM_MB);
	// 	RAM_GB = ss.str();
	// 	//RAM usage
	// 	theApp.pInf->labelMemUsage->setText(QString::fromStdString(RAM_GB) + " GB");
	// 	//Thread usage
	// 	theApp.pInf->labelThreadUsage->setText(QString::fromStdString(nOfThreads));
	// 	return;
	// }
	// else if (StatusFlag == "9") // Geant4 error
	// {
	// 	timer_checkProcessInfo_Server->stop();
	// 	theApp.SetMessageBox("Geant4 error!!! with " + theApp.m_3DHumanData_MultiplePhantom[0].PhantomName);

	// 	theApp.pInf->StopSaveButton->setDisabled(true); // Stop and Save button disable
	// 	return;
	// }

	// // Update using cumulative result
	// // Get cumulative result from DBserver
	// //std::string AirKermaResult;
	// //serverConnect->recvAirKermaResultData(AirKermaResult);
	// //if (AirKermaResult != "NULL" && b_IsAirKermaCalculated == true) // update 되었으면서 첫번째 계산일 때
	// //{
	// //	
	// //	// show airkerma output
	// //	std::stringstream ss_outputAK(AirKermaResult);
	// //  theApp.LoadOutputData_AirKerma(ss_outputAK);
	// //  b_IsAirKermaCalculated = false; // 계산 완료
	// //}


	// std::string Cumulative;
	// int Run_ID;
	// serverConnect->recvCumulativeData(Run_ID, Cumulative);
	// if (Cumulative == PreCumulative || Cumulative == "NULL") // Cumulative가 업데이트 되지 않았을때
	// {
	// 	if (nEvent == 0) // No Run, In initialization
	// 	{
	// 		ProgressQstr = "MC Simulation Initilization";
	// 		for (int i = 0; i < nowRunningIndex; i++)
	// 		{
	// 			ProgressQstr = ProgressQstr + "."; // MC Simuilation Running...
	// 		}
	// 		theApp.pInf->labelNowTimeRemaining->setText("Calculating");
	// 	}
	// 	else // Running 중에 업데이트가 되지 않은 경우 
	// 	{
	// 		ProgressQstr = "MC Simulation Running";
	// 		for (int i = 0; i < nowRunningIndex; i++)
	// 		{
	// 			ProgressQstr = ProgressQstr + "."; // MC Simuilation Running...
	// 		}
	// 	}
	// 	theApp.pInf->ProgressLabel->setText(ProgressQstr);

	// 	if (b_IsTargetErrorAbsent == true && b_IsTargetNPSAbsent == true) // No target error
	// 	{
	// 		theApp.pInf->bar->setFormat("Continuous Run");
	// 		theApp.pInf->labelNowTimeRemaining->setText("Not estimated");
	// 	}
	// 	else // Common process
	// 	{
	// 		theApp.pInf->bar->setValue(ProgressBarValue + (ProgressFactor * 0.8)); // margin 0.8
	// 		ProgressBarValue = ProgressBarValue + ProgressFactor;
	// 	}

	// 	return;
	// }
	// else if (Cumulative != PreCumulative) // Cumulative가 업데이트 되었을때
}

// internal helper
void Manager_Calculation::handleProcessError(const QString& errorMessage)
{
	LOG_ERROR(QString("Handling process error: %1").arg(errorMessage));

	try {
		// 타이머 정지
		if (m_timer_LocalCheck && m_timer_LocalCheck->isActive()) {
			m_timer_LocalCheck->stop();
			LOG_INFO("Timer stopped due to error");
		}

		// UI 상태 초기화
		if (theApp.pInf) {
			theApp.pInf->ProgressLabel->setText("Error occurred");
			theApp.pInf->labelNowTimeRemaining->setText("Error");
			LOG_DEBUG("UI error state set");
		}

		// 에러 카운터 증가
		static int errorCount = 0;
		errorCount++;
		LOG_ERROR(QString("Total error count: %1").arg(errorCount));

		// 연속 에러가 너무 많으면 추가 조치
		if (errorCount > 10) {
			LOG_ERROR("Too many errors detected - consider stopping application");
		}

	}
	catch (...) {
		LOG_ERROR("Exception occurred during error handling cleanup");
	}
}


std::vector<std::pair<double, double>> Manager_Calculation::performAdaptiveDownsampling(const std::vector<std::pair<double, double>>& originalData, int targetSize) {

	// 목표 크기보다 작거나 같으면 그대로 반환
	if (originalData.size() <= targetSize) {
		return originalData;
	}

	if (targetSize <= 2) {
		// 목표 크기가 2 이하면 첫 번째와 마지막만 반환
		std::vector<std::pair<double, double>> result;
		result.push_back(originalData.front());
		if (targetSize == 2 && originalData.size() > 1) {
			result.push_back(originalData.back());
		}
		return result;
	}

	// 각 포인트의 중요도 계산
	std::vector<double> importanceScores;
	importanceScores.reserve(originalData.size());

	// 첫 번째와 마지막 포인트는 항상 최고 중요도
	importanceScores.push_back(1000000.0);

	for (size_t i = 1; i < originalData.size() - 1; i++) {
		double importance = 0.0;

		// 이전 구간 변화율 계산
		double prevTimeDiff = originalData[i].first - originalData[i - 1].first;
		double prevValueDiff = std::abs(originalData[i].second - originalData[i - 1].second);
		double prevChangeRate = (prevTimeDiff > 0) ? prevValueDiff / prevTimeDiff : 0;

		// 다음 구간 변화율 계산
		double nextTimeDiff = originalData[i + 1].first - originalData[i].first;
		double nextValueDiff = std::abs(originalData[i + 1].second - originalData[i].second);
		double nextChangeRate = (nextTimeDiff > 0) ? nextValueDiff / nextTimeDiff : 0;

		// 곡률 계산 (방향 변화)
		double curvature = 0.0;
		if (prevTimeDiff > 0 && nextTimeDiff > 0) {
			double prevSlope = (originalData[i].second - originalData[i - 1].second) / prevTimeDiff;
			double nextSlope = (originalData[i + 1].second - originalData[i].second) / nextTimeDiff;
			curvature = std::abs(nextSlope - prevSlope);
		}

		// 종합 중요도 = 변화율(70%) + 곡률(30%)
		importance = (prevChangeRate + nextChangeRate) * 0.7 + curvature * 0.3;
		importanceScores.push_back(importance);
	}

	importanceScores.push_back(1000000.0); // 마지막 포인트

	// 중요도와 인덱스를 함께 정렬
	std::vector<std::pair<double, int>> importanceIndex;
	importanceIndex.reserve(originalData.size());

	for (size_t i = 0; i < originalData.size(); i++) {
		importanceIndex.push_back({ importanceScores[i], static_cast<int>(i) });
	}

	// 중요도 순으로 정렬 (높은 순)
	std::sort(importanceIndex.begin(), importanceIndex.end(),
		[](const std::pair<double, int>& a, const std::pair<double, int>& b) {
			return a.first > b.first;
		});

	// 상위 targetSize개 선택
	std::vector<int> selectedIndices;
	selectedIndices.reserve(targetSize);

	int maxSelections = getmin(targetSize, static_cast<int>(importanceIndex.size()));
	for (int i = 0; i < maxSelections; i++) {
		selectedIndices.push_back(importanceIndex[i].second);
	}

	// 시간 순서로 재정렬
	std::sort(selectedIndices.begin(), selectedIndices.end());

	// 결과 생성
	std::vector<std::pair<double, double>> result;
	result.reserve(selectedIndices.size());

	for (int idx : selectedIndices) {
		result.push_back(originalData[idx]);
	}

	return result;
}

void Manager_Calculation::timer_LocalInit_Stop()
{
	if(m_timer_LocalInit)
	{
		m_timer_LocalInit->stop();
	}
}
void Manager_Calculation::timer_LocalCheck_Stop()
{
	if(m_timer_LocalCheck)
	{
	m_timer_LocalCheck->stop();
	}
}
void Manager_Calculation::timer_ServerInit_Stop()
{
	if(m_timer_ServerInit)
	{
		m_timer_ServerInit->stop();
	}
}
void Manager_Calculation::timer_ServerCheck_Stop()
{
	if(m_timer_ServerCheck)
	{
		m_timer_ServerCheck->stop();
	}
}
void Manager_Calculation::timer_uiRunning_Stop()
{
	if(m_timer_uiRunning)
	{
		m_timer_uiRunning->stop();
	}
}
// ... performAdaptiveDownsampling 및 Server 관련 함수들도 동일한 방식으로 이동 ...
// ServerInitialization, Calculate_Server, DataInitialization_Server, checkProcessInfo_Server 구현