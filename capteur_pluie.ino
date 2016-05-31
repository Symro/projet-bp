// Importation de la Time library
#include <elapsedMillis.h>

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
unsigned int interval = 500;
//
bool elapsedTimeAlert = false;

void setup() {
  // initialition de la communication 9600 bits par seconde
  Serial.begin(9600);
}

void loop() {
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

// Vérification de l'état si il a changé ou non au bout d'un temps défini afin de prévenir le personnel
void checkStateAfterLimit() {
  if (state == lastState && state == 0 && !elapsedTimeAlert) {
    text = String("Letat est inchange depuis 5heures pour cette personne: ");
    text = text + state;
    // sendAlert(text, state);
    elapsedTimeAlert = true;
  }
}

// Affichage sur terminal
void sendAlert(String text, int state) {
  text = text + state;
  Serial.println(text);
}
