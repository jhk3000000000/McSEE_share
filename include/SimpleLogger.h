#ifndef SIMPLELOGGER_H
#define SIMPLELOGGER_H

#include <QMutex>
#include <QFile>
#include <QTextStream>
#include <QString>

class SimpleLogger {
private:
	static QMutex logMutex;
	static QFile* logFile;
	static QTextStream* logStream;
	static bool isInitialized;
	static QString logFolderPath;
	static QString currentLogFileName;

public:
	static void initialize();
	static void log(const QString& level, const QString& function, const QString& message);
	static void cleanup();

private:
	static QString createLogFileName();
	static bool createLogFolder();
};

// ===== 로깅 매크로 정의 =====
#define LOG_INFO(msg) SimpleLogger::log("INFO", __FUNCTION__, msg)
#define LOG_ERROR(msg) SimpleLogger::log("ERROR", __FUNCTION__, msg)
#define LOG_WARNING(msg) SimpleLogger::log("WARNING", __FUNCTION__, msg)
#define LOG_DEBUG(msg) SimpleLogger::log("DEBUG", __FUNCTION__, msg)

#endif // SIMPLELOGGER_H