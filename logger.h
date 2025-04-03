// Devanjali Das, Gurleen Bassali, Karan Modi, Zainab Jahangir

#ifndef LOGGER_H
#define LOGGER_H

#include <QString>
#include <QTextEdit>

// Manages logging of events and errors to log Widget
class Logger {
public:
    // Constructor
    Logger(QTextEdit* logWidget = nullptr);

    // Logs event to log widget
    void logEvent(const QString &message);

    // Logs error message to log widget
    void logError(const QString &error);

private:
    // Writes log entry
    void writeToLog(const QString &level, const QString &message);

    QTextEdit* m_logWidget;
};

#endif
