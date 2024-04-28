#include <QApplication>
#include <QStyleFactory>

#include "ui/mainwindow.hpp"

int main(int argc, char *argv[])
{
	qSetMessagePattern(
		"[%{time h:mm:ss.zzz} %{if-category}%{category}: %{endif}%{type}] %{message}");

	qInfo() << "App launched.";

	QApplication a(argc, argv);
	QApplication::setStyle(QStyleFactory::create("Fusion"));
	QApplication::setOrganizationName("Kitswas");
	QApplication::setOrganizationDomain("io.github.kitswas");
	QApplication::setApplicationName("VirtualGamePad");

	MainWindow w;
	w.show();
	return QApplication::exec();
}
