//serial buffer for request url
char serialbuffer[1000];

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
bool elapsedTimeAlert = false;
//WiFi Setup
//String ssid = "Bbox-B4328E";
//String ssid = "Sylvain";
String ssid = "GateHetic2-RDC";
String password = "!!hetic2016!!";
//String password = "papro  to93";
String ip = "178.62.76.82";
//String ip = "192.168.43.232";
//user infos
String username = "durand";
int type = 1;

void setup() {
  //connection à ESP8266
  Serial1.begin(9600);
  // initialition de la communication 9600 bits par seconde
  Serial.begin(9600);

  // Reset de l'ESP
  Serial1.println("AT+RST");
  //AT+CIOBAUD=9600
  
  //connexion au wifi
  Serial.println("AT+CWJAP=\"" + ssid + "\",\"" + password + "\"");
  Serial1.println("AT+CWJAP=\"" + ssid + "\",\"" + password + "\"");
  //delay(5000); 

  // initialize les pin en tant que sortie.
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
}

void loop() {
  // lecture la pin analogique ou est relié le module comparateur
  currentSensorValue5 = analogRead(A5);
  currentSensorValue4 = analogRead(A4);
  currentSensorValue3 = analogRead(A3);
  checkSensorValue();

  // Vérification si il y a eu changement du state après 5h
  //if (elapsedTime > interval) {
    //checkStateAfterLimit();
  //}
  
  if(state != lastState && state > 0){
    text = String("Etat:");
    sendAlert(text, state);
  }

  // Délai d'attente avant le prohain check en ms
  delay(2000);
  //affiche dans la console tout ce que l'ESP remonte comme info
  while (Serial1.available() > 0) {
    Serial.write(Serial1.read());
  }
  
  if (Serial.available() > 0) {
    //lecture des commandes
    int len = Serial.readBytesUntil('\n', serialbuffer, sizeof(serialbuffer));
    
    //trim buffer to length of the actual message
    String message = String(serialbuffer).substring(0,len-1);
    Serial.println("message: " + message);
    //check si c'est une commande ou une url
    if(message.substring(0,2)=="AT"){
      //execution de la commande
      Serial.println("COMMAND REQUEST");
      Serial1.println(message); 
    }else{
     //execution de la requete web
      Serial.println("WEB REQUEST");
      WebRequest(message);
    }
  }

  // On écrase lastState avant de réexecuter la loop
  lastState = state;
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
    Serial.println(captorValues[i]);
  }

  if (state == 1){
    digitalWrite(2, HIGH);
  }
  if (state == 2){
    digitalWrite(3, HIGH);
  }
  if (state == 3){
    digitalWrite(4, HIGH);
  }
  Serial.println(state);
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
  //Serial.println(text);
  Serial.println(ip + "/alert?resident=" + username + "&type=" + type + "&zone=" + state);
  WebRequest(ip + "/alert?resident=" + username + "&type=" + type + "&zone=" + state);
}

// envoi d'une requette http
void WebRequest(String request){
 //split de l'url
  int slash = request.indexOf('/');
     
  //recup du domaine
  String domain;
  if(slash>0){  
    domain = request.substring(0,slash);
  }else{
    domain = request;
  }

  //récup du path
  String path;
  if(slash>0){  
    path = request.substring(slash);   
  }else{
    path = "/";          
  }
     
  //log du domaine et du path
  Serial.println("domain: |" + domain + "|");
  Serial.println("path: |" + path + "|");     
     
  //Commande start
  //String startcommand = "AT+CIPSTART=\"TCP\",\"" + domain + "\", 80"; 
  //Serial1.println(startcommand);
  //delay(3000);
  //Serial.println(startcommand);
  String startcommand = "AT+CIPSTART=\"TCP\",\"" + ip + "\",\"6060";
  Serial1.println(startcommand);
  delay(300);
     
     
  //test si il y a une erreur au start
  if(Serial1.find("Error")){
    Serial.println("error on start");
    return;
  }
   
  //Requête http
  String sendcommand = "POST http://"+ domain + path + " HTTP/1.0\r\n\r\n\r\n";
   
  Serial.print(sendcommand);
   
  //envoi 
  Serial1.print("AT+CIPSEND=");
  Serial1.println(sendcommand.length());
   
  //debug
  Serial.print("AT+CIPSEND=");
  Serial.println(sendcommand.length());
  
  //delay(5000);
  if(Serial1.find(">")){
    Serial.println(">");
  }else{
    Serial1.println("AT+CIPCLOSE");
    Serial.println("connect timeout");
    delay(1000);
    return;
  }
  delay(1000);
  //Serial.print(getcommand);
  Serial1.print(sendcommand); 
}
