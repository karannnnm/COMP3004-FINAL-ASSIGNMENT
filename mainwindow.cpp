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
    , controlIQ(nullptr)
    , controlIQTimer(nullptr)
    , batteryPopup(nullptr)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);  // Default page: power off screen

    // Instantiate BolusCalculator and pass in the existing ProfileManager reference
    bolusCalc = new BolusCalculator(*profileManager);

    // Instantiate ControlIQ
    controlIQ  = new ControlIQ();
    //controlIQ->fetchBolusData(*bolusCalc);
    controlIQ->linkCurrentBloodGlucoseLevel(*bolusCalc);
    controlIQ->fetchCurrentProfile(*bolusCalc);


    // Create and start the control iq timer (fires every 5 seconds)
    controlIQTimer = new QTimer(this);
    connect(controlIQTimer, SIGNAL(timeout()), this, SLOT(onControlIQTimerTimeout()));
    controlIQTimer->start(5000);

    // Initialize logger with the log widget from the UI
    logger = new Logger(ui->log);

    // Instantiate the battery simulation (USBConnection) and let MainWindow be its parent.
    battery = new USBConnection(this);
    battery->simulateBatteryDrain(); // Start battery drain immediately

    // Create a QTimer to update the battery display every 1 second.
    QTimer *batteryTimer = new QTimer(this);
    connect(batteryTimer, &QTimer::timeout, this, &MainWindow::updateBatteryDisplay);
    batteryTimer->start(1000);

    // Set up the CGM chart
    cgmSeries = new QLineSeries();
    QChart *chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(cgmSeries);

    // Configure chart axes
    axisY = new QValueAxis;
    axisY->setRange(2, 22); // Glucose levels range from 2 to 22 mmol/L.
    axisY->setTitleText("Glucose Level (mmol/L)");
    chart->addAxis(axisY, Qt::AlignLeft);
    cgmSeries->attachAxis(axisY);

    axisX = new QValueAxis;
    axisX->setTitleText("Time");
    chart->addAxis(axisX, Qt::AlignBottom);
    cgmSeries->attachAxis(axisX);

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
    connect(ui->fetchCGMButton, SIGNAL(clicked()), this, SLOT(onFetchFromCGMButtonClicked()));

    // Connect ControlIQ signals to MainWindow slots
    connect(controlIQ, SIGNAL(immediateDoseDelivered()), this, SLOT(onImmediateDoseDelivered()));
    connect(controlIQ, SIGNAL(extendedDoseCompleted()), this, SLOT(onExtendedDoseCompleted()));
    connect(controlIQ, SIGNAL(automaticBolusAdministered()), this, SLOT(onAutomaticBolusAdministered()));

    connect(ui->resumeBolusButton, SIGNAL(released()), this, SLOT(onResumeBolusButtonClicked()));
    connect(ui->pauseBolusButton, SIGNAL(released()), this, SLOT(onPauseBolusButtonClicked()));
    connect(ui->stopBolusButton,  SIGNAL(released()), this, SLOT(onStopBolusButtonClicked()));

}

// Destructor
MainWindow::~MainWindow()
{
   // delete profileManager;
    delete bolusCalc;
    delete controlIQ;
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
    if (!controlIQ->isGlucoseLevelSafe()) {
        QMessageBox::warning(this, "Unsafe Glucose Level", "Blood glucose level is too low for safe bolus delivery. Please wait until it recovers.");
        return;
    }

    // Cannot start new bolus if one is already in progress or paused
    BolusDeliveryStatus status = controlIQ->getBolusStatus();
    if (status == BolusDeliveryStatus::RUNNING || status == BolusDeliveryStatus::PAUSED) {
        QMessageBox::warning(this, "Bolus In Progress", "A bolus is already in progress or paused. Please complete or cancel the current bolus before starting a new one.");
        return;
    }

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
        QMessageBox::warning(this, "Invalid Input", "The sum of the immediate and extended percentages must equal 100.");
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

    logger->logEvent("Bolus confirmation successful: " + result); // Log the successful bolus confirmation with the calculated detail
    QMessageBox::information(this, "Bolus Calculation", result); // Display the results in an information popup

    // Update ControlIQ with the latest bolus calculator data
    controlIQ->fetchBolusData(*bolusCalc);
    controlIQ->linkCurrentBloodGlucoseLevel(*bolusCalc);
    controlIQ->fetchCurrentProfile(*bolusCalc);

    // Start Bolus
    controlIQ->startBolus();

    resetBolusCalculatorUI();

    // Update home screen UI profile
    QString currentProfileText = ui->profilesComboBox->currentText();
    if (currentProfileText.trimmed().isEmpty()) {
        ui->profileValue->setText("Default Profile");
    } else {
        ui->profileValue->setText(currentProfileText);
    }
}

void MainWindow::resetBolusCalculatorUI()
{
    // Clear the input fields
    ui->bloodGlucoseLevelValue->setValue(0.0);
    ui->carbohydratesValue->setValue(0.0);
    ui->suggestedDoseValue->clear();
    ui->overrideDoseValue->setValue(0.0);

    // Reset these fields to zero or other defaults
    ui->quickBolusPercent->setValue(0);
    ui->extendedBolusPercent->setValue(0);
    ui->extendedBolusHours->setValue(0);

    // Disable the override dose, quick bolus, and extended bolus fields
    ui->overrideDoseValue->setEnabled(false);
    ui->quickBolusPercent->setEnabled(false);
    ui->extendedBolusPercent->setEnabled(false);
    ui->extendedBolusHours->setEnabled(false);
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

void MainWindow::onControlIQTimerTimeout()
{
    controlIQ->mimicGlucoseSpike(); // Mimic natural glucose spik
    controlIQ->monitorGlucoseLevel(); // Monitor glucose level and deliver basal insulin accordingly
    controlIQ->deliverExtendedBolus(); // Deliver extended bolus insulin if a bolus has been started --> If no bolus running function will return immediately
    controlIQ->predictBolusRequired(); // Predict if we need automatic correction bolus

    double currentBG = controlIQ->getCurrentBloodGlucose();

    // Update BG Level
    ui->glucoseLevel->setText(QString::number(currentBG));

    // Update IOB
    controlIQ->simulateIOBFluctuation();
    ui->iobValue->setText(QString::number(controlIQ->getIOB(), 'f', 1));

    // Update CGM graph
    timeCounter++;
    cgmSeries->append(timeCounter, currentBG);

    // Show the last 20 time units
    if (timeCounter > 20) {
        axisX->setRange(timeCounter - 20, timeCounter);
    } else {
        axisX->setRange(0, 20);
    }

    // Update Insulin Fill Gauge
    controlIQ->simulateInsulinFillGaugeFluctuation();

    int insulinLevel = controlIQ->getInsulinFillGauge();
    ui->insulinFillGaugeValue->setText(QString::number(insulinLevel));
    if (insulinLevel <= 0 && !insulinPopup) {
        insulinPopup = new QDialog(this);
        insulinPopup->setModal(true);
        insulinPopup->setWindowTitle("Low Insulin Warning");

        // Create a vertical layout for the dialog.
        QVBoxLayout *layout = new QVBoxLayout(insulinPopup);

        // Add a label with the warning message.
        QLabel *warningLabel = new QLabel("The insulin fill gauge is empty.\nPlease refill the insulin.", insulinPopup);
        layout->addWidget(warningLabel);

        // Add a "Refill Insulin" button.
        QPushButton *refillButton = new QPushButton("Refill Insulin", insulinPopup);
        layout->addWidget(refillButton);

        connect(refillButton, SIGNAL(released()), this, SLOT(onRefillInsulinClicked()));

        insulinPopup->setLayout(layout);
        insulinPopup->exec(); // Block until the user responds.
    }
}


void MainWindow::onFetchFromCGMButtonClicked()
{
    // Get the current glucose level from CGM
    double currentGlucose = bolusCalc->fetchCurrentGlucoseLevelFromCGM();
    double randCarb = controlIQ->generateRandomDouble();
    ui->bloodGlucoseLevelValue->setValue(currentGlucose);
    ui->carbohydratesValue->setValue(randCarb);

}

void MainWindow::onImmediateDoseDelivered()
{
    QMessageBox::information(this, "Bolus Update", "Immediate insulin dose delivered. Starting extended insulin delivery (if applicable).");
}

void MainWindow::onExtendedDoseCompleted()
{
    QMessageBox::information(this, "Bolus Update", "Extended insulin delivery completed!");
}

void MainWindow::onAutomaticBolusAdministered()
{
    QMessageBox::warning(this, "BG Level Too High", "BG Level Predicted to Reach Maximum - Administering Automatic Correction Bolus");
}

void MainWindow::onRefillInsulinClicked()
{
    // Reset the insulin fill gauge to 250 (its maximum)
    controlIQ->setInsulinFillGauge(250);

    // Update the UI
    ui->insulinFillGaugeValue->setText(QString::number(250));

    // Close and delete the popup
    if (insulinPopup) {
        insulinPopup->close();
        delete insulinPopup;
        insulinPopup = nullptr;
    }
}

void MainWindow::onResumeBolusButtonClicked()
{
    // Check if the blood glucose level is safe before resuming bolus.
    if (!controlIQ->isGlucoseLevelSafe()) {
        QMessageBox::warning(this, "Unsafe Glucose Level", "Blood glucose is too low. Cannot resume bolus delivery.");
        return;
    }
    controlIQ->resumeBolus();
}

void MainWindow::onPauseBolusButtonClicked()
{
    controlIQ->userPaused = true;
    controlIQ->pauseBolus();
}

void MainWindow::onStopBolusButtonClicked()
{
    controlIQ->suspendBolus();
    QMessageBox::warning(this, "Bolus Stopped", "Bolus STOPPED");
}
