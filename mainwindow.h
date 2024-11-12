#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlQueryModel>
#include "client.h"
#include <QItemSelection>
#include <QSqlTableModel>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_valider_clicked();

    void on_annuler_clicked();

    void on_afficher_clicked();

    void updateTable();

    void on_modifier_clicked();

    void on_supprimer_clicked();

//
    void on_trier_2_clicked();

    void on_rechercher_clicked();

    void on_exporter_clicked();



    void afficherStatistiques() ;
    void afficherStatistiquesGraphiqueBarres();
    //void afficherStatistiquesFeedback();

//m4
    void on_ok_feedback_clicked(); // Nouveau slot pour le bouton OK du feedback
    void on_submit_feedback_clicked(); // Nouveau slot pour le bouton Submit du feedback



private:
    Ui::MainWindow *ui;
   // Vérifiez que cela est fait quelque part dans votre code
    Client clientManager; // Instance de la classe Client pour gérer les clients
   //  ClientManager f;
    QSqlQueryModel *clientModel; // Modèle pour afficher les données des clients
    Client *selectedClient; // Client sélectionné
    Client client;
    Client *currentClient;



    void populateClientTable();
    void displayClientDetails(Client *client);
    void clearClientDetails();
    Client* getSelectedClient();
    void updateClient(Client* client);
     void loadExistingFeedback(int clientId);

};

#endif // MAINWINDOW_H
