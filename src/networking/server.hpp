#pragma once

#include <QDialog>
#include <QElapsedTimer>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>

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
	void updateConnectionStats();

  private:
	void initServer();
	void serveClient();
	void startLatencyMeasurement();

	Ui::Server *ui;
	QTcpSocket *clientConnection;
	bool isGamepadConnected;
	QTimer *statsUpdateTimer;
	QElapsedTimer latencyTimer;
	int packetCount;
	int bytesReceived;
	int latency;
	qint64 lastPacketTime;
};
