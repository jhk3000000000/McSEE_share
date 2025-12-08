#pragma once
#include <QObject>
#include "GenericWorker.h" // 방금 만든 GenericWorker 포함

class TaskManager : public QObject
{
	Q_OBJECT

public:
	// 싱글톤 인스턴스를 얻는 함수 -> GUI 애플리케이션 어디서든 접근 가능한 싱글톤 설계, 모든 쓰레드와 프로그레스 다이얼로그 관리
	static TaskManager& instance();

	// 가장 중요한 함수: 작업을 스레드에서 실행시킵니다.
	void runTask(const QString& title, const QString& label, Task task, FinishedCallback onFinished);

private:
	// 싱글톤 패턴을 위한 private 생성자
	explicit TaskManager(QObject *parent = nullptr);
	TaskManager(const TaskManager&) = delete;
	TaskManager& operator=(const TaskManager&) = delete;
};