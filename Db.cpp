#include "Db.h"

Db::Db(QString dbFilePath)
{
    // 接続の初期化
    connectionName = "db";
    db_ = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    db_.setDatabaseName(dbFilePath);
    if (!db_.open()) {
        qDebug()<<db_.lastError();
    }
    // テーブル作成
    QSqlQuery query(db_);
    if (query.prepare("create table data("
                      "  isbn text primary key"
                      ", title text"
                      ", author text"
                      ", cover blob"
                      ")")) {
        if (!query.exec()) {
            qDebug()<<query.lastError();
            qDebug()<<query.lastQuery()<<query.boundValues();
        }
    } else {
        qDebug()<<query.lastError();
    }
}

Db::~Db() {
    db_.close();
    QSqlDatabase::removeDatabase(connectionName);
}

int Db::insert(Entity e) {
    QSqlQuery query(db_);
    if (query.prepare("insert into data (isbn, title, author, cover)"
                      " values(?, ?, ?, ?)")) {
        query.addBindValue(e.getIsbn());
        query.addBindValue(e.getTitle());
        query.addBindValue(e.getAuthor());
        query.addBindValue(e.getCover());
        if (query.exec()) {
            qDebug()<<query.lastInsertId().toULongLong() << "added";
            return 0;
        } else {
            qDebug()<<query.lastError();
            qDebug()<<query.lastQuery()<<query.boundValues();
            return query.lastError().nativeErrorCode().toInt();
        }
    } else {
        qDebug()<<query.lastError();
        return -1;
    }
}

int Db::update(Entity e) {
    QSqlQuery query(db_);
    if (query.prepare("update data set"
                      "  title = ?"
                      ", author = ?"
                      ", cover = ?"
                      " where isbn = ?")) {
        query.addBindValue(e.getTitle());
        query.addBindValue(e.getAuthor());
        query.addBindValue(e.getCover());
        query.addBindValue(e.getIsbn());
        if (query.exec()) {
            qDebug()<<query.lastInsertId().toULongLong() << "updated";
            return 0;
        } else {
            qDebug()<<query.lastError();
            qDebug()<<query.lastQuery()<<query.boundValues();
            return query.lastError().nativeErrorCode().toUInt();
        }
    } else {
        qDebug()<<query.lastError();
        return -1;
    }
}

QList<Entity> Db::selectAll() {
    QList<Entity> list;
    QSqlQuery query(db_);
    if (query.prepare("select isbn, title, author, cover"
                      " from data")) {
        if (query.exec()) {
            while (query.next()) {
                list.append(Entity(
                                query.value("isbn").toString(),
                                query.value("title").toString(),
                                query.value("author").toString(),
                                query.value("cover").toByteArray()));
            }
        }
    }
    return list;
}

QByteArray Db::getCover(QString isbn) {
    QSqlQuery query(db_);
    if (query.prepare("select cover"
                      " from data where isbn = ?")) {
        query.addBindValue(isbn);
        if (query.exec()) {
            query.next();
            return query.value("cover").toByteArray();
        }
    }
    return QByteArray();
}
