#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ProfileManager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    ProfileManager *profileManager;
    const QString correctPIN = "1234";  // Hardcoded PIN

    // Helper functions
    void displayProfileDetails(const Profile &profile);
    void setDetailsReadOnly(bool readOnly);

private slots:
    void onPowerButtonHeld(); // Called when power button is held
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


};
#endif // MAINWINDOW_H
