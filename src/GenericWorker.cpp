#include "pch.h"
#include "GenericWorker.h"

GenericWorker::GenericWorker(Task task, QObject* parent)
	: QObject(parent), m_task(task)
{
}

void GenericWorker::process()
{
	// 생성자에서 전달받은 작업(람다 함수)을 실행하고, 그 결과를 finished 시그널로 보냅니다.
	if (m_task) {
		emit finished(m_task());
	}
	else {
		emit finished(QVariant()); // 작업이 없으면 빈 결과를 보냄
	}
}