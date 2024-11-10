#include "databasemanager.h"
#include <QRegularExpression>
#include <QStandardPaths>
#include <QFileInfo>
#include <QDir>

DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager instance;
    return instance;
}

DatabaseManager::DatabaseManager() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setConnectOptions("QSQLITE_USE_QT_VFS");
    this->error.clear();
}

DatabaseManager::~DatabaseManager() {
    closeDatabase();
}

bool DatabaseManager::openDatabase() {
    QMutexLocker locker(&mutex);
    if (db.isOpen()) return true;

    // Try to get a default database location, on the documents folder
    QString databasePath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    databasePath.append("/CookNest");
    // First, check if the directory exist
    if(!QDir(databasePath).exists()){
        QDir().mkdir(databasePath);
    }
    databasePath.append("/default.db");

    // Now check if the database already exists
    bool dbExists = false;
    if(QFileInfo::exists(databasePath))
        dbExists = true;

    db.setDatabaseName(databasePath);
    if (!db.open()) {
        this->error.append("Unable to open database: ");
        this->error.append(db.lastError().text());
        return false;
    }
    // If the database is a new one, execute the SQL Schema script
    if(!dbExists){
        QFile schemaFile(":/data/database/schema.sql");
        if(!this->executeQueryFromFile(schemaFile))
            return false;
    }
    return true;
}

void DatabaseManager::closeDatabase() {
    QMutexLocker locker(&mutex);
    if (db.isOpen())
        db.close();
}

QSqlQuery DatabaseManager::executeQuery(const QString &queryStr) {
    QMutexLocker locker(&mutex);
    QSqlQuery query(db);
    if (!query.exec(queryStr)) {
        this->error.append("Query failed: ");
        this->error.append(query.lastError().text());
    }
    return query;
}

bool DatabaseManager::executeQueryFromFile(QFile &qf) {
    // Read the file content
    qf.open(QIODevice::ReadOnly);
    QString queryStr(qf.readAll());
    qf.close();

    QSqlQuery query(db);

    //Parse and execute each line
    //Replace comments and tabs and new lines with space
    queryStr = queryStr.replace(QRegularExpression("(\\/\\*(.|\\n)*?\\*\\/|^--.*\\n|\\t|\\n)", QRegularExpression::CaseInsensitiveOption|QRegularExpression::MultilineOption), " ");
    //Remove waste spaces
    queryStr = queryStr.trimmed();
    //Extract queries
    QStringList qList = queryStr.split(';', Qt::SkipEmptyParts);
    // Execute
    foreach(const QString &s, qList){
        if(!query.exec(s)){
            this->error.append("Query failed: ");
            this->error.append(query.lastError().text());
            return false;
        }
    }
    return true;
}

// Check if the database is open
bool DatabaseManager::isOpen() const {
    return db.isOpen();
}

QString DatabaseManager::getError(){
    QString retError = this->error;
    this->error.clear();
    return retError;
}
