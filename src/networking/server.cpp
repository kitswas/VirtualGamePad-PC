#include "server.hpp"

#include "ui_server.h"

#include <QByteArray>
#include <QDataStream>
#include <QHostAddress>
#include <QList>
#include <QMessageBox>
#include <QNetworkInterface>
#include <QThread>

Server::Server(QWidget *parent) : QDialog(parent), ui(new Ui::Server)
{
	ui->setupUi(this);
	initServer();
}

Server::~Server()
{
	delete ui;
}

void Server::initServer()
{
	tcpServer = new QTcpServer(this);
	//	tcpServer->setListenBacklogSize(1);
	if (!tcpServer->listen(QHostAddress::AnyIPv4, 7878))
	{
		QMessageBox::critical(this, tr("VGamepad Server"),
							  tr("Unable to start the server: %1.").arg(tcpServer->errorString()));
		close();
		return;
	}
	QString message = tr("The server is running on\n\nPort: `%1`\n\nAt the following IP Address(es):\n\n")
						  .arg(tcpServer->serverPort());
	const QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
	for (const QHostAddress &entry : ipAddressesList)
	{
		if (entry.isGlobal())
			message += tr("`%1`\n").arg(entry.toString());
	}
	ui->statusLabel->setText(message);
	connect(tcpServer, &QTcpServer::newConnection, this, &Server::handleConnection);
}

void Server::handleConnection()
{
	QTcpSocket *clientConnection = tcpServer->nextPendingConnection();
	QString connectionMessage;
	connectionMessage =
		tr("Connected to %1 at `%2 : %3`")
			.arg(clientConnection->peerName().isEmpty() ? "Unknown device" : clientConnection->peerName(),
				 clientConnection->peerAddress().toString(), QString::number(clientConnection->peerPort()));
	qDebug() << connectionMessage;
	ui->clientLabel->setText(connectionMessage);
	connect(clientConnection, &QAbstractSocket::disconnected, clientConnection, &QObject::deleteLater);
	connect(clientConnection, &QAbstractSocket::disconnected, this, [this]() {
		// Perform Null checks, because this lambda is also called when the server is closed
		// Note, the order is important
		if (this != nullptr && ui != nullptr && ui->clientLabel != nullptr)
			ui->clientLabel->setText(tr("No client connected"));
	});
	connect(clientConnection, &QAbstractSocket::readyRead, this, [clientConnection]() {
		qDebug() << "Received: " << clientConnection->bytesAvailable() << "bytes";
		QByteArray request = clientConnection->readAll();
		qDebug() << "Request: " << request;
	});
}
