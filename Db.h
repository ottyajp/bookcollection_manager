#ifndef DB_H
#define DB_H

#include "Entity.h"

#include <QString>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QList>

class Db
{
public:
    Db(QString);
    ~Db();
    int insert(Entity);
    int update(Entity);
    QList<Entity> selectAll();
    QByteArray getCover(QString);
private:
    QString connectionName;
    QSqlDatabase db_;
};

#endif // DB_H
