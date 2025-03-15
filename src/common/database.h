#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

class Database : public QObject {
    Q_OBJECT
public:
    explicit Database(QObject *parent = nullptr);
    ~Database();
    
    bool openDatabase();
    void closeDatabase();
    bool createTables();
    
private:
    QSqlDatabase db;
};

#endif // DATABASE_H
