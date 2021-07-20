#include "robotdiag.hpp"
#include "robotsim.hpp"

#include <vector>
#include <queue>
#include <cstdio>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace s3gro;

RobotDiag::RobotDiag(){}

// Le destructeur sera normalement appellé à la fermeture de l'application.
// Écrit des statistiques à l'écran.
RobotDiag::~RobotDiag() {
    stop_recording();
}

void RobotDiag::push_event(RobotState new_robot_state) {
    // Conserve toutes les données
    data_.push_back(new_robot_state);

    // Ajoute le dernier événement à la file d'exportation
    queue_.push(new_robot_state);
}

void RobotDiag::set_csv_filename(const std::string& file_name) {
    csv_filename_ = file_name;
}

void RobotDiag::start_recording() {
    // Indique que le système de diagnostic fonctionne (à mettre à 'false' lors
    // de la fermeture pour interrompre le fil d'exportation).
    run_ = true;

    // Démarre le simulateur:
    // TODO: Supprimer cette ligne si vous testez avec un seul moteur
    robotsim::init(this, 8, 10, 3);   // Spécifie le nombre de moteurs à 
                                      // simuler (8) et le délai moyen entre
                                      // les événements (10 ms) plus ou moins
                                      // un nombre aléatoire (3 ms).

    // TODO : Lancement du fil.
}

void RobotDiag::stop_recording() {
    // Indique que le système de diagnostic doit être arrêté.
    run_ = false;

    // TODO : Fermeture du fil.

    robotsim::stop_and_join();

    printf("Final vector size: %zu\n", data_.size());
}

// Fonction d'exportation vers CSV.
// Doit être exécutée dans un fil séparé et écrire dans le fichier CSV
// lorsque de nouvelles données sont disponibles dans queue_.
void RobotDiag::export_loop() {
    if (csv_filename_.empty()) {
        csv_filename_ = "/tmp/robotdiag.csv";
    }

    FILE* out = fopen(csv_filename_.c_str(), "w");

    if (out == NULL) {
        printf("ERROR: Cannot open output file.\n");
        return;
    }

    // En-tête du fichier CSV, respectez le format.
    fprintf(out, "motor_id;t;pos;vel;cmd\n");

    // TODO: Synchronisation et écriture.
    
    fclose(out);
}

