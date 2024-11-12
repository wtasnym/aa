#include "client.h"
#include <QSqlQuery>
#include <QtDebug>
#include <QSqlError>
#include<QSqlQueryModel>
#include<QObject>
#include <QtCharts/QPieSeries>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QPdfWriter>
#include <QPainter>
#include <QPrinter>
#include <QFile>
#include <QTextDocument>
#include <QDesktopServices>
#include <QUrl>
#include <QMessageBox>
#include <QPieSeries>
#include <utility> // pour std::move


Client::Client() {}

Client::Client(int id_client, const QString& nom, const QString& prenom,int telephone, const QString& email, const QString& adresse)
    : id_client(id_client), nom(nom), prenom(prenom), email(email), telephone(telephone), adresse(adresse) {}

bool Client::ajouter() {
    if (nom.isEmpty() || prenom.isEmpty() || telephone <= 0 || email.isEmpty() || adresse.isEmpty()) {
        qDebug() << "Erreur: tous les champs doivent être remplis.";
        return false;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO client (id_client, nom, prenom, telephone, email, adresse) VALUES (:id_client, :nom, :prenom, :telephone, :email, :adresse)");

    query.bindValue(":id_client", id_client);
    query.bindValue(":nom", nom);
    query.bindValue(":prenom", prenom);
    query.bindValue(":telephone", telephone);
    query.bindValue(":email", email);
    query.bindValue(":adresse", adresse);

    if (!query.exec()) {
        qDebug() << "Erreur lors de l'ajout du client:" << query.lastError().text();
        return false;
    }

    QMessageBox::information(nullptr, QObject::tr("Succès"), QObject::tr("Client ajouté avec succès."));
    return true;
}

QSqlQueryModel* Client::afficher() {
    QSqlQueryModel* model = new QSqlQueryModel();
    QSqlQuery query;
    query.prepare("SELECT * FROM client");

    if (!query.exec()) {
        qDebug() << "Erreur lors de l'exécution de la requête :" << query.lastError().text();
        return nullptr;
    }

    model->setQuery(query);
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Nom"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Prénom"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Téléphone"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Email"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("Adresse"));

    return model;
}

bool Client::supprimer(int id) {
    QSqlQuery query;
    query.prepare("DELETE FROM client WHERE id_client = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "Erreur lors de la suppression:" << query.lastError().text();
        return false;
    }
    return true;
}




bool Client::modifier() {
    QSqlQuery query;
    query.prepare("UPDATE client SET nom = :nom, prenom = :prenom, telephone = :telephone, email = :email, adresse = :adresse WHERE id_client = :id_client");

    query.bindValue(":nom", nom);
    query.bindValue(":prenom", prenom);
    query.bindValue(":telephone", telephone);
    query.bindValue(":email", email);
    query.bindValue(":adresse", adresse);
    query.bindValue(":id_client", id_client); // Utilisation de l'ID du client actuel

    if (!query.exec()) {
        qDebug() << "Erreur lors de la mise à jour du client:" << query.lastError().text();
        return false;
    }

    return true;
}




QSqlQueryModel* Client::trier(const QString& critere, bool ascendant) {
    QSqlQuery query;
    QString queryString = "SELECT * FROM client";

    if (!critere.isEmpty()) {
        queryString += " ORDER BY " + critere;
        if (!ascendant) {
            queryString += " DESC";
        }
    }

    query.prepare(queryString);
    query.exec();

    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery(std::move(query));

    return model;
}


QSqlQueryModel* Client::rechercher(const QString& valeur) {
    QSqlQuery query;
    QString queryString;

    if (valeur.toInt() > 0) {
        // Recherche par ID (si la valeur est un entier positif)
        queryString = "SELECT * FROM client WHERE id_client = :valeur";
    } else {
        // Recherche par nom ou prenom
        queryString = "SELECT * FROM client "
                      "WHERE nom LIKE :valeurLike "
                      "OR prenom LIKE :valeurLike";
    }

    query.prepare(queryString);

    if (valeur.toInt() > 0) {
        query.bindValue(":valeur", valeur);
    } else {
        query.bindValue(":valeurLike", "%" + valeur + "%");
    }

    qDebug() << "Executing query:" << queryString;
    qDebug() << "Binding values - " << (valeur.toInt() > 0 ? "exact:" : "LIKE pattern:") << valeur;

    if (!query.exec()) {
        qDebug() << "Error executing query:" << query.lastError().text();
    }

    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery(std::move(query));

    return model;
}
//--------------PDF
bool Client::genererPDF(const QString &filePath) {
    QPdfWriter writer(filePath);
    writer.setPageSize(QPageSize(QPageSize::A4));
    writer.setResolution(300);

    QPainter painter(&writer);
    if (!painter.isActive()) {
        qDebug() << "Failed to initialize painter.";
        return false;
    }

    // Define table parameters
    const int headerHeight = 60;
    const int rowHeight = 60;
    const int columnWidth = writer.width() / 6;
    const int tableTop = 100;
    const int tableLeft = 40;
    const QStringList headers = {"ID", "Nom", "Prenom", "Téléphone", "Email", "Adresse"};

    // Draw table headers with a thicker border
    QPen pen(Qt::black);
    pen.setWidth(2);
    painter.setPen(pen);
    painter.setFont(QFont("Arial", 12, QFont::Bold));
    for (int i = 0; i < headers.size(); ++i) {
        painter.drawRect(tableLeft + i * columnWidth, tableTop, columnWidth, headerHeight);
        painter.drawText(tableLeft + i * columnWidth + 10, tableTop + 35, headers[i]);
    }

    // Draw table rows with standard border
    pen.setWidth(1);
    painter.setPen(pen);
    painter.setFont(QFont("Arial", 10));
    int y = tableTop + headerHeight;
    QSqlQuery query;
    query.prepare("SELECT * FROM client");

    if (!query.exec()) {
        qDebug() << "Error fetching archives for PDF:" << query.lastError().text();
        return false;
    }

    while (query.next()) {
        for (int i = 0; i < headers.size(); ++i) {
            painter.drawRect(tableLeft + i * columnWidth, y, columnWidth, rowHeight);
            painter.drawText(tableLeft + i * columnWidth + 10, y + 25, query.value(i).toString());
        }
        y += rowHeight;
    }

    // Ajouter les statistiques par adresse
    int statisticsTop = y + 50;
    painter.setFont(QFont("Arial", 12, QFont::Bold));
    painter.drawText(tableLeft, statisticsTop, "Statistiques par adresse:");
    painter.setFont(QFont("Arial", 10));

    QMap<QString, int> statistiques = obtenirStatistiquesParAdresse();
    int statisticsY = statisticsTop + 30;
    for (const auto& adresse : statistiques.keys()) {
        int count = statistiques[adresse];
        painter.drawText(tableLeft, statisticsY, QString("%1: %2 clients").arg(adresse, QString::number(count)));
        statisticsY += rowHeight;
    }

    painter.end();
    return true;
}
// ------------ stat
QMap<QString, int> Client::obtenirStatistiquesParAdresse() {
    QMap<QString, int> statistiques;
    QSqlQuery query;
    query.prepare("SELECT adresse, COUNT(*) as count FROM client GROUP BY adresse");

    if (!query.exec()) {
        qDebug() << "Erreur lors de l'exécution de la requête:" << query.lastError().text();
        return statistiques; // Retourne une carte vide en cas d'erreur
    }

    while (query.next()) {
        QString adresse = query.value(0).toString();
        int count = query.value(1).toInt();
        statistiques[adresse] = count; // Ajoute l'adresse et le compte à la carte
    }

    return statistiques;
}
//m4


Client Client::rechercherClientById(int id) {
    QSqlQuery query;
    query.prepare("SELECT * FROM client WHERE id_client = :id_client");
    query.bindValue(":id_client", id); // Corrigé pour correspondre au nom du paramètre

    if (query.exec() && query.next()) {
        // Si un client est trouvé, retourner un objet Client
        return Client(query.value(0).toInt(), // id_client
                      query.value(1).toString(), // nom
                      query.value(2).toString(), // prenom
                      query.value(3).toInt(), // telephone
                      query.value(4).toString(), // email
                      query.value(5).toString()); // adresse
    }

    // Retourne un client par défaut (ID 0) si non trouvé
    return Client(0, "", "", 0, "", ""); // Assurez-vous que votre constructeur Client accepte ces paramètres
}

bool Client::ajouterFeedback(int id_feedback, int rating,  QString& commentaire) {
    QSqlQuery query;
    query.prepare("INSERT INTO feedback (id_feedback, rating, commentaire) VALUES (:id_feedback, :rating, :commentaire)");
    query.bindValue(":id_feedback", id_feedback);
    query.bindValue(":rating", rating);
    query.bindValue(":commentaire", commentaire);

    if (!query.exec()) {
        qDebug() << "Erreur lors de l'ajout du feedback:" << query.lastError().text();
        return false; // Échec de l'ajout
    }

    return true; // Ajout réussi
}

