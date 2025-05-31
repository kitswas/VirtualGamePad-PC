#include "badge.hpp"

#include <QDesktopServices>
#include <QMouseEvent>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPixmap>

Badge::Badge(QWidget *parent) : QLabel(parent), networkManager(new QNetworkAccessManager(this))
{
	setCursor(Qt::PointingHandCursor);
}

void Badge::loadBadge(const QString &imageUrl, const QString &linkUrl)
{
	this->linkUrl = linkUrl;

	// Fix the "most vexing parse" issue by using braces initialization
	QNetworkRequest request{QUrl(imageUrl)};
	QNetworkReply *reply = networkManager->get(request);

	connect(reply, &QNetworkReply::finished, this, &Badge::onNetworkReplyFinished);
}

void Badge::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton && !linkUrl.isEmpty())
	{
		QDesktopServices::openUrl(QUrl(linkUrl));
		event->accept();
	}
	else
	{
		QLabel::mousePressEvent(event);
	}
}

void Badge::onNetworkReplyFinished()
{
	QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
	if (!reply)
		return;

	if (reply->error() == QNetworkReply::NoError)
	{
		QPixmap pixmap;
		pixmap.loadFromData(reply->readAll());
		setPixmap(pixmap);

		if (!linkUrl.isEmpty())
		{
			setToolTip(linkUrl);
		}
	}

	reply->deleteLater();
}
