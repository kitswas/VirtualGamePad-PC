#include "appdir.hpp"
#include "platform/windows/console.hpp"
#include "ui/mainwindow.hpp"

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QLockFile>
#include <QMutex>
#include <QStandardPaths>
#include <QStyleFactory>
#include <memory>

#if defined(QT_DEBUG)
const QString logFilePath = "virtualgamepad.log";
#elif defined(_WIN32)
const QString logFilePath = "virtualgamepad.log";
#elif defined(__linux__)
const QString logFilePath =
	QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/virtualgamepad.log";
#endif

/**
 * @brief Maximum number of log files to try to create.
 * If all are in use, logging to file will be disabled.
 */
const int8_t max_log_files = 5;

static QFile logFile(logFilePath);

void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
	static QMutex logFileMutex;
	QString txt = qFormatLogMessage(type, context, msg);

	QMutexLocker locker(&logFileMutex);
	QTextStream textStream(&logFile);
	textStream << txt << Qt::endl;

	if (type == QtFatalMsg)
		abort();
}

int main(int argc, char *argv[])
{
#ifdef _WIN32
	// Attach to parent console if launched from terminal
	attachToParentConsole();
#endif

	// Try to find a free log file slot (virtualgamepad.1.log, virtualgamepad.2.log, etc.)
	static std::unique_ptr<QLockFile> lockFile;
	QFileInfo baseInfo(logFilePath);
	QString baseName = baseInfo.completeBaseName();
	QString suffix = baseInfo.suffix();
	// Use the directory from the configured path, or current dir if none specified
	QDir logDir = baseInfo.dir();

	for (int i = 1; i <= max_log_files; ++i)
	{
		QString fileName = QString("%1.%2.%3").arg(baseName).arg(i).arg(suffix);
		QString fullPath = logDir.filePath(fileName);

		auto l = std::make_unique<QLockFile>(fullPath + ".lock");
		l->setStaleLockTime(0);
		if (l->tryLock())
		{
			lockFile = std::move(l);
			logFile.setFileName(fullPath);
			break;
		}
	}

	qSetMessagePattern("[%{time yyyy-MM-ddTHH:mm:ss.zzz} %{pid} %{type}] %{message}");

	bool logFileOpened = false;
	if (lockFile)
	{
		logFileOpened = logFile.open(QIODevice::WriteOnly | QIODevice::Truncate |
									 QIODevice::Unbuffered | QIODevice::Text);
	}
	else
	{
		qWarning() << "Failed to acquire lock for any log file slot (1-" << max_log_files << "). "
		"Logging to file disabled.";
	}

	qInfo() << "Log file path:" << QFileInfo(logFile).absoluteFilePath();
	QtMessageHandler oldMessageHandler;
	if (logFileOpened)
	{
		oldMessageHandler = qInstallMessageHandler(customMessageHandler);
	}
	else
	{
		qWarning() << "Failed to open log file.";
	}

	qInfo() << "Launching app...";

	QApplication a(argc, argv);
	QApplication::setStyle(QStyleFactory::create("Fusion"));
	QApplication::setOrganizationName("kitswas");
	QApplication::setOrganizationDomain("io.github.kitswas");
	QApplication::setApplicationName("VirtualGamePad");
	QApplication::setApplicationVersion(APP_VERSION);
	qInfo() << "App data directory:" << getAppDataDir();

	MainWindow w;
	w.show();
	qInfo() << "Application initialized successfully. Version:"
			<< QApplication::applicationVersion();
	int result = QApplication::exec();
	qInfo() << "Application shutting down with exit code:" << result;

	if (logFileOpened)
	{
		qInstallMessageHandler(oldMessageHandler); // Reset message handler
		logFile.close();
		// Set permissions (allow non-admin users to read/write if run as admin)
		//! This does not modify ACLs, so the effectiveness can be limited
		// Needs more testing
		logFile.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner |
							   QFileDevice::ReadGroup | QFileDevice::WriteGroup |
							   QFileDevice::ReadUser | QFileDevice::WriteUser);
	}

	return result;
}
