#pragma once

#include <QLabel>
#include <QNetworkAccessManager>
#include <QUrl>

class Badge : public QLabel
{
	Q_OBJECT

  public:
	explicit Badge(QWidget *parent = nullptr);
	~Badge() override;

	// Load the badge image from a URL
	void loadBadge(const QString &imageUrl, const QString &linkUrl = "");

  protected:
	// Override mousePressEvent to handle clicks
	void mousePressEvent(QMouseEvent *event) override;

  private slots:
	void onNetworkReplyFinished();

  private:
	QNetworkAccessManager *networkManager;
	QString linkUrl;
};
