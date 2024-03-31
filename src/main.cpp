#include <QApplication>
#include <QStyleFactory>

#include "mainwindow.hpp"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.setStyle(QStyleFactory::create("Fusion"));
	a.setOrganizationName("Kitswas");
	a.setOrganizationDomain("io.github.kitswas");
	a.setApplicationName("VirtualGamePad");
	MainWindow w;
	w.show();
	return a.exec();
}
