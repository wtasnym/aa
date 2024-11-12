#ifndef CLIENT_H
#define CLIENT_H

#include <QString>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QMap>

class Client {
private:
    QString nom, prenom, adresse, email,commentaire;
    int id_client, telephone,rating,id_feedback;


public:
    // Constructeurs
    Client();
    Client(int id_client, const QString& nom, const QString& prenom, int telephone, const QString& email, const QString& adresse);

    // Getters
    int getIdClient() const { return id_client; }
    QString getNom() const { return nom; }
    QString getPrenom() const { return prenom; }
    int getTelephone() const { return telephone; }
    QString getEmail() const { return email; }
    QString getAdresse() const { return adresse; }

    // Setters
    void setIdClient(int i) { id_client = i; }
    void setNom(const QString& n) { nom = n; }
    void setPrenom(const QString& p) { prenom = p; }
    void setTelephone(int t) { telephone = t; }
    void setEmail(const QString& e) { email = e; }
    void setAdresse(const QString& a) { adresse = a; }

    // CRUD
    bool ajouter();
    bool supprimer(int id);
    QSqlQueryModel* afficher();
    bool modifier();

    // Fonctionnalités avancées
    QSqlQueryModel* trier(const QString& critere, bool ascendant = true);
    QSqlQueryModel* rechercher(const QString& valeur);
    bool genererPDF(const QString &);
    QMap<QString, int> obtenirStatistiquesParAdresse();

    // Recherche par ID
    Client rechercherClientById(int id);
     static bool ajouterFeedback(int id_feedback, int rating,   QString& commentaire);
    //int countMonthlyFeedback(int clientId);
};



#endif // CLIENT_H
