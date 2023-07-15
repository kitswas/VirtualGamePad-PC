#include "server.hpp"

#include "ui_server.h"

#include "../../third-party-libs/QR-Code-generator/cpp/qrcodegen.hpp"

#include <QByteArray>
#include <QDataStream>
#include <QHostAddress>
#include <QList>
#include <QMessageBox>
#include <QNetworkInterface>
#include <QThread>

/**
 * @brief Creates a QR code from a string
 *
 * This function uses [Nayuki's QR Code Generator library](https://github.com/nayuki/QR-Code-generator/tree/master/cpp).\n
 * Guided by:
 * https://stackoverflow.com/a/39951669/8659747
 * @param data The string to encode
 * @param border The padding around the QR code (in pixels)
 * @param scalingFactor The scaling factor for the final image
 * @return QImage
 */
QImage createQR(const QString &data, const int border = 1, const uint scalingFactor = 10)
{
	char *str = data.toUtf8().data();
	qrcodegen::QrCode qr = qrcodegen::QrCode::encodeText(str, qrcodegen::QrCode::Ecc::HIGH);
	const int s = qr.getSize(); // s is the length of a side of the QR code
	qDebug() << "QR side:" << s;
	QImage image(s + 2 * border, s + 2 * border, QImage::Format_Mono);
	image.setColor(1, QColor("black").rgb());
	image.setColor(0, QColor("white").rgb());
	image.fill(0); // Whitewash
	for (int y = 0; y < s; y++)
	{
		for (int x = 0; x < s; x++)
		{
			const int color = qr.getModule(x, y); // 0 for white, 1 for black
			image.setPixel(x + border, y + border, color);
		}
	}
	return image.scaled(image.size() * scalingFactor);
}

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
	QString message = tr("**Warning:** The server will stop if you close this window.\n\n");
	message += tr("The server is running on\n\nPort: `%1`\n\nAt the following IP Address(es):\n\n")
				   .arg(tcpServer->serverPort());
	const QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
	for (const QHostAddress &entry : ipAddressesList)
	{
		if (entry.isGlobal())
		{
			ui->IPList->addItem(tr("%1").arg(entry.toString()));
			QLabel *QRWidget = new QLabel();
			QRWidget->setPixmap(
				QPixmap::fromImage(createQR(tr("%1:%2").arg(entry.toString()).arg(tcpServer->serverPort()))));
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
	connect(ui->IPList, &QListWidget::currentItemChanged, this, [this](QListWidgetItem *current, QListWidgetItem *) {
		if (current != nullptr)
		{
			ui->QRViewer->setCurrentIndex(ui->IPList->row(current));
		}
	});
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
