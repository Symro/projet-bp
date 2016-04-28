// Importation de la Time library
#include <elapsedMillis.h>

// Déclare global variables
String text;
int currentSensorValue;
// Etat du change 0:sec, 1:un peu humide, 2:humide, 3:saturation
int state = 0, lastState = 0;

// variable du temps passé en ms
elapsedMillis elapsedTime;
// Délai entre les vérifications en ms
// 1h 3600000
// 5h 18000000
unsigned int interval = 5000;
//
bool elapsedTimeAlert = false;

void setup() {
  // initialition de la communication 9600 bits par seconde
  Serial.begin(9600);
}

void loop() {
  // lecture la pin analogique ou est relié le module comparateur
  currentSensorValue = analogRead(A5);
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

// set State en fonction de la valeur du capteur
void checkSensorValue() {
  if (currentSensorValue < 800 && currentSensorValue > 600) {
    text = String("Chambre:1 Patient:nom Etat:");
    state = 1;
  } else if (currentSensorValue < 600 && currentSensorValue > 500) {
    text = String("Chambre:1 Patient:nom Etat:");
    state = 2;
  } else if (currentSensorValue < 500) {
    text = String("Chambre:1 Patient:nom Etat:");
    state = 3;
  } else if (currentSensorValue >= 800) {
    text = String("Chambre:1 Patient:nom Etat:");
    state = 0;
  }
  sendAlert(text, state);
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

