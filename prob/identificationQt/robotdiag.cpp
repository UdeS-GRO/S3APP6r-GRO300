#include "robotdiag.hpp"
#include "robotsim.hpp"
#include <vector>
#include <queue>
#include <cstdio>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace s3gro;


RobotDiag::RobotDiag()
{
    // Démarre le simulateur:
    robotsim::init(this, 8, 10, 3);   // Spécifie le nombre de moteurs à 
                                      // simuler (8) et le délai moyen entre
                                      // les événements (10 ms) plus ou moins
                                      // un nombre aléatoire (3 ms).

    // Indique que le système de diagnostic fonctionne (à mettre à 'false' lors
    // de la fermeture pour interrompre le fil d'exportation).
    run_ = true;

}

// Le destructeur sera normalement appellé à la fermeture de l'application.
// Écrit des statistiques à l'écran.
RobotDiag::~RobotDiag()
{
    run_ = false;

    robotsim::stop_and_join();

    printf("Final vector size: %zu\n", data_.size());
}

void RobotDiag::push_event(RobotState evt)
{
    // Conserve toutes les données
    data_.push_back(evt);
    // Ajoute le dernier événement à la file d'exportation
    queue_.push(evt);

}

// Fonction d'exportation vers CSV.
// Doit être exécutée dans un fil séparé et écrire dans le fichier CSV
// lorsque de nouvelles données sont disponibles dans queue_.
void RobotDiag::export_loop()
{
    FILE* out = fopen("/tmp/robotdiag.csv", "w");

    if (out == NULL) {
        printf("ERROR: Cannot open output file.\n");
        return;
    }

    // En-tête du fichier CSV, respectez le format.
    fprintf(out, "motor_id;t;pos;vel;cmd\n");

    // TODO: Synchronisation et écriture ici !
    
    fclose(out);
}

