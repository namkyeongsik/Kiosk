#include "database.h"

Database::Database(QObject *parent) : QObject(parent) {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("kiosk_data.db");
}

Database::~Database() {
    closeDatabase();
}

bool Database::openDatabase() {
    if (!db.open()) {
        qDebug() << "데이터베이스 열기 실패: " << db.lastError().text();
        return false;
    }
    qDebug() << "데이터베이스 연결 성공";
    return true;
}

void Database::closeDatabase() {
    if (db.isOpen()) {
        db.close();
        qDebug() << "데이터베이스 연결 종료";
    }
}

bool Database::createTables() {
    if (!db.isOpen()) {
        qDebug() << "데이터베이스가 열려있지 않습니다!";
        return false;
    }

    QSqlQuery query;

    QString createOrdersTable = R"(
        CREATE TABLE IF NOT EXISTS orders (
            order_id INTEGER PRIMARY KEY AUTOINCREMENT,
            total_cost INTEGER NOT NULL,
            order_time DATETIME DEFAULT CURRENT_TIMESTAMP
        )
    )";

    QString createIndividualOrdersTable = R"(
        CREATE TABLE IF NOT EXISTS individual_orders (
            order_id INTEGER NOT NULL,
            entry_id INTEGER NOT NULL,
            extra_shots INTEGER DEFAULT 0 Check(extra_shots in (0, 1)),
            whipped_cream INTEGER DEFAULT 0 Check(whipped_cream in (0, 1)),
            ice INTEGER DEFAULT 0 Check(ice in (0, 1)),
            FOREIGN KEY (order_id) REFERENCES orders(order_id)
        )
    )";

    QString createEntryListTable = R"(
        CREATE TABLE IF NOT EXISTS entry_list (
            entry_id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            price INTEGER NOT NULL,
            image_path TEXT NOT NULL
        )
    )";

    QString createStocksTable = R"(
        CREATE TABLE IF NOT EXISTS stocks (
            entry_id INTEGER NOT NULL,
            stock INTEGER NOT NULL,
            FOREIGN KEY (entry_id) REFERENCES entry_list(entry_id)
        )
    )";

    QString createPointTable = R"(
        CREATE TABLE IF NOT EXISTS point (
            phone_number TEXT PRIMARY KEY NOT NULL,
            point INTEGER Default 10000
        )
    )";

    QString createCreditTable = R"(
        CREATE TABLE IF NOT EXISTS credit (
            card_number TEXT PRIMARY KEY NOT NULL,
            balance INTEGER Default 10000
        )
    )";


    if (!query.exec(createOrdersTable)) {
        qDebug() << "주문 테이블 생성 실패: " << query.lastError().text();
        return false;
    }
    if (!query.exec(createIndividualOrdersTable)) {
        qDebug() << "개인 주문 테이블 생성 실패: " << query.lastError().text();
        return false;
    }
    if (!query.exec(createEntryListTable)) {
        qDebug() << "엔트리 리스트 테이블 생성 실패: " << query.lastError().text();
        return false;
    }
    if (!query.exec(createStocksTable)) {
        qDebug() << "재고 테이블 생성 실패: " << query.lastError().text();
        return false;
    }
    if (!query.exec(createPointTable)) {
        qDebug() << "보유 포인트 테이블 생성 실패: " << query.lastError().text();
        return false;
    }
    if (!query.exec(createCreditTable)) {
        qDebug() << "카드 잔액 테이블 생성 실패: " << query.lastError().text();
        return false;
    }

    qDebug() << "테이블 생성 완료";
    return true;
}
