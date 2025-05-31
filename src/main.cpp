#include "ui/mainwindow.hpp"

#include <QApplication>
#include <QFile>
#include <QMutex>
#include <QStyleFactory>

static QFile logFile("LogFile.log");
static QMutex logFileMutex;

void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
	QString txt = qFormatLogMessage(type, context, msg);

	QMutexLocker locker(&logFileMutex);
	QTextStream textStream(&logFile);
	textStream << txt << Qt::endl;

	if (type == QtFatalMsg)
		abort();
}

int main(int argc, char *argv[])
{
	qSetMessagePattern("[%{time yyyy-MM-ddTHH:mm:ss.zzz} %{type}] %{message}");
	bool logFileOpened =
		logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Unbuffered |
					 QIODevice::Text | QIODevice::Truncate);

	if (logFileOpened)
	{
		qInstallMessageHandler(customMessageHandler);
	}
	else
	{
		qWarning() << "Failed to open log file.";
	}

	qInfo() << "App launched.";

	QApplication a(argc, argv);
	QApplication::setStyle(QStyleFactory::create("Fusion"));
	QApplication::setOrganizationName("kitswas");
	QApplication::setOrganizationDomain("io.github.kitswas");
	QApplication::setApplicationName("VirtualGamePad");
	QApplication::setApplicationVersion(APP_VERSION);

	MainWindow w;
	w.show();
	return QApplication::exec();
}
