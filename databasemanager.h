#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QFile>
#include <QVariant>
#include <QMutex>
#include <QMutexLocker>
#include <QDebug>
#include <QString>

class DatabaseManager {
public:
    static DatabaseManager& instance();

    bool openDatabase(const QString &dbName);
    void closeDatabase();

    QSqlQuery executeQuery(const QString &queryStr);
    bool executeQueryFromFile(QFile &qf);

    bool isOpen() const;

    QString getError(void){ return this->error; }

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
