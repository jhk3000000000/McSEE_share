// ************************************************************************
// 
// This program is free software; you can redistribute itand /or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110 - 1335 USA.
// 
// ************************************************************************
#ifndef DB_SERVER_CONNECT_HH_
#define DB_SERVER_CONNECT_HH_

#include <mysql/mysql.h> // 외부 라이브러리 디렉토리 경로가 mysql.h가 있는 폴더 바깥에 있어서 mysql/mysql.h로 잡음

// McSEE DB server communicator
// Author: Sungho Moon

class DBServerConnect
{
public:
	DBServerConnect();
	~DBServerConnect();

	void connectServer();
	void disconnectServer();

	// Data commuication
	void sendRowInitialData(const std::string& command);
	void uiRunningSignalUpdate();
	void sendDBFileContent(int col_idx, const std::string& dbsend_file);
	void sendDBFileContent_HugeData(int col_idx, std::string chunk);
	std::vector<std::string> readInChunks(const std::string &filename);
	void sendStopSignal();
	void sendStopByNPS();

	void CheckGeant4Status(std::string& Flag);

	bool readFlag();
	void recvServerPCInfo(std::string& Threads, std::string& RAM);
	void recvCumulativeData(int &runID, std::string &cumulative_data);
	void recvResultData(std::string& result_data);
	void recvAirKermaResultData(std::string& result_data);
	void recvTinySkinDoseFile(std::string& result_data);
	void recvSkinDoseUnitConversionFactor(std::string& result_data);
	
	
	// Table
	void createTable();
	void dropTable();

private:	
	
	MYSQL* conn;
	MYSQL_RES* result;
	MYSQL_ROW row;
	
	std::string sql;
	std::string HOSTIP;
	std::string USERID;
	std::string PASSWD;
	unsigned int PORTID;
	std::string DB_NAME;
	std::string DB_TABLE_ui2g4, DB_TABLE_g42ui, DB_TABLE_phsp, DB_TABLE_skindose;
	std::string computerID;
	std::string computerID_Korean;
	std::vector<std::string> col_name_send, col_name_recv, col_name_phsp, col_name_skindose;
	const size_t CHUNK_SIZE_phspFile = 1.5e7; // 15 MB chunk -> 300 chunks ~ 4.5GB
	//const size_t CHUNK_SIZE_phspFile = 1.0e7; // 10 MB chunk

	int index;
};
#endif