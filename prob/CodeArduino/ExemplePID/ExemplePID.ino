/* Code d'exemple pour controle PID sur Arduino
 * Auteurs: Jean-Samuel Lauzon, 
 *          Jonathan Vincent
 *          
 * date:    8 avril 2018
*/

/* Fonctions de départ pour le shield Arduino_X */ 


#include "SimpleTimer.h" // librairie de chronometres
#include "ArduinoJson.h" // librairie de syntaxe JSON
#include "Fonctions_GRO.h"
#include <SPI.h> // librairie Communication SPI


/* Constantes */
#define BAUD            115200 // Frequence de transmission serielle
#define UPDATE_PERIODE_GEN  100  // Periode (ms) d'envoie d'etat general


Fonctions_GRO fonction; // fonctions supplémentaires

// Moteurs
int pwm1 = 0;
int pwm2 = 0;

// Encodeurs
signed long encoder1count   = 0;
signed long encoder2count   = 0;  
signed long last_enc1_count = 0; // Lectures du cycle précédent
signed long last_enc2_count = 0;

// Electroaimant
int pinElectroaimant = 32;  // selection Grove possible ... D2, D8, D32

// Lectures Analogique
int potVex = 0;

// Lecture encodeur Vex
signed long encodervex = 0;  // selection Grove possible ... D2-D3 interrupt

// Autres variables globales
float   cur_vel_;           // variable d'état, vitesse du moteur.
float   des_vel_ = 0.0;     // la consigne en vitesse.
float   cur_cmd_;           // la commande en cours.

double ENC_TO_VEL = 1000.0/(UPDATE_PERIODE_GEN)/15000;   // Facteur de conversion encodeur -> deg/s.

// Configuration du PID
float K_P = 0.0; // Facteur P du PID
float K_I = 0.0; // Facteur I du PID.
float K_D = 0.0; // Facteur D du PID.

// État du PID (inutilisé pour l'instant)
float errorI = 0;
float last_error = 0;

bool    should_send_;       // Indique qu'on est prêt à transmettre l'état.
SimpleTimer timer;          // objet pour chronometre
bool state_transmit_; 
bool state_electroaimant_;   

// Message objects
StaticJsonBuffer<500> jsonBuffer;
JsonVariant pi_msg = jsonBuffer.createObject();
JsonObject& state_msg = jsonBuffer.createObject();


/* Fonctions et rappels(callbacks) generaux */

// Retourne le signe d'un entier (-1 négatif, +1 positif)
static inline int8_t sgn(int val) {
  if (val < 0) return -1;
  if (val==0) return 0;
  return 1;
}

void timerCallback(){
  /* Rappel du chronometre */
  should_send_ = true;
}

void analyseMessage(JsonVariant msg){
  /* Fonction d'analyse du message recu */
  JsonVariant parse_msg;

  // Determiner si le message est un succes
  if (!msg.success()) {
//    Serial.println("parseObject() failed");
    return;
  }
  
  /*Si il y a commande de lecture
  en continue*/
  parse_msg =  msg["read"];
  String buff_str = msg["read"];
  if(parse_msg.success()){
    if (buff_str.equals("true")){
      state_transmit_ = true;
    }
    if (buff_str.equals("false")){
      state_transmit_ = false;
    }
  }

  /*Si il y a commande d'Action sur l'aimant */
  parse_msg =  msg["aimant"];
  String buff_str1 = msg["aimant"];
  if(parse_msg.success()){
    if (buff_str1.equals("true")){
      state_electroaimant_ = true;
    }
    if (buff_str1.equals("false")){
      state_electroaimant_ = false;
    }
  }
  
  /*Si il y a commande de changement
  de vitesse desiree*/
  parse_msg =  msg["set_des_vel"];
  if (parse_msg.success()) {
    des_vel_ = msg["set_des_vel"];
  }

    /*Si il y a commande de changement
  de de parametres PID*/
  parse_msg = msg["set_PID"];
  if (parse_msg.success()) {
      K_P = msg["set_PID"][0];
      K_I = msg["set_PID"][1];
      K_D = msg["set_PID"][2];
  }
  
  
}

void serialEvent(){
  /* Rappel lors de reception sur port serie */
  StaticJsonBuffer<500> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(Serial);
  pi_msg = root;
}


void send_state_data(){
  /* Fonction pour envoie de l'etat */
  state_msg.printTo(Serial); // Met le message en attente dans le port serie
  Serial.println();// Envoie le message
}

void run_PID(){
  float new_cmd = 0.0;
  /* Fonction du controlleur de vitesse */
  
  /* TODO: Code de contrôle du PID */

  /* La sortie du PID doit être inscrite dans la variable "new_cmd".
     Cette variable est ensuite filtrée pour éviter des sauts de commandes lors
     de changements de signes.
     En effet, il faut envoyer au moins une commande nulle (0,0) aux moteurs
     avant de changer de direction.
     C'est le rôle du code qui suit dans la fonction, et vous pouvez le laisser
     intact.
     Le résultat est stocké dans la variable globale "cur_cmd_".
     Cette variable contient donc la dernière commande envoyée aux moteurs.
  */

  float err = des_vel_ - cur_vel_;
  new_cmd = cur_cmd_ + K_P * err;
  
  // Pour s'assurer de passer par zero si changement de signe
  if (sgn(new_cmd)+sgn(cur_cmd_)==0){
     fonction.ControleMoteurs(0,0);
  }

  cur_cmd_ = new_cmd;

  // Si reinitialisation des moteurs
  if (des_vel_ == 0){
    fonction.ControleMoteurs(0,0);
    errorI = 0;
    last_error = 0;
  }
  else{
    // Sinon envoie de la commande au moteur
    fonction.ControleMoteurs(int(cur_cmd_),0);
  }

}

void updateState(){

    // Lecture Potentiomètre VEX sur A5
   potVex = analogRead(A5);

    // Lecture des pulses d'encodeurs
    encoder1count = fonction.lireEncodeur(1); 
    encoder2count = fonction.lireEncodeur(2);
    cur_vel_ = ENC_TO_VEL*(encoder1count);

    fonction.clearEncodeurCount();
  
    // Lecture encodeur Vex (D2-D3)
    encodervex = fonction.lectureEncodeurVex();
    if(encodervex > 600) fonction.razEncodeurVex(); // exemple de remise à 0
   // Serial.print(" Encodeur Vex =  "); Serial.println(encodervex); 
 
    state_msg["enc_1"] = encoder1count;
    state_msg["enc_2"] = encoder2count;
    state_msg["pot_vex"] = potVex;
    state_msg["encodeur_vex"] = encodervex;
    state_msg["des_vel"] = des_vel_;
    state_msg["etat_electroaimant"] = state_electroaimant_;

    // TODO: Variables d'états manquantes: vitesse, commande.
    send_state_data();
    
    should_send_ = false;
}


void setup()
{
    Serial.begin(BAUD); // Initialisation de la communication serielle 
  while (!Serial) continue;

   fonction.init();

    timer.setInterval(UPDATE_PERIODE_GEN, timerCallback);// Initialisation du timer
    state_transmit_ = false; // met à true pour pas attendre la commande "READ en continue" du PI si non connecté// false;
    should_send_ == false;

    delay(1000);
    
}

void loop() {

/* Fonction de la boucle principale*/
  if(should_send_ == true && state_transmit_ == true){
    updateState();
    run_PID();
  }

  timer.run();
  analyseMessage(pi_msg);

}

