#include "mainwindow.h"

#include <QApplication>
#include <QMainWindow>
#include "databasemanager.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    // Connect to the signal "about to close", and close the database
    QObject::connect(&a, &QCoreApplication::aboutToQuit, []() {
        DatabaseManager::instance().closeDatabase();
    });
    return a.exec();
}
