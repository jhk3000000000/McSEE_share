#pragma once
#include <QObject>
#include <QVariant>
#include <functional> // std::function을 위해 필요

// 어떤 타입의 결과든 반환할 수 있도록 QVariant 사용
using Task = std::function<QVariant()>;
using FinishedCallback = std::function<void(QVariant)>;

class GenericWorker : public QObject
{
	Q_OBJECT

public:
	explicit GenericWorker(Task task, QObject* parent = nullptr);

public slots:
	void process();

signals:
	void finished(const QVariant& result);

private:
	Task m_task;
};