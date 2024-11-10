#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void showEvent(QShowEvent *event) override;

private:
    Ui::MainWindow *ui;

    /**
     * @brief startDatabase
     * Start the SQLite database. If there's an error, open a simple dialog
     * to present the error, and, when the user accepts the error, close the application.
     */
    void startDatabase();
};
#endif // MAINWINDOW_H
