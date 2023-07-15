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
	ui->IPList->viewport()->setAutoFillBackground(false);
	clientConnection = nullptr;
	initServer();
}

Server::~Server()
{
	// IMPORTANT: clientConnection should not be accessed when the server is closed
	if (clientConnection != nullptr)
		clientConnection->disconnectFromHost(); // Close clientConnection gracefully
	delete ui;
}

void Server::initServer()
{
	tcpServer = new QTcpServer(this);
	tcpServer->setListenBacklogSize(0);
	if (!tcpServer->listen(QHostAddress::AnyIPv4, 7878))
	{
		QMessageBox::critical(this, tr("VGamepad Server"),
							  tr("Unable to start the server: %1.").arg(tcpServer->errorString()));
		close();
		return;
	}
	QString message = tr("\n**Warning:** The server will stop if you close this window.\n");
	message += tr("The server is running on\n\nPort: `%1`\n\nAt the following IP Address(es):\n\n")
				   .arg(tcpServer->serverPort());
	const QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
	for (const QHostAddress &entry : ipAddressesList)
	{
		if (entry.isGlobal())
		{
			ui->IPList->addItem(tr("%1").arg(entry.toString()));
			QImage QR(250, 250, QImage::Format_Mono);
			QR.fill(1);
			QLabel *QRWidget = new QLabel();
			QRWidget->setPixmap(QPixmap::fromImage(QR));
			ui->QRViewer->addWidget(QRWidget);
		}
	}
	if (ui->IPList->count() > 0)
	{
		// Select the first row
		ui->IPList->setCurrentRow(0);
		// And grab the focus
		ui->IPList->setFocus(Qt::OtherFocusReason);
	}
	ui->statusLabel->setText(message);
	connect(tcpServer, &QTcpServer::newConnection, this, &Server::handleConnection);
}

void Server::handleConnection()
{
	clientConnection = tcpServer->nextPendingConnection();
	QString connectionMessage;
	connectionMessage =
		tr("Connected to %1 at `%2 : %3`")
			.arg(clientConnection->peerName().isEmpty() ? "Unknown device" : clientConnection->peerName(),
				 clientConnection->peerAddress().toString(), QString::number(clientConnection->peerPort()));
	qDebug() << connectionMessage;
	ui->clientLabel->setText(connectionMessage);
	tcpServer->pauseAccepting();
	connect(clientConnection, &QAbstractSocket::disconnected, clientConnection, &QObject::deleteLater);
	connect(clientConnection, &QAbstractSocket::disconnected, this, [this]() { tcpServer->resumeAccepting(); });
	connect(clientConnection, &QAbstractSocket::disconnected, this,
			[this]() { ui->clientLabel->setText(tr("No client connected")); });
	connect(clientConnection, &QAbstractSocket::readyRead, this, &Server::serveClient);
}

void Server::serveClient()
{
	qDebug() << "Received: " << clientConnection->bytesAvailable() << "bytes";
	QByteArray request = clientConnection->readAll();
	qDebug() << "Request: " << request;
}
