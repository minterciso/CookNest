#include "databasemanager.h"
#include <QRegularExpression>

// Singleton instance
DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager instance;
    return instance;
}

// Constructor
DatabaseManager::DatabaseManager() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setConnectOptions("QSQLITE_USE_QVFS");
    this->error.clear();
}

// Destructor
DatabaseManager::~DatabaseManager() {
    closeDatabase();
}

// Open the SQLite database
bool DatabaseManager::openDatabase(const QString &dbName) {
    QMutexLocker locker(&mutex);
    if (db.isOpen()) return true;

    db.setDatabaseName(dbName);
    if (!db.open()) {
        this->error.append("Unable to open database: ");
        this->error.append(db.lastError().text());
        return false;
    }
    return true;
}

// Close the SQLite database
void DatabaseManager::closeDatabase() {
    QMutexLocker locker(&mutex);
    if (db.isOpen())
        db.close();
}

// Execute a query and return the result
QSqlQuery DatabaseManager::executeQuery(const QString &queryStr) {
    QMutexLocker locker(&mutex);
    QSqlQuery query(db);
    if (!query.exec(queryStr)) {
        this->error.append("Query failed: ");
        this->error.append(query.lastError().text());
    }
    return query;
}

/**
 * @brief Execute a suit of queries from a SQL File.
 * @param qf The File that has the SQL Script to execute
 * @return boolean
 */
bool DatabaseManager::executeQueryFromFile(QFile &qf) {
    QMutexLocker locker(&mutex);
    // Rad the file content
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
