#include "mainwindow.h"
#include "ui_mainwindow.h"
#include"client.h"

#include<QMessageBox>
#include<QIntValidator>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QGraphicsScene>
#include <QSqlQuery>
#include <QFileDialog>
#include <QRegularExpression>
#include <QDate>
#include <QtDebug>
#include <QSqlError>
#include <QSqlQuery>
//
#include <QtCharts/QValueAxis>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarSeries>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
//m4
#include <QDebug>
#include <QCheckBox>
#include <QRadioButton>
#include <QString>
#include <QToolButton>
#include <QStandardItemModel>


#include <QDate>
#include <QTime>
#include <QDateTime>
#include <QDateEdit>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), clientManager(), clientModel(new QSqlQueryModel()), selectedClient(nullptr) {
    ui->setupUi(this);
    ui->tableView->setModel(client.afficher());
    connect(ui->statistique, &QPushButton::clicked, this, &MainWindow::afficherStatistiques);
    connect(ui->statistique, &QPushButton::clicked, this, &MainWindow::afficherStatistiquesGraphiqueBarres);
    connect(ui->ok, &QPushButton::clicked, this, &MainWindow::on_ok_feedback_clicked);
    connect(ui->submit, &QPushButton::clicked, this, &MainWindow::on_submit_feedback_clicked);
   // connect(ui->statistique2, &QPushButton::clicked, this, &MainWindow::afficherStatistiquesFeedback);
    updateTable();
     //ui->tableView3->setFixedSize(600, 700);







}

MainWindow::~MainWindow() {
    delete ui;
    delete clientModel;
}








void MainWindow::on_valider_clicked() {
    if (ui->id_client->text().isEmpty() || ui->nom->text().isEmpty() ||
        ui->prenom->text().isEmpty() || ui->telephone->text().isEmpty() ||
        ui->email->text().isEmpty() || ui->adresse->text().isEmpty()) {
        QMessageBox::warning(this, QObject::tr("Champs vides"), QObject::tr("Veuillez remplir tous les champs."));
        return;
    }

    bool ok;
    int id_client = ui->id_client->text().toInt(&ok);
    if (!ok || id_client <= 0) {
        QMessageBox::warning(this, QObject::tr("Erreur d'ID"), QObject::tr("L'ID doit être un nombre entier positif."));
        return;
    }

    QString nom = ui->nom->text().trimmed();
    QString prenom = ui->prenom->text().trimmed();

    QString telephoneStr = ui->telephone->text();
    if (telephoneStr.length() != 8 || !telephoneStr.toUInt(&ok) || !ok) {
        QMessageBox::warning(this, QObject::tr("Erreur de téléphone"), QObject::tr("Le téléphone doit être composé de 8 chiffres."));
        return;
    }
    int telephone = telephoneStr.toInt();

    QString email = ui->email->text();
    QRegularExpression emailRegex("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.com$");
    if (!emailRegex.match(email).hasMatch()) {
        QMessageBox::warning(this, QObject::tr("Erreur d'email"), QObject::tr("L'email doit être au format nom.prenom@gmail.com."));
        return;
    }

    QString adresse = ui->adresse->text();
    // Vérifications supplémentaires pour l'adresse peuvent être ajoutées ici.

    Client newClient(id_client, nom, prenom, telephone, email, adresse);
    if (newClient.ajouter()) {
        QMessageBox::information(this, QObject::tr("Ajout Réussi"), QObject::tr("L'ajout du client a été effectué avec succès."));
    } else {
        QMessageBox::critical(this, QObject::tr("Ajout Échoué"), QObject::tr("L'ajout du client a échoué."));
    }
    updateTable();
}
void MainWindow::on_annuler_clicked()
{
    ui->id_client->clear();
    ui->nom->clear();
    ui->prenom->clear();
    ui->telephone->clear();
    ui->email->clear();
    ui->adresse->clear();
}

void MainWindow::on_supprimer_clicked()
{
    QModelIndexList selectedIndexes = ui->tableView->selectionModel()->selectedRows();
    if (selectedIndexes.isEmpty()) {
        QMessageBox::warning(this, QObject::tr("Aucune sélection"), QObject::tr("Veuillez sélectionner un client dans la table."));
        return;
    }

    int row = selectedIndexes.first().row();
    int id_client = clientModel->data(clientModel->index(row, 0)).toInt(); // Supposons que l'ID est dans la première colonne

    if (clientManager.supprimer(id_client)) {
        QMessageBox::information(this, QObject::tr("Suppression Réussie"), QObject::tr("La suppression du client a été effectuée avec succès."));
    } else {
        QMessageBox::critical(this, QObject::tr("Suppression Échouée"), QObject::tr("La suppression du client a échoué."));
    }

    updateTable();
}



void MainWindow::on_afficher_clicked() {
    updateTable();
}


void MainWindow::updateTable() {
    delete clientModel;
    clientModel = clientManager.afficher();
    if (clientModel) {
        ui->tableView->setModel(clientModel);
    }
}


Client* MainWindow::getSelectedClient() {
    QModelIndexList selectedIndexes = ui->tableView->selectionModel()->selectedRows();
    if (!selectedIndexes.isEmpty()) {
        int row = selectedIndexes.first().row();
        int id_client = clientModel->data(clientModel->index(row, 0)).toInt();
        QString nom = clientModel->data(clientModel->index(row, 1)).toString();
        QString prenom = clientModel->data(clientModel->index(row, 2)).toString();
        QString adresse = clientModel->data(clientModel->index(row, 5)).toString(); // Adresse ici
        QString email = clientModel->data(clientModel->index(row, 4)).toString(); // Email ici
        int telephone = clientModel->data(clientModel->index(row, 3)).toInt(); // Téléphone ici

        return new Client(id_client, nom, prenom, telephone, email, adresse);
    }
    return nullptr;
}

void MainWindow::updateClient(Client* client) {
    if (!client) {
        QMessageBox::warning(this, QObject::tr("Erreur"), QObject::tr("Client invalide. Aucune mise à jour effectuée."));
        return;
    }

    // Récupérer les nouvelles informations à partir des champs de saisie
    QString nom = ui->nom->text();
    QString prenom = ui->prenom->text();
    int telephone = ui->telephone->text().toInt();
    QString email = ui->email->text();
    QString adresse = ui->adresse->text();

    // Mettre à jour le client existant
    QSqlQuery updateQuery;
    updateQuery.prepare("UPDATE client SET nom = :nom, prenom = :prenom, telephone = :telephone, email = :email, adresse = :adresse WHERE id_client = :id_client");
    updateQuery.bindValue(":nom", nom);
    updateQuery.bindValue(":prenom", prenom);
    updateQuery.bindValue(":telephone", telephone);
    updateQuery.bindValue(":email", email);
    updateQuery.bindValue(":adresse", adresse);
    updateQuery.bindValue(":id_client", client->getIdClient());

    if (!updateQuery.exec()) {
        qDebug() << "Erreur lors de la mise à jour du client:" << updateQuery.lastError().text();
        QMessageBox::critical(this, QObject::tr("Erreur de mise à jour"), QObject::tr("La mise à jour du client a échoué."));
        return; // Sortir si la mise à jour échoue
    }

    // Supprimé le message d'information
    // QMessageBox::information(this, QObject::tr("Mise à jour réussie"), QObject::tr("Le client a été mis à jour avec succès."));
}




void MainWindow::on_modifier_clicked()
{
    selectedClient = getSelectedClient();
    if (!selectedClient) {
        QMessageBox::warning(this, QObject::tr("Aucun client sélectionné"), QObject::tr("Veuillez sélectionner un client dans la table."));
        return;
    }

    // Affichez les détails du client sélectionné
    displayClientDetails(selectedClient);

    // Mettre à jour le client
    updateClient(selectedClient);

    // Suppression de l'ancien client
    int idClientToDelete = selectedClient->getIdClient(); // ID du client à supprimer
    QSqlQuery deleteQuery;
    deleteQuery.prepare("DELETE FROM client WHERE id_client = :id_client");
    deleteQuery.bindValue(":id_client", idClientToDelete);

    if (!deleteQuery.exec()) {
        qDebug() << "Erreur lors de la suppression du client:" << deleteQuery.lastError().text();
        QMessageBox::critical(this, QObject::tr("Erreur de suppression"), QObject::tr("La suppression du client a échoué."));
    }

    // Mettre à jour la table après la modification
    updateTable(); // Actualiser la table après la mise à jour
}
void MainWindow::displayClientDetails(Client *client) {
    if (client) {
        ui->id_client->setText(QString::number(client->getIdClient()));
        ui->nom->setText(client->getNom());
        ui->prenom->setText(client->getPrenom());
        ui->adresse->setText(client->getAdresse()); // Adresse ici
        ui->email->setText(client->getEmail()); // Email ici
        ui->telephone->setText(QString::number(client->getTelephone())); // Téléphone ici
    }
}


void MainWindow::on_trier_2_clicked()
{
    QString critere = ui->comboBox->currentText().toLower();

    // Récupérez et ajustez le critère de tri
    if (critere == "id") {
        critere = "id_client";
    } else if (critere == "prenom") {
        critere = "prenom";
    } else if (critere == "nom") {
        critere = "nom";
    }

    bool ascendant = true;  // Optionnel : Ajouter une logique pour gérer l'ordre de tri
    QSqlQueryModel* model = client.trier(critere, ascendant);
    ui->tableView->setModel(model);
}
void MainWindow::on_rechercher_clicked() {
    QString valeur = ui->recherche->text().trimmed(); // Récupération et nettoyage de la valeur saisie

    if (valeur.isEmpty()) {
        QMessageBox::warning(this, QObject::tr("Erreur"), QObject::tr("Veuillez entrer une valeur de recherche."));
        return;
    }

    QSqlQueryModel* model = client.rechercher(valeur);
    if (model) {
        ui->tableView->setModel(model);
    } else {
        QMessageBox::warning(this, QObject::tr("Erreur"), QObject::tr("Aucun résultat trouvé."));
    }
}

void MainWindow::on_exporter_clicked() {
    QString filePath = QFileDialog::getSaveFileName(this, "Enregistrer le PDF", "", "Fichiers PDF (*.pdf)");

    if (filePath.isEmpty()) {
        return; // L'utilisateur a annulé la boîte de dialogue
    }

    if (client.genererPDF(filePath)) {
        QMessageBox::information(this, QObject::tr("Succès"), QObject::tr("PDF généré avec succès."));
    } else {
        QMessageBox::critical(this, QObject::tr("Échec"), QObject::tr("Échec de la génération du PDF."));
    }
}

void MainWindow::afficherStatistiques() {
    QMap<QString, int> statistiques = clientManager.obtenirStatistiquesParAdresse();

    if (statistiques.isEmpty()) {
        QMessageBox::information(this, QObject::tr("Statistiques"), QObject::tr("Aucune donnée à afficher."));
        return;
    }

    QString result;
    for (const auto& [key, value] : statistiques.toStdMap()) {
        result += QString("%1 : %2\n").arg(key).arg(value);
    }

    QMessageBox::information(this, QObject::tr("Statistiques par Adresse"), result);
}

void MainWindow::afficherStatistiquesGraphiqueBarres() {
    QMap<QString, int> statistiques = clientManager.obtenirStatistiquesParAdresse();

    if (statistiques.isEmpty()) {
        QMessageBox::information(this, QObject::tr("Statistiques"), QObject::tr("Aucune donnée à afficher."));
        return;
    }

    QBarSet *set = new QBarSet("Nombre de clients");

    for (const auto& value : statistiques) {
        *set << value;
    }

    QBarSeries *series = new QBarSeries();
    series->append(set);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Statistiques des Clients par Adresse");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(statistiques.keys());
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Nombre de Clients");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMinimumSize(800, 600);
    chartView->setMaximumSize(1200, 800);

    QFont font = chart->titleFont();
    font.setPointSize(16);
    chart->setTitleFont(font);

    for (QAbstractAxis *axis : chart->axes()) {
        axis->setTitleFont(font);
        axis->setLabelsFont(font);
    }

    QGraphicsScene *scene = new QGraphicsScene(this);
    scene->addWidget(chartView);
    ui->stat_view->setScene(scene);
    ui->stat_view->setRenderHint(QPainter::Antialiasing);
}

//m4
void MainWindow::on_ok_feedback_clicked() {
    bool ok;
    int id_feedback = ui->id_feedback->text().toInt(&ok);

    if (!ok || id_feedback <= 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez entrer un ID valide.");
        return;
    }

    Client client = clientManager.rechercherClientById(id_feedback);
    if (client.getIdClient() == 0) {
        QMessageBox::warning(this, "Erreur", "ID du client n'existe pas.");
        return;
    }
    else {
        // Afficher un message indiquant que le client existe
        QMessageBox::information(this, "Client Trouvé",
                                 QString("Le client %1 %2 a été trouvé.")
                                     .arg(client.getNom())
                                     .arg(client.getPrenom()));

        // Afficher le nom et le prénom
        ui->nom->setText(client.getNom());
        ui->prenom->setText(client.getPrenom());
    }
}





void MainWindow::on_submit_feedback_clicked() {
    // Récupérer l'ID du feedback
    int id_feedback = ui->id_feedback->text().toInt(); // Assurez-vous que c'est l'ID du feedback

    // Vérifiez que l'ID de feedback est valide
    if (id_feedback <= 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez entrer un ID de feedback valide.");
        return;
    }

    // Déterminer la note sélectionnée à partir des boutons radio
    int rating = 0;
    if (ui->r1->isChecked()) rating = 1;
    else if (ui->r2->isChecked()) rating = 2;
    else if (ui->r3->isChecked()) rating = 3;

    // Obtenir le commentaire de la zone de texte
    QString commentaire = ui->commentaire->toPlainText();

    // Vérifiez que tous les champs sont remplis
    if (rating <= 0 || commentaire.trimmed().isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez remplir tous les champs.");
        return;
    }

    // Vérifier si un feedback existe déjà avec cet ID
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT COUNT(*) FROM feedback WHERE id_feedback = :id_feedback");
    checkQuery.bindValue(":id_feedback", id_feedback);

    if (!checkQuery.exec() || !checkQuery.next()) {
        QMessageBox::critical(this, "Erreur", "Erreur lors de la vérification du feedback: " + checkQuery.lastError().text());
        return;
    }

    // Si le feedback existe, mettre à jour, sinon insérer un nouveau feedback
    if (checkQuery.value(0).toInt() > 0) {
        // Mettre à jour le feedback existant
        QSqlQuery updateQuery;
        updateQuery.prepare("UPDATE feedback SET rating = :rating, commentaire = :commentaire WHERE id_feedback = :id_feedback");
        updateQuery.bindValue(":rating", rating);
        updateQuery.bindValue(":commentaire", commentaire);
        updateQuery.bindValue(":id_feedback", id_feedback);

        if (updateQuery.exec()) {
            QMessageBox::information(this, "Succès", "Feedback mis à jour avec succès.");
        } else {
            QMessageBox::critical(this, "Erreur", "Échec de la mise à jour du feedback: " + updateQuery.lastError().text());
        }
    } else {
        // Insérer un nouveau feedback
        QSqlQuery insertQuery;
        insertQuery.prepare("INSERT INTO feedback (id_feedback, rating, commentaire) VALUES (:id_feedback, :rating, :commentaire)");
        insertQuery.bindValue(":id_feedback", id_feedback);
        insertQuery.bindValue(":rating", rating);
        insertQuery.bindValue(":commentaire", commentaire);

        if (insertQuery.exec()) {
            QMessageBox::information(this, "Succès", "Feedback ajouté avec succès.");
        } else {
            QMessageBox::critical(this, "Erreur", "Échec de l'ajout du feedback: " + insertQuery.lastError().text());
        }
    }

    // Optionnel : Réinitialiser les champs après l'ajout ou la mise à jour
    ui->id_feedback->clear();
    ui->commentaire->clear();
    ui->r1->setChecked(false);
    ui->r2->setChecked(false);
    ui->r3->setChecked(false);
}
void MainWindow::loadExistingFeedback(int id_feedback) {
    QSqlQuery query;
    query.prepare("SELECT rating, commentaire FROM feedback WHERE id_feedback = :id_feedback");
    query.bindValue(":id_feedback", id_feedback);

    if (query.exec() && query.next()) {
        int rating = query.value(0).toInt();
        QString commentaire = query.value(1).toString();

        // Afficher le commentaire actuel dans l'interface utilisateur
        ui->commentaire->setPlainText(commentaire);

        // Définir le bouton radio correspondant à la note
        if (rating == 1) {
            ui->r1->setChecked(true);
        } else if (rating == 2) {
            ui->r2->setChecked(true);
        } else if (rating == 3) {
            ui->r3->setChecked(true);
        }
    } else {
        QMessageBox::warning(this, "Erreur", "Aucun feedback trouvé pour cet ID de feedback.");
    }
}
//m5////////////////////////////////////////////////
