#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "databasemanager.h"

#include <QMessageBox>
#include <QInputDialog>
#include <QSettings>

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
    this->ui->tabWidget->setDisabled(true);
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

void MainWindow::on_actionQuit_triggered()
{
    // Before quiting, close the product
    this->on_actionClose_triggered();
    // Now quit the application
    QApplication::quit();
}


void MainWindow::on_actionNew_triggered()
{
    // First, close if there's anything open
    this->on_actionClose_triggered();

    // Open a Input Dialog to query the user the name of the product.
    bool ok;
    QMessageBox msgBox;
    QString newProductName = QInputDialog::getText(this, "New Product", "Product name:", QLineEdit::Normal, QString(), &ok);
    msgBox.setOption(QMessageBox::Option::DontUseNativeDialog);

    qDebug() << "NewProduct: Trying to create a new product with name " << newProductName;

    if(newProductName.trimmed().length() == 0 && ok){
        msgBox.setText("No product name provided!");
        msgBox.setIcon(QMessageBox::Warning);
        qDebug() << "NewProduct: No product name provided";
        msgBox.exec();
        return;
    }
    // We have a valid name, check if there's anything on the database with that name
    QSqlQuery query;
    QString queryText;
    queryText = "SELECT COUNT(*) AS qtd FROM product WHERE name = '";
    queryText.append(newProductName);
    queryText.append("';");
    query = DatabaseManager::instance().executeQuery(queryText);
    if(!query.exec()){
        qCritical() << "NewProduct: Unable to execute query: " << query.lastError();
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setText("Unable to check if product already exists. Please check logs");
        msgBox.exec();
    }
    query.first();
    bool exists = query.value("qtd").toBool();
    if(exists){
        // Product already exists, warn the user and return
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("Product already exists");
        msgBox.exec();
        qWarning() << "NewProduct: Product " << newProductName << " already exists.";
        return;
    }
    query.clear();

    // Insert the new recipe on the database
    queryText = "INSERT INTO product(name) VALUES ('";
    queryText.append(newProductName);
    queryText.append("');");
    query = DatabaseManager::instance().executeQuery(queryText);
    if(query.numRowsAffected()!=1){
        // Something went wrong
        qCritical() << "NewProduct: There was an error inserting a new product:" << query.lastError();
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setText("There was an error creating a new product. Check the logs.");
        msgBox.exec();
        return;
    }
    // Get the product id and store it on the Settings
    query.clear();
    queryText = "SELECT id FROM product WHERE name='";
    queryText.append(newProductName);
    queryText.append("';");
    query = DatabaseManager::instance().executeQuery(queryText);
    query.first();
    QSettings settings;
    settings.beginGroup("product");
    settings.setValue("open", true);
    settings.setValue("id", query.value("id").toInt());
    settings.endGroup();
    // All is goot, so allow the tab widget to be used
    this->ui->tabWidget->setDisabled(false);
    qDebug() << "NewProduct: Product created and enabled";
}


void MainWindow::on_actionOpen_triggered()
{
    // First, close if there's anything open
    this->on_actionClose_triggered();
    // Try to open an existing product. This should open a list of products to open, in the Input Dialog
    QStringList productsList;
    bool ok;
    // First, get all products from the database
    qDebug() << "OpenProduct: Finding all products on database.";
    QSqlQuery query = DatabaseManager::instance().executeQuery("SELECT name FROM product ORDER BY name");
    qDebug() << "OpenProduct: Got Products:";
    while(query.next()){
        QString product = query.value("name").toString();
        qDebug() << " - " << product;
        productsList << product;
    }
    // Display the input dialog
    QString selectedProduct = QInputDialog::getItem(this, "Select product to open", "Product:", productsList,0, false, &ok);
    qDebug() << "OpenProduct: Selected product " << selectedProduct;
    // Get the product id
    QString queryText = "SELECT id FROM product WHERE name = '";
    queryText.append(selectedProduct);
    queryText.append("'");
    query = DatabaseManager::instance().executeQuery(queryText);
    query.first();
    QSettings settings;
    settings.beginGroup("product");
    settings.setValue("open", true);
    settings.setValue("id", query.value("id").toInt());
    settings.endGroup();
    this->ui->tabWidget->setDisabled(false);
}


void MainWindow::on_actionClose_triggered()
{
    // Simply remove the "productId" from the memory, and set the tabs to false, as well as correct the settings
    QSettings settings;
    settings.beginGroup("product");

    if(settings.value("open").toBool()){
        qDebug() << "CloseProduct: Closing product with id " << settings.value("id").toInt();
        settings.setValue("open", false);
        settings.setValue("id", -1);
        this->ui->tabWidget->setDisabled(true);
        qDebug() << "CloseProduct: Product Closed";
    }
    settings.endGroup();
}

