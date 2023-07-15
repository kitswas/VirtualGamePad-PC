#include <QApplication>
#include <QStyleFactory>

#include "mainwindow.hpp"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.setStyle(QStyleFactory::create("Fusion"));
	MainWindow w;
	w.show();
	return a.exec();
}
