#pragma once

#include <QApplication>
#include <QDir>
#include <QFileInfo>

/**
 * @brief Returns the application data directory, based on the platform and build type.
 *
 * @details
 * - On Windows, it returns the application directory. (portable app)
 * - On Linux, it returns home with a subdir. (AppImage mounts in /tmp)
 * - In debug builds, it returns the application directory.
 *
 * @warning Instantiate the QApplication object first
 */
inline auto getAppDataDir()
{
#if defined(QT_DEBUG)
	return QApplication::applicationDirPath();
#elif defined(_WIN32)
	return QApplication::applicationDirPath();
#else
	return QFileInfo(QApplication::applicationDirPath()).dir().absolutePath();
#endif
}
