#include <cstdio>

enum 
{
    PHASE_UP,
    PHASE_STAY_FULL,
    PHASE_DN,
    PHASE_STAY_HALF,
    PHASE_DONE
};

int main(int argc, char** argv)
{
    // Modèle et situation initiale:
    double t    = 0.00; // Temps, s
    double dt   = 0.01; // Incrément de temps, s 
    double qd   = 0.00; // Vitesse, m/s
    double qdd  = 0.00; // Accélération, m/s^2
    double cmd  = 0.00; // Commande à appliquer, m/s^2

    int phase = PHASE_UP;   // Phase en cours

    // Boucle jusqu'à ce qu'à la dernière phase, mais peut être interrompue par
    // Ctrl-C. 
    while (phase != PHASE_DONE) {
        switch (phase) {
            case PHASE_UP:
                // Phase 1 : Appliquer 0.2 m/s jusqu'à ce que la vitesse de 1
                // m/s soit atteinte (+/- 0.001 m/s).
                cmd = 0.2;

                break;

            case PHASE_STAY_FULL:
                // On maintient la commande de 0.2 m/s pendant au moins une
                // seconde (+/- 0.01 s)
                cmd = 0.2;
                break;

            case PHASE_DN:
                // On réduit
                break;

            case PHASE_STAY_HALF:
                break;

            case PHASE_DONE:
                break;

        };

        // Applique la vitesse choisie et met à jour le modèle :
        qd  += qdd * dt;
        qdd  = cmd;
        t   += dt;
        
        printf("Vitesse en cours: %f\n", qd);

    }

    return 0;
}
