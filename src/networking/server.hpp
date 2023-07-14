#pragma once

#include <QDialog>
#include <QTcpServer>
#include <QTcpSocket>

namespace Ui
{
class Server;
}

class Server : public QDialog
{
	Q_OBJECT

  public:
	explicit Server(QWidget *parent = nullptr);
	~Server();
	QTcpServer *tcpServer = nullptr;

  private slots:
	void handleConnection();

  private:
	void initServer();
	void serveClient();

	Ui::Server *ui;
	QTcpSocket *clientConnection;
};
