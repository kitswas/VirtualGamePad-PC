#include "server.hpp"

#include "../../third-party-libs/QR-Code-generator/cpp/qrcodegen.hpp"
#include "../settings/settings_singleton.hpp"
#include "executor.hpp"
#include "ui_server.h"

#include <QByteArray>
#include <QDataStream>
#include <QDateTime>
#include <QHostAddress>
#include <QList>
#include <QMessageBox>
#include <QNetworkInterface>
#include <QThread>
#include <QTimer>

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
	ui->setupUi(this);
	ui->IPList->viewport()->setAutoFillBackground(false);
	// delete this when stop button is clicked
	QPushButton::connect(ui->stopButton, &QPushButton::clicked, this, &Server::destroyServer);
	clientConnection = nullptr;
	tcpServer = new QTcpServer(this);
	isGamepadConnected = false;

	// Initialize connection stats
	packetCount = 0;
	bytesReceived = 0;
	latency = 0;
	lastPacketTime = 0;

	// Create and connect the stats timer
	statsUpdateTimer = new QTimer(this);
	connect(statsUpdateTimer, &QTimer::timeout, this, &Server::updateConnectionStats);
	statsUpdateTimer->start(1000); // Update stats every second

	initServer();
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

	// Clean up timer
	if (statsUpdateTimer)
	{
		statsUpdateTimer->stop();
		statsUpdateTimer->deleteLater();
	}

	delete ui;
}

void Server::initServer()
{
	tcpServer->setListenBacklogSize(0);
	// Get port from settings with 8080 as default
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
	qInfo() << "Server started.";
}

void Server::handleConnection()
{
	clientConnection = tcpServer->nextPendingConnection();
	// disable Nagle's algorithm to avoid delay and bunching of small packages
	clientConnection->setSocketOption(QAbstractSocket::SocketOption::LowDelayOption, 1);
	// Keep alive is not required as the client sends a stream of data
	// Set receive buffer size to the size of the gamepad reading structure
	constexpr uint8_t num_elements = 8; // Number of elements in the buffer
	constexpr uint8_t bufferSize =
		(sizeof(vgp_data_exchange_gamepad_reading) * num_elements + 7) >> 3; // In bytes, rounded up

	clientConnection->setSocketOption(QAbstractSocket::SocketOption::ReceiveBufferSizeSocketOption,
									  bufferSize);

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
		ui->connectionStatsLabel->setText(tr("Connection status: No data"));
		qInfo() << "Device disconnected.";
		isGamepadConnected = false;
		packetCount = 0;
		bytesReceived = 0;
		latency = 0;
		lastPacketTime = 0;
		tcpServer->resumeAccepting();
	});
	connect(clientConnection, &QAbstractSocket::readyRead, this, &Server::serveClient);

	// Reset connection stats
	packetCount = 0;
	bytesReceived = 0;
	latency = 0;
	lastPacketTime = QDateTime::currentMSecsSinceEpoch();
	updateConnectionStats();
}

void Server::serveClient()
{
	qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
	qint64 packetLatency = 0;

	if (lastPacketTime > 0)
	{
		packetLatency = currentTime - lastPacketTime;
		latency = packetLatency; // Store the most recent latency value
	}

	lastPacketTime = currentTime;

	int receivedBytes = clientConnection->bytesAvailable();
	qDebug() << "Received: " << receivedBytes << "bytes";
	QByteArray request = clientConnection->readAll();
	qDebug() << "Request: " << request;

	// Update statistics
	packetCount++;
	bytesReceived += receivedBytes;

	vgp_data_exchange_gamepad_reading gamepad_reading =
		parse_gamepad_state(request.constData(), request.size());
	inject_gamepad_state(gamepad_reading);
}

void Server::destroyServer()
{
	this->deleteLater();
}

void Server::updateConnectionStats()
{
	if (isGamepadConnected && clientConnection != nullptr)
	{
		// Calculate data rate (bytes per second)
		double dataRate = bytesReceived / 1024.0; // Convert to KB

		QString statsMessage = tr("Connection status: **Active**  |  Latency: **%1 ms**  |  "
								  "Packets: **%2**  |  Data Rate: **%3 KB/s**")
								   .arg(latency)
								   .arg(packetCount)
								   .arg(QString::number(dataRate, 'f', 2));

		ui->connectionStatsLabel->setText(statsMessage);

		// Reset counters for next update period (but keep packet count cumulative)
		bytesReceived = 0;
	}
	else
	{
		ui->connectionStatsLabel->setText(tr("Connection status: **Inactive**"));
	}
}

void Server::startLatencyMeasurement()
{
	latencyTimer.start();
}
