// Devanjali Das, Gurleen Bassali, Karan Modi, Zainab Jahangir

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCharts/QChartView>
QT_CHARTS_USE_NAMESPACE
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QtCharts/QValueAxis>
#include <QMainWindow>
#include <QDialog> // added include for QDialog
#include "ProfileManager.h"
#include "logger.h"
#include "battery.h"
#include "BolusCalculator.h"
#include "BolusCalculator.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    // Constructor
    MainWindow(QWidget *parent = nullptr);
    // Destructor
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    ProfileManager *profileManager;
    BolusCalculator *bolusCalc;
    ControlIQ *controlIQ;
    QTimer *controlIQTimer;

    const QString correctPIN = "1234";
    Logger* logger;
    bool chargingInProgress = false;

    USBConnection* battery;
    QDialog *batteryPopup = nullptr; // added member variable

    QLineSeries *cgmSeries;      // Persistent pointer for the CGM data series
    QValueAxis *axisX;           // X-axis for the time
    QValueAxis *axisY;           // Y-axis for the blood glucose level
    int timeCounter = 0;

    // Helper functions
    void displayProfileDetails(const Profile &profile); // Displays the details of the provided profile in the UI
    void setDetailsReadOnly(bool readOnly); // Sets the read-only state for profile detail fields
    void updateProfileComboBox();
    void showChargerPopup();
    void resetBolusCalculatorUI();


private slots:
    void onPowerButtonHeld();
    void onUnlockButtonClicked();
    void onBolusButtonClicked();
    void onHistoryButtonClicked();
    void onLogoButtonClicked();
    void option1Clicked();
    void option2Clicked();
    void onNewProfileClicked();
    void onProfileSelected();
    void onEditProfileClicked();
    void onSaveProfileClicked();
    void onDeleteProfileClicked();

    void updateBatteryDisplay();
    void onChargerButtonClicked();
    void onBatteryFullyCharged();

    void onCalculateDoseButtonClicked();
    void onConfirmBolusButtonClicked();

    void checkBatteryAfterDelay();
    void onControlIQTimerTimeout();
    void onFetchFromCGMButtonClicked();

    void onImmediateDoseDelivered();
    void onExtendedDoseCompleted();


};

#endif // MAINWINDOW_H
