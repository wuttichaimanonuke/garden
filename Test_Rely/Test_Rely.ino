void setup() {
  Serial.begin(9600);
  
  pinMode(13, OUTPUT);      //LED
  digitalWrite(13, HIGH);
  pinMode(11, OUTPUT);      //Rely Power
  digitalWrite(11, HIGH);
  pinMode(6, OUTPUT);       //Rely Off.
  digitalWrite(11, LOW);

}

void loop() {
  Serial.println("LOW");
  digitalWrite(6,LOW); // This will Turn ON the relay 1
  delay(3000);
  Serial.println("HIGH");
  digitalWrite(6,HIGH); // This will Turn the Relay Off
  delay(3000);
}
