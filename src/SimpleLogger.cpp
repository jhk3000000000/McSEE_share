#include "pch.h"
#include "SimpleLogger.h"
#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QMutexLocker>
#include <QDebug>

// static 멤버 변수 정의
QMutex SimpleLogger::logMutex;
QFile* SimpleLogger::logFile = nullptr;
QTextStream* SimpleLogger::logStream = nullptr;
bool SimpleLogger::isInitialized = false;
QString SimpleLogger::logFolderPath;
QString SimpleLogger::currentLogFileName;

void SimpleLogger::initialize() {
	QMutexLocker locker(&logMutex);

	if (!isInitialized) {
		// Log 폴더 경로 설정
		logFolderPath = QCoreApplication::applicationDirPath() + "/UI_Log";

		// Log 폴더 생성
		if (!createLogFolder()) {
			qDebug() << "Failed to create log folder:" << logFolderPath;
			return;
		}

		// 현재 시간으로 로그 파일명 생성
		currentLogFileName = createLogFileName();
		QString fullLogPath = logFolderPath + "/" + currentLogFileName;

		logFile = new QFile(fullLogPath);
		if (logFile->open(QIODevice::WriteOnly | QIODevice::Append)) {
			logStream = new QTextStream(logFile);
			// logStream->setCodec("UTF-8"); // Qt 5.13에서는 필수, Qt6에서는 필요 없음
			isInitialized = true;

			// 시작 로그
			QString startMsg = QString("=== Application Started at %1 ===")
				.arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
			*logStream << startMsg << "\n";
			*logStream << QString("Log file: %1").arg(currentLogFileName) << "\n";
			*logStream << QString("Application path: %1").arg(QCoreApplication::applicationDirPath()) << "\n";
			*logStream << "================================================\n";
			logStream->flush();

			qDebug() << "Logging initialized. File:" << fullLogPath;
		}
		else {
			qDebug() << "Failed to open log file:" << fullLogPath;
			delete logFile;
			logFile = nullptr;
		}
	}
}

QString SimpleLogger::createLogFileName() {
	// 현재 시간으로 파일명 생성
	// 형식: YYYY-MM-DD_HH-MM-SS.log
	// 예시: 2024-01-15_14-30-25.log
	QDateTime now = QDateTime::currentDateTime();
	QString fileName = now.toString("yyyy-MM-dd_hh-mm-ss") + ".log";
	return fileName;
}

bool SimpleLogger::createLogFolder() {
	QDir dir;

	// Log 폴더가 존재하지 않으면 생성
	if (!dir.exists(logFolderPath)) {
		if (dir.mkpath(logFolderPath)) {
			qDebug() << "Created log folder:" << logFolderPath;
			return true;
		}
		else {
			qDebug() << "Failed to create log folder:" << logFolderPath;
			return false;
		}
	}

	// 이미 존재하면 쓰기 권한 확인
	QFileInfo folderInfo(logFolderPath);
	if (!folderInfo.isWritable()) {
		qDebug() << "Log folder is not writable:" << logFolderPath;
		return false;
	}

	return true;
}

void SimpleLogger::log(const QString& level, const QString& function, const QString& message) {
	QMutexLocker locker(&logMutex);

	if (isInitialized && logStream) {
		QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
		QString logEntry = QString("[%1] [%2] %3: %4")
			.arg(timestamp)
			.arg(level)
			.arg(function)
			.arg(message);

		*logStream << logEntry << "\n";
		logStream->flush();

		// 콘솔에도 출력 (에러와 경고만)
		if (level == "ERROR" || level == "WARNING") {
			qDebug() << logEntry;
		}
	}
}

void SimpleLogger::cleanup() {
	QMutexLocker locker(&logMutex);

	if (isInitialized) {
		if (logStream) {
			QString endMsg = QString("=== Application Ended at %1 ===")
				.arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
			*logStream << "================================================\n";
			*logStream << endMsg << "\n";
			*logStream << QString("Total session time: %1")
				.arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")) << "\n";
			logStream->flush();
			delete logStream;
			logStream = nullptr;
		}

		if (logFile) {
			logFile->close();
			delete logFile;
			logFile = nullptr;
		}

		qDebug() << "Logging cleanup completed. Log saved to:" << logFolderPath + "/" + currentLogFileName;
		isInitialized = false;
	}
}