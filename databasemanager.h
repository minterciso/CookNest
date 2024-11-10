#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QFile>
#include <QVariant>
#include <QMutex>
#include <QMutexLocker>
#include <QDebug>
#include <QString>

class DatabaseManager {
public:
    static DatabaseManager& instance();

    /**
     * @brief Open the database, if the database does not exist, create and execute the initial schema file.
     * @return true if the database was opened successfully, false otherwise
     */
    bool openDatabase();

    /**
     * @brief Close the database gracefully
     */
    void closeDatabase();

    QSqlQuery executeQuery(const QString &queryStr);
    /**
     * @brief Execute a suit of queries from a SQL File.
     * @param qf The File that has the SQL Script to execute
     * @return true if the file was executed successfully, false otherwise and set the error parameter
     */
    bool executeQueryFromFile(QFile &qf);

    /**
     * @brief Check if the database is open, and return the status.
     * @return true if the database is open, false otherwise
     */
    bool isOpen() const;

    /**
     * @brief Get the last error found.
     * @return The last error found.
     */
    QString getError(void);

private:
    DatabaseManager();
    ~DatabaseManager();
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    QSqlDatabase db;
    QMutex mutex;
    QString error;
};

#endif // DATABASEMANAGER_H
