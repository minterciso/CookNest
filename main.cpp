#include "mainwindow.h"

#include <QApplication>
#include <QMainWindow>
#include <QSettings>
#include "databasemanager.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    // Set the application information for ease of usage of the Settings on the application
    QCoreApplication::setOrganizationName("JITProgramer");
    QCoreApplication::setOrganizationDomain("jitprogramer.com");
    QCoreApplication::setApplicationName("Cook Nest");
    w.show();
    // Connect to the signal "about to close", and close the database
    QObject::connect(&a, &QCoreApplication::aboutToQuit, []() {
        DatabaseManager::instance().closeDatabase();
    });
    // Check if we have a product already opened, if we have, close it to clean the Settings
    QSettings settings;
    settings.beginGroup("product");
    if(settings.value("open").toBool()){
        settings.setValue("open", false);
        settings.setValue("id", -1);
    }
    settings.endGroup();
    return a.exec();
}
