#include "pch.h"
#include "TaskManager.h"
#include <QThread>
#include <QProgressDialog>
#include <QApplication> // 최상위 위젯을 찾기 위해 필요

TaskManager& TaskManager::instance()
{
	static TaskManager instance;
	return instance;
}

TaskManager::TaskManager(QObject *parent) : QObject(parent) {}

void TaskManager::runTask(const QString& title, const QString& label, Task task, FinishedCallback onFinished)
{
	QThread* thread = new QThread();
	GenericWorker* worker = new GenericWorker(task);
	worker->moveToThread(thread);

	// 로딩창 (QProgressDialog) 생성
	// setMinimum(0), setMaximum(0)은 빙글빙글 도는 무한 로딩바를 만듭니다.
	QProgressDialog* progressDialog = new QProgressDialog(label, "Cancel", 0, 0, nullptr);
	progressDialog->setWindowTitle(title);
	progressDialog->setWindowModality(Qt::WindowModal);

	// 작업이 시작되면 로딩창을 띄웁니다.
	connect(thread, &QThread::started, progressDialog, &QProgressDialog::show);
	connect(thread, &QThread::started, worker, &GenericWorker::process);

	// 작업이 끝나면 콜백 함수를 호출하고 모든 것을 정리합니다.
	connect(worker, &GenericWorker::finished, this, [onFinished](const QVariant& result) {
		if (onFinished) {
			onFinished(result); // 메인 스레드에서 안전하게 결과 처리
		}
		});
	connect(worker, &GenericWorker::finished, progressDialog, &QProgressDialog::accept);
	connect(worker, &GenericWorker::finished, thread, &QThread::quit);
	connect(worker, &GenericWorker::finished, worker, &GenericWorker::deleteLater);
	connect(thread, &QThread::finished, thread, &QThread::deleteLater);

	// 사용자가 취소 버튼을 누르면 스레드를 종료 시도
	connect(progressDialog, &QProgressDialog::canceled, thread, &QThread::requestInterruption);

	thread->start();
}