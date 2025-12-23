#pragma once

#include <QObject>
#include <QTimer>
#include <vector>
#include <utility>
#include "DBServerConnect.h"

// 윈도우 API 사용 시
#ifdef Q_OS_WIN
#include <windows.h>
#endif

#ifndef getmax
    #define getmax(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef getmin
    #define getmin(a,b)            (((a) < (b)) ? (a) : (b))
#endif

class Manager_Calculation : public QObject
{
    Q_OBJECT

public:
    explicit Manager_Calculation(QObject *parent = nullptr);
    virtual ~Manager_Calculation();

    std::string timeStamp;
    FILETIME OrgandoseFile_PRE_ModifiedTime = { 0,0 };
    FILETIME SkindoseFile_PRE_ModifiedTime = { 0,0 };

signals:
    // [추가] GUI에게 로딩 메시지를 띄우라고 요청하는 신호
    void requestShowProgress(QString message);
    
    // [추가] 로딩 메시지를 끄라고 요청하는 신호
    void requestHideProgress();
public slots:
    // =========================================================
    // [Local Calculation Process]
    // =========================================================
    void Calculate_Local();                  // (이동됨) 로컬 계산 시작 진입점/
    void DoseCalculation_StopAndSave();      // (이동됨) 계산 중단 및 저장

    // =========================================================
    // [Server Calculation Process]
    // =========================================================
    void ServerInitialization();             // (이동됨) 서버 초기화/
    void Calculate_Server();                 // (이동됨) 서버 계산 시작 진입점/
    void uiRunning();                        // (이동됨) 서버 UI 신호/

    // =========================================================
    // [Utilities]
    // =========================================================
    std::vector<std::pair<double, double>> performAdaptiveDownsampling(
        const std::vector<std::pair<double, double>>& originalData, int targetSize); //
    void timer_LocalInit_Stop();
    void timer_LocalCheck_Stop();
    void timer_ServerInit_Stop();
    void timer_ServerCheck_Stop();
    void timer_uiRunning_Stop();

private slots:
    // =========================================================
    // [Internal Timers & Process Logic]
    // =========================================================
    void DataInitialization_Local();         // (이동됨) 타이머에 의해 호출/
    void checkProcessInfo_Local();           // (이동됨) 주기적 프로세스 체크/

    void DataInitialization_Server();        // (이동됨)/
    void checkProcessInfo_Server();          // (이동됨)/

private:
    // 내부 헬퍼
    void handleProcessError(const QString& errorMessage); // (이동됨)/

    // 타이머 객체들
    QTimer* m_timer_LocalInit = nullptr;
    QTimer* m_timer_LocalCheck = nullptr;
    QTimer* m_timer_ServerInit = nullptr;
    QTimer* m_timer_ServerCheck = nullptr;
    QTimer* m_timer_uiRunning = nullptr;
    
    // 서버 통신 객체
    DBServerConnect* serverConnect = nullptr; // 기존 FunctionPanelRight의 멤버를 이동
};