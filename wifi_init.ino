#include <SoftwareSerial.h>
SoftwareSerial ESP8266(10, 11);

String ssid = "!!"; // Garder les guillements
String password = "!!"; // Garder les guillements
bool espInit = false;

void setup() {
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
  while (ESP8266.available())   {
    Serial.println(ESP8266.readString());
  }
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
  ESP8266.println(commande);
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
