#ifndef CONNECTION_H
#define CONNECTION_H

#include <QSqlDatabase>

class Connection {
public:
    Connection();
    bool createConnect();
    void closeConnection();

private:
    QSqlDatabase db;  // Déclare QSqlDatabase en tant que variable membre
};

#endif // CONNECTION_H
