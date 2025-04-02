#include "mainwindow.h"
#include <QtCharts/QChartView>
QT_CHARTS_USE_NAMESPACE
#include "ui_mainwindow.h"
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QtCharts/QValueAxis>
#include <QMessageBox>
#include <QInputDialog>

QT_CHARTS_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , profileManager(new ProfileManager)

{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);  // Set page at index 0 (power off screen) as default

    //cgm chart
    QLineSeries *series = new QLineSeries();
    series->append(0, 3.4);
    series->append(1, 3.2);
    series->append(2, 3.0);
    series->append(3, 3.2);
    series->append(4, 20);

    QChart *chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(series);

    //chart axes
    QValueAxis *axisY = new QValueAxis;
    axisY->setRange(2, 22); // Set the range from 2 to 22
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

    //options drop down menu
    QMenu *dropDownMenu = new QMenu(this);
    dropDownMenu->addAction("Insulin Delivery Settings", this, SLOT(option1Clicked()));
    dropDownMenu->addAction("Power Off", this, SLOT(option2Clicked()));
    ui->optionsButton->setMenu(dropDownMenu);

    //can only change profile info in edit mode
    ui->profileNameValue->setReadOnly(true); //can never edit name
    ui->basalRateValue->clear();
    ui->carbRatioValue->clear();
    ui->correctionValue->clear();
    ui->targetGlucoseValue->clear();
    setDetailsReadOnly(true); //can maybe add default values

    //data log
    ui->log->setReadOnly(true);


    //signals & slots
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
}
MainWindow::~MainWindow()
{
    delete profileManager;
    delete ui;
}

void MainWindow::onPowerButtonHeld()
{
    ui->stackedWidget->setCurrentIndex(1);
}

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

void MainWindow::onBolusButtonClicked()
{
    ui->stackedWidget->setCurrentIndex(3);
}

void MainWindow::onHistoryButtonClicked()
{
    ui->stackedWidget->setCurrentIndex(5);
}

void MainWindow::onLogoButtonClicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}

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

void MainWindow::displayProfileDetails(const Profile &profile)
{
    ui->profileNameValue->setText(QString::fromStdString(profile.getName()));
    ui->basalRateValue->setValue(profile.getBasalRate());
    ui->carbRatioValue->setValue(profile.getCarbRatio());
    ui->correctionValue->setValue(profile.getCorrectionFactor());
    ui->targetGlucoseValue->setValue(profile.getTargetGlucose());
}


void MainWindow::onNewProfileClicked()
{
    // Gather profile data
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

    // Create a new Profile and add it to the manager
    Profile newProfile(name.toStdString(), basalRate, correctionFactor, carbRatio, targetGlucose);
    profileManager->addProfile(newProfile);

    // Add the profile to the list widget & store its name in the item data
    QListWidgetItem *item = new QListWidgetItem(name, ui->profileList);
    item->setData(Qt::UserRole, name);
}

void MainWindow::onProfileSelected()
{
    QListWidgetItem *selectedItem = ui->profileList->currentItem();
    if (!selectedItem)
        return;

    QString profileName = selectedItem->data(Qt::UserRole).toString();
    Profile* profile = profileManager->getProfile(profileName.toStdString());
    if (profile)
    {
        displayProfileDetails(*profile);
    }
}


void MainWindow::onEditProfileClicked()
{
    QListWidgetItem *selectedItem = ui->profileList->currentItem();
    if (!selectedItem)
        return;

    // Allow editing of the profile details
    setDetailsReadOnly(false);

}

void MainWindow::onSaveProfileClicked()
{
    QListWidgetItem *selectedItem = ui->profileList->currentItem();
    if (!selectedItem)
        return;

    QString profileName = selectedItem->data(Qt::UserRole).toString();

    // Retrieve updated values from the UI
    double newBasal = ui->basalRateValue->text().toDouble();
    double newCarbRatio = ui->carbRatioValue->text().toDouble();
    double newCorrFactor = ui->correctionValue->text().toDouble();
    double newTargetGlucose = ui->targetGlucoseValue->text().toDouble();

    // Update the profile in ProfileManager
    bool success = profileManager->updateProfile(profileName.toStdString(), newBasal, newCorrFactor, newCarbRatio, newTargetGlucose);
    if (success)
    {
        setDetailsReadOnly(true);
        QMessageBox::information(this, "Success", "Profile updated successfully.");
    }
    else
    {
        QMessageBox::warning(this, "Error", "Profile update failed.");
    }

}


void MainWindow::onDeleteProfileClicked()
{
    QListWidgetItem *selectedItem = ui->profileList->currentItem();
    if (!selectedItem)
        return;

    QString profileName = selectedItem->data(Qt::UserRole).toString();

    int ret = QMessageBox::question(this, "Delete Profile",
                                    "Are you sure you want to delete the profile '" + profileName + "'?");
    if (ret == QMessageBox::Yes)
    {
        bool success = profileManager->deleteProfile(profileName.toStdString());
        if (success)
        {
            delete selectedItem; // Remove the item from the list

            // Prevent auto selecting next list item
            ui->profileList->blockSignals(true);
            ui->profileList->clearSelection();
            ui->profileList->blockSignals(false);

            // Clear the detail fields
            ui->profileNameValue->clear();
            ui->basalRateValue->clear();
            ui->carbRatioValue->clear();
            ui->correctionValue->clear();
            ui->targetGlucoseValue->clear();
        }
    }
}
