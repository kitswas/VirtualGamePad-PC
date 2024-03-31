#pragma once

#include <QDialog>
#include <QTcpServer>
#include <QTcpSocket>

namespace Ui
{
class Server;
}

class Server : public QWidget
{
	Q_OBJECT

  public:
	explicit Server(QWidget *parent = nullptr);
	~Server() override;
	QTcpServer *tcpServer = nullptr;

  private slots:
	void handleConnection();
	void destroyServer();

  private:
	void initServer();
	void serveClient();

	Ui::Server *ui;
	QTcpSocket *clientConnection;
	bool isGamepadConnected;
};
