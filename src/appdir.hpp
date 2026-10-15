#pragma once

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QStandardPaths>
#include <QTemporaryFile>
#include <cstdlib>

/**
 * @brief Determines if the application should run in portable mode.
 *
 * @details
 * Portable mode is determined by the build-time flag PORTABLE_BUILD.
 *
 * In portable mode, settings and data are stored alongside the executable.
 * In installable mode, standard OS locations are used (~/.config, %LOCALAPPDATA%, etc.).
 */
constexpr bool isPortableMode()
{
#ifdef PORTABLE_BUILD
	return true;
#else
	return false;
#endif
}

/**
 * @brief Checks writability by creating a file.
 * @details QFileInfo::isWritable() ignores NTFS ACLs unless a global flag is set, so it cannot be trusted.
 */
inline bool isDirWritable(const QString &path)
{
	QTemporaryFile probe(QDir(path).filePath("write_probe_XXXXXX"));
	return probe.open();
}

/**
 * @brief Returns the directory for configuration files (settings.ini).
 *
 * @details
 * - Portable mode: Application directory (Windows) or parent directory (Linux)
 * - Installable mode:
 *   - Linux: ~/.config/VirtualGamePad/
 *   - Windows: %LOCALAPPDATA%/<org>/<app>/ (Qt AppConfigLocation)
 * - Debug builds: Always use application directory
 *
 * @warning Instantiate the QApplication object first
 */
inline QString getConfigDir()
{
#if defined(QT_DEBUG)
	// Debug builds always use application directory
	return QApplication::applicationDirPath();
#else
	if (isPortableMode())
	{
		// Portable mode: store alongside executable
#if defined(_WIN32)
		const QString portableDir = QApplication::applicationDirPath();
#else
		// Linux: use parent directory (dist/ instead of dist/bin/)
		const QString portableDir = QFileInfo(QApplication::applicationDirPath()).dir().absolutePath();
#endif
		// A portable build placed in a read-only location (Program Files, /usr, AppImage mount)
		// must fail loudly, not silently lose settings.
		if (isDirWritable(portableDir))
		{
			return portableDir;
		}
		const QString message = QString("Portable build cannot write to its directory:\n%1\n\n"
										"Move the app to a writable folder, or use a build made with "
										"-DPORTABLE_BUILD=OFF for installed packages.")
									.arg(portableDir);
		qCritical().noquote() << message;
		QMessageBox::critical(nullptr, "VirtualGamePad cannot start", message);
		std::exit(EXIT_FAILURE);
	}
	else
	{
		// Installable mode: use standard OS locations
		QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
		QDir dir(configPath);
		if (!dir.exists())
		{
			dir.mkpath(".");
		}
		return configPath;
	}
#endif
}

/**
 * @brief Returns the directory for application data files (profiles, etc.).
 *
 * @details
 * - Portable mode: Same as config directory
 * - Installable mode:
 *   - Linux: ~/.local/share/VirtualGamePad/
 *   - Windows: same as config directory
 * - Debug builds: Always use application directory
 *
 * @warning Instantiate the QApplication object first
 */
inline QString getDataDir()
{
#if defined(QT_DEBUG)
	// Debug builds always use application directory
	return QApplication::applicationDirPath();
#else
	if (isPortableMode())
	{
		// Portable mode: same as config directory
		return getConfigDir();
	}
	else
	{
		// Installable mode: use standard OS locations
#if defined(_WIN32)
		// Windows: use AppConfigLocation (same as config dir)
		QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
#else
		// Linux: use AppDataLocation for separation
		QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
#endif
		QDir dir(dataPath);
		if (!dir.exists())
		{
			dir.mkpath(".");
		}
		return dataPath;
	}
#endif
}
