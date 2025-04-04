// Devanjali Das, Gurleen Bassali, Karan Modi, Zainab Jahangir

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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
    const QString correctPIN = "1234";
    Logger* logger;
    USBConnection* battery;

    // Helper functions

    // Displays the details of the provided profile in the UI
    void displayProfileDetails(const Profile &profile);

    // Sets the read-only state for profile detail fields
    void setDetailsReadOnly(bool readOnly);

    void updateProfileComboBox();


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
};


#endif // MAINWINDOW_H
