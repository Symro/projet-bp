// Importation de la Time library
#include <SoftwareSerial.h>
#include <elapsedMillis.h>

SoftwareSerial ESP8266(10, 11);

// Déclare global variables
String text;
int currentSensorValue5, currentSensorValue4, currentSensorValue3;
int captorValues[3];
// Etat du change 0:sec -- > 3:saturation
int state = 0, lastState = 0;
// variable du temps passé en ms
elapsedMillis elapsedTime;
// Délai entre les vérifications en ms
// 1h 3600000
// 5h 18000000
unsigned int interval = 3600000;
//
bool elapsedTimeAlert = false;

// variables pour le wifi
String ssid = "Bbox-B4328E"; // Garder les guillements
String password = "4D8A272250"; // Garder les guillements
bool espInit = false;

//variables pour la communication avec le terminal
String chaineReception="";
char caractereRecu = 0;
int byteRecu;

void setup() {
  // initialition de la communication 9600 bits par seconde
  Serial.begin(9600);

  ESP8266.begin(9600);
  envoiEsp("AT+CIOBAUD=9600");
  transmissionEsp(4000);

  ESP8266.begin(9600);
  while (!espInit) {
    initESP8266();
  }
}

void loop() {
  while(espInit){
    // lecture la pin analogique ou est relié le module comparateur
    currentSensorValue5 = analogRead(A5);
    currentSensorValue4 = analogRead(A4);
    currentSensorValue3 = analogRead(A3);
    checkSensorValue();

    // Vérification si il y a eu changement du state après 5h
    if (elapsedTime > interval) {
      checkStateAfterLimit();
    }

    // On écrase lastState avant de réexecuter la loop
    lastState = state;

    // Délai d'attente avant le prohain check en ms
    delay(2000);
  }

  // Lorsque l'on reçoit un message via la console on va traiter chaque byte pour reformer la commande envoyée
  while (Serial.available() > 0) {
    byteRecu = Serial.read();   
    // conversion de l'octet en caractère
    caractereRecu = char(byteRecu);
    // Ajout du caractère au string
    chaineReception = chaineReception + caractereRecu;
    // Si on reçoi le signal de la touche entrée
    if (byteRecu == 10) {
      envoiEsp(chaineReception);
      chaineReception = "";
    }
  }

  // Délai d'attente
  delay(2000);
}

// set State en fonction de la valeur du capteur
void checkSensorValue() {
  // On check nos entrées avec une marge d'erreur au cas ou et on evite d'écraser la détéction d'une fuite une fois que le courant passe plus
  // puis on les mets dan un tableau
  if (currentSensorValue5 > 10) {
    captorValues[0] = currentSensorValue5;
  }
  if (currentSensorValue4 > 10) {
    captorValues[1] = currentSensorValue4;
  }
  if (currentSensorValue3 > 10) {
    captorValues[2] = currentSensorValue3;
  }

  // On va checker chaque entrée du tableau pour donner une valeur à l'état de la couche
  state = 0;
  int i;
  for (i = 0; i < 3; i = i + 1) {
    if (captorValues[i] >= 10) {
      state++;
    }
  }
  //Serial.println(state);
  
  text = String("Etat:");
  sendAlert(text, state);
}

// initialisation de l'esp
void initESP8266() {
  Serial.println("// DEBUT DE L'INITIALISATION");
  envoiEsp("AT");
  transmissionEsp(4000);
  Serial.println("// wifi mode");
  envoiEsp("AT+CWMODE=3");
  transmissionEsp(5000);
  Serial.println("// set paramaters");
  envoiEsp("AT+CWJAP=\"" + ssid + "\",\"" + password + "\"");
  transmissionEsp(4000);
  Serial.println("// recup de lip");
  envoiEsp("AT+CIFSR");
  transmissionEsp(4000);
  Serial.println("// connexion multiple");
  envoiEsp("AT+CIPMUX=1");
  transmissionEsp(4000);
  //Serial.println("// set en tant que serveur");
  //envoiEsp("AT+CIPSERVER=1,80");
  //transmissionEsp(4000);
  Serial.println("// INITIALISATION TERMINEE");
  espInit = true;
}

// Envoi de commande à l'esp
void envoiEsp(String commande) {
  ESP8266.println("commande: "+ commande);
}

// Affichage des transmissions de l'esp
void transmissionEsp(const int timeout) {
  String reponse = "";
  long int time = millis();
  while ((time + timeout) > millis())   {
    while (ESP8266.available())     {
      char c = ESP8266.read();
      reponse += c;
    }
  }
  Serial.print(reponse);
}

// Vérification de l'état si il a changé ou non au bout d'un temps défini afin de prévenir le personnel
void checkStateAfterLimit() {
  if (state == lastState && state == 0 && !elapsedTimeAlert) {
    text = String("Letat est inchange depuis 5heures pour cette personne: ");
    text = text + state;
    sendAlert(text, state);
    elapsedTimeAlert = true;
  }
}

// Affichage sur terminal
void sendAlert(String text, int state) {
  text = text + state;
  Serial.println(text);
}
