char serialbuffer[1000];//serial buffer for request url

//globales values
int currentSensorValue5, currentSensorValue4, currentSensorValue3;
int captorValues[3];
// Etat du change 0:sec -- > 3:saturation
int state = 0, lastState = 0;
//globales serveur
//String ip = "192.168.1.54";
String ip = "178.62.76.82";

void setup() {
  //connection à l'ESP8266
  Serial1.begin(115200);
  //debug avec la console
  Serial.begin(115200);

  // initialize les pin en tant que sortie.
  pinMode(2, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(8, INPUT);

  //set mode needed for new boards
  Serial1.println("AT+RST");
  //delay(1000);
  //Serial1.println("AT+CWMODE=1");
  //delay(1000);//delay after mode change
  //Serial1.println("AT+RST");
  
  //connect to wifi network
  Serial1.println("AT+CWJAP=\"Sylvain\",\"paproto93\"");
  delay(1000);
}

void loop() {
  Serial.println(state);
  //output everything from ESP8266 to the Arduino Micro Serial output
  while (Serial1.available() > 0) {
    Serial.write(Serial1.read());
  }
  
  if (Serial.available() > 0) {
    //read from serial until terminating character
    int len = Serial.readBytesUntil('\n', serialbuffer, sizeof(serialbuffer));
  
    //trim buffer to length of the actual message
    String message = String(serialbuffer).substring(0,len-1);
    Serial.println("message: " + message);
 
    //Verification si une commande est envoyé ou une url dans la console
    if(message.substring(0,2)=="AT"){
      //make command request
      Serial.println("COMMAND REQUEST");
      Serial1.println(message); 
    }else{
      
     // On débute la liaison
     String startcommand = "AT+CIPSTART=\"TCP\",\"" + ip + "\",6060";
     Serial.println(startcommand);
     Serial1.println(startcommand);
     delay(3000);  
     // requete web
      Serial.println("WEB REQUEST");
      WebRequest(message);
    }
  }

  // lecture des connexions au capteur
  currentSensorValue5 = analogRead(A5);
  currentSensorValue4 = analogRead(A4);
  currentSensorValue3 = analogRead(A3);
  checkSensorValue();

  // Si l'état n'est plus le même que le précédent on envoie une notification
  if(state != lastState && state > 0){
    sendAlert();
  }
  
  // On écrase lastState avant de réexecuter la loop
  lastState = state;

  delay(1000);
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
  if (state == 1){
    digitalWrite(2, HIGH);
  }
  if (state == 2){
    digitalWrite(4, HIGH);
  }
  if (state == 3){
    digitalWrite(6, HIGH);
  }
}

// Préparation de l'envoi des notifications0
void sendAlert() {
  // On débute la liaison
  String startcommand = "AT+CIPSTART=\"TCP\",\"" + ip + "\",6060";
  Serial.println(startcommand);
  Serial1.println(startcommand);
  delay(3000);  
  // requete web
  Serial.println("WEB REQUEST");
  WebRequest("178.62.76.82:6060/alert?resident=duhamel&type=1&zone=1");
}

//web request needs to be sent without the http for now, https still needs some working
void WebRequest(String request){
  delay(1000);
  //find the dividing marker between domain and path
  int slash = request.indexOf('/');
     
  //grab the domain
  String domain;
  if(slash>0){  
    domain = request.substring(0,slash);
  }else{
    domain = request;
  }

  //get the path
  String path;
  if(slash>0){  
    path = request.substring(slash);   
  }else{
    path = "/";          
  }
     
  //output domain and path to verify
  Serial.println("domain: |" + domain + "|");
  Serial.println("path: |" + path + "|");        
     
  //test for a start error
  if(Serial1.find("Error")){
    Serial.println("error on start");
    return;
  }
     
  //create the request command
  String sendcommand = "POST http://"+ domain + path + " HTTP/1.0\r\n\r\n\r\n";//works for most cases
     
  Serial.print(sendcommand);
     
  //send 
  Serial1.print("AT+CIPSEND=");
  Serial1.println(sendcommand.length());
     
  //debug the command
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
