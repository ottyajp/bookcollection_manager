#ifndef ENTITY_H
#define ENTITY_H

#include <QString>
#include <QByteArray>

class Entity
{
public:
    Entity();
    Entity(QString isbn, QString title, QString author, QByteArray cover);
    void setIsbn(QString);
    void setTitle(QString);
    void setAuthor(QString);
    void setCover(QByteArray);
    QString getIsbn();
    QString getTitle();
    QString getAuthor();
    QByteArray getCover();

private:
    QString isbn;
    QString title;
    QString author;
    QByteArray cover;
};

#endif // ENTITY_H
