String text;

void setup() {
  // initialition de la communication 9600 bits par seconde
  Serial.begin(9600);
}


void loop() {
  // lecture la pin analogique ou est relié le module comparateur
  int value = analogRead(A5);
  text = String("");

  if (value < 950 && value > 600) {
    text = String("Commence a etre humide ");
    text = text + value;
    Serial.println(text);
  } else if (value < 600) {
    text = String("humide ");
    text = text + value;
    Serial.println(text);
  } else if (value >= 800) {
    text = String("Sec ");
    text = text + value;
    Serial.println(text);
  }
  delay(1000);
}
