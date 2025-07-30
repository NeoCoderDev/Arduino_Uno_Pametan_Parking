void setup() {
  Serial.begin(9600);    // Serijska komunikacija sa računarom
  Serial2.begin(115200); // Serijska komunikacija sa ESP8266 (RX2/TX2)
  Serial.println("Arduino Mega ready");
}

void loop() {
  if (Serial2.available()) {
    String command = Serial2.readStringUntil('\n');
    Serial.print("Received command: ");
    Serial.println(command); // Ispisuje primljenu komandu za debagovanje
    if (command == "RESERVE") {
      reserveParking();  // Poziva funkciju za rezervaciju parking mesta
    }
  }
}

void reserveParking() {
  // Logika za rezervaciju parking mesta
  Serial.println("Parking reserved!");
  // Dodajte kod za rezervaciju mesta, uključivanje LED, itd.
}
