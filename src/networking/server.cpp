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
	QString message =
		tr("The server is running on\n\nPort: %1\n\nAt the following IP Address(es):\n\n").arg(tcpServer->serverPort());
	const QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
	for (const QHostAddress &entry : ipAddressesList)
	{
		if (entry.isGlobal())
			message += tr("%1\n").arg(entry.toString());
	}
	ui->statusLabel->setText(message);
	connect(tcpServer, &QTcpServer::newConnection, this, &Server::handleConnection);
}

void Server::handleConnection()
{
	QTcpSocket *clientConnection = tcpServer->nextPendingConnection();
	qDebug() << "Established connection with" << clientConnection->peerName() << "at"
			 << clientConnection->peerAddress().toString() << ":" << clientConnection->peerPort();
	connect(clientConnection, &QAbstractSocket::disconnected, clientConnection, &QObject::deleteLater);
	connect(clientConnection, &QAbstractSocket::readyRead, this, [clientConnection]() {
		qDebug() << "Received: " << clientConnection->bytesAvailable() << "bytes";
		QByteArray request = clientConnection->readAll();
		qDebug() << "Request: " << request;
	});
}
