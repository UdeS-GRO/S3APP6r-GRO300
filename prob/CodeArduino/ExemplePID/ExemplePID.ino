/* Code d'exemple pour controle PID sur Arduino
 * Auteurs: Jean-Samuel Lauzon, 
 *          Jonathan Vincent
 *          
 * date:    8 avril 2018
*/

#include "SimpleTimer.h" // librairie de chronometres
#include "ArduinoJson.h" // librairie de syntaxe JSON
#include <Servo.h> //librairie Servo
#include <SPI.h> // librairie Communication SPI
#include "IRremote_GRO.h" // librairie Capteur InfraRouge
#include "IRremoteInt_GRO.h" // librairie Capteur InfraRouge
#include "LCD_LK162.h"  // afficheur LCD 16x2
#include "Fonctions_GRO.h" // Librairie de fonction GRO


/* Constantes */
#define BAUD            115200 // Frequence de transmission serielle
#define UPDATE_PERIODE_GEN  200  // Periode (ms) d'envoie d'etat general
#define CMD_MIN         0.0 // Commande min (pour PID).
#define CMD_MAX         1.0 // Commande max (pour PID).
#define IR_RECV_PIN 26      // pin où relier le capteur IR pour fin de test


/* Variables globales */
double ENC_TO_VEL = 1000.0/(UPDATE_PERIODE_GEN)/15000;   // Facteur de conversion encodeur -> deg/s.
double CMD_TO_PWM = 200;                              // Facteur de conversion commande -> PWM.
Servo monServoA, monServoB;  // pour tester le contrôle de 2 servos testés sur D8-D9

IRrecv irrecv(IR_RECV_PIN); // recepteur IR
decode_results irResultats; // resultats IR
LCD_LK162 lcd;      // LCD
Fonctions_GRO fonction; // fonctions supplémentaires

// IMU 
float heading;
float Axyz[3];
float Gxyz[3];
float Mxyz[3];
float temperature;

// Encodeurs
signed long encoder1count = 0;
signed long encoder2count = 0;

// Capteurs de courant
float shuntvoltage = 0;
float busvoltage = 0;
float current_mA = 0;
float loadvoltage = 0;

// Electroaimant
int pinElectroaimant = 32;  // selection Grove possible ... D2, D8, D32

// Lectures Analogique
int potvex = 0;

// Servo
int servo_MoteurA_pin = 8; // selection Grove possible D2-D3, D8-D9, D32-D33
int servo_MoteurB_pin = 9; // selection Grove possible D2-D3, D8-D9, D32-D33

// Autres variables globales
float   cur_vel_;           // variable d'état, vitesse du moteur.
float   des_vel_ = 0.0;     // la consigne en vitesse.
float   cur_cmd_;           // la commande en cours.
bool    should_send_;       // Indique qu'on est prêt à transmettre l'état.
SimpleTimer timer;          // objet pour chronometre
String commande;            // string 
bool state_transmit_;                 

// PID
float K_P = 0.0; // Facteur P du PID
float K_I = 0.0; // Facteur I du PID.
float K_D = 0.0; // Facteur D du PID.

float errorI = 0;
float last_error = 0;

// Message objects
StaticJsonBuffer<500> jsonBuffer;
JsonVariant pi_msg = jsonBuffer.createObject();
JsonObject& state_msg = jsonBuffer.createObject();


/* Fonctions et rappels(callbacks) generaux */


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
    Serial.println("parseObject() failed");
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
  /* Fonction du controlleur de vitesse */
  
  /* TODO: Code de contrôle du PID */

  /* La sortie du PID doit être inscrite dans la variable "cur_cmd_".
     Cette variable est ensuite filtrée pour éviter des sauts de commandes lors
     de changements de signes.
     En effet, il faut envoyer au moins une commande nulle (0,0) aux moteurs
     avant de changer de direction.
     C'est le rôle du code qui suit dans la fonction, et vous pouvez le laisser
     intact.
  */
  
  // Pour s'assurer de passer par zero si changement de signe
  if (sgn(cur_cmd_buff)+sgn(cur_cmd_)==0){
     fonction.ControleMoteurs(0,0);
  }

  // Si reinitialisation des moteurs
  if (des_vel_ == 0){
    fonction.ControleMoteurs(0,0);
    errorI = 0;
    last_error = 0;
  }
  else{
    // Sinon envoie de la commande au moteur
    fonction.ControleMoteurs(int(cur_cmd_buff),0);
  }
  // Update de l'etat de la comande
  state_msg["cmd"] = int(cur_cmd_buff);
}

void updateState(){

fonction.lireAccel_Data(Axyz);
    fonction.lireGyro_Data(Gxyz);
    fonction.lireMag_Data(Mxyz);
    fonction.lireCap(&heading);
    fonction.lireTemperature(&temperature);

    // Lecture des pulses d'encodeurs
    encoder1count = fonction.lireEncodeur(1); 
    encoder2count = fonction.lireEncodeur(2);
    cur_vel_ = ENC_TO_VEL*encoder1count;

    fonction.clearEncodeurCount();

    // lectures du capteur de courant
    fonction.lireTensionShunt(&shuntvoltage);
    fonction.lireTensionBus(&busvoltage);
    fonction.lireCourant_mA(&current_mA);
    fonction.lireTensionCharge(&loadvoltage);

    // Lecture Potentiomètre VEX sur A5
    potvex = analogRead(A5);

    state_msg["accel_x"] = Axyz[0];
    state_msg["accel_y"] = Axyz[1];
    state_msg["accel_z"] = Axyz[2];

    state_msg["gyro_x"] = Gxyz[0];
    state_msg["gyro_y"] = Gxyz[1];
    state_msg["gyro_z"] = Gxyz[2];

    state_msg["mag_x"] = Mxyz[0];
    state_msg["mag_y"] = Mxyz[1];
    state_msg["mag_z"] = Mxyz[2];

    state_msg["heading"] = heading;
    state_msg["temperature"] = temperature;

    state_msg["enc_1"] = encoder1count;
    state_msg["enc_2"] = encoder2count;
    state_msg["shunt_voltage"] = shuntvoltage;
    state_msg["bus_voltage"] = busvoltage;
    state_msg["current"] = current_mA;
    state_msg["load_voltage"] = loadvoltage;
    
    state_msg["pot_vex"] = potvex;
    state_msg["des_vel"] = des_vel_;
    state_msg["cur_vel"] = cur_vel_;
    send_state_data();
    
    should_send_ = false;
}

/* Fonction d'initialisation et boucle principale*/
void setup(){
  Serial.begin(BAUD); // Initialisation de la communication serielle 
  while (!Serial) continue;

  fonction.init();
  monServoA.attach(servo_MoteurA_pin);
  monServoB.attach(servo_MoteurB_pin);
  irrecv.enableIRIn(); // Start the receiver

  timer.setInterval(UPDATE_PERIODE_GEN, timerCallback);// Initialisation du timer
  state_transmit_ = false;
  should_send_ == false;
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

