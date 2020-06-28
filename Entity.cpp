#include "Entity.h"

Entity::Entity()
{
}

Entity::Entity(QString isbn, QString title, QString author, QByteArray cover) {
    this->setIsbn(isbn);
    this->setTitle(title);
    this->setAuthor(author);
    this->setCover(cover);
}

void Entity::setIsbn(QString isbn) {
    this->isbn = isbn;
}

void Entity::setTitle(QString title) {
    this->title = title;
}

void Entity::setAuthor(QString author) {
    this->author = author;
}

void Entity::setCover(QByteArray cover) {
    this->cover = cover;
}

QString Entity::getIsbn() {
    return this->isbn;
}

QString Entity::getTitle() {
    return this->title;
}

QString Entity::getAuthor() {
    return this->author;
}

QByteArray Entity::getCover() {
    return this->cover;
}
