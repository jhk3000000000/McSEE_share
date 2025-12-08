#include "pch.h"
#include <algorithm>
#include "DBServerConnect.h"

#include "ETHuman3DApp.h"
#include "FunctionPanelRight.h"

DBServerConnect::DBServerConnect()
{
	conn = mysql_init(NULL);
	if (conn == NULL) {
		theApp.SetMessageBox("Server connection initialization failed!");
		return;
	}
	// Flag-> -1: Just initialized, 0: All input entered, 1: Geant4 running, 2: Geant4 complete sucessfully, 3: Geant4 stop signal executed, 9: Geant4 error
	col_name_send = { "Index", "Flag", "uiRunSignal", "computer_ID", "server_ID","command",
		"Simulation_info","phantom_collection","list_collection","macro","err","activity","layer.ele","layer.node","layer.material","glasses.ele","glasses.node","glasses.material","layer_dosimeter.ele","layer_dosimeter.node","layer_dosimeter.material" };
	col_name_recv = { "Index", "computer_ID", "run_ID", "cumulative", "result", "Threads", "RAM" };
	col_name_phsp = { "phspFile_1", "phspFile_2", "phspFile_3", "phspFile_4", "phspFile_5", "phspFile_6", "phspFile_7", "phspFile_8", "phspFile_9", "phspFile_10",
		"phspFile_11", "phspFile_12", "phspFile_13", "phspFile_14", "phspFile_15", "phspFile_16", "phspFile_17", "phspFile_18", "phspFile_19", "phspFile_20",
		"phspFile_21", "phspFile_22", "phspFile_23", "phspFile_24", "phspFile_25", "phspFile_26", "phspFile_27", "phspFile_28", "phspFile_29", "phspFile_30",
		"phspFile_31", "phspFile_32", "phspFile_33", "phspFile_34", "phspFile_35", "phspFile_36", "phspFile_37", "phspFile_38", "phspFile_39", "phspFile_40",
		"phspFile_41", "phspFile_42", "phspFile_43", "phspFile_44", "phspFile_45", "phspFile_46", "phspFile_47", "phspFile_48", "phspFile_49", "phspFile_50",
		"phspFile_51", "phspFile_52", "phspFile_53", "phspFile_54", "phspFile_55", "phspFile_56", "phspFile_57", "phspFile_58", "phspFile_59", "phspFile_60",
		"phspFile_61", "phspFile_62", "phspFile_63", "phspFile_64", "phspFile_65", "phspFile_66", "phspFile_67", "phspFile_68", "phspFile_69", "phspFile_70",
		"phspFile_71", "phspFile_72", "phspFile_73", "phspFile_74", "phspFile_75", "phspFile_76", "phspFile_77", "phspFile_78", "phspFile_79", "phspFile_80",
		"phspFile_81", "phspFile_82", "phspFile_83", "phspFile_84", "phspFile_85", "phspFile_86", "phspFile_87", "phspFile_88", "phspFile_89", "phspFile_90",
		"phspFile_91", "phspFile_92", "phspFile_93", "phspFile_94", "phspFile_95", "phspFile_96", "phspFile_97", "phspFile_98", "phspFile_99", "phspFile_100",
		"phspFile_101", "phspFile_102", "phspFile_103", "phspFile_104", "phspFile_105", "phspFile_106", "phspFile_107", "phspFile_108", "phspFile_109", "phspFile_110",
		"phspFile_111", "phspFile_112", "phspFile_113", "phspFile_114", "phspFile_115", "phspFile_116", "phspFile_117", "phspFile_118", "phspFile_119", "phspFile_120",
		"phspFile_121", "phspFile_122", "phspFile_123", "phspFile_124", "phspFile_125", "phspFile_126", "phspFile_127", "phspFile_128", "phspFile_129", "phspFile_130",
		"phspFile_131", "phspFile_132", "phspFile_133", "phspFile_134", "phspFile_135", "phspFile_136", "phspFile_137", "phspFile_138", "phspFile_139", "phspFile_140",
		"phspFile_141", "phspFile_142", "phspFile_143", "phspFile_144", "phspFile_145", "phspFile_146", "phspFile_147", "phspFile_148", "phspFile_149", "phspFile_150",
		"phspFile_151", "phspFile_152", "phspFile_153", "phspFile_154", "phspFile_155", "phspFile_156", "phspFile_157", "phspFile_158", "phspFile_159", "phspFile_160",
		"phspFile_161", "phspFile_162", "phspFile_163", "phspFile_164", "phspFile_165", "phspFile_166", "phspFile_167", "phspFile_168", "phspFile_169", "phspFile_170",
		"phspFile_171", "phspFile_172", "phspFile_173", "phspFile_174", "phspFile_175", "phspFile_176", "phspFile_177", "phspFile_178", "phspFile_179", "phspFile_180",
		"phspFile_181", "phspFile_182", "phspFile_183", "phspFile_184", "phspFile_185", "phspFile_186", "phspFile_187", "phspFile_188", "phspFile_189", "phspFile_190",
		"phspFile_191", "phspFile_192", "phspFile_193", "phspFile_194", "phspFile_195", "phspFile_196", "phspFile_197", "phspFile_198", "phspFile_199", "phspFile_200",
		"phspFile_201", "phspFile_202", "phspFile_203", "phspFile_204", "phspFile_205", "phspFile_206", "phspFile_207", "phspFile_208", "phspFile_209", "phspFile_210",
		"phspFile_211", "phspFile_212", "phspFile_213", "phspFile_214", "phspFile_215", "phspFile_216", "phspFile_217", "phspFile_218", "phspFile_219", "phspFile_220",
		"phspFile_221", "phspFile_222", "phspFile_223", "phspFile_224", "phspFile_225", "phspFile_226", "phspFile_227", "phspFile_228", "phspFile_229", "phspFile_230",
		"phspFile_231", "phspFile_232", "phspFile_233", "phspFile_234", "phspFile_235", "phspFile_236", "phspFile_237", "phspFile_238", "phspFile_239", "phspFile_240",
		"phspFile_241", "phspFile_242", "phspFile_243", "phspFile_244", "phspFile_245", "phspFile_246", "phspFile_247", "phspFile_248", "phspFile_249", "phspFile_250",
		"phspFile_251", "phspFile_252", "phspFile_253", "phspFile_254", "phspFile_255", "phspFile_256", "phspFile_257", "phspFile_258", "phspFile_259", "phspFile_260",
		"phspFile_261", "phspFile_262", "phspFile_263", "phspFile_264", "phspFile_265", "phspFile_266", "phspFile_267", "phspFile_268", "phspFile_269", "phspFile_270",
		"phspFile_271", "phspFile_272", "phspFile_273", "phspFile_274", "phspFile_275", "phspFile_276", "phspFile_277", "phspFile_278", "phspFile_279", "phspFile_280",
		"phspFile_281", "phspFile_282", "phspFile_283", "phspFile_284", "phspFile_285", "phspFile_286", "phspFile_287", "phspFile_288", "phspFile_289", "phspFile_290",
		"phspFile_291", "phspFile_292", "phspFile_293", "phspFile_294", "phspFile_295", "phspFile_296", "phspFile_297", "phspFile_298", "phspFile_299", "phspFile_300" };
	col_name_skindose = { "skinDose_1" };

	//  ** .exe 파일 이름 설정
	//  프로젝트 우클릭
	//	->일반->대상이름 변경
	//	->링커->출력 파일->이름 변경

// McSEE_local
	//HOSTIP = "166.104.20.131";
	//USERID = "McSEE"; // UserID와 DB_NAME에 grant all previlege 해야함(MariaDB에서)
	//PASSWD = "hurellabkhi";
	//PORTID = 3306;
	//DB_NAME = "mcsee"; //Schema name
	//DB_TABLE_ui2g4 = "ui2g4";
	//DB_TABLE_g42ui = "g42ui";
	//DB_TABLE_phsp = "_phsp";
	//DB_TABLE_skindose = "_skindose";

// McSEE_NAS
	HOSTIP = "166.104.40.201";
	USERID = "McSEE_NAS"; // UserID와 DB_NAME에 grant all previlege 해야함(MariaDB에서)
	PASSWD = "!Hurellab96";
	PORTID = 3307;
	DB_NAME = "McSEE_NAS"; //Schema name
	DB_TABLE_ui2g4 = "ui2g4";
	DB_TABLE_g42ui = "g42ui";
	DB_TABLE_phsp = "_phsp";
	DB_TABLE_skindose = "_skindose";

	computerID = theApp.SetComputerID();	
}

DBServerConnect::~DBServerConnect()
{

}

void DBServerConnect::connectServer()
{
	// Connect server
	if (mysql_real_connect(conn, HOSTIP.c_str(), USERID.c_str(), PASSWD.c_str(), DB_NAME.c_str(), PORTID, NULL, 0) == NULL)
	{
		mysql_close(conn);
		theApp.SetMessageBox("Server connection to HOST failed!");
		return;
	}
	// Send result folder directory
	theApp.pRt->resultDirectoryQstr = "./result/" + QString::fromStdString(computerID);
}

void DBServerConnect::disconnectServer()
{
	// disconnect server
	mysql_close(conn);
}

void DBServerConnect::sendRowInitialData(const std::string& command)
{
	// set index
	sql = "SELECT `Index` FROM `" + DB_TABLE_ui2g4 + "` ORDER BY `Index` DESC LIMIT 1;";
	mysql_query(conn, sql.c_str());
	result = mysql_store_result(conn);
	if (result) {
		MYSQL_ROW row = mysql_fetch_row(result);
		if (row) {
			int highestIndex = atoi(row[0]);  // Convert the string result to an integer
			index = highestIndex + 1;
		}
		else {
			index = 0;  // If no rows exist, start with index 0.
		}
		mysql_free_result(result);  // Don't forget to free the result after using it!
	}
	else {
		// Handle error
	}
	//unsigned long num_rows = mysql_num_rows(result);
	////row = mysql_fetch_row(result);
	//index = num_rows - 1;

	// Insert row
	sql = "INSERT INTO `" + DB_NAME + "`.`" + DB_TABLE_ui2g4 + "` ("
		"`Index`, "
		"`Flag`, "
		"`uiRunSignal`, "
		"`computer_ID`, "
		"`server_ID`, "
		"`command`, "
		"`Simulation_info`, "
		"`phantom_collection`, "
		"`list_collection`, "
		"`macro`, "
		"`err`, "
		"`activity`, "
		"`layer.ele`, "
		"`layer.node`, "
		"`layer.material`, "
		"`glasses.ele`, "
		"`glasses.node`, "
		"`glasses.material`, "
		"`layer_dosimeter.ele`, "
		"`layer_dosimeter.node`, "
		"`layer_dosimeter.material` "
		") VALUES ("
		+ std::to_string(index) + ", "  // Index
		"-1, "  // Flag
		"0, "  // uiRunSignal
		"'" + computerID + "', "  // computer_ID
		"'NULL', "  // server_ID
		"'" + command + "', "  // command
		"'NULL', "  // Simulation_info
		"'NULL', "  // phantom_collection
		"'NULL', "  // list_collection
		"'NULL', "  // macro
		"'NULL', "  // err
		"'NULL', "  // activity
		"'NULL', "  // layer.ele
		"'NULL', "  // layer.node
		"'NULL', "  // layer.material
		"'NULL', "  // glasses.ele
		"'NULL', "  // glasses.node
		"'NULL', "  // glasses.material
		"'NULL', "  // layer_dosimeter.ele
		"'NULL', "  // layer_dosimeter.node
		"'NULL');";  // layer_dosimeter.material
	mysql_query(conn, sql.c_str());

	sql = "INSERT INTO `" + DB_NAME + "`.`" + DB_TABLE_phsp + "` ("
		"`Index`, "
		"`computer_ID`, "
		"`phspFile_1`, `phspFile_2`, `phspFile_3`, `phspFile_4`, `phspFile_5`, `phspFile_6`, `phspFile_7`, `phspFile_8`, `phspFile_9`, `phspFile_10`, "
		"`phspFile_11`, `phspFile_12`, `phspFile_13`, `phspFile_14`, `phspFile_15`, `phspFile_16`, `phspFile_17`, `phspFile_18`, `phspFile_19`, `phspFile_20`, "
		"`phspFile_21`, `phspFile_22`, `phspFile_23`, `phspFile_24`, `phspFile_25`, `phspFile_26`, `phspFile_27`, `phspFile_28`, `phspFile_29`, `phspFile_30`, "
		"`phspFile_31`, `phspFile_32`, `phspFile_33`, `phspFile_34`, `phspFile_35`, `phspFile_36`, `phspFile_37`, `phspFile_38`, `phspFile_39`, `phspFile_40`, "
		"`phspFile_41`, `phspFile_42`, `phspFile_43`, `phspFile_44`, `phspFile_45`, `phspFile_46`, `phspFile_47`, `phspFile_48`, `phspFile_49`, `phspFile_50`, "
		"`phspFile_51`, `phspFile_52`, `phspFile_53`, `phspFile_54`, `phspFile_55`, `phspFile_56`, `phspFile_57`, `phspFile_58`, `phspFile_59`, `phspFile_60`, "
		"`phspFile_61`, `phspFile_62`, `phspFile_63`, `phspFile_64`, `phspFile_65`, `phspFile_66`, `phspFile_67`, `phspFile_68`, `phspFile_69`, `phspFile_70`, "
		"`phspFile_71`, `phspFile_72`, `phspFile_73`, `phspFile_74`, `phspFile_75`, `phspFile_76`, `phspFile_77`, `phspFile_78`, `phspFile_79`, `phspFile_80`, "
		"`phspFile_81`, `phspFile_82`, `phspFile_83`, `phspFile_84`, `phspFile_85`, `phspFile_86`, `phspFile_87`, `phspFile_88`, `phspFile_89`, `phspFile_90`, "
		"`phspFile_91`, `phspFile_92`, `phspFile_93`, `phspFile_94`, `phspFile_95`, `phspFile_96`, `phspFile_97`, `phspFile_98`, `phspFile_99`, `phspFile_100`, "
		"`phspFile_101`, `phspFile_102`, `phspFile_103`, `phspFile_104`, `phspFile_105`, `phspFile_106`, `phspFile_107`, `phspFile_108`, `phspFile_109`, `phspFile_110`, "
		"`phspFile_111`, `phspFile_112`, `phspFile_113`, `phspFile_114`, `phspFile_115`, `phspFile_116`, `phspFile_117`, `phspFile_118`, `phspFile_119`, `phspFile_120`, "
		"`phspFile_121`, `phspFile_122`, `phspFile_123`, `phspFile_124`, `phspFile_125`, `phspFile_126`, `phspFile_127`, `phspFile_128`, `phspFile_129`, `phspFile_130`, "
		"`phspFile_131`, `phspFile_132`, `phspFile_133`, `phspFile_134`, `phspFile_135`, `phspFile_136`, `phspFile_137`, `phspFile_138`, `phspFile_139`, `phspFile_140`, "
		"`phspFile_141`, `phspFile_142`, `phspFile_143`, `phspFile_144`, `phspFile_145`, `phspFile_146`, `phspFile_147`, `phspFile_148`, `phspFile_149`, `phspFile_150`, "
		"`phspFile_151`, `phspFile_152`, `phspFile_153`, `phspFile_154`, `phspFile_155`, `phspFile_156`, `phspFile_157`, `phspFile_158`, `phspFile_159`, `phspFile_160`, "
		"`phspFile_161`, `phspFile_162`, `phspFile_163`, `phspFile_164`, `phspFile_165`, `phspFile_166`, `phspFile_167`, `phspFile_168`, `phspFile_169`, `phspFile_170`, "
		"`phspFile_171`, `phspFile_172`, `phspFile_173`, `phspFile_174`, `phspFile_175`, `phspFile_176`, `phspFile_177`, `phspFile_178`, `phspFile_179`, `phspFile_180`, "
		"`phspFile_181`, `phspFile_182`, `phspFile_183`, `phspFile_184`, `phspFile_185`, `phspFile_186`, `phspFile_187`, `phspFile_188`, `phspFile_189`, `phspFile_190`, "
		"`phspFile_191`, `phspFile_192`, `phspFile_193`, `phspFile_194`, `phspFile_195`, `phspFile_196`, `phspFile_197`, `phspFile_198`, `phspFile_199`, `phspFile_200`, "
		"`phspFile_201`, `phspFile_202`, `phspFile_203`, `phspFile_204`, `phspFile_205`, `phspFile_206`, `phspFile_207`, `phspFile_208`, `phspFile_209`, `phspFile_210`, "
		"`phspFile_211`, `phspFile_212`, `phspFile_213`, `phspFile_214`, `phspFile_215`, `phspFile_216`, `phspFile_217`, `phspFile_218`, `phspFile_219`, `phspFile_220`, "
		"`phspFile_221`, `phspFile_222`, `phspFile_223`, `phspFile_224`, `phspFile_225`, `phspFile_226`, `phspFile_227`, `phspFile_228`, `phspFile_229`, `phspFile_230`, "
		"`phspFile_231`, `phspFile_232`, `phspFile_233`, `phspFile_234`, `phspFile_235`, `phspFile_236`, `phspFile_237`, `phspFile_238`, `phspFile_239`, `phspFile_240`, "
		"`phspFile_241`, `phspFile_242`, `phspFile_243`, `phspFile_244`, `phspFile_245`, `phspFile_246`, `phspFile_247`, `phspFile_248`, `phspFile_249`, `phspFile_250`, "
		"`phspFile_251`, `phspFile_252`, `phspFile_253`, `phspFile_254`, `phspFile_255`, `phspFile_256`, `phspFile_257`, `phspFile_258`, `phspFile_259`, `phspFile_260`, "
		"`phspFile_261`, `phspFile_262`, `phspFile_263`, `phspFile_264`, `phspFile_265`, `phspFile_266`, `phspFile_267`, `phspFile_268`, `phspFile_269`, `phspFile_270`, "
		"`phspFile_271`, `phspFile_272`, `phspFile_273`, `phspFile_274`, `phspFile_275`, `phspFile_276`, `phspFile_277`, `phspFile_278`, `phspFile_279`, `phspFile_280`, "
		"`phspFile_281`, `phspFile_282`, `phspFile_283`, `phspFile_284`, `phspFile_285`, `phspFile_286`, `phspFile_287`, `phspFile_288`, `phspFile_289`, `phspFile_290`, "
		"`phspFile_291`, `phspFile_292`, `phspFile_293`, `phspFile_294`, `phspFile_295`, `phspFile_296`, `phspFile_297`, `phspFile_298`, `phspFile_299`, `phspFile_300` "
		") VALUES (" + std::to_string(index) + ", '" + computerID + "', "  // Index
		"'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', "
		"'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', "
		"'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', "
		"'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', "
		"'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', "
		"'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', "
		"'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', "
		"'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', "
		"'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', "
		"'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', "
		"'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', "
		"'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', "
		"'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', "
		"'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', "
		"'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', "
		"'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', "
		"'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', "
		"'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', "
		"'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', "
		"'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', "
		"'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', "
		"'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', "
		"'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', "
		"'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', "
		"'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', "
		"'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', "
		"'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', "
		"'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', "
		"'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', "
		"'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL');";
	mysql_query(conn, sql.c_str());
}

void DBServerConnect::uiRunningSignalUpdate()
{
	sql = "SELECT `uiRunSignal` FROM `" + DB_TABLE_ui2g4 + "` WHERE `computer_ID`= '" + computerID + "';";
	mysql_query(conn, sql.c_str());
	result = mysql_store_result(conn);
	row = mysql_fetch_row(result);
	std::string pre = std::string(row[0]);
	std::string uiRunningID = std::to_string(std::stoi(pre) + 1); // 1 더하기

	sql = "UPDATE `" + DB_TABLE_ui2g4 + "` SET `uiRunSignal`='" + uiRunningID + "' WHERE `computer_ID`= '" + computerID + "';";
	mysql_query(conn, sql.c_str());
}

void DBServerConnect::sendDBFileContent(int col_idx, const std::string& dbsend_file)
{
	std::ifstream ifs;
	std::stringstream ss;
	std::string file_content;

	if (col_idx == 1) file_content = dbsend_file;
	else
	{
		ifs.open(dbsend_file);
		if (!ifs.is_open()) {
			return;
		}
		ss << ifs.rdbuf();
		ifs.close();
		file_content = ss.str();
	}
	sql = "UPDATE `" + DB_NAME + "`.`" + DB_TABLE_ui2g4 + "` SET `" + col_name_send[col_idx] + "`='" + file_content +
		+"' WHERE `Index`=" + std::to_string(index) + ";";
	mysql_query(conn, sql.c_str());
}

void DBServerConnect::sendDBFileContent_HugeData(int col_idx, std::string chunk)
{	
	sql = "UPDATE `" + DB_NAME + "`.`" + DB_TABLE_phsp + "` SET `" + col_name_phsp[col_idx] + "`= '" + chunk + "' WHERE `Index`=" + std::to_string(index) + ";";
	mysql_query(conn, sql.c_str());

	// 각 column 별로 데이터 불러오기
	//if (col_idx == 12)
	//{
	//	std::ofstream ofs("cumulative_data.txt");
	//	theApp.SetMessageBox("start");

	//	sql = "SELECT `phspFile_1` FROM `" + DB_TABLE_send + "` WHERE `Index` = " + std::to_string(index) + ";";
	//	mysql_query(conn, sql.c_str());

	//	result = mysql_store_result(conn);
	//	row = mysql_fetch_row(result);
	//	theApp.SetMessageBox("1");
	//	ofs << std::string(row[0]);
	//	theApp.SetMessageBox("Written 1");

	//	sql = "SELECT `phspFile_2` FROM `" + DB_TABLE_send + "` WHERE `Index` = " + std::to_string(index) + ";";
	//	mysql_query(conn, sql.c_str());

	//	result = mysql_store_result(conn);
	//	row = mysql_fetch_row(result);
	//	theApp.SetMessageBox("2");
	//	ofs << std::string(row[0]);
	//	theApp.SetMessageBox("Written 2");

	//	ofs.close();
	//}
	

	// 한 column에서 데이터 이어 붙일 때!
	/*if (col_idx == 11) {
		sql = "UPDATE `" + DB_NAME + "`.`" + DB_TABLE_send + "` SET `" + col_name_send[col_idx] + "`= '" + chunk + "' WHERE `Index`=" + std::to_string(index) + ";";
	}
	else {
		sql = "UPDATE `" + DB_NAME + "`.`" + DB_TABLE_send + "` SET `" + col_name_send[col_idx] + "`= CONCAT(`" + col_name_send[col_idx] + "`, '" + chunk + "') WHERE `Index`=" + std::to_string(index) + ";";
	}*/
	
		
	//// 업로드 잘 되었는 지 확인	
	//while (true)
	//{
	//	sql = "SELECT LENGTH(`" + col_name_send[col_idx] + "`) AS byte_size FROM `" + DB_NAME + "`.`" + DB_TABLE_send + "` WHERE `Index` = " + std::to_string(index) + ";";

	//	mysql_query(conn, sql.c_str());
	//	result = mysql_store_result(conn);
	//	if (result == nullptr) {
	//		theApp.SetMessageBox("Result Error: " + QString::fromStdString(mysql_error(conn)));
	//		return;  // or handle the error in another appropriate way
	//	}

	//	row = mysql_fetch_row(result);
	//	if (row != nullptr) {
	//		std::string data = std::string(row[0]);
	//		double UploadedFileSize = std::stod(data); // 파일변환과정에서 줄 하나당 1byte 더 늘어나야 함, 그런데 CONCAT으로 가져오면 딱 맞음
	//		theApp.SetMessageBox(QString::number(UploadedFileSize) + " / " + QString::number(theApp.pRt->pshpFileSize));
	//		if (UploadedFileSize >= theApp.pRt->pshpFileSize * 0.999) {
	//			break; // 파일 용량 전부 업로드 됨.
	//		}
	//	}
	//	else {
	//		theApp.SetMessageBox("No rows returned or error fetching row: " + QString::fromStdString(mysql_error(conn)));
	//	}
	//	std::this_thread::sleep_for(std::chrono::seconds(1));
	//}
}

std::vector<std::string> DBServerConnect::readInChunks(const std::string &filename) {
	std::vector<std::string> chunks;
	std::ifstream ifs(filename, std::ios::binary | std::ios::ate);
	if (!ifs.is_open()) {
		std::cerr << "Failed to open the file!" << std::endl;
		return chunks;
	}

	std::ifstream::pos_type pos = ifs.tellg();
	size_t file_size = pos;
	
	for (size_t offset = 0; offset < file_size; offset += CHUNK_SIZE_phspFile) {
		ifs.seekg(offset, std::ios::beg);
		size_t buffer_size;
		if (CHUNK_SIZE_phspFile <= static_cast<size_t>(file_size - offset)) buffer_size = CHUNK_SIZE_phspFile;
		else buffer_size = static_cast<size_t>(file_size - offset);

		std::vector<char> buffer(buffer_size);

		ifs.read(&buffer[0], buffer_size);
		chunks.push_back(std::string(buffer.begin(), buffer.end()));
	}

	return chunks;
}

void DBServerConnect::sendStopSignal()
{
	sql = "UPDATE `" + DB_TABLE_ui2g4 + "` SET `Flag`=3 WHERE `computer_ID`= '" + computerID + "';";
	mysql_query(conn, sql.c_str());
}

void DBServerConnect::sendStopByNPS()
{
	sql = "UPDATE `" + DB_TABLE_ui2g4 + "` SET `Flag`=4 WHERE `computer_ID`= '" + computerID + "';";
	mysql_query(conn, sql.c_str());
}

bool DBServerConnect::readFlag()
{
	sql = "SELECT `Flag`" + DB_TABLE_ui2g4 + "` WHERE `computer_ID`= '" + computerID + "';";
	mysql_query(conn, sql.c_str());
	result = mysql_store_result(conn);
	row = mysql_fetch_row(result);
	int status = atoi(row[0]);

	if (status == 2)
		return true;
	else
		return false;
}

void DBServerConnect::CheckGeant4Status(std::string& Flag)
{
	sql = "SELECT `Flag` FROM `" + DB_TABLE_ui2g4 + "` WHERE `computer_ID`= '" + computerID + "';";
	mysql_query(conn, sql.c_str());
	result = mysql_store_result(conn);
	row = mysql_fetch_row(result);
	Flag = std::string(row[0]);
}

void DBServerConnect::recvServerPCInfo(std::string& Threads, std::string& RAM)
{
	sql = "SELECT `Threads`, `RAM` FROM `" + DB_TABLE_g42ui + "` WHERE `computer_ID`= '" + computerID + "';";
	mysql_query(conn, sql.c_str());
	result = mysql_store_result(conn);

	unsigned long num_rows = mysql_num_rows(result);
	if (num_rows < 1)
	{
		Threads = "NULL"; RAM = "NULL";
		return; // 아직 업데이트 안 되었으면 return;
	}		
	row = mysql_fetch_row(result);
	Threads = std::string(row[0]);
	RAM = std::string(row[1]);
}


void DBServerConnect::recvCumulativeData(int &run_ID, std::string &cumulative_data)
{
	sql = "SELECT `run_ID`, `cumulative` FROM `" + DB_TABLE_g42ui + "` WHERE `computer_ID`= '" + computerID + "';";
	mysql_query(conn, sql.c_str());
	result = mysql_store_result(conn);
	row = mysql_fetch_row(result);
	run_ID = atoi(row[0]);
	cumulative_data = std::string(row[1]);
}

void DBServerConnect::recvResultData(std::string& result_data)
{
	sql = "SELECT `result` FROM `" + DB_TABLE_g42ui + "` WHERE `computer_ID`= '" + computerID + "';";
	mysql_query(conn, sql.c_str());
	result = mysql_store_result(conn);
	row = mysql_fetch_row(result);
	result_data = std::string(row[0]);
}

void DBServerConnect::recvAirKermaResultData(std::string& result_data)
{
	sql = "SELECT `result_AK` FROM `" + DB_TABLE_g42ui + "` WHERE `computer_ID`= '" + computerID + "';";
	mysql_query(conn, sql.c_str());
	result = mysql_store_result(conn);
	row = mysql_fetch_row(result);
	result_data = std::string(row[0]);
}

void DBServerConnect::recvTinySkinDoseFile(std::string& result_data)
{
	sql = "SELECT `skinDose_1` FROM `" + DB_TABLE_skindose + "` WHERE `computer_ID`= '" + computerID + "';";
	mysql_query(conn, sql.c_str());
	result = mysql_store_result(conn);
	row = mysql_fetch_row(result);
	result_data = std::string(row[0]);
}

void DBServerConnect::recvSkinDoseUnitConversionFactor(std::string& result_data)
{
	sql = "SELECT `Unit` FROM `" + DB_TABLE_skindose + "` WHERE `computer_ID`= '" + computerID + "';";
	mysql_query(conn, sql.c_str());
	result = mysql_store_result(conn);
	row = mysql_fetch_row(result);
	result_data = std::string(row[0]);
}


void DBServerConnect::createTable()
{
	// ui2g4
	// Create Table
	sql = "CREATE TABLE `" + DB_NAME + "`.`" + DB_TABLE_ui2g4 + "` ("
		"`Index` SMALLINT,"
		"`Flag` TINYINT,"
		"`uiRunSignal` INT,"
		"`computer_ID` TINYTEXT,"
		"`server_ID` TINYTEXT,"
		"`command` TINYTEXT,"
		"`Simulation_info` MEDIUMTEXT,"
		"`phantom_collection` MEDIUMTEXT,"
		"`list_collection` MEDIUMTEXT,"
		"`macro` MEDIUMTEXT,"
		"`err` TEXT,"
		"`activity` TEXT,"
		"`layer.ele` LONGTEXT,"
		"`layer.node` LONGTEXT,"
		"`layer.material` LONGTEXT,"
		"`glasses.ele` LONGTEXT,"
		"`glasses.node` LONGTEXT,"
		"`glasses.material` LONGTEXT,"
		"`layer_dosimeter.ele` LONGTEXT,"
		"`layer_dosimeter.node` LONGTEXT,"
		"`layer_dosimeter.material` LONGTEXT,"
		" PRIMARY KEY(`Index`));";
	mysql_query(conn, sql.c_str());

	// Set first row
	sql = "INSERT INTO `" + DB_NAME + "`.`" + DB_TABLE_ui2g4 + "` ("
		"`Index`, " // (0)
		"`Flag`, " // (1)
		"`uiRunSignal`, " // (2)
		"`computer_ID`, " // (3)
		"`server_ID`, " // (4)
		"`command`, " // (5)
		"`Simulation_info`, " // (6)
		"`phantom_collection`, " // (7)
		"`list_collection`, " // (8)
		"`macro`, " // (9)
		"`err`, " // (10)
		"`activity`, " // (11)
		"`layer.ele`, " // (12)
		"`layer.node`, " // (13)
		"`layer.material`, " // (14)
		"`glasses.ele`, " // (15)
		"`glasses.node`, " // (16)
		"`glasses.material`, " // (17)
		"`layer_dosimeter.ele`, " // (18)
		"`layer_dosimeter.node`, " // (19)
		"`layer_dosimeter.material` " // (20)
		") VALUES ('-1','-1','0','NULL','NULL','NULL'," // Index, Flag, uiRunSignal, compurter_ID, server_ID, command
			"'NULL', "  // Simulation_info
			"'NULL', "  // phantom_collection
			"'NULL', "  // list_collection
			"'NULL', "  // macro
			"'NULL', "  // err
			"'NULL', "  // activity
			"'NULL', "  // layer.ele
			"'NULL', "  // layer.node
			"'NULL', "  // layer.material
			"'NULL', "  // glasses.ele
			"'NULL', "  // glasses.node
			"'NULL', "  // glasses.material
			"'NULL', "  // layer_dosimeter.ele
			"'NULL', "  // layer_dosimeter.node
			"'NULL');"; // layer_dosimeter.material
	mysql_query(conn, sql.c_str());

	// g42ui
	// Create Table
	sql = "CREATE TABLE `" + DB_NAME + "`.`" + DB_TABLE_g42ui + "` ("
		"`Index` SMALLINT,"
		"`computer_ID` TINYTEXT,"
		"`run_ID` SMALLINT,"
		"`cumulative` MEDIUMTEXT,"
		"`result` MEDIUMTEXT,"
		"`result_AK` MEDIUMTEXT,"
		"`Threads` SMALLINT,"
		"`RAM` INT,"
		" PRIMARY KEY(`Index`));";
	mysql_query(conn, sql.c_str());

	 //Set first row
	sql = "INSERT INTO `" + DB_NAME + "`.`" + DB_TABLE_g42ui + "` ("
		"`Index`, "
		"`computer_ID`, "
		"`run_ID`, "
		"`cumulative`, "
		"`result`, "
		"`result_AK`, "
		"`Threads`, "
		"`RAM` "
		") VALUES ('-1','NULL','0','NULL','NULL','NULL','-1','-1');";
	mysql_query(conn, sql.c_str());

	// _phsp
	// Create Table
	sql = "CREATE TABLE `" + DB_NAME + "`.`" + DB_TABLE_phsp + "` ("
		"`Index` SMALLINT,"
		"`computer_ID` TINYTEXT,"
		"`phspFile_1` MEDIUMTEXT,`phspFile_2` MEDIUMTEXT,`phspFile_3` MEDIUMTEXT,`phspFile_4` MEDIUMTEXT,`phspFile_5` MEDIUMTEXT,"
		"`phspFile_6` MEDIUMTEXT,`phspFile_7` MEDIUMTEXT,`phspFile_8` MEDIUMTEXT,`phspFile_9` MEDIUMTEXT,`phspFile_10` MEDIUMTEXT,"
		"`phspFile_11` MEDIUMTEXT,`phspFile_12` MEDIUMTEXT,`phspFile_13` MEDIUMTEXT,`phspFile_14` MEDIUMTEXT,`phspFile_15` MEDIUMTEXT,"
		"`phspFile_16` MEDIUMTEXT,`phspFile_17` MEDIUMTEXT,`phspFile_18` MEDIUMTEXT,`phspFile_19` MEDIUMTEXT,`phspFile_20` MEDIUMTEXT,"
		"`phspFile_21` MEDIUMTEXT,`phspFile_22` MEDIUMTEXT,`phspFile_23` MEDIUMTEXT,`phspFile_24` MEDIUMTEXT,`phspFile_25` MEDIUMTEXT,"
		"`phspFile_26` MEDIUMTEXT,`phspFile_27` MEDIUMTEXT,`phspFile_28` MEDIUMTEXT,`phspFile_29` MEDIUMTEXT,`phspFile_30` MEDIUMTEXT,"
		"`phspFile_31` MEDIUMTEXT,`phspFile_32` MEDIUMTEXT,`phspFile_33` MEDIUMTEXT,`phspFile_34` MEDIUMTEXT,`phspFile_35` MEDIUMTEXT,"
		"`phspFile_36` MEDIUMTEXT,`phspFile_37` MEDIUMTEXT,`phspFile_38` MEDIUMTEXT,`phspFile_39` MEDIUMTEXT,`phspFile_40` MEDIUMTEXT,"
		"`phspFile_41` MEDIUMTEXT,`phspFile_42` MEDIUMTEXT,`phspFile_43` MEDIUMTEXT,`phspFile_44` MEDIUMTEXT,`phspFile_45` MEDIUMTEXT,"
		"`phspFile_46` MEDIUMTEXT,`phspFile_47` MEDIUMTEXT,`phspFile_48` MEDIUMTEXT,`phspFile_49` MEDIUMTEXT,`phspFile_50` MEDIUMTEXT,"
		"`phspFile_51` MEDIUMTEXT,`phspFile_52` MEDIUMTEXT,`phspFile_53` MEDIUMTEXT,`phspFile_54` MEDIUMTEXT,`phspFile_55` MEDIUMTEXT,"
		"`phspFile_56` MEDIUMTEXT,`phspFile_57` MEDIUMTEXT,`phspFile_58` MEDIUMTEXT,`phspFile_59` MEDIUMTEXT,`phspFile_60` MEDIUMTEXT,"
		"`phspFile_61` MEDIUMTEXT,`phspFile_62` MEDIUMTEXT,`phspFile_63` MEDIUMTEXT,`phspFile_64` MEDIUMTEXT,`phspFile_65` MEDIUMTEXT,"
		"`phspFile_66` MEDIUMTEXT,`phspFile_67` MEDIUMTEXT,`phspFile_68` MEDIUMTEXT,`phspFile_69` MEDIUMTEXT,`phspFile_70` MEDIUMTEXT,"
		"`phspFile_71` MEDIUMTEXT,`phspFile_72` MEDIUMTEXT,`phspFile_73` MEDIUMTEXT,`phspFile_74` MEDIUMTEXT,`phspFile_75` MEDIUMTEXT,"
		"`phspFile_76` MEDIUMTEXT,`phspFile_77` MEDIUMTEXT,`phspFile_78` MEDIUMTEXT,`phspFile_79` MEDIUMTEXT,`phspFile_80` MEDIUMTEXT,"
		"`phspFile_81` MEDIUMTEXT,`phspFile_82` MEDIUMTEXT,`phspFile_83` MEDIUMTEXT,`phspFile_84` MEDIUMTEXT,`phspFile_85` MEDIUMTEXT,"
		"`phspFile_86` MEDIUMTEXT,`phspFile_87` MEDIUMTEXT,`phspFile_88` MEDIUMTEXT,`phspFile_89` MEDIUMTEXT,`phspFile_90` MEDIUMTEXT,"
		"`phspFile_91` MEDIUMTEXT,`phspFile_92` MEDIUMTEXT,`phspFile_93` MEDIUMTEXT,`phspFile_94` MEDIUMTEXT,`phspFile_95` MEDIUMTEXT,"
		"`phspFile_96` MEDIUMTEXT,`phspFile_97` MEDIUMTEXT,`phspFile_98` MEDIUMTEXT,`phspFile_99` MEDIUMTEXT,`phspFile_100` MEDIUMTEXT,"
		"`phspFile_101` MEDIUMTEXT,`phspFile_102` MEDIUMTEXT,`phspFile_103` MEDIUMTEXT,`phspFile_104` MEDIUMTEXT,`phspFile_105` MEDIUMTEXT,"
		"`phspFile_106` MEDIUMTEXT,`phspFile_107` MEDIUMTEXT,`phspFile_108` MEDIUMTEXT,`phspFile_109` MEDIUMTEXT,`phspFile_110` MEDIUMTEXT,"
		"`phspFile_111` MEDIUMTEXT,`phspFile_112` MEDIUMTEXT,`phspFile_113` MEDIUMTEXT,`phspFile_114` MEDIUMTEXT,`phspFile_115` MEDIUMTEXT,"
		"`phspFile_116` MEDIUMTEXT,`phspFile_117` MEDIUMTEXT,`phspFile_118` MEDIUMTEXT,`phspFile_119` MEDIUMTEXT,`phspFile_120` MEDIUMTEXT,"
		"`phspFile_121` MEDIUMTEXT,`phspFile_122` MEDIUMTEXT,`phspFile_123` MEDIUMTEXT,`phspFile_124` MEDIUMTEXT,`phspFile_125` MEDIUMTEXT,"
		"`phspFile_126` MEDIUMTEXT,`phspFile_127` MEDIUMTEXT,`phspFile_128` MEDIUMTEXT,`phspFile_129` MEDIUMTEXT,`phspFile_130` MEDIUMTEXT,"
		"`phspFile_131` MEDIUMTEXT,`phspFile_132` MEDIUMTEXT,`phspFile_133` MEDIUMTEXT,`phspFile_134` MEDIUMTEXT,`phspFile_135` MEDIUMTEXT,"
		"`phspFile_136` MEDIUMTEXT,`phspFile_137` MEDIUMTEXT,`phspFile_138` MEDIUMTEXT,`phspFile_139` MEDIUMTEXT,`phspFile_140` MEDIUMTEXT,"
		"`phspFile_141` MEDIUMTEXT,`phspFile_142` MEDIUMTEXT,`phspFile_143` MEDIUMTEXT,`phspFile_144` MEDIUMTEXT,`phspFile_145` MEDIUMTEXT,"
		"`phspFile_146` MEDIUMTEXT,`phspFile_147` MEDIUMTEXT,`phspFile_148` MEDIUMTEXT,`phspFile_149` MEDIUMTEXT,`phspFile_150` MEDIUMTEXT,"
		"`phspFile_151` MEDIUMTEXT,`phspFile_152` MEDIUMTEXT,`phspFile_153` MEDIUMTEXT,`phspFile_154` MEDIUMTEXT,`phspFile_155` MEDIUMTEXT,"
		"`phspFile_156` MEDIUMTEXT,`phspFile_157` MEDIUMTEXT,`phspFile_158` MEDIUMTEXT,`phspFile_159` MEDIUMTEXT,`phspFile_160` MEDIUMTEXT,"
		"`phspFile_161` MEDIUMTEXT,`phspFile_162` MEDIUMTEXT,`phspFile_163` MEDIUMTEXT,`phspFile_164` MEDIUMTEXT,`phspFile_165` MEDIUMTEXT,"
		"`phspFile_166` MEDIUMTEXT,`phspFile_167` MEDIUMTEXT,`phspFile_168` MEDIUMTEXT,`phspFile_169` MEDIUMTEXT,`phspFile_170` MEDIUMTEXT,"
		"`phspFile_171` MEDIUMTEXT,`phspFile_172` MEDIUMTEXT,`phspFile_173` MEDIUMTEXT,`phspFile_174` MEDIUMTEXT,`phspFile_175` MEDIUMTEXT,"
		"`phspFile_176` MEDIUMTEXT,`phspFile_177` MEDIUMTEXT,`phspFile_178` MEDIUMTEXT,`phspFile_179` MEDIUMTEXT,`phspFile_180` MEDIUMTEXT,"
		"`phspFile_181` MEDIUMTEXT,`phspFile_182` MEDIUMTEXT,`phspFile_183` MEDIUMTEXT,`phspFile_184` MEDIUMTEXT,`phspFile_185` MEDIUMTEXT,"
		"`phspFile_186` MEDIUMTEXT,`phspFile_187` MEDIUMTEXT,`phspFile_188` MEDIUMTEXT,`phspFile_189` MEDIUMTEXT,`phspFile_190` MEDIUMTEXT,"
		"`phspFile_191` MEDIUMTEXT,`phspFile_192` MEDIUMTEXT,`phspFile_193` MEDIUMTEXT,`phspFile_194` MEDIUMTEXT,`phspFile_195` MEDIUMTEXT,"
		"`phspFile_196` MEDIUMTEXT,`phspFile_197` MEDIUMTEXT,`phspFile_198` MEDIUMTEXT,`phspFile_199` MEDIUMTEXT,`phspFile_200` MEDIUMTEXT,"
		"`phspFile_201` MEDIUMTEXT,`phspFile_202` MEDIUMTEXT,`phspFile_203` MEDIUMTEXT,`phspFile_204` MEDIUMTEXT,`phspFile_205` MEDIUMTEXT,"
		"`phspFile_206` MEDIUMTEXT,`phspFile_207` MEDIUMTEXT,`phspFile_208` MEDIUMTEXT,`phspFile_209` MEDIUMTEXT,`phspFile_210` MEDIUMTEXT,"
		"`phspFile_211` MEDIUMTEXT,`phspFile_212` MEDIUMTEXT,`phspFile_213` MEDIUMTEXT,`phspFile_214` MEDIUMTEXT,`phspFile_215` MEDIUMTEXT,"
		"`phspFile_216` MEDIUMTEXT,`phspFile_217` MEDIUMTEXT,`phspFile_218` MEDIUMTEXT,`phspFile_219` MEDIUMTEXT,`phspFile_220` MEDIUMTEXT,"
		"`phspFile_221` MEDIUMTEXT,`phspFile_222` MEDIUMTEXT,`phspFile_223` MEDIUMTEXT,`phspFile_224` MEDIUMTEXT,`phspFile_225` MEDIUMTEXT,"
		"`phspFile_226` MEDIUMTEXT,`phspFile_227` MEDIUMTEXT,`phspFile_228` MEDIUMTEXT,`phspFile_229` MEDIUMTEXT,`phspFile_230` MEDIUMTEXT,"
		"`phspFile_231` MEDIUMTEXT,`phspFile_232` MEDIUMTEXT,`phspFile_233` MEDIUMTEXT,`phspFile_234` MEDIUMTEXT,`phspFile_235` MEDIUMTEXT,"
		"`phspFile_236` MEDIUMTEXT,`phspFile_237` MEDIUMTEXT,`phspFile_238` MEDIUMTEXT,`phspFile_239` MEDIUMTEXT,`phspFile_240` MEDIUMTEXT,"
		"`phspFile_241` MEDIUMTEXT,`phspFile_242` MEDIUMTEXT,`phspFile_243` MEDIUMTEXT,`phspFile_244` MEDIUMTEXT,`phspFile_245` MEDIUMTEXT,"
		"`phspFile_246` MEDIUMTEXT,`phspFile_247` MEDIUMTEXT,`phspFile_248` MEDIUMTEXT,`phspFile_249` MEDIUMTEXT,`phspFile_250` MEDIUMTEXT,"
		"`phspFile_251` MEDIUMTEXT,`phspFile_252` MEDIUMTEXT,`phspFile_253` MEDIUMTEXT,`phspFile_254` MEDIUMTEXT,`phspFile_255` MEDIUMTEXT,"
		"`phspFile_256` MEDIUMTEXT,`phspFile_257` MEDIUMTEXT,`phspFile_258` MEDIUMTEXT,`phspFile_259` MEDIUMTEXT,`phspFile_260` MEDIUMTEXT,"
		"`phspFile_261` MEDIUMTEXT,`phspFile_262` MEDIUMTEXT,`phspFile_263` MEDIUMTEXT,`phspFile_264` MEDIUMTEXT,`phspFile_265` MEDIUMTEXT,"
		"`phspFile_266` MEDIUMTEXT,`phspFile_267` MEDIUMTEXT,`phspFile_268` MEDIUMTEXT,`phspFile_269` MEDIUMTEXT,`phspFile_270` MEDIUMTEXT,"
		"`phspFile_271` MEDIUMTEXT,`phspFile_272` MEDIUMTEXT,`phspFile_273` MEDIUMTEXT,`phspFile_274` MEDIUMTEXT,`phspFile_275` MEDIUMTEXT,"
		"`phspFile_276` MEDIUMTEXT,`phspFile_277` MEDIUMTEXT,`phspFile_278` MEDIUMTEXT,`phspFile_279` MEDIUMTEXT,`phspFile_280` MEDIUMTEXT,"
		"`phspFile_281` MEDIUMTEXT,`phspFile_282` MEDIUMTEXT,`phspFile_283` MEDIUMTEXT,`phspFile_284` MEDIUMTEXT,`phspFile_285` MEDIUMTEXT,"
		"`phspFile_286` MEDIUMTEXT,`phspFile_287` MEDIUMTEXT,`phspFile_288` MEDIUMTEXT,`phspFile_289` MEDIUMTEXT,`phspFile_290` MEDIUMTEXT,"
		"`phspFile_291` MEDIUMTEXT,`phspFile_292` MEDIUMTEXT,`phspFile_293` MEDIUMTEXT,`phspFile_294` MEDIUMTEXT,`phspFile_295` MEDIUMTEXT,"
		"`phspFile_296` MEDIUMTEXT,`phspFile_297` MEDIUMTEXT,`phspFile_298` MEDIUMTEXT,`phspFile_299` MEDIUMTEXT,`phspFile_300` MEDIUMTEXT,"
		" PRIMARY KEY(`Index`));";
	mysql_query(conn, sql.c_str());

	// Set first row
	sql = "INSERT INTO `" + DB_NAME + "`.`" + DB_TABLE_phsp + "` ("
		"`Index`, "
		"`computer_ID`, "
		"`phspFile_1`, `phspFile_2`, `phspFile_3`, `phspFile_4`, `phspFile_5`, `phspFile_6`, `phspFile_7`, `phspFile_8`, `phspFile_9`, `phspFile_10`, "
		"`phspFile_11`, `phspFile_12`, `phspFile_13`, `phspFile_14`, `phspFile_15`, `phspFile_16`, `phspFile_17`, `phspFile_18`, `phspFile_19`, `phspFile_20`, "
		"`phspFile_21`, `phspFile_22`, `phspFile_23`, `phspFile_24`, `phspFile_25`, `phspFile_26`, `phspFile_27`, `phspFile_28`, `phspFile_29`, `phspFile_30`, "
		"`phspFile_31`, `phspFile_32`, `phspFile_33`, `phspFile_34`, `phspFile_35`, `phspFile_36`, `phspFile_37`, `phspFile_38`, `phspFile_39`, `phspFile_40`, "
		"`phspFile_41`, `phspFile_42`, `phspFile_43`, `phspFile_44`, `phspFile_45`, `phspFile_46`, `phspFile_47`, `phspFile_48`, `phspFile_49`, `phspFile_50`, "
		"`phspFile_51`, `phspFile_52`, `phspFile_53`, `phspFile_54`, `phspFile_55`, `phspFile_56`, `phspFile_57`, `phspFile_58`, `phspFile_59`, `phspFile_60`, "
		"`phspFile_61`, `phspFile_62`, `phspFile_63`, `phspFile_64`, `phspFile_65`, `phspFile_66`, `phspFile_67`, `phspFile_68`, `phspFile_69`, `phspFile_70`, "
		"`phspFile_71`, `phspFile_72`, `phspFile_73`, `phspFile_74`, `phspFile_75`, `phspFile_76`, `phspFile_77`, `phspFile_78`, `phspFile_79`, `phspFile_80`, "
		"`phspFile_81`, `phspFile_82`, `phspFile_83`, `phspFile_84`, `phspFile_85`, `phspFile_86`, `phspFile_87`, `phspFile_88`, `phspFile_89`, `phspFile_90`, "
		"`phspFile_91`, `phspFile_92`, `phspFile_93`, `phspFile_94`, `phspFile_95`, `phspFile_96`, `phspFile_97`, `phspFile_98`, `phspFile_99`, `phspFile_100`, "
		"`phspFile_101`, `phspFile_102`, `phspFile_103`, `phspFile_104`, `phspFile_105`, `phspFile_106`, `phspFile_107`, `phspFile_108`, `phspFile_109`, `phspFile_110`, "
		"`phspFile_111`, `phspFile_112`, `phspFile_113`, `phspFile_114`, `phspFile_115`, `phspFile_116`, `phspFile_117`, `phspFile_118`, `phspFile_119`, `phspFile_120`, "
		"`phspFile_121`, `phspFile_122`, `phspFile_123`, `phspFile_124`, `phspFile_125`, `phspFile_126`, `phspFile_127`, `phspFile_128`, `phspFile_129`, `phspFile_130`, "
		"`phspFile_131`, `phspFile_132`, `phspFile_133`, `phspFile_134`, `phspFile_135`, `phspFile_136`, `phspFile_137`, `phspFile_138`, `phspFile_139`, `phspFile_140`, "
		"`phspFile_141`, `phspFile_142`, `phspFile_143`, `phspFile_144`, `phspFile_145`, `phspFile_146`, `phspFile_147`, `phspFile_148`, `phspFile_149`, `phspFile_150`, "
		"`phspFile_151`, `phspFile_152`, `phspFile_153`, `phspFile_154`, `phspFile_155`, `phspFile_156`, `phspFile_157`, `phspFile_158`, `phspFile_159`, `phspFile_160`, "
		"`phspFile_161`, `phspFile_162`, `phspFile_163`, `phspFile_164`, `phspFile_165`, `phspFile_166`, `phspFile_167`, `phspFile_168`, `phspFile_169`, `phspFile_170`, "
		"`phspFile_171`, `phspFile_172`, `phspFile_173`, `phspFile_174`, `phspFile_175`, `phspFile_176`, `phspFile_177`, `phspFile_178`, `phspFile_179`, `phspFile_180`, "
		"`phspFile_181`, `phspFile_182`, `phspFile_183`, `phspFile_184`, `phspFile_185`, `phspFile_186`, `phspFile_187`, `phspFile_188`, `phspFile_189`, `phspFile_190`, "
		"`phspFile_191`, `phspFile_192`, `phspFile_193`, `phspFile_194`, `phspFile_195`, `phspFile_196`, `phspFile_197`, `phspFile_198`, `phspFile_199`, `phspFile_200`, "
		"`phspFile_201`, `phspFile_202`, `phspFile_203`, `phspFile_204`, `phspFile_205`, `phspFile_206`, `phspFile_207`, `phspFile_208`, `phspFile_209`, `phspFile_210`, "
		"`phspFile_211`, `phspFile_212`, `phspFile_213`, `phspFile_214`, `phspFile_215`, `phspFile_216`, `phspFile_217`, `phspFile_218`, `phspFile_219`, `phspFile_220`, "
		"`phspFile_221`, `phspFile_222`, `phspFile_223`, `phspFile_224`, `phspFile_225`, `phspFile_226`, `phspFile_227`, `phspFile_228`, `phspFile_229`, `phspFile_230`, "
		"`phspFile_231`, `phspFile_232`, `phspFile_233`, `phspFile_234`, `phspFile_235`, `phspFile_236`, `phspFile_237`, `phspFile_238`, `phspFile_239`, `phspFile_240`, "
		"`phspFile_241`, `phspFile_242`, `phspFile_243`, `phspFile_244`, `phspFile_245`, `phspFile_246`, `phspFile_247`, `phspFile_248`, `phspFile_249`, `phspFile_250`, "
		"`phspFile_251`, `phspFile_252`, `phspFile_253`, `phspFile_254`, `phspFile_255`, `phspFile_256`, `phspFile_257`, `phspFile_258`, `phspFile_259`, `phspFile_260`, "
		"`phspFile_261`, `phspFile_262`, `phspFile_263`, `phspFile_264`, `phspFile_265`, `phspFile_266`, `phspFile_267`, `phspFile_268`, `phspFile_269`, `phspFile_270`, "
		"`phspFile_271`, `phspFile_272`, `phspFile_273`, `phspFile_274`, `phspFile_275`, `phspFile_276`, `phspFile_277`, `phspFile_278`, `phspFile_279`, `phspFile_280`, "
		"`phspFile_281`, `phspFile_282`, `phspFile_283`, `phspFile_284`, `phspFile_285`, `phspFile_286`, `phspFile_287`, `phspFile_288`, `phspFile_289`, `phspFile_290`, "
		"`phspFile_291`, `phspFile_292`, `phspFile_293`, `phspFile_294`, `phspFile_295`, `phspFile_296`, `phspFile_297`, `phspFile_298`, `phspFile_299`, `phspFile_300` "
		") VALUES ('-1','NULL',"
		"'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', "
		"'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', "
		"'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', "
		"'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', "
		"'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', "
		"'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', "
		"'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', "
		"'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', "
		"'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', "
		"'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', "
		"'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', "
		"'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', "
		"'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', "
		"'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', "
		"'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', "
		"'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', "
		"'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', "
		"'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', "
		"'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', "
		"'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', "
		"'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', "
		"'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', "
		"'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', "
		"'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', "
		"'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', "
		"'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', "
		"'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', "
		"'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', "
		"'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', "
		"'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL');";
	mysql_query(conn, sql.c_str());

	// _skindose
	// Create Table
	sql = "CREATE TABLE `" + DB_NAME + "`.`" + DB_TABLE_skindose + "` ("
		"`Index` SMALLINT,"
		"`computer_ID` TINYTEXT,"
		"`Unit` TINYTEXT,"
		"`skinDose_1` MEDIUMTEXT,"
		" PRIMARY KEY(`Index`));";
	mysql_query(conn, sql.c_str());

	// Set first row
	sql = "INSERT INTO `" + DB_NAME + "`.`" + DB_TABLE_skindose + "` ("
		"`Index`, "
		"`computer_ID`, "
		"`Unit`, "
		"`skinDose_1` "
		") VALUES ('-1','NULL','NULL',"
		"'NULL');";
	mysql_query(conn, sql.c_str());
}

void DBServerConnect::dropTable()
{
	// Drop Table
	sql = "DROP TABLE `" + DB_NAME + "`.`" + DB_TABLE_ui2g4 + "`;";
	mysql_query(conn, sql.c_str());
}