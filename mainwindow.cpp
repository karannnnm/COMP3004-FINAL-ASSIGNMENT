#include "mainwindow.h"
#include <QtCharts/QChartView>
QT_CHARTS_USE_NAMESPACE
#include "ui_mainwindow.h"
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QtCharts/QValueAxis>
#include <QMessageBox>

QT_CHARTS_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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

    //can only change profile info in edit mode
    //can maybe add default values
    ui->profileNameValue->setReadOnly(true);
    ui->basalRateValue->setReadOnly(true);
    ui->carbRatioValue->setReadOnly(true);
    ui->correctionValue->setReadOnly(true);
    ui->targetGlucoseValue->setReadOnly(true);

    //example profile value - no data stored
    QListWidgetItem *newItem = new QListWidgetItem("Test", ui->profileList); //later can attach profile data to list item
    ui->profileList->addItem(newItem);

    //data log
    ui->log->setReadOnly(true);


    //signals & slots
    connect(ui->powerButton, SIGNAL(released()), this, SLOT(onPowerButtonHeld()));
    connect(ui->unlockButton, SIGNAL(released()), this, SLOT(onUnlockButtonClicked()));


}

MainWindow::~MainWindow()
{
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
    } else {
        QMessageBox::warning(this, "Incorrect PIN", "The PIN you entered is incorrect. Please try again.");
        ui->passwordValue->clear();
    }

}
