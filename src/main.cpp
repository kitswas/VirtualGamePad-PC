#include "appdir.hpp"
#include "platform/windows/console.hpp"
#include "ui/mainwindow.hpp"

#include <QApplication>
#include <QFile>
#include <QMutex>
#include <QStandardPaths>
#include <QStyleFactory>

#if defined(QT_DEBUG)
const QString logFilePath = "virtualgamepad.log";
#elif defined(_WIN32)
const QString logFilePath = "virtualgamepad.log";
#elif defined(__linux__)
const QString logFilePath =
	QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/virtualgamepad.log";
#endif

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

	qSetMessagePattern("[%{time yyyy-MM-ddTHH:mm:ss.zzz} %{type}] %{message}");
	bool logFileOpened =
		logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Unbuffered |
					 QIODevice::Text | QIODevice::Truncate);

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
