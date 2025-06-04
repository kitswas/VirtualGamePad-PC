#include "server.hpp"

#include "../../third-party-libs/QR-Code-generator/cpp/qrcodegen.hpp"
#include "../settings/settings_singleton.hpp"
#include "executor.hpp"
#include "ui_server.h"

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
 * This function uses [Nayuki's QR Code Generator
 * library](https://github.com/nayuki/QR-Code-generator/tree/master/cpp).\n Guided by:
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
	qDebug() << "QR code generated with size: " << s;
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

Server::Server(QWidget *parent) : QWidget(parent), ui(new Ui::Server)
{
	qInfo() << "Initializing TCP server";

	ui->setupUi(this);
	ui->IPList->viewport()->setAutoFillBackground(false);
	// delete this when stop button is clicked
	connect(ui->stopButton, &QPushButton::clicked, this, &Server::destroyServer);
	clientConnection = nullptr;
	tcpServer = new QTcpServer(this);
	isGamepadConnected = false;
	initServer();

	qDebug() << "Server widget initialized";
}

Server::~Server()
{
	// IMPORTANT: clientConnection should not be accessed when the server is closed
	if (isGamepadConnected && clientConnection != nullptr)
	{
		clientConnection->close(); // Close clientConnection gracefully
		clientConnection = nullptr;
		isGamepadConnected = false;
	}
	tcpServer->close(); // And then close the server
	qInfo() << "Server stopped.";
	tcpServer->deleteLater();
	delete ui;
}

void Server::initServer()
{
	qInfo() << "Starting TCP server initialization";

	tcpServer->setListenBacklogSize(0);
	int port = SettingsSingleton::instance().port();
	if (port < 1024 || port > 65535)
	{
		port = 0; // 0 means random port
	}

	if (!tcpServer->listen(QHostAddress::AnyIPv4, port))
	{
		QMessageBox::critical(this, tr("VGamepad Server"),
							  tr("Unable to start the server: %1.").arg(tcpServer->errorString()));
		close(); // Close the error dialog
		tcpServer->close();
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
			QRWidget->setPixmap(QPixmap::fromImage(
				createQR(tr("%1:%2").arg(entry.toString()).arg(tcpServer->serverPort()))));
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
	connect(ui->IPList, &QListWidget::currentItemChanged, this,
			[this](QListWidgetItem *current, QListWidgetItem *) {
				if (current != nullptr)
				{
					ui->QRViewer->setCurrentIndex(ui->IPList->row(current));
				}
			});
	connect(tcpServer, &QTcpServer::newConnection, this, &Server::handleConnection);
	qInfo() << "Server started successfully on port:" << tcpServer->serverPort();
}

void Server::handleConnection()
{
	clientConnection = tcpServer->nextPendingConnection();
	// disable Nagle's algorithm to avoid delay and bunching of small packages
	clientConnection->setSocketOption(QAbstractSocket::LowDelayOption, 1);
	isGamepadConnected = true;
	QString connectionMessage;
	connectionMessage =
		tr("Connected to %1 at `%2 : %3`")
			.arg(clientConnection->peerName().isEmpty() ? "Unknown device"
														: clientConnection->peerName(),
				 clientConnection->peerAddress().toString(),
				 QString::number(clientConnection->peerPort()));
	qInfo().noquote() << connectionMessage;
	ui->clientLabel->setText(connectionMessage);
	tcpServer->pauseAccepting();
	connect(clientConnection, &QAbstractSocket::disconnected, clientConnection,
			&QObject::deleteLater);
	connect(clientConnection, &QAbstractSocket::disconnected, this, [this]() {
		ui->clientLabel->setText(tr("No device connected"));
		qInfo() << "Device disconnected.";
		qDebug() << "Average Request Interval" << averageRequestInterval
				 << "ms, Request Count:" << requestCount;
		isGamepadConnected = false;
		tcpServer->resumeAccepting();
	});
	connect(clientConnection, &QAbstractSocket::readyRead, this, &Server::serveClient);

	qInfo() << "New client connection received";
}

void Server::serveClient()
{
#ifdef QT_DEBUG
	qDebug() << "Received: " << clientConnection->bytesAvailable() << "bytes";
#endif

	QByteArray request = clientConnection->readAll();

#ifdef QT_DEBUG
	qDebug() << "Request: " << request;
#endif

	QTime currentTime = QTime::currentTime();

	requestCount++;
	// Calculate performance metrics
	if (lastRequestTime.isValid())
	{
		int elapsed = lastRequestTime.msecsTo(currentTime);
		// Update average request interval, using running average
		averageRequestInterval += (elapsed - averageRequestInterval) / requestCount;
	}
	lastRequestTime = currentTime;

	vgp_data_exchange_gamepad_reading gamepad_reading =
		parse_gamepad_state(request.constData(), request.size());
	inject_gamepad_state(gamepad_reading);
}

void Server::destroyServer()
{
	this->deleteLater();
}
