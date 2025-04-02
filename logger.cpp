// Devanjali Das, Gurleen Bassali, Karan Modi, Zainab Jahangir

#include "logger.h"
#include <QDateTime>

// Constructor
Logger::Logger(QTextEdit* logWidget) : m_logWidget(logWidget) {
    if (m_logWidget) {
        logEvent("Logger initialized");
    }
}

// Logs event to log widget
void Logger::logEvent(const QString &message) {
    writeToLog("INFO", message);
}

// Logs error message to log widget
void Logger::logError(const QString &error) {
    writeToLog("ERROR", error);
}

// Writes log entry
void Logger::writeToLog(const QString &level, const QString &message) {
    if (!m_logWidget) {
        return;
    }
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString formattedMessage = QString("[%1] [%2] %3").arg(timestamp, level, message);
    m_logWidget->append(formattedMessage);
}
