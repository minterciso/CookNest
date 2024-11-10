#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "databasemanager.h"

#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);

    // After the form is loaded, execute the database start
    static bool firstLoad = true;
    if (firstLoad) {
        firstLoad = false;
        this->startDatabase();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::startDatabase(){
    QString error;
    // Try to open the Database
    if(!DatabaseManager::instance().openDatabase()){
        // There was an error, capture it and display the error
        error = DatabaseManager::instance().getError();
        QMessageBox dlg;
        dlg.setOption(QMessageBox::Option::DontUseNativeDialog);
        dlg.setIcon(QMessageBox::Critical);
        dlg.setText(error);
        dlg.setStandardButtons(QMessageBox::Ok);
        int ret = dlg.exec();
        if(ret == QMessageBox::Ok){
            QApplication::quit();
        }
    }
}
