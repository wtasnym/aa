#include "connection.h"
#include <QSqlDatabase>
#include <QDebug>
#include <QSqlError>

Connection::Connection() {
    db = QSqlDatabase::addDatabase("QODBC");
}

bool Connection::createConnect() {
    db.setDatabaseName("projetcpp_2A36"); // Nom de la source de données
    db.setUserName("system");               // Nom d'utilisateur
    db.setPassword("cleaning");             // Mot de passe

    if (!db.open()) {
        qDebug() << "Erreur: " << db.lastError().text();
        return false;
    }
    return true;
}

void Connection::closeConnection() {
    db.close();
}


