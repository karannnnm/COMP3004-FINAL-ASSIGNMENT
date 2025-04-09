// Devanjali Das, Gurleen Bassali, Karan Modi, Zainab Jahangir

#include "mainwindow.h"
#include <QtCharts/QChartView>
QT_CHARTS_USE_NAMESPACE
#include "ui_mainwindow.h"
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QtCharts/QValueAxis>
#include <QMessageBox>
#include <QInputDialog>
#include <QVBoxLayout>
#include <QTimer>
#include <QDebug> // Added for outputting to application output
QT_CHARTS_USE_NAMESPACE

// Constructor
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , profileManager(new ProfileManager)
    , bolusCalc(nullptr)
    , batteryPopup(nullptr)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);  // Default page: power off screen

    // Instantiate BolusCalculator and pass in the existing ProfileManager reference
    bolusCalc = new BolusCalculator(*profileManager);

    // Initialize logger with the log widget from the UI
    logger = new Logger(ui->log);

    // Instantiate the battery simulation (USBConnection) and let MainWindow be its parent.
    battery = new USBConnection(this);

    // Start battery drain immediately
    battery->simulateBatteryDrain();

    // Create a QTimer to update the battery display every 1 second.
    QTimer *batteryTimer = new QTimer(this);
    connect(batteryTimer, &QTimer::timeout, this, &MainWindow::updateBatteryDisplay);
    batteryTimer->start(1000);

    // Set up the CGM chart
    QLineSeries *series = new QLineSeries();
    series->append(0, 3.4);
    series->append(1, 3.2);
    series->append(2, 3.0);
    series->append(3, 3.2);
    series->append(4, 20);

    QChart *chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(series);

    // Configure chart axes
    QValueAxis *axisY = new QValueAxis;
    axisY->setRange(2, 22); // Glucose levels range from 2 to 22 mmol/L.
    axisY->setTitleText("Glucose Level (mmol/L)");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    QValueAxis *axisX = new QValueAxis;
    axisX->setTitleText("Time");
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    chart->setTitle("CGM Data");
    ui->chartView->setChart(chart);
    ui->chartView->setRenderHint(QPainter::Antialiasing);

    // Set up options drop-down menu
    QMenu *dropDownMenu = new QMenu(this);
    dropDownMenu->addAction("Insulin Delivery Settings", this, SLOT(option1Clicked()));
    dropDownMenu->addAction("Power Off", this, SLOT(option2Clicked()));
    ui->optionsButton->setMenu(dropDownMenu);

    // Initialize profile detail fields
    ui->profileNameValue->setReadOnly(true);
    ui->basalRateValue->clear();
    ui->carbRatioValue->clear();
    ui->correctionValue->clear();
    ui->targetGlucoseValue->clear();
    setDetailsReadOnly(true);

    // Set log widget to read-only
    ui->log->setReadOnly(true);

    // Connect UI buttons to their respective slots
    connect(ui->powerButton, SIGNAL(released()), this, SLOT(onPowerButtonHeld()));
    connect(ui->unlockButton, SIGNAL(released()), this, SLOT(onUnlockButtonClicked()));
    connect(ui->bolusButton, SIGNAL(released()), this, SLOT(onBolusButtonClicked()));
    connect(ui->historyButton, SIGNAL(released()), this, SLOT(onHistoryButtonClicked()));
    connect(ui->tandemLogoButton, SIGNAL(released()), this, SLOT(onLogoButtonClicked()));

    connect(ui->newProfileButton, SIGNAL(released()), this, SLOT(onNewProfileClicked()));
    connect(ui->profileList, SIGNAL(itemSelectionChanged()), this, SLOT(onProfileSelected()));
    connect(ui->editProfileButton, SIGNAL(released()), this, SLOT(onEditProfileClicked()));
    connect(ui->saveProfileButton, SIGNAL(released()), this, SLOT(onSaveProfileClicked()));
    connect(ui->deleteProfileButton, SIGNAL(released()), this, SLOT(onDeleteProfileClicked()));

    connect(ui->chargerButton, SIGNAL(released()), this, SLOT(onChargerButtonClicked()));
    connect(battery, SIGNAL(batteryFullyCharged()), this, SLOT(onBatteryFullyCharged()));

    connect(ui->calculateDoseButton, SIGNAL(released()), this, SLOT(onCalculateDoseButtonClicked()));
    connect(ui->confirmBolusButton, SIGNAL(released()), this, SLOT(onConfirmBolusButtonClicked()));
}

// Destructor
MainWindow::~MainWindow()
{
    delete profileManager;
    delete ui;
    delete logger;
}

// Updates the batteryValue label with the current battery level
void MainWindow::updateBatteryDisplay()
{
    int level = battery->getBatteryLevel();
    ui->batteryValue->setText(QString("%1%").arg(level));

    if (level == 0)
    {
        QTimer::singleShot(1000, this, SLOT(checkBatteryAfterDelay()));
    }

    // Low battery warning
    static bool warningShown = false;
    if (level < 20 && !warningShown) {
        warningShown = true;
        logger->logEvent("Battery is below 20%. Please charge.");
        qDebug() << "Battery is below 20%. Please charge.";
        QMessageBox::warning(this, "Low Battery", "Battery is below 20%. Please charge.");
    } else if (level >= 20) {
        warningShown = false;
    }

    // Only log "fully charged" once, until battery level dips below 100 again
    static bool batteryFullLogged = false;
    if (level == 100 && !batteryFullLogged) {
        logger->logEvent("Battery fully charged. Charging stopped. Battery drain simulation resumed.");
        qDebug() << "Battery fully charged. Charging stopped. Battery drain simulation resumed.";
        batteryFullLogged = true;
    } else if (level < 100) {
        batteryFullLogged = false;
    }
}

// New slot: checks battery level after a delay
void MainWindow::checkBatteryAfterDelay()
{
    if (battery->getBatteryLevel() == 0 && !batteryPopup && !chargingInProgress) {
        showChargerPopup();
    }
}

// Called when the chargerButton is clicked; initiates the charger connection
void MainWindow::onChargerButtonClicked()
{
    if (batteryPopup) {
        batteryPopup->close();
        batteryPopup = nullptr;
    }
    // Set the charging flag to true so that the popup is not shown later
    chargingInProgress = true;

    // Re-enable all buttons since charger is being connected
    ui->bolusButton->setEnabled(true);
    ui->historyButton->setEnabled(true);
    ui->optionsButton->setEnabled(true);
    ui->chargerButton->setEnabled(false);
    logger->logEvent("Charger button clicked. Initiating charger connection.");
    qDebug() << "Charger button clicked. Initiating charger connection.";
    battery->connectCharger();
}

void MainWindow::onBatteryFullyCharged() {
    ui->chargerButton->setEnabled(true);
    chargingInProgress = false;
}

// Called when the power button is held to transition from power off screen
void MainWindow::onPowerButtonHeld()
{
    ui->stackedWidget->setCurrentIndex(1);
}

// Called when the unlock button is clicked; verifies the PIN
void MainWindow::onUnlockButtonClicked()
{
    QString enteredPIN = ui->passwordValue->text();
    if(enteredPIN == correctPIN) {
        // Correct PIN: transition to home screen
        ui->stackedWidget->setCurrentIndex(2);
        ui->passwordValue->clear();
        ui->tandemLogoButton->setEnabled(true);
    } else {
        QMessageBox::warning(this, "Incorrect PIN", "The PIN you entered is incorrect. Please try again.");
        ui->passwordValue->clear();
    }
}

// Called when the bolus button is clicked; transitions to the bolus screen
void MainWindow::onBolusButtonClicked()
{
    logger->logEvent("Bolus button clicked. Switching to bolus screen.");
    qDebug() << "Bolus button clicked. Switching to bolus screen.";
    ui->stackedWidget->setCurrentIndex(3);
}

// Called when the history button is clicked; transitions to the history screen
void MainWindow::onHistoryButtonClicked()
{
    ui->stackedWidget->setCurrentIndex(5);
}

// Called when the Tandem logo is clicked; returns to the home screen
void MainWindow::onLogoButtonClicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}

// Called when the "Insulin Delivery Settings" option is selected
void MainWindow::option1Clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
}

void MainWindow::option2Clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->tandemLogoButton->setEnabled(false);
}

void MainWindow::setDetailsReadOnly(bool readOnly)
{
    ui->basalRateValue->setReadOnly(readOnly);
    ui->carbRatioValue->setReadOnly(readOnly);
    ui->correctionValue->setReadOnly(readOnly);
    ui->targetGlucoseValue->setReadOnly(readOnly);
}

// Displays details of provided profile onto UI
void MainWindow::displayProfileDetails(const Profile &profile)
{
    ui->profileNameValue->setText(QString::fromStdString(profile.getName()));
    ui->basalRateValue->setValue(profile.getBasalRate());
    ui->carbRatioValue->setValue(profile.getCarbRatio());
    ui->correctionValue->setValue(profile.getCorrectionFactor());
    ui->targetGlucoseValue->setValue(profile.getTargetGlucose());
}

// Called to create a new profile using user input
void MainWindow::onNewProfileClicked()
{
    QString name = QInputDialog::getText(this, "New Profile", "Enter profile name:");
    if (name.isEmpty())
        return;

    bool ok;
    double basalRate = QInputDialog::getDouble(this, "New Profile", "Enter basal rate:", 0.0, 0.0, 10000.0, 2, &ok);
    if (!ok)
        return;

    double correctionFactor = QInputDialog::getDouble(this, "New Profile", "Enter correction factor:", 0.0, 0.0, 10000.0, 2, &ok);
    if (!ok)
        return;

    double carbRatio = QInputDialog::getDouble(this, "New Profile", "Enter carb ratio:", 0.0, 0.0, 10000.0, 2, &ok);
    if (!ok)
        return;

    double targetGlucose = QInputDialog::getDouble(this, "New Profile", "Enter target glucose:", 0.0, 0.0, 10000.0, 2, &ok);
    if (!ok)
        return;

    // Create a new profile and add it to the manager
    Profile newProfile(name.toStdString(), basalRate, correctionFactor, carbRatio, targetGlucose);
    profileManager->addProfile(newProfile);

    // Add the new profile to the list widget
    QListWidgetItem *item = new QListWidgetItem(name, ui->profileList);
    item->setData(Qt::UserRole, name);

    // Log the profile creation even
    logger->logEvent("Profile created: " + name);
    qDebug() << "Profile created:" << name;

    // Update the bolus calculator combo box.
    updateProfileComboBox();
}

// Called when a profile is selected from the list; displays its details
void MainWindow::onProfileSelected()
{
    QListWidgetItem *selectedItem = ui->profileList->currentItem();
    if (!selectedItem)
        return;

    // Retrieve and display details of the selected profile
    QString profileName = selectedItem->data(Qt::UserRole).toString();
    Profile* profile = profileManager->getProfile(profileName.toStdString());
    if (profile) {
        displayProfileDetails(*profile);
        // Log profile selection.
        logger->logEvent("Profile selected: " + profileName);
        qDebug() << "Profile selected:" << profileName;
    }
}

void MainWindow::onEditProfileClicked()
{
    QListWidgetItem *selectedItem = ui->profileList->currentItem();
    if (!selectedItem)
        return;

    // Log that profile editing has been initiated
    QString editMsg = "Profile editing initiated for: " + selectedItem->data(Qt::UserRole).toString();
    logger->logEvent(editMsg);
    qDebug() << editMsg;
    setDetailsReadOnly(false);
}

// Called to save changes made to a profilE
void MainWindow::onSaveProfileClicked()
{
    QListWidgetItem *selectedItem = ui->profileList->currentItem();
    if (!selectedItem)
        return;

    // Retrieve updated values from UI
    QString profileName = selectedItem->data(Qt::UserRole).toString();
    double newBasal = ui->basalRateValue->text().toDouble();
    double newCarbRatio = ui->carbRatioValue->text().toDouble();
    double newCorrFactor = ui->correctionValue->text().toDouble();
    double newTargetGlucose = ui->targetGlucoseValue->text().toDouble();

    // Update the profile in the manager
    bool success = profileManager->updateProfile(profileName.toStdString(), newBasal, newCorrFactor, newCarbRatio, newTargetGlucose);
    if (success) {
        setDetailsReadOnly(true);
        QString successMsg = "Profile updated successfully: " + profileName;
        logger->logEvent(successMsg);
        qDebug() << successMsg;
        QMessageBox::information(this, "Success", "Profile updated successfully.");
    } else {
        QString failMsg = "Profile update failed for: " + profileName;
        logger->logEvent(failMsg);
        qDebug() << failMsg;
        QMessageBox::warning(this, "Error", "Profile update failed.");
    }
}

// Called to delete the selected profile
void MainWindow::onDeleteProfileClicked()
{
    QListWidgetItem *selectedItem = ui->profileList->currentItem();
    if (!selectedItem)
        return;

    QString profileName = selectedItem->data(Qt::UserRole).toString();
    int ret = QMessageBox::question(this, "Delete Profile",
                                    "Are you sure you want to delete the profile '" + profileName + "'?");
    if (ret == QMessageBox::Yes) {
        bool success = profileManager->deleteProfile(profileName.toStdString());
        if (success) {
            // Log profile deletion.
            QString delMsg = "Profile deleted: " + profileName;
            logger->logEvent(delMsg);
            qDebug() << delMsg;

            delete selectedItem;
            ui->profileList->blockSignals(true);
            ui->profileList->clearSelection();
            ui->profileList->blockSignals(false);
            ui->profileNameValue->clear();
            ui->basalRateValue->clear();
            ui->carbRatioValue->clear();
            ui->correctionValue->clear();
            ui->targetGlucoseValue->clear();

            // Update the bolus calculator box if profile deleted
            updateProfileComboBox();
        }
    }
}

void MainWindow::updateProfileComboBox() {
    ui->profilesComboBox->clear();
    const auto& profiles = profileManager->getProfiles();
    for (const auto &profile : profiles) {
        ui->profilesComboBox->addItem(QString::fromStdString(profile.getName()));
    }
}

void MainWindow::onCalculateDoseButtonClicked()
{
    QString selectedProfile = ui->profilesComboBox->currentText();

    // Log the beginning of bolus dose calculation
    logger->logEvent("Starting bolus dose calculation for profile: " + selectedProfile);
    qDebug() << "Starting bolus dose calculation for profile:" << selectedProfile;

    // Tell BolusCalculator to load that profile
    bolusCalc->getProfileByName(selectedProfile.toStdString());

    // Retrieve user input for current BG and carbs
    double currentBG = ui->bloodGlucoseLevelValue->value();
    double totalCarbs = ui->carbohydratesValue->value();

    // Pass these inputs to BolusCalculator
    bolusCalc->setCurrentGlucoseLevel(currentBG);
    bolusCalc->setTotalCarbs(totalCarbs);

    // Perform the calculation
    double suggestedDose = bolusCalc->calculateBolus();

    // Log the calculated suggested dose
    logger->logEvent("Suggested bolus dose calculated: " + QString::number(suggestedDose));
    qDebug() << "Suggested bolus dose calculated:" << suggestedDose;

    // Display the result in the “Suggested Dose” label/line edit
    ui->suggestedDoseValue->setText(QString::number(suggestedDose));
    ui->overrideDoseValue->setEnabled(true);
    ui->quickBolusPercent->setEnabled(true);
    ui->extendedBolusPercent->setEnabled(true);
    ui->extendedBolusHours->setEnabled(true);
}

void MainWindow::onConfirmBolusButtonClicked()
{
    logger->logEvent("Bolus confirmation initiated.");
    qDebug() << "Bolus confirmation initiated.";

    // Check if the suggested dose field is empty
    if (ui->suggestedDoseValue->text().isEmpty()) {
        logger->logEvent("Bolus confirmation failed: Suggested dose is empty. Please calculate the dose first.");
        qDebug() << "Bolus confirmation failed: Suggested dose is empty.";
        QMessageBox::warning(this, "Error", "No suggested dose calculated. Please calculate the dose first.");
        return;
    }

    int immediatePercent = ui->quickBolusPercent->value();
    int extendedPercent = ui->extendedBolusPercent->value();
    int duration = ui->extendedBolusHours->value();

    // Check that the percentages add up to 100
    if (immediatePercent + extendedPercent != 100) {
        logger->logEvent("Bolus confirmation failed: The sum of immediate and extended percentages does not equal 100.");
        qDebug() << "Bolus confirmation failed: Percentages do not add up to 100.";
        QMessageBox::warning(this, "Invalid Input",
                             "The sum of the immediate and extended percentages must equal 100.");
        return;
    }

    // Check if the override dose field is filled
    double overrideDose = ui->overrideDoseValue->value();
    if (overrideDose > 0) {
        bolusCalc->setPercentOfImmediateDose(immediatePercent);
        bolusCalc->setDurationForExtendedBolus(duration);
        bolusCalc->setPercentOfExtendedDose(extendedPercent);
        bolusCalc->overrideBolus(overrideDose);
    } else {
        // Update the BolusCalculator with these values
        bolusCalc->setPercentOfImmediateDose(immediatePercent);
        bolusCalc->calculateImmediateBolusDose();
        bolusCalc->setDurationForExtendedBolus(duration);
        bolusCalc->setPercentOfExtendedDose(extendedPercent);
        bolusCalc->calculateExtendedBolusDose();
    }

    QString result;
    result += QString("Immediate Insulin Dose: %1 units at meal time\n")
                  .arg(bolusCalc->getImmediateDose(), 0, 'f', 1);
    result += QString("Extended Insulin Delivery: %1 units per hour for %2 hours\n")
                  .arg(bolusCalc->getExtendedDosePerHour(), 0, 'f', 2)
                  .arg(duration);
    result += QString("Total Insulin Administered: %1 units")
                  .arg(bolusCalc->getTotalBolusAfterIOB(), 0, 'f', 0);

    // Log the successful bolus confirmation with the calculated details
    logger->logEvent("Bolus confirmation successful: " + result);
    qDebug() << "Bolus confirmation successful:" << result;

    // Display the results in an information popup
    QMessageBox::information(this, "Bolus Calculation", result);
}

void MainWindow::showChargerPopup()
{
    if (batteryPopup)
        return;
    // Disable buttons except for the charger button
    ui->bolusButton->setEnabled(false);
    ui->historyButton->setEnabled(false);
    ui->optionsButton->setEnabled(false);

    // Create the popup
    batteryPopup = new QDialog(this);
    batteryPopup->setWindowModality(Qt::NonModal);
    batteryPopup->setWindowTitle("Battery Empty");
    QVBoxLayout *layout = new QVBoxLayout(batteryPopup);
    QLabel *label = new QLabel("Battery is 0%. Please connect the charger.", batteryPopup);
    layout->addWidget(label);
    batteryPopup->show();
}
