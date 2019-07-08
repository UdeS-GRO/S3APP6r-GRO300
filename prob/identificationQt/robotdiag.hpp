#ifndef ROBOTDIAG_HPP
#define ROBOTDIAG_HPP

#include <queue>
#include <vector>
#include <string>

namespace s3gro
{
    struct RobotState
    {
        int   id;       // Identifiant du moteur (0: réel)
        float t;        // Temps (en s) 
        float cur_cmd;  // Commande en cours
        float cur_pos;  // Position en cours
        float cur_vel;  // Vitesse en cours
    };

    class RobotDiag
    {
    private:
        bool run_;                      // Indique si le fil d'exportation
                                        // fonctionne.

        std::queue<RobotState>  queue_; // File d'attente à l'exportation.
        std::vector<RobotState> data_;  // Toutes les données reçues.

        std::string csv_filename_;

    public:
        RobotDiag();
        ~RobotDiag();
        
        // Fonction à appeler lors de la réception d'un nouveau message.
        void push_event(RobotState new_state);

        // Fonction pour changer le nom de fichier de sauvegarde CSV.
        void set_csv_filename(const std::string& fname);

        // Fonction qui démarre le fil d'enregistrement
        void start_recording();

        // Fonction qui arrête le fil d'enregistrement
        void stop_recording();
    private:
        // Fonction à isoler dans un fil.
        void export_loop();
    };

}

#endif

