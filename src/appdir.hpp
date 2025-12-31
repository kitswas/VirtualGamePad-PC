#pragma once

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>

/**
 * @brief Determines if the application should run in portable mode.
 *
 * @details
 * Portable mode is determined by the build-time flag PORTABLE_BUILD.
 *
 * In portable mode, settings and data are stored alongside the executable.
 * In installable mode, standard OS locations are used (~/.config, %APPDATA%, etc.).
 */
inline bool isPortableMode()
{
#ifdef PORTABLE_BUILD
	return true;
#else
	return false;
#endif
}

/**
 * @brief Returns the directory for configuration files (settings.ini).
 *
 * @details
 * - Portable mode: Application directory (Windows) or parent directory (Linux)
 * - Installable mode:
 *   - Linux: ~/.config/VirtualGamePad/
 *   - Windows: %APPDATA%/VirtualGamePad/
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
		return QApplication::applicationDirPath();
#else
		// Linux: use parent directory (dist/ instead of dist/bin/)
		return QFileInfo(QApplication::applicationDirPath()).dir().absolutePath();
#endif
	}
	else
	{
		// Installable mode: use standard OS locations
		QString configPath =
			QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
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
 *   - Windows: %APPDATA%/VirtualGamePad/ (same as config)
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
